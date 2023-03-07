/* 

	Description:

		Iteration of spooler program with strict alternation solution. 

	Plan:

		Okay, we will have a global variable called <turn>. 
		This will keep track of which thread gets to access the spooler 
		at once time. 

		When turn = 0, it's the producer's turn to be in the spooler.
		When turn = 1, it's the printer's turn to be in the spooler.

		I'll just be following the examples from the class notes.


*/

// --- Import moudles ---

#include "strict_alternation.h"
#include "util.h"


// --- Create Needed Global Variables ---

// Creating enum for convenience.
enum TURN {
	PRODUCER = 0,
	PRINTER = 1
};

int turn = PRODUCER; // Initialize producer to be first in spooler. 


void *strict_printer(void *arg)
{

	// Convert id argument to integer
	int myid = *((int*)arg);

	// Have it print jobs forever
	while(true)
	{

		// If not printer's turn, wait
		while (turn != PRINTER) {}

		// When printer's turn, do work in spooler
		// and switch turn to producer.
		print_job(myid);
		turn = PRODUCER;

		sleep(4);
	}
}


void *strict_process(void *arg)
{

	// Convert arguments to integer
	int myid = *((int*)arg);
	int job = 0;

	// Have it create print jobs forever
	while(true)
	{

		// Wait for producer's turn
		while (turn != PRODUCER) {}

		// When producer's turn, do work in spooler
		// and switch turn to printer
		produce_job(myid, job);
		turn = PRINTER;

		job++;
		sleep(1);
	}
}


void demo_strict_alternation()
{

	// Create 1 thread for producer, and 1 for processor. 
	// Both have ID 0. 
	pthread_t process_thread;
	pthread_t printer_thread;
	int process_id = 0;
	int printer_id = 0;

	// Create the threads. Let them do their job. 
	pthread_create(&process_thread, NULL, strict_process, (void *)&process_id);
	sleep(1);
	pthread_create(&printer_thread, NULL, strict_printer, (void *)&printer_id);

	// Join the threads - this never really gets called. 
	pthread_join(process_thread, NULL);
	pthread_join(printer_thread, NULL);
}
