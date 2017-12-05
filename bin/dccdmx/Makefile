# $Id: Makefile,v 1.4 2014/08/29 20:58:15 dechavez Exp $
# Makefile for dccdmx

VPATH  = 
INCDIR = $(VPATH)../../include
BINDIR = $(VPATH)../../../bin/$(PLATFORM)
LIBDIR = $(VPATH)../../../lib/$(PLATFORM)
NOTESDIR= $(VPATH)../../ReleaseNotes
OPTMIZ = -g
INCS   = -I$(INCDIR)
DEFS   = -D$(OSNAME) -D$(OSTYPE)  -D_REENTRANT
DEFS   += -D_POSIX_PTHREAD_SEMANTICS
CFLAGS = $(OPTMIZ) $(INCS) $(DEFS) $(SITEFLAGS)
LIBS   = -L$(LIBDIR)
LIBS  += -lida10
LIBS  += -lisi
LIBS  += -lisidb
LIBS  += -ldbio
LIBS  += -lcssio
LIBS  += -lmseed
LIBS  += -llogio
LIBS  += -lutil
LIBS  += -lm
LIBS  += $(MTLIBS)
LIBS  += -lz

OBJS   = ReleaseNotes.o
OBJS  += main.o
OBJS  += ca.o
OBJS  += cf.o
OBJS  += exit.o
OBJS  += ii.o
OBJS  += lm.o
OBJS  += log.o
OBJS  += signals.o
OBJS  += ts.o
OUTPUT = dccdmx

all: OBJS/$(PLATFORM) $(BINDIR) FORCE
	cd OBJS/$(PLATFORM); \
	$(MAKE) -f ../../Makefile VPATH=../../ $(OUTPUT)

release: FORCE
	vi ReleaseNotes.c; $(MAKE) remove install

install: OBJS/$(PLATFORM) $(BINDIR) FORCE
	cd OBJS/$(PLATFORM); \
	$(MAKE) -f ../../Makefile VPATH=../../ doinstall notes

clean: OBJS/$(PLATFORM) FORCE
	rm -f OBJS/$(PLATFORM)/*.o

remove: OBJS/$(PLATFORM) $(BINDIR) FORCE
	rm -f OBJS/$(PLATFORM)/$(OUTPUT) $(BINDIR)/$(OUTPUT)

doinstall: $(OUTPUT)
	$(INSTALL) -c -m 755 $(OUTPUT) $(BINDIR)

notes: $(VPATH)ReleaseNotes.c
	$(INSTALL) -c -m 664 $(VPATH)ReleaseNotes.c $(NOTESDIR)/$(OUTPUT)

REMOVE: $(BINDIR)
	rm -rf OBJS
	cd $(BINDIR); rm -f $(OUTPUT)

$(BINDIR):
	mkdir -p $@

OBJS/$(PLATFORM):
	mkdir -p $@

FORCE:

$(OUTPUT): $(OBJS) $(LIBDIR)/libida10.a $(LIBDIR)/libutil.a
	$(CC) $(CFLAGS) -o $@ $(OBJS) $(LIBS)

$(OBJS): $(INCDIR)/ida10.h dccdmx.h
