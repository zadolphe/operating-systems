// you need to modify this file

#include "scheduler.h"
#include "common.h"
#include <vector>
#include <queue>
#include <climits>
#include <iostream>
#include <utility>
#include <algorithm>

using namespace std;

// this is the function you should edit
//
// runs Round-Robin scheduling simulator
// input:
//   quantum = time slice
//   max_seq_len = maximum length of the reported executing sequence
//   processes[] = list of process with populated IDs, arrival_times, and bursts
// output:
//   seq[] - will contain the execution sequence but trimmed to max_seq_len size
//         - idle CPU will be denoted by -1
//         - other entries will be from processes[].id
//         - sequence will be compressed, i.e. no repeated consecutive numbers
//   processes[]
//         - adjust finish_time and start_time for each process
//         - do not adjust other fields
//
//
void simulate_rr(
    int64_t quantum, 
    int64_t max_seq_len,
    std::vector<Process> & processes,
    std::vector<int> & seq
) {
    //Clears the sequence
    seq.clear();

    //Initialize local variables
    int64_t time = 0;
    int64_t remaining_time = -1;
    //this wi
    int process_counter = 0;
    int processor = -1;
    //we want to see how far along the process we are at
    int process_length = processes.size();
    int buffer = 100000;
    int64_t amount;
    int64_t remaining_bursts = 0;
    int64_t Remaining = 0;
    queue<pair<int, int64_t>> queue1;

    while(true){
        //Initialize Variables
        int sizeQueue = queue1.size();
        //int sizeSequence = seq.size();
        int64_t arrival;
        bool checkProcess = false;
        queue<pair<int, int64_t>> r_queue;       
        //Breaks the loop if there are no processes left.
        if (process_length == 0) {
            break;
        }
        //push the process to queue based on arrival
        while (processes[process_counter].arrival_time == time && processes.size() > 0){
            queue1.push(make_pair(process_counter, processes[process_counter].burst));
            process_counter += 1;
        }
        //set our local queue and the arrival time
        r_queue = queue1;
        arrival = processes[process_counter].arrival_time;
        
        //We need to keep track of the CPU when it's idle then we will check for an empty and if a CPU has a process
        if(processor == -1){
            //Check if there is a queue and not the last process
            if ((process_length != 1) && (sizeQueue != 0)) { 
                if (process_counter == (int) processes.size()){
                    arrival = r_queue.front().second + time;
                }
                //counter to loop through the queue  
                //int the_counter = 0;
                for(int the_counter = 0; the_counter < int(sizeQueue); the_counter++){
                    //If there is no start time then we being the process.
                   // int the_start_time = processes[r_queue.front().first].start_time; 
                    if (r_queue.front().second < quantum){Remaining += (quantum - r_queue.front().second);}
                    
                }

                for(int the_counter=0; the_counter < int(sizeQueue); the_counter++){
                    //advance the current time to the of the process and increment in increasing quantum minus the remaining time slice
                    if (processes[r_queue.front().first].start_time == -1)
                    {processes[r_queue.front().first].start_time = time -Remaining + ((quantum * the_counter));}
                }

                // Our process will finish before it exceeds the quantum and set remaining components to the remaining variable
                for(int the_counter = 0; the_counter < int(sizeQueue); the_counter++){
                    //if the front of the queue and time are less than the arrival we set the arrival to our process to finish before quantum
                    if (arrival> r_queue.front().second + time ){arrival = r_queue.front().second + time;}
                    //then remove from the queue
                    r_queue.pop();
                }
                
                //we incrememt the time by rq.size()*quantum
                int rq_size = (sizeQueue*quantum);
                amount = (arrival - time) / rq_size;
                time += (amount * (rq_size));
                //counter to loop through the queue
                //int counter = 0;
                for(int counter =0; counter < sizeQueue; counter++){
                    //If the amount is not 0 then take (amount * quantum) from stack of queue.
                    if (amount != 0) {
                        queue1.front().second -= (amount * quantum);
                    }
                    //Add the first element of queue to the r_queue stack then pop the queue
                    r_queue.push(queue1.front());     
                    queue1.pop();
                }
                
                 
                //Check if amount is greater than the buffer.
                if (amount > buffer) {
                    amount = buffer;  //Set amount to buffer.
                }
                //Loop to iterate through our amount and reset the queue
                //reset queue1 
                queue1 = r_queue;
                int time_counter = 0;
                while(time_counter < amount){
                     //Set r_queue to queue1
                    r_queue = queue1;                       
                    int counter = 0;    //Create counter to iterate through the queue size.
                    while ( (amount > 0 && seq.back() != r_queue.front().first) && (counter < sizeQueue)) {   //Checks whether our amount and last index of sequence  is not equivalent to first index of queue.
                        seq.push_back(r_queue.front().first);    //If condition holds, then we push the queue to front of the sequence.             
                        r_queue.pop();   //Pop the top queue2.
                        counter++;
                    }time_counter++;
                }
            }
        } else {
                //Check the time if it's equal to remaining time to push into the top of the job.
                if (time == remaining_time) {    
                    queue1.push(make_pair(processor, remaining_bursts)); 
                    //reset the processor back to idle
                    processor = -1; 
                    continue;
                }
                //Default case we check if remaining_bursts is equivalent to 0
                if (remaining_bursts == 0){
                    //Set time to the finish_time of the processor
                    processes[processor].finish_time = time; 
                    //Set the processor back to idle.
                    processor = -1; 
                    process_length --;  
                    continue;
                }
                
            }

        //code from recorded tutorials and from mapp.cpp
        // We need to see if the CPU is idle and if the CPU has a process or empty
        if (!queue1.empty() && processor == -1) {
            //Set remaining_bursts to the first process in the queue and then remove it to move to the next
            remaining_bursts = queue1.front().second;
            processor = queue1.front().first;        
            queue1.pop();                           
            //we then check the current burst is equivalent to the remaining bursts
            // If condition holds, then we set the start time of processes to the amount for time
            if (remaining_bursts == processes[processor].burst) {    
                processes[processor].start_time = time;            
            } 
            //Set the remaining time equivalent to the time incremented by the quantum.
            remaining_time = time + quantum;     
            continue;
        }
    
        //In this case, updates the seq if needed
        //Check if sequence size = 0 and then push processor into the sequence
        if (seq.size() == 0) {
            seq.push_back(processor);
        }
        //If the back/bottom of sequence is not equivalent to processor, then push into sequence. 
        if (seq.back() != processor){
            //then push the processor to the bottom of the sequnce
            seq.push_back(processor);
        }
        //Checks if CPU processor is idle and queue is empty. 
        if (sizeQueue == 0 && processor == -1){
            time = processes[process_counter].arrival_time;
            continue;
        }
        // If the queue is empty and processor idle we set the time equal to the arrival time 
        if (sizeQueue == 0) {
            if(processor == -1){
                //Set the time equal to the arrival time 
                    time = processes[process_counter].arrival_time;
                    continue;
            } else 
            {
                //we need to update the remaining time for each process and the maximum N by dividing the quantum the smallest amount we can get
                amount = ((processes[process_counter].arrival_time - time) / quantum);
                    //Checks whether amount is greater than 0
                    if (amount > 0){
                        remaining_bursts -= (amount * quantum);
                        time += (amount * quantum);  
                        remaining_time = (time + quantum);
                    
                        //Checks if remaining burst is less than 0 then increment time and set to 0.
                        if (remaining_bursts < 0){
                            time += remaining_bursts;
                            remaining_bursts = 0;
                            //Sets remaining time to the time incremented by quantum units
                            remaining_time = (time + quantum);   
                        }
                    }
            }
        }

        //Check if the current execution is at the end of the process, if so set the flag to true
        if ((time + remaining_bursts) <= remaining_time) {
            checkProcess = true;
        }
        //Need to see if we are at the last process 
        if (checkProcess == true || process_length == 1){
            time += remaining_bursts;
            remaining_bursts = 0;
            for (long unsigned int counter = process_counter; (processes[counter].arrival_time < time) && (counter < processes.size());counter++){
                queue1.push(make_pair(process_counter, processes[process_counter].burst));
                process_counter += 1;
            }
        }

        //CPU burst executed executing one quantum for every processs
        
        if (remaining_bursts > 0) {
            //decrementing by the quantum time and increment the time by quantum
            remaining_bursts -= quantum;
            time += quantum;
            for (long unsigned int counter = process_counter; (counter < processes.size()) && (processes[counter].arrival_time < time); counter++){
                queue1.push(make_pair(process_counter, processes[process_counter].burst));
                process_counter += 1;
            }
        }

    }   
    //Finish loop and trim the sequence 
    seq.resize(min(max_seq_len, int64_t(seq.size())));
}
