
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
#define MAX_ARGS 32
#define MAX_CHARS 128


typedef struct {
    int pid;
    string cmd;
    long start;
} process; 

char *prompt = (char *)malloc(16 * sizeof(char));
int ampersand = 0; //if process in running in background
vector<process> children; //vector to dynamically allocade

void printStats(long startTime){ //start time in ms
    struct rusage usage;
    struct timeval processEnd, userEnd, systemEnd;

    //set the end time
    gettimeofday(&processEnd, NULL);
    long endTime = ((processEnd.tv_usec / 1000));

    //get all data for stats
    getrusage(RUSAGE_CHILDREN, &usage);
    userEnd = usage.ru_utime;
    systemEnd = usage.ru_stime;
    double userEndTime = ((userEnd.tv_usec / 1000));
    double systemEndTime = ((systemEnd.tv_usec / 1000));
    double wallClockTime = endTime - startTime;

    //return stats
    cout << "System Statistics For Process:\n";
    cout << "     User CPU Time: " << userEndTime << " milliseconds\n";
    cout << "     System CPU Time: " << systemEndTime << " milliseconds\n";
    cout << "     Wall Clock Time: " << wallClockTime << " milliseconds\n";
    cout << "     Involuntary Context Switches: " << usage.ru_nivcsw << endl;
    cout << "     Voluntary Context Switches: " << usage.ru_nvcsw << endl;
    cout << "     Major Page Faults: " << usage.ru_majflt << endl;
    cout << "     Minor Page Faults: " << usage.ru_minflt << endl;
}


int run (char ** inputArgs)
{
    int pid;
    long startTime;
    struct timeval processStart;

    //set start time
    gettimeofday(&processStart, NULL);
    startTime = ((processStart.tv_usec / 1000));

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

        else { //with a background process
            process child = {pid, inputArgs[0], startTime};
            children.push_back(child);
            cout << "[" << children.size() << "] " << children.back().pid << endl;
            return 0;
        }
    }

}

void exit() {
    if(children.size() > 0) { //need child to finish running
        cout << children.size() << "still need to finish" << endl;
        
        for(unsigned long i = 0; i < children.size(); i++){
            int status;
            pid_t result = waitpid(children.at(i).pid, &status, 0);
            if(result > 0){ //once child has finished running
                cout << "[" << i+1 << "] " << children.at(i).pid << " completed\n";
                printStats(children.at(i).start);
            }
        }
    }
    exit(0);
}

int runLine(char **out){
    char line[MAX_CHARS];
    char *token;
    int position = 0;

    cin.getline(line, MAX_CHARS);
    token = strtok(line, " ");
    while(token != NULL) {
        out[position] = token;
        token = strtok(NULL, " ");
        position++;
    }

    if(strcmp(out[position - 1], "&") == 0){ //if there is a backround process
        ampersand = 1;
        out[position - 1] = NULL;
    }
    else {
        ampersand = 0;
        out[position - 1] = NULL;
    }

    if(strcmp(out[0], "exit") == 0) { //exit
        exit();
        return 1;
    }
    else if(strcmp(out[0], "cd") == 0 && out[1] != NULL){ //cd
        if(chdir(out[1]) != 0){
            cerr << "CHDIR ERROR\n";
        }
    }   
    else if(strcmp(out[0], "set") == 0 && strcmp(out[1], "prompt") == 0 && strcmp(out[2], "=") == 0 && out[3] != NULL) { //set prompt
        strcpy(prompt, out[3]);
    }
    else {
        run(out);
    }

    return 0;
}

int main(int argc, char *argv[]){
    //allocate required size
    char **newArgs = (char **)malloc(MAX_ARGS * sizeof(char *));
    int i;

    prompt[0] = '=';
    prompt[1] = '=';
    prompt[2] = '>';
    prompt[3] = '\0';

    if(argc > 1){ //if there's greater than one argument, code takes the command line argument
        for(i = 1; i < argc; i++){
        newArgs[i - 1] = argv[i];
        }

        newArgs[argc - 1] = NULL;
        run(newArgs);
    }
    /*else { //if there is only one argument, enter shell mode
        while(1){
            cout << prompt << " ";

            char line[MAX_CHARS];
            char *token;
            int position = 0;

            cin.getline(line, MAX_CHARS); //read line

            for(unsigned long j = 0; j < children.size(); j++){ //background processes
                int childStatus;
                pid_t result = waitpid(children.at(j).pid, &childStatus, WNOHANG);

                if(result > 0){ //once completed
                    cout << "[" << j + 1 << "] " << children.at(j).pid << " completed\n";
                    printStats(children.at(j).start);
                    children.erase(children.begin() + j);
                }
            }

            //make input a token
            token = strtok(line, " ");
            while(token != NULL){
                newArgs[position] = token;
                token = strtok(NULL, " ");
                position++;
            }


            if(strcmp(newArgs[position - 1], "&") == 0){ //if there is a backround process
                ampersand = 1;
                newArgs[position - 1] = NULL;
            }
            else {
                ampersand = 0;
                newArgs[position - 1] = NULL;
            }

            if(strcmp(newArgs[0], "exit") == 0) { //exit
                exit();
                return 1;
            }
            else if(strcmp(newArgs[0], "cd") == 0 && newArgs[1] != NULL){ //cd
                if(chdir(newArgs[1]) != 0){
                    cerr << "CHDIR ERROR\n";
                }
            }   
            else if(strcmp(newArgs[0], "set") == 0 && strcmp(newArgs[1], "prompt") == 0 && strcmp(newArgs[2], "=") == 0 && newArgs[3] != NULL) { //set prompt
                strcpy(prompt, newArgs[3]);
            }
            else {
                run(newArgs);
            }
        }
    } */
    else { //if there is only one argument, enter shell mode
		while(1) {
			// print prompt
			cout << prompt << " ";

			char line[MAX_CHARS];
			char *token;
			int position = 0;

			cin.getline(line, MAX_CHARS); // read in line

			// check for finishing background processes
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

			if (strcmp(newArgs[position - 1], "&") == 0) { // background process
				ampersand = 1;
				newArgs[position - 1] = NULL;
			} else {
				ampersand = 0;
				newArgs[position] = NULL;
			}
			if (strcmp(newArgs[0], "exit") == 0) { // exit command
				exit();
			} else if (strcmp(newArgs[0], "cd") == 0 && newArgs[1] != NULL)  { // cd command
				if (chdir(newArgs[1]) != 0)
					cerr << "chdir error\n";
			} else if (strcmp(newArgs[0], "set") == 0 && strcmp(newArgs[1], "prompt") == 0 && strcmp(newArgs[2], "=") == 0 && newArgs[3] != NULL) { // set prompt command
				strcpy(prompt, newArgs[3]);
			} else if (strcmp(newArgs[0], "jobs") == 0) {
				if (children.size() == 0) {
					cout << "No jobs running\n";
				} else {
					for (unsigned long i = 0; i < children.size(); i++) {
						cout << "[" << i + 1 << "] " << children[i].pid << " " << children[i].cmd << endl;
			}
				}
			} else {
				run(newArgs);
			}
        }
    }
   
}



