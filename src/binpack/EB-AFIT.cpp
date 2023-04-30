//**********************
// INCLUDED HEADER FILES
//**********************

#include <time.h>

#include "../../include/binpack/EB_AFIT.hpp"
#include "../../include/datastructures.hpp"

using namespace std;

void sortCluster(vector<item>& cluster) {
    sort(cluster.begin(), cluster.end(), [](const item& a, const item& b) {
        return a.orig_rank < b.orig_rank;
    });
}

double EB_AFIT::getInversionCount(vector<item>& cluster) {
    /**
    * 
    * @brief Calculates the inversion count of a given 3D cluster of items
    * 
    * @param cluster - vector of item objects to calculate inversion count for
    * 
    * @return double - inversion count of the cluster, as a fraction of maximum number of inversions possible for n items
    * 
    */
    sortCluster(cluster);
    int num_cnt = 0;
    for (int i = 0; i < cluster.size(); i++) {
        for (int j = i + 1; j < cluster.size(); j++) {
            if (cluster[i].position.y < cluster[j].position.y) {
                if (cluster[i].position.x < cluster[j].position.x + cluster[j].size.length ||
                    cluster[i].position.x + cluster[i].size.length > cluster[j].position.x) {
                    if (cluster[i].position.z < cluster[j].position.z + cluster[j].size.height ||
                        cluster[i].position.z + cluster[i].size.height > cluster[j].position.z) {
                        num_cnt++;
                    }
                }
            }
        }
    }
    int n = cluster.size();
    double ans = 1 - (double)num_cnt / ((n * (n - 1)) / 2);
    cout << "Number Inversions = "<<num_cnt << endl;
    return ans;
}

time_t start, finish;

EB_AFIT::EB_AFIT() {
  /**
   * @brief Constructor of the class EB_AFIT
   * 
   * @param None
   * 
   * @return None
  */
}

void EB_AFIT::BinPack(vector<item>& cluster, Bin b) {
    /**
     * @brief Binpacks the cluster into the bin using the EB-AFIT algorithm
     *
     * @param  cluster - The cluster which has to be packed into the bin
     * @param  b - The bin into which the cluster has to be packed
     * 
     * @return None
     */
    time(&start);
    binpacker.n = cluster.size();
    binpacker.totalboxvol = 0;
    for (auto& it : cluster) {
        it.volume = it.size.length * it.size.width * it.size.height;
        binpacker.totalboxvol += it.volume;
    }
    binpacker.temp = 1;
    binpacker.totalvolume = binpacker.temp * b.size.length * b.size.width * b.size.height;
    binpacker.scrapfirst = new scrappad;
    for (int i = 0; i < binpacker.n; i++) {
        binpacker.boxStatus[i] = 0;
    }
    

    binpacker.bestvolume = 0;
    binpacker.packingbest = 0;
    binpacker.hundredpercent = 0;
    binpacker.itenum = 0;
    binpacker.px = b.size.length;
    binpacker.py = b.size.height;
    binpacker.pz = b.size.width;

    binpacker.remainpy = binpacker.py;
    binpacker.remainpz = binpacker.pz;
    binpacker.remainpx = binpacker.px;
    for (int i = 0; i < cluster.size(); i++) {
        cluster[i].orig_rank = i;
    }
    if (binpacker.scrapfirst == NULL) {
        printf("Insufficient memory available\n");
        exit(1);
    }
    int index = binpacker.n - 1;

    while(index>=0){
      int l = 0;
      int r = index;
      int ans = -1;
      Binpacker ansVar;
      while(l<=r){
        int mid = (l + r)/2;

        vector<item> temp;
        for(int i=mid;i<=index;i++){
          temp.push_back(cluster[i]);
        }

        Binpacker tempPacker(binpacker, temp.size());
        tempPacker.execiterations(temp);
        int allPacked = 1;
        for(int i=0;i<temp.size();i++){
          if(tempPacker.boxStatus[i]==0){
            allPacked = 0;
          }
        }

        if(allPacked){
          ans = mid;
          ansVar = tempPacker;
          tempPacker.scrapmemb = tempPacker.scrapfirst;
          r = mid-1;
          
          for(int i=ans;i<=index;i++){
            cluster[i] = temp[i-ans];
          }
        }
        else{
          l = mid+1;
        }
      }

      if(ans==-1){
        index--;
      }
      else{
        index = ans - 1;
        binpacker.remainpy = ansVar.remainpy;
        binpacker.remainpx = ansVar.remainpx;
        binpacker.remainpz = ansVar.remainpz;

        binpacker.packedy = ansVar.packedy;
        binpacker.prepackedy = ansVar.prepackedy;
        binpacker.preremainpy = ansVar.preremainpy;

        binpacker.scrapfirst = ansVar.scrapfirst;

        binpacker.boxx = ansVar.boxx;
        binpacker.boxy = ansVar.boxy;
        binpacker.boxz = ansVar.boxz;

        binpacker.bboxx = ansVar.bboxx;
        binpacker.bboxy = ansVar.bboxy;
        binpacker.bboxz = ansVar.bboxz;

        binpacker.cboxx = ansVar.cboxx;
        binpacker.cboxy = ansVar.cboxy;
        binpacker.cboxz = ansVar.cboxz;

        binpacker.bfx = ansVar.bfx;
        binpacker.bfy = ansVar.bfy;
        binpacker.bfz = ansVar.bfz;

        binpacker.bbfx = ansVar.bbfx;
        binpacker.bbfy = ansVar.bbfy;
        binpacker.bbfz = ansVar.bbfz;

        binpacker.px = ansVar.px;
        binpacker.py = ansVar.py;
        binpacker.pz = ansVar.pz;
      }
    }
    
    time(&finish);
    binpacker.report(cluster);
    this->packed_items = cluster;
    sortCluster(this->packed_items);
    return;
}

float EB_AFIT::CalculateCost() {
    /**
     * @brief Calculates the cost of the binpacking
     *
     * @param  None
     * 
     * @return The cost of the binpacking
     */
    cost = 100 - binpacker.percentagepackedbox;
    return cost;
}