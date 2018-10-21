#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <limits.h>
#include <signal.h>
#include <sys/wait.h>
#include <string.h>

int main(int argc, char **argv) {
    pid_t firstpid = 0, lastpid = 0;

    // Start first process
    firstpid = fork();

    if(firstpid < 0)
        printf("Something went wrong!\n");
    else if(firstpid > 0)    // We are parent. Immediately stop the new process
        kill(firstpid,SIGSTOP);  
    else  // We are a child process -- overwrite our process space with the new program
        execl("./printchars", "./printchars", "a", NULL);       // Print some "a"s

    // Start second process
    lastpid = fork();

    if(lastpid < 0)
        printf("Something went wrong!\n");
    else if(lastpid > 0)    // We are parent. Immediately stop the new process
        kill(lastpid,SIGSTOP);  
    else // We are a child process -- overwrite our process space with the new program
        execl("./printchars", "./printchars", "b", NULL);       // Print some "b"s

    // This part will only be run by the parent process -- the scheduler
    if(lastpid) {
        printf("Parent here.\n");
        kill(firstpid, SIGCONT);        // Start P1
        usleep(500000);
        kill(firstpid, SIGSTOP);
        kill(lastpid, SIGCONT);         // Start P2
        usleep(500000);
        kill(lastpid, SIGSTOP);
        kill(firstpid, SIGCONT);        // Start P1
        usleep(500000);
        kill(firstpid, SIGSTOP);
        kill(lastpid, SIGCONT);         // Start P2....
        usleep(500000);
        kill(lastpid, SIGSTOP);
    }

    // Kill all processes before finishing.
    printf("\nFinishing...\n");
    kill(firstpid, SIGTERM);
    kill(lastpid, SIGTERM);
}
