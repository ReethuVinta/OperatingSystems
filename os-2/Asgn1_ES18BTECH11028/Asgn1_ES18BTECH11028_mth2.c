#include <stdio.h>             // Importing libraries
#include <stdlib.h>
#include <math.h>
#include <pthread.h> 
#include <time.h> 

int *A;         // Declaring array globally
int p,n;        // p denotes the number of threads, n denotes the number of elements present in the array
typedef struct indices // Structure defined to stores indices
{
    int low;
    int high;
    int mid;
}indices;
void swap(int i,int j) // Function that swap the values present at i'th,j'th indices
{
    int temp;
    temp=A[i];
    A[i]=A[j];
    A[j]=temp;
}
int partition(int low,int high)// Function the find the partition
{
    int temp;
    int pivot = A[high];       // Pivot is the high'th element 
    int i = low-1;
    for(int j=low;j<high;j++)  // Iterate from low to high
    {
        if(A[j]<pivot)         // If value is less that pivot then swap with i
        {   
            i++;
            swap(i,j);         // Swapping
        }
    }
    swap(i+1,high);            // Swapping
    return i+1;                // Return the partion indice
}
void quicksort(int low,int high)  // Function to perfrom quick sort
{
    if(low<high)
    {
        int p=partition(low,high);// Find the partition index
        quicksort(low,p-1);       // Now sort the left portion of the partition index
        quicksort(p+1,high);      // Sort the right portion of the partition index
    }
}
void *merge(void *params)         // Perform to merge arrays
{
    indices index = *((indices *) params); // Unwrapping
    int low=index.low;
    int high=index.high;
    int mid=index.mid;
    int left[mid-low+1];            // Array to store the values from low to mid
    int right[high-mid];            // Array to store the values from mid to high

    for(int i=0;i<mid-low+1;i++)    // Copying the values
    {
        left[i]=A[i+low];
    }

    for(int i=0;i<high-mid;i++)     // Copying the values
    {
        right[i]=A[i+mid+1];
    }

    int i=0;                        // Initial index of left subarray 
    int j=0;                        // Initial index of right subarray 
    int k=low;                      // Initial index of merged subarray 

    while(i<mid-low+1 && j<high-mid)
    {
        if(left[i]<=right[j])
        {
            A[k++]=left[i++];
        }
        else
        {
            A[k++]=right[j++];
        }
    }
    while(i<mid-low+1)
    {
        A[k++]=left[i++];
    }
    while(j<high-mid)
    {
        A[k++]=right[j++];
    }
    pthread_exit(0);                // Terminating the current thread
}

void Merging() // Function that performs parallel merging
{
    int size_of_segment=n/p;            // Size of segment is tracked with this variable
    int current_thread_count=p;         // Variable that keeps track of number of threads to be executed in the current level of merging
    while(size_of_segment!=n)           // Iterate untill the size of segment is equal to n
    {
        current_thread_count=current_thread_count/2;       // Every round the number of threads will be halved
        size_of_segment=size_of_segment*2;                 // Every round the size of segment will be halved

        pthread_t threads[current_thread_count];           // Declaring thread array of size current_thread_count
        pthread_attr_t attr[current_thread_count];         // Contents are used at thread creation time to determine attributes for the new thread

        for(int i=0;i<current_thread_count;i++)            // Setting the attributes
        {
            pthread_attr_init(&attr[i]);
        }
        for(int i=0;i<current_thread_count;i++)            // Creating and passing the function to the thread
        {
            indices index;
            index.low=i*size_of_segment;
            index.high=index.low+size_of_segment-1;
            index.mid=index.low+(index.high-index.low)/2;
            indices *current_index = malloc(sizeof(indices));
            *current_index=index;
            pthread_create(&threads[i],&attr[i],merge,current_index); //Creating the thread passing the function
        }
        for (int i = 0; i < current_thread_count; i++) 
        {
            pthread_join(threads[i], NULL); // Joining the terminated threads 
        }
    }                           
}
void *ParallelSort(void *params)        // Function that performs sorting parallely
{
    int current_thread = *((int *) params); // Retrieve the value of current segment
    int low=current_thread*(n/p);           // Calculating the extreme indices of the current segment
    int high=((current_thread+1)*(n/p))-1;  // Calculating high
    quicksort(low,high);                    // Calling quicksort between the low and high index
    pthread_exit(0);                        // Terminating the current thread
}

int main()
{
    FILE *fileptr,*opfileptr;
    fileptr = freopen("inp.txt","r",stdin); // Opening the inp.txt file

    if(fileptr == NULL)                     // If there is error while open the file then this block gets executed
    {
        printf("Couldn't open the mentioned file \n");
        exit(1);                        
    }

    struct timeval start_time,end_time;     // Stores time when declared
    fscanf(fileptr,"%d", &n);       // Scanning n,p values from the input file
    fscanf(fileptr,"%d", &p);

    n=pow(2,n);                     // 2^n
    p=pow(2,p);                     // 2^p
     
    A=(int*)malloc(sizeof(int)*n);  // Allocating memory for the array

    for(int i=0;i<n;i++)            // Generating the random values for the array
    {
        A[i]=rand()%100;
    }
    fclose(fileptr);                // Closing the inp.txt file

    opfileptr = freopen("output.txt","w+",stdout); // Creating and opening output.txt to put our result
    if(opfileptr==NULL)             // If there is error while open the file then this block gets executed
    {
        printf("Couldn't open file named output.txt \n");
        exit(1);
    }

    for(int i=0;i<n;i++)            // Printing the array to the output file
    {
        printf("%d ",A[i]);
    }
    printf("\n");

    gettimeofday(&start_time, NULL); // Save the current time 
    pthread_t threads[p];            // Declaring thread array
    pthread_attr_t attr[p];          // Contents are used at thread creation time to determine attributes for the new thread

    for(int i=0;i<p;i++)
    {
        pthread_attr_init(&attr[i]);
    }
    // Creating p(here p is 2^p) threads
    for(int i=0;i<p;i++)
    {
        int *current_thread = malloc(sizeof(i));
        *current_thread=i;
        pthread_create(&threads[i],&attr[i],ParallelSort,current_thread); //Creating the thread passing the function
    }
    for (int i = 0; i < p; i++) 
    {
        pthread_join(threads[i], NULL); // Joining the terminated threads 
    }
    Merging();                       // Calling merging function that performs merging       
    gettimeofday(&end_time, NULL);   // Save the current time

    for(int i=0;i<n;i++)             // Printing the sorted array values into output.txt file
    {
        printf("%d ",A[i]);
    }
    printf("\n");
    // Calculating the elapsed time
    double timetaken = (end_time.tv_sec-start_time.tv_sec + (end_time.tv_usec-start_time.tv_usec)*0.000001); // Finding out the difference between start and end time
    printf("Time Taken: %0.3f Microseconds",timetaken*1e6);

    fclose(opfileptr);  // Closing the output.txt file

    return 0;
}