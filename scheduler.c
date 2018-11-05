#include "sched.h"

typedef struct ready_queue {
    PCB *process;
    struct ready_queue *next;
} ReadyQueue;
