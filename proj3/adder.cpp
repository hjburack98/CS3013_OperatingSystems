#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <pthread.h>
#include <stdint.h>
#include <time.h>
#include "mailbox.h"

#define MAXTHREAD 10

void *adder(int thread, int val);

int main(int argc, char *argv[]) {
    int threadIndex, valToAdd;

    //Check to see if arguments are good to go
    if(argc != 2){
        printf("Input line should be './adder numOfThreads");
        exit(1);
    }
    else if(atoi(argv[1]) > 10){
        printf("Input cannot have greater than 10 threads. Defaulting to 10 threads\n");
        inputThreads = 10;
    }

    else{
        inputThreads = atoi(argv[1]);
    }
    
    //Read more from input lines
 

    //allocate memory

}

/*//More to change!!!*/
//Specifically Time
void *adder(int thread, int addedVal) {
    int ops, startTime, endTime;
	struct msg *message;

    //start timer
    //startTime = time(3);
	
	message = (struct msg *)malloc(sizeof(struct msg));

	RecvMsg(thread, message); // wait for mail from parent

    message->iFrom = thread;
    message->value = message->value + addedVal;
    message->cnt = message->cnt + 1;
    //TIME


    SendMsg(0, message);

    return (void *)0;


    
}