#include "strict_alternation.h"
#include "util.h"


void *strict_printer(void *arg)
{
	int myid = *((int*)arg);

	while(true)
	{
		print_job(myid);
		sleep(4);
	}
}


void *strict_process(void *arg)
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


void demo_strict_alternation()
{
	pthread_t process_thread;
	pthread_t printer_thread;
	int process_id = 0;
	int printer_id = 0;

	// Create the threads
	pthread_create(&process_thread, NULL, strict_process, (void *)&process_id);
	sleep(1);
	pthread_create(&printer_thread, NULL, strict_printer, (void *)&printer_id);

	// Join the threads
	pthread_join(process_thread, NULL);
	pthread_join(printer_thread, NULL);
}
