/**
 * Optimizer.hpp
 * ~Rohit Raj
**/
#ifndef __OPTIMIZER__
#define __OPTIMIZER__
#include<string>
#include "interface/RoutePlanInterface.hpp"
#include "interface/BinPackInterface.hpp"
#include "interface/ClusteringInterface.hpp"
#include <pthread.h>

using namespace std;

class Optimizer{
    public:
        Optimizer(){}
        Optimizer(RoutePlanInterface* routePlannerInterface_, ClusteringInterface* clusteringInterface_, BinPackInterface* binPackInterface_, vector<item>& packages_, Coordinate& warehouse_, int numberRiders_, Bin& bin_, string logFileName_, bool verbose_, bool logToFile_, pthread_mutex_t* clusterLock = NULL, pthread_mutex_t* binPackLock = NULL, pthread_mutex_t* routePlanningLock = NULL);

        // Function to perform optimization for set of packages
        void optimize();
        void check_data();
        vector<vector<item>> GetClusters();

        void getPackingLog(vector<item>&);

        vector<item> GetPathForCluster(int seqNumberOfCluster);

        vector<item> GetPackagingForCluster(int seqNumberOfCluster);
        float GetClusteringCost();

        vector<float> GetPackagingCost();

        vector<float> GetRoutingCost();
        int getNumClusters();
        int dropoffs = INT_MAX;
        RoutePlanInterface* routePlannerInterface;
        ClusteringInterface* clusteringInterface;
        BinPackInterface* binPackInterface;
        vector<item> packages;
        Coordinate warehouse;
        int numberRiders;
        Bin bin;
        std::string logFileName;
        bool verbose;
        bool logToFile;
        int counter;

        vector<vector<item>> clusters;
        vector<vector<item>> clusterPaths;
        vector<vector<item>> clusterPackagings;

        pthread_mutex_t* clusterLock;
        pthread_mutex_t* routingLock;
        pthread_mutex_t* binPackLock;

        // Cost of each segment
        float clusteringCost;
        vector<float> packagingCost;
        vector<float> routePlanningCost;
};

#endif 