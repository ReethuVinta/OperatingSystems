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
int *colorofVertex;     // Array to mark the color of vertices

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
    colorofVertex = new int [N];

    for(int i=0;i<N;i++)               // Scanning the graph
    {
        for(int j=0;j<N;j++)
        {
            fscanf(input_ptr,"%d",&graph[i][j]);
        }
    }

    // Saving start time
    long long int start = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();     
    vector<int>colorTemp;
    int color;
    for(int i=0;i<N;i++)
    {
        colorTemp.clear();
        for(int j=0;j<N;j++)
        {
            if(graph[i][j])                         // If there is an edge and the vetex is already colored
            {
                if(colorofVertex[j]!=0)
                {
                    colorTemp.push_back(colorofVertex[j]);  // Push the color
                }
            }
        }
        color = 1;                                  // Colors start from 1

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
        colorofVertex[i]=color;                     // Set the color  
    }
    // Saving end time
    long long int end = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count(); 

    FILE *output = freopen("output.txt","w+",stdout); // Opening the output.txt file
    cout<<"Sequential"<<endl;
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