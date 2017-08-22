CC=gcc
CFLAGS=-g -Wall -I. -lm -lpthread
HEADERS=header.h crc.h reading.h counting.h measuring.h counting.h
OBJ= init.o counting.o crc.o reading.o measuring.o 
ALL=main.o counting.o crc.o reading.o init.o measuring.o



thesis: $(ALL)
	$(CC) -o $@ $(OBJ) $(CFLAGS)

%.o: %.c $(HEADERS)
	$(CC) -c -o $@ $<  $(CFLAGS)


clean:
	rm *.o 


#$*	Teljes forrasfile neve kiterjesztes nelkul
#$<	out-of-date forrasfile teljes neve(kiterjesztessel)
#$.	forrasfile teljes neve path nelkul
#$&.	forrasfile neve path nelku(kiterjesztes nelkul)
#$:	csak a path
#$@	Teljes aktualis target neve