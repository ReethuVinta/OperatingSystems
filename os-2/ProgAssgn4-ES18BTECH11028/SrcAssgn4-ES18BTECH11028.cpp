#include <iostream>
#include <fstream>
#include <thread>
#include <vector>
#include <unistd.h>
#include <random>
#include <chrono>
#include <semaphore.h>
#include <algorithm>
#include <sys/time.h>
using namespace std;

int N,X;                        // Input variables
double lambda,r,Gamma;
sem_t mutex,block;              // Declaring locks needed to carry out the function
bool must_wait = false;         // Boolean variable to indicate if a grp is formed or not
int eating = 0;                 // Number of people that are currently dining
int waiting = 0;                // Number of people that are currently waiting
double *waiting_time;           // Array that maintains waiting times of each thread   
typedef long long int lli;      // typedef

vector<pair<lli,pair<string, pair<int,int>>>>mainBuffer;    // Buffer that stores all the thread print statements
typedef struct store_time       // Structure to store time
{
    int hours;
    int minutes;
    int seconds;
    int millisecs;
} store_time;

store_time getSysTime(time_t input_time)    // Given time it return time store in a struct
{
    struct tm *time_info;
    store_time s;
    time_info = localtime(&input_time);
    s.hours = time_info->tm_hour;
    s.minutes = time_info->tm_min;
    s.seconds = time_info->tm_sec;
    return s;
}

void *Restaurant(void *params)             // Function that is passed inside the threads
{
    int thread_number = *((int *)params);  // Retrieve the customer number

    struct timeval access_gettimeofday;    // Noting down the access request time  
    time_t accessRequest = time(NULL);
    gettimeofday(&access_gettimeofday,NULL);
    store_time accessRequestStruct = getSysTime(accessRequest);  
    accessRequestStruct.millisecs = access_gettimeofday.tv_usec / 1000;   // Milli seconds in current time
    lli accessRequestMicroSeconds = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();   // Time in microsecnds from 1970 year

    // Entering the restaurant
    sem_wait(&mutex);             // This is to ensure that one person is processed at a time

    // Pushing the acess request print statement into the buffer
    string accessRequestTimeString = to_string(accessRequestStruct.hours) + ":" + to_string(accessRequestStruct.minutes) + ":" + to_string(accessRequestStruct.seconds) + ":" + to_string(accessRequestStruct.millisecs);
    mainBuffer.push_back({accessRequestMicroSeconds,{accessRequestTimeString,{thread_number,1}}});

    if(eating+1>X || must_wait == true) // Checking if the group is formed or not
    {
        waiting++;
        must_wait = true;
        sem_post(&mutex);
        sem_wait(&block);
    }

    else                
    {
        eating++;       // Incrementing
        must_wait = (waiting>0 && eating==X);
        sem_post(&mutex);
    }   
    
    struct timeval acessegiven_gettimeofday;    // Noting the enter time
    time_t accessGiven = time(NULL);          
    gettimeofday(&acessegiven_gettimeofday,NULL);            
    store_time accessGivenStruct = getSysTime(accessGiven);  
    accessGivenStruct.millisecs = acessegiven_gettimeofday.tv_usec / 1000;
    lli accessGivenMicroSeconds = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();

    // Eating
    int seed = chrono::system_clock::now().time_since_epoch().count(); // Seed to randomly intialise
    default_random_engine generator (seed);   
    exponential_distribution<double> SimulationOfEating(Gamma);
    usleep((SimulationOfEating)(generator)*1e3);  // Simulation of eating

    // Exiting
    sem_wait(&mutex);     // Here we are updating number of people dining as they done eating
    eating--;

    // Saving the waiting time of current customer in micro seconds
    waiting_time[thread_number-1] = (accessGivenMicroSeconds-accessRequestMicroSeconds);

    // Pushing the access given time string inside the main buffer
    string accessGivenTimeString = to_string(accessGivenStruct.hours) + ":" + to_string(accessGivenStruct.minutes) + ":" + to_string(accessGivenStruct.seconds) + ":" + to_string(accessGivenStruct.millisecs);
    mainBuffer.push_back({accessGivenMicroSeconds,{accessGivenTimeString,{thread_number,2}}});

    struct timeval exit_gettimeofday;       // Noting the enter time
    time_t exiting = time(NULL);          
    gettimeofday(&exit_gettimeofday,NULL);            
    store_time exitingStruct = getSysTime(exiting); 
    exitingStruct.millisecs = exit_gettimeofday.tv_usec / 1000;
    lli exitingMicroSeconds = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();
    
    // Pushing the exit time string inside the main buffer
    string exitingTimeString = to_string(exitingStruct.hours) + ":" + to_string(exitingStruct.minutes) + ":" + to_string(exitingStruct.seconds) + ":" + to_string(exitingStruct.millisecs);
    mainBuffer.push_back({exitingMicroSeconds,{exitingTimeString,{thread_number,3}}});

    if(eating == 0)   
    {
        int k = min(X,waiting);
        waiting-=k;
        eating+=k;
        must_wait = ( waiting >0 && eating==X); // If group is done eating and leaves the restaurant
        for(int i=0;i<k;i++)
        {
            sem_post(&block);
        }
    }

    sem_post(&mutex);

    pthread_exit(0); // Terminating the current thread
}

int main()
{
    FILE *input_ptr;           // File pointers used for handling read, write operations in the files
    input_ptr=fopen("input.txt","r"); // Opening the input.txt file

    if(input_ptr==NULL)        // If there is error while open the file then this block gets executed
    {
        printf("Couldn't open the mentioned file \n");
        exit(1);                        
    }

    fscanf(input_ptr,"%d",&N);   // Taking inputs
    fscanf(input_ptr,"%d",&X);
    fscanf(input_ptr,"%lf",&lambda);
    fscanf(input_ptr,"%lf",&r);
    fscanf(input_ptr,"%lf",&Gamma);

    fclose(input_ptr);           // Closing input file

    int seed = chrono::system_clock::now().time_since_epoch().count(); // Seed to randomly intialise
    default_random_engine generator (seed);   
    exponential_distribution<double> delay(lambda);
    uniform_int_distribution<int> entering(1, r*X);

    waiting_time = (double *)malloc(sizeof(double) * N); // Allocating memory for the array

    sem_init(&mutex, 0, 1);          // Iniatilising values of muetx locks
    sem_init(&block, 0, 0);

    pthread_t threads[N];            // Declaring thread array
    pthread_attr_t attr[N];          // Contents are used at thread creation time to determine attributes for the new thread

    for(int i=0;i<N;i++)             // Initialsing threads attributes
    {
        pthread_attr_init(&attr[i]);
    }

    int customers = N;
    int start = 0;
    
    while(customers>0)
    {
        int number_of_people_entering = min(customers, entering(generator));
        usleep((delay)(generator)*1e3);  // Set of people are entering with a delay
        cout<<"number of people entering "<<number_of_people_entering<<endl;
        cout<<"customers "<<customers<<endl;
        cout<<"start "<<start<<endl;
        for(int i = start; i < start+number_of_people_entering; i++)
        {
            int *thread_number = (int *)malloc(sizeof(int));
            *thread_number=i+1;
            pthread_create(&threads[i],&attr[i],Restaurant,thread_number); //Creating the thread passing the function
        }
        start+=number_of_people_entering;       // Updating the values
        customers-=number_of_people_entering;
    }
    for(int i=0;i<N;i++)
    {
        pthread_join(threads[i], NULL);         // Joining the terminated threads
    }

    sort(mainBuffer.begin(),mainBuffer.end());  // Sorting buffer using the first value i.e time in microseconds

    // Using main thread to print statements 
    FILE *output = freopen("Log.txt","w+",stdout); // Opening the Log.txt file
    for(auto s : mainBuffer)
    {
        if(s.second.second.second==1)
        {
            cout<<s.second.second.first<<"st customer access request at "<<s.second.first<<endl;
        }
        else if(s.second.second.second==2)
        {
            cout<<s.second.second.first<<"st customer given access at "<<s.second.first<<endl;
        }
        else if(s.second.second.second==3)
        {
            cout<<s.second.second.first<<"st customer is exiting at "<<s.second.first<<endl;
        }
    }
    double avg_waiting_time=0;
    for(int i = 0;i < N;i++)           // Calculating average waiting times
    {
        cout<<i+1<<"th thread waiting time is "<<waiting_time[i]<<" microsecs"<<endl;
        avg_waiting_time+=waiting_time[i];
    }
    cout<<"Average waiting time = "<<avg_waiting_time/N<<" microsecs"<<endl;
    fclose(output);
    return 0;
}