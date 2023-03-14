/* 
	Description:
	
		This module pertains to the code for Part 2 of Lab 4 of the EEE335 course.
		
		
	Notes:

		- Doing iteration 1 - process creation and destruction

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


// -- Declaring Macros --
#define PHILO_COUNT 5

// --- Declaring Function Prototypes ---
void philosopher(int i);
void parent();



// --- Initializing variables ---

// sem_t mutex = 1;
// sem_t s[5] = 0; <-- Initialized to 0 in order for it to work.

// --- Main Entry Point ---

int main(void)
{
	
	// Declaring var to capture fork() return result
	int result;

	// Have the parent create 5 different proceses with fork.
	// Make children run philosopher(). 
	for (int i = 0; i < PHILO_COUNT; i++) {

		result = fork();
		if (result == 0) {
			philosopher(i);
		}
		else {
			printf("Process created: p[%d] = %d\n", i, result);
			continue; // Create next child. 
		}
	}

	// Wait for all 5 children to finish with waitpid() 5 times
	int status;
	for (int i = 0; i < PHILO_COUNT; i++) {
		waitpid(-1, &status, 0);
	}

	// Back in parent here, call parent()
	parent();
	

	

	

}



/* Description:

	Function that gets called by the created child process. 
	Child processes in question are the philosophers. Function does the applicable philospher behavior. 
	
Arguments: 
	<int i> : The assigned "ID" of philospher (0 to 5). */

void philosopher(int i) {

	// Prints ID, and then special exits
	printf("PHILOSOPHER: I am the philosopher %d\n", i);
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