#include "mailbox.h"
#include <stdio.h>

int SendMsg(int iTo, struct msg *pMsg) {
	if (iTo < 0 || iTo > inputThreads + 1) {
		printf("called SendMsg with invalid arguments\n");
		return 1;
	}
	
	sem_wait(semSend[iTo]); // wait until the mailbox is empty

	allMailboxes[iTo] = pMsg;
	printf("%d %d %d %d", pMsg->iFrom, pMsg->value, pMsg->cnt, pMsg->tot);

	sem_post(semRecieve[iTo]); // you have mail!

	return 0;
}

int RecvMsg(int iFrom, struct msg *pMsg) {
	sem_wait(semRecieve[iFrom]); // wait for mail to arrive

	if (iFrom < 0 || iFrom > inputThreads + 1) {
		printf("called recvMsg with invalid arguments\n");
		return 1;
	}
	
	*pMsg = *allMailboxes[iFrom];

	sem_post(semSend[iFrom]); // mark this mailbox empty
    
	return 0;
}