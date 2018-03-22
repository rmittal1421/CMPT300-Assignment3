//
// Created by Raghav Mittal on 2018-03-3.
//

#ifndef ASS3_SIMULATOR_H
#define ASS3_SIMULATOR_H

#include "list.h"

//using System.Threading

struct msg {
    char* message;
    int state;
    int processIDForMSG;
    int source;
};
typedef struct msg MSG;

struct process {
    int priority;
    int idNumber;
    int stateOfProcess;
    MSG* message;
};

struct semaphore {
    int semaphoreID;
    int value;
    LIST* processesList;
};

// Define the names of the structs used for easy use.

typedef struct process PCB;
typedef struct semaphore SEM;

// Create all the queues required.
/*
 * We need 3 ready queues according to the priority of the process.
 * namely 0 == high, 1 == norm, 2 == low.
 */

//since semaphores can only take id from 0 to 4, I only need an array of size 5.
int semaphoreIDS [5];
LIST *readyQueue0, *readyQueue1, *readyQueue2;
PCB *initProcess;
int processID;
unsigned int high, norm, low;
unsigned int running, ready;
int blocked;
PCB *runningProcess;
LIST *blockedQueue, *messageQueue, *toBeSentQueue, *toRecieveQueue, *semaphoreQueue;

void initializeConstants ();
void setSemaphores ();
void initializeQueues ();
void createInitProcess ();

void createProcess (int priority);
void quantum ();
void forkProcess ();
void killProcess(int processID);
int comparator (PCB* thisProcess, int* comparisonArg);
void exitProcess ();
void send (int processId, char* msg);
void recieve ();
void reply (int processId, char* msg);
void newSemaphore (int semaphoreID, int value);
void semaphoreP (int semaphoreID);
void semaphoreV (int semaphoreID);
void processInformation (int processID);
void totalInformation ();
int checkIfInitisAlive ();

#endif //ASS3_SIMULATOR_H
