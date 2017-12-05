# $Id: Makefile,v 1.1 2017/10/20 01:24:12 dauerbach Exp $
# Makefile for mstoida

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
LIBS   += -lida10
LIBS   += -lmseed
LIBS   += -ldmc
LIBS   += -llogio
LIBS   += -lutil
LIBS   += -lm

LIBDEPS  = $(LIBDIR)/libida10.a
LIBDEPS += $(LIBDIR)/libmseed.a
LIBDEPS += $(LIBDIR)/libdmc.a
LIBDEPS += $(LIBDIR)/liblogio.a
LIBDEPS += $(LIBDIR)/libutil.a

OBJS   = ReleaseNotes.o
OBJS  += main.o
OUTPUT = mstoida

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

$(OBJS): $(VPATH)/mstoida.h

$(OUTPUT): $(OBJS) $(LIBDEPS)
	$(CC) $(CFLAGS) -o $@ $(OBJS) $(LIBS)
