
#include <iostream>
using namespace std;
#include <sys/types.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <sys/wait.h>
#include <string.h>
#include <vector>


typedef struct {
    int pid;
    string cmd;
    long start;
} process; 

char *prompt = (char *)malloc(16 * sizeof(char));
vector<process> children; //vector to dynamically allocade

void printStats(long startTime){ //start time in ms
    struct rusage usage;
    struct timeval processEnd, userEnd, systemEnd;
    gettimeofday(RUSAGE_CHILDREN, &processEnd)
    long endTime = (processEnd.tv_sec * 1000) + (processEnd.tv_sec / 1000);
    userEnd = usage.ru_utime;
    systemEnd = usage.ru_stime;
    double userEndTime = (userEndTime.tv_sec * 1000) + (userEndTime.tv_sec / 1000);
    double systemEndTime = (systemEndTime.tvSec * 1000) + (systemEndTime.tv_sec / 1000);
    double wallClockTime = endTime - startTime;
    cout << "System Statistics For Process:/n";
    cout << "/tUser CPU Time: " << userEndTime << " milliseconds";
    cout << "/tSystem CPU Time: " << systemEndTime << " milliseconds";
    cout << "/tWall Clock Time: " << wallClockTime << " milliseconds";
}

int run (char ** inputArgs)
{
    int pid;
    long startTime;
    struct timeval processStart;

    gettimeofday(&processStart, NULL);
    startTime = (processStart.tv_sec * 1000) + (processStart.tv_sec / 1000);

    //fork process
    pid = fork();

    if(pid < 0){ //fork error
        cerr << "FORK ERROR\n";
        exit(1);
    }
    else if(pid == 0) { //child process
        if(execvp(inputArgs[0], iinputArgs) < 0){ //execution error
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
            cout << "[" << children.size() << "] " << children.back().pid << endl
            return 0;
        }
    }

}

int main(int argc, char *argv[]){
    char ** newArgs = (char *)malloc(16 * sizeOf(char));
    
    for(i = 1; i < argc; i++){
        newArgs[i - 1] = argv[i];
    }
    newArgs[argc - 1] = NULL;
    execute(newArgs);
}



