// this is the ONLY file you should edit and submit to D2L

#include "deadlock_detector.h"
#include "common.h"
#include <iostream>
#include <string>
#include <vector>
using namespace std;


//Global Variables
int node;
int n;
string Process = "_p";
string Resource = "_r";
const string Request = "->";
const string Assign = "<-";
//int N;
int the_counter;
//
unordered_map<int, string> hash_map;
vector<string> push;

//The graph class to speed up program 
class Graph{
    //class field/member variables 
    
    public:
        vector<vector<int>> adj_list;
        vector<int> outCount;  
        
} the_graph;

//Graph function 
    void Graph_f(bool inp, int p1, int r1){

        //push to the back of the adjacency list and outCount for our resource
        int res = (int) the_graph.adj_list.size();
        //we need to handle a request edge
        while(res == r1)
        {
            the_graph.outCount.push_back(0);
            the_graph.adj_list.push_back({});
            //we then increment the node 
            node += 1;
            break;              
        }
        //push to the back of the adjacency list and outCount for our process
        int the_process = int (the_graph.adj_list.size());
        while ( the_process == p1){
            the_graph.adj_list.push_back({});
            the_graph.outCount.push_back(0);
            //we then increment the node 
            node += 1;
            break;
        }
        // Call the graph function based the input flag to convert strings in the request or assign
        if (inp == true){
                the_graph.adj_list[p1].push_back(r1);
                the_graph.outCount[r1]++;
        } else{
                the_graph.adj_list[r1].push_back(p1);
                the_graph.outCount[p1]++;    
        }
    }

//We put the topological sort into a class for better performance 
class Sort {
    //counter for cycle detection, not public used only in sort 
    int cycle_counter;
    public:
//topological sort function within the graph class for inheritance to speed up program
int topo_sort(){
        vector<int> the_out = the_graph.outCount;
        vector<int> zeros;
        //intialize our cycle counter to zero within the function 
        cycle_counter = 0;

        int i = 0;
        //loop through the out 
        while (i < int(the_out.size())){
            // Check if node is zero
            if(the_out[i] == 0){
                //add this to the back of the vector not the front to speed up
                zeros.push_back(i); 
            }
            i++;
        }    

        //Topological sort to check if size is empty, set our global variable to  remove one entry from zeros[]
        while (!zeros.empty()){   
            n = zeros.back();       
            zeros.pop_back();

            //in every N for the graph adjacency list we need to append n2 to zeros
            for (int n2: the_graph.adj_list[n]){
                the_out[n2] --;
                
                if (the_out[n2] == 0){
                    zeros.push_back(n2); 
                }
            }
            cycle_counter += 1;
        }
        // Keep pushing to the back of the vector to speed up while the cycle counter is less than node
        if (cycle_counter<node) {
            
            //We want to insert into our unordered map every element for the length of the out for our string 
            for (int i =0; i < int(the_out.size()); i++) {
                
                string node = hash_map.at(i);
                
                //We need to make sure that the end of the node has "p" and that the vector is greater than 0 
                if ( (node.back() == 'p')&& (the_out[i] > 0)) {
                    //here we want to get the substring then push back to the node
                    node = node.substr(0, node.size() -2);    
                    push.push_back(node); 
                }                
            
            }
            return 1;
        } return 0;
    }
};



/*void check_r(){
    if (sign == Request) {
            tasks[0].append(Process);
            tasks[2].append(Resource);
            Graph_f(false, convertString.get(tasks[0]), convertString.get(tasks[2]));
        } 
}*/

/*void check_s(){
        if (sign == Assign) {
            tasks[0].append(Process);
            tasks[2].append(Resource);
            Graph_f(true, convertString.get(tasks[0]), convertString.get(tasks[2]));
        }
}*/

/// this is the function you need to (re)implement
///
/// parameter edges[] contains a list of request- and assignment- edges
///   example of a request edge, process "p1" resource "r1"
///     "p1 -> r1"
///   example of an assignment edge, process "XYz" resource "XYz"
///     "XYz <- XYz"
///
/// You need to process edges[] one edge at a time, and run a deadlock
/// detection after each edge. As soon as you detect a deadlock, your function
/// needs to stop processing edges and return an instance of Result structure
/// with edge_index set to the index that caused the deadlock, and dl_procs set
/// to contain with names of processes that are in the deadlock.
///
/// To indicate no deadlock was detected after processing all edges, you must
/// return Result with edge_index = -1 and empty dl_procs[].
///

//struct implemented in deadlock_detector.h class.
Result detect_deadlock(const std::vector<std::string> & edges)
{  
    // initialized classes and variables
    Result result;
    Sort topo_sort;
    Word2Int convertString;
     
    //We need to process all the edges, begin with setting edge_index to -1
    result.edge_index = -1;
    int edge_count = 0;
    while ( edge_count < int(edges.size())){
        string myEdge = edges[edge_count];
        vector<string> tasks = split(myEdge);
        string sign = tasks[1];

        //Given the sign input, we proceed with the following:
        if(sign == Assign || sign == Request){
            //append tasks to the resources and process
            tasks[2].append(Resource);
            tasks[0].append(Process);
        }
        if (sign == Assign) {
            //do the conversion
            Graph_f(true, convertString.get(tasks[0]), convertString.get(tasks[2]));
        }
        
        else if (sign == Request) {
            Graph_f(false, convertString.get(tasks[0]), convertString.get(tasks[2]));
        }
     
        //check_r();
        //check_s();
        else {
            continue;   //Default would have an invalid input for sign
        }

        // add to hash_map so that we can conver it back to string
        hash_map[convertString.get(tasks[0])] = tasks[0];
        hash_map[convertString.get(tasks[2])] = tasks[2];   
        // We need to check if there is a cycle or not and set edge_index = -1 if not
        if (result.dl_procs.empty()) {
            result.edge_index = -1;
        }//apply topo sort 
        if (!topo_sort.topo_sort() == 0) {
            result.edge_index = edge_count;
            break;
        }
        edge_count++;
    }
    result.dl_procs = push;
    return result;
}
