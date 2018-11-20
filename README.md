# Scheduler

The aim of this project is to implement a simple user space scheduler, which is implemented with C language.

Target platform: Linux

## Usage

Scheduler uses the Makefile to compile the executable file "sched".
Thus, all you need to do for the compiling this project is just type "make".

When you run the scheduler, you need to pass the file path name of the config file, which contains the list of processes that should be scheduled, as a command line argument.

    i.e. "./sched chars.conf"

Moreover, you could choose the scheduling algorithm to use.

    1) round robin scheduling: "./sched -rr chars.conf"

    2) priority based scheduling: "./sched -p chars.conf"

As a default, the sched will use the round robin scheduling algorithm, but, the processes will be ordered by the priority of each process. So, if you use "-rr" option, the process queue will be ordered by the index of the processes (first come first enqueue -> the process that arrives first will be enqueued first), however, if you use the default option, then the process queue will be ordered by the priority of each process.

## Design & Implementation


## Scheduling Algorithms

### First Come First Serve (FCFS)

- Jobs are executed on first come, first serve basis.
- It is a non-preemptive, pre-emptive scheduling algorithm.
- Its implementation is based on FIFO (First In First Out) queue.
- Poor in performance as average wait time is high.

### Shortest Job Next (SJN)

- This is also known as shortest job first (SJF).
- This is a non-preemptive, pre-emptive scheduling algorithm.
- Best approach to minimize waiting time.
- Easy to implement in Batch systems where required CPU time is known in advance.
- Impossible to implement in interactive systems where required CPU time is not known.
- The processer should know in advance how much time process will take.

### Priority Based Scheduling

- Priority scheduling is a non-preemptive algorithm.
- It is one of the most common scheduling algorithms in batch systems.
- Each process is assigned a priority.
- Process with highest priority is to be executed first and so on.
- Processes with same priority are executed on first come first served basis.
- Priority can be decided based on memory requirements, time requirements or any other resource requirement.

### Shortest Remaining Time

- Shortest remaining time (SRT) is the preemptive version of the SJN algorithm.
- The processor is allocated to the job closest to completion.
- The older job can be preempted by a newer ready job with shorter time to completion.
- Impossible to implement in interactive systems where required CPU time is not known.
- It is often used in batch environments where short jobs need to give preference.

### Round Robin Scheduling

- Round Robin is the preemptive process scheduling algorithm.
- Each process is provided a fix time to execute, it is called a quantum.
- Once a process is executed for a given time period, it is preempted and other process executes for a given time period.
- Context switching is used to save states of preempted processes.

### Multiple Level Queue

- Multiple-level queues are not an independent scheduling algorithm. 
- All Queues make use of other existing algorithms to group and schedule jobs with common characteristics.
- Multiple queues are maintained for processes with common characteristics.
- Each queue can have its own scheduling algorithms.
- Priorities are assigned to each queue.