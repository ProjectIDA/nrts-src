# $Id: Makefile,v 1.4 2014/08/11 20:45:06 dechavez Exp $
# Makefile for imseed

VPATH   = 
INCDIR  = $(VPATH)../../include
BINDIR  = $(VPATH)../../../bin/$(PLATFORM)
LIBDIR  = $(VPATH)../../../lib/$(PLATFORM)

OPTMIZ  = -g
INCS   += -I$(INCDIR)
DEFS    = -D$(OSNAME) -D$(OSTYPE)
DEFS   += -D_POSIX_PTHREAD_SEMANTICS
CFLAGS  = $(OPTMIZ) $(INCS) $(DEFS) $(SITEFLAGS)

LIBS  = -L$(LIBDIR)
LIBS += -lmseed
LIBS += -lida10
LIBS += -lisi
LIBS += -lida
LIBS += -lisidb
LIBS += -ldbio
LIBS += -llogio
LIBS += -ldmc
LIBS += -lliss
LIBS += -lutil
LIBS += -lm
LIBS += -lz
LIBS += $(MTLIBS)
LIBS += $(SOCKLIBS)

OBJS  = ReleaseNotes.o
OBJS += main.o
OBJS += exit.o
OBJS += log.o
OBJS += process.o
OBJS += signals.o
OBJS += write.o

OUTPUT  = imseed
 
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

$(OBJS): $(VPATH)/imseed.h $(INCDIR)/mseed.h
 
$(OUTPUT): $(OBJS) $(LIBDIR)/libida10.a $(LIBDIR)/libida.a $(LIBDIR)/libutil.a $(LIBDIR)/libdmc.a $(LIBDIR)/libmseed.a
	$(CC) $(CFLAGS) -o $@ $(OBJS) $(LIBS)
