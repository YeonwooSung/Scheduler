#include "sched.h"

typedef struct ready_queue {
    bool terminated;
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
    ReadyQueue *queue = (ReadyQueue *)malloc(sizeof(ReadyQueue));

    queue->process = p_list;
    p_list = p_list->next;

    // check if there are more process control blocks in the linked list of pcb.
    if (p_list) {
        //call itself recursively to make the next node
        queue->next = makeQueue(p_list);
    } else {
        queue->next = NULL;
    }

    return queue;
}

void roundRobin(ReadyQueue *queue) {
    pid_t pid;

    //TODO use the endless loop, and free the node of the finished process

    while (queue) {
        pid = queue->process->pid;
        kill(pid, SIGCONT);
        usleep(5000);
        kill(pid, SIGSTOP);

        int status;
        // just simply check if the process with the given pid is terminated.
        int ret = waitpid(pid, &status, WNOHANG); // use WNOHANG option not to wait.

        if (ret == pid) { //if the given process is terminated, then the waitpid returns the pid.
            queue->terminated = true;
        }

        queue = queue->next;
    }
}

void scheduleProcesses(PCB *p_list) {
    // allocate the memory to make the ready queue recursively
    ReadyQueue *queue = makeQueue(p_list);

    //TODO use the proper scheduling function
}
