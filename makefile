CC=gcc
CFLAGS=-g -Wall -Iheader -lm -lpthread
HDIR=$(header/$(*.h))
SDIR=$(sources/$(*.c))
HEADERS=header.h crc.h reading.h counting.h measuring.h counting.h
OBJ:=$(patsubst %.c,%.o,$(SDIR))
ALL=main.o counting.o crc.o reading.o init.o measuring.o


#thesis: main.o counting.o crc.o init.o reading.o measuring.o
#	gcc -o thesis main.o counting.o crc.o init.o reading.o measuring.o $(CFLAGS)

#main.o: main.c
#	gcc -c main.c $(CFLAGS) 

#Init.o: Init.c $(HDIR)/header.h
#	gcc -c $(SDIR)/Init.c $(CFLAGS) 

#counting.o: counting.c $(HDIR)/counting.h
#	gcc -c $(SDIR)/counting.c $(CFLAGS) 

#measuring.o: measuring.c $(HDIR)/measuring.h
#	gcc -c $(SDIR)/measuring.c $(CFLAGS) 

#reading.o: reading.c $(HDIR)/reading.h
#	gcc -  $(SDIR)/reading.c $(CFLAGS) 

#crc.o: crc.c $(HDIR)/crc.h
#	gcc -c $(SDIR)/crc.c $(CFLAGS) 

thesis: $(OBJ)
	$(CC) -o $@ $(OBJ) $(CFLAGS)

%.o: $(SDIR) $(HDIR)
	$(CC) -c -o $@ $<  $(CFLAGS)

clean:
	rm *.o 


#$*	Teljes forrasfile neve kiterjesztes nelkul
#$<	out-of-date forrasfile teljes neve(kiterjesztessel)
#$.	forrasfile teljes neve path nelkul
#$&.	forrasfile neve path nelku(kiterjesztes nelkul)
#$:	csak a path
#$@	Teljes aktualis target neve