IDIR =../include:./uthash/src/:./jsmn
CC=gcc
CFLAGS=-I$(IDIR)

ODIR=./
LDIR =../lib

LIBS=-lm

DEPS = uthash jsmn 

OBJ = jogger.o



jsmn:
	@if [ ! -d $@ ]; then git clone https://github.com/zserge/jsmn.git  $@; fi
	# $(TARGET_REPO) $@; fi
	# git clone https://github.com/zserge/jsmn.git

uthash:
	@if [ ! -d $@ ]; then git clone https://github.com/troydhanson/uthash.git $@; fi
	# $(TARGET_REPO) $@; fi
	# git clone https://github.com/troydhanson/uthash.git


%.o: %.c  $(DEPS)
	$(info $$OBJ = $(OBJ))
	$(info $$IDIR = $(IDIR))
	$(info $$DEPS = $(DEPS))
	$(CC) -g -c -o $@ $< $(CFLAGS)

jogger: $(OBJ) 
	$(CC) -g -o $@ $^ $(CFLAGS) $(LIBS)


joggerdbg:
	gcc -E -I../src jogger.c >/tmp/a.c
	egrep -v '^#' /tmp/a.c > /tmp/b.c
	indent /tmp/b.c
	$(CC) -g -o /tmp/b $(CFLAGS) /tmp/b.c  $(LIBS)


.PHONY: clean

clean:
	rm -f $(ODIR)/*.o *~ core $(INCDIR)/*~  
	rm -rf sjsmn uthash

