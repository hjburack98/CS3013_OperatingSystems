#ifndef MAILBOX_H_
#define MAILBOX_H_

#include <semaphore.h>

#define REQUEST 1
#define REPLY 2

struct msg {
	int iFrom; // who sent the message
	int value;    // its value
	int cnt;  // count of operaions 
	int tot;  // total time
};

// global variables
struct msg **allMailboxes;
pthread_t *allThreads;        
sem_t **semSend;
sem_t **semRecieve;
int inputThreads;

int SendMsg(int iTo, struct msg *pMsg);
int RecvMsg(int iFrom, struct msg *pMsg);

#endif