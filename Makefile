all:
	gcc -o printchars printchars.c
	gcc -o sched sched.c scheduler.c

clean:
	rm printchars
	rm sched
