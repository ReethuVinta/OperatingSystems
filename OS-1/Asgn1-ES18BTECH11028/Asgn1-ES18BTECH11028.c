#include <stdio.h>       
#include <fcntl.h>        
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <unistd.h>       

int main(int argc, char** argv)
{
    if(argv[1]==NULL) // If no arguments are passed then exit the program with an error message
    {
        printf("Error! Please provide an argument. \n");
        return 0;
    }

    // Declaring all the variables
    const int SIZE = 512;                // Specifying size of the shared memory object
    const char *name = "OS_Assign1";     // Name of the shared memory object
    int fd;                              // File descriptor for shared memory object
    char *ptr;                           // Pointer to the shared memory object
    pid_t pid;                           // Creating a pid variable
    struct timeval start_time,end_time;  // Stores time when declared 
 
    fd = shm_open(name, O_CREAT | O_RDWR, 0666); // Create a shared memory object
    ftruncate(fd, SIZE);                 // Configuring the size of the shared memory object
    ptr = (char *)mmap(0, SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0); // Mapping the memory of shared memory object
    
    pid = fork();                        // Fork a child process 
    if (pid < 0)                         // Error occurred, if pid is less than zero 
    { 
        fprintf(stderr, "Fork Failed");
        return 1;
    }
    else if (pid == 0)                   // Child process, if pid is equal to 0
    { 
        gettimeofday(&start_time, NULL); // Save the current time
        sprintf(ptr,"%ld %ld",start_time.tv_sec,start_time.tv_usec); // Writing the current time to the created memory object  
        if(execvp(argv[1], argv+1) < 0)  // If execvp outputs 0, it couldn't execute the given command; else it executes it
        {
            printf("Could not execute the given command! \n");
        }
        exit(0);                         // child exits once it execute the given command 
    }
    else                                 // Parent process, if pid is greater than 0
    { 
        wait(NULL);                      // Wait till child process terminates 
        gettimeofday(&end_time, NULL);   // Save the current time
        long int start_sec,start_microsec;
        sscanf(ptr,"%ld %ld",&start_sec,&start_microsec); // Scanning the saved variables from memory object
        double elapsed = (end_time.tv_sec-start_sec + (end_time.tv_usec-start_microsec)*0.000001); // Finding out the difference between start and end time
        printf("Elapsed time: %lf\n",elapsed); //Printing the elapsed time to user
        shm_unlink(name);                // Unlinking the memory object
    }
    return 0;
}