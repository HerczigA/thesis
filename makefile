CC=gcc
CFLAGS=-g -Wall -I. -lm -lpthread
HEADERS=header.h crc.h reading.h counting.h measuring.h counting.h
OBJ= main.o  counting.o crc.o reading.o
ALL=main.o  counting.o crc.o reading.o 



hello: hello.o helloworld.o
	$(CC) -o hello $(OBJ) $(CFLAGS)
#%.o: %.c $(DEPS)
#	$(CC) -c -o $@ $< $(CFLAGS)
%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)


clean:
	rm *.o 