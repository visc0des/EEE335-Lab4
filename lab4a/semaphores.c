/* 

	Description:

		Iteration of spooler program with semaphores. 

	Plan:

		Pretty self explanatory. Just use sempahores to 
		ensure certain processes go to sleep when another is in the
		spooler, and certain processes sleep when the spooler is
		empty/full (though it should never become empty).


		Pay attention with order!

		sem_post() -> up()
		sem_wait() -> down()
		sem_init() -> for creation

	
*/


// --- Import modules ---

#include "semaphores.h"
#include "util.h"
#include <semaphore.h>

// --- Created needed global variables ---

// Creating semaphores
sem_t spooler_open;
sem_t empty;
sem_t full;


void *semaphore_printer(void *arg)
{
	int myid = *((int*)arg);

	while(true)
	{

		// Decrement full space sema, then set spooler_open sem to locked
		sem_wait(&full);
		sem_wait(&spooler_open);		

		// Do print job
		print_job(myid);

		// When finihed, unlock spooler_open, and increment empty sema
		sem_post(&spooler_open);
		sem_post(&empty);

		sleep(4);
	}
}


void *semaphore_process(void *arg)
{
	int myid = *((int*)arg);
	int job = 0;

	while(true)
	{

		// Decrement empty space sema, set spooler_open sem to locked
		sem_wait(&empty);
		sem_wait(&spooler_open);

		// Create print job
		produce_job(myid, job);
		job++;

		// Unlocked spooler_open, increment full sema
		sem_post(&spooler_open);
		sem_post(&full);

		sleep(1);
	}
}


void demo_semaphores()
{
	// -- Initialize semaphores --

	printf("\nWe're going to create the sempahores.");

	// Spooler mutex
	int result = sem_init(&spooler_open, 0, 1); 
	if (result != 0) {
		printf("\nERROR - created spooler_open sempahore returned code: %d", result);
		exit(-1);
	}
	printf("\nFirst semaphore created.");

	// Semaphore for empty spaces (intialize to 10)
	result = sem_init(&empty, 0, SPOOLER_SIZE); 
	if (result != 0) {
		printf("\nERROR - created empty sempahore returned code: %d", result);
		exit(-1);
	}

	printf("\nSecond sempahore created.");

	// Sempahore for full slots (initialize to 0)
	result = sem_init(&full, 0, 0); 
	if (result != 0) {
		printf("\nERROR - created full sempahore returned code: %d", result);
		exit(-1);
	}

	printf("\nWe've created the sempahores.");

	// Created threads and execute them accordingly
	pthread_t process_thread0;
	pthread_t process_thread1;
	pthread_t printer_thread0;
	pthread_t printer_thread1;
	int process_id0 = 0;
	int process_id1 = 1;
	int printer_id0 = 0;
	int printer_id1 = 1;

	pthread_create(&process_thread0, NULL, semaphore_process, (void *)&process_id0);
	pthread_create(&process_thread1, NULL, semaphore_process, (void *)&process_id1);
	sleep(1);
	pthread_create(&printer_thread0, NULL, semaphore_printer, (void *)&printer_id0);
	pthread_create(&printer_thread1, NULL, semaphore_printer, (void *)&printer_id1);

	pthread_join(process_thread0, NULL);
	pthread_join(process_thread1, NULL);
	pthread_join(printer_thread0, NULL);
	pthread_join(printer_thread1, NULL);
}
