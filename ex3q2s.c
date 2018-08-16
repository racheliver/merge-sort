// Server code in C to sort the array
#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <errno.h>
#include <fcntl.h>
#include <stdbool.h>
#include <wait.h>
#include <error.h>
//the array number for the input 
//the array number2 for the output 
static int* number;
static int* number2;
void mergeSort(int arr[], int l, int r);
void merge(int arr[], int l, int m, int r);
int hostname_to_ip(char * hostname , char* ip);

// Driver code
int main(int argc, char* argv[])
{
//checking the input correct
    if (argc<3)
    {
        printf("please use: ./ex3q2s <filename> <port>\n");
        exit(0);
    }
//opening file
    int lentgh, level,i;
    int buffer[256];
    FILE *f = fopen(argv[1],"r");
	if(f==NULL)
	{
	puts("error opening file: No such file or directory");
	exit(1);
	}

    fscanf(f,"%d",&lentgh);
    fscanf(f,"%d",&level);
    number = mmap(NULL, sizeof(number)*lentgh, PROT_READ | PROT_WRITE,
                  MAP_SHARED | MAP_ANONYMOUS, -1,0);

    number2 = mmap(NULL, sizeof(number)*lentgh, PROT_READ | PROT_WRITE,
                   MAP_SHARED | MAP_ANONYMOUS, -1,0);

    for (int i=0;i<lentgh;i++)
    {
        number[i]=0;
        fscanf(f,"%d,",&number[i]);
    }
    printf("Amount of numbers that sort: %d\n",lentgh);
    printf("Degree of parallelism: %d\n",level);
    printf("before sort: ");
    for (int i=0;i<lentgh;i++)
    {
        if(i!=lentgh-1)
            printf("%d,",number[i]);
        else
            printf("%d\n",number[i]);
    }
//close the file
	fclose(f);



    int socket_desc, c, read_size; int client_sock[level];
    struct sockaddr_in server, client;
    int client_reply[lentgh];


    // Create socket
    socket_desc = socket(AF_INET, SOCK_STREAM, 0);

    if (socket_desc == -1) {
        printf("Could not create socket");
    }
    //puts("Socket created");


    // Prepare the sockaddr_in structure
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(atoi(argv[2]));

    // Bind the socket
    if (bind(socket_desc, (struct sockaddr*)&server, sizeof(server)) < 0) {

        // print the error message
        perror("bind failed. Error");
        return 1;
    }
    //puts("bind done");

    // lsiten to the socket
    listen(socket_desc, level);
    int j = 0;int  h = 0;
    int *ptr = number;
    int *ptr2= number2;
    int sizeCopy=lentgh/level;
    while(j<level)
    {


        pid_t  pid ;

        //puts("Waiting for incoming connections...");
        c = sizeof(struct sockaddr_in);

        // accept connection from an incoming client

        client_sock[j] = accept(socket_desc, (struct sockaddr*)&client, (socklen_t*)&c);

        if (client_sock[j] < 0) {
            perror("accept failed");
            return 1;
        }
        char ip[100];
        if (recv(client_sock[j], &ip,  sizeof(char)*9, 0) < 0) {
            puts("recv failed");
            return 0;
        }
        printf("Got request from %s\n",ip);
        int n;buffer[0]=sizeCopy;
        // puts("Connection accepted");
        printf("sending '%d' numbers to socket '%d' \n",buffer[0],client_sock[j]);
	//sending the size of arrays
            n = write(client_sock[j],buffer,sizeof(int));
	//sending the arrays
            if (n < 0)
                perror("ERROR writing to socket");
            if (send(client_sock[j], ptr, sizeCopy* sizeof(int), 0) < 0) {
                puts("Send failed");
                return 1;
            }

            // Receive a reply from the server
            if (recv(client_sock[j], &client_reply, sizeCopy * sizeof(int), 0) < 0) {
                puts("recv failed");
                return 0;
            }

            // puts("client reply :\n");
   
		//copy the input using pointers Arithmetic to the arrays
            for (int k = 0; k <sizeCopy ; k++) {
                *(ptr2 + k) =  client_reply[k];
            }


            ptr = ptr +sizeCopy;
            ptr2 = ptr2 +sizeCopy;
      
            j++;




       
    }
	
    close(socket_desc);
    int replay =0;int avg;
    int mergeCouples=2*sizeCopy;
	//merge in the server the rest of the sorted arrays
    while (mergeCouples!=lentgh)
    {

        while (replay!=lentgh)
        {
            avg = (2*replay+mergeCouples)/2;
            merge(number2,replay,avg-1,replay+mergeCouples-1);

            replay=replay+mergeCouples;
        }
        replay=0;
        mergeCouples=2*mergeCouples;
    }

    if(mergeCouples==lentgh)
    {

        merge(number2,0,(lentgh/2)-1,lentgh-1);
    }

    for (int i=0;i<lentgh;i++)
    {
        if(i!=lentgh-1)
            printf("%d,",number2[i]);
        else
            printf("%d\n",number2[i]);
    }
    return 0;
}

// Function to sort the array
// Merges two subarrays of arr[].
// First subarray is arr[l..m]
// Second subarray is arr[m+1..r]
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

/* l is for left index and r is right index of the
   sub-array of arr to be sorted */
void mergeSort(int arr[], int l, int r)
{
    if (l < r)
    {
        // Same as (l+r)/2, but avoids overflow for
        // large l and h
        int m = l+(r-l)/2;

        // Sort first and second halves
        mergeSort(arr, l, m);
        mergeSort(arr, m+1, r);

        merge(arr, l, m, r);
    }
}

/* UTILITY FUNCTIONS */
/* Function to print an array */
void printArray(int A[], int size)
{
    int i;
    for (i=0; i < size; i++)
        printf("%d ", A[i]);
    printf("\n");
}


