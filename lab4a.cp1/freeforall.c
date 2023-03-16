#include "freeforall.h"
#include "util.h"


void *freeforall_printer(void *arg)
{

	// Convert id argument to integer
	int myid = *((int*)arg);

	// Have it print jobs forever
	while(true)
	{
		print_job(myid);
		sleep(4);
	}
}


void *freeforall_process(void *arg)
{

	// Convert arguments to integer
	int myid = *((int*)arg);
	int job = 0;

	// Have it create print jobs forever
	while(true)
	{
		produce_job(myid, job);
		job++;
		sleep(1);
	}
}


void demo_freeforall()
{

	// Create 1 thread for producer, and 1 for processor. 
	// Both have ID 0. 
	pthread_t process_thread;
	pthread_t printer_thread;
	int process_id = 0;
	int printer_id = 1;

	// Create the threads. Let them do their job. 
	pthread_create(&process_thread, NULL, freeforall_process, (void *)&process_id);
	sleep(1);
	pthread_create(&printer_thread, NULL, freeforall_printer, (void *)&printer_id);

	// Join the threads - this never really gets called. 
	pthread_join(process_thread, NULL);
	pthread_join(printer_thread, NULL);
}
