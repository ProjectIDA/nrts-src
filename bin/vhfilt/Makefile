# $Id: Makefile,v 1.1 2015/10/30 19:45:41 dechavez Exp $
# Makefile for vhfilt

VPATH   = 
INCDIR  = $(VPATH)../../include
BINDIR  = $(VPATH)../../../bin/$(PLATFORM)
LIBDIR  = $(VPATH)../../../lib/$(PLATFORM)

OPTMIZ  = -g
INCS   += -I$(INCDIR)
DEFS    = -D$(OSNAME) -D$(OSTYPE)
CFLAGS  = $(OPTMIZ) $(INCS) $(DEFS) $(SITEFLAGS)

LIBS  = -L$(LIBDIR)
LIBS += -lfilter
LIBS += -llogio
LIBS += -lmseed
LIBS += -lutil
LIBS += -ldmc
LIBS += -lz
LIBS += -lm
LIBS += $(SOCKLIBS)

OBJS  = ReleaseNotes.o
OBJS += main.o
OBJS += exit.o
OBJS += loadf.o
OBJS += log.o
OBJS += output.o
OBJS += process.o
OBJS += read.o
OBJS += reject.o

OUTPUT  = vhfilt
 
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

$(OBJS): vhfilt.h $(INCDIR)/mseed.h $(INCDIR)/util.h 
 
$(OUTPUT): $(OBJS) $(LIBDIR)/libmseed.a $(LIBDIR)/libutil.a $(LIBDIR)/libdmc.a
	$(CC) $(CFLAGS) -o $@ $(OBJS) $(LIBS)
