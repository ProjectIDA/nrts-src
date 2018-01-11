# $Id: Makefile,v 1.2 2018/01/10 23:53:48 dechavez Exp $
# Unix makefile for i10dld

VPATH   =
INCDIR  = $(VPATH)../../include
BINDIR  = $(VPATH)../../../bin/$(PLATFORM)
LIBDIR  = $(VPATH)../../../lib/$(PLATFORM)

OPTMIZ  = -g
INCS   += -I$(INCDIR) -I$(OPTDIR)/include -I$(XML2INCS)
DEFS    = -D$(OSNAME) -D$(OSTYPE) -DOSVER=$(OSVER) -D_REENTRANT
CFLAGS  = $(OPTMIZ) $(INCS) $(DEFS) $(SITEFLAGS)

LIBS    = -L$(LIBDIR) -L$(OPTDIR)/lib
LIBS   += -lisidl
LIBS   += -lnrts
LIBS   += -lisi
LIBS   += -lliss
LIBS   += -lida
LIBS   += -lisidb
LIBS   += -liacp
LIBS   += -ldbio
LIBS   += -lutil
LIBS   += -lmsgq
LIBS   += -ldali
LIBS   += -lmseed -ldmc
LIBS   += -lida10
LIBS   += -llogio
LIBS   += -lm -lz
LIBS   += $(MTLIBS)
LIBS   += $(SOCKLIBS)

OBJS  = ReleaseNotes.o
OBJS += main.o
OBJS += debug.o
OBJS += dl.o
OBJS += exit.o
OBJS += log.o
OBJS += signals.o

OUTPUT  = i10dld

all: OBJS/$(PLATFORM) FORCE
	cd OBJS/$(PLATFORM); \
	make -f ../../Makefile VPATH=../../ $(OUTPUT)

install: OBJS/$(PLATFORM) FORCE
	cd OBJS/$(PLATFORM); \
	make -f ../../Makefile VPATH=../../ doinstall

clean: OBJS/$(PLATFORM) FORCE
	rm -f OBJS/$(PLATFORM)/*

remove: OBJS/$(PLATFORM) FORCE
	rm -f OBJS/$(PLATFORM)/$(OUTPUT) $(BINDIR)/$(OUTPUT)

REMOVE: FORCE
	rm -rf OBJS $(BINDIR)/$(OUTPUT)

doinstall: $(OUTPUT)
	rm -f $(BINDIR)/$(OUTPUT)
	cp $(OUTPUT) $(BINDIR)/$(OUTPUT)
	chmod 4755 $(BINDIR)/$(OUTPUT)

OBJS/$(PLATFORM):
	mkdir -p OBJS/$(PLATFORM)

FORCE:

$(OBJS):

$(OUTPUT): $(OBJS) i10dld.h $(LIBDIR)/libisidl.a
	$(CC) $(CFLAGS) -o $@ $(OBJS) $(LIBS)
