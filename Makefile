IDIR =../include:./uthash/src/:./jsmn
CC=gcc
CFLAGS=-I$(IDIR)

ODIR=./
LDIR =../lib

LIBS=-lm

DEPS = uthash.h jsmn.h 

OBJ = jogger.o

%.o: %.c $(DEPS)
	$(info $$OBJ = $(OBJ))
	$(info $$IDIR = $(IDIR))
	$(info $$DEPS = $(DEPS))
	$(CC) -g -c -o $@ $< $(CFLAGS)

jogger: $(OBJ)
	$(info $$OBJ = $(OBJ))
	$(info $$IDIR = $(IDIR))
	$(info $$DEPS = $(DEPS))
	$(CC) -g -o $@ $^ $(CFLAGS) $(LIBS)

.PHONY: clean

clean:
	rm -f $(ODIR)/*.o *~ core $(INCDIR)/*~ 

