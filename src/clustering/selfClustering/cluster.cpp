#include<bits/stdc++.h>
#include "../../../include/clustering/selfClustering/selfClustering.hpp"
using namespace std;

namespace CachingSelfClustering{
    map<vector<item>, vector<vector<item>> > cacheMap;
}

int angularCutMetric = 3;

void createCircle(vector<item>& items) {
    sort(items.begin(), items.end(), [&](item a, item b) {
        return a.polarCoordinate.angle < b.polarCoordinate.angle;
    });
}

// float getDistance(coordinate a, coordinate b) {
//     return a.first*a.first + b.first*b.first -2*a.first*b.first*cos(a.second - b.second);
// }

// int findStartLine(vector<pair<int, int> >& circle) {

//     float largest = 0;
//     int index = -1;
//     int n=circle.size();
//     for(int i=0;i<circle.size();i++) {
//         float temp = circle[(i+1)%n].second - circle[i].second;
//         if(temp > largest) {
//             largest = temp;
//             index = i;
//         }
//     }
//     return (index+1)%n;
// }

vector<vector<item> > createRadialCuts(vector<vector<item> >& angularCuts, Bin deliveryBag) {
    // TODO: Define this max volume and max weight
    float maxVolume = deliveryBag.getVolume(), maxWeight = deliveryBag.getCapacity(); 

    int k = angularCuts.size();
    vector<vector<item> > clusters;
    for(int i=0;i<k;i++) 
    {
        sort(angularCuts[i].begin(), angularCuts[i].end(), [&](item a, item b) {
            return a.polarCoordinate.radius < b.polarCoordinate.radius;
        });
        float totalWeight = 0, totalVolume = 0;
        vector<item> currentCluster;
        currentCluster.push_back(angularCuts[i][0]);
        totalWeight += angularCuts[i][0].weight;
        totalVolume += angularCuts[i][0].size.length * angularCuts[i][0].size.width * angularCuts[i][0].size.height;
        for(int j=1;j<angularCuts[i].size();j++) {
            float tempVolume = totalVolume + angularCuts[i][j].size.length * angularCuts[i][j].size.width * angularCuts[i][j].size.height;
            float tempWeight = totalWeight + angularCuts[i][j].weight; 

            if(tempVolume < maxVolume && tempWeight < maxWeight) {
                currentCluster.push_back(angularCuts[i][j]);
                totalVolume = tempVolume;
                totalWeight = tempWeight;
            }
            else {
                clusters.push_back(currentCluster);
                currentCluster.clear();
                currentCluster.push_back(angularCuts[i][j]);
                totalVolume = angularCuts[i][j].size.length * angularCuts[i][j].size.width * angularCuts[i][j].size.height;
                totalWeight = angularCuts[i][j].weight;
            }
        }
        clusters.push_back(currentCluster);
    }
    return clusters;
}

bool checkClusterValidity(vector<vector<item> >& clusters) {
    return true;
}

vector<vector<item> > createAngularCuts(vector<item>& circle, Bin deliveryBag) {
    vector<vector<item> > cuts;
    int n = circle.size();

    vector<pair<float, int> > angles;
    for(int i=0;i<n-1;i++) {
        angles.push_back(make_pair(circle[(i+1)%n].polarCoordinate.angle-circle[i].polarCoordinate.angle, i));
    }
    angles.push_back(make_pair(2*M_PI - circle[n-1].polarCoordinate.angle+circle[0].polarCoordinate.angle, n-1));

    sort(angles.begin(), angles.end());
    for(int i=0;i<angles.size();i++) {
        cout << angles[i].first << " " << angles[i].second << endl;
    }
    // int numberOfAngularCuts  = (angles.size()+angularCutMetric-1)/angularCutMetric;
    float maxVolume = deliveryBag.getVolume()*SELFCLUSTERING::angularCutMetric, maxWeight = deliveryBag.getCapacity()*angularCutMetric;
    int starting = (angles.back().second+1)%n;
    angles.pop_back();
    vector<item> current;
    float totalVolume = 0, totalWeight = 0;
    for(int i=0;i<n;i++) {
        float tempVolume = totalVolume + circle[(starting+i)%n].getVolume();
        float tempWeight = totalWeight + circle[(starting+i)%n].weight;
        if((starting+i)%n == (angles.back().second+1)%n) {
            cuts.push_back(current);
            cout<<"Cut at: "<<(starting+i)%n<<endl;
            current.clear();
            current.push_back(circle[(starting+i)%n]);
            totalVolume = circle[(starting+i)%n].getVolume();
            totalWeight = circle[(starting+i)%n].weight;
            angles.pop_back();
            continue;
        }
        if(tempVolume < maxVolume && tempWeight < maxWeight) {
            current.push_back(circle[(starting+i)%n]);
            totalVolume = tempVolume;
            totalWeight = tempWeight;
        }
        else {
            cuts.push_back(current);
            cout<<"Cut at: "<<(starting+i)%n<<endl;
            current.clear();
            current.push_back(circle[(starting+i)%n]);
            totalVolume = circle[(starting+i)%n].getVolume();
            totalWeight = circle[(starting+i)%n].weight;
        }
    }
    cuts.push_back(current);

    // vector<vector<item> > cuts;
    
    // for(int i=0;i<points.size();i++) {
    //     vector<item> currentCut;
    //     for(int j=points[i];j<points[(i+1)%points.size()];j++) {
    //         currentCut.push_back(circle[j]);
    //     }
    //     if(points[i] > points[(i+1)%points.size()])
    //     {
    //         for(int j=0;j<n+points[(i+1)%points.size()]-points[i];j++) {
    //             currentCut.push_back(circle[(j+points[i])%n]);
    //         }
    //     }
    //     cuts.push_back(currentCut);
    // }
    // vector<vector<item> > finalClusters = createRadialCuts(cuts);

    // int l=0, r=n, ans=n;
    // while(l<=r){
    //     int mid = (l+r)/2;
    //     vector<vector<item> > cuts;
    //     vector<int> points;
    //     for(int i=n-1;i>n-1-mid;i--) 
    //     {
    //         points.push_back((angles[i].second+1)%n);
    //     }
    //     sort(points.begin(), points.end());
    //     for(int i=0;i<points.size();i++) {
    //         vector<item> currentCut;
    //         for(int j=points[i];j<points[(i+1)%points.size()];j++) {
    //             currentCut.push_back(circle[j]);
    //         }
    //         if(points[i] > points[(i+1)%points.size()])
    //         {
    //             for(int j=0;j<n+points[(i+1)%points.size()]-points[i];j++) {
    //                 currentCut.push_back(circle[(j+points[i])%n]);
    //             }
    //         }
    //         cuts.push_back(currentCut);
    //     }
    //     vector<vector<item> > finalClusters = createRadialCuts(cuts);
        
    //     if(checkClusterValidity(finalClusters)) {
    //         ans=mid;
    //         r=mid-1;
    //     }
    //     else l=mid+1;
    // }

    // vector<vector<item> > cuts;
    // vector<int> points;
    // for(int i=n-1;i>n-1-ans;i--) 
    // {
    //     points.push_back((angles[i].second+1)%n);
    // }
    // sort(points.begin(), points.end());
    // for(int i=0;i<points.size();i++) {
    //     vector<item> currentCut;
    //     for(int j=points[i];j<points[(i+1)%points.size()];j++) {
    //         currentCut.push_back(circle[j]);
    //     }
    //     if(points[i] > points[(i+1)%points.size()])
    //     {
    //         for(int j=0;j<n+points[(i+1)%points.size()]-points[i];j++) {
    //             currentCut.push_back(circle[(j+points[i])%n]);
    //         }
    //     }
    //     cuts.push_back(currentCut);
    // }
    cout<<cuts.size()<<endl;
    return cuts;
}

void assignPolarCoordinate(item& temp, Coordinate warehouse) {
    double x = temp.coordinate.longitude - warehouse.longitude;
    double y = temp.coordinate.latitude - warehouse.latitude;
    temp.polarCoordinate.radius = sqrt((temp.coordinate.longitude-warehouse.longitude)*(temp.coordinate.longitude-warehouse.longitude) + (temp.coordinate.latitude-warehouse.latitude)*(temp.coordinate.latitude-warehouse.latitude));
    temp.polarCoordinate.angle = atan2(temp.coordinate.latitude-warehouse.latitude, temp.coordinate.longitude-warehouse.longitude);
    // if(x<0) temp.polarCoordinate.angle+=M_PI;
    if(y<0) temp.polarCoordinate.angle+=2*M_PI;
    temp.polarCoordinate.angle*=180.0/M_PI;
    cout<<temp.polarCoordinate.angle*180/M_PI<<" "<<x<<" "<<y<<endl;
}

vector<vector<item> > SELFCLUSTERING::calculateCluster(vector<item>& items, Coordinate warehouse, int numberOfRiders, Bin bin) {
    if((CachingSelfClustering::cacheMap).find(items) != (CachingSelfClustering::cacheMap).end()){
		clusters = (CachingSelfClustering::cacheMap)[items];
		return clusters;
	}	
    // vector<item> items;
    for(int i=0;i<items.size();i++) {
        assignPolarCoordinate(items[i], warehouse);
    }
    createCircle(items);


    vector<vector<item> > angularClusters = createAngularCuts(items, bin);
    // return angularClusters;
    vector<vector<item> > clusters = createRadialCuts(angularClusters, bin);
    (CachingSelfClustering::cacheMap)[items] = clusters;		
    return clusters;
}

void printClusters(vector<vector<item> >& clusters, vector<item>& items) {
    vector<int>  clusterIndex(items.size());
    cout<<clusters.size()<<endl;
    for(int i=0;i<clusters.size();i++) {
        for(int j=0;j<clusters[i].size();j++) {
            clusterIndex[clusters[i][j].id] = i;
            // cout<<i<<" ";
        }
    }
    // cout<<endl;
    ofstream myfile;
  	myfile.open ("output.txt");

	for(int i=0;i<clusterIndex.size();i++) {
		myfile<<clusterIndex[i]<<" ";
        // cout<<clusterIndex[i]<<" ";
	}
    // cout<<endl;
	myfile<<endl;
	myfile.close();
}

// int main() {
//     ifstream input;
// 	input.open("input.txt");
// 	Coordinate warehouse;
// 	input>>warehouse.longitude>>warehouse.latitude;
//     warehouse.longitude*=warehouse.scale;
//     warehouse.latitude*=warehouse.scale;
// 	Bin bin;
// 	input>>bin.size.length>>bin.size.width>>bin.size.height;
// 	bin.capacity = 25;
// 	int n;
// 	input>>n;
// 	vector<item> items(n);
// 	for(int i=0;i<n;i++) {
// 		input>>items[i].coordinate.longitude>>items[i].coordinate.latitude;
//         items[i].coordinate.longitude*=items[i].scale;
//         items[i].coordinate.latitude*=items[i].scale;
// 		input>>items[i].size.length>>items[i].size.width>>items[i].size.height;
// 		items[i].weight = 1;
//         items[i].id = i;
// 	}

//     vector<vector<item> > clusters = calculateCluster(items, warehouse, 1, bin);
    
//     printClusters(clusters, items);

//     return 0;
// }