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
    char **strArr = 0;
    size_t count = 0;
    char *tmp = str;
    char *last_delimiter = 0;
    char delim[2];
    delim[0] = del;
    delim[1] = '\0';

    /* Count the number of elements that will be extracted. */
    while (*tmp) {
        if (del == *tmp) {
            count++;
            last_delimiter = tmp;
        }
        tmp++;
    }

    /* Add space for the trailing token. */
    count += last_delimiter < (str + strlen(str) - 1);

    /* Add space for terminating null string so caller knows where the list of returned strings ends. */
    count += 1;

    *counter = count;

    strArr = malloc(sizeof(char *) * count);

    if (strArr) {
        size_t idx = 0;
        char *token = strtok(str, delim);

        while (token) {
            *(strArr + idx++) = strdup(token);
            token = strtok(0, delim);
        }
        *(strArr + idx) = 0;
    }

    return strArr;
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
        return NULL;
    }

    int read;
    size_t len = 0;

    while ((read = getline(&line, &len, fp)) != -1) {// reads text until newline
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
        } else {
            //argv is a string array that will be used for list of the command line arguments of child process.
            char **argv = malloc(sizeof(char *) * (counter + 1));
            char **temp = argv;

            size_t counting = 1;

            while (counting <= counter) {
                *temp = (char *) malloc(strlen(str[counting]));
                strcpy(*temp, str[counting]);

                temp += 1;
                counting += 1;
            }
            *temp = NULL;

            /* 
             * The first argument of the execv should be the file path to the executable file.
             * And the second argument of the execv is the string array that contains the command
             * line arguments of the child process.
             */
            int ret = execv(str[1], argv);

            // check if the execv fails
            if (ret == -1) {
                fprintf(stderr, "Failed to execute the process %s\n", str[1]);
            }
        }

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
        }

        /*
         * If the process control list is null, that means that there are no process that the scheduler should manage.
         * Therefore, the scheduler should be terminated, as long as there is no process to manage.
         */
        if (pcb == NULL) {
            exit(0);
        }

        //TODO scheduling!!
    }

    return 1;
}
