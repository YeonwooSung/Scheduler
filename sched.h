#ifndef SCHED_H
#define SCHED_H

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <limits.h>
#include <signal.h>
#include <sys/wait.h>
#include <string.h>

typedef struct process_control_list {
    pid_t pid;
    struct process_control_list *prev;
    struct process_control_list *next;
} List;

#endif