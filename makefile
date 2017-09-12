CC=gcc
CFLAGS=-g -Wall -lpthread -lwiringPi -Iheader
SRC:=$(wildcard src/*.c)
OBJ:=$(wildcard *.o)
#OBJ=$(patsubst src/%.c,obj/%.o, $(SRC))
#ALL:=$(wildcard %.o) main.o
ALL= main.o counting.o crc.o measuring.o reading.o Init.o
RESULT=/home/herczig/thesis/thesis/app/thesis

$(RESULT): main.o $(OBJ)
	$(CC) -o $@ $(ALL) $(CFLAGS) 
main.o:main.c
	$(CC) -c main.c $(CFLAGS)
$(OBJ):src/%.c 
	$(CC) -c -o $@ $<  $(CFLAGS)


clean:
	rm *.o src/*.o  $(RESULT)


#$*	Teljes forrasfile neve kiterjesztes nelkul
#$<	out-of-date forrasfile teljes neve(kiterjesztessel)
#$.	forrasfile teljes neve path nelkul
#$&.	forrasfile neve path nelku(kiterjesztes nelkul)
#$:	csak a path
#$@	Teljes aktualis target neve



