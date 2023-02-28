#include "semaphores.h"
#include "util.h"


void *semaphore_printer(void *arg)
{
	int myid = *((int*)arg);

	while(true)
	{
		print_job(myid);
		sleep(4);
	}
}


void *semaphore_process(void *arg)
{
	int myid = *((int*)arg);
	int job = 0;

	while(true)
	{
		produce_job(myid, job);
		job++;
		sleep(1);
	}
}


void demo_semaphores()
{
	// Initialize semaphores
	/* INSERT CODE HERE */

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
