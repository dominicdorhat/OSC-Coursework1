// authors: Tan Song Ning, Dominic Alphonsus Dorhat

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <semaphore.h>
#include <pthread.h>
#include "coursework.h"
#include "linkedlist.h"

struct listPointers {
    struct element * head;
    struct element * tail;
};

sem_t sync, full, sleep_producer;

struct listPointers record[MAX_PRIORITY];

// TODO: decide proper place to define these 2 structs
struct timeval startTime;
struct timeval endTime;

int fullVal, produced, consumed, flag[3], producedFlag;

// TODO: decide how to exit the 3 consumers 

int queueIterator(struct listPointers * record) {
    for(int i = 0; i < MAX_PRIORITY; i++) {
        if (record[i].head != NULL) {
            return i;
        }
    }
}

// consumer
void * consumer(void * p) {

	//consumes job
	// for (int i = 0; i < MAX_NUMBER_OF_JOBS / NUMBER_OF_CONSUMERS; i++) {
    while(consumed < NUMBER_OF_JOBS) {        
        
        sem_wait(&full); // not just as a counter, but to sleep consumers when there's nth in the buffer (prevent busy waiting)
        sem_wait(&sync);

        int syncVal, temp;
        sem_getvalue(&full, &fullVal);
        sem_getvalue(&sync, &syncVal);
        printf("\n(Consumer %d) i got the semaphore, with fullVal: %d and sync: %d\n", *((int * )p), fullVal, syncVal);
        temp = fullVal;
                		
        // for (int j = 0; flag[*((int *)p)] == 0 && j < MAX_PRIORITY; j++) {
        // if (producedFlag == 0){ // enter only if producer not producing anything 
            // for (int j = 0; producedFlag == 0 && j < MAX_PRIORITY; j++) {

            //     // printf("i got the semaphore i should do smth 1\n");
            //     if (record[j].head != NULL) {
            //         sem_wait(&full);
            //         // sem_wait(&sync);

            //         int syncVal;
            //         sem_getvalue(&full, &fullVal);
            //         printf("\n(Consumer %d) i got the semaphore, with fullVal: %d", *((int * )p), fullVal);

            //         // printf("i got the semaphore i should do smth 2\n");
            //         printf("\n(Consumer %d says): I am going to consume!!!! at priority queue: %d ",*((int *)p), j);
            //         printf("\t|\tiPriority actually consumed: %d\n", ((struct process *) (record[j].head->pData))->iPriority);
                    
            //         // TODO: change to runJob when we include RR
            //         runPreemptiveJob((struct process *) (record[j].head->pData), &startTime, &endTime);
            //         // free(record[j].head->pData);
            //         removeFirst(&record[j].head, &record[j].tail);
            //         // sem_wait(&full);

            //         consumed++;		
            //         sem_getvalue(&full, &fullVal);
            //         printf("Consumer %d, Produced = %d, Consumed = %d: Items in buffer = %d\n", *((int *)p), produced, consumed, fullVal);

            //         //sem_post(&sync); // TODO: decide when to sem_post()

            //         sem_getvalue(&full, &fullVal);
            //         if (fullVal == (MAX_BUFFER_SIZE - 1)) { // 9 
            //             int sleepVal; // TODO: remove later
            //             sem_getvalue(&sleep_producer, &sleepVal);
            //             printf("(items at  9)sleepProducer semaphore before sem_post = %d\n", sleepVal);
            //             sem_post(&sleep_producer); // wake producer the moment there's available space in a queue
            //         }		
            //     }   
            // }
        sem_getvalue(&full, &fullVal);

        int index;
        index = queueIterator(record); // find job to eat
        printf("after china function, index = %d\n", index);

        printf("\n(Consumer %d says): I am going to consume!!!! at priority queue: %d ",*((int *)p), index);
        printf("\t|\tiPriority actually consumed: %d\n", ((struct process *) (record[index].head->pData))->iPriority);

        runPreemptiveJob((struct process *) (record[index].head->pData), &startTime, &endTime);
        removeFirst(&record[index].head, &record[index].tail);
        consumed++;
        sem_getvalue(&full, &fullVal);
        printf("Consumer %d, Produced = %d, Consumed = %d: Items in buffer = %d\n", *((int *)p), produced, consumed, fullVal);

        sem_getvalue(&full, &fullVal);
        if (temp == (MAX_BUFFER_SIZE - 1)) { // 9         
            int sleepVal; // TODO: remove later
            sem_getvalue(&sleep_producer, &sleepVal);
            printf("(items at  9)sleepProducer semaphore before sem_post = %d\n", sleepVal);
            sem_post(&sleep_producer); // wake producer the moment there's available space in a queue
        }


        // } else { sem_post(&full); } // post back the full semaphore (because of reset + not actually consuming anything)

        // flag[*((int * )p)] = 0;
        // flag[0] = 0, flag[1] = 0, flag[2] = 0;
        // producedFlag = 0;
        // printf("i got the semaphore i should do smth 3: (consumerid: %d), flag is %d\n", *((int *)p), producedFlag);
        sem_post(&sync);

        sem_getvalue(&sync, &syncVal);
        printf("(Consumer %d) i no longer have the semaphore, sem sync value is: %d, flag is %d\n", *((int * )p), syncVal, producedFlag);

        printf("Consumed: %d\n", consumed);
        if (consumed == 100) break;
	}	
    printf("Consumer has consumed a 100!!!\n");    
}

// producer
void * producer(void * p) {
    struct process *newProcess;

	// produces job
	for (int i = 0; i < 100; i++) {		
		sem_getvalue(&full, &fullVal);
        int sleepVal;
        sem_getvalue(&sleep_producer, &sleepVal);

		if(fullVal == MAX_BUFFER_SIZE) {
			sem_wait(&sleep_producer); // sleep producer at 10 items in buffer
		}		

		printf("\n(Producer) production starting... sync: %d, sleepVal: %d\n", fullVal, sleepVal);
        sem_wait(&sync);

		// critical section
        newProcess= generateProcess();
        addLast(newProcess, &record[newProcess->iPriority].head , &record[newProcess->iPriority].tail); 

		produced++;		
		printf("Producer %d, Produced = %d, Consumed = %d: priority level added: %d\n", 1, produced, consumed, newProcess->iPriority);
        
		// end of critical section		
		
         int syncVal;
        sem_getvalue(&sync, &syncVal);
        sem_getvalue(&full, &fullVal);
        printf("(Producer Report: )producer sync semahore: %d + 1, full semaphore/items in the buffer: %d + 1\n", syncVal, fullVal);
        sem_post(&sync);        
        sem_post(&full);

        // TODO: decide flag
        // flag[0] = 1, flag[1] = 1, flag[2] = 1;
        // producedFlag = 1;
        // sem_getvalue(&full, &fullVal);
        // printf("(Produced) Items in buffer = %d\n", fullVal);

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
    printf("producer sudah join!!!\n");

    for(int i = 0; i < NUMBER_OF_CONSUMERS; i++)
        pthread_join(consThread[i], NULL);	

    printf("consumer sudah join!!!\n");
		
	// print final semaphore values
	sem_getvalue(&sync, &syncVal);
	sem_getvalue(&sleep_producer, &sleep_producerVal);
	sem_getvalue(&full, &fullVal);
	printf("sSync = %d, sSleepProducer = %d, sFull = %d\n", syncVal, sleep_producerVal, fullVal);
	
	return 0;

}

