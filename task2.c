// consumer producer problem

#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include <pthread.h>
#include "coursework.h"
#include "linkedlist.h"

sem_t sync, delay_consumer;

int items, syncVal, delayVal;

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
		
		// critical section
		items--;
		temp = items;
		
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
	for (int i = 0; i < MAX_NUMBER_OF_JOBS; i++) {

		// critical section
		sem_wait(&sync);		

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
	sem_init(&delay_consumer, 0, 0);
	
	pthread_create(&prodThread, NULL, producer, NULL);
	pthread_create(&consThread, NULL, consumer, NULL);
	
	pthread_join(prodThread, NULL);
	pthread_join(consThread, NULL);	
		
	// print final semaphore values
	sem_getvalue(&sync, &syncVal);
	sem_getvalue(&delay_consumer, &delayVal);
	printf("sSync = %d, sDelayConsumer = %d\n", syncVal, delayVal);

	return 0;

}

