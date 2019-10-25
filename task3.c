// Bounded consumer producer problem

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <semaphore.h>
#include <pthread.h>
#include "coursework.h"
#include "linkedlist.h"

sem_t sync,full,empty;
struct element *queue = NULL;
struct element *queueTail = NULL;
int items, syncVal, fullVal, emptyVal, produced, consumed;

void visualise() {
	struct element *queueCur = NULL; 
	queueCur = queue;
	// while (queueCur != NULL ){
	// 	printf("\t iIndex = %d\n", items);
	// }

	for(; queueCur != NULL; queueCur = queueCur->pNext) {
		printf("%c", (char *) (queueCur->pData));
	}
	printf("\n");
}

// consumer
void * consumer(void * p) {
	int consumer_id = *(int *) p; 

	//consumes job
	for (int i = 0; i < MAX_NUMBER_OF_JOBS/NUMBER_OF_CONSUMERS; i++) {
	// while(1){
		sem_wait(&full);
		sem_wait(&sync);		
		// critical section
		removeFirst(&queue, &queueTail);
		items--;
		consumed++;		
		printf("Consumer id: %d, Produced = %d, Consumed = %d ", consumer_id, produced, consumed);
		visualise(); 
		// end of critical section
		sem_post(&sync);
		if (i != MAX_NUMBER_OF_JOBS/NUMBER_OF_CONSUMERS - 1) {
			sem_post(&empty);
		}
		
	}	
}

// producer
void * producer(void * p) {
	int producer_id = *(int *) p;
	char * star = (char *) '*'; // create star

	// produces job
	for (int i = 0; i < MAX_NUMBER_OF_JOBS; i++) {
	//while(1){
		sem_wait(&empty);
		sem_wait(&sync);	
		// critical section
        addLast( star, &queue, &queueTail); 
		items++;
		produced++;		
		printf("Producer id: %d, Produced = %d, Consumed = %d ", producer_id, produced, consumed);
		visualise();
		// end of critical section		
		sem_post(&sync);
		sem_post(&full);
	}				
}

int main() {

    pthread_t prodThreadArr[NUMBER_OF_PRODUCERS], consThreadArr[NUMBER_OF_CONSUMERS];
	int consID[NUMBER_OF_CONSUMERS] = {0,1,2,3};
	int prodID[NUMBER_OF_PRODUCERS] = {0};

	sem_init(&sync, 0, 1);
	sem_init(&empty, 0, 50); //PRODUCER SLEEPS ON BINARY SEM
	sem_init(&full, 0, 0);

	for (int i = 0; i < NUMBER_OF_PRODUCERS ; i++ ) {
		pthread_create(&prodThreadArr[i], NULL, producer, (void *) &(prodID[i]) ); 
	}

	for (int i = 0; i < NUMBER_OF_CONSUMERS; i++){
		pthread_create(&consThreadArr[i], NULL, consumer, (void * ) &(consID[i]) );
	}

	for (int i = 0; i < NUMBER_OF_PRODUCERS; i++)
		pthread_join(prodThreadArr[i], NULL);
	
	for (int i = 0; i < NUMBER_OF_CONSUMERS; i++)
		pthread_join(consThreadArr[i], NULL);
		
	// print final semaphore values
	sem_getvalue(&sync, &syncVal);
	sem_getvalue(&empty, &emptyVal);
	sem_getvalue(&full, &fullVal);
	printf("sSync = %d, sEmpty = %d, sFull = %d\n", syncVal, emptyVal, fullVal);
	
	return 0;

}

