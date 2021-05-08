Instructions to execute the file :

1) First create a file named "inp-params.txt" and give the processes data inside it.
   Format -
   n -> Number of processes
   ProcessID BurstTime Period NumberofRepeats -> n lines with this order should be there.
   The order of ProcessID should go from 1 to n (It should be strictly in this order)
   This file should be placed in the same directory where .cpp files are present

2) Open the terminal :
   To compile RMS file :
    a) Without context switch code
        g++ -o RMS Assgn2-RMS-ES18BTECH11028.cpp
        ./RMS
    b) Extra credit - With context switch code :
        g++ -o RMS-CS Assgn2-RMS-CS-ES18BTECH11028.cpp
        ./RMS-CS

    To compile EDF file :
    a) Without context switch code
        g++ -o EDF Assgn2-EDF-ES18BTECH11028.cpp
        ./EDF
    b) Extra credit - With context switch code :
        g++ -o EDF-CS Assgn2-EDF-CS-ES18BTECH11028.cpp
        ./EDF-CS

3) Log, Stat files will be created in the same directory that the code is run

4) Test cases used for drawing the graph are present in "testcases" folder. 
   One testcase is with CPU Utilisation < 1 and the other one with CPU Utilisation >1

5) The outputs in the log file will be as follows
   Ex : Process P1 starts execution at time 0
        Process P1 finishes execution at time 20
        Process P2 starts execution at time 20  (It is not 21)
        Process P2 is preempted by Process P1 at time 50. Remaining processing time:5
        Process P1 starts execution at time 50
        Process P1 finishes execution at time 70
        Process P2 resumes execution at time 70
        Process P1 finishes execution at time 75