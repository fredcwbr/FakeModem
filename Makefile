VERBOSE = TRUE

TARGET = FakeModem 

VPATH=./src 
IDIR =-I../include -I./uthash/src/ -I./jsmn
ODIR=./obj
LDIR =../lib

CC=gcc
CFLAGS=$(IDIR)

LIBS=-lm

DEPS = uthash jsmn 

OBJS = $(ODIR)/atTreatCmds.o \
	$(ODIR)/configFromJson.o \
	$(ODIR)/global.o \
	$(ODIR)/helper.o \
	$(ODIR)/jsmninc.o \
	$(ODIR)/mdmMM.o \
	$(ODIR)/FakeModem.o \

SOURCES = atTreatCmds.c configFromJson.c FakeModem.c global.c helper.c jsmninc.c mdmMM.c
HEADERS = atTreatCmds.h configFromJson.h enums.h global.h FakeModem.h helper.h jsmninc.h mdmMM.h


FakeModem: $(OBJS)
	$(CC) -g -o $@ $^ $(CFLAGS) $(LIBS) 


jsmn:
	@if [ ! -d $@ ]; then git clone https://github.com/zserge/jsmn.git  $@; fi
	# $(TARGET_REPO) $@; fi
	# git clone https://github.com/zserge/jsmn.git

uthash:
	@if [ ! -d $@ ]; then git clone https://github.com/troydhanson/uthash.git $@; fi
	# $(TARGET_REPO) $@; fi
	# git clone https://github.com/troydhanson/uthash.git


$(ODIR)/%.o: %.c  $(DEPS) $(HEADERS)
	$(info $$OBJ = $(OBJS))
	$(info $$IDIR = $(IDIR))
	$(info $$ODIR = $(ODIR))
	$(info $$DEPS = $(DEPS))
	$(CC) -g -c -o $@ $< $(CFLAGS)



.PHONY: all clean dist-clean

all:	$(TARGET)

clean:
	rm -f $(ODIR)/* *~ core $(INCDIR)/*~  

dist-clean:
	rm -f $(ODIR)/* *~ core $(INCDIR)/*~  
	rm -rf sjsmn uthash
