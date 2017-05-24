CC=gcc
CFLAGS=-g -Wall -I. -lm
DEPS=header.h crc.h reading.h counting.h
OBJ= main.o RS_485.o counting.o crc.o reading.o
ALL=main.o RS_485.o counting.o crc.o reading.o main



hello: hello.o helloworld.o
	$(CC) -o hello $(OBJ) $(CFLAGS)
#%.o: %.c $(DEPS)
#	$(CC) -c -o $@ $< $(CFLAGS)
%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)


clean:
	rm *.o 