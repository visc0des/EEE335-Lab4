#include "util.h"


// The printer spooler and its iterators
char *spooler[SPOOLER_SIZE];	// array of pointers to char
int it_next_add = 0;			// index in the spooler where the next job should be added
int it_next_remove = 0;			// index in the spooler for the next job to be printed



/* 
	Description:

		The function simulates a thread printing a job from the spooler.
		When job is printed, it is removed from the spooler. 

	Arguments:
		<int myid> : The thread id doing the print job. 

*/
void print_job(int myid)
{

	// Status print
	printf("Printer %d printing slot %d: %s\n", myid, it_next_remove, spooler[it_next_remove]);

	// Free the "next to remove" location in spooler and pause for 1 seoncd
	free(spooler[it_next_remove]);
	sleep(1);

	// Increment pointer of next location to remove
	it_next_remove = (it_next_remove + 1) % SPOOLER_SIZE;
}



/*
	Description:

		The function simulates production of print job by a thread.  
		Produced job gets put into the spooler as a string. 

	Arguments:
		<int myid> : The thread ID doing the job. 
		<int job> : The print job number to create.

*/
void produce_job(int myid, int job)
{

	// Print status
	printf("Process %d is creating print job #%d in spooler slot %d\n", myid, job, it_next_add);

	// Create and announce print job string, store it in spooler 
	char *text = (char*)malloc(BUFFER_SIZE * sizeof(char));
	sprintf(text, "Hello, this is print job %d - created by process %d", job, myid);
	spooler[it_next_add] = text;

	// Wait 1 second, then increment pointer to next print job location in spooler
	sleep(1);
	it_next_add = (it_next_add + 1) % SPOOLER_SIZE;
}
