// TSP approximation using google OR tools
#ifndef __TSP_OR_EDD__
#define __TSP_OR_EDD__

#include"../interface/RoutePlanInterface.hpp"
#include "ortools/constraint_solver/routing.h"
#include "ortools/constraint_solver/routing_enums.pb.h"
#include "ortools/constraint_solver/routing_index_manager.h"
#include "ortools/constraint_solver/routing_parameters.h"

using namespace operations_research;
class TSP_OR_EDD : public RoutePlanInterface{

    public : 
        TSP_OR_EDD(DistanceType method) : RoutePlanInterface(method){ routing_method = 1; }
        void PlanRoute(vector<item>& cluster, Coordinate warehouse);
        void CalculateCost();
        // convert longitude to X

        // Compute distances between every pair of points
        void ComputeEuclideanDistanceMatrix(std::vector<item>& cluster);

        // Convert the Google OR-Tool based route to vector<item> Path
        void savePath(vector<item>&clusters, const RoutingIndexManager& manager,
                   const RoutingModel& routing, const Assignment *solution);

        Coordinate getWarehouse(){
            return warehouse;
        }
    private:
        
        std::vector<std::vector<int64_t>> distances;
        const int num_vehicles=1;
        const int speed = 30;
        // for scaling the distances from float to integer type
        const int scale = 1000;
        Coordinate warehouse;
        const RoutingIndexManager::NodeIndex depot{0};
};

#endif