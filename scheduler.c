#include "sched.h"

typedef struct ready_queue { //TODO time measurement
    char terminated;
    PCB *process;
    double spentTime; //to calculate the burst time
    struct ready_queue *next;
} ReadyQueue;

typedef ReadyQueue HighLevelQueue; // the queue that contains the processes with high priority
typedef ReadyQueue LowLevelQueue; // the queue that contains the processes with low priority

typedef ReadyQueue FinishQueue; // the queue that contains the finished processes

double calculateToSecond(struct timespec *begin, struct timespec *end) {
    long sec = end->tv_sec - begin->tv_sec;
    long nanoSec = end->tv_nsec - begin->tv_nsec;

    // check if start nanosecond is greater than end nanosecond
    if (begin->tv_nsec > end->tv_nsec) {
        sec -= 1;
        nanoSec += 1000000000.0f;
    }

    // add the precomputed seconds and nanoseconds
    return (double)sec + (double)(nanoSec / 1000000000.0f);
}

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

        queue->spentTime = 0;

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
        struct timespec begin, end;
        int pid = queue->process->pid;

        printf("\n%s (pid=%d)\n", queue->process->pathName, pid);

        clock_gettime(CLOCK_REALTIME, &begin);
        kill(pid, SIGCONT);

        // wait until the child process is terminated.
        waitpid(pid, &status, 0);
        clock_gettime(CLOCK_REALTIME, &end);

        //calculate the toal spent time
        double spentTime = calculateToSecond(&begin, &end);

        queue->spentTime += spentTime;

        printf("\nProcess %d finished..\n", pid);

        queue = queue->next;
    }
}

/**
 * This function checks if the child process is terminated by using the waitpid.
 *
 * @param (queue) The linked list for ready queue.
 * @param (pid) The process id of the target process.
 */
void checkIfProcessTerminated(ReadyQueue *queue, int pid) {
    int status;
    // just simply check if the process with the given pid is terminated.
    int ret = waitpid(pid, &status, WNOHANG); // use WNOHANG option not to wait.

    if (ret != 0) { //if the given process is terminated, then the waitpid returns the pid.
        queue->terminated = 1;
    }

}

/**
 * This function manages the process control blocks with the round robin scheduling algorithm.
 * Round Robin is the preemptive process scheduling algorithm. Each process is provided
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
                struct timespec begin, end;
                pid = queue->process->pid;
                printf("\nExecute %s (pid=%d)\n", queue->process->pathName, pid);

                clock_gettime(CLOCK_REALTIME, &begin);

                kill(pid, SIGCONT);
                usleep(500000);
                kill(pid, SIGSTOP);

                clock_gettime(CLOCK_REALTIME, &end);

                //calculate the total spent time
                double spentTime = calculateToSecond(&begin, &end);
                queue->spentTime += spentTime;

                checkIfProcessTerminated(queue, pid); //check if the child process is terminated.
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

/**
 * The aim of this method is to manage the processes by using the multiple feedback queue algorithm.
 *
 * @param (queue) The ready queue.
 * @param (avgPriority) The average of the priority of all processes.
 * @return The finish queue that contains all finished processes.
 */
FinishQueue *multiLevelQueueScheduling(ReadyQueue *queue, unsigned avgPriority) {
    FinishQueue *finished = NULL;
    FinishQueue *tempF = NULL;
    HighLevelQueue *high = NULL;
    HighLevelQueue *tempH = NULL;
    LowLevelQueue *low = NULL;
    LowLevelQueue *tempL = NULL;
    int pid;
    char checker;

    //devide the ready queue to high level queue and low level queue
    while (queue) {
        if (queue->process->priority < avgPriority) { //compare the current process's priority with the average priority
            if (high == NULL) {
                tempH = queue;
                high = tempH;
                queue = queue->next;
                high->next = NULL;
            } else {
                high->next = queue;
                high = high->next;
                queue = queue->next;
                high->next =  NULL;
            }
        } else {
            if (low == NULL) {
                tempL = queue;
                low = tempL;
                queue = queue->next;
                low->next = NULL;
            } else {
                low->next = queue;
                low = low->next;
                queue = queue->next;
                low->next = NULL;
            }
        }
    }

    high = tempH; //reset the starting point of the high level queue after finishing the iteration
    low = tempL;  //reset the starting point of the low level queue after finishing the iteration

    unsigned highLevelRunTime = 800000;
    unsigned lowLevelRunTime = 500000;
    unsigned runTime = 100000;

    struct timespec begin, end;

    for (int count = 0; ; count++) {
        HighLevelQueue *beforeH = NULL;
        LowLevelQueue *beforeL = NULL;

        //iterate the high level queue
        while (high) {
            checker = 1; //initialise the value of the local variable checker

            if (high->terminated == 0) {
                if (count > 5) {
                    // for every 5 turn, decrease the priority of the process.
                    high->process->priority -= 1;
                }

                pid = high->process->pid;

                /*
                 * If the priority of the process is less than 2 (i.e. 0 or 1) that means that this process
                 * is extremely important. Thus, it should be scheduled by non-preemptive way.
                 */
                if (high->process->priority < 2) {
                    clock_gettime(CLOCK_REALTIME, &begin);

                    kill(pid, SIGCONT);

                    int status;
                    // wait until the child process is terminated.
                    int ret = waitpid(pid, &status, 0);

                    clock_gettime(CLOCK_REALTIME, &end);

                    //calculate the total spent time
                    double spentTime = calculateToSecond(&begin, &end);
                    high->spentTime += spentTime;

                    printf("\n\tProcess %d finished\n", pid);
                } else {
                    printf("\nExecute %s (pid=%d)\n", high->process->pathName, pid);

                    clock_gettime(CLOCK_REALTIME, &begin);

                    kill(pid, SIGCONT);
                    usleep(highLevelRunTime);
                    kill(pid, SIGSTOP);

                    clock_gettime(CLOCK_REALTIME, &end);

                    //calculate the total spent time
                    double spentTime = calculateToSecond(&begin, &end);
                    high->spentTime += spentTime;

                    checkIfProcessTerminated(high, pid); //check if the child process is terminated.
                }

                beforeH = high;
                high = high->next;
            } else {
                if (high == tempH) {
                    //if the first node of the high level queue is finished, change the starting point of the high level queue
                    tempH = high->next;
                }

                //move the finished process to the finish queue
                if (finished) {
                    finished->next = high;
                    finished = finished->next;
                } else {
                    finished = high;
                    tempF = finished;
                }

                if (beforeH) {
                    beforeH->next = high->next; //link the previous node and next node
                }

                high = high->next;
                finished->next = NULL;
            }
        }

        //iterate the low level queue
        while (low) {
            checker = 1; //initialise the value of the local variable checker

            if (low->terminated == 0) {
                if (count > 5) {
                    // for every 5 turn, increase the priority of the process.
                    low->process->priority -= 1;

                    /*
                     * If the increased priority of this process is greater than average priority
                     * move the current process to the high level queue.
                     */
                    if (low->process->priority < avgPriority) {

                        if (beforeL) {
                            beforeL->next = low->next;
                        }

                        high = low;
                        low = low->next;
                        high->next = NULL;
                        high = high->next;

                        continue;
                    }
                }

                pid = low->process->pid;
                printf("\nExecute %s (pid=%d)\n", low->process->pathName, pid);

                clock_gettime(CLOCK_REALTIME, &begin);

                kill(pid, SIGCONT);
                usleep(lowLevelRunTime);
                kill(pid, SIGSTOP);

                clock_gettime(CLOCK_REALTIME, &end);

                //calculate the total spent time
                double spentTime = calculateToSecond(&begin, &end);
                low->spentTime += spentTime;

                checkIfProcessTerminated(low, pid); //check if the child process is terminated.

                beforeL = low;
                low = low->next;
            } else {
                if (low == tempL) {
                    //if the first node of the low level queue is finished, change the starting point of the low level queue
                    tempL = low->next;
                }

                //move the finished process to the finish queue
                if (finished) {
                    finished->next = low;
                    finished = finished->next;
                } else {
                    finished = low;
                    tempF = finished;
                }

                if (beforeL) {
                    beforeL->next = low->next; //link the previous node and next node
                }

                low = low->next;
                finished->next = NULL;
            }
        }

        high = tempH; //reset the starting point of the high level queue after finishing the iteration
        low = tempL; //reset the starting point of the low level queue after finishing the iteration

        if (count > 5) {
            highLevelRunTime += runTime;
            lowLevelRunTime += runTime;
            count -= 5;
        }

        if (high == NULL && low == NULL) { //if both high level queue and low level queue are empty, break the endless loop
            break;
        }
    }

    finished = tempF;

    return finished;
}

void printOutStatAndSpentTime(ReadyQueue *queue) {
    printf("\n\nStatus of each process:\n");
    while (queue) {
        PCB *p = queue->process;
        double spentTime = queue->spentTime;
        printf("%s (pid=%d) (priority=%d): spent_time(%lf)\n", p->pathName, p->pid, p->priority, spentTime);

        queue = queue->next;
    }
    printf("\n");
}

/**
 * This function calls the suitable scheduling function to schedule the processes.
 *
 * @param (p_list) the linked list of process control blocks
 * @param (mode) 0 -> round robin scheduling (ordered by the priority)
 *               1 -> round robin scheduling
 *               2 -> priority based scheduling
 *               3 -> multiple feedback queue scheduling
 * @param (avgPriority) The average priority of all processes
 */
void scheduleProcesses(PCB *p_list, char mode, unsigned avgPriority) {
    // allocate the memory to make the ready queue recursively
    ReadyQueue *queue = makeQueue(p_list);

    switch(mode) {
        case 0:
        case 1:
            roundRobin(queue);
            break;
        case 2: priorityBasedScheduling(queue);
            break;
        case 3: queue = multiLevelQueueScheduling(queue, avgPriority);
            break;
        default: printf("Invalid mode!");
    }

    //print out the total spent time of all processes
    printOutStatAndSpentTime(queue);

    freeQueue(queue); //free the allocated memory
}
