#include<bits/stdc++.h> // Library function
using namespace std;

int n;                  // Number of processes
double current_time=0;  // Clock variable 
int ProcessID;          // Varibale to store process ID
int missed_deadlines=0; // Variable to count the number of missing deadlines
double context_switch=0.01; // Context switch
typedef struct process  // Struct that as the values related to a process
{
    int ProcessID;      // Store process ID 
    double Bursttime;   // Burst time
    int Period;         // Period of process
    int K;              // Number of instances of process
    int K_Backup;       // Backup of number of instances of process
    double NextDeadline;// Next dead line
    double RemainingBurstTime; // Remaining burst time
    double StartTime;   // Start time 
    double WaitingTime; // Overall waiting time
    bool prempted;      // Boolean variable to keep track of premption
}process;

struct Compare          // Custom operator that does ordering in priority queue
{
    bool operator() (pair<int,double>a, pair<int,double>b)
    {	
        if(a.second!=b.second) return a.second > b.second;
        else return a.first > b.first; // If same periods, then take the process with small pid
    }
};

// Event queue : It contains the list of processes that will enter the ready queue
// Ready queue : It contains the list of processes that can be scheduled
priority_queue<pair<int,double>,vector<pair<int,double>>,Compare>event_queue,ready_queue; // Priority queues that deal with scheduling 

void edf_scheduling(process P[])   // Function that performs edf_scheduling on the given list of processes
{
    FILE* log_ptr;        // File pointer to handle file operations
    log_ptr=freopen("EDF-CS-Log.txt","w+",stdout);    // Creating a new txt file to write the outputs      
    if(log_ptr==NULL)     // If there is error while open the file then this block gets executed
    {
        printf("Couldn't open EDF-CS-Log.txt file \n");
        exit(1);                        
    }

    for(int i=0;i<n;i++)
    {
        cout<<"Process P"<<P[i].ProcessID;
        cout<<"; processing time="<<P[i].Bursttime;
        cout<<"; deadine:"<<P[i].NextDeadline;
        cout<<"; period:"<<P[i].Period;
        cout<<" joined the system at time "<<current_time<<endl;
        if(P[i].K>0)      // Initially we are pushing the first instance of all the processes into the event queue
        {
            event_queue.push({P[i].ProcessID,current_time});
        }
    }
    while (!event_queue.empty() || !ready_queue.empty())   // Keep running the loop until at least one of the queues is not empty
    {
        if(!event_queue.empty() && abs(event_queue.top().second-current_time)<1e-4)   // If the top element in the event queue is equal to the current time
        {
            ProcessID=event_queue.top().first;  // Take the top element's process ID
            P[ProcessID-1].NextDeadline = current_time + P[ProcessID-1].Period;  // Setting the nextdeadline for this process ID
            if(P[ProcessID-1].K>0)
            {
                if(P[ProcessID-1].RemainingBurstTime != 0.0)     // The process entered the event_queue but couldn't finish the task
                {
                    missed_deadlines++;                                     // Hence we take that as missed deadline
                    P[ProcessID-1].WaitingTime+=P[ProcessID-1].Period;      // Set the waiting time
                    P[ProcessID-1].StartTime = current_time;                // Setting the start time to be the current time
                    cout<<fixed << setprecision(5)<<"Process P"<<ProcessID<<" missed deadline at time "<<current_time<<endl;
                    ready_queue.pop();
                    ready_queue.push({ProcessID,P[ProcessID-1].NextDeadline}); // Hence we push it into the ready queue
                }
                else    // Remaining burst time is zero means that the process still didn't enter the ready queue
                {
                    P[ProcessID-1].StartTime = current_time;
                    ready_queue.push({ProcessID,P[ProcessID-1].NextDeadline}); // Hence we push it into the ready queue
                }
            }
            P[ProcessID-1].RemainingBurstTime = P[ProcessID-1].Bursttime;      // Here we set the remaining burst time equal to that of total burst time
            P[ProcessID-1].K--;                                                // As we already added the element/removed(if deadline is not passed) we decrement the K value
            event_queue.pop();
            if(P[ProcessID-1].K>0)
            {
                event_queue.push({ProcessID,P[ProcessID-1].NextDeadline});     // If there are more instances of this process then add them into the event queue
            }
            continue;
        }
        if(!ready_queue.empty())    // If there are processes already in ready queue that are to scheduled
        {
            ProcessID=ready_queue.top().first;  // Take the first element's process ID
            if(abs(P[ProcessID-1].RemainingBurstTime-P[ProcessID-1].Bursttime)<1e-4)  // If the remaining burst time is full then we immediatly schedule this process as this was not yet scheduled till now
            {
                if(current_time+P[ProcessID-1].RemainingBurstTime > P[ProcessID-1].NextDeadline)
                {
                    missed_deadlines++;        // It means that it didn't pass the deadline
                    P[ProcessID-1].WaitingTime+=P[ProcessID-1].Period; // Set the waiting time 
                    P[ProcessID-1].RemainingBurstTime = 0;             // As the process missed it's deadline, set the remaining burst time to be 0
                    cout<<fixed << setprecision(5)<<"Even if Process P"<<ProcessID<<" is scheduled now, it would miss it's deadline at time "<<P[ProcessID-1].NextDeadline<<endl;
                    P[ProcessID-1].NextDeadline=INT_MAX;
                    ready_queue.pop();        // As this instance is completed, we pop it off
                    continue;
                }
                current_time+=context_switch; // Ading the context switch
                cout<<fixed << setprecision(5)<<"Process P"<<ProcessID<<" starts executing at time "<<current_time<<endl;   
                P[ProcessID-1].prempted=false;
            }
            else if(P[ProcessID-1].prempted)  // If this process is the one which was prempted by some other process earlier
            {
                if(current_time+P[ProcessID-1].RemainingBurstTime > P[ProcessID-1].NextDeadline)
                {
                    missed_deadlines++;       // It means that it didn't pass the deadline
                    P[ProcessID-1].WaitingTime+=P[ProcessID-1].Period;       // Set the waiting time 
                    P[ProcessID-1].RemainingBurstTime = 0;                   // As the process missed it's deadline, set the remaining burst time to be 0      // As the process missed it's deadline, set the remaining burst time to be 0
                    cout<<fixed << setprecision(5)<<"Even if Process P"<<ProcessID<<" is scheduled now, it would miss it's deadline at time "<<P[ProcessID-1].NextDeadline<<endl;
                    P[ProcessID-1].NextDeadline=INT_MAX; 
                    ready_queue.pop();        // As this instance is completed, we pop it off
                    continue;
                }
                current_time+=context_switch; // Ading the context switch
                cout<<fixed << setprecision(5)<<"Process P"<<ProcessID<<" resumes executing at time "<<current_time<<endl;
                P[ProcessID-1].prempted=false;
            }

            if(event_queue.empty())     // If there are no elements present in event queue, that can be sent into ready queue next
            {
                if(P[ProcessID-1].RemainingBurstTime+current_time<=P[ProcessID-1].NextDeadline) // Process is completed in this case
                {
                    P[ProcessID-1].WaitingTime+=(current_time-P[ProcessID-1].StartTime+P[ProcessID-1].RemainingBurstTime-P[ProcessID-1].Bursttime); // Set the waiting time
                    current_time+=P[ProcessID-1].RemainingBurstTime;                        // Set the current time
                    P[ProcessID-1].RemainingBurstTime = 0;                                  // As the process completed executing, set the remaining burst time to be 0
                    P[ProcessID-1].NextDeadline+=P[ProcessID-1].Period;                     // Setting the next deadline
                    cout<<fixed << setprecision(5)<<"Process P"<<ProcessID<<" finishes executing at time "<<current_time<<endl;
                    ready_queue.pop();       // As this instance is completed, we pop it off
                }
                else
                {
                    missed_deadlines++;     // It means that it didn't pass the deadline
                    P[ProcessID-1].WaitingTime+=P[ProcessID-1].Period;                      // Set the waiting time
                    current_time = P[ProcessID-1].NextDeadline;                             // Set the current time
                    P[ProcessID-1].RemainingBurstTime = 0;                                  // As the process missed it's deadline, set the remaining burst time to be 0
                    P[ProcessID-1].NextDeadline+=P[ProcessID-1].Period;                     // Setting the next deadline
                    cout<<"Process P"<<ProcessID<<" missed deadline at time "<<current_time<<endl;
                    ready_queue.pop();      // As this instance is completed, we pop it off
                }
            }
            else // If there are elements present in the event queue that are ready to be pushed into ready queue
            {
                // If the current element to be scheduled from the event queue doesn't miss the deadline and the time taken by it is less than the time taken for the next process from event queue to enter ready queue
                if(P[ProcessID-1].RemainingBurstTime+current_time<=P[ProcessID-1].NextDeadline && event_queue.top().second >= P[ProcessID-1].RemainingBurstTime+current_time)
                {
                    P[ProcessID-1].WaitingTime+=(current_time-P[ProcessID-1].StartTime+P[ProcessID-1].RemainingBurstTime-P[ProcessID-1].Bursttime); // Set the waiting time
                    current_time+=P[ProcessID-1].RemainingBurstTime;                        // Set the current time
                    P[ProcessID-1].RemainingBurstTime = 0;                                  // As the process finshed it's deadline, set the remaining burst time to be 0
                    cout<<fixed << setprecision(5)<<"Process P"<<ProcessID<<" finishes executing at time "<<current_time<<endl;
                    P[ProcessID-1].NextDeadline+=P[ProcessID-1].Period;                     // Set the next deadline
                    ready_queue.pop();
                }
                // If the period of the to be scheduled element is greater than the top of the event queue(next element to be pushed to ready queue) period, the premption takes place
                else if(event_queue.top().second < P[ProcessID-1].NextDeadline)
                {
                    P[ProcessID-1].RemainingBurstTime -= event_queue.top().second-current_time; // Set the remanin burst time, as this process will keep running till the process from event queue enters into ready queue
                    current_time=event_queue.top().second;   // Set the current time
                    
                    if((P[event_queue.top().first-1].K!=0) && ((P[ProcessID-1].NextDeadline > P[event_queue.top().first-1].NextDeadline) || (event_queue.top().first < ProcessID && P[ProcessID-1].NextDeadline == P[event_queue.top().first-1].NextDeadline ))) // Checking the period, to see the priority
                    {
                        cout<<fixed << setprecision(5)<<"Process P"<<ProcessID<<" is prempted by Process P"<<event_queue.top().first<<" at time "<<current_time;
                        cout<<fixed << setprecision(5)<<" Remaining process time: "<<P[ProcessID-1].RemainingBurstTime<<endl;
                        P[ProcessID-1].prempted=true;       // Setting the prempt flag for this process
                    }
                }
                else
                {
                    missed_deadlines++;     // It means that it didn't pass the deadline
                    P[ProcessID-1].WaitingTime+=(current_time-P[ProcessID-1].StartTime+P[ProcessID-1].RemainingBurstTime-P[ProcessID-1].Bursttime); // Set the waiting time
                    current_time = P[ProcessID-1].NextDeadline;                             // Set the current time
                    P[ProcessID-1].RemainingBurstTime = 0;                                  // As the process missed it's deadline, set the remaining burst time to be 0
                    P[ProcessID-1].NextDeadline+=P[ProcessID-1].Period;                     // Set next deadine
                    cout<<fixed << setprecision(5)<<"Process P"<<ProcessID<<" missed deadline at time "<<current_time<<endl;
                    ready_queue.pop();     // As this instance is completed, we pop it off
                }
            }
        }
        else
        {
            if(!event_queue.empty())       // If there are element present in event queue to be scheduled, but no elements in ready queue then CPU is idle
            {
                current_time = event_queue.top().second;
                if(P[event_queue.top().first-1].K>0)
                {
                    cout<<fixed << setprecision(5)<<"CPU is idle till time "<<current_time<<endl;
                    continue;
                }
                else break;
            }
        }
    }
}
int main()
{
    FILE *input_ptr,*log_ptr,*stats_ptr;           // File pointers used for handling read, write operations in the files
    input_ptr=freopen("inp-params.txt","r",stdin); // Opening the inp.txt file

    if(input_ptr==NULL)     // If there is error while open the file then this block gets executed
    {
        printf("Couldn't open the mentioned file \n");
        exit(1);                        
    }
    cin>>n;
    int total_numberof_processes=0; // Total number of processes that entered the system
    process P[n];          // Array of strcuts of processes
    double avg_waiting_time=0; // Variable to calculate the average waiting time
    for(int i=0;i<n;i++)
    {
        cin>>P[i].ProcessID>>P[i].Bursttime>>P[i].Period>>P[i].K; // Reading input from the inp-params.txt file
        total_numberof_processes+=P[i].K;                         // Counting the number of processes
        P[i].NextDeadline = P[i].Period;                          // Setting the next deadline to be equal to the period
        P[i].RemainingBurstTime = 0;                              // Setting the remaining burst time to be 0, as the process is not scheduled yet 
        P[i].WaitingTime = 0;                                     // Setting the waiting time to be 0
        P[i].K_Backup = P[i].K;
    }
    fclose(input_ptr);

    edf_scheduling(P);          // Calling edf scheduling on the processes

    stats_ptr=freopen("EDF-CS-Stats.txt","w+",stdout);  // Creating a new txt file to write the outputs      
    if(stats_ptr==NULL)                     // If there is error while open the file then this block gets executed
    {
        printf("Couldn't open EDF-CS-Stats.txt file \n");
        exit(1);                        
    }

    cout<<"Number of processes that came into the system "<<total_numberof_processes<<endl;
    cout<<"Number of processes that successfully completed "<<total_numberof_processes-missed_deadlines<<endl;
    cout<<"Number of processes that missed their deadlines "<<missed_deadlines<<endl<<endl;
    cout<<"Average Waiting time of individual process :"<<endl;
    for(int i=0;i<n;i++)    // Calculating average waiting times
    {
        P[i].WaitingTime/=P[i].K_Backup;
        cout<<fixed << setprecision(5)<<"Process "<<P[i].ProcessID<<" Waiting time - "<<P[i].WaitingTime<<endl;
        avg_waiting_time+=P[i].WaitingTime;
    }
    avg_waiting_time/=n;
    cout<<fixed << setprecision(5)<<endl<<"Average Waiting time of all the process "<<avg_waiting_time<<endl;

    return 0;
}