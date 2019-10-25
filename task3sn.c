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
	int temp; 

	sem_wait(&delay_consumer);

	//consumes job
	for (int i = 0; i < MAX_NUMBER_OF_JOBS; i++) {
	
		sem_wait(&sync);		
		//
		// critical section
		
		//REMOVE STARS BY REMOVEFIRST OF LINKED LIST
		items--;
		visualise(); 		
		// end of critical section
		sem_post(&sync);

		if (temp == 0) {					
			if(i != (MAX_NUMBER_OF_JOBS - 1)) {
				sem_wait(&delay_consumer);
			} 		
		}
		
	}	
}

// producer
void * producer() {
	
	// produces job
	while(1){

		// critical section
		sem_wait(&sync);		
		//GENERATE STARS INTO NODE + ADD NODE TO LAST OF LINKED LIST
		items++;
		visualise();		

		if (items == 1) { // wakeup up consumer			
			sem_post(&delay_consumer);			
		}

		// end of critical section		
		sem_post(&sync);
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

