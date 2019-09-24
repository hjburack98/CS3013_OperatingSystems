#ifndef MAILBOX_H_
#define MAILBOX_H_

#include <semaphore.h>

#define REQUEST 1
#define REPLY 2

struct msg {
	int iSender; /* sender of the message (0 .. number-of-threads) */
	int type;    /* its type */
	int value1;  /* first value */
	int value2;  /* second value */
};

// global variables
struct msg **allMailboxes;
pthread_t **alLThreads;        
sem_t **semArray1;
sem_t **semArray2;
int inputThreads;

int SendMsg(int iTo, struct msg *pMsg);
int RecvMsg(int iFrom, struct msg *pMsg);

#endif