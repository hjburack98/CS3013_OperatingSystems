#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdint.h>
#include <time.h>
#include <string.h>
#include "mailbox.h"

#define MAXTHREAD 10

void *adder(void *arg);

int main(int argc, char *argv[]) {
    int threadIndex, valToAdd, i;
    struct msg *received;
	struct msg **mailToSend;

    //Check to see if arguments are good to go
    if(argc != 2){
        printf("Input line should be './adder numOfThreads");
        exit(1);
    }
    else if(atoi(argv[1]) > 10){
        printf("Input cannot have greater than 10 threads. Defaulting to 10 threads\n");
        inputThreads = 10;
    }
    else if(atoi(argv[1]) < 1) {
        printf("number of threads cannot be less than 1");
        exit(1);
    }

    //set number of input threads
    inputThreads = atoi(argv[1]);
    
    //allocate memory
    allMailboxes = (struct msg **)malloc((inputThreads + 1) * sizeof(struct msg *));
	mailToSend = (struct msg **)malloc(inputThreads * sizeof(struct msg *));
    semArray1 = (sem_t **)malloc((inputThreads + 1) * sizeof(sem_t *));
	semArray2 = (sem_t **)malloc((inputThreads + 1) * sizeof(sem_t *));
    allThreads = (pthread_t **)malloc(inputThreads * sizeof(pthread_t *));
	received = (struct msg *)malloc(sizeof(struct msg));

    //make mailboxes
    for (i = 0; i <= inputThreads; i++) {
		// allocate more memory
		semArray1[i] = (sem_t *)malloc(sizeof(sem_t));
		semArray2[i] = (sem_t *)malloc(sizeof(sem_t));
		sem_init(semArray1[i], 0, 1); //psem
		sem_init(semArray2[i], 0, 0); //csem
	}

    //read from input lines
    while(1){
        char inputStr[100];
        int sscanfResult;
        fgets(inputStr, 100, stdin);
        sscanfResult = sscanf(inputStr, "%d %d", &valToAdd, &threadIndex);
        if(sscanfResult != 2){
            break;
        }
        if(threadIndex > inputThreads){
            break;
        }
    }

    // //make threads
    // for (i = 0; i < inputThreads; i++) {
	// 	allThreads[i] = (pthread_t *)malloc(sizeof(pthread_t *));
	// 	if (pthread_create(allThreads[i], NULL, &adder, (void *)(intptr_t)(i + 1))) {
	// 		printf("error creating thread %d\n", i + 1);
	// 	}
	// }

    //read from input lines
    

}

// /*//More to change!!!*/
// //Specifically Time
// void *adder(void *arg) {
//     int index, ops, startTime, endTime;
// 	struct msg *message;

//     //start timer
//     //startTime = time(3);

//     index = (int)(intptr_t)arg;
// 	message = (struct msg *)malloc(sizeof(struct msg));

// 	RecvMsg(index, message); // wait for mail from parent

//     message->iFrom = index;
//     message->value = message->value + addedVal;
//     message->cnt = message->cnt + 1;
//     //TIME


//     SendMsg(0, message);

//     return (void *)0;


    
// }