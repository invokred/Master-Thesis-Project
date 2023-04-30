// #include "datastructures.hpp"
// #include "../../include/datastructures.hpp"
#ifndef __CLARKE__HEADER__
#define __CLARKE__HEADER__
#include "../../../include/interface/ClusteringInterface.hpp"
using namespace std;


#include<iostream>
#include<cmath>
#include<vector>
#include<map>
#include<queue>
using namespace std;
const double R = 6371;

class Cluster{
    public:
        int p1,p2;
        double weight;
        double volume;
        double distance;
        int rank;
    Cluster();
};

class Clarke: public ClusteringInterface{
    public:
    Clarke(DistanceType method) : ClusteringInterface(method){ clustering_method = 1; multithreading = 1;}
    vector<item> packages; Coordinate warehouse; int numPackages;int numRiders; Bin b;double distThresh;
    vector<Cluster> Clusters;
    vector<Cluster> final_Clusters;
    vector<int> parent;
    priority_queue<pair<double,pair<int,int>>> q;
    double depotDist(Coordinate&);
    // void Clarke::setParams(vector<item> packages, Coordinate warehouse, int numRiders, Bin b);
    void solve();
    public:
    void ComputeClusters(vector<item>& packages, Coordinate warehouse, int numRiders, Bin b);
    double compute_savings(item&,item&);
    void create_pq();
    void make_set(int);
    int find_set(int);
    void test();
    void consolodate_further();
    bool cmp(const pair<int,int> ,const pair<int,int> );
    void merge_sort(vector<pair<int,int>> , int  , int );
    Cluster initCluster(int);
    int union_sets(int a,int b, bool);
    void CalculateCost();
};
#endif