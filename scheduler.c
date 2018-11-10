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

/**
 * This function manages the pcbs with the priority based scheduling algorithm.
 * The priority based scheduling is a non-preemptive algorithm.
 *
 * As I used the merge sort to sort the linked list of pcb based on the priority of each process,
 * the ready queue is ordered by the priority of the processes. Thus, this function just activate
 * each processes and wait until the child process finishes.
 *
 * @param (queue) the ready queue that is ordered by the priority of the processes
 */
void priorityBasedScheduling(ReadyQueue *queue) {
    int status;

    while (queue) {
        kill(queue->process->pid, SIGCONT);

        // wait until the child process is terminated.
        int ret = waitpid(queue->process->pid, &status, 0);

        queue = queue->next;
    }
}

/**
 * This function manages the process control blocks with the round robin scheduling algorithm.
 * Round Robin is the preemptive process scheduling algorithm. And each process is provided
 * a fix time to execute, it is called a quantum. Once a process is executed for a given time
 * period, it is preempted and other process executes for a given time period.
 *
 * @param (queue) the ready queue that contains the processes that should be scheduled.
 */
void roundRobin(ReadyQueue *queue) {
    pid_t pid;
    char checker;
    ReadyQueue *temp = queue; //to store the memory address of the head node of the ready queue

    for (;;) {
        checker = 1; //initialise the value of the local variable checker

        while (queue) {

            if (queue->terminated == 0) { //to check if the process of the current node is terminated
                pid = queue->process->pid;
                kill(pid, SIGCONT);
                usleep(500000);
                kill(pid, SIGSTOP);

                int status;
                // just simply check if the process with the given pid is terminated.
                int ret = waitpid(pid, &status, WNOHANG); // use WNOHANG option not to wait.

                if (ret != 0) { //if the given process is terminated, then the waitpid returns the pid.
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
