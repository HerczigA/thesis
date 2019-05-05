CC=gcc
PP=g++
COMPILER = $(CC)
CFLAGS= -lpthread -Wall -Iheader
SRC:=src/%.c
SRC_ALL=$(wildcard src/*.c)
OBJ:=obj/%.o
OBJDIR=obj/
ALL_OBJ:=$(patsubst src/%.c,obj/%.o,$(SRC_ALL) main.c)
RESULT=bin/thesis

debug : CFLAGS += -g
debug : $(RESULT)

$(RESULT):$(ALL_OBJ)
	$(COMPILER) -o $@ $^ $(CFLAGS)

$(OBJDIR)main.o:main.c
	$(COMPILER) -c -o $@ $< $(CFLAGS)

$(OBJ):$(SRC)
	$(COMPILER) -c -o $@ $< $(CFLAGS)

.PHONY: clean

clean:
	rm  obj/*.o  $(RESULT)

install:	
	sudo cp ./bin/thesis /usr/bin/

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



