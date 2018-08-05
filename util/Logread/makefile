CFLAGS= -g -Wall 
CC=gcc

logread: main.c
	$(CC) -o $@ $< $(CFLAGS)

install:
	install -m 770 logread /usr/bin/

clean:
	rm logread /usr/bin/logread