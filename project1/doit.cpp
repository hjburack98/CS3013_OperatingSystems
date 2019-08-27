
#include <iostream>
using namespace std;
#include <sys/types.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <sys/wait.h>
#include <string.h>
#include <vector>
#include <unistd.h>
#include <stdlib.h>


typedef struct {
    int pid;
    string cmd;
    long start;
} process; 

int ampersand = 0; //if process in running in background
vector<process> children; //vector to dynamically allocade

void printStats(long startTime){ //start time in ms
    struct rusage usage;
    struct timeval processEnd, userEnd, systemEnd;
    gettimeofday(&processEnd, NULL);
    long endTime = ((processEnd.tv_sec * 1000));//) + (processEnd.tv_sec / 1000));
    getrusage(RUSAGE_CHILDREN, &usage);
    userEnd = usage.ru_utime;
    systemEnd = usage.ru_stime;
    double userEndTime = ((userEnd.tv_sec * 1000) + (userEnd.tv_sec / 1000));
    double systemEndTime = ((systemEnd.tv_sec * 1000) + (systemEnd.tv_sec / 1000));
    double wallClockTime = endTime - startTime;
    cout << "System Statistics For Process:\n";
    cout << "     User CPU Time: " << userEndTime << " milliseconds\n";
    cout << "     System CPU Time: " << systemEndTime << " milliseconds\n";
    cout << "     Wall Clock Time: " << wallClockTime << " milliseconds\n";
}


int run (char ** inputArgs)
{
    int pid;
    long startTime;
    struct timeval processStart;

    gettimeofday(&processStart, NULL);
    startTime = ((processStart.tv_sec * 1000) + (processStart.tv_sec / 1000));

    //fork process
    pid = fork();

    if(pid < 0){ //fork error
        cerr << "FORK ERROR\n";
        exit(1);
    }
    else if(pid == 0) { //child process
        if(execvp(inputArgs[0], inputArgs) < 0){ //execution error
            cerr << "EXECUTION ERROR\n";
            exit(1);
        }
        return 0;
    }
    else { //parent process
        if(!ampersand) { //if there are no background processes
            wait(0);
            printStats(startTime);
            return 0;
        }

        else {
            process child = {pid, inputArgs[0], startTime};
            children.push_back(child);
            cout << "[" << children.size() << "] " << children.back().pid << endl;
            return 0;
        }
    }

}

int main(int argc, char *argv[]){
    char **newArgs = (char **)malloc(32 * sizeof(char *));
    int i;
    
    for(i = 1; i < argc; i++){
        newArgs[i - 1] = argv[i];
    }
    newArgs[argc - 1] = NULL;
    run(newArgs);
}



