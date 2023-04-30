#ifndef __LIN_KERNIGHAN_TSP__
#define __LIN_KERNIGHAN_TSP__
#include "../interface/RoutePlanInterface.hpp"

class TSP_LK : public RoutePlanInterface{
    public : 
        TSP_LK(DistanceType method): RoutePlanInterface(method){}
        void PlanRoute(vector<item>& cluster, Coordinate warehouse);
        void CalculateCost();

        void lin_kernighan();
        void three_opt(int tour[], int i, int j, int k);
        void two_opt(int tour[], int i, int j);
        double tour_distance(int tour[]);
        

        Coordinate getWarehouse(){
            return warehouse;
        }

    private:
        // std::vector<std::vector<int64_t>> distances;
        const int num_vehicles=1;
        // for scaling the distances from float to integer type
        const int SCALING_FACTOR=100; 
        Coordinate warehouse;

};


#endif