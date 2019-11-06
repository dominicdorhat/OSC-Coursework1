// Bounded consumer producer problem

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <semaphore.h>
#include <pthread.h>
#include "coursework.h"
#include "linkedlist.h"

sem_t sync,full,sleep_producer;
struct element *queue = NULL;
struct element *queueTail = NULL;
int items, syncVal, fullVal, sleep_producerVal, produced, consumed;

void visualise() {
	struct element *queueCur = NULL; 
	queueCur = queue;

	for(; queueCur != NULL; queueCur = queueCur->pNext) {
		printf("%c", (char *) (queueCur->pData));
	}
	printf("\n");
}

// consumer
void * consumer(void * p) {

	//consumes job
	for (int i = 0; i < MAX_NUMBER_OF_JOBS; i++) {
	// while(1){
		sem_wait(&full);
		sem_wait(&sync);		

		// critical section
		removeFirst(&queue, &queueTail);
		items--;
		consumed++;		
		printf("Consumer %d, Produced = %d, Consumed = %d: ", 1, produced, consumed);
		visualise(); 
		// end of critical section

		sem_post(&sync);
		// if (i != MAX_NUMBER_OF_JOBS/NUMBER_OF_CONSUMERS - 1) {
		
		sem_getvalue(&full, &fullVal);
		if (fullVal == 49) {
			sem_post(&sleep_producer);
		}
		
	}	
}

// producer
void * producer(void * p) {
	char * star = (char *) '*'; // create star

	// produces job
	for (int i = 0; i < MAX_NUMBER_OF_JOBS; i++) {
	//while(1){
		
		sem_getvalue(&full, &fullVal);

		if(fullVal == 50 ) {
			sem_wait(&sleep_producer);
		}		
		sem_wait(&sync);

		// critical section
        addLast( star, &queue, &queueTail); 
		items++;
		produced++;		
		printf("Producer %d, Produced = %d, Consumed = %d: ", 1, produced, consumed);
		visualise();

		// end of critical section		
		sem_post(&sync);
		sem_post(&full);
	}				
}

int main() {

	pthread_t prodThread, consThread;

	sem_init(&sync, 0, 1); 
	sem_init(&sleep_producer, 0, 0); // binary sem for sleeping producer
	sem_init(&full, 0, 0); // counting semaphore

	pthread_create(&prodThread, NULL, producer, NULL); 
	pthread_create(&consThread, NULL, consumer, NULL );

	pthread_join(prodThread, NULL);	
	pthread_join(consThread, NULL);
		
	// print final semaphore values
	sem_getvalue(&sync, &syncVal);
	sem_getvalue(&sleep_producer, &sleep_producerVal);
	sem_getvalue(&full, &fullVal);
	printf("sSync = %d, sSleepProducer = %d, sFull = %d\n", syncVal, sleep_producerVal, fullVal);
	
	return 0;

}

