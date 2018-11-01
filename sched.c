#include "sched.h"

#define PCB_STRUCT_SIZE sizeof(PCB)

/**
 * This function splits the string with the given delimiter.
 *
 * @param str the target string that should be splited
 * @param del the delimiter
 * @return the 2D pointer, which is a string array that stores the splited strings
 */
char **split(char *str, const char del) {
    char **result = 0;
    size_t count = 0;
    char *tmp = str;
    char *last_comma = 0;
    char delim[2] = {del, 0};

    /* Count the number of delimiters. */
    while (*tmp) {
        if (del == *tmp) {
            count++;
            last_comma = tmp;
        }
        tmp++;
    }

    /* Add space for the trailing token. */
    count += last_comma < (str + strlen(str) - 1);

    /* Add a space for terminating null string so caller knows where the list of returned strings ends. */
    count += 1;

    result = (char **) malloc(sizeof(char *) * count);

    if (result) {
        unsigned idx = 0;
        char *token = strtok(str, delim);

        while (token) {
            *(result + idx) = strdup(token); //use the strdup() to copy the token string
            token = strtok(0, delim);
        }
        *(result + idx) = 0;
    }

    return result;
}

/**
 * The aim of this function is to create the child process and add the process information to the process control block.
 *
 * @param config_file the file name of the config file
 * @param plist the pointer that points to the linked list of the process control block.
 * @return the pointer that points to the last process control block
 */
PCB *createProcesses(char *config_file, PCB *plist) {
    FILE *fp = fopen(config_file, "r");
    char *line = NULL;

    if (fp == NULL) {
        perror("Error opening file");
    }

    int val;

    while ((val = fscanf(fp, "%[^\n]", line)) > 0) { // reads text until newline
        char **str = split(line, ' '); //split the read line with the whitespace.

        pid_t pid = fork();

        if (pid < 0) {
            fprintf(stderr, "Failed to execute the process %s\n", str[1]);
        } else if (pid > 0) {
            kill(pid, SIGSTOP); //send the STOP signal to the corresponding process

            PCB *newProcess = (PCB *)malloc(PCB_STRUCT_SIZE);

            newProcess->prev = plist;
            newProcess->pid = pid;
            newProcess->next = NULL;

            newProcess->pathName = str[1];

            plist = newProcess;
        } else {
            fclose(fp);
            //TODO execute the process
        }

        //TODO free the splited string -> strdup (except str[1])
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
