// consumer producer problem

#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include <pthread.h>
#include "coursework.h"
#include "linkedlist.h"

sem_t sync, delay_consumer;
struct element *queue = NULL;
struct element *queueCur = NULL;
struct element *queuePrev = NULL;
struct element *queueTail = NULL;
int items, val;

void visualise() {
	printf("iIndex = %d\n", items);
}

// consumer
void * consumer(void * p) {
	int temp; 

	sem_wait(&delay_consumer);

	//consumes job
	for (int i = 0; i < NUMBER_OF_JOBS; i++) {
		sem_wait(&sync);
		

		items--;
		temp = items;
		visualise();

		sem_post(&sync);
		if (temp == 0) {
			sem_wait(&delay_consumer);
		}
	}
}

// producer
void * producer() {
	
	// produces job
	for (int i = 0; i < NUMBER_OF_JOBS; i++) {
		//struct process *oTemp = generateProcess(); // create process node         
		sem_wait(&sync);

		//addLast(oTemp, &queue, &queueTail);
		items++;
		visualise();
		if (items == 1) {
			sem_post(&delay_consumer);
		}
		sem_getvalue(&delay_consumer, &val);
		printf("delay_cons: %d\n", val);
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

}

