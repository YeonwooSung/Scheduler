all:
	gcc -o printchars printchars.c
	gcc sched.c scheduler.c -o sched

clean:
	rm printchars
	rm sched