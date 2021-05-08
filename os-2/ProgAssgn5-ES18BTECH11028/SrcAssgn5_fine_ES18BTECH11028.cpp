#include <iostream>     // Importing necessary libraries
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

// Global declrations
int **graph;            // Adjacency matrix of graph
int N,K;                // Number of vertices, Number of threads
bool *externalVertex;   // Boolean array that is used to denote if a vertex is a external vertex or not
int *colorofVertex;     // Array to mark the color of vertices
int *partitionIndex;    // Array to denote which partitition does the vertice belongs to
pthread_mutex_t *lock;  // Mutex locks

void* coloring(void *params)    // Thread funtion
{
    int thread_number = *((int *)params); // Retrieve the thread number
    vector<int>internal,external,colorTemp,locktemp;
    for(int i=0;i<N;i++)
    {
        if(partitionIndex[i]==thread_number)    // If the partition is equal to the current thread
        {
            if(externalVertex[i])               // If it is an external vertex push into external vector
            {
                external.push_back(i);
            }   
            else                                // If it is an internal vertex push into internal vector
            {
                internal.push_back(i);
            }
        }
    }
    // Coloring internal vertices
    int color;                                      // Variable used to color a vertex
    int internalVerticesCount = internal.size();    // Size of internal vector
    int externalVerticesCount = external.size();    // Size of external vector

    for(int i=0;i<internalVerticesCount;i++)        // Iterate through all the internal vertices
    {
        colorTemp.clear();
        for(int j=0;j<N;j++)                        // Take one element present in internal vector
        {
            if(graph[internal[i]][j])               // If there an edge
            {
                if(colorofVertex[j]!=0)             // And it is colored vertex
                {
                    colorTemp.push_back(colorofVertex[j]);  // Add into the vector
                }
            }
        }
        color = 1;                                  // COlors start from 1

        sort(colorTemp.begin(),colorTemp.end());    // Sorting alloted colors
        auto itr = colorTemp.begin();
        while(itr!=colorTemp.end())
        {
            if(*itr == color)                       // If there is already a vertex with same color then increment it
            {
                color++;
            }
            itr++;
        }
        colorofVertex[internal[i]]=color;           // Set the color
    }
    

    // Coloring external vertices
    for(int i=0;i<externalVerticesCount;i++)
    {
        colorTemp.clear();
        locktemp.clear();

        for(int j=0;j<N;j++)                // Find all the vertices that are connected to this external vetex
        {
            if(graph[external[i]][j])
            {
                locktemp.push_back(j);      // Push those vertices
            }   
        }
        locktemp.push_back(external[i]);    // Locking the vertex also

        sort(locktemp.begin(), locktemp.end()); // Sort the vector

        for(int i=0;i<locktemp.size();i++)
        {
            pthread_mutex_lock(&lock[i]);   // Acquire the locks
        }
        for(int j=0;j<N;j++)
        {
            if(graph[external[i]][j] && colorofVertex[j]!=0)    // If there is an edge and the vetex is already colored
            {
                colorTemp.push_back(colorofVertex[j]);          // Push the color
            }
        }

        color = 1;                                  // COlors start from 1
        sort(colorTemp.begin(),colorTemp.end());    // Sorting alloted colors
        auto itr = colorTemp.begin();
        while(itr!=colorTemp.end())
        {
            if(*itr == color)                       // If there is already a vertex with same color then increment it
            {
                color++;
            }
            itr++;
        }
        colorofVertex[external[i]]=color;           // Set the color             
        for(int i=0;i<locktemp.size();i++)
        {
            pthread_mutex_unlock(&lock[i]);         // Releasing the locks
        }   
    }
    pthread_exit(0);
}


int main()
{
    FILE *input_ptr;           // File pointers used for handling read, write operations in the files
    input_ptr=fopen("input_params.txt","r"); // Opening the input_params.txt file

    if(input_ptr==NULL)        // If there is error while open the file then this block gets executed
    {
        printf("Couldn't open the mentioned file \n");
        exit(1);                        
    }
    fscanf(input_ptr,"%d",&K);
    fscanf(input_ptr,"%d",&N);   // Taking inputs
    
    // Allocating memory to the global declarations
    graph = new int * [N];
    for (int i = 0; i < N; ++i )
    {
        graph[i] = new int [N];
    }
    externalVertex = new bool [N];
    colorofVertex = new int [N];
    partitionIndex = new int [N];
    lock = new pthread_mutex_t[N];
    
    for(int i=0;i<N;i++)
    {
        pthread_mutex_init(&lock[i],NULL);
    }

    for(int i=0;i<N;i++)        // Scanning the graph
    {
        for(int j=0;j<N;j++)
        {
            fscanf(input_ptr,"%d",&graph[i][j]);
        }
    }

    int partitionSize = N/K;        // Size of partition
    int index = 1;
    // Partition the vetices into size of N/K
    for(int i=0;i<N && index<=K;i+=partitionSize)
    {
        for(int j=i;j<i+partitionSize;j++)
        {
            partitionIndex[j]=index;
        }
        index++;
    }
    // If there are vertices left over add them to the last thread
    if(N%K!=0)
    {
        index--;
        for(int i=0;i<N;i++)
        {
            if(partitionIndex[i]==0)
            {
                partitionIndex[i]=index;
            }
        }
    }
    srand(time(0));
    unsigned seed = 0;
    random_shuffle(partitionIndex, partitionIndex + N); // random shuffling of partitions

    for(int i=0;i<N;i++)
    {
        for(int j=0;j<N;j++)
        {
            if(graph[i][j] && partitionIndex[i]!=partitionIndex[j]) // If there is an edge and partition index of both are diff 
            {
                externalVertex[i]=true;             // Then it is a external vertex
            }
        }
    }

    pthread_t threads[K];            // Declaring thread array
    pthread_attr_t attr[K];          // Contents are used at thread creation time to determine attributes for the new thread

    for(int i=0;i<K;i++)             // Initialsing threads attributes
    {
        pthread_attr_init(&attr[i]);
    }

    // Saving start time
    long long int start = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count(); 
    for(int i=0;i<K;i++)
    {
        int *thread_number = (int *)malloc(sizeof(int));
        *thread_number = i + 1;
        pthread_create(&threads[i], &attr[i], coloring, thread_number); //Creating the thread passing the function
    }
    for(int i=0;i<K;i++)
    {
        pthread_join(threads[i], NULL);         // Joining the terminated threads
    }
    // Saving end time
    long long int end = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count(); 

    FILE *output = freopen("output.txt","w+",stdout); // Opening the output.txt file
    cout<<"Fine-Grained Lock"<<endl;
    cout<<"--------------------"<<endl;

    int numberofColorsUsed = 0;
    for(int i=0;i<N;i++)
    {
        numberofColorsUsed = max(numberofColorsUsed, colorofVertex[i]);
    }
    cout<<"Number of colours used "<<numberofColorsUsed<<endl;
    cout<<"Time taken by alogorithm is "<<(end-start)<<" milliseconds"<<endl;
    cout<<"Colours :"<<endl;
    for(int i=0;i<N;i++)
    {
        cout<<"V"<<i+1<<" - "<<colorofVertex[i]<<endl;
    }
    
    return 0;
}