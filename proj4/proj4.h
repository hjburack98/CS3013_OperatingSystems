#ifndef SERVER_HEADER
#define SERVER_HEADER

/* some globals and function prototypes */

long totBadFiles;
long totDirs;
long totRegFiles;
long totSpecFiles;
long totRegBytes;
long totTxtFiles;
long totTxtBytes;


pthread_mutex_t mutex;

struct process {
    char *file;
    struct stat *buffer;
};

void* processFile(void *processPointer);
void setup(void);
void printStats(void);

#endif