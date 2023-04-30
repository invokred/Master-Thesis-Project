// Hybrid Genetic Search-CVRP (HGS) Algorithm computes optimal clusters and routes simultaneously
#include "../../../include/clustering/HGS/HGS.hpp"
#include <map>

namespace CachingHGS{
	map<vector<item>, vector<vector<item>> > cacheMap;
}

/**
 * @brief Construct a new HGS::HGS object 
 * 
 * @param method 
 */
HGS::HGS(DistanceType method,bool EDD): ClusteringInterface(method){
			// pathInstance = instance_path_name;
			// pathSolution = solution_path_name;
			this->EDD = EDD;
			isRoundingInteger = false;
			verbose = true;
			pathSolution = "../tests/HGS_sol.txt";
			ap = default_algorithm_parameters();
			// ap.timeLimit = 30.0;
			// ap.nbIter = 25000;
			ap.nbIter = 2000;
}
HGS::HGS(DistanceType method,double penaltyDuration,double penaltyCapacity,bool EDD): ClusteringInterface(method){
			// pathInstance = instance_path_name;
			// pathSolution = solution_path_name;
			this->EDD = EDD;
			this->penaltyDuration = penaltyDuration;
			this->penaltyCapacity = penaltyCapacity;
			isRoundingInteger = false;
			verbose = true;
			pathSolution = "../tests/HGS_sol.txt";
			ap = default_algorithm_parameters();
			// ap.timeLimit = 30.0;
			// ap.nbIter = 25000;
			ap.nbIter = 2000;
}

/**
 * @brief evaluate and output details of location cluster that each rider is assigned to
 * 
 * @param packages details of packages to be delivered
 * @param warehouse depot/ warehouse information
 * @param numRiders number of riders
 * @param b details of bin/ rider bag
 */
void HGS::ComputeClusters(vector<item> &packages, Coordinate warehouse, int numRiders, Bin b) {
	if((CachingHGS::cacheMap).find(packages) != (CachingHGS::cacheMap).end()){
		clusters = (CachingHGS::cacheMap)[packages];
		return;
	}	
	int nbVeh = numRiders;
	// InstanceCVRPLIB cvrp(pathInstance, isRoundingInteger);
	int n = packages.size();
	// double time_to_deliver_in_km = 1.5; 
	double avgspeed = 0.35; // 21 km/hr
	double DurationLimit = 300 * avgspeed;//Duration Limit is given in kms
	double time_to_deliver_in_km = 0; 
	vector<double> x_coords(n+1);
	vector<double> y_coords(n+1);
	vector<double> demands(n+1,0.);
	vector<double> service_time(n+1, 7.5 * avgspeed);
	service_time[0] = 0;
	x_coords[0] = warehouse.latitude;
	y_coords[0] = warehouse.longitude;
	std::vector < std::vector< double > > dist_mtx = std::vector < std::vector< double > >(n + 1, std::vector <double>(n + 1));
	for(int i = 1;i <= n;i++){
		x_coords[i] = packages[i-1].coordinate.latitude;
		y_coords[i] = packages[i-1].coordinate.longitude;
		demands[i] = packages[i-1].getVolume();
		// demands[i] = packages[i-1].weight;
	}
	for (int i = 0; i <= n; i++){
		for (int j = 0; j <= n; j++){
			if(i != 0 && j != 0)
				dist_mtx[i][j] = Dist(packages[i-1].coordinate,packages[j-1].coordinate);
			else if(i == 0 && j != 0)
				dist_mtx[i][j] = Dist(warehouse,packages[j-1].coordinate);
			else if(j == 0 && i != 0)
				dist_mtx[i][j] = Dist(packages[i-1].coordinate,warehouse);
			else dist_mtx[i][j] = 0;
			// dist_mtx[i][j] = Dist(packages[i].coordinate,packages[j].coordinate);
			if (isRoundingInteger) dist_mtx[i][j] = round(dist_mtx[i][j]);
		}
	}
	cout << "X : ";
	for(int i = 0;i < x_coords.size();i++){
		cout << x_coords[i]<< ' ' <<i<<' ';
	}
	cout << "\nY : ";
	for(int i = 0;i < y_coords.size();i++){
		cout << y_coords[i]<< ' ';
	}
	cout<<"\nD : ";
	double total_demand = 0;
	for(int i = 0;i < demands.size();i++){
		cout << demands[i]<< ' ';
		total_demand += demands[i];
	}
	cout << "\n\nBin Capacity : " << b.capacity << endl;
	cout << "\n\nBin Volume : " << b.getVolume() << endl;
	cout << "Total Demand : " << total_demand << endl;
	cout << "Number of Riders : " << numRiders << endl;
	cout << "Number of Packages : " << n << endl;
	cout << "Average Volume per cluster : " << total_demand / numRiders << endl << endl;
	cout << endl;
	std::vector<double> expectation(n, 1e30); 
	// TODO: add expectation in HGS
	for(int i=0;i<n;i++) expectation[i] = packages[i].time;
	
	if(this->EDD){
		cout<<"With EDD"<<endl;
		Params params = Params(x_coords,y_coords,dist_mtx,service_time,demands,
				b.capacity,DurationLimit,numRiders,true,verbose, expectation,ap);
		params.averageSpeed = avgspeed;
		print_algorithm_parameters(ap);
		GeneticParallel solver(params,4);
		solver.run();
		cout << "Solver Completed"<<endl;
		// Exporting the best solution
		const Individual indiv = *solver.population->getBestFound();
		if (solver.population->getBestFound() != NULL)
		{
			for (int k = 0; k < (int)indiv.chromR.size(); k++)
			{
				if (!indiv.chromR[k].empty())
				{
					vector<item> cluster;
					for (int i : indiv.chromR[k]){
						if(i != 0)
							cluster.push_back(packages[i-1]);
					} 
					clusters.push_back(cluster);
				}
			}
			if (params.verbose) std::cout << "----- WRITING BEST SOLUTION IN : " << pathSolution << std::endl;
			solver.population->exportCVRPLibFormat(*solver.population->getBestFound(), pathSolution);
			solver.population->exportSearchProgress(pathSolution + ".PG.csv", pathInstance);
		}
	}
	else{
		expectation = std::vector<double>(n+1,1e30); 
		// Params params(x_coords,y_coords,dist_mtx,service_time,demands,
		// 		b.capacity,1.e30,numRiders,false,verbose, expectation,ap);
		Params params(x_coords,y_coords,dist_mtx,service_time,demands,
				b.capacity,DurationLimit,numRiders,true,verbose, expectation,ap);
		params.averageSpeed = avgspeed;
		params.penaltyCapacity = this->penaltyCapacity;
		params.penaltyDuration = this->penaltyDuration;
		// params.penaltyMoreThan25 = 2*avgspeed;
		print_algorithm_parameters(ap);
		GeneticParallel solver(params,4);
		solver.run();
		cout << "Solver Completed"<<endl;
		// Exporting the best solution
		
		if (solver.population->getBestFound() != NULL)
		{	
			cout << "Solution Found !" << endl;
			Individual indiv = *solver.population->getBestFound();
			for (int k = 0; k < (int)indiv.chromR.size(); k++)
			{
				if (!indiv.chromR[k].empty())
				{
					vector<item> cluster;
					for (int i : indiv.chromR[k]){
						if(i != 0)
							cluster.push_back(packages[i-1]);
					} 
					clusters.push_back(cluster);
				}
			}
			if (params.verbose) std::cout << "----- WRITING BEST SOLUTION IN : " << pathSolution << std::endl;
			solver.population->exportCVRPLibFormat(*solver.population->getBestFound(), pathSolution);
			solver.population->exportSearchProgress(pathSolution + ".PG.csv", pathInstance);
		}
		else {
			cout << "best found NULL" << endl;
			clusters.clear();
		}
		
	}
	(CachingHGS::cacheMap)[packages] = clusters;		
}

void HGS::CalculateCost() {}
