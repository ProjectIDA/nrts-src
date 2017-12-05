# $Id: Makefile,v 1.14 2017/10/20 01:09:46 dauerbach Exp $
# Makefile for ida10

VPATH   = 
INCDIR  = $(VPATH)../../include
BINDIR  = $(VPATH)../../../bin/$(PLATFORM)
LIBDIR  = $(VPATH)../../../lib/$(PLATFORM)

OPTMIZ  = -g
INCS   += -I$(INCDIR)
DEFS    = -D$(OSNAME) -D$(OSTYPE)
CFLAGS  = $(OPTMIZ) $(INCS) $(DEFS) $(SITEFLAGS)

LIBS  = -L$(LIBDIR)
LIBS   += -lida10
LIBS   += -lmseed
LIBS   += -ldmc
LIBS   += -llogio
LIBS   += -lutil
LIBS += -lz
LIBS += -lm
LIBS += $(SOCKLIBS)

LIBDEPS  = $(LIBDIR)/libida10.a
LIBDEPS += $(LIBDIR)/libmseed.a
LIBDEPS += $(LIBDIR)/libdmc.a
LIBDEPS += $(LIBDIR)/liblogio.a
LIBDEPS += $(LIBDIR)/libutil.a

OBJS  = ReleaseNotes.o
OBJS += main.o

OUTPUT  = ida10
 
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

$(OBJS): $(INCDIR)/ida10.h $(INCDIR)/util.h

$(OUTPUT): $(OBJS) $(LIBDEPS) 
	$(CC) $(CFLAGS) -o $@ $(OBJS) $(LIBS)
