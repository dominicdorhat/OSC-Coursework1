// FCFS

#include <stdio.h>
#include <stdlib.h>
#include "coursework.h"
#include "linkedlist.h"

int main() {
    
    // linked list
    struct element *queue = NULL;
    struct element *queueCur = NULL;
    struct element *queueTail = NULL;

    int avgResponseTime, responseTime, avgTurnAroundTime, turnAroundTime;

// create job
    printf("PROCESS LIST:\n");
    for (int i = 0; i < NUMBER_OF_JOBS; i++) {
        struct process *oTemp = generateProcess(); // create first process node         
        addLast(oTemp, &queue, &queueTail);
        
        
        printf("\tProcess Id = %d, Initial Burst Time = %d, Remaining Burst Time = %d\n",
        ((struct process *) (queueTail -> pData)) -> iProcessId,     
        ((struct process *) (queueTail -> pData)) -> iInitialBurstTime,
        ((struct process *) (queueTail -> pData)) -> iRemainingBurstTime);            

    }    
    queueCur = queue; 
    printf("END:\n");            

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
        queueCur = queueCur->pNext;
    }

    // printf("Average response time = %.6f\n", (float) avgResponseTime / NUMBER_OF_JOBS);
    // printf("Average turn around time = %.6f\n", (float) avgTurnAroundTime / NUMBER_OF_JOBS);    
}