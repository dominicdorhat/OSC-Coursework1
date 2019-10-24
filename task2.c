// consumer producer problem

#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include <pthread.h>
#include "coursework.h"
#include "linkedlist.h"

sem_t sync, delay_consumer;

int items, val1, val2, flag;

void visualise() {
	printf("iIndex = %d\n", items);
}

// consumer
void * consumer(void * p) {
	int temp; 

	sem_wait(&delay_consumer);

	//consumes job
	for (int i = 0; i < MAX_NUMBER_OF_JOBS; i++) {
	//  while(1) {
		sem_wait(&sync);
		sem_getvalue(&delay_consumer, &val2);
		printf("DelayConsumer after sem_wait() in consumer after while(1)= %d\n", val2);


		// critical section
		items--;
		temp = items;
		
		// visualise(); // decide if calling this fcuntion shuold be in critical section or not
		printf("consumer: %d\n", items);
		// end of critical section
		sem_post(&sync);

		if (temp == 0) {			
			sem_getvalue(&delay_consumer, &val2);
			printf("Items = %d\n", items);
			printf("temp= %d\n", temp);
			printf("DelayConsumer before sem_wait() in consumer (temp == 0)= %d\n", val2);			
			
			

			sem_wait(&delay_consumer);

			// wont be printed as thread will sleep
			sem_getvalue(&delay_consumer, &val2);
			printf("DelayConsumer after sem_wait() in consumer (temp == 0)= %d\n", val2);

		}
		
	}
}

// producer
void * producer() {
	
	// produces job
	for (int i = 0; i < MAX_NUMBER_OF_JOBS; i++) {

		// critical section
		sem_wait(&sync);
		// sem_getvalue(&sync, &val2);
		// printf("sync after sem_wait() in producer after for loop = %d\n", val2);

		items++;
		// visualise();
		printf("producer: %d\n", items);

		if (items == 1) { // wakeup up consumer		
			sem_getvalue(&delay_consumer, &val2);
			printf("DelayConsumer before sem_post() in producer (items == 1) = %d\n", val2); // should be -1
	
			sem_post(&delay_consumer);
			sem_getvalue(&delay_consumer, &val2);
			printf("DelayConsumer after sem_post() in producer(items == 1) = %d\n", val2);

		}
		// sem_getvalue(&delay_consumer, &val);
		// printf("delay_cons: %d\n", val);

		// end of critical section
		// sem_getvalue(&delay_consumer, &val2);
		// printf("DelayConsumer before sem_post() in producer  end of producer= %d\n", val2);
		printf("prod: i: %d\n", i);
		
		sem_post(&sync);
	}			
}

int main() {
    pthread_t prodThread, consThread;
	sem_init(&sync, 0, 1);

	// TODO: decide initial semaphore value
	sem_init(&delay_consumer, 0, 0);
	printf("create thread\n");
	pthread_create(&prodThread, NULL, producer, NULL);
	pthread_create(&consThread, NULL, consumer, NULL);
	printf("before producer\n");
	
	pthread_join(prodThread, NULL);
	printf("producer done\n");
	int vall = pthread_join(consThread, NULL);
	printf("val1 = %d\n", vall);
	

	// print final semaphore values
	sem_getvalue(&sync, &val1);
	sem_getvalue(&delay_consumer, &val2);
	printf("sSync = %d, sDelayConsumer = %d\n", val1, val2);

	return 0;

}

