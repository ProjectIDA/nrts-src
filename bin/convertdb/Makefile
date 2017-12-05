# $Id: Makefile,v 1.1 2015/03/05 01:06:48 dechavez Exp $
# Makefile for convertdb

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
#ANTELOPELIBS += -lbrttpool

LIBS  = -L$(LIBDIR)
LIBS += -ldccdb
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
OBJS += abbrev.o
OBJS += chan.o
OBJS += exit.o
OBJS += log.o
OBJS += lookup.o
OBJS += match.o
OBJS += new.o
OBJS += old.o
OBJS += print.o
OBJS += sanity.o
OBJS += seedloc.o
OBJS += site.o
OBJS += stage.o
OBJS += strings.o
OBJS += units.o

OUTPUT  = convertdb
 
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

$(OBJS): $(VPATH)/convertdb.h
 
$(OUTPUT): $(OBJS) $(LIBDIR)/libmseed.a $(LIBDIR)/libutil.a $(LIBDIR)/libdccdb.a
	$(CC) $(CFLAGS) -o $@ $(OBJS) $(LIBS)
