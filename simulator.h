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
    int idNumber;
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
int semaphore [10];
LIST *readyQueue0, *readyQueue1, *readyQueue2;
PCB *initProcess;
long processID;
unsigned int high, norm, low;
unsigned int running, waiting, blocked;
PCB *runningProcess;
LIST *blockedQueue, *messageQueue;

void initializeConstants ();
void setSemaphores ();
void initializeQueues ();
void createInitProcess ();

void createProcess (int priority);
void quantum ();
void fork ();
void killProcess(int processID);
int comparator (PCB* thisProcess, int* comparisonArg);
void exitProcess ();
void send (int processId, char* msg);

#endif //ASS3_SIMULATOR_H
