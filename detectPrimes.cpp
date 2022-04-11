// =============================================================================
// You must modify this file and then submit it for grading to D2L.
// =============================================================================

#include "detectPrimes.h"
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <thread>
#include <atomic>
#include <math.h>
#include <pthread.h>

//This makes our life easier
using namespace std;

//-------------------------------- GLOBAL VARIABLES -----------------------------//
//Initialized global variables
atomic<bool> flag (false);
atomic<bool> global_finished (false);
atomic<bool> task1 (false);

//Thread-synchronization variables.
pthread_barrier_t myBarrier;
pthread_mutex_t myMutex;
double count1;

//Globalized variables from trivial cases. 
int64_t i = 5;
int64_t nextNum;

//Global variables.
int global_threads;
vector<int64_t> global_nums;
bool global_Prime;

bool value;
vector<bool> value_vector;
vector<int64_t> result;

//--------------------------------- TRIVIAL CASES -------------------------------//
// returns true if n is prime, otherwise returns false
// param: 
// n: number needed to check.
// start: start index for while loop.
// work: how many times i need to be increased. 

static bool is_prime(int64_t n, int64_t i, int64_t max)
{
//Parallelize the inner loop.

  // handle trivial cases
  if (n < 2) return false;
  if (n <= 3) return true; // 2 and 3 are primes
  if (n % 2 == 0) return false; // handle multiples of 2
  if (n % 3 == 0) return false; // handle multiples of 3
  // try to divide n by every number 5 .. sqrt(n)
  //int64_t i = 5;
  //int64_t max = sqrt(n)

  //Handle thread cancellation
  flag.store(false, memory_order_relaxed);
  while (i <= max && flag == false) {
    if ((n % i == 0) || (n % (i + 2) == 0))  {
      flag.store(true, memory_order_relaxed);
      return false;
    }
    i += 6;
  }
  // didn't find any divisors, so it must be a prime
  return true;
}

//-------------------------------- DECLARE FUNCTIONS ---------------------------//

static void serial_algorithm(int res, int tid_int);
static void parallel_algorithm(int tid_int); 
static void serial_algorithm2(int res, int tid_int); 

//-------------------------------- THREAD FUNCTIONS ---------------------------//

void * thread_f1(void * tid){
  int tid_int;
  tid_int = (long int) tid;

  //Repeat forever:
  while(1){
    
    // wait for all threads to be synchronized
    int res = pthread_barrier_wait(&myBarrier);

//------------------- SERIAL TASK --------------------------------//

    //Algorithm for serial task.
    serial_algorithm(res, tid_int);

    // wait for all threads to be synchronized
    pthread_barrier_wait(&myBarrier);   


//------------------- PARALLEL TASK ------------------------------//
//executed by all threads, via barrier

    // if global_finished flag is set, exit thread.
    if (global_finished == true) {
      //cout << finished << endl;
      break;
    }

    //Algorithm for parallel task.
    parallel_algorithm(tid_int);

    // wait for all threads to be synchronized
    pthread_barrier_wait(&myBarrier);


//------------------ SERIAL TASK 2 -------------------------//

    //Algorithm for serial task.
    serial_algorithm2(res, tid_int);
  }
    //Exit thread
    pthread_exit(NULL);
}


//----------------- SERIAL TASK ALGORITHM --------------------------//
static void serial_algorithm(int res, int tid_int) {
    //This part of the code was taken from barrierExp2.cpp provided by tutorials.

    //serial task - pick one thread using barrier
    if( res == PTHREAD_BARRIER_SERIAL_THREAD)
    { 

      //if no more numbers left then set global_finished = true
      if (count1 != global_nums.size()){

        // pick a number
        nextNum = global_nums[count1];

        // update flag!
        task1.store(false, std::memory_order_relaxed);  

        if (nextNum < 1000000){

          // update flag!
          task1.store(true, std::memory_order_relaxed);
          value = is_prime(nextNum, 5, (int) sqrt(nextNum));
          value_vector[tid_int] = value;
        }
        count1++;
      }
      //otherwise: divide work for each thread.
      else { 
        global_finished.store(true,std::memory_order_relaxed); //It indicate to all threads to quit.
      }
    }
}

//----------------- PARALLEL TASK ALGORITHM --------------------------//
static void parallel_algorithm(int tid_int) {
//serial task – pick one thread using barrier
//combine the per-thread results and update the result[] array if necessary
    int64_t max = sqrt(nextNum);
    bool value;

    if (task1 == false){
      
    //divide work among threads
    int64_t split;
    split = max / (6 * global_threads);

    int64_t start_x;
    start_x = (i + split * 6 * tid_int);

    int64_t end_x;
    end_x = (start_x + split*6);
    //In this case we would have 5 mod 6.

    //checks if the value of 5 is greater than the square root. 
    if (i >= max){
      end_x = max;  //Sets end_x to be square root. 
    }
    int nThreads = global_threads - 1;

    //checks whether tid_int is equivalent to nThread parameter. 
    if (tid_int == nThreads) {
      while(end_x < max){
        end_x += 6;   //Increment end_x by 6 if condition satisfied.
      }
      if (end_x > max) {
        end_x -= 6;   // Increment end_X by 6 if condition satisfied.
      }
    }

    //Set our boolean variable to the function is_prime.
    value = is_prime(nextNum, start_x, end_x);

//We want to prevent multiple threads from assessing the same memory.
    //Thread locks code.
    pthread_mutex_lock(&myMutex);

    value_vector[tid_int] = value;

    //Thread unlocks code.
    pthread_mutex_unlock(&myMutex);
    }
}

//----------------- SERIAL TASK 2 ALGORITHM --------------------------//
static void serial_algorithm2(int res, int tid_int) {
    // combine per thread result
    global_Prime = true;

    if ( res == PTHREAD_BARRIER_SERIAL_THREAD){

      //Check cases for task1 
      switch (task1){
        case true:
          //Concatenate properties
          global_Prime &= value_vector[tid_int];
          break;
        case false:
          for (int i = 0; i < global_threads; i++){
            //Concatenate properties
            global_Prime &= value_vector[i];
            if (value_vector[i] == 0) {
              break;
            }
          }
      }

      if (global_Prime) {
        result.push_back(nextNum);
      }
    }
} 


//------------------------------- DETECT PRIMES ------------------------------------//

// This function takes a list of numbers in nums[] and returns only numbers that
// are primes.
//
// The parameter n_threads indicates how many threads should be created to speed
// up the computation.
// -----------------------------------------------------------------------------
// You will most likely need to re-implement this function entirely.
// Note that the current implementation ignores n_threads. Your multithreaded
// implementation must use this parameter.
std::vector<int64_t>
detect_primes(const std::vector<int64_t> & nums, int n_threads)
{ 
  // initialize variables
  global_nums = nums;
  value_vector.resize(n_threads);

//-------------------------------- BARRIERS ----------------------//

  //Parse command line arguments
  // create a barrier for total number of threads.
  global_threads = n_threads; 
  pthread_barrier_init(&myBarrier, NULL, n_threads);


//-------------------------------- USE ALL THREAD ----------------------//

  //Declare thread
  pthread_t t1[n_threads];

  //Create actual thread
  for (long i = 0; i < n_threads; i++){
    pthread_create(&t1[i], NULL, thread_f1, (void*) i);
  }

  //Wait for join Thread
  for (int i = 0; i < n_threads; i++){
    pthread_join(t1[i], NULL);
  }

  return result;
}
