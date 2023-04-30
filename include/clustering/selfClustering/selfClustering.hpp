#ifndef SELF_CLUSTERING_H
#define SELF_CLUSTERING_H
#include <bits/stdc++.h>
#include "../../interface/ClusteringInterface.hpp"
#include "../../datastructures.hpp"
// #include "cluster.hpp"

using namespace std;

class SELFCLUSTERING : public ClusteringInterface {

    private:
        vector<vector<item>> calculateCluster(vector<item>& items, Coordinate warehouse, int numberOfRiders, Bin bin);

    public:
        const static int angularCutMetric = 3;
        SELFCLUSTERING(DistanceType method) : ClusteringInterface(method){
            // delta = rho;
        }
        void ComputeClusters(vector<item>& packages, Coordinate warehouse, int numRiders, Bin b) {
            setClusters(calculateCluster(packages, warehouse, numRiders, b));
        }
        void CalculateCost(){
            cost = 0;
        }
};

#endif  