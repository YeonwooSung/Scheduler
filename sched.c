#include "sched.h"

#define PCB_STRUCT_SIZE sizeof(PCB)

/**
 * This function splits the string with the given delimiter.
 *
 * @param str the target string that should be splited
 * @param del the delimiter
 * @param counter to count the number of splited strings
 * @return the 2D pointer, which is a string array that stores the splited strings
 */
char **split(char *str, const char del, size_t *counter) {
    char **result = 0;
    char *tmp = str;
    size_t count = 0;
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

    *counter = count;

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
 * The aim of this function is to free the strings in the string array.
 * It will free all strings except strArr[1], which is the file path name of the executable file of the child process.
 *
 * @param strArr the string array that contains the splited string, which should be freed
 * @param counter the number of string elements that the string array has
 */
void freeStrings(char **strArr, size_t counter) {
    free(strArr[0]);

    size_t count = 2;

    while (counter != count) {
        free(strArr[count]);
        count += 1;
    }
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
        size_t counter = 0; //to count the number of splited strings

        char **str = split(line, ' ', &counter); //split the read line with the whitespace.

        pid_t pid = fork();

        if (pid < 0) {
            fprintf(stderr, "Failed to execute the process %s\n", str[1]);
        } else if (pid > 0) {
            kill(pid, SIGSTOP); //send the STOP signal to the corresponding process

            PCB *newProcess = (PCB *) malloc(PCB_STRUCT_SIZE);

            newProcess->prev = plist;
            newProcess->pid = pid;
            newProcess->next = NULL;

            newProcess->pathName = str[1];
            newProcess->priority = atoi(str[0]);

            plist = newProcess;
            printf("%d, %s\n", pid, str[1]);
        } else {
            //TODO

            int ret = execl(str[1], str[1], str[2], str[3], str[4], NULL);

            //check if the execv fails
            if (ret == -1) {
                fprintf(stderr, "Failed to execute the process %s\n", str[1]);
            }
        }

        printf("(createProcess) pid: %d\n", pid);

        freeStrings(str, counter); //free the splited strings (except the file path name)
        free(str);
    }

    fclose(fp);
    return plist;
}

/* The sched is a simple process scheduler that runs on the user mode. */
int main(int argc, char **argv) {
    //it requires at least one command line argument, which should be the file path name of the config file.
    if (argc < 2) {
        printf("Usage: sched config_file ...\n");
    } else {
        PCB *pcb = NULL; // The linked list that will store the process id of each process.

        int i;
        for (i = 1; i < argc; i++) {
            pcb = createProcesses(argv[i], pcb);
            printf("%d\n", i);
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
            printf("pid: %d\npath: %s", pcb->pid, pcb->pathName);
        }
    }

    return 1;
}
