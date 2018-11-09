all:
	sched
	printchars

clean:
	rm printchars
	rm sched

sched: sched.c scheduler.c sched.h
	gcc sched.c scheduler.c -o sched -std=c99

printchars: printchars.c
	gcc printchars.c -o printchars -std=c99