#ifndef DIJKSTRA_HPP_
#define DIJKSTRA_HPP_

#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <limits>                    //Biggest int: std::numeric_limits<int>::max()
#include "array_queue.hpp"
#include "array_stack.hpp"
#include "heap_adjustable_hash_priority_queue.hpp"
#include "hash_graph.hpp"


namespace ics {
    typedef std::string                NodeName;
    typedef pair<NodeName, NodeName>   Edge;
    static int hash_str(const std::string& s) {
        std::hash<std::string> str_hash;
        return str_hash(s);
    }
    typedef HashOpenSet<std::string, hash_str> NodeSet;
    class Info {
    public:
        Info() { }

        Info(std::string a_node) : node(a_node) { }

        bool operator==(const Info &rhs) const { return node == rhs.node && cost == rhs.cost && from == rhs.from; }

        bool operator!=(const Info &rhs) const { return !(*this == rhs); }

        friend std::ostream &operator<<(std::ostream &outs, const Info &i) {
            outs << "Info[" << i.node << "," << i.cost << "," << i.from << "]";
            return outs;
        }

        //Public instance variable definitions
        std::string node = "?";
        int cost = std::numeric_limits<int>::max();
        std::string from = "?";
    };


    bool gt_info(const Info &a, const Info &b) { return a.cost < b.cost; }
    int hash_info(const Info & a) {return -1 * a.cost; };
    //Put needed hashing functions here

    typedef ics::HashGraph<int>                                               DistGraph;
    typedef ics::HeapAdjustableHashPriorityQueue<Info, gt_info,hash_info>     CostPQ;
    typedef ics::HashOpenMap<std::string, Info, DistGraph::hash_str>          CostMap;
    typedef ics::pair<std::string, Info>                                      CostMapEntry;

//Return the final_map as specified in the lecture-note description of
//  extended Dijkstra algorithm
    std::string find_last(ArrayQueue<std::string> a)
    {
        std::string final;
        while(!a.empty())
        {
            final = a.dequeue();
        }
        return final;
    }
    std::string find_shortest(std::string i,int length,const DistGraph &b,ArrayQueue<std::string>nodes,CostMap& c)
    {
        int smaller = std::numeric_limits<int>::max();
        std::string min = "?";
        for(auto aa:(b.all_nodes()[i].out_nodes))
        {
            Edge e(i,aa);
            int len = length + b.all_edges()[e];
            if(len < c[aa].cost)
            {
                c[aa].cost = len;
                c[aa].from = i;
            }
        }
        for(auto aa:c)
        {
            if(aa.second.cost < smaller)
            {
                bool determine = true;
                for(auto bb : nodes)
                {
                    if(bb == aa.first){
                        determine = false;
                        break;
                    }
                }
                if(determine) {
                    smaller = aa.second.cost;
                    min = aa.first;
                }
            }
        }
        return min;
    }
    CostMap extended_dijkstra(const DistGraph &g, std::string start_node) {
        CostMap answer_map;
        ArrayQueue<std::string> nodes;
        for(auto aa : g.all_nodes())
        {
            if(aa.first == start_node)
            {
                Info temp;
                temp.node = aa.first;
                temp.cost = 0;
                answer_map.put(aa.first,temp);
                nodes.enqueue(aa.first);
            }
            else
            {
                if(aa.second.in_nodes.size() != 0) {
                    Info temp;
                    temp.node = aa.first;
                    answer_map.put(aa.first,temp);
                }
            }
        }
//        HashOpenMap <std::string,int> out_nodes;
//        bool b = true;
//        bool determine = true;
        while(answer_map.size() != nodes.size()) {
//            answer_map.size() != nodes.size()
            std::string check = find_last(nodes);
            std::string next = find_shortest(check, answer_map[check].cost, g, nodes, answer_map);
            for (auto aa : nodes) {
                if (aa == next) {}
            }
            nodes.enqueue(next);
        }
        return answer_map;
    }


//Return a queue whose front is the start node (implicit in answer_map) and whose
//  rear is the end node
    ArrayQueue <std::string> recover_path(const CostMap &answer_map, std::string end_node) {
        ArrayQueue<std::string> final;
        std::string current = end_node;
        final.enqueue(current);
        while(true)
        {
            std::string prev = answer_map[current].from;
            if(prev == "?")
            {
                break;
            }
            final.enqueue(prev);
            current = prev;
        }
        return final;
    }


}

#endif /* DIJKSTRA_HPP_ */
