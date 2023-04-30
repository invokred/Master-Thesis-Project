/**
 * RoutePlanInterface.hpp
 * ~Rohit Raj
**/
#ifndef __ROUTEPLANPACK_INTERFACE__
#define __ROUTEPLANPACK_INTERFACE__

#include<vector>
#include<fstream>
#include<iostream>
#include"../datastructures.hpp"
#include "../DistMatrix.hpp"
using namespace std;

class RoutePlanInterface{
    public:
    int drop_offs=0;
    bool routing_method = 0;
    const int SCALING_FACTOR=100; 
    vector<vector<double>> DistMatrix;
    DistanceType method;
        RoutePlanInterface(DistanceType method) {
            this -> method = method;
            cost = 0;
        }
        // store the path planned for all clusters  
        virtual void PlanRoute(vector<item>& cluster, Coordinate warehouse) = 0;
        // virtual void PlanRoute(vector<item>& cluster, Coordinate warehouse) = 0;
        // calculate the cost of the path computed 
        virtual void CalculateCost() = 0; 
        void ComputeDistMatrix(vector<item>& cluster,Coordinate warehouse){
            vector<Coordinate> coordinates;
            coordinates.push_back(warehouse);
            for(auto& elt: cluster){
                coordinates.push_back(elt.coordinate);
            }
            DistMatrix = compute_matrix(coordinates, method,0);
        }
        void PrintRoutes()
        {
            int32_t i=0;
            cout<<"Printing the routes for the cluster -- " << endl;
            cout<<"Warehouse -> ";
            for(auto &it : plannedPath){
                it.print(); cout<<" -> ";
            }
            cout<<"-> Warehouse"<<endl;
            return;
        }

        void PrintRoutesToFile(string filename)
        {
            std::ofstream out(filename, std::ios_base::app);
            int32_t i=0;
            out<<"The Routes for the cluster -- " << endl;
            out<<"Warehouse -> ";
            for(auto &it : plannedPath){
                it.printToFile(out); out<<" -> ";
            }
            out<<"-> Warehouse"<<endl;
            return;
        }

        vector<item> GetPaths(){
            return plannedPath;
        }

        float GetPathPlanningCost(){
            return cost;
        }
        double Dist(struct Coordinate c1,struct Coordinate c2){
            return getDist(c1,c2,method);
           
        }
    protected:
        
        vector<item> plannedPath;
        float cost;

};

#endif 