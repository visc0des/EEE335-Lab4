/* 

	Description:

		Iteration of spooler program with Peterson's Algorithm. 

	Plan:

		Alright, it'll be like strict alternation, 
		however we will have an 'interested in entering crit. reg.' array representative 
		of each applicable thread, and update the gatekeeping while loop condition.

		This won't be the humble version of the solution, but instead the greedy version.

		This time, will have turns selected by the passed in thread ID

*/


// --- Import modules ---

#include "peterson.h"
#include "util.h"
#include "freeforall.h"

// Using global counter variable for print jobs
int global_print_job_counter = 0;


// --- Create needed variables ---

// Creating boolean enum
enum BOOL {
	TRUE = 1,
	FALSE = 0
};

// Creating prod thread enum
enum TID {
	PROD_0 = 0,
	PROD_1 = 1
};


// Creating interested array and turn int (give first turn to thread 0)
int prod_turn = PROD_0;
int interested[2] = {FALSE, FALSE};


void *peterson_process(void *arg)
{
	int myid = *((int*)arg);
	// int job = 0;
	
	while(true)
	{

		// Get other prod thread id
		int other_id = 1 - myid;
		
		// Set this thread to interested, and switch it to its turn.
		interested[myid] = TRUE;
		prod_turn = myid;

		// wait until producer 1's interested is false
		while (prod_turn == myid && interested[other_id] == TRUE);


		// When passed gate, create job in spooler, set this thread's interested to FALSE
		produce_job(myid, global_print_job_counter);
		global_print_job_counter++;
		interested[myid] = FALSE;
	

		sleep(1);
	}
}


// Should we create a separate peterson_printer()? 


void demo_peterson()
{
	pthread_t process_thread0;
	pthread_t process_thread1;
	pthread_t printer_thread;
	int process_id0 = PROD_0;
	int process_id1 = PROD_1;
	int printer_id = 0;

	// Create producer threads (2 peterson's)
	pthread_create(&process_thread0, NULL, peterson_process, (void *)&process_id0);
	pthread_create(&process_thread1, NULL, peterson_process, (void *)&process_id1);
	sleep(1);

	// Create printer thread (type freeforall)
	pthread_create(&printer_thread, NULL, freeforall_printer, (void *)&printer_id);

	// Join the threads
	pthread_join(process_thread0, NULL);
	pthread_join(process_thread1, NULL);
	pthread_join(printer_thread, NULL);
}
