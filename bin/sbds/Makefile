# $Id: Makefile,v 1.3 2015/07/17 18:58:29 dechavez Exp $
# Unix makefile for sbds

VPATH   = 
INCDIR  = $(VPATH)../../include
BINDIR  = $(VPATH)../../../bin/$(PLATFORM)
LIBDIR  = $(VPATH)../../../lib/$(PLATFORM)

OPTMIZ  = -g
INCS   += -I$(INCDIR)
DEFS    = -D$(OSNAME) -D$(OSTYPE) -DOSVER=$(OSVER) -D_REENTRANT
DEFS   += -D_POSIX_PTHREAD_SEMANTICS
CFLAGS  = $(OPTMIZ) $(INCS) $(DEFS) $(SITEFLAGS)

LIBS    = -L$(LIBDIR)
LIBS   += -lsbd
LIBS   += -laddoss
LIBS   += -lisidl
LIBS   += -lnrts
LIBS   += -lisi
LIBS   += -lliss
LIBS   += -lida
LIBS   += -lisidb
LIBS   += -liacp
LIBS   += -llogio
LIBS   += -ldbio
LIBS   += -lutil
LIBS   += -ldali
LIBS   += -lmseed
LIBS   += -lida10
LIBS   += -ldmc
LIBS   += -lmsgq
LIBS   += -lz
LIBS   += -lm
LIBS   += $(MTLIBS)
LIBS   += $(POSIX4LIB)
LIBS   += $(SOCKLIBS)

OBJS  = ReleaseNotes.o
OBJS += main.o
OBJS += addoss.o
OBJS += client.o
OBJS += exit.o
OBJS += dlmgr.o
OBJS += init.o
OBJS += log.o
OBJS += par.o
OBJS += service.o
OBJS += signals.o
OBJS += write.o

OUTPUT  = sbds
 
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
	chmod 755 $(BINDIR)/$(OUTPUT)
 
OBJS/$(PLATFORM):
	mkdir -p OBJS/$(PLATFORM)

FORCE:

$(OBJS): sbds.h $(INCDIR)/sbd.h $(INCDIR)/util.h 
 
$(OUTPUT): $(OBJS) $(LIBDIR)/libisidl.a $(LIBDIR)/libiacp.a $(LIBDIR)/libutil.a $(LIBDIR)/liblogio.a $(LIBDIR)/libisidl.a $(LIBDIR)/libaddoss.a $(LIBDIR)/libsbd.a $(LIBDIR)/libida10.a
	$(CC) $(CFLAGS) -o $@ $(OBJS) $(LIBS)
