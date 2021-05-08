#include <iostream>  // Importing necessary libraries
#include <chrono> 
#include <pthread.h> 
#include <sys/time.h> 
#include <fstream>
#include <unistd.h>
#include <atomic>
#include <random>
#include <iomanip>

using namespace std;

int n,k;
double lambda1,lambda2;
FILE *output = fopen("TAS-Log.txt","w+");          // Opening the TAS-Log.txt file
double *waiting_time;                              // Array that maintains waiting times of each thread
double max_waiting_time;                           // Maximum waiting time amoung all the instances of threads
atomic_flag lock_variable = ATOMIC_FLAG_INIT;      // Lock variable

typedef struct store_time  // Structure to store time
{
    int hours;
    int minutes;
    int seconds;
}store_time;

store_time getSysTime(time_t input_time)  // Given time it return time store in a struct
{
    struct tm* time_info;
    store_time s;
    time_info = localtime(&input_time);
    s.hours = time_info->tm_hour;
    s.minutes = time_info->tm_min;
    s.seconds = time_info->tm_sec;
    return s;
}

void *testCS(void *params)   // Main function
{
    int thread_number = *((int *) params); // Retrieve the thread number
    
    int seed = chrono::system_clock::now().time_since_epoch().count(); // Seed to randomly intialise
    default_random_engine generator (seed); 
    exponential_distribution<double> SimulationCS(1/lambda1);
    exponential_distribution<double> SimulationRS(1/lambda2);

    for(int i=0;i<k;i++)
    {
        // Entry Section
        time_t reqEnterTime = time(NULL);                      
        store_time reqEnterStruct = getSysTime(reqEnterTime);  // Enter time
        fprintf(output, "%d th CS Requested at %d:%d:%d by thread %d \n",i+1, reqEnterStruct.hours, reqEnterStruct.minutes, reqEnterStruct.seconds,thread_number);
        
        while (atomic_flag_test_and_set(&lock_variable));      // Do nothing, keep looping
        // Critical Section
        time_t accEnterTime = time(NULL);
        store_time accEnterStruct = getSysTime(accEnterTime);  // Critical section entered time
        
        waiting_time[thread_number-1]+= double(accEnterTime - reqEnterTime);
        max_waiting_time=max(max_waiting_time,double(accEnterTime - reqEnterTime)); // Saving the max waiting time

        fprintf(output, "%d th CS Entered at %d:%d:%d by thread %d \n",i+1, accEnterStruct.hours, accEnterStruct.minutes, accEnterStruct.seconds,thread_number);   
        
        usleep((SimulationCS)(generator)*1e3);  // Simulation of critical-section
        // Exit section
        time_t exitTime = time(NULL);
        store_time exitStruct = getSysTime(exitTime);  // Exit time

        fprintf(output, "%d th CS Exited at %d:%d:%d by thread %d \n",i+1, exitStruct.hours, exitStruct.minutes, exitStruct.seconds,thread_number);   

        lock_variable.clear();
        // Remainder section
        usleep((SimulationRS)(generator)*1e3);  // Simulation of Reminder Section
    }
    pthread_exit(0);                            // Terminating the current thread
}
int main()
{
    FILE *input_ptr;           // File pointers used for handling read, write operations in the files
    input_ptr=fopen("inp-params.txt","r"); // Opening the inp.txt file

    if(input_ptr==NULL)        // If there is error while open the file then this block gets executed
    {
        printf("Couldn't open the mentioned file \n");
        exit(1);                        
    }

    fscanf(input_ptr,"%d",&n);   // Taking inputs
    fscanf(input_ptr,"%d",&k);
    fscanf(input_ptr,"%lf",&lambda1);
    fscanf(input_ptr,"%lf",&lambda2);

    fclose(input_ptr);               // Closing input file
    waiting_time = (double*)malloc(sizeof(double)*n);  // Allocating memory for the array
    pthread_t threads[n];            // Declaring thread array
    pthread_attr_t attr[n];          // Contents are used at thread creation time to determine attributes for the new thread

    for(int i=0;i<n;i++)             // Initialsing threads attributes
    {
        pthread_attr_init(&attr[i]);
    }

    for(int i=0;i<n;i++)
    {
        int *thread_number = (int *)malloc(sizeof(int));
        *thread_number=i+1;
        pthread_create(&threads[i],&attr[i],testCS,thread_number); //Creating the thread passing the function
    }

    for (int i = 0; i < n; i++) 
    {
        pthread_join(threads[i], NULL); // Joining the terminated threads 
    }

    double avg_waiting_time=0;
    for(int i = 0;i < n;i++)           // Calculating average waiting times
    {
        avg_waiting_time+=waiting_time[i];
    }
    fprintf(output,"Average waiting time = %lf\n",avg_waiting_time/(n*k));
    fprintf(output,"Maximum waiting time = %lf\n",max_waiting_time);

    fclose(output);               // Closing input file
    return 0;
}