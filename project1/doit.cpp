
#include <iostream>
using namespace std;
#include <sys/types.h>
#include <sys/resource.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>
#include <sys/time.h>
#include <vector>
#include <stdlib.h>
#define MAX_CHARS 128
#define MAX_ARGS 32

typedef struct {
	int pid;
	string cmd;
	long start;
} process;

char *prompt = (char *)malloc(32 * sizeof(char));
int ampersand = 0; //if process in running in background
vector<process> children; //vector to dynamically allocade


void printStats(long startTime) { //start time in ms
	struct rusage usage;
	struct timeval processEnd, userEnd, systemEnd;

    //set end time
	gettimeofday(&processEnd, NULL);
	long endTime = (processEnd.tv_sec * 1000) + (processEnd.tv_usec / 1000);

    //get all the data for stats
	getrusage(RUSAGE_CHILDREN, &usage);
	userEnd = usage.ru_utime;
	systemEnd = usage.ru_stime;

    //convert times to ms
	double userTime = ((userEnd.tv_sec * 1000) + (userEnd.tv_usec / 1000));
	double systemTime = ((systemEnd.tv_sec * 1000) + (systemEnd.tv_usec / 1000));
	double wallClockTime = endTime - startTime;
	cout << "USAGE STATS:\n";
	cout << "   system time = " << systemTime << " ms\n";
	cout << "   user time = " << userTime << " ms\n";
	cout << "   wall clock time = " << wallClockTime << " ms\n";
	cout << "   involuntary context switches = " << usage.ru_nivcsw << endl;
	cout << "   voluntary context switches = " << usage.ru_nvcsw << endl;
	cout << "   page faults requiring I/O = " << usage.ru_majflt << endl;
	cout << "   page faults serviced without I/O = " << usage.ru_minflt << endl;
}

void safeExit() {
	if (children.size() > 0) { //if the child is still running
		cout << "Waiting for " << children.size() << " process(es) to finish" << endl;
		for (unsigned long i = 0; i < children.size(); i++) {
			int status;
			pid_t result = waitpid(children.at(i).pid, &status, 0);

			if (result > 0) { //if the child finished
				cout << "[" << i + 1 << "] " << children.at(i).pid << " Completed\n";
				printStats(children.at(i).start);
			}
		}
	}
	exit(0);
}

int run(char ** newArgs) {
	int pid;
	long start_ms;
	struct timeval wc_start;

	gettimeofday(&wc_start, NULL);
	start_ms = ((wc_start.tv_sec * 1000) + (wc_start.tv_usec / 1000));

	/* fork it */
	pid = fork();
		
	if (pid < 0) { //fork error
		cerr << "FORK ERROR\n";
		exit(1);
	} 
    else if (pid == 0) { //child running
		if (execvp(newArgs[0], newArgs) < 0) { //execution error
			cerr << "EXECUTION ERROR\n";
			exit(1);
		}
		return 0;
	} 
    else { //parent running
		if (!ampersand) { //if no background processes
			wait(0);
			printStats(start_ms);
			return 0;
		} 
        else {
			process child = {pid, newArgs[0], start_ms};
			children.push_back(child);
			cout << "[" << children.size() << "] " << children.back().pid << endl;
			return 0;
		}
	}
}

int main(int argc, char *argv[]) {
	char **newArgs = (char **)malloc(MAX_ARGS * sizeof(char *));
	int i;
	prompt[0] = '=';
	prompt[1] = '=';
	prompt[2] = '>';
	prompt[3] = '\0';

	if (argc > 1) { //Command Line
		for (i = 1; i < argc; i++) {
			newArgs[i - 1] = argv[i];
		}
		newArgs[argc - 1] = NULL;
		run(newArgs);
	} 

    else { //Shell
		while (1) {
			// print prompt
			cout << prompt << " ";

			char line[MAX_CHARS];
			char *token;
			int position = 0;

			cin.getline(line, MAX_CHARS); //read shell line

			//check to see if background processes finished
			for (unsigned long j = 0; j < children.size(); j++) {
				int childStatus;
				pid_t result = waitpid(children.at(j).pid, &childStatus, WNOHANG);

				if (result > 0) { // child quit
					cout << "[" << j + 1 << "] " << children.at(j).pid << " Completed\n";
					printStats(children.at(j).start);
					children.erase(children.begin() + j);
				}
			}

			// tokenize the input line
			token = strtok(line, " ");
			while (token != NULL) {
				newArgs[position] = token;
				token = strtok(NULL, " ");	
				position++;
			}

            //Check for background process
			if (strcmp(newArgs[position - 1], "&") == 0) { 
				ampersand = 1;
				newArgs[position - 1] = NULL;
			} 
            else {
				ampersand = 0;
				newArgs[position] = NULL;
			}
			if (strcmp(newArgs[0], "exit") == 0) { //exit
				safeExit();

			} 
            else if (strcmp(newArgs[0], "cd") == 0 && newArgs[1] != NULL)  { //cd
			    if (chdir(newArgs[1]) != 0){
					cerr << "chdir error\n";
                }

			} 
            else if (strcmp(newArgs[0], "set") == 0 && strcmp(newArgs[1], "prompt") == 0 && strcmp(newArgs[2], "=") == 0 && newArgs[3] != NULL) { //set prompt
				strcpy(prompt, newArgs[3]);
			} 
            else if (strcmp(newArgs[0], "jobs") == 0) {
				if (children.size() == 0) {
					cout << "No jobs running\n";
				} 
                else 
                {
					for (unsigned long i = 0; i < children.size(); i++) {
						cout << "[" << i + 1 << "] " << children[i].pid << " " << children[i].cmd << endl;
			    }
				}
			} 
            else {
				run(newArgs);
			}
		}
	}
}