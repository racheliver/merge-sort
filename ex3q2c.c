// Client code in C to sort the array
#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netdb.h>
#include <error.h>
#include <stdlib.h>
#include<errno.h> //For errno - the error number
#include<netdb.h> //hostent
#include<arpa/inet.h>
int hostname_to_ip(char * hostname , char* ip);
void mergeSort(int arr[], int l, int r);
void merge(int arr[], int l, int m, int r);
/*In this program, we will demonstrate the communication between processes sitting on different computers.
 * Client side: will need to get details about the port server and IP address
 * */


// Driver code
int main(int argc, char* argv[])
{
    int sock;
    struct hostent *server1;
    struct sockaddr_in server;
    int server_reply[2];int buffer[256];char localhost[256];

    int message[2],read_size;
    /*checking the input correct*/
    if (argc<3)
    {
        printf("please use: ./ex3q2c <hostname> <port>\n");
        exit(0);
    }
    if (strcmp(argv[1],"localhost")!=0)
    {
        puts("couldn't resolve host name: Success");
        exit(0);
    }
    char *hostname = argv[1];
    char ip[100];
    /*change the host name to ip*/
    hostname_to_ip(hostname , ip);
    // printf("%s resolved to %s" , hostname , ip);


    // Create socket
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == -1) {
        printf("Could not create socket");
    }

    //printf("my sock client: %d\n",sock);
    server1 = gethostbyname(argv[1]);
    if (server1 == NULL)
        perror("ERROR, no such host");

    server.sin_family = AF_INET;
    bcopy((char *)server1->h_addr, (char *)  &server.sin_addr.s_addr, server1->h_length);
    server.sin_port = htons(atoi(argv[2]));

    // Connect to remote server
    if (connect(sock, (struct sockaddr*)&server, sizeof(server)) < 0) {
        perror("connect failed. Error");
        return 1;
    }

    //puts("Connected\n");
    //sending the ip of the computer to server
    write(sock, &ip, 9*sizeof(char));
    int sizeOfArray=0,n;
    //read the size that we will need to sort
    n = read(sock, buffer,sizeof(int));//
    if (n < 0)
        perror("ERROR reading from socket");
    else if (n == 0)
        perror("ERROR reading from socket, no connection");


    // Receive a message(arrays) from server
    while ((read_size = recv(sock, &message,buffer[0] * sizeof(int), 0)) > 0) {
        printf("Array recived: ");
        for (int i = 0; i <buffer[0]; i++) {
            if (i<buffer[0]-1)
                printf("%d,", message[i]);
            else
                printf("%d", message[i]);
        }printf("\n");
        mergeSort(message, 0, buffer[0]-1);//0 -1
        printf("Array Send: ");
        for (int i = 0; i <buffer[0]; i++) {
            if (i<buffer[0]-1)
                printf("%d,", message[i]);
            else
                printf("%d", message[i]);
        }printf("\n");

        write(sock, &message, buffer[0]* sizeof(int));
        exit(0);
    }

    if (read_size == 0) {
        //puts("server disconnected\n");
    }
    else if (read_size == -1) {
        perror("recv failed\n");
    }
    return 0;
}


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

int hostname_to_ip(char * hostname , char* ip)
{
    struct hostent *he;
    struct in_addr **addr_list;
    int i;

    if ( (he = gethostbyname( hostname ) ) == NULL)
    {
        // get the host info
        herror("gethostbyname");
        return 1;
    }

    addr_list = (struct in_addr **) he->h_addr_list;

    for(i = 0; addr_list[i] != NULL; i++)
    {
        //Return the first one;
        strcpy(ip , inet_ntoa(*addr_list[i]) );
        return 0;
    }

    return 1;
}
