#include<iostream>
#include<vector>
#include "./include/clustering/fesif/fesif.hpp"
#include "./include/clustering/HGS/HGS.hpp"
#include "./include/clustering/Clarke/clarke.hpp"


#include "./include/routeplan/TSP_OR.hpp"
#include "./include/routeplan/TSP_OR_EDD.hpp"
#include "./include/routeplan/TSP_LK.hpp"
#include "./include/clustering/HGS/HGS.hpp"
#include "./include/routeplan/TSP_CK.hpp"

#include "./include/binpack/EB_AFIT.hpp"
#include "./include/Ensembler.hpp"

#include <grpcpp/grpcpp.h>

#include "main.pb.h"
#include "main.grpc.pb.h"

using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::Status;

using optimizer::Package;
using optimizer::ResponsePackage;
// using optimizer::Bin;
// using optimizer::Dimension;
// using optimizer::Coordinate;
// using optimizer::Position;
// using optimizer::Cluster;
using optimizer::OptimizerRequest;
using optimizer::OptimizerResponse;
// using optimizer::optimizer;

class DataModel{
	public:
	Coordinate warehouse;
	Bin bin;
	vector<item> packages;
	int numRiders;
    DataModel(){
        warehouse.latitude = 0;
        warehouse.longitude = 0;
        packages = vector<item>();
        bin = Bin();
    }
};
DataModel getData(const OptimizerRequest *request){
    DataModel dm;
    dm.numRiders = request->riders();
    dm.warehouse.latitude = request->warehouse().latitude()/1e6;
    dm.warehouse.longitude = request->warehouse().longitude()/1e6;
    for(Package package : request->packages()){
        item i;
        i.id = package.id();
        i.coordinate.latitude = package.coordinates().latitude()/1e6;
        i.coordinate.longitude = package.coordinates().longitude()/1e6;
        i.weight = package.weight();
        i.size.height = package.size().height();
        i.size.width = package.size().width();
        i.size.length = package.size().length();
        i.volume = i.getVolume();
        dm.packages.push_back(i);
    }
    dm.bin.capacity = request->bin().capacity();
    dm.bin.size.height = request->bin().size().height();
    dm.bin.size.width = request->bin().size().width();
    dm.bin.size.length = request->bin().size().length();
    return dm;
}
void setData(Ensembler *optim,OptimizerResponse *reply){
    int numClusters = optim->getNumClusters();
    cout << "-----------------------------Returning Result--------------------"<<endl;
    cout << "numClusters : " << numClusters << endl;
    for(int i = 0;i < numClusters;i++){
        reply->add_clusters();
        vector<item> cluster = optim->GetPackagingForCluster(i);
        cout << "Size of Cluster : " << i << " : " << cluster.size();
        int numPackages = cluster.size();
        for(int j = 0;j < numPackages;j++){
            cout << "\t package " << j << " position : " << cluster[j].position.x << ' ' << cluster[j].position.y << ' ' << cluster[j].position.z << " dimension : " << cluster[j].size.length << ' ' << cluster[j].size.width << ' ' << cluster[j].size.height << endl;
            ResponsePackage pkg;
            pkg.set_id(cluster[j].id);
            optimizer::Position pos;
            pos.set_x(cluster[j].position.x);
            pos.set_y(cluster[j].position.y);
            pos.set_z(cluster[j].position.z);
            pos.set_length(cluster[j].size.length);
            pos.set_breadth(cluster[j].size.width);
            pos.set_height(cluster[j].size.height);
            // pkg.set_position(pos);
            *(pkg.mutable_position()) = pos;
            reply->mutable_clusters(i)->add_packages();
            *(reply->mutable_clusters(i)->mutable_packages(j)) = pkg;
        }
    }
    cout << "--------------------------setData successful --------------------" << endl;
}

class OptimizerServiceImpl final : public optimizer::optimizer::Service
{
    Status StartService(ServerContext *context, const OptimizerRequest *request,
                   OptimizerResponse *reply) override
    {
        std::cout << "Received request" << std::endl;
        (*reply) = OptimizerResponse();
        // RoutePlanInterface* rp = new TSP_OR(EUCLIDEAN);
        // RoutePlanInterface* rp = new TSP_OR(REAL);
        // RoutePlanInterface* rp = new TSP_OR(REAL);
    	// ClusteringInterface* cls = new HGS(HAVERSINE,3.66,2.06);

	    BinPackInterface* bp =  new EB_AFIT;
        DataModel dm = getData(request);

        bool verbose = true;
	    bool logToFile = true;
	    string logFileName = "FESIF_TSP_LK.txt";

        vector<item> items = dm.packages;
        cout << "Before";
        int package_count = dm.packages.size();
        int l=1;int r=std::min<int>(25*dm.numRiders,package_count);int ans=1;
        BinPackInterface* binpack = new EB_AFIT;
        while(l<=r) {
            int mid = (l+r)/2;
            vector<item> current;
            auto bb = dm.bin;
            bb.size.height *= dm.numRiders;
            bb.size.height *=0.95;
            cout << "Before current";
            for(int i=0;i<mid;i++) current.push_back(items[i]);
            cout << "After";
            binpack->BinPack(current, bb);
            cout << "After binpack" << endl;
            cout<<l<<" "<<r<<" "<<binpack->CalculateCost()<<" " <<mid<<endl;
            double costTemp =   binpack->CalculateCost();
            cout << costTemp << endl;
            if(costTemp<0.0001) 
            {
                ans=mid;
                l=mid+1;
            }
            else r=mid-1;
        }
        cout << "After Binary";
        vector<item> final_item;
        for(int i=0;i<ans;i++) final_item.push_back(items[i]);
        dm.packages = final_item;

        vector<string> routingAlgorithms = {"TSP_OR", "TSP_LK", "TSP_CK"};
        vector<string> binPackingAlgorithms = {"EB_AFIT"};
        vector<string> clusteringAlgorithms = {"CLARKE", "SELF", "FESIF", "HGS"};

        // vector<string> routingAlgorithms = {"TSP_OR"};
        // vector<string> binPackingAlgorithms = {"EB_AFIT"};
        // vector<string> clusteringAlgorithms = {"CLARKE"};
        cout<<"Started ensembler"<<endl;
        Ensembler* optim = new Ensembler(routingAlgorithms, binPackingAlgorithms, clusteringAlgorithms, dm.packages, dm.warehouse, dm.numRiders, dm.bin); 
        cout<<"Completed ensembler formation."<<endl;
        try{
        cout<<"Running ensembler"<<endl;
        optim->EnsembleRun();
        cout<<"Essembler running done."<<endl;
        }
        catch(const char* msg)
        {
            std::cerr << msg << std::endl;
            return Status::CANCELLED;
        }
        catch(const string msg)
        {
            std::cerr << msg << std::endl;
            return Status::CANCELLED;
        }
        catch(...)
        {
            std::cerr << "Unknown exception" << std::endl;
            return Status::CANCELLED;
        }

        if(verbose)
            optim->Report();
        
        setData(optim,reply);
        return Status::OK;
    }
};

void RunServer(const std::string &port)
{
    std::string server_address("0.0.0.0:" + port);
    OptimizerServiceImpl service;

    ServerBuilder builder;
    builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
    builder.RegisterService(&service); 
    std::unique_ptr<Server> server(builder.BuildAndStart());
    std::cout << "Server listening on " << server_address << std::endl;
    server->Wait();
}

int32_t main(int argc, char **argv)
{
    RunServer("50051");
    return 0;
}
