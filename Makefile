CC=gcc
CFLAGS=-g -Wall  -lpthread  -Iheader
SRC:=src/%.c
SRC_ALL=$(wildcard src/*.c)
OBJ:=obj/%.o
OBJDIR=obj/
ALL_OBJ:=$(patsubst src/%.c,obj/%.o,$(SRC_ALL) main.c)
RESULT=app/thesis

$(RESULT):$(ALL_OBJ)
#	$(CC) -o $@ $(ALL_OBJ) $(CFLAGS)
	$(CC) -o $@ $^$(CFLAGS)
$(OBJDIR)main.o:main.c
	$(CC) -c -o $@ $< $(CFLAGS)

$(OBJ):$(SRC)
	$(CC) -c -o $@ $< $(CFLAGS)

.PHONY: clean

clean:
	rm  obj/*.o  $(RESULT)

install:	
	cp ~/thesis/app/thesis /usr/bin/

help:
	@echo you can use two possibilities 
	@echo Firt -make clean- to clean the object files and the app from app dir
	@echo Second -make- to compile and build from the sources to the binary into app dir

#$*	Teljes forrasfile neve kiterjesztes nelkul
#$<	out-of-date forrasfile teljes neve(kiterjesztessel)
#$.	forrasfile teljes neve path nelkul
#$&.	forrasfile neve path nelku(kiterjesztes nelkul)
#$:	csak a path
#$@	Teljes aktualis target neve



