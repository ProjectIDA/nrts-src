# $Id: Makefile,v 1.2 2018/01/11 23:30:11 dauerbach Exp $
# Makefile for msdmx

VPATH  = 
INCDIR = $(VPATH)../../include
BINDIR = $(VPATH)../../../bin/$(PLATFORM)
LIBDIR = $(VPATH)../../../lib/$(PLATFORM)
OPTMIZ = -g
INCS   = -I$(INCDIR)
DEFS   = -D$(OSNAME) -D$(OSTYPE)
CFLAGS = $(OPTMIZ) $(INCS) $(DEFS) $(SITEFLAGS)

LIBS   = -L$(LIBDIR)
LIBS  += -lcssio
LIBS  += -llogio
LIBS  += -lmseed
LIBS  += -ldmc
LIBS  += -llogio
LIBS  += -lutil
LIBS  += -lz
LIBS  += -lm
LIBS  += $(SOCKLIBS)
LIBS  += $(MTLIBS)

OBJS   = ReleaseNotes.o
OBJS  += main.o
OBJS  += exit.o
OBJS  += files.o
OBJS  += handle.o
OBJS  += log.o
OBJS  += process.o
OBJS  += wfdisc.o
OBJS  += write.o

OUTPUT = msdmx

all: OBJS/$(PLATFORM) $(BINDIR) FORCE
	cd OBJS/$(PLATFORM); \
	$(MAKE) -f ../../Makefile VPATH=../../ $(OUTPUT)

release: FORCE
	vi ReleaseNotes.c; $(MAKE) remove install

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

$(OUTPUT): $(OBJS) $(LIBDIR)/libmseed.a $(LIBDIR)/libutil.a $(LIBDIR)/libcssio.a
	$(CC) $(CFLAGS) -o $@ $(OBJS) $(LIBS)

$(OBJS): $(INCDIR)/mseed.h msdmx.h
