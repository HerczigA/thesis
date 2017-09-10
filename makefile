CC=gcc
CFLAGS=-g -Wall -lpthread -lwiringPi -Iheader
SRC=$(wildcard sources/%.c)
OBJ=$(SRC:.c=.o)
ALL=$(wildcard %.o)
RESULT=/home/herczig/thesis/thesis/app/thesis

$(RESULT): $(ALL)
	$(CC) -o $@ $(ALL) $(CFLAGS) 
%.o: sources/%.c 
	$(CC) -c -o $@ $<  $(CFLAGS)


clean:
	rm *.o 


#$*	Teljes forrasfile neve kiterjesztes nelkul
#$<	out-of-date forrasfile teljes neve(kiterjesztessel)
#$.	forrasfile teljes neve path nelkul
#$&.	forrasfile neve path nelku(kiterjesztes nelkul)
#$:	csak a path
#$@	Teljes aktualis target neve