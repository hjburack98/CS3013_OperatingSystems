#ifndef SERVER_HEADER
#define SERVER_HEADER

#include <pthread.h>
/* some globals and function prototypes */

int totBadFiles;
int totDirs;
int totRegFiles;
int totSpecFiles;
int totRegBytes;
int totTxtFiles;
int totTxtBytes;

pthread_mutex_t mutex;

struct file {
    char *name;
    struct stat *stats;
};

void setup();
void printStats();
void* getStats(void *file);

#endif