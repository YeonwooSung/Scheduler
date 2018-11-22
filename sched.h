#ifndef SCHED_H
#define SCHED_H

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
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

#endif