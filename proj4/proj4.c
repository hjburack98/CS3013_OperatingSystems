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

#define MAX_LINE_LENGTH 128
#define MAX_NUM_THREADS 15

int main(int argc, char** argv) {
    int threadMode; /* whether or not we are in threaded or serial mode */
    int numThreads; /* user-specified number of threads */
    int currThread; /* how many threads are currently running */
    int oldestThread; /* oldest running thread */
    int statRet, i;
    char* inputStatus;
    char* inputLine;
    struct stat **buf;
    pthread_t **threads;
    struct timeval start, end; /* keep track of time */
    struct rusage usage;

    gettimeofday(&start, 0);
    getrusage(RUSAGE_SELF, &usage);

    init_globals();

    if (argc > 1 && strcmp(argv[1], "thread") == 0) {
        if (argc > 2 && (atoi(argv[2]) <= MAX_NUM_THREADS) && (atoi(argv[2]) >= 1)) {
            numThreads = atoi(argv[2]);
            printf("Running in threaded architecture mode with %d threads\n", numThreads);
            buf = (struct stat**)malloc(sizeof(struct stat*) * numThreads);
            for (i = 0; i<numThreads; i++) {
                buf[i] = (struct stat*)malloc(sizeof(struct stat));
            }
        }
        else {
            printf("Threaded mode requires number of threads argument 1 < n < 15\n");
            return 0;
        }
        threadMode = 1; /* threaded */
        currThread = 0;
        oldestThread = 0;
        threads = (pthread_t**)malloc(sizeof(pthread_t *)*numThreads); /* allocating thread pointer array, will allocate/dispatch threads later */
    }
    else {
        printf("Running in serial architecture mode\n");
        buf = (struct stat**)malloc(sizeof(struct stat*)); 
        buf[0] = (struct stat*)malloc(sizeof(struct stat));
        threadMode = 0; /* serial architecture */
    }


    while(1) {
        inputLine = (char *)malloc(sizeof(char) * MAX_LINE_LENGTH);
        inputStatus = fgets(inputLine, MAX_LINE_LENGTH, stdin); /* grab filename from stdin */
        if (inputStatus == NULL || inputLine == NULL || inputLine[0] == '\0') { /* reached EOF/error */
            break;
        }
        char *token = strtok(inputLine, "\n "); /* get rid of trailing spaces/newline */
        
        struct fileProcess *toProcess;
        toProcess = (struct fileProcess*)malloc(sizeof(struct fileProcess));
        toProcess->file = inputLine;

        if (!threadMode) { /* serial architecture */
            toProcess->buf = buf[0];
            process_file(toProcess);
        }

        else if (threadMode) {
            if (currThread < numThreads) { /* make/dispatch new worker thread */
                threads[currThread] = (pthread_t *)malloc(sizeof(pthread_t));
                toProcess->buf = buf[currThread];
               // printf("Dispatching thread %d on file %s\n", currThread, toProcess->file);
                pthread_create(threads[currThread], NULL, process_file, (void *)toProcess);
                currThread++;
            }
            else { /* join on the oldest thread then re-make/dispatch */
               // printf("Joining threads on oldest thread, index %d\n", oldestThread);
                pthread_join(*(threads[oldestThread]), NULL);
                toProcess->buf = buf[oldestThread];
                //printf("Dispatching thread %d on file %s\n", oldestThread, toProcess->file);
                pthread_create(threads[oldestThread], NULL, process_file, (void *)toProcess);

                /* update oldest thread */
                if (oldestThread == numThreads -1 ) {
                    oldestThread = 0; /* loop back around */
                }
                else {
                    oldestThread++;
                }
                
            }
        }
    }
    /* wait for all threads to finish jobs*/
    if (threadMode) {
        for (i = 0; i< numThreads; i++) {
            pthread_join(*(threads[i]), NULL);
        }
    }
    /* display file results */
    print_results();

    /* get and print time info */
    getrusage(RUSAGE_SELF, &usage);
    gettimeofday(&end, 0);
    double clock_elapsed = ((end.tv_sec - start.tv_sec)*1000) + ((double)(end.tv_usec - start.tv_usec))/1000;
    double user_elapsed = ((usage.ru_utime.tv_sec)*1000) + ((double)(usage.ru_utime.tv_usec))/1000;
    double system_elapsed = ((usage.ru_stime.tv_sec)*1000) + ((double)(usage.ru_stime.tv_usec))/1000;
    printf("System Time (ms): %f\nUser Time (ms): %f\nWall-Clock Time (ms): %f\n", system_elapsed, user_elapsed, clock_elapsed);

    return 0;
}

void *process_file(void *processPointer) {
    struct fileProcess *toProcess = (struct fileProcess *)processPointer;
    char *file = toProcess->file;
    struct stat *buf = toProcess->buf;
    //printf("Processing file %s\n", file);
    char currChar[1];
    int statRet;
    int fdIn, cnt;
    int byteCount;
    statRet = stat(file, buf);

    if (statRet < 0) { /* bad file */
        pthread_mutex_lock(&mutex);
        totalBadFiles++;
        pthread_mutex_unlock(&mutex);
        //pthread_exit(NULL);
	return (void*)0;
    }
    if (S_ISDIR(buf->st_mode)) {
        pthread_mutex_lock(&mutex);
        totalDirectories++;
        pthread_mutex_unlock(&mutex);
    }
    if (S_ISREG(buf->st_mode)) {
        pthread_mutex_lock(&mutex);
        totalRegFiles++;
        totalRegBytes = totalRegBytes + buf->st_size;
        pthread_mutex_unlock(&mutex);
    }
    else if (!(S_ISDIR(buf->st_mode)) && !(S_ISREG(buf->st_mode))) {
        pthread_mutex_lock(&mutex);
        totalSpecFiles++;
        pthread_mutex_unlock(&mutex);
    }
    if ((fdIn = open(file, O_RDONLY)) < 0) { /* couldn't open, not a text file */
        
       // pthread_exit(NULL);
	return (void*)0;
    }
    
    byteCount = 0;
    while ((cnt = read(fdIn, currChar, 1)) > 0) {
        if (isprint(currChar[0]) || isspace(currChar[0])) {
            byteCount++;
        }
        else { /* file wasn't entirely readable, return 0 */
            byteCount = 0;
            break;
        }
    }
    if (byteCount > 0) {
        pthread_mutex_lock(&mutex);
        totalTxtBytes = totalTxtBytes + byteCount;
        totalTxtFiles++;
        pthread_mutex_unlock(&mutex);
    }
    //pthread_exit(NULL);
    return (void*)0;
}

void init_globals() {
    totalBadFiles = 0;
    totalDirectories = 0;
    totalRegFiles = 0;
    totalSpecFiles = 0;
    totalRegBytes = 0;
    totalTxtFiles = 0;
    totalTxtBytes = 0;
    if (pthread_mutex_init(&mutex, NULL) < 0) {
        perror("failed init mutex");
        exit(1);
    }
}

void print_results(){ 
    printf("Bad Files: %ld\n", totalBadFiles);
    printf("Directories: %ld\n", totalDirectories);
    printf("Regular Files: %ld\n", totalRegFiles);
    printf("Special Files: %ld\n", totalSpecFiles);
    printf("Regular File Bytes: %ld\n", totalRegBytes);
    printf("Text Files: %ld\n", totalTxtFiles);
    printf("Text File Bytes: %ld\n", totalTxtBytes);
}