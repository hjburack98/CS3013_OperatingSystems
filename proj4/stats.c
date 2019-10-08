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

#include "stats.h"

#define MAX_LINE 256
#define YEET break

int main(int argc, char** argv){
    int i;
    char* inputLine;
    char* inputStatus;
    struct stat **buffer;

    setup();

    //serial architecture
    printf("Serial Architecture:\n");
    buffer = (struct stat**)malloc(sizeof(struct stat*));
    buffer[0] = (struct stat*)malloc(sizeof(struct stat));

    while(1){
        inputLine = (char *)malloc(sizeof(char) * MAX_LINE);
        inputStatus = fgets(inputLine, MAX_LINE, stdin); //gets name

        //If there is a problem or it reached EOF
        if(inputStatus == NULL){
            YEET;
        }
        if(inputLine == NULL){
            YEET;
        }
        if(inputLine[0] == '\0'){
            YEET;
        }

        char *tok = strtok(inputLine, "\n "); //get rid of trailing spaces and next line char

        //create file to get info
        struct file *fileToUse;
        fileToUse = (struct file*)malloc(sizeof(struct file));
        fileToUse->name = inputLine;


        //process file
        fileToUse->stats = buffer[0];
        getStats(fileToUse);
    }


    printStats();

    return 0;
}


void setup() {
    totBadFiles = 0;
    totDirs = 0;
    totRegFiles = 0;
    totSpecFiles = 0;
    totRegBytes = 0;
    totTxtFiles = 0;
    totTxtBytes = 0;
}

void* getStats(void *file){
    struct file *fileToUse = (struct file *)file;
    char *name = fileToUse->name;
    struct stat *stats = fileToUse->stats;

    char current[1];
    int returnStat;
    int openFile;
    int txtBytes;

    returnStat = stat(file, stats);

    //bad file
    if(returnStat < 0){
        totBadFiles++;
        return (void *) 0;
    }

    //directories
    if (S_ISDIR(stats->st_mode)) {
        totDirs++;
    }

    //regular files
    if(S_ISREG(stats->st_mode)){
        totRegFiles++;
        totRegBytes = totRegBytes + stats->st_size;
    }

    //special files
    if(!(S_ISDIR(stats->st_mode)) && !(S_ISREG(stats->st_mode))){
        totSpecFiles++;
    }

    //can't open file
    openFile = open(file, O_RDONLY);
    if(openFile < 0){
        return (void *) 0;
    }
    
    //txt files
    txtBytes = 0;

    while(read(openFile, current, 1) > 0){
        if(isprint(current[0]) || isspace(current[0])){
            txtBytes++;
        }
        else{
            txtBytes = 0;
            YEET;
        }
    }

    if(txtBytes > 0){
        totTxtBytes = totTxtBytes + txtBytes;
        totTxtFiles++;
    }

    //finish
    return (void *) 0;

}

void printStats(){ 
    printf("Bad Files: %ld\n", totBadFiles);
    printf("Directories: %ld\n", totDirs);
    printf("Regular Files: %ld\n", totRegFiles);
    printf("Special Files: %ld\n", totSpecFiles);
    printf("Regular File Bytes: %ld\n", totRegBytes);
    printf("Text Files: %ld\n", totTxtFiles);
    printf("Text File Bytes: %ld\n", totTxtBytes);
}