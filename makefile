CC=gcc
CFLAGS=-g -Wall -Iheader -lm -lpthread
HDIR=header
SDIR=sources
HEADERS=header.h crc.h reading.h counting.h measuring.h counting.h
OBJ= init.o counting.o crc.o reading.o measuring.o 
ALL=main.o counting.o crc.o reading.o init.o measuring.o


thesis: main.o counting.o crc.o init.o reading.o measuring.o
	gcc -o thesis main.o counting.o crc.o init.o reading.o measuring.o $(CFLAGS)

main.o: main.c
	gcc -c main.c $(CFLAGS) 

Init.o: Init.c $(HDIR)/header.h
	gcc -c sources/Init.c $(CFLAGS) 

counting.o: counting.c $(HDIR)/counting.h
	gcc -c sources/counting.c $(CFLAGS) 

measuring.o: measuring.c $(HDIR)/measuring.h
	gcc -c sources/measuring.c $(CFLAGS) 

reading.o: reading.c $(HDIR)/reading.h
	gcc -  sources/reading.c $(CFLAGS) 

crc.o: crc.c $(HDIR)/crc.h
	gcc -c sources/crc.c $(CFLAGS) 

#thesis: $(ALL)
#	$(CC) -o $@ $(OBJ) $(CFLAGS)

#%.o: %.c $(HEADERS)
#	$(CC) -c -o $@ $<  $(CFLAGS)

#.PHONY: all
clean:
	rm *.o 


#$*	Teljes forrasfile neve kiterjesztes nelkul
#$<	out-of-date forrasfile teljes neve(kiterjesztessel)
#$.	forrasfile teljes neve path nelkul
#$&.	forrasfile neve path nelku(kiterjesztes nelkul)
#$:	csak a path
#$@	Teljes aktualis target neve