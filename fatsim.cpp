// -------------------------------------------------------------------------------------
// this is the only file you need to edit
// -------------------------------------------------------------------------------------
//
// (c) 2021, Pavol Federl, pfederl@ucalgary.ca
// Do not distribute this file.

#include "fatsim.h"
#include <cstdio>
#include <iostream>
#include <vector>
#include <iterator>//for list to call prev()
#include <math.h>
#include <stack>
#include <map>
#include <list>
#include <algorithm>
#include <set>
using namespace std;

//Initiate global variables
vector<long> blocks, depth_level;
map<long, long> block_length;
vector<bool> visited, looped_vec;
long length_vec;
long chain_length; 
long i, iter_val;
long next_ptr = -1;
long the_counter;

//Apply depth first search in reverse cycle 
void dfs_function(const vector<long>& fat, vector<long> depth_val, map<long, bool> visited_flag) {
    //apply dfs indefinitely
    while (1) {
      // get the depth value and marked as visited
      visited_flag[iter_val] = true;
      depth_val.push_back(iter_val);
      chain_length = 0;
      //auto visited_val = visited_flag[fat[val]]; 

     /* switch (fat[v]) {
        case -1:
        next_ptr = v;
        length_vec = 0;
        break;
      }*/
      //check if visited flag is true and if it contains the same depth_value 
      if ((visited_flag[fat[iter_val]] == true)||(looped_vec[fat[iter_val]] == true)) {  
        chain_length = 1;
        break;
      
      }//Check if pointer set to -1 to start search  
      /*switch(fat[iter_val]) {
        case -1:
        length_vec = 0;
        next_ptr = val;
        break;
        default:{val = fat[val];}
      } */
      if (fat[iter_val] == -1) { 
        length_vec = 0;
        next_ptr = iter_val;
        break;
      //else checks if node has been visited and if so, set our vector to the depth assign the next pointer  
      } if (visited[fat[iter_val]] == true) {
        next_ptr = blocks[fat[iter_val]];
        length_vec = depth_level[fat[iter_val]];
        break;
      } 
      /*switch(visited[fat[val]]){
        case true:
        next_ptr = blocks[fat[val]];
        length_vec = depth_level[fat[val]];
        break;
      }*/ iter_val = fat[iter_val];

      //value becomes the file allocator value 
    } //decrement the counter by decreasing the size of depth
    auto depth_length = depth_val.size();  
    the_counter = (depth_length-1);

    //set the conditionals as it iterates through each node in the graph
    while (the_counter >= 0) {
      //set the iteration variable to the depth value in that iteration, then mark as visited and set block iteration to the next pointer
      i = depth_val[the_counter];
      blocks[i] = next_ptr;
      visited[i] = true;
      //if the chain is at the start, set the depth and increment the vector then set the depth to the vector
      if(chain_length == 0){
          length_vec++;
          depth_level[i] = length_vec; 
          
        //if the chain is equal 1 we want to set vector variable at this instance to true
      } if(chain_length == 1){
          looped_vec[i] = true; //Set the vector variable instance to true.
          
      }the_counter -= 1;
      
    }
}
/*void assign_block(int iterator, map<long, long> block_length, vector<long> depth_level, long next_ptr){
  if (block_length[next_ptr]<depth_level[iterator]) {
        block_length[next_ptr]=depth_level[iterator];
      }
}*/


// reimplement this function
std::vector<long> fat_check(const std::vector<long>& fat) {

  //Initiate the file allocator size, nodes variables, depth and blocks 
  vector<long> the_nodes;
  map<long, long> block_length;
  vector<long> depth_val;
  int fat_length = ((int)fat.size());
  map<long, bool> visited_flag;

  //file allocator size variables since we start in reverse
  blocks = depth_level = vector<long>(fat_length);
  visited = looped_vec = vector<bool>(fat_length);
  
  long j = 0;
  while(j < fat_length){
    switch(visited[j]){
      case false:
      //set the global variable value to the iteration
      iter_val = j;
      //check the files by applying depth first search to the reverse list
      dfs_function(fat, depth_val, visited_flag);
      //working backwards, if the longest file block is less than depth then set the next pointer to that depth
      //assign_block(j, block_length, depth_level, next_ptr);
      if (block_length[next_ptr]<depth_level[j]) {
        block_length[next_ptr]=depth_level[j];
      }
    }
    j++;
  }
    /*if (visited[j] == false){
      //adjust the global variable value 
      val = j;
      //check the files by applying depth first search to the reverse list
      dfs_function(fat, depth_val, visited_flag);
      if (block_length[next_ptr]<depth_level[j]) {
        block_length[next_ptr]=depth_level[j];
      }
    }*/
    
  /*for (long int i = 0; i < fat_length; i++) {
    if (visited[i] == false) {
      
      val = i;
      
      //Calls the dfs_function method. (Depth First Search Algorithm)
      dfs_function(fat, depth_val, visited_flag);

      if (depth_level[i] > block_length[next_ptr]) {
        block_length[next_ptr] = depth_level[i];
      }
    }
  }*/
  auto block = block_length.begin(); 
  while(block != block_length.end()){
    if (block->second > 0) {
      the_nodes.push_back(block->second);
    }
    block++;
  } //sort to reverse order the end to the beginning of node
  sort(the_nodes.begin(), the_nodes.end());
  return the_nodes;
}

  /*
  //Searches through for the remainder of depth. 
  for (auto j = block_length.begin(); j != block_length.end(); j++) {
    if (j->second > 0) {
      the_nodes.push_back(j->second);
    }
  }*/
