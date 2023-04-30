/**
 * BinPackInterface.hpp
 * ~Archisman Pathak
**/
#ifndef __BINPACK_INTERFACE__
#define __BINPACK_INTERFACE__

#include<vector>
#include<fstream>
#include<iostream>
#include"../datastructures.hpp"

using namespace std;
class BinPackInterface{
    public:
        BinPackInterface() {
            cost = 0;
        }
        // store the computed cluster in clusters 
        virtual void BinPack(vector<item>& cluster, Bin b) = 0;
        // calculate the cost of the cluster computed 
        virtual float CalculateCost() = 0; 
        virtual double getInversionCount(vector<item>& cluster) = 0;
        void PrintPackedData()
        {
            std::cout<<"Printing the packing -- \n";
            for(auto &it : packed_items)
            {
                it.print(); std::cout<<"\n";
            }
            std::cout<<"\n";
            return;
        }

        void PrintPackedDataToFile(string file_name)
        {
            std::ofstream out(file_name, std::ios_base::app);
            out<<"Printing the packing -- \n";
            for(auto &it : packed_items)
            {
                it.printToFile(out); out<<"\n";
            }
            out<<"\n";
            return;
        }

        vector<item> GetPackaging(){
            return packed_items;
        }

        float GetPackagingCost(){
            return cost;
        }

    protected:
        vector<item> packed_items;
        float cost;

};

#endif 