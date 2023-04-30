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
#include "./include/clustering/HGS/HGS.hpp"

#include "./include/binpack/EB_AFIT.hpp"

#include "./include/Ensembler.hpp"


class DataModel{
	public:
	Coordinate warehouse;
	Bin bin;
	vector<item> packages;
	int optimal;
	int numRiders = 5;
};


int get_rand(int a,int b){
    return a + rand()%(b - a);
}

DataModel ReadVRPs(string filename){
	DataModel dm;
	ifstream inputFile;
	inputFile.open(filename);
	int index = 0;
	cout << 'a';
	while(filename[index] != 'k')
		index++;
	index++;
	string nums;
	cout << 'b';
	while(filename[index] != '.'){
		nums.push_back(filename[index]);
		index++;
	}
	cout << 'c';
	dm.numRiders = stoi(nums);
	cout << nums << dm.numRiders << endl;
	if(!inputFile.is_open()){
		std::cout<<"Cannot Open File\n";
	}

	Coordinate warehouse;
	std::string content, content2, content3;
	double serviceTimeData = 0.;
	
	getline(inputFile, content);
	getline(inputFile, content);
	string opt;
	index = content.length()-1;
	while(content[index] != ' '){
		opt.push_back(content[index]);
		index--;
	}
	reverse(opt.begin(),opt.end());
	dm.optimal = stoi(opt.substr(0,opt.length() - 1));
	getline(inputFile, content);
	int nbClients;int durationLimit;
	double vehicleCapacity = 1.e30;
	bool isDurationConstraint;
	for (inputFile >> content ; content != "NODE_COORD_SECTION" ; inputFile >> content)
	{
		if (content == "DIMENSION") { inputFile >> content2 >> nbClients; nbClients--; } // Need to substract the depot from the number of nodes
		else if (content == "EDGE_WEIGHT_TYPE")	inputFile >> content2 >> content3;
		else if (content == "CAPACITY")	inputFile >> content2 >> vehicleCapacity;
		else if (content == "DISTANCE") { inputFile >> content2 >> durationLimit; isDurationConstraint = true; }
		else if (content == "SERVICE_TIME")	inputFile >> content2 >> serviceTimeData;
		else throw std::string("Unexpected data in input file: " + content);
	}
	if (nbClients <= 0) throw std::string("Number of nodes is undefined");
	if (vehicleCapacity == 1.e30) throw std::string("Vehicle capacity is undefined");
	
	std::vector<double> x_coords = std::vector<double>(nbClients);
	std::vector<double> y_coords = std::vector<double>(nbClients);
	std::vector<double> demands = std::vector<double>(nbClients);
	std::vector<double> service_time = std::vector<double>(nbClients);


	// Reading node coordinates
	// depot must be the first element
	// 		- i = 0 in the for-loop below, or
	// 		- node_number = 1 in the .vrp file
	// customers are
	// 		- i = 1, 2, ..., nbClients in the for-loop below, or
	// 		- node_number = 2, 3, ..., nb_Clients in the .vrp file

	int node_number;
	int depot_lat,depot_lon;
	inputFile >> node_number >> depot_lat>> depot_lon ;
	for (int i = 0; i < nbClients; i++)
	{
		inputFile >> node_number >> x_coords[i] >> y_coords[i];
		if (node_number != i + 2) throw std::string("The node numbering is not in order.");
	}
	cout << "coordinates taken" << endl;
	// Reading demand information
	inputFile >> content;
	if (content != "DEMAND_SECTION") throw std::string("Unexpected data in input file: " + content);

	inputFile >> content >> content;
	for (int i = 0; i < nbClients; i++)
	{
		inputFile >> node_number >> demands[i];
		if (node_number != i + 2) throw std::string("The node numbering is not in order.");
		// service_time[i] = (i == 0) ? 0. : serviceTimeData ;
	}

	
	cout << "demands taken" << endl;
	// Reading depot information (in all current instances the depot is represented as node 1, the program will return an error otherwise)
	inputFile >> content >> content2 >> content3 >> content3;
	if (content != "DEPOT_SECTION") throw std::string("Unexpected data in input file: " + content);
	if (content2 != "1") throw std::string("Expected depot index 1 instead of " + content2);
	if (content3 != "EOF") throw std::string("Unexpected data in input file: " + content3);

	
	// add items to datamodel
	dm.packages.resize(nbClients);
	dm.bin.capacity = vehicleCapacity;
	dm.bin.size.height = 100;
	dm.bin.size.length = 80;
	dm.bin.size.width = 80;
	dm.warehouse.latitude  = depot_lat;
	dm.warehouse.longitude = depot_lon;
	int ub = 14 , lb = 9;
	srand ( time(NULL) );

    vector<int> min_dimensions(3),max_dimensions(3),bin_dimensions(3);
    min_dimensions = {3,3,3};
    max_dimensions = {40,20,40};
    bin_dimensions = {80,100,80};


	for(int i=0; i<nbClients; i++)
	{
		dm.packages[i].coordinate.latitude = x_coords[i];
		dm.packages[i].coordinate.longitude = y_coords[i];
		dm.packages[i].volume = 1;
		dm.packages[i].size.width = get_rand(min_dimensions[0],max_dimensions[0]);
		dm.packages[i].size.height = get_rand(min_dimensions[1],max_dimensions[1]);
		dm.packages[i].size.length = get_rand(min_dimensions[2],max_dimensions[2]);
		dm.packages[i].weight = demands[i];
		// dm.packages[i].time = ((rand() % (ub - lb + 1)) + lb)*60 ;
		dm.packages[i].time = ((rand() % (ub - lb))+1);
	}
	return dm;
}


// int main(int argc, char** argv) {
    
// //  	// ifstream input;
// //  	// input.open("input.txt");
// //  	Coordinate warehouse;
// //  	cin>>warehouse.longitude>>warehouse.latitude;
// //  	Bin bin; 
// //  	cin>>bin.size.length>>bin.size.width>>bin.size.height;
// //  	bin.capacity = 25;
// //  	int n;
// //  	cin>>n;
// //  	vector<item> items(n);
// //  	for(int i=0;i<n;i++) {
// //  		cin>>items[i].coordinate.longitude>>items[i].coordinate.latitude;
// //  		cin>>items[i].size.length>>items[i].size.width>>items[i].size.height;
// // 		items[i].volume = items[i].size.length * items[i].size.width * items[i].size.height;
// //  		items[i].weight = 1;
// //  	}
// // 	int numRiders = 5;
// // 	RoutePlanInterface* rp = new TSP;
// // 	ClusteringInterface* cls = new FESIF;
// // 	BinPackInterface* bp =  new EB_AFIT;
// // 	Optimizer optim(rp, cls, bp, items, warehouse, numRiders, bin,"FESIF_TSP.txt", false, true);

// // 	optim.optimize();
// // 	vector<float> costs = optim.GetRoutingCost();
// // 	float total_cost = 0;
// // 	for(int i = 0 ; i< costs.size();i++)
// // 		total_cost+=costs[i];
// // 	cout<<"Total Cost of All Routes is ===> "<< total_cost<<endl;
// //  	// f.ComputeClusters(items, warehouse, 100, bin); //wrapperLMD(items, warehouse, 100, bin);
// //  	// freeGlobalMemory();
// //  	// f.localFree();
// // 	// f.PrintClustersToFile("clusters.txt");
 	
// // 	// TSP_OR tsp;
// // 	// tsp.PlanRoute(f.GetClusters()[0], f.getWarehouse());
// // 	return 0;
// //  }

// int main1(int argc, char** argv){
// 	DataModel dm;
	
// 	if(argv[1] != NULL){
// 		try{
// 			dm = ReadVRPs(argv[1]);
// 		}
// 		catch(string err){
// 			cout << err;
// 		}
		
// 	}
// 	else{
// 		string s;
// 		cin >> s;
// 		try{
// 			dm = ReadVRPs(s);
// 		}
// 		catch(string err){
// 			cout << err;
// 		}
// 		// dm = ReadVRPs(s);
// 	}
// 	// string s;
// 	// cin >> s;
// 	// cout << s << endl;
// 	// try{
// 	// 	dm = ReadVRPs(s);
// 	// }
// 	// catch(string err){
// 	// 	cout << err << endl;
// 	// }
// 	cout << "Capacity : " << dm.bin.capacity << " depoX :" << dm.warehouse.latitude <<" depoY : " << dm.warehouse.longitude << endl;
// 	for(int i = 0;i < dm.packages.size();i++){
// 		cout << i << " : x : " << dm.packages[i].coordinate.latitude << " y : " << dm.packages[i].coordinate.longitude << " weight : "<< dm.packages[i].weight << endl;
// 	}
// 	RoutePlanInterface* rp = new TSP_LK(EUCLIDEAN);
// 	ClusteringInterface* cls = new FESIF(EUCLIDEAN);
// 	BinPackInterface* bp =  new EB_AFIT;
// 	Optimizer optim(rp, cls, bp, dm.packages, dm.warehouse, dm.numRiders, dm.bin,"FESIF_TSP.txt", false, false);

// 	optim.optimize();
// 	vector<float> rcosts = optim.GetRoutingCost();
// 	float total_cost = 0;
// 	for(auto &x : rcosts)
// 	{
// 		total_cost+=x;
// 	}	

// 	if(verbose)
// 		std::cout<<"\nTotal Cost for routing: "<<total_cost<<" km"<<std::endl;
 	
// 	if(logToFile)
// 	{
// 		std::ofstream out(logFileName, std::ios_base::app);
// 		out<<"\nTotal Cost for routing: "<<total_cost<<" km"<<std::endl;
// 	}

// 	return 0;
//  }

int32_t main(int argc, char** argv){
	DataModel dm;
	
	if(argv[1] != NULL){
		try{
			dm = ReadVRPs(argv[1]);
		}
		catch(string err){
			cout << err;
		}
	}
	else{
		string s;
		cin >> s;
		try{
			dm = ReadVRPs(s);
		}
		catch(string err){
			cout << err;
		}
		// dm = ReadVRPs(s);
	}
	// string s;
	// cin >> s;
	// cout << s << endl;
	// try{
	// 	dm = ReadVRPs(s);
	// }
	// catch(string err){
	// 	cout << err << endl;
	// }
	cout << "Capacity : " << dm.bin.capacity << " depoX :" << dm.warehouse.latitude <<" depoY : " << dm.warehouse.longitude << endl;
	for(int i = 0;i < dm.packages.size();i++){
		cout << i << " : x : " << dm.packages[i].coordinate.latitude << " y : " << dm.packages[i].coordinate.longitude << " weight : "<< dm.packages[i].weight << endl;
	}

	vector<string> routingAlgorithms = {"TSP_OR", "TSP_LK", "TSP_CK", "TSP_OR_EDD"};
	vector<string> binPackingAlgorithms = {"EB_AFIT"};
	vector<string> clusteringAlgorithms = {"CLARKE", "SELF", "FESIF", "HGS"};

	Ensembler* ens = new Ensembler(routingAlgorithms, binPackingAlgorithms, clusteringAlgorithms, dm.packages, dm.warehouse, dm.numRiders, dm.bin); 

	ens->EnsembleRun();
	ens->Report();

	return 0;
}
