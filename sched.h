#ifndef SCHED_H
#define SCHED_H

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <time.h>
#include <string.h>

typedef struct process_control_block {
    char priority;
    unsigned index;
    pid_t pid;
    char *pathName;
    struct process_control_block *prev;
    struct process_control_block *next;
} PCB;

void scheduleProcesses(PCB *p_list, char mode, unsigned int avgPriority);

double calculateToSeconds(struct timespec *begin, struct timespec *end) {
    long sec = end->tv_sec - begin->tv_sec;
    long nanoSec = end->tv_nsec - begin->tv_nsec;

    // check if start nanosecond is greater than end nanosecond
    if (begin->tv_nsec > end->tv_nsec) {
        sec -= 1;
        nanoSec += 1000000000;
    }

    // add the precomputed seconds and nanoseconds
    return (double)sec + (double)(nanoSec / 1000000000);
}

#endif