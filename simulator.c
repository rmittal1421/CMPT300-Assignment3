//
// Created by Raghav Mittal on 2018-03-18.
//

#include <stdlib.h>
#include <stdio.h>
#include "simulator.h"

int giveAndIncreementID() {
    return processID++;
}

char* namePriority (int priority) {
    return (priority == 0) ? "HIGH" : (priority == 1) ? "NORM" : "LOW";
}

//Global
void initializeConstants () {
    processID = 0;
    high = 0;
    norm = 1;
    low = 2;
    running = 0;
    waiting = 1;
    blocked = -1;
}

void setSemaphores () {
    for (int i = 0; i < 10; ++i) {
        semaphore [i] = 0;
    }
}

void initializeQueues() {
    readyQueue0 = ListCreate();
    readyQueue1 = ListCreate();
    readyQueue2 = ListCreate();

    blockedQueue = ListCreate();
    messageQueue = ListCreate();
}

void createInitProcess () {
    initProcess = malloc (sizeof(*initProcess));
    initProcess->idNumber = giveAndIncreementID ();
    initProcess->priority = low;
    initProcess->stateOfProcess = running;
    initProcess->message = NULL;
    runningProcess = initProcess;
}

void createProcess (int priority) {
    PCB* generateProcess = malloc(sizeof(*generateProcess));

    if (generateProcess == NULL) {
        printf ("Failure in creating a new process");
        return;
    }

    generateProcess->idNumber = giveAndIncreementID();
    generateProcess->priority = priority;
    generateProcess->stateOfProcess = waiting;
    generateProcess->message = NULL;

    if (runningProcess == initProcess) {
        initProcess->stateOfProcess = waiting;
        runningProcess = generateProcess;
        runningProcess->stateOfProcess = running;
    }
    else {
        ListPrepend((priority == 0) ? readyQueue0 : (priority == 1) ? readyQueue1 : readyQueue2, generateProcess);
    }

    printf ("CreateProcess result : Successfully created a new process with process id#: %d and "
                    "placed on the ready queue for processes with %s priority.\n",
                         generateProcess->idNumber, namePriority(priority));

//    printf ("Count of queue 0 is %d.\n", ListCount(readyQueue0));
//    printf ("Count of queue 1 is %d.\n", ListCount(readyQueue1));
//    printf ("Count of queue 2 is %d.\n", ListCount(readyQueue2));

    return;
}

void quantum () {
    if (runningProcess == initProcess) {
        return;
    }

    if (runningProcess->stateOfProcess == runningProcess) {
        int priority = runningProcess->priority;

        ListPrepend((priority == 0) ? readyQueue0 : (priority == 1) ? readyQueue1 : readyQueue2, runningProcess);
        runningProcess->stateOfProcess = waiting;
    }

    /*
     * Now we need to start the process which is waiting in the ready queue to get it's turn.
     * The processes with higher priority get to go first and so on.
     * If no process is left in the game, then initProcess becomes running process.
     */

    PCB* toBeRunningProcess = (ListCount(readyQueue0) ? ListTrim(readyQueue0) :
                               (ListCount(readyQueue1)) ? ListTrim(readyQueue1) :
                                (ListCount(readyQueue2)) ? ListTrim(readyQueue2) : initProcess);

    toBeRunningProcess->stateOfProcess = running;
    runningProcess = toBeRunningProcess;

    printf ("After quantum, the running process's PID is: %d and"
                    " is chosen from the readyQueue: %d.\n", runningProcess->idNumber, runningProcess->priority);

    return;
}

void fork () {
    if (runningProcess == initProcess) {
        printf ("Attempting to fork init process. Hence, failed");
        return;
    }
    else {
        //To copy a process, we have to create a new one with the same priority and my create is designed in such a
        //manner that the process is automatically placed on the desired queue.
        printf ("Fork result : ");
        createProcess (runningProcess->priority);

        //The reports will be given by createProcess function so we do not need to include any print statement here.
        return;
    }
}

int comparator(PCB *tempProcess, int *comparisonArg) {
    return (tempProcess->idNumber == comparisonArg);
}

void killProcess(int processID) {
    printf ("Kill result : ");
    if (processID == initProcess->idNumber) {
        printf ("Trying to killProcess init process. Failed");
        return;
    }

    //Cases : Process to be killed can be at three places : Any ready queue, running process, init process.
    if (runningProcess->idNumber == processID) {
        if (runningProcess == initProcess) {
            printf ("Failure, Trying to killProcess init process even when there are other processes in the system left.\n");
            return;
        }
        else {
            runningProcess->stateOfProcess = waiting;
            free (runningProcess);
            quantum();
        }
    }
    else {
        //Search in the list finds the item if it is there and returns a pointer to that item.
        //If it is not there, then NULL is returned.
        //otherwise, current is set to the found item and returned.
        //hence we can remove it right away.

        PCB* tempProcess;

        if ((ListSearch(readyQueue0, comparator, processID)) == NULL) {
            tempProcess = ListRemove (readyQueue0);
        }
        else if ((tempProcess = ListSearch(readyQueue1, comparator, processID)) != NULL) {
            tempProcess = ListRemove (readyQueue1);
        }
        else if ((tempProcess = ListSearch(readyQueue2, comparator, processID)) != NULL) {
            tempProcess = ListRemove (readyQueue2);
        }

        free (tempProcess);
    }

    printf ("Successfully killed the desired process with process id# : %d.\n", processID);
    return;
}

void exitProcess () {
    printf ("Exit result : ");
    killProcess (runningProcess->idNumber);
}

/*
 * A message is sent to the process whose id is given by the running process and the running process is blocked
 * And it waits for the reply back.
 * Until then, if the process for which the message has been sent is not running, the message is stored in a Queue.
 */
void send (int processId, char* msg) {
    MSG* msgToSend = malloc(sizeof(*msgToSend));
    msgToSend->idNumber = processId;
    msgToSend->source = runningProcess->idNumber;
    msgToSend->state = running;
    msgToSend->message = msg;
    ListPrepend (messageQueue, msgToSend);

    if (runningProcess == initProcess) {
        printf ("Send result : Message has been saved in a message Queue and will be sent when there is a process"
                        " since there are no processes as of now.");
    }
    else {
        ListPrepend (blockedQueue, runningProcess);
        runningProcess->stateOfProcess = blocked;
        printf ("Send result : Successfully saved a message in messageQueue to send out to reciever with "
                        "id : %d from the sender with id %d.\n", processId, runningProcess->idNumber);
        printf ("The sender with processID : %d has been blocked.\n", runningProcess->idNumber);
        quantum();
    }
    return;
}

void recieve () {

}




