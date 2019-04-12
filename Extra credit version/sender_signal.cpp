//sender.cpp

#include <sys/shm.h>
//#include <sys/shared.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "shared.h"    /* For the message struct */
#include <iostream>
#include <fstream>
#include <signal.h>

using namespace std;
/*----------------------------------------------------------------------
IPC SHARED_MEMORY
ftok(): is use to generate a unique key.

shmget(): int shmget(key_t,size_tsize,intshmflg); upon successful completion, shmget() returns an identifier for the shared memory segment.


shmat(): Before you can use a shared memory segment, you have to attach yourself
to it using shmat(). void *shmat(int shmid ,void *shmaddr ,int shmflg);
shmid is shared memory id. shmaddr specifies specific address to use but we should set
it to zero and OS will automatically choose the address.

shmdt(): When you’re done with the shared memory segment, your program should
detach itself from it using shmdt(). int shmdt(void *shmaddr);

shmctl(): when you detach from shared memory,it is not destroyed. So, to destroy
shmctl() is used. shmctl(int shmid,IPC_RMID,NULL);
----------------------------------------------------------------------------------
IPC MESSAGE QUEUE
ftok(): is use to generate a unique key.

msgget(): either returns the message queue identifier for a newly created message
queue or returns the identifiers for a queue which exists with the same key value.

msgsnd(): Data is placed on to a message queue by calling msgsnd().

msgrcv(): messages are retrieved from a queue.

msgctl(): It performs various operations on a queue. Generally it is use to
destroy message queue.
---------------------------------------------------------------------------------*/
/* The size of the shared memory chunk */
#define SHARED_MEMORY_CHUNK_SIZE 1000

/* The ids for the shared memory segment and the message queue */
int shmid;

/* The pointer to the shared memory */
void* sharedMemPtr;

/**
 * Sets up the shared memory segment and message queue
 * @param shmid - the id of the allocated shared memory
 * @param msqid - the id of the shared memory
 */

void init(int& shmid, void*& sharedMemPtr)
{
// TODO:
//  1. Create a file called keyfile.txt containing string "Hello world"
 	ofstream outfile ("keyfile.txt");
	outfile << "Hello world" << endl;
	outfile.close();
// 2. Use ftok("keyfile.txt", 'a') in order to generate the key.
		key_t key = ftok("keyfile.txt", 'a');
    	//Check if the key is a valid key
	if(key < 0){
	 	perror("ftok");
		exit(-1);
	}

	/*  	3. Use the key in the TODO's below. Use the same key for the queue
		    and the shared memory segment. This also serves to illustrate the difference
	      between the key and the id used in message queues and shared memory. The id
		    for any System V objest (i.e. message queues, shared memory, and sempahores)
		    is unique system-wide among all SYstem V objects. Two objects, on the other hand,
		    may have the same key.*/

	/* TODO: Get the id of the shared memory segment. The size of the segment must be SHARED_MEMORY_CHUNK_SIZE */
	shmid = shmget(key,SHARED_MEMORY_CHUNK_SIZE,0666|IPC_CREAT);
  	// Check if the shared memory id is valid
	if(shmid < 0){
   	 	perror("shmget");
   	 	exit(-1);
  	}

	/* TODO: Attach to the shared memory */
	sharedMemPtr = (char*)shmat(shmid,(void*)0,0);
	// Check if shared memory pointer is valid
  	if(((void*)sharedMemPtr) < 0){
		perror("shmat");
		exit(-1);
	}
}

/**
 * Performs the cleanup functions
 * @param sharedMemPtr - the pointer to the shared memory
 * @param shmid - the id of the shared memory segment
 * @param msqid - the id of the message queue
 */

void cleanUp(const int& shmid, void* sharedMemPtr)
{
	/* TODO: Detach from shared memory */
	//shmdt(): When you’re done with the shared memory segment, your program should
	//detach itself from it using shmdt(). int shmdt(void *shmaddr);
  	if(shmdt(sharedMemPtr) < 0){
		perror("shmdt");
		exit(-1);
	}
}

/**
 * The main send function
 * @param fileName - the name of the file
 */
void send(const char* fileName)
{
	/* Open the file for reading */
	FILE* fp = fopen(fileName, "r");

	/* Was the file open? */
	if(!fp){
		perror("fopen");
		exit(-1);
	}

	/* Read the whole file */
	while(!feof(fp)){
		/* Read at most SHARED_MEMORY_CHUNK_SIZE from the file and store them in shared memory.
 		 * fread will return how many bytes it has actually read (since the last chunk may be less
 		 * than SHARED_MEMORY_CHUNK_SIZE).
 		 */
		if((size = fread(sharedMemPtr, sizeof(char), SHARED_MEMORY_CHUNK_SIZE, fp)) < 0){
			perror("fread");
			exit(-1);
		}

    //sends a signal to receive that data has been transfered
     signal(SIGUSR1,handler);


	} // end of send function


	/** TODO: once we are out of the above loop, we have finished sending the file.
 	  * Lets tell the receiver that we have nothing more to send. We will do this by
 	  * sending a message of type SENDER_DATA_TYPE with size field set to 0.
	  */
    	start_stop = 0;
	/* Close the file */
	fclose(fp);

}


int main(int argc, char** argv)
{

	/* Check the command line arguments */
	if(argc < 2){
		fprintf(stderr, "USAGE: %s <FILE NAME>\n", argv[0]);
		exit(-1);
	}

	/* Connect to shared memory and the message queue */
	init(shmid, sharedMemPtr);

	/* Send the file */
	send(argv[1]);

	/* Cleanup */
	cleanUp(shmid, sharedMemPtr);

	return 0;
}
