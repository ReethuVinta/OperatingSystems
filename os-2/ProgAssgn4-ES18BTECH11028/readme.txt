Instructions to execute the file :

1) First create a file named "input.txt" and give the data inside it.
   Format -
   N X lambda r gamma
   Here n -> Number of threads
        X -> Number of seats at the table
        lambda -> delay value to be exponentially distributed with an average (in seconds)
        r -> double value which controls upper bound of unifrom distribution
        gamma -> delay values to be exponentially distributed with an average (in seconds)
   This file should be placed in the same directory where .cpp files are present

2) Open the terminal :
    a) To compile TAS file :
       g++ -o SrcAssgn4-ES18BTECH11028 SrcAssgn4-ES18BTECH11028.cpp -lpthread -lrt
       ./SrcAssgn4-ES18BTECH11028
    
3) Log will be created in the same directory that the code is run.

4) Statements are printed in sorted time order.

5) Graphs, 
   For first graph these are the values that I have considered
   X = 4 
   lambda = 0.4 
   r = 0.7 
   gamma = 0.2

   For second graph these are the values that I have considered
   N = 15
   lambda = 0.4
   r = 0.7 
   gamma = 0.2
