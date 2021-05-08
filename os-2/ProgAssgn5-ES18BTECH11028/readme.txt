Instructions to execute the file :

1) First create a file named "input_params.txt" and give the data inside it.
   Format -
   k n 
   n rows, n columns of graph
   Here k -> Number of instances
        n -> Number of threads
        graph is undirected and is stored in adjaceny matrix format.

   This file should be placed in the same directory where .cpp files are present

2) Open the terminal :
    a) To compile coarse-grained file :
        g++ -pthread -o coarse SrcAssgn5_coarse_ES18BTECH11028.cpp
        ./coarse

    b) To compile fine-grained file :
        g++ -pthread -o fine SrcAssgn5_fine_ES18BTECH11028.cpp
        ./fine

    c) To compile sequential file :
        g++ -pthread -o sequential SrcAssgn5_seq_ES18BTECH11028.cpp
        ./sequential
    
3) output.txt will be created in the same directory that the code is run.

4) For plots 1, 2 I have taken K = 50 and varied number of vertices in the graph 
   from 1*10^3 to 5*10^3 in the increment of 10^3.

   For plots 3, 4 I have taken N = 10^3 and varied number of threads from 
   10 to 50 in the increment of 5.