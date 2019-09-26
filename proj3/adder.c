#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdint.h>
#include <time.h>
#include <string.h>
#include "mailbox.h"

#define MAXTHREAD 10
#define YEET break
void *adder(void *arg);

int main(int argc, char *argv[]) {
    int threadIndex, valToAdd, i;

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
    semSend = (sem_t **)malloc((inputThreads + 1) * sizeof(sem_t *));
	semRecieve = (sem_t **)malloc((inputThreads + 1) * sizeof(sem_t *));
    allThreads = (pthread_t **)malloc(inputThreads * sizeof(pthread_t *));

    //make mailboxes
    for (i = 0; i <= inputThreads; i++) {
		// allocate more memory
		semSend[i] = (sem_t *)malloc(sizeof(sem_t));
		semRecieve[i] = (sem_t *)malloc(sizeof(sem_t));
		sem_init(semSend[i], 0, 1); //psem
		sem_init(semRecieve[i], 0, 0); //csem
	}

    //make threads
    for(i = 0; i < inputThreads; i++) {
        pthread_t pthread;
        allThreads[i] = pthread;
        if(pthread_create(&pthread, NULL, adder, (void *) (i+1)) != 0){
            perror("pthread_create error");
            exit(1);
        }
    }

    //read from input lines
        char inputStr[100];
        int sscanfResult;
    while(1){
        fgets(inputStr, 100, stdin);
        sscanfResult = sscanf(inputStr, "%d %d", &valToAdd, &threadIndex);
        if(sscanfResult != 2 || threadIndex > inputThreads)
           YEET;
        struct msg *sentMessage;
        sentMessage->iFrom = 0;
        sentMessage->value = valToAdd;
        sentMessage->cnt = 0;
        sentMessage->tot = 0;
        SendMsg(threadIndex, sentMessage);    
    }


    for(i = 0; i < inputThreads; i++){
        printf("The result from thread %d is %d from %d operations during %d secs.", 
        allMailboxes[i]->iFrom, allMailboxes[i]->value, allMailboxes[i]->cnt, allMailboxes[i]->tot);
    }

    for(i = 0; i < inputThreads; i++){
        pthread_join(allThreads[i], NULL);

        if(sem_destroy(semSend[i]) < 0){
            perror("sem_destroy error");
            exit(1);
        }

        if(sem_destroy(semRecieve[i]) < 0){
            perror("sem_destroy error");
            exit(1);
        }
    }

}

/*//More to change!!!*/
//Specifically Time
void *adder(void *arg) {
    int index = (intptr_t) arg;
    int addedVal = 0;
    int count = 0;
	struct msg *recievedMessage;
    struct msg *sentMessage;
    int running = 1; //
    int startTimer = time(NULL);


    while(running != 0){
        RecvMsg(index, &recievedMessage);

        count++;
        addedVal += recievedMessage->value;
    }

    int endTimer = time(NULL);
    int totalTime = endTimer - startTimer;

    sentMessage->iFrom = index;
    sentMessage->value = addedVal;
    sentMessage->cnt = count;
    sentMessage->tot = totalTime;

    SendMsg(0, sentMessage);

    return (void *)0;
    
}