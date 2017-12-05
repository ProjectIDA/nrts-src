# $Id: Makefile,v 1.3 2014/08/11 19:08:05 dechavez Exp $
# Unix makefile for wgmcd

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
LIBS   += -lwg
LIBS   += -lisidl
LIBS   += -lnrts
LIBS   += -lisi
LIBS   += -lliss
LIBS   += -lida
LIBS   += -lisidb
LIBS   += -liacp
LIBS   += -llogio
LIBS   += -lida10
LIBS   += -ldbio
LIBS   += -lutil
LIBS   += -ldali
LIBS   += -ldmc
LIBS   += -lmsgq
LIBS   += -lz
LIBS   += -lm
LIBS   += $(MTLIBS)
LIBS   += $(POSIX4LIB)
LIBS   += $(SOCKLIBS)

OBJS  = ReleaseNotes.o
OBJS += main.o
OBJS += client.o
OBJS += echo.o
OBJS += exit.o
OBJS += init.o
OBJS += log.o
OBJS += par.o
OBJS += recv.o
OBJS += send.o
OBJS += service.o
OBJS += signals.o
OBJS += util.o

OUTPUT  = wgmcd
 
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

$(OBJS): wgmcd.h $(INCDIR)/wg.h $(INCDIR)/util.h 
 
$(OUTPUT): $(OBJS) $(LIBDIR)/libisidl.a $(LIBDIR)/libiacp.a $(LIBDIR)/libutil.a $(LIBDIR)/liblogio.a $(LIBDIR)/libisidl.a
	$(CC) $(CFLAGS) -o $@ $(OBJS) $(LIBS)
