CC=gcc
CFLAGS= -lpthread  -Iheader
SRC:=src/%.c
SRC_ALL=$(wildcard src/*.c)
OBJ:=obj/%.o
OBJDIR=obj/
ALL_OBJ:=$(patsubst src/%.c,obj/%.o,$(SRC_ALL) main.c)
RESULT=app/thesis

debug: CFLAGS += -g -Wall

$(RESULT):$(ALL_OBJ)
	$(CC) -o $@ $^ $(CFLAGS) 2>makeAppError.txt

$(OBJDIR)main.o:main.c
	$(CC) -c -o $@ $< $(CFLAGS) 2>makeMainOError.txt

$(OBJ):$(SRC)
	$(CC) -c -o $@ $< $(CFLAGS) 2>makeObjectError.txt

.PHONY: clean

clean:
	rm  obj/*.o  $(RESULT)

install:	
	cp /home/herczig/Prog/C/Thesis/thesis/app/thesis /usr/bin/

help:
	@echo With this makefile you can do the follows:
	@echo -make clean- to clean the object files and the app from app dir
	@echo -make- to compile and build from the sources to the binary into app dir
	@echo -make debug- to compile the binary for debug
	@echo -make install- to copy the binary (if exist) to /usr/bin/ folder( or whatever you set in makefile)
	

#$*	Teljes forrasfile neve kiterjesztes nelkul
#$<	out-of-date forrasfile teljes neve(kiterjesztessel)
#$.	forrasfile teljes neve path nelkul
#$&.	forrasfile neve path nelku(kiterjesztes nelkul)
#$:	csak a path
#$@	Teljes aktualis target neve



