// consumer producer problem

#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include <pthread.h>
#include "coursework.h"
#include "linkedlist.h"

sem_t sync, delay_consumer;

int items, val1, val2;

void visualise() {
	printf("iIndex = %d\n", items);
}

// consumer
void * consumer(void * p) {
	int temp; 

	sem_wait(&delay_consumer);

	//consumes job
	for (int i = 0; i < MAX_NUMBER_OF_JOBS; i++) {
	// while(1) {
		sem_wait(&sync);

		// if (temp == 0) {
		// 	sem_wait(&delay_consumer);
		// } else {
		// 	sem_post(&delay_consumer);
		// }
		
		// critical section
		items--;
		temp = items;
		
		// visualise(); // decide if calling this fcuntion shuold be in critical section or not
		printf("consumer: %d\n", items);
		// end of critical section
		sem_post(&sync);

		if (temp == 0) {			
			sem_getvalue(&delay_consumer, &val2);
			printf("DelayConsumer before sem_wait()= %d\n", val2);
			// TODO: problem where consumer eats non-existing items, delay_consumer semaphore is 1 before sem_wait, therefore consumer wont sleep
			sem_wait(&delay_consumer);
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
		// visualise();
		printf("producer: %d\n", items);

		if (items == 1) { // wakeup up consumer			
			sem_post(&delay_consumer);
		}
		// sem_getvalue(&delay_consumer, &val);
		// printf("delay_cons: %d\n", val);

		// end of critical section
		sem_post(&sync);
	}			
}

int main() {
    pthread_t prodThread, consThread;
	sem_init(&sync, 0, 1);
	sem_init(&delay_consumer, 0, 1);
	pthread_create(&prodThread, NULL, producer, NULL);
	pthread_create(&consThread, NULL, consumer, NULL);
	pthread_join(prodThread, NULL);
	pthread_join(consThread, NULL);

	// print final semaphore values
	sem_getvalue(&sync, &val1);
	sem_getvalue(&delay_consumer, &val2);
	printf("sSync = %d, sDelayConsumer = %d\n", val1, val2);

}

