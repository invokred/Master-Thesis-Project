#include <iostream>
#include <cstdlib>
#include <algorithm>
#include <cmath>
#include "../../include/routeplan/TSP_LK.hpp"
using namespace std;

const int N = 2000; // maximum number of cities
int num;                // number of cities
double dist[N][N];    // distance between cities
int tour[N];          // current tour
int best_tour[N];     // best tour found so far
double best_distance; // length of the best tour found so far

double TSP_LK::tour_distance(int tour[])
{
    double d = 0;
    for (int i = 0; i < num; i++)
        d += dist[tour[i]][tour[i + 1]];
    return d;
}

void TSP_LK::two_opt(int tour[], int i, int j)
{
    reverse(tour + i, tour + j + 1);
}

void TSP_LK::three_opt(int tour[], int i, int j, int k)
{
    int tmp[N];
    for (int p = 0; p < i; p++)
        tmp[p] = tour[p];
    int idx = i;
    for (int p = i; p <= k; p++)
        tmp[idx++] = tour[p];
    for (int p = j; p > k; p--)
        tmp[idx++] = tour[p];
    for (int p = j + 1; p < num; p++)
        tmp[idx++] = tour[p];
    for (int p = 0; p < num; p++)
        tour[p] = tmp[p];
}

void TSP_LK::lin_kernighan()
{
    // Initialize the current tour as a random permutation of the cities
    for (int i = 0; i < num; i++)
        tour[i] = i;
    random_shuffle(tour, tour + num);
    best_distance = tour_distance(tour);
    copy(tour, tour + num, best_tour);
    // Iterate until no further improvements can be made
    while (true)
    {
        bool improved = false;
        // Iterate over all possible 2-opt and 3-opt moves
        for (int i = 0; i < num - 2; i++)
            for (int j = i + 2; j < num; j++)
                for (int k = i + 1; k < j; k++)
                {
                    two_opt(tour, i, j);
                    double new_distance = tour_distance(tour);
                    if (new_distance < best_distance)
                    {
                        best_distance = new_distance;
                        copy(tour, tour + num, best_tour);
                        improved = true;
                        break;
                    }
                    three_opt(tour, i, j, k);
                    new_distance = tour_distance(tour);
                    if (new_distance < best_distance)
                    {
                        best_distance = new_distance;
                        copy(tour, tour + num, best_tour);
                        improved = true;
                        break;
                    }
                }
        if (!improved)
            break;
    }
}


void TSP_LK::PlanRoute(vector<item>& cluster, Coordinate warehouse)
{
    ComputeDistMatrix(cluster, warehouse);
    vector<item> mod_cluster;
    mod_cluster.push_back(item(10, 10, 10, warehouse.latitude, warehouse.longitude));
    for(auto x: cluster)
    {
        mod_cluster.push_back(x);
    }
    num = mod_cluster.size();
    for(int i=0;i<num;i++)
    {
        for(int j=0;j<num;j++)
        {
        //     dist[i][j] = static_cast<int>(
        //   Dist(mod_cluster[i].coordinate, mod_cluster[j].coordinate));
            dist[i][j] = static_cast<int>(this->DistMatrix[i][j]);
        }
    }

    auto start = clock();
    lin_kernighan();

    cout << "Best Tour: ";
    for (int i = 0; i < num; i++)
        cout << best_tour[i] << " ";
    cout << best_tour[0] << endl;

    cout.setf(ios::fixed);
    cout.precision(5);
    best_distance = best_distance;
    cout << "Best Distance: " << best_distance << endl;

    cout << "Time: " << (clock() - (double)start) / (double)CLOCKS_PER_SEC << endl;
    cost = best_distance;
}

void TSP_LK::CalculateCost(){

}
// int main()
// {
//     cin >> num;
//     for (int i = 0; i < num; i++)
//         for (int j = 0; j < num; j++)
//             cin >> dist[i][j];

//     auto start = clock();
//     lin_kernighan();

//     cout << "Best Tour: ";
//     for (int i = 0; i < num; i++)
//         cout << best_tour[i] << " ";
//     cout << best_tour[0] << endl;

//     cout.setf(ios::fixed);
//     cout.precision(5);
//     cout << "Best Distance: " << best_distance << endl;

//     cout << "Time: " << (clock() - (double)start) / (double)CLOCKS_PER_SEC << endl;
//     return 0;
// }