#ifndef SERVER_HEADER
#define SERVER_HEADER

#include <pthread.h>
/* some globals and function prototypes */

long totalBadFiles;
long totalDirectories;
long totalRegFiles;
long totalSpecFiles;
long totalRegBytes;
long totalTxtFiles;
long totalTxtBytes;

pthread_mutex_t mutex;

struct fileProcess {
    char *file;
    struct stat *buf;
};

void* process_file(void *processPointer);
void init_globals(void);
void print_results(void);

#endif