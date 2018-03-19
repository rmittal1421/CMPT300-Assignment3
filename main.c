#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "simulator.h"

//void acceptTheInput(int accepted);

/*
 * https://stackoverflow.com/questions/13542055/how-to-do-scanf-for-single-char-in-c
 * The code to take the input has been copied from the link given above.
 */
//void acceptTheInput(int accepted) {
//    scanf ()
//}

int main() {
    /*
     * setup the ids for semaphores,
     * setup all the queues used whenever the simulator is started,
     * create the required init process and set it's priority to low.
     */
    initializeConstants ();
    setSemaphores ();
    initializeQueues ();
    createInitProcess ();

    while (1) {
        //char* ch = getchar();
        //ch = toupper (*ch);
        if (1) {
            createProcess (0);
            createProcess(1);
            createProcess(2);
            fork ();
            //killProcess (1);
            //exitProcess();
//            killProcess(2);
//            killProcess(3);
            send (2, "Hey there");
            printf ("runningProcess id is : %d\n", runningProcess->idNumber);
        }
        break;
    }

    return 0;
}

