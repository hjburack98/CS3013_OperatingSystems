#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdint.h>
#include "mailbox.h"

#define MAXTHREAD 10

void *addit(void *arg);