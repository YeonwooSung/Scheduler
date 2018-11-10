# Scheduler

The aim of this project is to implement a simple user space scheduler, which is implemented with C language.

## Usage

Scheduler uses the Makefile to compile the executable file "sched".
Thus, all you need to do for the compiling this project is just type "make".

The sched accepts a single command line argument, which is the file path of the config file.


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