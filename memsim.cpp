// -------------------------------------------------------------------------------------
// this is the only file you need to edit
// -------------------------------------------------------------------------------------
//
// (c) 2021, Pavol Federl, pfederl@ucalgary.ca
// Do not distribute this file.
#include "memsim.h"
#include <cassert>
#include <iostream>
#include <stdlib.h>
#include <unordered_map>
#include <vector>
#include <iomanip>
#include <list>
#include <set>//for struct
#include <iterator>
#include <math.h>
using namespace std;  


//from list_struct.cpp, list_trav.cpp, list_iterator.cpp, list_player.cpp
//and tutorial A5
//use a struct to partition 
struct Partition {
  int tag;
  int64_t size, addr;

  bool portion() {
    return this->tag == -1;
  }

Partition(int64_t addr, int64_t size, int tag) {
    this->tag = tag;
    this-> size = size;
    this-> addr = addr;
  }
};

typedef list<Partition>::iterator PartitionRef;
typedef set<PartitionRef>::iterator PartitionRefIterator;

struct scmp {  
  bool operator()(const PartitionRef & c1, const PartitionRef & c2) const {    
    if (c1->size == c2->size) 
      return c1->addr < c2->addr;
    else   
      return c1->size > c2->size;  
  }   
};

//Simulator
struct Simulator {
   
  // this will be sorted partitions by size/address
  set<PartitionRef, scmp> free_blocks;
  // all tagged partitions using unoredered map which will improve time
  unordered_map<long, vector<PartitionRef>> tagged_blocks;
  // linked list for all_blcoks
  list<Partition> all_blocks;
  //set structure fields - make sure this is the same as the sum of partition sizes 
  int64_t page_size;
  int64_t pages_requested;
  //int64_t page_volume = pages_requested*page_size;

  Simulator(int64_t page_size) {
    this->page_size = page_size;
    this->pages_requested = 0;
    // constructor
    //this->all_blocks = std::list<all_blocks>();
    //this->tagged_blocks = std::unordered_map<int, std::list<PartitionRef>>();
  }
  //use algorithms for page with this method 
  int64_t page_request(int64_t size) {
    int64_t req_per_page = (size / page_size);
    int64_t rem = size % page_size; 
    switch(rem){
      case 0:
      return req_per_page;
      break;
      default:
      return req_per_page +1;
    } 
  }

// Pseudocode for allocation request: 
    // - search through the list of partitions from start to end, and
    //   find the largest partition that fits requested size
    //     - in case of ties, pick the first partition found
    // - if no suitable partition found:
    //     - get minimum number of pages from OS, but consider the
    //       case when last partition is free
    //     - add the new memory at the end of partition list
    //     - the last partition will be the best partition
    // - split the best partition in two if necessary
    //     - mark the first partition occupied, and store the tag in it
    //     - mark the second partition free

//allocation function
  void allocation(int allocator_tag, int allocation) {
    bool suitable_Partition = true;
    PartitionRef the_largest;
    //PartitionRef smallest_part = all_blocks.begin();
    //bool free_blocks_valid;
    bool all_blocks_valid;
    set<PartitionRef, scmp>::iterator request = free_blocks.begin();
    //PartitionRef reverse_largest_block = all_blocks.reverse();
    //auto smallest_partition = all_blocks.begin();
    PartitionRef max_partition = all_blocks.end();
    
  
    //first check if the partition is empty, if so set the largest partition to request that fits and change the bool flag to false
    if (!free_blocks.empty()){
      //set<PartitionRef, scmp>::iterator request = free_blocks.begin();
      max_partition = * request;
      //free_blocks_valid = false;
    }

    // check if the largest parition is less than the size, if so then we have no parition that fits
    if (max_partition->tag != -1 || (max_partition->size < allocation)) { 
      //set the flags to false and the largest partition at the end of the blocks
      all_blocks_valid = false;
      max_partition = all_blocks.end();
      suitable_Partition = false;
    }
    //if we do not have a fitting partition then enlarge to the end 
    if (suitable_Partition == false && all_blocks_valid == false){
      //enlarge or expand the partition to the end
      PartitionRef end_node = prev(this -> all_blocks.end());
      //when the largest partition is not equal to the front we remove the last node and add new pages and set up new requests
      if (max_partition !=all_blocks.begin()){
        if (end_node -> portion()){
          //initialize teh variable for new pages and request
          int64_t the_request = this->page_request(allocation-end_node->size);
          //remove the last node 
          free_blocks.erase(end_node);

          //We are going to now add the new pages for the request
          end_node->size += the_request * this -> page_size;
          pages_requested += the_request;
    
        } else if(suitable_Partition == false){
          //do something 
        }
         else {
          //we are going to set up new requests and add the allocation amount needed and update the page requests
          int64_t the_request = this->page_request(allocation);
          pages_requested += the_request;
          //new node variable 
          int64_t new_node_addr = end_node -> addr + (end_node->size);
          //create new blocks to add to all blocks using the allocation amount, tag and new node address
          Partition new_blocks = Partition(new_node_addr, allocation, allocator_tag);
          //push back the new blocks 
          this->all_blocks.push_back(new_blocks); 
          
          
        }
      }
      
      // Checks that partition is not empty and equal to the front 
      if (max_partition == all_blocks.begin()){  
        //allocate the amount to make the request and update the page requests
          int64_t the_request = this->page_request(allocation);
          pages_requested += the_request;

          // set the variable for the partition add the new partition to the back of the blocks 
          auto parti = Partition(0, ceil((double) allocation /page_size)*page_size, -1);
          all_blocks.push_back(parti);
            
      } 
      
      // Checks that partition is empty
        else {
          //do something 
      }

      //now set the largest partition to the last index of the partitions, does not insert duplicates
      max_partition = prev(all_blocks.end());
      free_blocks.insert(max_partition);
    }

//Code is from tutorials files set_bounds.cpp, set_find.cpp, set_basic.cpp
//Separate and split partitions
    //Initialize variables and erase the largest partition in a new variable to keep the old value 
    vector<PartitionRef> partition;
    auto the_largest_partition = max_partition;
    free_blocks.erase(the_largest_partition);

    if (max_partition->size >allocation){
      //append remaining partition, we need to get the size of the set 
      Partition rem_parti = Partition(max_partition->addr, allocation, allocator_tag);
      this->all_blocks.insert(max_partition, rem_parti);


      //add the address with the amount and subtract from the size
      max_partition->size = (max_partition->size - allocation);
      max_partition->addr += allocation;
      this->free_blocks.insert(max_partition);

      //Check for left over partition amount 
      if (tagged_blocks.count(allocator_tag) <= 0){
        // append this amouht to top of list  
        partition.push_back(prev(max_partition));
        tagged_blocks[allocator_tag] = partition;
      }
      //push back previous largest partition to the back of the partition list
      else{
        tagged_blocks.at(allocator_tag).push_back(prev(max_partition));
      
      }
    }
    else if (max_partition->size <= allocation){
       //We want our variable largrest parition set to the allocation tag.
      max_partition->tag = allocator_tag;

      //if the allocator tag in the tagged blocks is not empty then we add the largest partition to the top and set the tagged block 
      //to the partition vector variable 
      if (tagged_blocks.count(allocator_tag) <= 0) {
        partition.push_back(max_partition);
        tagged_blocks[allocator_tag] = partition;

      }
      //if the tagged block has a remainder then we add the largest partition to the ontop to the vector 
      else if(tagged_blocks.count(allocator_tag) > 0) { 
        tagged_blocks.at(allocator_tag).push_back(max_partition);
      }
    }
    
  }

//Deallocation function
/* Pseudocode:
 - for every partition
 - if partition is occupied and has a matching tag:
 - mark the partition free
 - merge any adjacent free partitions
 */
  void deallocation(int deallocation_tag) { 
    //initiate variables for deallocation
    int64_t adj_part = -1;
    PartitionRef partition_2;
    auto Partitions = tagged_blocks.find(deallocation_tag);
    //set the blockpartition to the tagged block for reference
    vector<PartitionRef> block_Partition = this->tagged_blocks[deallocation_tag];

    //create a partition based on reference vector. 
    for (PartitionRef partition : block_Partition){
        partition_2 = next(partition);
      //Check if partitions is not equivalent to the tagged_blocks' last instance value, then set the partition variables
      if (this->tagged_blocks.end()!=Partitions){
        PartitionRef partition_1 = prev(partition);
        block_Partition = Partitions-> second;
        
        //set values accordingly if partition_1 and partition_2 or both are occupied   
        //set to zero if the first/front partition and back partition are occupied
        if (partition_2->portion() &&partition_1->portion()){adj_part= 0;}
          //if just the first/front partition set adjacent partition to 1
        else if(partition_1->portion()){adj_part= 1;
        } 
        //else set the to two if second parition occupied
        else if(partition_2->portion()) {
          adj_part= 2;
        } if (partition_2->portion() || partition_1->portion() ){
          //adj_part = adj_part;
        } 
        else {
          //adj_part = adj_part;
        } 
      
        // We need to combine free partitions that are next or adjacent to each other
         if(adj_part == 0){
          this->free_blocks.erase(partition_1);
          this->free_blocks.erase(partition_2);
          this->all_blocks.erase(partition_2);
          this->all_blocks.erase(partition);
          auto combined_size = (partition->size + partition_2->size);
          partition_1->size += combined_size;
          free_blocks.insert(partition_1);
        } else if(adj_part == 1){
           free_blocks.erase(partition_1);
          partition_1->size += partition->size;
          this->free_blocks.insert(partition_1);
          this->all_blocks.erase(partition);
          
          
        } else if(adj_part == 2){
          partition->tag = -1;
          free_blocks.erase(partition_2);
          partition->size += partition_2->size;
          this->free_blocks.insert(partition);
          this->all_blocks.erase(partition_2);
        } else {
          partition->tag = -1;
          free_blocks.insert(partition);
        }
        
      }//clear the partition block and assign the second member from partitions to current partition iteration
      block_Partition.clear();
      Partitions->second = block_Partition;
    }
  }

//get the result
  MemSimResult getStats() {
    //initiate variables
    MemSimResult the_result;
    set<PartitionRef, scmp>::iterator the_request;
    
    if(free_blocks.size()!=0){
      the_request = this->free_blocks.begin();
      the_result.max_free_partition_address = (*the_request)->addr;
      the_result.max_free_partition_size = (*the_request)->size;
      
    } else if(free_blocks.size()==0) {
      the_result.max_free_partition_address = 0;
      the_result.max_free_partition_size = 0;
    }//update page requests 
    the_result.n_pages_requested = this->pages_requested;
    
    return the_result;
  }

void check_consistency() {
// you do not need to implement this method at all - this is just my suggestion
    // to help you with debugging

    // mem_sim() calls this after every request to make sure all data structures
    // are consistent. Since this will probablly slow down your code, you should
    // disable calling this in the mem_sim() function below before submitting
    // your code for grading.

    // here are some suggestions for consistency checks (see appendix also):

    // make sure the sum of all partition sizes in your linked list is
    // the same as number of page requests * page_size

    // make sure your addresses are correct

    // make sure the number of all partitions in your tag data structure +
    // number of partitions in your free blocks is the same as the size
    // of the linked list

    // make sure that every free partition is in free blocks

    // make sure that every partition in free_blocks is actually free

    // make sure that none of the partition sizes or addresses are < 1
  }


}; 
//end of simulator struct

// re-implement the following function
// ===================================
// parameters:
//    page_size: integer in range [1..1,000,000]
//    requests: array of requests
// return:
//    some statistics at the end of simulation
MemSimResult mem_sim(int64_t page_size, const std::vector<Request> & requests)
{

  Simulator sim(page_size);
  //loop through the requests and call the allocation functions
  for(const auto &req: requests) {
    if(req.tag >= 0){
      sim.allocation(req.tag, req.size);
    } else if(req.tag < 0){
      sim.deallocation(-(req.tag));
    } sim.check_consistency();
  }
  
  return sim.getStats();
}
