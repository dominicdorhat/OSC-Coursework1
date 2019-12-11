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

sem_t sync, full, sleep_producer, empty;

struct listPointers record[MAX_PRIORITY];

// TODO: decide proper place to define these 2 structs
struct timeval startTime;
struct timeval endTime;

int fullVal, produced, consumed, hyperIndex = 0, dirtyBit = 0;

// TODO: decide how to exit the 3 consumers 

struct process * queueIterator(struct listPointers * record) {
	struct process * jobToConsume;

    for(int i = hyperIndex; i < MAX_PRIORITY & dirtyBit == 0; i++) {
        if (record[i].head != NULL) {
			jobToConsume = (struct process *) record[i].head->pData;
			removeFirst(&record[i].head, &record[i].tail);
			hyperIndex = i;
            return jobToConsume;
        }
    }
	hyperIndex = 0;
	dirtyBit = 0;
    return NULL;
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
		printf("hi Consumer %d, Process Id = %d (%s), Priority = %d, Previous Burst Time = %d, Remaining Burst Time = %d, Response Time = %d, Turnaround Time = %d, Consumed = %d\n", 
		iConsumerId, pProcess->iProcessId, 
		pProcess->iPriority < MAX_PRIORITY / 2 ? "FCFS" : "RR", pProcess->iPriority, 
		pProcess->iPreviousBurstTime, 
		pProcess->iRemainingBurstTime, 
		iResponseTime, 
		iTurnAroundTime, consumed);

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
		printf("Consumer %d, Process Id = %d (%s), Priority = %d, Previous Burst Time = %d, Remaining Burst Time = %d, Turnaround Time = %d consumed = %d\n", 
		iConsumerId, 
		pProcess->iProcessId, 
		pProcess->iPriority < MAX_PRIORITY / 2 ? "FCFS" : "RR", 
		pProcess->iPriority, 
		pProcess->iPreviousBurstTime, 
		pProcess->iRemainingBurstTime, 
		iTurnAroundTime, consumed		);
		free(pProcess);
		
		return NULL;
	}
}

// void printSem(char* semaphore_name){
// 	if(semaphore_name == "sync"){
// 		sem_getvalue(&sync,&syncVal);
// 		printf("Sync is %d---------\n",syncVal);
// 	} else if(semaphore_name == "full"){
// 		sem_getvalue(&full,&fullVal);
// 		printf("Full is %d---------\n",fullVal);
// 	}
// }

// consumer
void * consumer(void * p) {
    int syncVal, temp, sleepVal;
	
	
    while(1) {        
        
        struct process * job;
		struct process * jobToConsume;
		int index = -1;


        sem_wait(&full); 
		sem_getvalue(&full,&fullVal);
		printf("Full is %d---------\n",fullVal);
        
		sem_wait(&sync);          		
        jobToConsume = queueIterator(record); 
		sem_post(&sync);
		
        if (jobToConsume != NULL){ //if there is sth to consume
			index = jobToConsume->iPriority;
            runJob(jobToConsume, &startTime, &endTime);
            job = processJob( *(int * )p, jobToConsume, startTime, endTime);
			
            
            if (job != NULL) {
	
				sem_wait(&sync);
                addLast(job, &record[index].head , &record[index].tail); 
				sem_post(&sync);

                sem_post(&full);

            } else {                
                consumed++;
            }

		} else {
			sem_post(&full);	
		}

		sem_post(&empty);

        if (consumed >= 100) break;
	}	
     sem_post(&full);
	 return NULL;
}

// producer
void * producer(void * p) {
    struct process *newProcess;

	
    while(1) {

        sem_wait(&empty);    
		// critical section
        newProcess = generateProcess();
		
		sem_wait(&sync);
		dirtyBit = 1;
        addLast(newProcess, &record[newProcess->iPriority].head , &record[newProcess->iPriority].tail); 
		sem_post(&sync);

		produced++;		
		// printf("Producer %d, Produced = %d, Consumed = %d: priority level added: %d\n", 1, produced, consumed, newProcess->iPriority);
        printf("Producer %d, Process Id = %d (%s), Priority = %d, Initial Burst Time = %d\n", 
		0, 
		newProcess->iProcessId,
		newProcess->iPriority < MAX_PRIORITY / 2 ? "FCFS" : "RR", 
		newProcess->iPriority, 
		newProcess->iInitialBurstTime);
        
		// end of critical section	
		sem_post(&full);
      
        if(produced == MAX_NUMBER_OF_JOBS) break;
	}		
	return NULL;
}

int main() {

	pthread_t prodThread, consThread[NUMBER_OF_CONSUMERS];
	int syncVal, sleep_producerVal;
    int consumerID[NUMBER_OF_CONSUMERS] = {0, 1, 2};

	sem_init(&sync, 0, 1); // mutual exclusion
	sem_init(&sleep_producer, 0, 0); // binary sem for sleeping producer
	sem_init(&full, 0, 0); // counting semaphore
    sem_init(&empty, 0, 100);

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


