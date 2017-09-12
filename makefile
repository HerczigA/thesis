CC=gcc
CFLAGS=-g -Wall -lpthread -lwiringPi -Iheader
SRC:=src/%.c
OBJDIR:=$(patsubst src/%.c, obj/%.o, src/*.c,main.c)
OBJ:=obj/%.o
#OBJDIR=obj/
ALL=obj/*.o
#ALL=obj/main.o obj/Init.o obj/reading.o obj/measuring.o obj/crc.o obj/counting.o
RESULT=/home/herczig/thesis/thesis/app/thesis

$(RESULT):$(ALL) 
	$(CC) -o $@ $< $(CFLAGS) 
$(OBJDIR)main.o:main.c 
	$(CC) -c -o $@ $< $(CFLAGS)
$(OBJ):$(SRC)
	$(CC) -c -o $@ $< $(CFLAGS)


clean:
	rm  obj/*.o  $(RESULT)


#$*	Teljes forrasfile neve kiterjesztes nelkul
#$<	out-of-date forrasfile teljes neve(kiterjesztessel)
#$.	forrasfile teljes neve path nelkul
#$&.	forrasfile neve path nelku(kiterjesztes nelkul)
#$:	csak a path
#$@	Teljes aktualis target neve



