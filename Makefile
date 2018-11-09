all:
	printchars
	sched

clean:
	rm printchars
	rm sched

sched: sched.c scheduler.c sched.h
	gcc sched.c scheduler.c -o sched

printchars: printchars.c
	gcc printchars.c -o printchars