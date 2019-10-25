// Bounded consumer producer problem

#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include <pthread.h>
#include "coursework.h"
#include "linkedlist.h"

sem_t sync,full,empty;

int items, syncVal, fullVal, emptyVal;

void visualise() {
	printf("iIndex = %d\n", items);
}

// consumer
void * consumer(void * p) {

	//consumes job
	while(1){
	
		sem_wait(&full);
		sem_wait(&sync);		
		// critical section
		//REMOVE STARS BY REMOVEFIRST OF LINKED LIST
		items--;
		visualise(); 		
		// end of critical section
		sem_post(&sync);
		sem_post(&empty);
		
	}	
}

// producer
void * producer() {
	
	// produces job
	while(1){
		sem_wait(&empty);
		sem_wait(&sync);	
		// critical section
		//GENERATE STARS INTO NODE + ADD NODE TO LAST OF LINKED LIST
		items++;
		visualise();		
		// end of critical section		
		sem_post(&sync);
		sem_post(&full);
	}				
}

int main() {
    pthread_t prodThread, consThread;
	sem_init(&sync, 0, 1);
	sem_init(&empty, 0, MAX_BUFFER_SIZE);
	sem_init(&full, 0, 0);
	
	pthread_create(&prodThread, NULL, producer, NULL);
	pthread_create(&consThread, NULL, consumer, NULL);
	
	pthread_join(prodThread, NULL);
	pthread_join(consThread, NULL);	
		
	// print final semaphore values
	sem_getvalue(&sync, &syncVal);
	sem_getvalue(&empty, &emptyVal);
	sem_getvalue(&full, &fullVal);
	printf("sSync = %d, sEmpty = %d, sFull = %d\n", syncVal, emptyVal, fullVal);

	return 0;

}

