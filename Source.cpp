/*******************************************************************************
* File Name          : circularQueue.cpp
* Description        : Circular Queue assignment skeleton starting point
*
* Author			 : Kenosha F Vaz
* Date				 : June 27, 2017
******************************************************************************
*/

#include "stdafx.h"
#include <windows.h>
#include <strsafe.h>
#include <math.h>

DWORD WINAPI myReadThreadFunction(LPVOID lpParam);                                   // function used by read from queue thread
unsigned int putToCircularQueue(char *ptrInputBuffer, unsigned int bufferLength);    // circular queue function to add data to queue
unsigned int getFromCircularQueue(char *ptrOutputBuffer, unsigned int bufferLength); // circular queue function to remove data from queue

#define BUFFER_SIZE         50                // local buffer used for sending or receiving data to/from queue in main and worker thread

#define INPUT_TIMEOUT_MS    5000               // dequeue every 5 seconds

#define CIRCULAR_QUEUE_SIZE 50                // size of the circular queue

char tempQueue[BUFFER_SIZE];

unsigned int initialLength, finalLength;
																					 // data structure used to keep track of circular queue (DO NOT CHANGE THIS STRUCTURE)
struct myQueueStruct {
	char *ptrBuffer = NULL;                    // pointer to start of the circular queue buffer
	volatile unsigned int queueCount = 0;      // number of characters currently in cirucular queue
	volatile char *ptrCircularHead = NULL;     // location where data is added to queue
	volatile char *ptrCircularTail = NULL;     // loation where data is removed from queue
};

struct myQueueStruct myQueue;                  // create an instance of the circular queue data structure

											   // DO NOT CHANGE THE CODE IN MAIN
int main()
{
	HANDLE  hThread;            // handle to thread
	char    inputBuffer[BUFFER_SIZE];

	myQueue.ptrBuffer = (char *)calloc(CIRCULAR_QUEUE_SIZE, sizeof(char));
	if (myQueue.ptrBuffer == NULL)
	{
		printf("Error unable to allocate memory for buffer\n");
		exit(-1);
	}

	myQueue.ptrCircularHead = myQueue.ptrCircularTail = myQueue.ptrBuffer;

	// create a thread that will consume the data we type in to demonstrate dequeing the data
	hThread = CreateThread(
		NULL,                   // default security attributes
		0,                      // use default stack size  
		myReadThreadFunction,   // thread function name
		NULL,                   // argument to thread function (not being used)
		0,                      // use default creation flags 
		NULL);                  // returns the thread identifier (not being used)

								// get a string from the console and queue it to circular queue
	while (1)
	{
		scanf_s("%s", inputBuffer, BUFFER_SIZE);    // get data from console

													// put the data into the circular queue but check if an error (marked by queue function returning 0) occurred
		if (putToCircularQueue(inputBuffer, strlen(inputBuffer)) == 0)
			printf("Error queuing data\n");

	}
	return 0;
}

// FUNCTION      : putToCircularQueue
// DESCRIPTION   :
//   Put the supplied data into a circular queue
// PARAMETERS    :
//   ptrInputBuffer - a pointer to the buffer to be put into the queue
//   bufferLength   - how many characters are in the buffer being put into the queue
//
// RETURNS       :
//   Number of characters successfully queued (0 on failure)

unsigned int putToCircularQueue(char *ptrInputBuffer, unsigned int bufferLength)
{
	unsigned int numberOfCharsQueued = 0,amtOfSpaceAvailable=0;  // number of characters placed in circular queue

	// add code to implement the adding to the circular queue using the data structures in myQueueStruct
	// and the passed in pointer and data to be added count

	myQueue.queueCount = CIRCULAR_QUEUE_SIZE - (abs(myQueue.ptrCircularHead - myQueue.ptrCircularTail)) + bufferLength;							//sets the bufferLength as the queueCount.
	
								//If Memory Queue is Empty
	if (myQueue.ptrCircularTail == myQueue.ptrCircularHead) {
		if (myQueue.ptrBuffer == myQueue.ptrCircularHead) {							//For the first Entry only!
			memcpy((void *)myQueue.ptrCircularHead, ptrInputBuffer, bufferLength);
			numberOfCharsQueued = +bufferLength;
			myQueue.ptrCircularHead = myQueue.ptrCircularHead + numberOfCharsQueued;
			return numberOfCharsQueued;
		}
		else if ((myQueue.ptrCircularHead - myQueue.ptrBuffer) == (CIRCULAR_QUEUE_SIZE - 1)) {			//If empty Queue has Head pointer at the end of Queue.
			myQueue.ptrCircularHead = myQueue.ptrBuffer - 1;						//Wrap around is set as First Entry.
			memcpy((void *)myQueue.ptrCircularHead, ptrInputBuffer, bufferLength);
			numberOfCharsQueued = +bufferLength;
			myQueue.ptrCircularHead = myQueue.ptrCircularHead + numberOfCharsQueued;
			return numberOfCharsQueued;
		}
		else {																		//Any other situation will mean the ptrHead will be in the middle of the Circular Queue
			// Here we check if the wrap is necessary and continue onwards...
			amtOfSpaceAvailable = CIRCULAR_QUEUE_SIZE - numberOfCharsQueued;
			if (bufferLength < amtOfSpaceAvailable) {								//checks if new data to added is smaller than Space Available.
				if ((bufferLength) > (CIRCULAR_QUEUE_SIZE - (unsigned int)myQueue.ptrCircularHead)) {	//checks if pre-wrap space is smaller than buffer length.
					initialLength = (bufferLength - 1) - (CIRCULAR_QUEUE_SIZE - (unsigned int)myQueue.ptrCircularHead);
					finalLength = bufferLength - initialLength;

								//Pre-wrap Step
					printf("\nSpace Available! Proceed to Write...\n\n");
					memcpy(tempQueue, ptrInputBuffer, initialLength);
					memcpy((void *)myQueue.ptrCircularHead, tempQueue, initialLength);
					myQueue.ptrCircularHead = myQueue.ptrBuffer - 1;

								//Post-wrap Step
					printf("Space Available! Proceed to Write...\n\n");
					memcpy(tempQueue, ptrInputBuffer+initialLength, finalLength);
					memcpy((void *)myQueue.ptrCircularHead, tempQueue, finalLength);

					numberOfCharsQueued = +bufferLength;

					myQueue.ptrCircularHead = myQueue.ptrBuffer + finalLength;
					return finalLength;
				}
			}
			else {				//bufferLength can fit in the space available
				memcpy((void *)myQueue.ptrCircularHead, ptrInputBuffer, bufferLength);
				numberOfCharsQueued = +bufferLength;
				myQueue.ptrCircularHead = myQueue.ptrCircularHead + numberOfCharsQueued;
				return numberOfCharsQueued;
			}
		}
	}
								//If Memory contains contains data AND ptrHead's position is greater than ptrTail's position
	else if (myQueue.ptrCircularHead > myQueue.ptrCircularTail) {
		numberOfCharsQueued = (unsigned int)myQueue.ptrCircularHead - (unsigned int)myQueue.ptrCircularTail;
		amtOfSpaceAvailable = CIRCULAR_QUEUE_SIZE - numberOfCharsQueued;
		if (bufferLength < amtOfSpaceAvailable) {										//checks if new data to added is smaller than Space Available.
			if ((bufferLength) > (CIRCULAR_QUEUE_SIZE - (unsigned int)myQueue.ptrCircularHead)) {					//checks if pre-wrap space is smaller than buffer length.
				initialLength = (bufferLength - 1) - (CIRCULAR_QUEUE_SIZE - (unsigned int)myQueue.ptrCircularHead);	//initial length to be filled before wrap around.
				finalLength = bufferLength - initialLength;								//remaining buffer length to filled after wrap around.
				
								//Pre-wrap Step
				printf("\nSpace Available! Proceed to Write...\n\n");
				memcpy(tempQueue, ptrInputBuffer, initialLength);
				memcpy((void *)myQueue.ptrCircularHead, tempQueue, initialLength);
				myQueue.ptrCircularHead = myQueue.ptrBuffer - 1;

								//Post-wrap Step
				printf("Space Available! Proceed to Write...\n\n");
				memcpy(tempQueue, ptrInputBuffer, finalLength);
				myQueue.ptrCircularHead = myQueue.ptrBuffer + finalLength;
				return finalLength;
			}
			else {				//bufferLength can fit in the space available
				memcpy((void *)myQueue.ptrCircularHead, ptrInputBuffer, bufferLength);
				numberOfCharsQueued = +bufferLength;
				myQueue.ptrCircularHead = myQueue.ptrCircularHead + numberOfCharsQueued;
				return numberOfCharsQueued;
			}
		}
	}
								//If Memory contains contains data AND ptrHead's position is smaller than ptrTail's position
	else if (myQueue.ptrCircularHead<myQueue.ptrCircularTail) {
		
								//Please NOTE that the variables are now flipped for better control of results.
		amtOfSpaceAvailable = (unsigned int)myQueue.ptrCircularHead - (unsigned int)myQueue.ptrCircularTail;
		numberOfCharsQueued = CIRCULAR_QUEUE_SIZE - amtOfSpaceAvailable;

		if (bufferLength < amtOfSpaceAvailable) {										//checks if new data to added is smaller than Space Available.
			if ((bufferLength) >(CIRCULAR_QUEUE_SIZE - (unsigned int)myQueue.ptrCircularHead)) {					//checks if pre-wrap space is smaller than buffer length.
				initialLength = (bufferLength - 1) - (CIRCULAR_QUEUE_SIZE - (unsigned int)myQueue.ptrCircularHead);	//initial length to be filled before wrap around.
				finalLength = bufferLength - initialLength;								//remaining buffer length to filled after wrap around.

																						//Pre-wrap Step
				printf("\nSpace Available! Proceed to Write...\n\n");
				memcpy(tempQueue, ptrInputBuffer, initialLength);
				memcpy((void *)myQueue.ptrCircularHead, tempQueue, initialLength);
				myQueue.ptrCircularHead = myQueue.ptrBuffer - 1;

				//Post-wrap Step
				printf("Space Available! Proceed to Write...\n\n");
				memcpy(tempQueue, ptrInputBuffer, finalLength);
				myQueue.ptrCircularHead = myQueue.ptrBuffer + finalLength;
				return finalLength;
			}
			else {				//bufferLength can fit in the space available
				memcpy((void *)myQueue.ptrCircularHead, ptrInputBuffer, bufferLength);
				numberOfCharsQueued = +bufferLength;
				myQueue.ptrCircularHead = myQueue.ptrCircularHead + numberOfCharsQueued;
				return numberOfCharsQueued;
			}
		}
	}

	else {						//Take Memory as Full and return when empty.
		printf("\nMemory Full! Revert to Main...\n\n");
		myQueue.ptrCircularHead = myQueue.ptrBuffer;

		return 0;
	}

	return numberOfCharsQueued;
}

// FUNCTION      : getFromCircularQueue
// DESCRIPTION   :
//   Read as much data as there is room for from the circular queue
// PARAMETERS    :
//   ptrOutputBuffer - a pointer to the buffer to place the data read from queue
//   bufferLength    - maxiumum number of characters that can be read (IE the size of the output buffer)
//
// RETURNS       :
//   Number of characters successfully queued (0 if none are read)

unsigned int getFromCircularQueue(char *ptrOutputBuffer, unsigned int bufferLength)
{
	unsigned int readCount = 0;


	// add code to implement the removing from the circular queue using the data structures in myQueueStruct
	// and place the data into the ptrOutputBuffer up to a maximum number of bytes specified by bufferLength
	if (myQueue.ptrCircularHead != myQueue.ptrCircularTail) {
		if (myQueue.ptrCircularHead >= myQueue.ptrCircularTail) {
			readCount = myQueue.ptrCircularHead - myQueue.ptrCircularTail;
			memcpy(ptrOutputBuffer,(void *)myQueue.ptrCircularTail,readCount);			//takes source data and copies to temp queue
			printf("De-queuing data\n\n");
			myQueue.queueCount -= readCount;											//queue count is now zero.
			myQueue.ptrCircularHead = myQueue.ptrCircularTail;							//sets queue as empty.
			return readCount;
		}else if(myQueue.ptrCircularHead<myQueue.ptrCircularTail){
			for (int i = 0; i < BUFFER_SIZE; i++) {
				tempQueue[i] = '\0';
			}
			readCount = (unsigned int)myQueue.ptrCircularTail - (CIRCULAR_QUEUE_SIZE - 1);	//takes the last position as the difference of removable space from the CIRCULAR QUEUE SIZE
			initialLength = (bufferLength - 1) - (CIRCULAR_QUEUE_SIZE - readCount);			//if remaining memory is less than the string size, this is initial memory
			finalLength = bufferLength - initialLength;										//takes the string size difference which will be added at the wrap around
			
			memcpy(tempQueue, (void *)myQueue.ptrCircularTail, initialLength);				//sets the data from the Tail's position
			printf("Wrapped Data De-Queued :\t %s\n", tempQueue);
			myQueue.ptrCircularTail = myQueue.ptrBuffer;
			
			memcpy(tempQueue+initialLength, (void *)myQueue.ptrBuffer, finalLength);						//sets the data from the beginning position upto final length.
			printf("Second Wrapped Data De-Queued :\t%s\n",tempQueue);
			myQueue.ptrCircularTail = myQueue.ptrCircularHead;
			return readCount;
		}
	}
	
	
	// return the count of the bytes that was dequeued

	return readCount;
}

// FUNCTION      : myReadThreadFunction
// DESCRIPTION   :
//   A seperate thread from the main program that will independently check for data in queue and print it out
//   DO NOT CHANGE THIS CODE!
// PARAMETERS    :
//   lpParam - arguement passed into thread (not used in this example)
//
// RETURNS       :
//   Will never return so no effective return value

DWORD WINAPI myReadThreadFunction(LPVOID lpParam)
{
	char readBuffer[BUFFER_SIZE];     // local buffer to put the data into when reading from queue and print
	unsigned int  readCount = 0;      // the number of characters read from queue

	while (1)
	{
		Sleep(INPUT_TIMEOUT_MS);      // wait for some data to be received

									  // check if data is available and if so print it out
		readCount = getFromCircularQueue(readBuffer, BUFFER_SIZE);
		if (readCount != 0)           // check for reads that did not get any data
			printf("UNQUEUE: %s\n", readBuffer);
	}
	return 0;    // will never reach here
}