#include "sched.h"

#define PCB_STRUCT_SIZE sizeof(PCB)

PCB *createProcesses(char *config_file, PCB *plist) {
    FILE *fp = fopen(config_file, "r");
    char *line = NULL;

    if (fp == NULL) {
        perror("Error opening file");
    }

    int val;

    while ((val = fscanf(fp, "%[^\n]", line)) > 0) { // reads text until newline
        //TODO split the read line for the execl
        pid_t pid = fork();

        if (pid < 0) {
            fprintf(stderr, "Failed to execute the process"); //TODO add process name to the error msg
        } else if (pid > 0) {
            kill(pid, SIGSTOP); //send the STOP signal to the corresponding process

            PCB *newProcess = (PCB *)malloc(PCB_STRUCT_SIZE);

            newProcess->prev = plist;
            newProcess->pid = pid;
            newProcess->next = NULL;

            plist = newProcess;
        } else {
            fclose(fp);
            //TODO execute the process
        }
    }

    fclose(fp);
    return plist;
}

int main(int argc, char **argv) {
    if (argc < 2) {
        printf("Usage: sched config_file ...");
    } else {
        PCB *pcb = NULL; // The linked list that will store the process id of each process.

        int i;
        for (i = 0; i < argc; i++) {
            pcb = createProcesses(argv[i], pcb);
        }

        /*
         * If the process control list is null, that means that there are no process that the scheduler should manage.
         * Therefore, the scheduler should be terminated, as long as there is no process to manage.
         */
        if (pcb == NULL) {
            exit(0);
        }

        /**
         * This part will only be run by the parent process.
         */
        if (pcb->pid != 0) {
            //
        }
    }

    return 1;
}
