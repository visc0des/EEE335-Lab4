#pragma once

#include <pthread.h>
#include <semaphore.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>


#define SPOOLER_SIZE 10			// number of items in the printer spooler
#define BUFFER_SIZE 1000		// size of the buffer of char used to generate the print jobs


void print_job(int myid);
void produce_job(int myid, int job);
