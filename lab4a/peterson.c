#include "peterson.h"
#include "util.h"
#include "freeforall.h"


void *peterson_process(void *arg)
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


void demo_peterson()
{
	pthread_t process_thread0;
	pthread_t process_thread1;
	pthread_t printer_thread;
	int process_id0 = 0;
	int process_id1 = 1;
	int printer_id = 0;

	// Create the threads
	pthread_create(&process_thread0, NULL, peterson_process, (void *)&process_id0);
	pthread_create(&process_thread1, NULL, peterson_process, (void *)&process_id1);
	sleep(1);
	pthread_create(&printer_thread, NULL, freeforall_printer, (void *)&printer_id);

	// Join the threads
	pthread_join(process_thread0, NULL);
	pthread_join(process_thread1, NULL);
	pthread_join(printer_thread, NULL);
}
