#include "sched.h"

typedef struct ready_queue {
    char terminated;
    PCB *process;
    struct ready_queue *next;
} ReadyQueue;

/**
 * The aim of this function is to allocate the memory to make the ready queue.
 *
 * @param (p_list) the linked list of process control blocks
 * @return The pointer that points to the head node of the generated ready queue.
 */
ReadyQueue *makeQueue(PCB *p_list) {

    // check if there are more process control blocks in the linked list of pcb.
    if (p_list) {
        ReadyQueue *queue = (ReadyQueue *)malloc(sizeof(ReadyQueue));

        queue->process = p_list;
        queue->terminated = 0;

        //call itself recursively to make the next node
        queue->next = makeQueue(p_list->next);

        return queue;
    } else {
        return NULL;
    }

}

/**
 * This function frees the allocated memory of the ready queue.
 *
 * @param (queue) the pointer that points to the ready queue
 */
void freeQueue(ReadyQueue *queue) {
    while (queue) {
        ReadyQueue *temp = queue->next;
        free(queue);
        queue = temp;
    }
}

void roundRobin(ReadyQueue *queue) {
    pid_t pid;
    char checker;
    ReadyQueue *temp = queue;

    for (;;) {
        checker = 1; //initialise the value of the local variable checker

        while (queue) {

            if (!(queue->terminated)) { //to check if the process of the current node is terminated
                pid = queue->process->pid;
                kill(pid, SIGCONT);
                usleep(5000000);
                kill(pid, SIGSTOP);


                int status;
                // just simply check if the process with the given pid is terminated.
                int ret = waitpid(pid, &status, WNOHANG); // use WNOHANG option not to wait.

                if (ret == pid) { //if the given process is terminated, then the waitpid returns the pid.
                    queue->terminated = 1;
                }
            }

            checker = checker & queue->terminated; //use the AND operator to check if all processes are terminated.

            queue = queue->next;
        }

        // if all processes are terminated, then the value of the checker should be true
        if (checker) {
            break; // if all processes are terminated, break the endless for loop
        }

        queue = temp; //reset the queue pointer to the first node.
    }
}

void scheduleProcesses(PCB *p_list) {
    // allocate the memory to make the ready queue recursively
    ReadyQueue *queue = makeQueue(p_list);

    //TODO use the proper scheduling function
    roundRobin(queue);

    freeQueue(queue); //free the allocated memory
}
