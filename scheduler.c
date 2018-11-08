#include "sched.h"

typedef struct ready_queue {
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
    ReadyQueue *queue = (ReadyQueue *) malloc(sizeof(ReadyQueue));

    queue->process = p_list;
    p_list = p_list->next;

    if (p_list) { // check if there are more process control blocks in the linked list of pcb.
        queue->next = makeQueue(p_list);
    } else {
        queue->next = NULL;
    }

    return queue;
}

void roundRobin(ReadyQueue *queue) {
    PCB *pcb;

    while (queue) {
        pcb = queue->process;
        kill(pcb->pid, SIGCONT);
        usleep(5000);
        kill(pcb->pid, SIGSTOP);
        queue = queue->next;
    }
}

void scheduleProcesses(PCB *p_list) {
    // allocate the memory to make the ready queue recursively
    ReadyQueue *queue = makeQueue(p_list);

    //TODO
}
