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

int fullVal, produced, consumed, producedFlag;

// TODO: decide how to exit the 3 consumers 

int queueIterator(struct listPointers * record) {
    for(int i = 0; i < MAX_PRIORITY; i++) {
        if (record[i].head != NULL) {
            return i;
        }
    }
    return -1;
}

struct process * processJob(int iConsumerId, struct process * pProcess, struct timeval oStartTime, struct timeval oEndTime)
{
	int iResponseTime;
	int iTurnAroundTime;
    int dAverageResponseTime;
    int dAverageTurnAroundTime;
	
	// first time to be processed (FCFS and RR)
	if(pProcess->iPreviousBurstTime == pProcess->iInitialBurstTime && pProcess->iRemainingBurstTime > 0)
	{
		iResponseTime = getDifferenceInMilliSeconds(pProcess->oTimeCreated, oStartTime);	
		dAverageResponseTime += iResponseTime;
		printf("Consumer %d, Process Id = %d (%s), Priority = %d, Previous Burst Time = %d, Remaining Burst Time = %d, Response Time = %d\n", 
			iConsumerId, pProcess->iProcessId, 
			pProcess->iPriority < MAX_PRIORITY / 2	 ? "FCFS" : "RR",
			pProcess->iPriority, pProcess->iPreviousBurstTime, 
			pProcess->iRemainingBurstTime, 
			iResponseTime);

		return pProcess;

	// job finish processing (RR)
	} else if(pProcess->iPreviousBurstTime == pProcess->iInitialBurstTime && pProcess->iRemainingBurstTime == 0) {
		iResponseTime = getDifferenceInMilliSeconds(pProcess->oTimeCreated, oStartTime);	
		dAverageResponseTime += iResponseTime;
		iTurnAroundTime = getDifferenceInMilliSeconds(pProcess->oTimeCreated, oEndTime);
		dAverageTurnAroundTime += iTurnAroundTime;
		printf("Consumer %d, Process Id = %d (%s), Priority = %d, Previous Burst Time = %d, Remaining Burst Time = %d, Response Time = %d, Turnaround Time = %d\n", 
		iConsumerId, pProcess->iProcessId, 
		pProcess->iPriority < MAX_PRIORITY / 2 ? "FCFS" : "RR", pProcess->iPriority, 
		pProcess->iPreviousBurstTime, 
		pProcess->iRemainingBurstTime, 
		iResponseTime, 
		iTurnAroundTime);

		free(pProcess);

		return NULL;

	// job yet to be finished (RR)
	} else if(pProcess->iPreviousBurstTime != pProcess->iInitialBurstTime && pProcess->iRemainingBurstTime > 0) {
		printf("Consumer %d, Process Id = %d (%s), Priority = %d, Previous Burst Time = %d, Remaining Burst Time = %d\n", 
		iConsumerId, 
		pProcess->iProcessId,
		pProcess->iPriority < MAX_PRIORITY / 2 ? "FCFS" : "RR", 
		pProcess->iPriority, 
		pProcess->iPreviousBurstTime, 
		pProcess->iRemainingBurstTime);		

		return pProcess;

	// job finished (RR)
	} else if(pProcess->iPreviousBurstTime != pProcess->iInitialBurstTime && pProcess->iRemainingBurstTime == 0) {
		iTurnAroundTime = getDifferenceInMilliSeconds(pProcess->oTimeCreated, oEndTime);
		dAverageTurnAroundTime += iTurnAroundTime;
		printf("Consumer %d, Process Id = %d (%s), Priority = %d, Previous Burst Time = %d, Remaining Burst Time = %d, Turnaround Time = %d\n", 
		iConsumerId, 
		pProcess->iProcessId, 
		pProcess->iPriority < MAX_PRIORITY / 2 ? "FCFS" : "RR", 
		pProcess->iPriority, 
		pProcess->iPreviousBurstTime, 
		pProcess->iRemainingBurstTime, 
		iTurnAroundTime);
		free(pProcess);

		return NULL;
	}
}

// consumer
void * consumer(void * p) {

	//consumes job
    // while(consumed < NUMBER_OF_JOBS) {        
    while(1) {        
        
        struct process * job;
        sem_wait(&full); // not just as a counter, but to sleep consumers when there's nth in the buffer (prevent busy waiting)
        sem_wait(&sync);

        int syncVal, temp;
        sem_getvalue(&full, &fullVal);
        sem_getvalue(&sync, &syncVal);
        printf("\n(Consumer %d) i got the semaphore, with fullVal: %d and sync: %d\n", *((int * )p), fullVal, syncVal);
        temp = fullVal;
                		
        sem_getvalue(&full, &fullVal);

        int index;
        index = queueIterator(record); // find job to eat
        printf("after china function, index = %d\n", index);

        printf("\n(Consumer %d says): I am going to consume!!!! at priority queue: %d ",*((int *)p), index);
        // printf("\t|\tiPriority actually consumed: %d\n", ((struct process *) (record[index].head->pData))->iPriority);

        if (index >= 0){
            runJob((struct process *) (record[index].head->pData), &startTime, &endTime);
            job = processJob( *(int * )p, (struct process *) (record[index].head->pData), startTime, endTime);
            removeFirst(&record[index].head, &record[index].tail);
            
            if (job != NULL) {
                // RR addlast                 
                addLast(job, &record[index].head , &record[index].tail); 
                sem_post(&full);
                sem_getvalue(&full, &fullVal);
                temp = fullVal;
            } else {                
                consumed++;
            }
        } else {
            printf("nth? %d\n", index);
        }

        sem_getvalue(&full, &fullVal);
        // printf("Consumer %d, Produced = %d, Consumed = %d: Items in buffer = %d\n", *((int *)p), produced, consumed, fullVal);

        sem_getvalue(&full, &fullVal);
        if (temp == (MAX_BUFFER_SIZE - 1)) { // 9         
            int sleepVal; // TODO: remove later
            sem_getvalue(&sleep_producer, &sleepVal);
            printf("(items at  9)sleepProducer semaphore before sem_post = %d\n", sleepVal);
            sem_post(&sleep_producer); // wake producer the moment there's available space in a queue
        }

        sem_post(&sync);

        sem_getvalue(&sync, &syncVal);
        printf("(Consumer %d) i no longer have the semaphore, sem sync value is: %d, flag is %d\n", *((int * )p), syncVal, producedFlag);

        printf("Consumed: %d\n", consumed);
        if (consumed == 100) break;
	}	
    // printf("Consumer has consumed a 100!!!\n");    
     sem_post(&full);
}

// producer
void * producer(void * p) {
    struct process *newProcess;

	// produces job
	// for (int i = 0; i < MAX_NUMBER_OF_JOBS; i++) {		
    while(1) {
        int sleepVal, semwait;
        sem_getvalue(&sleep_producer, &sleepVal);
		sem_getvalue(&full, &fullVal);

		if(fullVal == MAX_BUFFER_SIZE) {
			sem_wait(&sleep_producer); // sleep producer at 10 items in buffer
		}		

		printf("\n(Producer) production starting... sync: %d, sleepVal: %d\n", fullVal, sleepVal);
        semwait = sem_wait(&sync);
		sem_getvalue(&full, &fullVal);
        sem_getvalue(&sleep_producer, &sleepVal);
        printf("\n(Producer) after sem-wait(sync).. sync: %d, sleepVal: %d, semwait: %d\n", fullVal, sleepVal, semwait);


		// critical section
        newProcess= generateProcess();
        addLast(newProcess, &record[newProcess->iPriority].head , &record[newProcess->iPriority].tail); 

		produced++;		
		// printf("Producer %d, Produced = %d, Consumed = %d: priority level added: %d\n", 1, produced, consumed, newProcess->iPriority);
        printf("Producer %d, Process Id = %d (%s), Priority = %d, Initial Burst Time = %d\n", 
		0, 
		newProcess->iProcessId,
		newProcess->iPriority < MAX_PRIORITY / 2 ? "FCFS" : "RR", 
		newProcess->iPriority, 
		newProcess->iInitialBurstTime);

        
		// end of critical section		
		
         int syncVal;
        sem_getvalue(&sync, &syncVal);
        sem_getvalue(&full, &fullVal);
        printf("(Producer Report: )producer sync semahore: %d + 1, full semaphore/items in the buffer: %d + 1\n", syncVal, fullVal);		
        
        sem_post(&full);
        sem_post(&sync);        
        if(produced == MAX_NUMBER_OF_JOBS) { break;}
	}			
}

int main() {

	pthread_t prodThread, consThread[NUMBER_OF_CONSUMERS];
	int syncVal, sleep_producerVal;
    int consumerID[NUMBER_OF_CONSUMERS] = {0, 1, 2};

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

