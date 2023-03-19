/* 
	Description:
	
		This module pertains to the code for Part 2 of Lab 4 of the EEE335 course.
		
		
	Notes:

		- Doing iteration 1 - process creation and destruction
		- Doing iteration 2 - Adding shared memory

		- For print statements, need to put \n at end of string so that the printf() library buffer prints the
		string immediately. Was experiencing minor issues where print statements in philosophers weren't printing out in time, 
		even when waitpid() was called in the parent. 

		- _exit() is used in child processes since it does not flush the stdio buffer in the parent process. 
		Basically, for good practice. 
	
		
	Author: OCdt Liethan Velasco
	Version: March 14 2023
	
	*/


// -- Importing libraries --

#include <stdio.h>		// needed for printf, sprintf, ...
#include <stdlib.h>		// needed for exit()
#include <unistd.h>		// needed for sleep, fork, waitpid, _exit, ftruncate
#include <sys/wait.h>	// needed for waitpid

// --- Libraries for iteration 2 ---

#include <sys/mman.h>
#include <fcntl.h>


// -- Declaring Macros --
#define PHILO_COUNT 5


// --- Initializing variables ---

// sem_t mutex = 1;
// sem_t s[5] = 0; <-- Initialized to 0 in order for it to work.

struct shared_memory {
	
	int k; // <-- for testing in iteration 2. 

};


// --- Declaring Function Prototypes ---
void philosopher(int i, struct shared_memory* shared_mem);
void parent();

// --- Main Entry Point ---

int main(void)
{


	// ----- Code given from instructions to create shared memory ----

	int fd_shm; 	// var to store inode number of shared memory file
	fd_shm = shm_open("philo_shared", O_RDWR | O_CREAT, 0660);
	if (fd_shm == -1) {
		printf("Function shm_open() failed.\n");
		exit(-1);
	}

	// Truncate size of shared memory to size of the struct
	if (ftruncate(fd_shm, sizeof(struct shared_memory)) == -1) {
		printf("Function ftruncate() failed.\n");
		exit(-1);
	}

	// Maps shared memory object to virt memory space of processes.
	// NULL -> Kernel chooses address for mapping
	// sizeof(...) -> size of the object
	// PROT... -> protections of object in mem space
	// MAP_SHARED -> makes map updates visible to other processes in same region and in underlying file in /dev/shm/
	// fd_shm -> inode of underlying file
	// 0 -> offset in file of object's location. 
	struct shared_memory* shared_mem = mmap(NULL, sizeof(struct shared_memory), PROT_READ | PROT_WRITE, MAP_SHARED, fd_shm, 0);
	if (shared_mem == MAP_FAILED) {
		printf("Function mmap() failed.\n");
		exit(-1);
	}
	shared_mem -> k = 0;
	
	
	// Declaring var to capture fork() return result
	int result;

	// Have the parent create 5 different proceses with fork.
	// Make each child run philosopher(). 
	for (int i = 0; i < PHILO_COUNT; i++) {

		result = fork();
		if (result == 0) {
			philosopher(i, shared_mem);
		}
		else {

			// Parent prints child created, and repeat
			printf("Process created: p[%d] = %d\n", i, result);
			continue; 

		}
	}

	// Wait for all 5 children to finish by calling waitpid() 5 times
	int status;
	for (int i = 0; i < PHILO_COUNT; i++) {
		waitpid(-1, &status, 0);
	}

	// Back in parent here, call parent()
	parent();


	// ---- Given code to end shared memory ----

	// Ends mapping of shared mem object to specified address at shared_mem of specified size
	if (munmap(shared_mem, sizeof(struct shared_memory)) == -1) {
		printf("Function munmap() failed.\n");
		exit(-1);
	}

	// Deletes name of shared memory object and the file in /dev/shm/
	if ((shm_unlink("philo_shared")) == -1) {
		printf("Function shm_unlink() failed.\n");
		exit(-1);
	}

}



/* Description:

	Function that gets called by the created child process. 
	Child processes in question are the philosophers. Function does the applicable philospher behavior. 
	
Arguments: 
	<int i> : The assigned "ID" of philospher (0 to 5). */

void philosopher(int i, struct shared_memory* shared_mem){

	// Testing shared memory
	for (int j = 0; j < 100; j++) {

		int temp = shared_mem -> k;
		temp++;
		usleep(5);
		shared_mem -> k = temp;
		usleep(5);

	}


	printf("PHILOSOPHER: I am the philosopher %d	k = %d\n", i, shared_mem -> k);
	_exit(0); 

}



/* Description:

	Function that gets called by the parent process.
	For now, prints a declaration of its identity. 

*/
void parent() {
	
	printf("PARENT: I am the parent process.\n");

	exit(0);

}