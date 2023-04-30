#ifndef __CLUSTERING_FESIF__
#define __CLUSTERING_FESIF__
#include <bits/stdc++.h>
#include "../../interface/ClusteringInterface.hpp"
#include "global.h"
#include "HST.h"
#include "utils.h"


using namespace std;

class FESIF : public ClusteringInterface {

    private:
        int nP;
        int* reqPool;
        int* reqPos;
        const double step;
        double rho, delta;

        void localInit();
        void localFree();
        void budget(int wid, vector<int>& Rw, vector<int>& Sw, double delta);
        void updateRoute(int wid, vector<int>& Rw, vector<int>& Sw);
        vector<int> getRequest(int wid);
        void FESI();
        vector<vector<int> > createResult();
        vector<vector<item> > wrapperLMD(vector<item>& items, Coordinate warehouse, int numberOfRiders, Bin bin);

    public:
        FESIF(DistanceType method): nP(0), step(2.0), rho(1), ClusteringInterface(method){
            delta = rho;
        }
        void ComputeClusters(vector<item>& packages, Coordinate warehouse, int numRiders, Bin b) {
            setClusters(wrapperLMD(packages, warehouse, numRiders, b));
        }
        void CalculateCost(){
            cost = 0;
        }
};

#endif  