// authors: Tan Song Ning, Dominic Alphonsus Dorhat

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <semaphore.h>
#include <pthread.h>
#include "coursework.h"
#include "linkedlist.h"

sem_t sync, full, sleep_producer;

struct element *queue = NULL;
struct element *queueTail = NULL;

int fullVal, produced, consumed;

// TODO: decide how to exit the 3 consumers 

// consumer
void * consumer(void * p) {

	//consumes job
	for (int i = 0; i < MAX_NUMBER_OF_JOBS / NUMBER_OF_CONSUMERS; i++) {
    // while(1) {        
        if (consumed == 100) {
            break;
        }
		sem_wait(&full);
		sem_wait(&sync);		

		// critical section
		removeFirst(&queue, &queueTail);
		consumed++;		
        sem_getvalue(&full, &fullVal);
		printf("Consumer %d, Produced = %d, Consumed = %d: Items in buffer = %d\n", *((int *)p), produced, consumed, fullVal);
		// visualise(); 
		// end of critical section

		sem_post(&sync);
		
		sem_getvalue(&full, &fullVal);
		if (fullVal == (MAX_BUFFER_SIZE - 1)) {
			sem_post(&sleep_producer); // wake consumer the momenr there's available space
		}		

	}	
}

// producer
void * producer(void * p) {
    struct process *newProcess = generateProcess();

	// produces job
	for (int i = 0; i < MAX_NUMBER_OF_JOBS; i++) {		
		sem_getvalue(&full, &fullVal);

		if(fullVal == MAX_BUFFER_SIZE - 1 ) {
			sem_wait(&sleep_producer); // sleep producer at 50 items in buffer
		}		
		sem_wait(&sync);

		// critical section
        addLast(newProcess, &queue, &queueTail); 
		produced++;		
		printf("Producer %d, Produced = %d, Consumed = %d: \n", 1, produced, consumed);
        
		// end of critical section		
		
        sem_post(&sync);
		sem_post(&full);
        sem_getvalue(&full, &fullVal);
        printf("(Produced) Items in buffer = %d\n", fullVal);

	}			
}

int main() {

	pthread_t prodThread, consThread[NUMBER_OF_CONSUMERS];
	int syncVal, sleep_producerVal;
    int consumerID[NUMBER_OF_CONSUMERS] = {1, 2, 3};

	sem_init(&sync, 0, 1); // mutual exclusion
	sem_init(&sleep_producer, 0, 0); // binary sem for sleeping producer
	sem_init(&full, 0, 0); // counting semaphore

	pthread_create(&prodThread, NULL, producer, NULL); 
	
    for(int i = 0; i < NUMBER_OF_CONSUMERS; i++) 
        pthread_create(&consThread[i], NULL, consumer, &consumerID[i]);

    pthread_join(prodThread, NULL);

    for(int i = 0; i < NUMBER_OF_CONSUMERS; i++)
        pthread_join(consThread[i], NULL);	
		
	// print final semaphore values
	sem_getvalue(&sync, &syncVal);
	sem_getvalue(&sleep_producer, &sleep_producerVal);
	sem_getvalue(&full, &fullVal);
	printf("sSync = %d, sSleepProducer = %d, sFull = %d\n", syncVal, sleep_producerVal, fullVal);
	
	return 0;

}

