#include "util.h"


// The printer spooler and its iterators
char *spooler[SPOOLER_SIZE];	// array of pointers to char
int it_next_add = 0;			// index in the spooler where the next job should be added
int it_next_remove = 0;			// index in the spooler for the next job to be printed


void print_job(int myid)
{
	printf("Printer %d printing slot %d: %s\n", myid, it_next_remove, spooler[it_next_remove]);
	free(spooler[it_next_remove]);
	sleep(1);
	it_next_remove = (it_next_remove + 1) % SPOOLER_SIZE;
}

void produce_job(int myid, int job)
{
	printf("Process %d is writing job %d to the spooler in slot %d\n", myid, job, it_next_add);
	char *text = (char*)malloc(BUFFER_SIZE * sizeof(char));
	sprintf(text, "Hello, this is process %d print job %d", myid, job);
	spooler[it_next_add] = text;
	sleep(1);
	it_next_add = (it_next_add + 1) % SPOOLER_SIZE;
}
