CC=gcc
CFLAGS=-g -Wall  -lpthread  -Iheader
SRC:=src/%.c
SRC_ALL=$(wildcard src/*.c)
OBJ:=obj/%.o
OBJDIR=obj/
ALL_OBJ:=$(patsubst src/%.c,obj/%.o,$(SRC_ALL) main.c)
RESULT=thesis_test_app

$(RESULT):$(ALL_OBJ)
	$(CC) -o $@ $(ALL_OBJ) $(CFLAGS)
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



