#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <zconf.h>
#include "simulator.h"

static const int MAXIMUM_POSSIBLE_LENGTH_OF_INPUT = 256;

void completeTheCommand(char ch, char string[256]);

void printMenu () {
    printf ("You can choose from the following commands:\n");
    printf ("(You have to type in the corresponding character for the command you want to implement.");
    printf ("(Once you choose a character, their will be a prompt to ask you the appropriate parameters for that command\n");
    printf ("\nCommand          Character\n");
    printf ("\nCreate           C");
    printf ("\nFork             F");
    printf ("\nKill             K");
    printf ("\nExit             E");
    printf ("\nQuantum          Q");
    printf ("\nSend             S");
    printf ("\nReceive          R");
    printf ("\nReply            Y");
    printf ("\nNew Semaphore    N");
    printf ("\nSemaphoreP       P");
    printf ("\nSemaphoreV       V");
    printf ("\nProcessInfo      I");
    printf ("\nTotalInfo        T\n\n");
}

void readInput (char inputFromUser[256]) {
    fflush (stdout);
    read (STDIN_FILENO, inputFromUser, MAXIMUM_POSSIBLE_LENGTH_OF_INPUT);
    fflush (stdin);
}

void completeTheCommand(char charForCommand, char inputFromUser[256]) {

    if (charForCommand == 'C') {
        printf("Please enter the priority for new proess (0-high, 1-norm, 2-low) : ");
        readInput(inputFromUser);
        inputFromUser[1] = '\0';
        int givenValue = inputFromUser[0] - '0';
        int priority = (givenValue == 0 || givenValue == 1 || givenValue == 2) ? givenValue : -1;

        if (priority == -1) {
            printf("Invalid priority level.\n");
        } else {
            createProcess(priority);
        }
    }

    else if (charForCommand == 'F') {
        forkProcess();
    }

    else if (charForCommand == 'K') {
        int processID;
        printf("Please enter the provess ID : ");
        scanf ("%d", &processID);
        killProcess (processID);
    }

    else if (charForCommand == 'E') {
        exitProcess();
    }

    else if (charForCommand == 'Q') {
        quantum();
    }

    else if (charForCommand == 'S') {
        printf ("Please enter the provess ID of process to send message to : ");
        scanf ("%d", &processID);
        char msg [40];
        printf ("Please enter the message to send : ");
        scanf ("%s", msg);
        send (processID, msg);
    }

    else if (charForCommand == 'R') {
        recieve ();
    }

    else if (charForCommand == 'Y') {
        printf ("Please enter the provess ID : ");
        scanf ("%d", &processID);
        char msg [40];
        printf ("Please enter the message : ");
        scanf ("%s", msg);
        reply (processID, msg);
    }

    else if (charForCommand == 'N') {
        printf("Please enter the semaphore ID for new semaphore (0-4) : ");
        readInput(inputFromUser);
        inputFromUser[1] = '\0';
        int semID, semaphoreValue;

        if (isdigit(inputFromUser[0])) {
            semID = inputFromUser[0] - '0';
            if (semID >= 0 && semID <= 4) {
                printf("Please enter the semaphore value for new semaphore (0-4) : ");
                readInput(inputFromUser);
                inputFromUser[1] = '\0';
                semaphoreValue;

                if (isdigit(inputFromUser[0]) && ((inputFromUser[0] - '0') >= 0)) {
                    semaphoreValue = inputFromUser[0] - '0';
                    newSemaphore(semID, semaphoreValue);
                } else {
                    printf("Invalid semaphore value (<0).");
                }
            } else {
                printf("Invalid ID number (<0 || >4).");
            }
        } else {
            printf("Invalid ID number.");
        }
    }

    else if (charForCommand == 'P') {
        printf("Please enter the semaphore ID for new semaphore (0-4) : ");
        readInput(inputFromUser);
        inputFromUser[1] = '\0';
        int semID;

        if (isdigit(inputFromUser[0])) {
            semID = inputFromUser[0] - '0';
            if (semID >= 0 && semID <= 4) {
                semaphoreP(semID);
            } else {
                printf("Invalid ID number (<0 || >4).");
            }
        } else {
            printf("Invalid ID number.");
        }
    }

    else if (charForCommand == 'V') {
        printf("Please enter the semaphore ID for new semaphore (0-4) : ");
        readInput(inputFromUser);
        inputFromUser[1] = '\0';
        int semID;

        if (isdigit(inputFromUser[0])) {
            semID = inputFromUser[0] - '0';
            if (semID >= 0 && semID <= 4) {
                semaphoreV(semID);
            } else {
                printf("Invalid ID number (<0 || >4).");
            }
        } else {
            printf("Invalid ID number.");
        }
    }

    else if (charForCommand == 'I') {
        int processID;
        printf("Please enter the provess ID : ");
        scanf ("%d", &processID);
        processInformation (processID);
    }

    else if (charForCommand == 'T') {
        totalInformation();
    }

    else {
        printf ("Invalid input.Please try again when prompted.\n");
    }
}

int main() {
    /*
     * setup the ids for semaphores,
     * setup all the queues used whenever the simulator is started,
     * create the required init process and set it's priority to low.
     */
    char inputFromUser [MAXIMUM_POSSIBLE_LENGTH_OF_INPUT];
    initializeConstants ();
    setSemaphores ();
    initializeQueues ();
    createInitProcess ();

    printMenu();

    while (checkIfInitisAlive()) {
        printf ("Please choose a char from the given menu to complete the corresponding command: ");
        fflush (stdout);
        read (STDIN_FILENO, inputFromUser, MAXIMUM_POSSIBLE_LENGTH_OF_INPUT);
        fflush (stdin);

        inputFromUser [1] = '\0';
        completeTheCommand((char) toupper(inputFromUser[0]), inputFromUser);
    }

    return 0;
}

