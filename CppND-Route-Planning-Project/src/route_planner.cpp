#include "route_planner.h"
#include <algorithm>
#include <limits>

RoutePlanner::RoutePlanner(RouteModel &model, float start_x, float start_y, float end_x, float end_y): m_Model(model) {
    // Convert inputs to percentage:
    start_x *= 0.01;
    start_y *= 0.01;
    end_x *= 0.01;
    end_y *= 0.01;

    // Store the start and end coordinates in this class's start and end node values
    RouteModel m_Model(model); 
    //std::cout << "Searching for closest map nodes...\n";
    start_node = &m_Model.FindClosestNode(start_x, start_y);
    end_node   = &m_Model.FindClosestNode(end_x, end_y);
    //std::cout << "Closest map nodea found.\n";
    
}


// Done 12/19
// TODO 3: Implement the CalculateHValue method.
// Tips:
// - You can use the distance tothe end_node for the h value.
// - Node objects have a distance method to determine the distance to another node.

float RoutePlanner::CalculateHValue(RouteModel::Node const *node) {
  float h_distance = 0.0;
  h_distance = node->distance(*end_node);
  return h_distance;
}


// TODO 4: Complete the AddNeighbors method to expand the current node by adding all unvisited neighbors to the open list.
// Tips:
// - Use the FindNeighbors() method of the current_node to populate current_node.neighbors vector with all the neighbors.
// - For each node in current_node.neighbors, set the parent, the h_value, the g_value. 
// - Use CalculateHValue below to implement the h-Value calculation.
// - For each node in current_node.neighbors, add the neighbor to open_list and set the node's visited attribute to true.

void RoutePlanner::AddNeighbors(RouteModel::Node *current_node) {
  current_node->FindNeighbors();
  float g_value_neighbor = 0.0;
  for (int i = 0; i<current_node->neighbors.size(); i++) {
    g_value_neighbor = current_node->distance(*current_node->neighbors[i]) + current_node->g_value;
    current_node->neighbors[i]->h_value = CalculateHValue(current_node->neighbors[i]);
    current_node->neighbors[i]->g_value = g_value_neighbor;
    current_node->neighbors[i]->parent = current_node;
    current_node->neighbors[i]->visited = true;
    open_list.push_back(current_node->neighbors[i]);
  }
}

// TODO 5: Complete the NextNode method to sort the open list and return the next node.
// Tips:
// - Sort the open_list according to the sum of the h value and g value.
// - Create a pointer to the node in the list with the lowest sum.
// - Remove that node from the open_list.
// - Return the pointer.

RouteModel::Node *RoutePlanner::NextNode() {
  int short_path_idx = 0;
  float f_min = std::numeric_limits<float>::max();
  float f = 0.0;
  for (int i = 0; i < open_list.size(); i++) {
    f = open_list[i]->g_value + open_list[i]->h_value;
    //std::cout << "Open List #" << i << ": " << open_list[i] << " " << f <<"\n";
    if (f <= f_min) {
      f_min = f;
      short_path_idx = i;
    }
  }
  RouteModel::Node* short_path_node = open_list[short_path_idx];
  open_list.erase(open_list.begin() + short_path_idx);
  //std::cout << "Shortest Path node is: " << short_path_node << " idx: " << short_path_idx << "\n";
  return short_path_node;
}

// TODO 6: Complete the ConstructFinalPath method to return the final path found from your A* search.
// Tips:
// - This method should take the current (final) node as an argument and iteratively follow the 
//   chain of parents of nodes until the starting node is found.
// - For each node in the chain, add the distance from the node to its parent to the distance variable.
// - The returned vector should be in the correct order: the start node should be the first element
//   of the vector, the end node should be the last element.

std::vector<RouteModel::Node> RoutePlanner::ConstructFinalPath(RouteModel::Node *current_node) {
    // Create path_found vector
    distance = 0.0f;
    std::vector<RouteModel::Node> path_found;
    path_found.push_back(*current_node);
    while(path_found.back().parent != nullptr) {
      distance += path_found.back().distance(*path_found.back().parent);
      path_found.push_back(*path_found.back().parent);
    }
    // reverse the vector of nodes
    std::reverse(path_found.begin(), path_found.end());
    distance *= m_Model.MetricScale(); // Multiply the distance by the scale of the map to get meters.
    std::cout << "Total path distance set is: " << distance << " meters\n";
    return path_found;
}


// TODO 7: Write the A* Search algorithm here.
// Tips:
// - Use the AddNeighbors method to add all of the neighbors of the current node to the open_list.
// - Use the NextNode() method to sort the open_list and return the next node.
// - When the search has reached the end_node, use the ConstructFinalPath method to return the final path that was found.
// - Store the final path in the m_Model.path attribute before the method exits. This path will then be displayed on the map tile.

void RoutePlanner::AStarSearch() {
    RouteModel::Node *current_node = nullptr;
    //RouteModel::Node *last_node = nullptr;
    //RouteModel::Node *next_node = nullptr;
    start_node->h_value = CalculateHValue(start_node);
    start_node->g_value = 0.0;
    start_node->visited = true;
    open_list.push_back(start_node);
    current_node = start_node;
    //AddNeighbors(current_node);
    end_node->h_value = CalculateHValue(end_node);
    //int count = 0;
    //float alt_f_value = 0.0;
    //float alt_g_value = 0.0;
    //float min_f_value = 0.0;
    //int   min_f_index = 0;
    while(current_node->h_value != 0.0) {
      current_node = NextNode();
      AddNeighbors(current_node);
      //last_node = current_node;
      // cut corners, literally
      //if (last_node->parent != nullptr) {
      //  for (int i = 0; i < last_node->parent->neighbors.size(); i++) {
      //    if (current_node == last_node->parent->neighbors[i]) {
      //      std::cout << "corner cut\n";
      //      current_node = last_node->parent->neighbors[i];
      //      current_node->g_value = current_node->parent->g_value + current_node->distance(*current_node->parent);
      //    }
      //  }
      // }
      // recalculate the open list g values
      //for (int i = 0; i < open_list.size(); i++) {
      //  open_list[i]->g_value = current_node->g_value + current_node->distance(*open_list[i]);
      //}
      
      // reset the open list
      //for (int i = 0; i < open_list.size(); i++) {
      //  open_list[i]->visited = false;
      //}
      //open_list.erase(open_list.begin(), open_list.end());
      //count += 1;
      //std::cout << "move count: " << count << "..............\n";
      //std::cout << "open list size" << open_list.size() << "\n";
      //std::cout << "current h_value: " << current_node->h_value << "\n";
      //std::cout << "current g_value: " << current_node->g_value << "\n";
    }
    m_Model.path = ConstructFinalPath(current_node);
}