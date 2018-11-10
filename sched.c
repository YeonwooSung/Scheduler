#include "sched.h"

#define PCB_STRUCT_SIZE sizeof(PCB)

/**
 * The aim of this function is to split the linked list of the process control blocks in half.
 * To devide it in half, I used fast node and slow node, where the fast node moves 2 blocks when the
 * slow node moves only one block.
 *
 * @param (head) The pointer that points to the head node of the linked list
 * @return The pointer that points to the central node of the linked list
 */
PCB *splitProcessControlBlocks(PCB *head) {
    PCB *fast = head, *slow = head;

    while (fast->next && fast->next->next) {
        fast = fast->next->next;
        slow = slow->next;
    }

    PCB *temp = slow->next;
    slow->next = NULL;
    return temp;
}

/**
 * The aim of this function is to merge the splited nodes of the linked list for the merge sort.
 *
 * @param (first) the pointer that points to the first half
 * @param (second) the pointer that points to the second half
 * @return The pointer that points to the head node of the merged linked list.
 */
PCB *mergePCB(PCB *first, PCB *second) {
    // check if first linked list is empty
    if (!first) {
        return second;
    }

    // check if second linked list is empty
    if (!second) {
        return first;
    }

    // Pick the smaller value
    if (first->priority < second->priority) { //TODO check the priority and index!

        first->next = mergePCB(first->next, second);
        first->next->prev = first;
        first->prev = NULL;
        return first;

    } else if (first->priority != second->priority || first->index > second->index) {

        second->next = mergePCB(first, second->next);
        second->next->prev = second;
        second->prev = NULL;
        return second;

    } else {
        second->next = mergePCB(first, second->next);
        second->next->prev = second;
        second->prev = NULL;
        return second;
    }
}

/**
 * This function sorts the process control blocks with the merge sort algorithm.
 *
 * @param (headNode) The head node of the linked list.
 * @return The pointer that points to the merge-sorted linked list.
 */
PCB *mergeSort(PCB *headNode) {
    if (!headNode || !headNode->next) {
        return headNode;
    }

    PCB *second = splitProcessControlBlocks(headNode);

    // Recur for the first half and the second half of the linked list.
    headNode = mergeSort(headNode);
    second = mergeSort(second);

    // Merge the two sorted halves, and return the sorted linked list.
    return mergePCB(headNode, second);
}

/**
 * This function trims the string to remove the whitespaces at the right-side of the given string.
 *
 * @param (s) the pointer that points to the target string
 * @return trimmed string
 */
char *trimStr(char *s) {
    char t[300];
    char *end;

    strcpy(t, s);
    end = t + strlen(t) - 1;

    while (end != t && isspace(*end)) { //(*end < 33)
        end--;
    }

    *(end + 1) = '\0';

    char *newStr = (char *) malloc(strlen(t) + 1);
    strcpy(newStr, t);

    free(s); //free the given string

    return newStr;
}

/**
 * This function splits the string with the given delimiter.
 *
 * @param (str) The target string that should be splited
 * @param (del) The delimiter
 * @param (counter) To count the number of splited strings
 * @return The 2D pointer, which is a string array that stores the splited strings
 */
char **splitStr(char *str, const char del, size_t *counter) {
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

    strArr = malloc(sizeof(char *) * count);

    size_t idx = 0;

    if (strArr) {
        char *token = strtok(str, delim);

        while (token) {
            *(strArr + idx++) = strdup(token); //use the trim() to remove the whitespaces in the token
            token = strtok(0, delim);
        }
        *(strArr + idx) = 0;
    }

    *counter = idx;

    return strArr;
}

/**
 * The aim of this function is to free the strings in the string array.
 * It will free all strings except strArr[1], which is the file path name of the executable file of the child process.
 *
 * @param (strArr) The string array that contains the splited string, which should be freed
 * @param (counter) The number of string elements that the string array has
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
 * @param (config_file) The file name of the config file
 * @param (plist) The pointer that points to the linked list of the process control block.
 * @param (index) The pointer that points to the index of the process.
 * @return The pointer that points to the last process control block
 */
PCB *createProcesses(char *config_file, PCB *plist, unsigned *index) {
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

        char **strArr = splitStr(line, ' ', &counter); //split the read line with the whitespace.

        *(strArr + counter - 1) = trimStr(strArr[counter - 1]);

        /* 
         * The 2D pointer argv is a string array that will be used for list of the command line 
         * arguments of child process.
         */
        char **argv = malloc(sizeof(char *) * counter);
        char **temp = argv;

        size_t counting = 1;

        while (counting < counter) {
            *temp = (char *) malloc(strlen(strArr[counting]) + 1);
            strcpy(*temp, strArr[counting]);

            temp += 1;
            counting += 1;
        }

        *temp = NULL;

        pid_t pid = fork();

        if (pid < 0) {
            fprintf(stderr, "Failed to execute the process %s\n", strArr[1]);
        } else if (pid > 0) {

            kill(pid, SIGSTOP); //send the STOP signal to the corresponding process

            PCB *newProcess = (PCB *) malloc(PCB_STRUCT_SIZE);

            newProcess->prev = plist; // append the new node to the linked list
            newProcess->pid = pid;    // set the process id
            newProcess->next = NULL;  // set the next node as NULL

            if (plist) {
                plist->next = newProcess;
            }

            // set the file path name of the process
            newProcess->pathName = strArr[1];
            newProcess->priority = atoi(strArr[0]);

            // set the index of the process
            newProcess->index = *index;
            *index += 1; //increase the index

            plist = newProcess;

        } else {
            /* 
             * The first argument of the execv should be the file path to the executable file.
             * And the second argument of the execv is the string array that contains the command
             * line arguments of the child process.
             */
            execv(strArr[1], argv);
        }

        temp = argv;

        //free the string array, which is used for the commnad line arguments of the child process.
        while (counting < counter) {
            free(temp);
            temp += 1;
            counting += 1;
        }
        free(argv);

        freeStrings(strArr, counter); //free the splited strings (except the file path name)
        free(strArr);
    }

    fclose(fp);
    return plist;
}

/**
 * This function frees all allocated memory.
 *
 * @param (p_list) The pointer that points to the head node of the linked list
 */
void freeList(PCB *p_list) {
    while (p_list) {
        free(p_list->pathName);
        PCB *next = p_list->next;
        free(p_list);
        p_list = next;
    }
}

/* The sched is a simple process scheduler that runs on the user mode. */
int main(int argc, char **argv) {

    //it requires at least one command line argument, which should be the file path name of the config file.
    if (argc < 2) {
        printf("Usage: sched config_file ...\n");
    } else {

        PCB *pcb = NULL; // The linked list that will store the process id of each process.

        int i;
        unsigned index = 0;

        for (i = 1; i < argc; i++) {
            pcb = createProcesses(argv[i], pcb, &index);
        }

        /*
         * If the process control list is null, that means that there are no process that the scheduler should manage.
         * Therefore, the scheduler should be terminated, as long as there is no process to manage.
         */
        if (pcb == NULL) {
            exit(0);
        }

        /* This part will only be run by the parent process. */
        if (pcb->pid > 0) {

            while (pcb->prev) { // use the while loop to find the head node of the linked list of process control blocks.
                pcb = pcb->prev;
            }

            //TODO found some error in the mergeSort()
            pcb = mergeSort(pcb); //sort the linked list of process control block with the merge sort algorithm

            // call the scheduleProcesses() function to schedule the processes
            scheduleProcesses(pcb);

            freeList(pcb); //free the dynamically allocated memory

            printf("\n\nFinishing.....................\n");
        }
    }

    return 1;
}
