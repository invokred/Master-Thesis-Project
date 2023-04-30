#ifndef __CLUSTERING_HGS__
#define __CLUSTERING_HGS__
// Hybrid Genetic Search-CVRP Algorithm computes optimal clusters and routes simultaneously
#include "../../interface/ClusteringInterface.hpp"
// #include "commandline.h"
#include "AlgorithmParameters.h"
#include "Genetic.h"
#include "LocalSearch.h"
#include "Split.h"
#include "InstanceCVRPLIB.h"
#include <iostream>
#include <string>
#include <climits>
#define edd true

using namespace std;

class HGS : public ClusteringInterface
{
	public:

		const int SCALING_FACTOR=100; 
		bool EDD;
		
		HGS(DistanceType method,bool EDD = false);
		double penaltyDuration,penaltyCapacity;
		HGS(DistanceType method,double penaltyDuration,double penaltyCapacity,bool EDD = false);
		// double Dist(struct Coordinate c1,struct Coordinate c2){
        //     if(method == EUCLIDEAN)
        //         return euclidean(c1,c2);
        //     else if(method == HAVERSINE) 
        //         return haversine(c1,c2)*SCALING_FACTOR;
        // }
		
		void ComputeClusters(vector<item> &packages, Coordinate warehouse, int numRiders, Bin b);
		void CalculateCost();

	private:
		string pathInstance;		// Instance path
		string pathSolution;		// Solution path
		bool isRoundingInteger;
		bool verbose;
		AlgorithmParameters ap;
		
};
#endif  