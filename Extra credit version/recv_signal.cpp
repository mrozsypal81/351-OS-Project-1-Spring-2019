//recv.cpp
#include <sys/shm.h>
//#include <sys/msg.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "shared.h"    /* For the message struct */
#include <iostream>
#include <fstream>

using namespace std;

/* The size of the shared memory chunk */
#define SHARED_MEMORY_CHUNK_SIZE 1000

/* The ids for the shared memory segment and the message queue */
int shmid;

/* The pointer to the shared memory */
void *sharedMemPtr;

/* The name of the received file */
const char recvFileName[] = "recvfile";


/**
 * Sets up the shared memory segment and message queue
 * @param shmid - the id of the allocated shared memory
 * @param msqid - the id of the shared memory
 * @param sharedMemPtr - the pointer to the shared memory
 */

void init(int& shmid, void*& sharedMemPtr)
{

/* TODO: 1. Create a file called keyfile.txt containing string "Hello world" (you may do
 	so manually or from the code).
			*/
	ofstream outfile ("keyfile.txt");
	outfile << "Hello world" << endl;
	outfile.close();
// 2. Use ftok("keyfile.txt", 'a') in order to generate the key.
	key_t key = ftok("keyfile.txt", 'a');
/* 3. Use the key in the TODO's below. Use the same key for the queue
	and the shared memory segment. This also serves to illustrate the difference
	between the key and the id used in message queues and shared memory. The id
	for any System V object (i.e. message queues, shared memory, and sempahores)
	is unique system-wide among all System V objects. Two objects, on the other hand,
	may have the same key.
 */
	/* TODO: Allocate a piece of shared memory. The size of the segment must be SHARED_MEMORY_CHUNK_SIZE. */
	shmid = shmget(key,SHARED_MEMORY_CHUNK_SIZE,0666|IPC_CREAT);
	//check if shared memory id is valid
	if(shmid < 0){
		perror("shmget");
		exit(-1);
	}
	/* TODO: Attach to the shared memory */
	sharedMemPtr = (char*)shmat(shmid,(void*)0,0);
	//check if shared memory pointer is valid
	if(((void*)sharedMemPtr) < 0){
		perror("shmat");
		exit(-1);
	}
}


/**
 * The main loop
 */
void mainLoop()
{
	/* Open the file for writing */
	FILE* fp = fopen(recvFileName, "w");

	/* Error checks */
	if(!fp)
	{
		perror("fopen");
		exit(-1);
	}

    /* TODO: Receive the message and get the message size. The message will
     * contain regular information. The message will be of SENDER_DATA_TYPE
     * (the macro SENDER_DATA_TYPE is defined in msg.h).  If the size field
     * of the message is not 0, then we copy that many bytes from the shared
     * memory region to the file. Otherwise, if 0, then we close the file and
     * exit.
     *
     * NOTE: the received file will always be saved into the file called
     * "recvfile"
     */

	/* Keep receiving until the sender set the size to 0, indicating that
 	 * there is no more data to send
 	 */

	while(start_stop != 0)
	{
		/* If the sender is not telling us that we are done, then get to work */
		int received = size;
		if(received != 0){
			/* Save the shared memory to file */
			if(fwrite(sharedMemPtr, sizeof(char), received, fp) < 0){
				perror("fwrite");
			}

			/* TODO: Tell the sender that we are ready for the next file chunk.
 			 * I.e. send a message of type RECV_DONE_TYPE (the value of size field
 			 * does not matter in this case).
 			 */
		 	 }
			 signal(SIGUSR2,handler);
		}
		/* We are done */
			/* Close the file */
			fclose(fp);
	}




/**
 * Perfoms the cleanup functions
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
	/* TODO: Deallocate the shared memory chunk */
	//shmctl(): when you detach from shared memory,it is not destroyed. So, to destroy
	//shmctl() is used. shmctl(int shmid,IPC_RMID,NULL);
	if(shmctl(shmid, IPC_RMID, NULL) < 0){
		perror("shmctl");
		exit(-1);
	}
}

/**
 * Handles the exit signal
 * @param signal - the signal type
 */

void ctrlCSignal(int signal)
{
	/* Free system V resources */
	// call clean up function to delete message queue and shared memory
	cleanUp(shmid, sharedMemPtr);
}

int main(int argc, char** argv)
{

	/* TODO: Install a singnal handler (see signaldemo.cpp sample file).
 	 * In a case user presses Ctrl-c your program should delete message
 	 * queues and shared memory before exiting. You may add the cleaning functionality
 	 * in ctrlCSignal().
 	 */
	signal(SIGINT, ctrlCSignal);

	/* Initialize */
	init(shmid, sharedMemPtr);

	/* Go to the main loop */
	mainLoop();

	// display the message store in recvfile
	string line;
	ifstream myfile ("recvfile");
	if (myfile.is_open())
	{
		while ( getline (myfile,line) )
		{
			cout << line << '\n';
		}
		myfile.close();
	}


	/** TODO: Detach from shared memory segment, and deallocate shared memory and message queue (i.e. call cleanup) **/
	cleanUp(shmid, sharedMemPtr);

	return 0;
}
