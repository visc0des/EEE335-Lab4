/* 
	Description:
	
		This module pertains to the code for Part 2 of Lab 4 of the EEE335 course.
		
		
	Notes:

		- Doing iteration 1 - Process creation and destruction
		- Doing iteration 2 - Adding shared memory
		- Doing iteration 3 - Adding Semaphores


		- For print statements, need to put \n at end of string so that the printf() library buffer prints the
		string immediately. Was experiencing minor issues where print statements in philosophers weren't printing out in time, 
		even when waitpid() was called in the parent. 

		- _exit() is used in child processes since it does not flush the stdio buffer in the parent process. 
		Basically, for good practice. 

		- Huh, turns out when I "killed" the program in the console, the created philosopher processes were never actually killed - the program
		was simply removed from the console. That explains the system started to get slower and slower and working less correctly as I kept running
		the program which did not have the philosophers kill themselves after a certain point in time. 

		This is what I believe caused my issue of the program refusing to work properly - there were too many processes running in the background, 
		hogging up all of the cpu time and not letting the most recently created philosopher processes to run at their full capacity - this must
		have been what caused the program to do really weird things, where philosophers would have supposedly "completed" a cycle when it has entered
		the hungry state - obviously, it had not completed an entire cycle at this point. Even some adjacent philosophers would be in the eating
		state when I had never programmed them to do so. At one point, a philsopher would for some darn reason enter the finished state, when I had
		never even implemented any sort of transition to the finished state yet. Some philosophers weren't even respecting the times! For example,
		I would set the EATING time to be 10 seconds, but most philosophers would only eat for 2 seconds!!!!! There was a 1/8 chance a philosopher would
		respect the entire wait time. Clearly something was definitely wrong. 

		Obviously the quick solution was to manually kill all the created processes associated with the program from a terminal. This instantly
		solved the problem where the code is now performing exactly how I'd like it to - philosophers incrementing completed cycles when they
		are supposed to, and no adjacent philosophers eating next to eachother. 



	Status:

		- Bug found - too many processes running in the background. They weren't getting killed automatically. Nice.
	
		
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

// --- Libraries for iteration 3 ---

#include <semaphore.h>




// -- Declaring Macros --

#define PHILO_COUNT 5
#define THINKING 	0 		// Philosopher macros to represent states
#define HUNGRY 		1
#define EATING 		2	
#define FINISHED 	4

#define LEFT		(i + PHILO_COUNT - 1) % PHILO_COUNT
#define RIGHT 		(i + 1) % PHILO_COUNT

#define THINK_TIME 	2
#define EAT_TIME 	3
#define MAX_CYCLES  4


// --- Initializing variables ---

// sem_t mutex = 1;
// sem_t s[5] = 0; <-- Initialized to 0 in order for it to work.

struct shared_memory {
	
	sem_t mutex;		// Semaphore for when philos enter their crit reg. 
	sem_t philo_sem[PHILO_COUNT]; // Semaphores repping philos getting forks 

	// Creating array of states tracking if philos are thinking, hungry, or eating
	int philo_state[PHILO_COUNT];

	// Array to keep track of philosopher cycles
	int philo_cycles[PHILO_COUNT];

};


// --- Declaring Function Prototypes ---
void philosopher(int i, struct shared_memory* shared_mem);
void parent();
void philo_think();
void philo_eat();
void take_forks(int i, struct shared_memory* shared_mem);
void put_forks(int i, struct shared_memory* shared_mem);
void test_forks(int i, struct shared_memory* shared_mem);
void printer_process(struct shared_memory* shared_mem);
int check_finished(int states[PHILO_COUNT]);


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
	
	// Initialize shared shared mutex to 1
	int sem_result = sem_init(&shared_mem -> mutex, 1, 1);
	if (sem_result == -1) {
		printf("Creation of mutex failed.\n");
		exit(-1);
	}
	
	// Initialize all philosopher semaphores to 0 (none of them have forks yet),
	// and set their states to all THINKING. Initialize philosopher cycles
	for (int i = 0; i < PHILO_COUNT; i++) {
		
		sem_result = sem_init(&shared_mem -> philo_sem[i], 1, 0);
		if (sem_result == -1) {
			printf("Creation of philosopher semaphore failed.\n");
			exit(-1);
		}

		shared_mem -> philo_state[i] = THINKING;

		shared_mem -> philo_cycles[i] = 0;

	}



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

	// Create additional process for printing philosopher statuses

	result = fork();
	if (result == 0) {
		printer_process(shared_mem);
	}
	else {
		printf("Printer process created of ID %d\n", result);
	}
	

	// Wait for all philo children and printer to finish by calling waitpid()
	int status;
	for (int i = 0; i < PHILO_COUNT + 1; i++) {
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

	// Let philosophers, think, try to eat, eat, stop eating, and then repeat
	while (1) {

		philo_think();
		take_forks(i, shared_mem);
		philo_eat();
		put_forks(i, shared_mem);

		// Check if philosopher was set to finished state. If so, exit loop.
		if (shared_mem -> philo_state[i] == FINISHED) {
			break;
		}

	}

	_exit(0); 

}


/*	Description:

		Function that gets called when philosopher is in their thinking state. Must
		happen for at least 2 seconds.

*/
void philo_think() {


	sleep(THINK_TIME);

}


/*	Description:

		Function that gets called when philosopher is in their eating state. Must
		happen for at least 3 seconds.

*/
void philo_eat() {

	sleep(EAT_TIME);

}


/* 	Description:

		Function is called when a philosopher is transitions from THINKING to HUNGRY.
		Here, philospher tries to take two adjacent forks, and if can, enters EATING state.
		If cannot, waits for adjacent philosphers to give up forks for them. 
		
	Arguments:
	
		<int i> : The philospher's ID. 
		<struct shared_memory* shared_mem> : The struct of shared memory shared between philosopher processes.
		
*/
void take_forks(int i, struct shared_memory* shared_mem) {

	// Philo enters crit region
	sem_wait(&shared_mem -> mutex);

	// Set state of philo to hungry, and test if it can get forks
	shared_mem -> philo_state[i] = HUNGRY;

	test_forks(i, shared_mem);

	// Exit crit. reg., and wait if forks weren't acquired
	sem_post(&shared_mem -> mutex);
	sem_wait(&shared_mem -> philo_sem[i]);
	

}


/*	Description:

		Function is called when a philosopher transitions from EATING to THINKING.
		When philospher gives up forks, lets adjacent philosophers know the forks are free
		and sees if they can enter their eating state now if they are HUNGRY.  
		
	Arguments: 
	
		<int i> : the philosopher's ID. 
		<struct shared_memory* shared_mem> : The struct of shared memory shared between philosopher processes.

*/
void put_forks(int i, struct shared_memory* shared_mem) {

		// Philo enters crit reg
		sem_wait(&shared_mem -> mutex);
		
		// Set philo to thinking if not completing 4th cycle, otherwise set FINISHED. 
		// Also, let adjacent philos use forks if they are hungry.
		shared_mem -> philo_state[i] = (shared_mem -> philo_cycles[i] == (MAX_CYCLES - 1)) ? FINISHED : THINKING; 
		test_forks(LEFT, shared_mem);
		test_forks(RIGHT, shared_mem);

		// Increment philosopher's total cycles completed
		shared_mem -> philo_cycles[i]++;

		// Exit philos crit reg
		sem_post(&shared_mem -> mutex);

}


/* Description:

		Function is called by philosopher when it checks to see if adjacent forks are free (adjacent
		philosophers are not currently eating). If they are, AND philo is HUNGRY, semaphores set so that it enters eating state. 
		If not not free, semaphores in take_forks() function make the philospher wait. If philo was not hungry, do nothing.
		
	Arguments:

		<int i> : the philosopher's ID. 
		<struct shared_memory* shared_mem> : The struct of shared memory shared between philosopher processes.
	
*/
void test_forks(int i, struct shared_memory* shared_mem) {

	// If calling philosopher is curerntly hungry, check if adjacent philos are eating.
	// If both are not, let calling philosopher eat and increment philo's semaphore so it doesn't end up blocking itself
	int this_state = shared_mem -> philo_state[i];
	int left_state = shared_mem -> philo_state[LEFT];
	int right_state = shared_mem -> philo_state[RIGHT];
	if (this_state == HUNGRY && left_state != EATING && right_state != EATING) {
		shared_mem -> philo_state[i] = EATING;
		sem_post(&shared_mem -> philo_sem[i]);
	}

}


/* 	Description:

		Function is called by the printer process. Prints statuses of the 5 philosophers
		every second. 
		
		Stops printing when all philosphers have reached the finished state.
		
	Arguments:
	
		<struct shared_memory* shared_mem> : Struct of shared memory between philospher processes. */
void printer_process(struct shared_memory* shared_mem) {

	
	int print_info = 1;
	while (print_info) {

		// Iterate through philo states and print out. 
		for (int i = 0; i < PHILO_COUNT; i++) {

			char* state_string;
			switch (shared_mem -> philo_state[i]) {

				case THINKING:
					state_string = "thinking (0)";
					break;
				case HUNGRY:
					state_string = "hungry   (1)";
					break;

				case EATING:
					state_string = "EATING   (2)";
					break;

				case FINISHED:
					state_string = "finished (4)";
					break;
			}


			printf("P%d %s  [%d]\t", i, state_string, shared_mem -> philo_cycles[i]);
		}

		
		// Iterate through philo cycles and print out
		printf("\tPHILO CYCLES: [");
		for (int i = 0; i < PHILO_COUNT; i++) {
			printf(" %d ", shared_mem -> philo_cycles[i]);
		}
		printf("]\n");

		// Check if all philos are in finished state. If so, stop loop. 
		if (check_finished(shared_mem -> philo_state)) {
			print_info = 0;
		}

		// Pause for 1 second
		sleep(1);

	} 

	// End process here
	_exit(0);
	
}


/*	Description:

		Function checks if all the states passed through parameters are all 
		in FINISHED state. 
		
	Arguments:
	
		<int states[PHILO_COUNT]> : The integer array representative of the philosopher's current states. 
		
	Returns: 1 if states are all FINISHED, 0 otherwise. */
int check_finished(int states[PHILO_COUNT]) {

	int ret_val = 1;
	for (int i = 0; i < PHILO_COUNT; i++) {
		if (states[i] != FINISHED){
			ret_val = 0;
		}
	}
	return ret_val;

}


/* Description:

	Function that gets called by the parent process.
	For now, prints a declaration of its identity. 

*/
void parent() {
	
	printf("PARENT: All philosophers have finished eating.\n");

}