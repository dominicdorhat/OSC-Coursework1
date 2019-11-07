// authors: Tan Song Ning, Dominic Alphonsus Dorhat

#include <stdio.h>
#include <stdlib.h>
#include "coursework.h"
#include "linkedlist.h"

int main() {
    
    // linked list
    struct element *queue = NULL;
    struct element *queueTail = NULL;

    int avgResponseTime, responseTime, avgTurnAroundTime, turnAroundTime;

    // ready queue
    for (int i = 0; i < NUMBER_OF_JOBS; i++) {
        struct process *newProcess = generateProcess();
        addLast(newProcess, &queue, &queueTail);        
    }    
    
    // run process
    for(int j = 0; j < NUMBER_OF_JOBS; j++) {
        
        struct timeval StartTime;  
        struct timeval EndTime; 
            
        runNonPreemptiveJob((struct process *) (queue -> pData) , &StartTime, &EndTime);        
        
        responseTime = getDifferenceInMilliSeconds(((struct process *) (queue -> pData)) -> oTimeCreated, StartTime);   
        turnAroundTime = getDifferenceInMilliSeconds(((struct process *) (queue -> pData)) -> oTimeCreated, EndTime);

        printf("Process Id = %d, Previous Burst Time = %d, New Burst Time = %d, Response Time = %d, Turn Around Time = %d\n",
            ((struct process *) (queue -> pData)) -> iProcessId,     
            ((struct process *) (queue -> pData)) -> iInitialBurstTime,
            ((struct process *) (queue -> pData)) -> iRemainingBurstTime,
            responseTime,   
            turnAroundTime);

        avgResponseTime += responseTime;
        avgTurnAroundTime += turnAroundTime;    

        free(queue->pData);    
        removeFirst(&queue, &queueTail);

    }

    printf("Average response time = %.6f\n", (float) avgResponseTime / NUMBER_OF_JOBS);
    printf("Average turn around time = %.6f\n", (float) avgTurnAroundTime / NUMBER_OF_JOBS);
    
}