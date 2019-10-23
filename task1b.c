// ROUND ROBIN

#include <stdio.h>
#include <stdlib.h>
#include "coursework.h"
#include "linkedlist.h"

int main() {
    
    // linked list
    struct element *queue = NULL;
    struct element *queueCur = NULL;
	struct element *queuePrev = NULL;
    struct element *queueTail = NULL;

    int avgResponseTime, responseTime, avgTurnAroundTime, turnAroundTime;

// create job
    printf("PROCESS LIST:\n");
    for (int i = 0; i < NUMBER_OF_JOBS; i++) {
        struct process *oTemp = generateProcess(); // create first process node         
        addLast(oTemp, &queue, &queueTail);
        
        
        printf("\tProcess Id = %d, Initial Burst Time = %d, Remaining Burst Time = %d, time created= %d\n",
        ((struct process *) (queueTail -> pData)) -> iProcessId,     
        ((struct process *) (queueTail -> pData)) -> iInitialBurstTime,
        ((struct process *) (queueTail -> pData)) -> iRemainingBurstTime,
            ((struct process *) (queueTail -> pData)) -> oTimeCreated     
        );            

    }    
    queueCur = queue; // reset cur pointer

    printf("END:\n");            

// run job
    for(int j = 0; j < NUMBER_OF_JOBS; j++) {
        
        struct timeval StartTime; // execution time 
        struct timeval EndTime; // execution time 
                    
        runPreemptiveJob((struct process *) (queueCur -> pData) , &StartTime, &EndTime);        
        
        responseTime = getDifferenceInMilliSeconds(((struct process *) (queue -> pData)) -> oTimeCreated, StartTime);   
        turnAroundTime = getDifferenceInMilliSeconds(((struct process *) (queue -> pData)) -> oTimeCreated, EndTime);

        printf("Process Id = %d, Previous Burst Time = %d, Remaining Burst Time = %d, Response Time = %d\n",
            ((struct process *) (queueCur -> pData)) -> iProcessId,     
            ((struct process *) (queueCur -> pData)) -> iPreviousBurstTime,
            ((struct process *) (queueCur -> pData)) -> iRemainingBurstTime,            
            responseTime);

        avgResponseTime += responseTime;
        avgTurnAroundTime += turnAroundTime;             
		queuePrev = queueCur;
        queueCur = queueCur->pNext;
    }

	int numCompletedJobs = 0;
    while(queue != NULL) {

		queuePrev = NULL; // reset prev queue
		queueCur = queue; // reset cur queue
        for(int k = 0; k < (NUMBER_OF_JOBS-numCompletedJobs); k++) {
        
        struct timeval StartTime; // execution time 
        struct timeval EndTime; // execution time 
                    
        runPreemptiveJob((struct process *) (queueCur -> pData) , &StartTime, &EndTime);        
        

        if (( ( (struct process *) (queueCur -> pData)) -> iRemainingBurstTime) == 0 ) {
			if (queueCur == queue) {
				removeFirst(&queue, &queueTail);
			} else {
				struct element *queueTemp = NULL;

				turnAroundTime = getDifferenceInMilliSeconds(((struct process *) (queue->pData))->oTimeCreated, EndTime);

				printf("Process Id = %d, Previous Burst Time = %d, Remaining Burst Time = %d, Turn around Time = %d\n",
					((struct process *) (queueCur->pData))->iProcessId,
					((struct process *) (queueCur->pData))->iPreviousBurstTime,
					((struct process *) (queueCur->pData))->iRemainingBurstTime,
					turnAroundTime);
				// remove node
				queuePrev->pNext = queueCur->pNext;
				queueTemp = queueCur;
				queueCur = queueCur->pNext;
				free(queueTemp);
				numCompletedJobs++;
			}
		
        } else {
             printf("Process Id = %d, Previous Burst Time = %d, Remaining Burst Time = %d\n",
            ((struct process *) (queueCur -> pData)) -> iProcessId,     
            ((struct process *) (queueCur -> pData)) -> iPreviousBurstTime,
            ((struct process *) (queueCur -> pData)) -> iRemainingBurstTime);

			 queuePrev = queueCur;
			 queueCur = queueCur->pNext;
        }
       
        avgResponseTime += responseTime;
        avgTurnAroundTime += turnAroundTime;    
		
        }
		printf("%d\n",numCompletedJobs);
    }
    

    // printf("Average response time = %.6f\n", (float) avgResponseTime / NUMBER_OF_JOBS);
    // printf("Average turn around time = %.6f\n", (float) avgTurnAroundTime / NUMBER_OF_JOBS);    
}