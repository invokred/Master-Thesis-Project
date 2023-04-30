/**
 * ClusteringInterface.hpp 
 * ~ Archisman Pathak
**/

#ifndef __CLUSTERING_INTERFACE__
#define __CLUSTERING_INTERFACE__

#include<vector>
#include<cstdint>
#include<string>
#include<iostream>
#include<fstream>
#include "../datastructures.hpp"
#include "../DistMatrix.hpp"

using namespace std;

class ClusteringInterface{
    public:
        const int SCALING_FACTOR=100;
        bool clustering_method=0;
        bool multithreading =0;
        double Spatial_Factor = 2;
        double Temporal_Factor = 0;
        double best_Temporal_Factor = 0;
        double best_Spatial_Factor = 0;
        const double Spatial_Factor_Lower_Limit = 0;
        const double Spatial_Factor_Upper_Limit = 10;
        const double Temporal_Factor_Lower_Limit = 0;
        const double Temporal_Factor_Upper_Limit = 1; 
        DistanceType method;
        ClusteringInterface(DistanceType method) {
            this->method = method;
            cost = 0;
        }
        // store the computed cluster in clusters 
        virtual void ComputeClusters(vector<item>& packages, Coordinate warehouse, int numRiders, Bin b) = 0;
        // calculate the cost of the cluster computed 
        virtual void CalculateCost() = 0;

        void PrintClusters()
        {
            int32_t i=0;
            for(auto &cluster : clusters)
            {
                cout<<"Printing Cluster -- " << i++ <<endl;
                for(auto &it : cluster){
                    it.print();
                }
                
            }
            return;
        }

        void PrintClustersToFile(string filename)
        {
            std::ofstream out(filename, std::ios_base::app);
            int32_t i=0;
            for(auto &cluster : clusters)
            {
                out<<"Printing Cluster -- " << i++ <<" \n";
                for(auto &it : cluster){
                    it.printToFile(out);
                }
                
            }
            return;
        }

        vector<vector<item>> GetClusters(){
            return clusters;
        }

        float GetClusteringCost(){
            return cost;
        }

        void setClusters(vector<vector<item> > createdClusters) {
            clusters = createdClusters;
        }

        Coordinate getWarehouse(){
            return warehouse;
        }
        double Dist(struct Coordinate c1,struct Coordinate c2){
            return getDist(c1,c2,method);
            // if(method == EUCLIDEAN)
            //     return euclidean_dist(c1,c2);
            // else if(method == HAVERSINE) 
            //     return haversine_dist(c1,c2)*SCALING_FACTOR;
            // else if(method == MANHATTAN)
            //     return manhattan_dist(c1,c2);
        }
    protected:
        int speed = 30;
        vector<vector<item>> clusters;
        float cost;
        Coordinate warehouse;
        Bin b;
        int numRiders;

};
#endif 