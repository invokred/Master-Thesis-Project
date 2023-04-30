// #include <iostream>
// #include <vector>
// #include <algorithm>
// #include "../../include/clustering/TimeWindow/TimeWindow.hpp"
// struct Package {
//     int id;
//     double x, y; // coordinates of the package
//     int delivery_time_window; // time window for delivery
// };
 
// // similarity function between two packages
// double similarity(const Package &p1, const Package &p2, double alpha) {
//     double spatial_distance = -sqrt((p1.x - p2.x) * (p1.x - p2.x) + (p1.y - p2.y) * (p1.y - p2.y));
//     double temporal_distance = abs(p1.delivery_time_window - p2.delivery_time_window);
//     return alpha * spatial_distance + (1 - alpha) * temporal_distance;
// }
 
// std::vector<std::vector<Package>> TimeWindowAffinityPropagation(std::vector<Package> packages, double alpha) {
//     int n = packages.size();
 
//     // create similarity matrix
//     std::vector<std::vector<double>> sim(n, std::vector<double>(n));
//     for (int i = 0; i < n; i++) {
//         for (int j = 0; j < n; j++) {
//             sim[i][j] = similarity(packages[i], packages[j], alpha);
//         }
//     }
 
//     // initialize cluster assignments
//     std::vector<int> cluster(n);
//     for (int i = 0; i < n; i++) {
//         cluster[i] = i;
//     }
 
//     // iterate until convergence
//     while (true) {
//         // update cluster assignments
//         std::vector<int> new_cluster = cluster;
//         for (int i = 0; i < n; i++) {
//             int max_j = -1;
//             double max_val = -1e9;
//             for (int j = 0; j < n; j++) {
//                 if (i == j) continue;
//                 double val = sim[i][j] - sim[i][cluster[i]];
//                 if (val > max_val) {
//                     max_val = val;
//                     max_j = j;
//                 }
//             }
//             new_cluster[i] = max_j;
//         }
 
//         // check for convergence
//         if (new_cluster == cluster) {
//             break;
//         }
//         cluster = new_cluster;
//     }
 
//     // create output clusters
//     std::vector<std::vector<Package>> clusters;
//     std::vector<bool> visited(n);
//     for (int i = 0; i < n; i++) {
//         if (visited[i]) continue;
//         int j = i;
//         std::vector<Package> c;
//         while (!visited[j]) {
//             visited[j] = true;
//             c.push_back(packages[j]);
//             j = cluster[j];
//         }
//         clusters.push_back(c);
//     }
 
//     return clusters;
// }
 
// int main() {
//     // create sample input
//     std::vector<Package> packages = {
//         {1, 1, 1, 10},
//         {2, 2, 2, 20},
//         {3, 3, 3, 30},
//         {4, 4, 4, 40},
//         {5, 5, 5, 50},
//     };
 
//     double alpha = 0.5; // weighting factor
 
//     // call the function
//     std::vector<std::vector<Package>> clusters = TimeWindowAffinityPropagation(packages, alpha);
 
//     // print the clusters
//     for (int i = 0; i < clusters.size(); i++) {
//         std::cout << "Cluster " << i + 1 << ": ";
//         for (int j = 0; j < clusters[i].size(); j++) {
//             std::cout << clusters[i][j].id << " ";
//         }
//         std::cout << std::endl;
//     }
 
//     return 0;
// }
//  