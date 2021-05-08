Instructions to execute the file :

1) First create a file named "inp-params.txt" and give the data inside it.
   Format -
   n k lambda1 lambda2
   Here n -> Number of threads
        k -> Number of instances
        lambda1 -> delay values to be exponentially distributed with an average (in milliseconds)
        lambda2 -> delay values to be exponentially distributed with an average (in milliseconds)
   This file should be placed in the same directory where .cpp files are present

2) Open the terminal :
    a) To compile TAS file :
        g++ -pthread -o TAS SrcAssgn3-TAS-ES18BTECH11028.cpp
        ./TAS

    b) To compile CAS file :
        g++ -pthread -o CAS SrcAssgn3-CAS-ES18BTECH11028.cpp
        ./CAS

    c) To compile CASBounded file :
        g++ -pthread -o CASBounded SrcAssgn3-CASBounded-ES18BTECH11028.cpp
        ./CASBounded
    
3) Log will be created in the same directory that the code is run.

4) Statements are printed in sorted time order.

5) For drawing graph, I took lambda1 = 20, lambda2 = 20, k = 10 and changed value of n from 10 to 50 
   by increasing it by 10 everytime.