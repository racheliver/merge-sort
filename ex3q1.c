/* Merge sort in C */
#include<stdio.h>
#include<stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include<string.h>
#include<assert.h>
#include <pwd.h>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>
#include <sys/mman.h>
#include <errno.h>
#include <fcntl.h>
#include <stdbool.h>
#include <math.h>
/*
 *
 * 
 * 
 * In this Program we will implement the MarSort algorithm in a distributed way (parallel calculation).
 * Distortion will be performed between different processes when communication
 * between them will be done using a shared memory.
 * At the beginning of the program, the sorting code will appear, and after that the main.
 *
 *
 * */
// Function to Merge Arrays L and R into A.
// lefCount = number of elements in L
// rightCount = number of elements in R.
void mergeSort(int arr[], int l, int r,double level);
void merge(int arr[], int l, int m, int r);
void mergeSort(int arr[], int l, int r,double level)
{
    if (l < r)
    {
        // Same as (l+r)/2, but avoids overflow for
        // large l and h
        
        /*
         * The level parameter represents the path to the required level of mixing.
         * 
         * */
        int m = l+(r-l)/2;
        if(level!=1  ){

            // Sort first and second halves
            level--;
            mergeSort(arr, l, m,level);

            mergeSort(arr, m+1, r,level);

            merge(arr, l, m, r);
        }
/*
 *    * Then we will produce twoprocesses, left and right  
 *    one that will sort the left side and the other that will sort the right side of the array,
 *    and thus processes will be created according to the necessary level of mixing
 * 
 * */
        else {
            pid_t lpid, rpid;
            lpid=fork();

            if (lpid<0)
            {
                // Lchild proc not created
                perror("Left Child Proc. not created\n");
                exit(-1);
            }
            else if (lpid==0)
            {
                printf("Create a process: %d\n",getpid());
                mergeSort(arr,l,m,-1);
                exit(0);
            }

            else
            {
                rpid = fork();

                if (rpid<0)
                {
                    // Rchild proc not created
                    perror("Right Child Proc. not created\n");
                    exit(-1);
                }
                else if(rpid==0)
                {

                    printf("Create a process: %d\n",getpid());
                    mergeSort(arr,m+1,r,-1);
                    exit(0);
                }
            }
            int status;

            // Wait for child processes to finish

            waitpid(lpid, &status, 0);
            waitpid(rpid, &status, 0);

            // Merge the sorted subarrays
            merge(arr, l, m, r);

        }
    }
}
//-------------mergeSort-------------------------


//-------------merge-------------------------
void merge(int arr[], int l, int m, int r)
{

    int i, j, k;
    int n1 = m - l + 1;
    int n2 =  r - m;

    /* create temp arrays */
    int L[n1], R[n2];

    /* Copy data to temp arrays L[] and R[] */
    for (i = 0; i < n1; i++)
        L[i] = arr[l + i];
    for (j = 0; j < n2; j++)
        R[j] = arr[m + 1+ j];

    /* Merge the temp arrays back into arr[l..r]*/
    i = 0; // Initial index of first subarray
    j = 0; // Initial index of second subarray
    k = l; // Initial index of merged subarray
    while (i < n1 && j < n2)
    {
        if (L[i] <= R[j])
        {
            arr[k] = L[i];
            i++;
        }
        else
        {
            arr[k] = R[j];
            j++;
        }
        k++;
    }

    /* Copy the remaining elements of L[], if there
       are any */
    while (i < n1)
    {
        arr[k] = L[i];
        i++;
        k++;
    }

    /* Copy the remaining elements of R[], if there
       are any */
    while (j < n2)
    {
        arr[k] = R[j];
        j++;
        k++;
    }



}
//-------------merge-------------------------
static int *A;//array to shm
int main(int argc ,char* argv[]) {
/*
 * Check that the input is valid for running the program,
 * then open the file and extract its details into the program.
 * The calculation  level of the user's query will be done by the log2 function .
 * sending the shared memory array A to mergesort
 *
 *
 *
 * */

    if (argc<2)
    {
        puts("please use: ./ex3q1 <filename>");
        exit(1);
    }
    int lentgh;int level;
    FILE *f= fopen(argv[1],"r");
    if(f==NULL)
    {
        puts("error opening file: No such file or directory");
        exit(1);
    }

    fscanf(f,"%d",&lentgh);
    fscanf(f,"%d",&level);
    A= mmap(NULL, (sizeof *A), PROT_READ | PROT_WRITE,
            MAP_SHARED | MAP_ANONYMOUS, -1, 0);


    int logF=(int)log2(level);

    for (int i=0;i<lentgh;i++)
    {
        A[i]=0;
        fscanf(f,"%d,",&A[i]);
    }
    printf("Amount of numbers that sort: %d\n",lentgh);
    printf("Degree of parallelism: %d\n",level);
    printf("Before sort: ");
    for (int i=0;i<lentgh;i++)
    {
        if(i!=lentgh-1)
            printf("%d,",A[i]);
        else
            printf("%d\n",A[i]);
    }


    fclose(f);
    // Calling merge sort to sort the array.
    mergeSort(A,0,lentgh-1,logF);

    //printing all elements in the array once its sorted.
    printf("After  sort:");
    for(int i = 0;i <lentgh;i++) printf("%d ",A[i]);
    printf("\n");
    return 0;
}

