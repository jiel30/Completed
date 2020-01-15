#include <string>
#include <iostream>
#include <fstream>
#include "array_queue.hpp"
#include "hash_graph.hpp"
#include "dijkstra.hpp"



std::string get_node_in_graph(const ics::DistGraph& g, std::string prompt, bool allow_QUIT) {
  std::string node;
  for(;;) {
    node = ics::prompt_string(prompt + " (must be in graph" + (allow_QUIT ? " or QUIT" : "") + ")");
    if ( (allow_QUIT && node == "QUIT") || g.has_node(node) )
      break;
  }
  return node;
}


int main() {
    ics::HashGraph<int> graph;
    std::ifstream file;
  try {
      ics::safe_open(file,"Enter the file name","flightcost.txt");


  } catch (ics::IcsError& e) {
    std::cout << e.what() << std::endl;
  }
  graph.load(file,";");
//  file.close();
  std::cout << graph << std::endl;
  std::string start = ics::prompt_string("Enter start node (must be in graph) ");
  auto path = extended_dijkstra(graph,start);
  std::cout << path << std::endl;
  while(true)
  {
      std::string end = ics::prompt_string("Enter stop node (must be in graph or QUIT) ");
      if(end == "QUIT")
      {
          break;
      }
      auto p = recover_path(path,end);
      std::cout << "Cost is " << path[end].cost << " ;path is " << p << std::endl;
  }

  return 0;
}
