//
// Created by Raghav Mittal on 2018-03-18.
//

#include <stdlib.h>
#include <stdio.h>
#include "simulator.h"

void printQueue(LIST *queue);

int giveAndIncrementID() {
    return processID++;
}

char *namePriority(int priority) {
    return (priority == 0) ? "HIGH" : (priority == 1) ? "NORM" : "LOW";
}

//Global
void initializeConstants() {
    processID = 0;
    high = 0;
    norm = 1;
    low = 2;
    running = 0;
    ready = 1;
    blocked = -1;
}

void setSemaphores() {
    for (int i = 0; i < 10; ++i) {
        semaphoreIDS[i] = 0;
    }
}

void initializeQueues() {
    readyQueue0 = ListCreate();
    readyQueue1 = ListCreate();
    readyQueue2 = ListCreate();

    blockedQueue = ListCreate();
    messageQueue = ListCreate();
    toBeSentQueue = ListCreate();
    toRecieveQueue = ListCreate();
    semaphoreQueue = ListCreate();
}

int checkIfInitisAlive() {
    return (initProcess->stateOfProcess == blocked) ? 0 : 1;
}

void createInitProcess() {
    initProcess = malloc(sizeof(*initProcess));
    initProcess->idNumber = giveAndIncrementID();
    initProcess->priority = low;
    initProcess->stateOfProcess = running;
    initProcess->message = NULL;
    runningProcess = initProcess;
}

void createProcess(int priority) {
    PCB *generateProcess = malloc(sizeof(*generateProcess));

    if (generateProcess == NULL) {
        printf("Failure in creating a new process");
        return;
    }

    generateProcess->idNumber = giveAndIncrementID();
    generateProcess->priority = priority;
    generateProcess->stateOfProcess = ready;
    generateProcess->message = NULL;

    if (runningProcess == initProcess) {
        initProcess->stateOfProcess = ready;
        runningProcess = generateProcess;
        runningProcess->stateOfProcess = running;
    } else {
        ListPrepend((priority == 0) ? readyQueue0 : (priority == 1) ? readyQueue1 : readyQueue2, generateProcess);
    }

    printf("CreateProcess result : Successfully created a new process with process id#: %d and "
                   "placed on the ready queue for processes with %s priority.\n",
           generateProcess->idNumber, namePriority(priority));

//    printf ("Count of queue 0 is %d.\n", ListCount(readyQueue0));
//    printf ("Count of queue 1 is %d.\n", ListCount(readyQueue1));
//    printf ("Count of queue 2 is %d.\n", ListCount(readyQueue2));

    return;
}

void quantum() {
    if (runningProcess == initProcess) {
        return;
    }

    int priority = runningProcess->priority;

    ListPrepend((priority == 0) ? readyQueue0 : (priority == 1) ? readyQueue1 : readyQueue2, runningProcess);
    runningProcess->stateOfProcess = ready;

    /*
     * Now we need to start the process which is waiting in the ready queue to get it's turn.
     * The processes with higher priority get to go first and so on.
     * If no process is left in the game, then initProcess becomes running process.
     */

    PCB *toBeRunningProcess = (ListCount(readyQueue0) ? ListTrim(readyQueue0) :
                               (ListCount(readyQueue1)) ? ListTrim(readyQueue1) :
                               (ListCount(readyQueue2)) ? ListTrim(readyQueue2) : initProcess);

    toBeRunningProcess->stateOfProcess = running;
    runningProcess = toBeRunningProcess;

    printf("After quantum, the running process's PID is: %d and"
                   " is chosen from the readyQueue: %d.\n", runningProcess->idNumber, runningProcess->priority);

    return;
}

void forkProcess() {
    if (runningProcess == initProcess) {
        printf("Attempting to fork init process. Hence, failed");
        return;
    } else {
        //To copy a process, we have to create a new one with the same priority and my create is designed in such a
        //manner that the process is automatically placed on the desired queue.
        printf("ForkProcess result : ");
        createProcess(runningProcess->priority);

        //The reports will be given by createProcess function so we do not need to include any print statement here.
        return;
    }
}

int comparator(PCB *tempProcess, int *comparisonArg) {
    return (tempProcess->idNumber == *comparisonArg);
}

void killProcess(int processID) {
    printf("Kill result : ");

    //Cases : Process to be killed can be at three places : Any ready queue, running process, init process.
    if (runningProcess->idNumber == processID) {
        if (runningProcess == initProcess) {
            free(runningProcess);
            runningProcess->stateOfProcess = blocked;
            printf("Success, Killed init process since it was the only running process"
                           " which means there are no other processes in the system left.\n");
            printf("Turning off system.\n");
            return;
        } else {
            runningProcess->stateOfProcess = ready;
            free(runningProcess);
            printf("Successfully killed the desired process with process id# : %d.\n", processID);
            quantum();
            return;
        }
    } else {
        //Search in the list finds the item if it is there and returns a pointer to that item.
        //If it is not there, then NULL is returned.
        //otherwise, current is set to the found item and returned.
        //hence we can remove it right away.

        PCB *tempProcess;

        if ((ListSearch(readyQueue0, comparator, &processID)) == NULL) {
            tempProcess = ListRemove(readyQueue0);
        } else if ((tempProcess = ListSearch(readyQueue1, comparator, &processID)) != NULL) {
            tempProcess = ListRemove(readyQueue1);
        } else if ((tempProcess = ListSearch(readyQueue2, comparator, &processID)) != NULL) {
            tempProcess = ListRemove(readyQueue2);
        }

        free(tempProcess);
    }

    printf("Successfully killed the desired process with process id# : %d.\n", processID);
    return;
}

void exitProcess() {
    printf("Exit result : ");
    killProcess(runningProcess->idNumber);
}

/*
 * A message is sent to the process whose id is given by the running process and the running process is blocked
 * And it waits for the reply back.
 * Until then, if the process for which the message has been sent is not running, the message is stored in a Queue.
 */
void send(int processId, char *msg) {
    MSG *msgToSend = malloc(sizeof(*msgToSend));

    msgToSend->processIDForMSG = processId;
    msgToSend->source = runningProcess->idNumber;
    msgToSend->state = ready;
    msgToSend->message = msg;
    ListPrepend(messageQueue, msgToSend);

    if (runningProcess == initProcess) {
        printf("Send result : Message has been saved in a message Queue and will be sent when there is a process"
                       " since there are no processes as of now.\n");
    } else {
        ListPrepend(blockedQueue, runningProcess);
        ListPrepend(toBeSentQueue, runningProcess);

        runningProcess->stateOfProcess = blocked;
        printf("Send result : Successfully saved a message in messageQueue to send out to reciever with "
                       "id : %d from the sender with id %d.\n", processId, runningProcess->idNumber);
        printf("The sender with processID : %d has been blocked.\n", runningProcess->idNumber);
        quantum();
    }
    return;
}

int messageComparator(MSG *tempMessage, int *comparisonArg) {
    return (tempMessage->processIDForMSG == *comparisonArg);
}

void recieve () {
    printf("recieve result : ");

    if (runningProcess != initProcess) {
        runningProcess->stateOfProcess = blocked;
        ListPrepend (blockedQueue, runningProcess);
        ListPrepend (toRecieveQueue, runningProcess);

        printf ("Sucess.\n");
        quantum();
    }
    else {
        printf ("Failed.\n");
    }
}

void reply (int processId, char *msg) {
    printf("reply result : ");

    MSG* messageToReply = NULL;
    PCB* processToSendTo;

    if ((runningProcess->message->state == ready) || (runningProcess->message->processIDForMSG != processId)) {
        messageToReply = malloc (sizeof(messageToReply));

        messageToReply->processIDForMSG = processId;
        messageToReply->message = msg;
        messageToReply->state = blocked;
        messageToReply->source = (int) runningProcess-processId;

        ListPrepend (messageQueue, messageToReply);

        if (ListCount (blockedQueue) != 0) {
            processToSendTo = ListFirst (blockedQueue);
            processToSendTo->stateOfProcess = ready;
            ListRemove (blockedQueue);

            int priority = processToSendTo->priority;
            ListPrepend((priority == 0) ? readyQueue0 : (priority == 1) ? readyQueue1 : readyQueue2, processToSendTo);

            if (runningProcess->message->processIDForMSG == processId) {
                free (runningProcess->message);
                runningProcess->message = NULL;
            }
        }

        if (ListCount(toBeSentQueue) != 0) {
            if (ListSearch(toBeSentQueue, messageComparator, &processId)) {
                ListRemove (toBeSentQueue);
                printf ("Success");
            }
        }

        if (ListCount(toRecieveQueue) != 0) {
            if (ListSearch(toRecieveQueue, messageComparator, &processId)) {
                ListRemove (toRecieveQueue);
                printf ("Success");
            }
        }
        else {
            printf ("Failed to unblock sender.\n");
        }
    }
    else {
        printf ("Failed, sender was not ready to recieve a reply.\n");
    }
}

int semaphoreComparator(SEM *tempSemaphore, int *comparisonArg) {
    return (tempSemaphore->semaphoreID == *comparisonArg);
}

void newSemaphore(int semaphoreID, int value) {
    printf("NewSemaphore result : ");
    if (ListSearch(semaphoreQueue, semaphoreComparator, &semaphoreID) != NULL) {
        printf("Failed to create a new semaphore with id: %d since it has been alredy occupied.\n", semaphoreID);
    } else {
        SEM *newSemaphore = malloc(sizeof(newSemaphore));
        newSemaphore->semaphoreID = semaphoreID;
        newSemaphore->value = value;
        newSemaphore->processesList = ListCreate();
        ListPrepend(semaphoreQueue, newSemaphore);
        printf("Successfully created a new semaphore with id : %d.\n", semaphoreID);
    }
    return;
}

void semaphoreP(int semaphoreID) {
    printf("SemaphoreP result : ");
    SEM *currentSemaphore;

    if ((currentSemaphore = ListSearch(semaphoreQueue, semaphoreComparator, &semaphoreID)) == NULL) {
        printf("No semaphore with id : %d was found.\n", semaphoreID);
    } else {
        currentSemaphore->value--;
        printf("Successfully decreased the value of the semaphore with id : %d\n", semaphoreID);

        if (currentSemaphore->value < 0) {
            //It means now we have to block the current running process.
            //otherwise decrementing the value for semaphore was all we had to do.

            runningProcess->stateOfProcess = blocked;
            ListPrepend(blockedQueue, runningProcess);
            ListPrepend(currentSemaphore->processesList, runningProcess);
            printf("And since the value of semaphore for this process has gone below 0, it has been blocked.\n");
            quantum();
        }
    }
}

void semaphoreV(int semaphoreID) {
    printf("SemaphoreV result : ");
    SEM *currentSemaphore;

    if ((currentSemaphore = ListSearch(semaphoreQueue, semaphoreComparator, &semaphoreID)) == NULL) {
        printf("No semaphore with id : %d was found.\n", semaphoreID);
    } else {
        currentSemaphore->value++;
        printf("Successfully increased the value of the semaphore with id : %d\n", semaphoreID);

        if (currentSemaphore->value >= 0) {
            //If the value of semaphore has increased to a non-negative number,
            //It means that we have to unblock a process which has been blocked by this semaphore.
            PCB *toUnblockProcess;

            if (ListFirst(currentSemaphore->processesList) != NULL) {
                toUnblockProcess = ListRemove(currentSemaphore->processesList);
                if (ListSearch(blockedQueue, comparator, &toUnblockProcess->idNumber)) {
                    ListRemove(blockedQueue);
                }
                toUnblockProcess->stateOfProcess = ready;
                int priority = toUnblockProcess->priority;
                ListPrepend((priority == 0) ? readyQueue0 : (priority == 1) ? readyQueue1 : readyQueue2,
                            runningProcess);
                printf("And since the value of semaphore for this process has become non-negative,"
                               " it has been removed from blocked queue and is now waiting on it's ready queue.\n");
            }
        }
    }
}

PCB *findTheProcess(int processID) {
    if (processID == initProcess->idNumber) {
        return initProcess;
    } else if (processID == runningProcess->idNumber) {
        return runningProcess;
    }

    PCB *ready0 = ListSearch(readyQueue0, comparator, &processID);
    PCB *ready1 = ListSearch(readyQueue1, comparator, &processID);
    PCB *ready2 = ListSearch(readyQueue2, comparator, &processID);
    PCB *block = ListSearch(blockedQueue, comparator, &processID);

    return (ready0 != NULL) ? ready0 : (ready1 != NULL) ? ready1 : (ready2 != NULL) ? ready2 : block;
}

void processInformation(int processID) {
    PCB *infoOf = findTheProcess(processID);

    if (infoOf == NULL) {
        printf("Failed! No process with id : %d found.\n", processID);
    } else {
        printf("\nProcess Info: \n");
        printf("ID number : %d, ", infoOf->idNumber);
        printf("Process Status : %s, ", (infoOf->stateOfProcess == 0)
                                        ? "running" : infoOf->stateOfProcess == 1 ? "ready" : "blocked");
        printf("Priority : %s\n", (infoOf->priority == high) ? "HIGH" : infoOf->priority == norm ? "NORM" : "LOW");
    }
}

void printQueue(LIST *queue) {
    PCB *itr = ListFirst(queue);
    while (itr != NULL) {
        processInformation(itr->idNumber);
        itr = ListNext(queue);
    }
}

void totalInformation() {
    /*
     * The order of print will be:
     * 1st- High priority queue
     * 2nd- Norm Priority Queue
     * 3rd- Low Priority Queue
     * 4th- Blocked Queue
     */

    printf("The total number of processes in the system are : %d.\n", processID - 1);

    printf("The running process is :\n");
    processInformation(runningProcess->idNumber);

    printf("\nReady queue with processes with high priority : \n");
    printQueue(readyQueue0);

    printf("\nReady queue with processes with norm priority : \n");
    printQueue(readyQueue1);

    printf("\nReady queue with processes with low priority : \n");
    printQueue(readyQueue2);

    printf("\nReady queue with blocked processes : \n");
    printQueue(blockedQueue);
}




