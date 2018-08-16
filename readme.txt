It is necessary to open the file with a notepad or WordPad

user name:rachelive
name:Racheli  Id:305710071

the program implement the sorting algorithm, "merge sort" in the distributed form (parallel calculation).

this project have 2 parts:
1) Decentralization of creating processes by share memory
2) client-server calling by socket.

In the first section: different processes W

***********what this program do************ill take part in sorting the array, 
when communication between them will be carried out with the help of shared memory.

Second: between processes that can run on different computers
 and communication between them (sorting the array) will be done using a socket.
 


********** How to Compile this program*************

go to the file location and enter 'make'.


***********How to run this program***********

 ***********file.txt is *needed* to run the program************
 **The input for each of the questions in this exercise will be a file with the following three lines:**
 1) First line - the number of numbers to be sorted
 2) A strong number of  "The level of the mix". 
 3) the array numbers to be sorted
 **************************************************************

to run the share memory processes:
enter: ./ex3q1  (##) 


to run the socket client-server:
server - enter: ./ex3q1s (##) (###) 
client - enter: ./ex3q1c localhost (###)


(##) - Enter the name of the TXT file (if in the ex3q1.c folder) Otherwise, enter the path to the file/
(###) - enter the same number of port (1024+) at the server and at the client


**********Description of program files*************

# ex3q1.c - share memory 
# ex3q2c.c - socket client 
# ex3q2s.c - socket server 
# Makefile - compile all the programs

**********Description What input we needed*************

only the file.txt that Mentioned earlier
	
***********An explanation of the output we get************

the program print the sorted array to the screan	


