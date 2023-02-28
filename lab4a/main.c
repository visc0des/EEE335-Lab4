/* 

	Description:

		Code for Lab 4 of the course EEE335.

	
	Notes: 

		- Changed print statements in util.c functions to make reading easier. 


	Status:




	Author: OCdt Velasco
	Version: started on February 28, 2023
	

*/




#include "util.h"
#include "freeforall.h"
#include "strict_alternation.h"
#include "peterson.h"
#include "semaphores.h"


// --- Declaring demo functions ---




int main(void) {


	while (true)
	{
		printf("Which mutual exclusion approach do you want to test?\n");
		printf("0. Free for all\n");
		printf("1. Strict Alternation\n");
		printf("2. Peterson's Solution\n");
		printf("3. Semaphore\n");
		printf("4. Quit\n");

		int choice;
		scanf("%d", &choice);

		if (choice == 0)
		{
			printf("Running Free-for-all\n");
			demo_freeforall();
		}
		if (choice == 1)
		{
			printf("Running Strict Alternation\n");
			demo_strict_alternation();
		}
		else if (choice == 2)
		{
			printf("Running Peterson's Solution\n");
			demo_peterson();
		}
		else if (choice == 3)
		{
			printf("Running Semaphore\n");
			demo_semaphores();

		}
		else if (choice == 4)
		{
			printf("Exiting program...\n");
			break;
		}
		else
		{
			;
		}
	}

	return EXIT_SUCCESS;
}
