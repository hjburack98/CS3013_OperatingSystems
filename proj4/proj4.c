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
    int usingThreads; 
    int numThreads; 
    int currentThread;
    int oldestThread; 
    int returnStats, i;
    char* inputInfo;
    char* inputLine;
    int numFiles;
    struct stat **stats;
    pthread_t **threads;
    struct timeval start, end;
    struct rusage usage;

    gettimeofday(&start, 0);
    getrusage(RUSAGE_SELF, &usage);


    setup();

    //if "thread" is the second argument
    if (argc > 1 && strcmp(argv[1], "thread") == 0) {

        //error checking 
        if (argc > 2 && (atoi(argv[2]) <= MAX_NUM_THREADS) && (atoi(argv[2]) >= 1)) {
            numThreads = atoi(argv[2]);

            printf("Running using threads. Using %d threads\n", numThreads);

            //allocate the right amount of space for stats
            stats = (struct stat**)malloc(sizeof(struct stat*) * numThreads);

            //allocate each individual stat space
            for (i = 0; i<numThreads; i++) {
                stats[i] = (struct stat*)malloc(sizeof(struct stat));
            }
        }

        //not in range of threads
        else {
            printf("Can only use between 1 and 15 threads\n");
            return 0;
        }

        usingThreads = 1; 
        currentThread = 0;
        oldestThread = 0;


        //making threads
        threads = (pthread_t**)malloc(sizeof(pthread_t *) * numThreads);
    }

    else {
        printf("Running with serial architecture\n");

        //same as with threads, but not looping through multiple positions
        stats = (struct stat**)malloc(sizeof(struct stat*)); 
        stats[0] = (struct stat*)malloc(sizeof(struct stat));

        //not using threads, so set that to 0
        usingThreads = 0;
    }


    while(1) {
        inputLine = (char *)malloc(sizeof(char) * MAX_LINE_LENGTH);

        //get input
        inputInfo = fgets(inputLine, MAX_LINE_LENGTH, stdin);
        if(inputInfo == NULL){
            YEET;
        }
        if(inputLine == NULL){
            YEET;
        }

        if(inputLine[0] == '\n'){
            printf("New Line\n");
            continue;
        }

        if(inputLine[0] == '\0'){
            YEET;
        }

        char *token = strtok(inputLine, "\n "); 
        
        struct process *nextProcess;
        nextProcess = (struct process*)malloc(sizeof(struct process));
        nextProcess->file = inputLine;

        //if using serial architecture, just run normally
        if (!usingThreads) {
            nextProcess->stats = stats[0];
            processFile(nextProcess);
        }

        //when we are using threads
        else if (usingThreads) {

            //make first thread
            if (currentThread < numThreads) {
                threads[currentThread] = (pthread_t *)malloc(sizeof(pthread_t));
                nextProcess->stats = stats[currentThread];
                pthread_create(threads[currentThread], NULL, processFile, (void *)nextProcess);
                currentThread++;
            }

            //join oldest, then remake accordingly
            else {
                pthread_join(*(threads[oldestThread]), NULL);
                nextProcess->stats = stats[oldestThread];
                pthread_create(threads[oldestThread], NULL, processFile, (void *)nextProcess);

                //update oldest
                if (oldestThread == numThreads -1 ) {
                    oldestThread = 0;
                }

                else {
                    oldestThread++;
                }

            }
        }
    }
    
    //finish everything before moving on
    if (usingThreads) {

        //join all the threads together
        for (i = 0; i< numThreads; i++) {
            pthread_join(*(threads[i]), NULL);
        }
    }

    printStats();

    //print time results
    getrusage(RUSAGE_SELF, &usage);
    gettimeofday(&end, 0);
    double wallClock = ((end.tv_sec - start.tv_sec)*1000) + ((double)(end.tv_usec - start.tv_usec))/1000;
    double userTime = ((usage.ru_utime.tv_sec)*1000) + ((double)(usage.ru_utime.tv_usec))/1000;
    double systemTime = ((usage.ru_stime.tv_sec)*1000) + ((double)(usage.ru_stime.tv_usec))/1000;
    printf("System Time (ms): %f\n", systemTime);
    printf("User Time (ms): %f\n", userTime);
    printf("Wall Clock Time (ms): %f\n", wallClock);

    return 0;
}

void *processFile(void *processPointer) {
    struct process *nextProcess = (struct process *)processPointer;
    char *file = nextProcess->file;
    struct stat *stats = nextProcess->stats;

    char current[1];
    int statRet;
    int fileInput;
    int canRead;
    int txtBytes;
    statRet = stat(file, stats);

    //bad file
    if (statRet < 0) {
        pthread_mutex_lock(&mutex);
        totBadFiles++;
        pthread_mutex_unlock(&mutex);
	    return (void*) 0;
    }

    //directory
    if (S_ISDIR(stats->st_mode)) {
        pthread_mutex_lock(&mutex);
        totDirs++;
        pthread_mutex_unlock(&mutex);
    }

    //regular file
    if (S_ISREG(stats->st_mode)) {
        pthread_mutex_lock(&mutex);
        totRegFiles++;
        totRegBytes = totRegBytes + stats->st_size;
        pthread_mutex_unlock(&mutex);
    }

    //special file
    else if (!(S_ISDIR(stats->st_mode)) && !(S_ISREG(stats->st_mode))) {
        pthread_mutex_lock(&mutex);
        totSpecFiles++;
        pthread_mutex_unlock(&mutex);
    }

    //text file shit
    if ((fileInput = open(file, O_RDONLY)) < 0) {
	    return (void*) 0;
    }
    
    txtBytes = 0;
    while ((canRead = read(fileInput, current, 1)) > 0) {

        //if they are txt characters
        if (isprint(current[0]) || isspace(current[0])) {
            txtBytes++;
        }
        else { //not readable txt characters
            txtBytes = 0;
            break;
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

    //problem with initializing mutex
    if (pthread_mutex_init(&mutex, NULL) < 0) {
        perror("init mutex error");
        exit(1);
    }
}

void printStats(){ 
    printf("Bad Files: %ld\n", totBadFiles);
    printf("Directories: %ld\n", totDirs);
    printf("Regular Files: %ld\n", totRegFiles);
    printf("Special Files: %ld\n", totSpecFiles);
    printf("Bytes in Regular Files: %ld\n", totRegBytes);
    printf("Text Files: %ld\n", totTxtFiles);
    printf("Bytes in Text Files: %ld\n", totTxtBytes);
}