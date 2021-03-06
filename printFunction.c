/*
 * Prints output on the screen depending on the type of process (FCFS or RR) and calculates the turn-around/response times.
 * The variables dAverageResponseTime and dAverageTurnAroundTime should be declared.
 * If the job has not finished completely, it is returned by the function, NULL otherwise.
 * To be included in your task4.c and task5.c
 *
 */

// TODO: decide if following libraries are needed or not
// TODO: remove unncessary comments when done

#include "coursework.h" 
#include <stdio.h>

// IMPORTANT NOTE: you are expected to consider the synchronisation in this function (that is, you are allowed to modify this function)
struct process * processJob(int iConsumerId, struct process * pProcess, struct timeval oStartTime, struct timeval oEndTime)
{
	int iResponseTime;
	int iTurnAroundTime;
	
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