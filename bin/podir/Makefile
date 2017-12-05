# $Id: Makefile,v 1.3 2015/09/04 17:09:45 dechavez Exp $
# Makefile for podir

VPATH   = 
INCDIR  = $(VPATH)../../include
BINDIR  = $(VPATH)../../../bin/$(PLATFORM)
LIBDIR  = $(VPATH)../../../lib/$(PLATFORM)

OPTMIZ  = -g
INCS   += -I$(INCDIR)
INCS   += -I$(ANTELOPE)/include
DEFS    = -D$(OSNAME) -D$(OSTYPE)
DEFS   += -D_POSIX_PTHREAD_SEMANTICS
CFLAGS  = $(OPTMIZ) $(INCS) $(DEFS) $(SITEFLAGS)

ANTELOPELIBS  = -L$(ANTELOPE)/lib
ANTELOPELIBS += -lds
ANTELOPELIBS += -ltrvltm
ANTELOPELIBS += -lresponse
ANTELOPELIBS += -lcoords
ANTELOPELIBS += -lalk
ANTELOPELIBS += -lstock
ANTELOPELIBS += -ldeviants
ANTELOPELIBS += -lbrttpool

LIBS  = -L$(LIBDIR)
LIBS += -ldccdb
LIBS += -lfilter
LIBS += -lmseed
LIBS += -llogio
LIBS += -lutil
LIBS += -lm
LIBS += -lz
LIBS += $(ANTELOPELIBS)
LIBS += $(MTLIBS)
LIBS += $(SOCKLIBS)

OBJS  = ReleaseNotes.o
OBJS += main.o
OBJS += b50.o
OBJS += b52.o
OBJS += b53.o
OBJS += b54.o
OBJS += b57.o
OBJS += b58.o
OBJS += cascade.o
OBJS += chan.o
OBJS += exit.o
OBJS += ha.o
OBJS += log.o
OBJS += req.o
OBJS += select.o
OBJS += site.o
OBJS += stage.o
OBJS += util.o

OUTPUT  = podir
 
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

$(OBJS): $(VPATH)/podir.h $(INCDIR)/mseed.h $(INCDIR)/dccdb.h
 
$(OUTPUT): $(OBJS) $(LIBDIR)/libmseed.a $(LIBDIR)/libdccdb.a $(LIBDIR)/libfilter.a
	$(CC) $(CFLAGS) -o $@ $(OBJS) $(LIBS)
