# $Id: Makefile,v 1.1 2017/09/28 18:20:44 dauerbach Exp $
# Makefile for siomet

VPATH  =
INCDIR = $(VPATH)../../include
BINDIR = $(VPATH)../../../bin/$(PLATFORM)
LIBDIR = $(VPATH)../../../lib/$(PLATFORM)
NOTESDIR= $(VPATH)../../ReleaseNotes
OPTMIZ = -g
INCS   = -I$(INCDIR)
DEFS   = -D$(OSNAME) -D$(OSTYPE)
CFLAGS = $(OPTMIZ) $(INCS) $(DEFS) $(SITEFLAGS)
LIBS    = -L$(LIBDIR)
LIBS   += -lisi
LIBS   += -lida
LIBS   += -lisidb
LIBS   += -ldbio
LIBS   += -lliss
LIBS   += -liacp
LIBS   += -lttyio
LIBS   += -lutil
LIBS   += -lida10
LIBS   += -lmseed
LIBS   += -llogio
LIBS   += -lmsgq
LIBS   += -lz
LIBS   += -lm
LIBS  += $(MTLIBS)
LIBS  += $(SOCKLIBS)

OBJS   = ReleaseNotes.o
OBJS  += main.o
OBJS  += crc.o
OBJS  += exit.o
OBJS  += ida10.o
OBJS  += info.o
OBJS  += log.o
OBJS  += packet.o
OBJS  += process.o
OBJS  += read.o
OBJS  += signals.o
OBJS  += timer.o
OUTPUT = siomet

all: OBJS/$(PLATFORM) $(BINDIR) FORCE
	cd OBJS/$(PLATFORM); \
	$(MAKE) -f ../../Makefile VPATH=../../ $(OUTPUT)

install: OBJS/$(PLATFORM) $(BINDIR) FORCE
	cd OBJS/$(PLATFORM); \
	$(MAKE) -f ../../Makefile VPATH=../../ doinstall

clean: OBJS/$(PLATFORM) FORCE
	rm -f OBJS/$(PLATFORM)/*.o

remove: OBJS/$(PLATFORM) $(BINDIR) FORCE
	rm -f OBJS/$(PLATFORM)/$(OUTPUT) $(BINDIR)/$(OUTPUT)

doinstall: $(OUTPUT)
	$(INSTALL) -c -m 755 $(OUTPUT) $(BINDIR)

REMOVE: $(BINDIR)
	rm -rf OBJS
	cd $(BINDIR); rm -f $(OUTPUT)

$(BINDIR):
	mkdir -p $@

OBJS/$(PLATFORM):
	mkdir -p $@

FORCE:

$(OUTPUT): $(OBJS) $(LIBDIR)/libutil.a $(LIBDIR)/libttyio.a $(LIBDIR)/liblogio.a
	$(CC) $(CFLAGS) -o $@ $(OBJS) $(LIBS)

$(OBJS): $(VPATH)/siomet.h
