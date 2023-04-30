#include "../../include/routeplan/TSP_OR.hpp"


void TSP_OR::ComputeEuclideanDistanceMatrix(std::vector<item>& cluster)
{
    distances.clear();
    distances.assign(cluster.size()+1, std::vector<int64_t>(cluster.size()+1, int64_t{0}));

    std::vector<item> mod_cluster;
    mod_cluster.push_back(item(1, 1, 1, warehouse.latitude, warehouse.longitude));
    for(auto x: cluster)
    {
        mod_cluster.push_back(x);
    }

    for (int fromNode = 0; fromNode < mod_cluster.size(); fromNode++) {
    for (int toNode = 0; toNode < mod_cluster.size(); toNode++) {
      if (fromNode != toNode)
        distances[fromNode][toNode] = static_cast<int64_t>(
          Dist(mod_cluster[fromNode].coordinate, mod_cluster[toNode].coordinate)*scale);
    }
  }
}

void TSP_OR::PlanRoute(vector<item> &cluster, Coordinate w){
   
    cout<<"Planning Route..."<<endl;
    plannedPath.clear();
    if(cluster.size() == 1){
      plannedPath.push_back(cluster[0]);
      cost = 2 * Dist(w, cluster[0].coordinate);
      std::cout << "Route distance: " << cost<< "km";
      std::cout << "  "<<endl;
      return;
    }
    ComputeDistMatrix(cluster, w);
    cout << "Dist Matrix Computed Successfully" << endl;
    RoutingIndexManager manager(cluster.size()+1, num_vehicles, depot);
    RoutingModel routing(manager);
    std::cout<<"Warehouse : "<<w.latitude<<" "<<w.longitude<<endl;
    warehouse = w;
    // ComputeEuclideanDistanceMatrix(cluster);
    // const int transit_callback_index = routing.RegisterTransitCallback(
    //   [this, &manager](int64_t from_index,
    //                                int64_t to_index) -> int64_t {
    //     // Convert from routing variable Index to distance matrix NodeIndex.
    //     auto from_node = manager.IndexToNode(from_index).value();
    //     auto to_node = manager.IndexToNode(to_index).value();
    //     return this->distances[from_node][to_node];
    //   });

    const int transit_callback_index = routing.RegisterTransitCallback(
      [this, &manager](int64_t from_index,
                                   int64_t to_index) -> int64_t {
        // Convert from routing variable Index to distance matrix NodeIndex.
        auto from_node = manager.IndexToNode(from_index).value();
        auto to_node = manager.IndexToNode(to_index).value();
        return static_cast<int64_t>(this->DistMatrix[from_node][to_node]*scale);
      });

    routing.SetArcCostEvaluatorOfAllVehicles(transit_callback_index);

    RoutingSearchParameters search_parameters = DefaultRoutingSearchParameters();
    search_parameters.set_first_solution_strategy(
      FirstSolutionStrategy::PATH_CHEAPEST_ARC);
    search_parameters.set_local_search_metaheuristic(
      LocalSearchMetaheuristic::GUIDED_LOCAL_SEARCH);
    search_parameters.mutable_time_limit()->set_seconds(10);
    routing.CloseModelWithParameters(search_parameters);

    vector<vector<int64_t>> initial_routes(1);
    for (int i = 0; i < cluster.size(); i++) initial_routes[0].push_back(i + 1);
    const Assignment* initial_solution =
      routing.ReadAssignmentFromRoutes(initial_routes, true);
    const Assignment* solution = routing.SolveFromAssignmentWithParameters(
      initial_solution, search_parameters);
    cout << "Route Planned Successfully" << endl;
    savePath(cluster, manager, routing, *solution);
}

void TSP_OR::savePath(vector<item>&clusters ,const RoutingIndexManager& manager,
                   const RoutingModel& routing, const Assignment& solution)
{
    int64_t index = routing.Start(0);
    int64_t distance{0};
    std::vector<item> mod_cluster;
    mod_cluster.push_back(item(1, 1, 1, warehouse.latitude, warehouse.longitude));
    for(auto x: clusters)
    {
        mod_cluster.push_back(x);
    }
    // std::cout<<"Debugggggg \n";
    while (routing.IsEnd(index) == false) {
    // std::cout<<mod_cluster[manager.IndexToNode(index).value()].coordinate.latitude << " " << mod_cluster[manager.IndexToNode(index).value()].coordinate.longitude ;
    if(manager.IndexToNode(index).value() !=0)
      plannedPath.push_back(mod_cluster[manager.IndexToNode(index).value()]);
    int64_t previous_index = index;
    index = solution.Value(routing.NextVar(index));
    distance += routing.GetArcCostForVehicle(previous_index, index, int64_t{0});
  }
  double tot_dist = (double)distance ;
  tot_dist /= scale;
  cost = tot_dist;
  std::cout << "Route distance: " << tot_dist<< "km";
  std::cout << "  ";
  std::cout << "Problem solved in " << routing.solver()->wall_time() << "ms" << std::endl;
}

void TSP_OR::CalculateCost(){
    ;
}