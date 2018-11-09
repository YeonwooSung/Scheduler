#include "sched.h"

typedef struct ready_queue {
    PCB *process;
    struct ready_queue *next;
} ReadyQueue;

/**
 * The aim of this function is to allocate the memory to make the ready queue.
 * It checks the priority of each pcb, and add the pcb to the queue only when the priority of the pcb
 * is in the given range.
 *
 * @param (queue) the pointer that points to the ready queue
 * @param (p_list) the pointer that points to the linked list of pcb
 * @return The linked list of remaining process control blocks.
 */
PCB *makeQueue(ReadyQueue *queue, PCB *p_list) {
    unsigned basePriority = p_list->priority;
    unsigned limitPriority = basePriority + 5;

    queue->process = p_list;
    p_list = p_list->next;

    while (p_list) {
        if (p_list->priority > limitPriority) {
            break;
        }

        queue->next = (ReadyQueue *) malloc(sizeof(ReadyQueue));
        queue = queue->next;
        queue->process = p_list;
        
        p_list = p_list->next;
    }

    queue->next = NULL;

    return p_list;
}

void roundRobin(ReadyQueue *queue) {
    pid_t pid;

    //TODO use the endless loop, and free the node of the finished process

    while (queue) {
        pid = queue->process->pid;
        kill(pid, SIGCONT);
        usleep(5000);
        kill(pid, SIGSTOP);

        //TODO check if the child process is terminated
        int status;
        // just simply check if the process with the given pid is terminated.
        int ret = waitpid(pid, &status, WNOHANG); // use WNOHANG option not to wait.

        queue = queue->next;
    }
}

void scheduleProcesses(PCB *p_list) {
    // allocate the memory to make the ready queue recursively
    ReadyQueue *queue = (ReadyQueue *) malloc(sizeof(ReadyQueue));

    /*
     *
     */
    PCB *remaining = makeQueue(queue, p_list);

    //TODO

    if (remaining) {
        //call itself recursively to schedule remaining processes.
        scheduleProcesses(remaining);
    }
}
