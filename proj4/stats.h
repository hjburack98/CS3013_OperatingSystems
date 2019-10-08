#ifndef REQUESTS_H
#define REQUESTS_H

#include <stdio.h>
#include <fcntl.h>
#include <stdbool.h>
#include <ctype.h>
#include <sys/stat.h>

//totals
int totBadFiles;
int totDirs;
int totRegFiles;
int totSpecFiles;
int totRegBytes;
int totTxtFiles;
int totTxtBytes;

//functions
void setup();
void printStats();
void* getStats(void *file);

struct file{ 
    char* name;
    struct stat *stats;
};

#endif