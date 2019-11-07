// authors: Tan Song Ning, Dominic Alphonsus Dorhat

#include <stdio.h>
#include <stdlib.h>
#include "coursework.h"
#include "linkedlist.h"

int main() {
    
    // linked list
    struct element *queue = NULL;
    struct element *queueCurrent = NULL;
	struct element *queuePrev = NULL;
    struct element *queueTail = NULL;

    int avgResponseTime, responseTime, avgTurnAroundTime = 0, turnAroundTime = 0;
    int numCompletedJobsTemp = 0, numCompletedJobs = 0;	

    struct timeval StartTime; 
    struct timeval EndTime; 

    // ready queue
    printf("PROCESS LIST:\n");
    for (int i = 0; i < NUMBER_OF_JOBS; i++) {
        struct process *newProcess = generateProcess();          
        addLast(newProcess, &queue, &queueTail);        
        
        printf("\tProcess Id = %d, Priority = %d, Initial Burst Time = %d, Remaining Burst Time = %d\n",
        ((struct process *) (queueTail -> pData)) -> iProcessId,
        ((struct process *) (queueTail -> pData)) -> iPriority,
        ((struct process *) (queueTail -> pData)) -> iInitialBurstTime,
        ((struct process *) (queueTail -> pData)) -> iRemainingBurstTime             
        );            

    }    
    queueCurrent = queue; // reset current pointer to head

    printf("END:\n\n");            

    // run 1st 10 jobs
    for(int j = 0; j < NUMBER_OF_JOBS; j++) {            
                    
        runPreemptiveJob((struct process *) (queueCurrent -> pData) , &StartTime, &EndTime);        
        
        responseTime = getDifferenceInMilliSeconds(((struct process *) (queue -> pData)) -> oTimeCreated, StartTime);   
        turnAroundTime = getDifferenceInMilliSeconds(((struct process *) (queue -> pData)) -> oTimeCreated, EndTime);

        printf("Process Id = %d, Priority = %d, Previous Burst Time = %d, Remaining Burst Time = %d, Response Time = %d\n",
            ((struct process *) (queueCurrent -> pData)) -> iProcessId, 
            ((struct process *) (queueCurrent -> pData)) -> iPriority,    
            ((struct process *) (queueCurrent -> pData)) -> iPreviousBurstTime,
            ((struct process *) (queueCurrent -> pData)) -> iRemainingBurstTime,            
            responseTime);

        avgResponseTime += responseTime;
                
		queuePrev = queueCurrent;
        queueCurrent = queueCurrent->pNext;
    }

    // continue running till every job finishes in RR fashion
    while(queue != NULL) {

		queuePrev = NULL; // reset previous pointer
		queueCurrent = queue; // reset current pointer

        // run (10 jobs - completed jobs) at a time
        for(int k = 0; k < (NUMBER_OF_JOBS - numCompletedJobsTemp); k++) {    
			runPreemptiveJob((struct process *) (queueCurrent -> pData) , &StartTime, &EndTime);                

			if (( ( (struct process *) (queueCurrent -> pData)) -> iRemainingBurstTime) == 0 ) {

                // first job finishes
				if (queueCurrent == queue) {
                    turnAroundTime = getDifferenceInMilliSeconds(((struct process *) (queue->pData))->oTimeCreated, EndTime);

					printf("Process Id = %d, Priority = %d, Previous Burst Time = %d, Remaining Burst Time = %d, Turn around Time = %d\n",
						((struct process *) (queueCurrent->pData))->iProcessId,
                        ((struct process *) (queueCurrent -> pData)) -> iPriority,
						((struct process *) (queueCurrent->pData))->iPreviousBurstTime,
						((struct process *) (queueCurrent->pData))->iRemainingBurstTime,
						turnAroundTime);

                    free(queue->pData);
					removeFirst(&queue, &queueTail);

					queueCurrent = queue;
                    avgTurnAroundTime += turnAroundTime;    

				} else { // middle job finishes
					struct element *queueTemp = NULL;

					turnAroundTime = getDifferenceInMilliSeconds(((struct process *) (queue->pData))->oTimeCreated, EndTime);

					printf("Process Id = %d, Priority = %d, Previous Burst Time = %d, Remaining Burst Time = %d, Turn around Time = %d\n",
						((struct process *) (queueCurrent->pData))->iProcessId,
                        ((struct process *) (queueCurrent -> pData)) -> iPriority,
						((struct process *) (queueCurrent->pData))->iPreviousBurstTime,
						((struct process *) (queueCurrent->pData))->iRemainingBurstTime,
						turnAroundTime);

					queuePrev->pNext = queueCurrent->pNext;
					queueTemp = queueCurrent;
					queueCurrent = queueCurrent->pNext;

                    free(queueTemp->pData);
					free(queueTemp);

                    avgTurnAroundTime += turnAroundTime;    
				}
			
				numCompletedJobs++;
		
			} else {
				 printf("Process Id = %d, Priority = %d, Previous Burst Time = %d, Remaining Burst Time = %d\n",
				((struct process *) (queueCurrent -> pData)) -> iProcessId,   
                ((struct process *) (queueCurrent -> pData)) -> iPriority,  
				((struct process *) (queueCurrent -> pData)) -> iPreviousBurstTime,
				((struct process *) (queueCurrent -> pData)) -> iRemainingBurstTime);

				 queuePrev = queueCurrent;
				 queueCurrent = queueCurrent->pNext;
			}    		
        }

			numCompletedJobsTemp = numCompletedJobs;
    }
    
    printf("Average response time = %.6f\n", (float) avgResponseTime / NUMBER_OF_JOBS);
    printf("Average turn around time = %.6f\n", (float) avgTurnAroundTime / NUMBER_OF_JOBS);    
}