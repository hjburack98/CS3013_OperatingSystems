#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <ctype.h>
#include <errno.h>
#include <fcntl.h>
#include "proj4.h"

#define MAX_LINE_LENGTH 256
#define MAX_NUM_THREADS 15
#define YEET break

int main(int argc, char** argv) {
    int usingThreads; //determine if we are using threads or not
    int numThreads; //number of threads to make
    int currentThread; //threads currently running
    int oldestThread;
    int resultStats, i;
    char* inputStatus;
    char* inputLine;
    struct stat **buffer;
    pthread_t **threads;

    setup();

    if (argc > 1 && strcmp(argv[1], "thread") == 0) {
        if (argc > 2 && (atoi(argv[2]) <= MAX_NUM_THREADS) && (atoi(argv[2]) >= 1)) {
            numThreads = atoi(argv[2]);
            printf("Running in threaded architecture mode with %d threads\n", numThreads);
            buffer = (struct stat**)malloc(sizeof(struct stat*) * numThreads);
            for (i = 0; i<numThreads; i++) {
                buffer[i] = (struct stat*)malloc(sizeof(struct stat));
            }
        }
        else {
            printf("Threaded mode requires number of threads argument 1 < n < 15\n");
            return 0;
        }
        usingThreads = 1;
        currentThread = 0;
        oldestThread = 0;
        threads = (pthread_t**)malloc(sizeof(pthread_t *)*numThreads);
    }
    else {
        printf("Running in serial architecture mode\n");
        buffer = (struct stat**)malloc(sizeof(struct stat*)); 
        buffer[0] = (struct stat*)malloc(sizeof(struct stat));
        usingThreads = 0; /* serial architecture */
    }


    while(1) {
        inputLine = (char *)malloc(sizeof(char) * MAX_LINE_LENGTH);
        inputStatus = fgets(inputLine, MAX_LINE_LENGTH, stdin); //get name
        if (inputStatus == NULL){
            YEET;
        }
        if(inputLine == NULL){
            YEET;
        }
        if(inputLine[0] == '\0') { //EOF
            YEET;
        }
        char *token = strtok(inputLine, "\n "); //remove new line and trialing spaces
        
        struct file *fileToRun;
        fileToRun = (struct file*)malloc(sizeof(struct file));
        fileToRun->name = inputLine;

        if (!usingThreads) { 
            fileToRun->stats = buffer[0];
            getStats(fileToRun);
        }

        else if (usingThreads) {
            if (currentThread < numThreads) { /* make/dispatch new worker thread */
                threads[currentThread] = (pthread_t *)malloc(sizeof(pthread_t));
                fileToRun->stats = buffer[currentThread];
               // printf("Dispatching thread %d on file %s\n", currThread, toProcess->file);
                pthread_create(threads[currentThread], NULL, getStats, (void *)fileToRun);
                currentThread++;
            }
            else {
                pthread_join(*(threads[oldestThread]), NULL);
                fileToRun->stats = buffer[oldestThread];
                pthread_create(threads[oldestThread], NULL, getStats, (void *)fileToRun);

                if (oldestThread == numThreads -1 ) {
                    oldestThread = 0;
                }
                else {
                    oldestThread++;
                }
                
            }
        }
    }

    if (usingThreads) {
        for (i = 0; i< numThreads; i++) {
            pthread_join(*(threads[i]), NULL);
        }
    }
    
    printStats();

    return 0;
}

void *getStats(void *file) {
    struct file *fileToRun = (struct fileProcess *)file;
    char *name = fileToRun->name;
    struct stat *stats = fileToRun->stats;

    char current[1];
    int returnStats;
    int openFile;
    int count;
    int txtBytes;

    //get stats
    returnStats = stat(file, stats);

    //is a bad file
    if (returnStats < 0) { 
        pthread_mutex_lock(&mutex);
        totBadFiles++;
        pthread_mutex_unlock(&mutex);
	    return (void*) 0;
    }

    //is a directory
    if (S_ISDIR(stats->st_mode)) {
        pthread_mutex_lock(&mutex);
        totDirs++;
        pthread_mutex_unlock(&mutex);
    }

    //is a regular file
    if (S_ISREG(stats->st_mode)) {
        pthread_mutex_lock(&mutex);
        totRegFiles++;
        totRegBytes = totRegBytes + stats->st_size;
        pthread_mutex_unlock(&mutex);
    }

    //is a special file
    else if (!(S_ISDIR(stats->st_mode)) && !(S_ISREG(stats->st_mode))) {
        pthread_mutex_lock(&mutex);
        totSpecFiles++;
        pthread_mutex_unlock(&mutex);
    }

    //couldn't open file
    if ((openFile = open(file, O_RDONLY)) < 0) { 
	    return (void*) 0;
    }

    //if txt file
    txtBytes = 0;
    while ((count = read(openFile, current, 1)) > 0) {
        if (isprint(current[0]) || isspace(current[0])) {
            txtBytes++;
        }
        else {
            txtBytes = 0;
            YEET;
        }
    }
    if (txtBytes > 0) {
        pthread_mutex_lock(&mutex);
        totTxtBytes = totTxtBytes + txtBytes;
        totTxtFiles++;
        pthread_mutex_unlock(&mutex);
    }

    return (void*) 0;
}

void setup() {
    totBadFiles = 0;
    totDirs = 0;
    totRegFiles = 0;
    totSpecFiles = 0;
    totRegBytes = 0;
    totTxtFiles = 0;
    totTxtBytes = 0;
    if (pthread_mutex_init(&mutex, NULL) < 0) {
        perror("failed init mutex");
        exit(1);
    }
}

void printStats(){ 
    printf("Bad Files: %ld\n", totBadFiles);
    printf("Directories: %ld\n", totDirs);
    printf("Regular Files: %ld\n", totRegFiles);
    printf("Special Files: %ld\n", totSpecFiles);
    printf("Bytes for Regular Files: %ld\n", totRegBytes);
    printf("Text Files: %ld\n", totTxtFiles);
    printf("Bytes for Text File Bytes: %ld\n", totTxtBytes);
}