#include "../include/Ensembler.hpp"
#include <pthread.h>

namespace myNamespace{
    vector<BinPackInterface*> BinPackingInterfaces;
    vector<ClusteringInterface*> ClusteringInterfaces;
    vector<RoutePlanInterface*> RoutePlanInterfaces; 

    vector<pthread_mutex_t*> locksClustering;
    vector<pthread_mutex_t*> locksRouting;
    vector<pthread_mutex_t*> locksBinPacking;

    vector<Optimizer*> optimizersUsed;
}



Ensembler::Ensembler(vector<string>& RoutePlanningAlgorithms_, vector<string>& BinPackingAlgorithms_, vector<string>& ClusteringAlgorithms_, vector<item>& packages_, Coordinate& warehouse_, int numberRiders_, Bin& bin_){
    /**
     * @brief for the Ensembler class. Initializes the RoutePlanningAlgorithms, BinPackingAlgorithms, and ClusteringAlgorithms 
     * based on the input vectors, packages, warehouse, numberRiders, and bin. 
     * 
     * @param RoutePlanningAlgorithms_ Vector of strings representing the route planning algorithms to be used. 
     * @param BinPackingAlgorithms_ Vector of strings representing the bin packing algorithms to be used. 
     * @param ClusteringAlgorithms_ Vector of strings representing the clustering algorithms to be used. 
     * @param packages_ Vector of item objects representing the packages to be delivered. 
     * @param warehouse_ Coordinate object representing the location of the warehouse. 
     * @param numberRiders_ Integer representing the number of riders for delivery. 
     * @param bin_ Bin object representing the bin for the packages. 
     */
    RoutePlanningAlgorithms = RoutePlanningAlgorithms_;
    BinPackingAlgorithms = BinPackingAlgorithms_;
    ClusteringAlgorithms = ClusteringAlgorithms_;

    if(myNamespace::RoutePlanInterfaces.empty()){
        for(auto routeAlgo :  RoutePlanningAlgorithms){
            pthread_mutex_t* lockAlgo = new pthread_mutex_t;
            pthread_mutex_init(lockAlgo, NULL);
            myNamespace::locksRouting.push_back(lockAlgo);
            
            RoutePlanInterface* rp = NULL;
            if (routeAlgo == "TSP_OR") rp = new TSP_OR(REAL);
            else if (routeAlgo == "TSP_LK") rp = new TSP_LK(REAL);
            else if (routeAlgo == "TSP_CK") rp = new TSP(REAL);
            else if (routeAlgo == "TSP_OR_EDD") rp = new TSP_OR_EDD(HAVERSINE);
            myNamespace::RoutePlanInterfaces.push_back(rp);
        }
    }

    if(myNamespace::ClusteringInterfaces.empty()){
        for(auto clusteringAlgo :  ClusteringAlgorithms){
            pthread_mutex_t* lockAlgo = new pthread_mutex_t;
            pthread_mutex_init(lockAlgo, NULL);
            myNamespace::locksClustering.push_back(lockAlgo);
            cout<<clusteringAlgo<<" "<<(lockAlgo)<<endl;

            ClusteringInterface* cls = NULL;
            if (clusteringAlgo == "CLARKE") cls = new Clarke(HAVERSINE);
            else if (clusteringAlgo == "FESIF") cls = new Clarke(HAVERSINE);
            else if (clusteringAlgo == "SELF") cls = new SELFCLUSTERING(HAVERSINE);
            else if (clusteringAlgo == "HGS") cls = new HGS(HAVERSINE, 3.66,2.06);
            myNamespace::ClusteringInterfaces.push_back(cls);
        }
    }

    if(myNamespace::BinPackingInterfaces.empty()){
        for(auto binpackingAlgo :  BinPackingAlgorithms){
            // pthread_mutex_t* lockAlgo = new pthread_mutex_t;
            // pthread_mutex_init(lockAlgo, NULL);
            myNamespace::locksBinPacking.push_back(NULL);

            BinPackInterface* bp = NULL;
            if (binpackingAlgo == "EB_AFIT") bp = new EB_AFIT;
            myNamespace::BinPackingInterfaces.push_back(bp);
        }
    }

    packages = packages_;
    bin = bin_;
    warehouse = warehouse_;
    numberRiders = numberRiders_;
}

void Ensembler::EnsembleRun(){
    /**
    * @brief EnsembleRun function for performing the ensemble run for the delivery problem.
    * The function performs a three-level nested loop to generate combinations of route planning, clustering, and bin packing algorithms.
    * For each combination, a new instance of the Ensembler class is created and run in a separate thread.
    * The threads are stored in a vector of pthread_t and joined at the end of the function.
    * The costs of each run are then calculated as a sum of routing cost and packaging cost, with a weight of 0.5.
    * The run with the minimum cost is chosen as the best run and its clusters, paths, and bin packing information is stored.
    * The costs of all the runs are also stored in the Costs vector.
    * 
    * @return None.
    */
    int numRoutePlanners = RoutePlanningAlgorithms.size();
    int numClusteringAlgos = ClusteringAlgorithms.size();
    int numBinpackingAlgos = BinPackingAlgorithms.size();

    vector<pthread_t> threadsRunning;

    for(int i=0;i<numRoutePlanners;i++){
        for(int j=0;j<numClusteringAlgos;j++){
            for(int k=0;k<numBinpackingAlgos;k++){
                vector<string> Combination = {RoutePlanningAlgorithms[i], ClusteringAlgorithms[j], BinPackingAlgorithms[k]};

                Ensembler* temp = new Ensembler(RoutePlanningAlgorithms, BinPackingAlgorithms, ClusteringAlgorithms, packages, warehouse, numberRiders, bin);

                temp->currentCombination = new int[3];
                temp->currentCombination[0] = i;
                temp->currentCombination[1] = j;
                temp->currentCombination[2] = k;

                pthread_t thread;
                int threadStatus = pthread_create(&thread, NULL, run, (void *)temp);
                cout<<"thread creating with "<<" "<<RoutePlanningAlgorithms[i]<<" "<<BinPackingAlgorithms[k]<<" "<<ClusteringAlgorithms[j]<<endl;
                threadsRunning.push_back(thread);
                ensemblersUsed.push_back(temp);
            }
        }
    }
    for(auto thread:threadsRunning){
        cout<<"Thread joined"<<endl;
        pthread_join(thread, NULL);
    }

    double minCost = 1e9;
    Optimizer* minIndex = NULL;

    for(auto tempOptimizer : (myNamespace::optimizersUsed)){
        double weight = 0.5;

        double routePlanningCost = 0.0;
        for(double cost: tempOptimizer->GetRoutingCost()){
            routePlanningCost += cost;
        }

        double packagingCost = 0.0;
        for(double cost: tempOptimizer->GetPackagingCost()){
            packagingCost += cost;
        }

        double cost = routePlanningCost + weight * packagingCost;
        if(cost < minCost){
            minCost = cost;
            minIndex = tempOptimizer;
        }
    }

    int num = minIndex->getNumClusters();

    for(int i = 0;i<num;i++){
        auto x = ((myNamespace::optimizersUsed)[0])->GetPackagingForCluster(i);
        ((myNamespace::optimizersUsed)[0])->getPackingLog(x);
    }
    vector<vector<item>> bestClusters = minIndex->GetClusters();
    int counter = 0;
    cout<<"Best Cluster Count: "<<bestClusters.size()<<endl;
    for(auto cluster : bestClusters){
        cout<<"Logging information for cluster - "<<counter<<endl;
        counter++;
        minIndex->getPackingLog(cluster);
    }

    cout<<"Min Cost: "<<minCost<<endl;

    clusters = minIndex->GetClusters();
    paths.clear();
    binPacking.clear();
    cout<<"Received Clusters"<<" "<<clusters.size()<<endl;
    for(int i=0;i<clusters.size();i++){
        cout<<"Cluster"<<" "<<i<<" with size: "<<clusters[i].size()<<endl;
        for(int j=0;j<clusters[i].size();j++) {
            cout<<clusters[i][j].coordinate.latitude<<" "<<clusters[i][j].coordinate.longitude<<endl;
        }
        // for(int j=0;j<clusters[i].size();j++) 
        // paths.push_back(minIndex->GetPathForCluster(i));
        // cout<<"Paths calculated for index: "<<i<<endl;
        binPacking.push_back(minIndex->GetPackagingForCluster(i));
        cout<<"Bin calculated for index: "<<i<<endl;
    }
    cout<<"Path planning and bin packing done"<<endl;

    clusters.clear();
    clusters.resize(minIndex->getNumClusters());

    for(int i=0;i<minIndex->getNumClusters();i++){
        clusters[i] = minIndex->GetPackagingForCluster(i);
    }

    for(auto ensembler: ensemblersUsed){
        Costs.push_back(ensembler->GetCosts()[0]);
    }

    return;
}

vector<item> Ensembler::GetPackagingForCluster(int cluster){
    /**
    * @brief Get the packaging for a cluster
    * 
    * @param cluster - The cluster number
    * @return vector<item> - The packaging for the cluster
    */
    return clusters[cluster];
}

int Ensembler::getNumClusters(){
    /**
    * @brief Get the number of clusters
    * 
    * @return int - The number of clusters
    */
    return clusters.size();
}



vector<pair<vector<string>, double>> Ensembler::GetCosts(){
    /**
    * @brief Get the costs of all the combinations of algorithms used
    *
    * @return vector<pair<vector<string>, double>> - A vector of pairs containing the combination of algorithms and the corresponding cost
    */
    return Costs;
}

void Ensembler::Report(){
    /**
    * 
    * @brief Writes the costs of each combination of algorithms to a file
    * This function takes in the costs of each combination of algorithms and writes it to a file. The file name is constructed
    * using the names of the algorithms and the file is saved in the "tests" directory. The costs are written to the file in a single
    * line, with the cost of the combination written in a double precision floating point format.
    * @param Costs a vector of pairs, each pair representing the combination of algorithms and its cost
    * @return void
    * 
    */
    for(auto x:Costs){
        string fileName = x.first[0] + x.first[1] + x.first[2];
        fileName += ".txt";

        ofstream output("./../tests/"+fileName);
        output<<x.second<<endl;
        output.close();
    }
    return;
}


void * run(void * arg){
    /**
    * @brief run function that runs the combinations of algorithms to optimize delivery of packages
    * 
    * @param arg Pointer to the Ensembler class object
    *
    * @return void
    *
    * This function runs the combination of algorithms chosen by the Ensembler class to optimize delivery of packages. It creates a temporary Optimizer object and optimizes the delivery by calling its optimize() method.
    * The costs of each optimization process are then calculated by adding the routing and packaging costs and stored as a pair of algorithm combination and total cost in the Costs vector of the Ensembler class object.
    */
    Ensembler* ensembler = (Ensembler*) arg;

    cout<<"Running combination - "<<ensembler->RoutePlanningAlgorithms[ensembler->currentCombination[0]]<<" "<<ensembler->ClusteringAlgorithms[ensembler->currentCombination[1]]<<" "<<ensembler->BinPackingAlgorithms[ensembler->currentCombination[2]]<<endl;;

    Optimizer* tempOptimizer = new Optimizer(myNamespace::RoutePlanInterfaces[ensembler->currentCombination[0]], myNamespace::ClusteringInterfaces[ensembler->currentCombination[1]], myNamespace::BinPackingInterfaces[ensembler->currentCombination[2]], ensembler->packages, ensembler->warehouse, ensembler->numberRiders, ensembler->bin, "", 0, 0, myNamespace::locksClustering[ensembler->currentCombination[1]], myNamespace::locksBinPacking[ensembler->currentCombination[2]], myNamespace::locksRouting[ensembler->currentCombination[0]]);

    tempOptimizer->optimize();

    (myNamespace::optimizersUsed).push_back(tempOptimizer);

    vector<string> Combination = {ensembler->RoutePlanningAlgorithms[ensembler->currentCombination[0]], ensembler->ClusteringAlgorithms[ensembler->currentCombination[1]], ensembler->BinPackingAlgorithms[ensembler->currentCombination[2]]};
    double weight = 0.5;

    double routePlanningCost = 0.0;
    for(double cost: tempOptimizer->GetRoutingCost()){
        routePlanningCost += cost;
    }

    double packagingCost = 0.0;
    for(double cost: tempOptimizer->GetPackagingCost()){
        packagingCost += cost;
    }

    (ensembler->Costs).push_back({Combination, routePlanningCost + weight * packagingCost});
}