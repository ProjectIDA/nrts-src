# $Id: Makefile,v 1.3 2015/08/24 18:24:50 dechavez Exp $
# Makefile for dccdb library

VPATH   = 
RANLIB  = ranlib
LIBDIR  = $(VPATH)../../../lib/$(PLATFORM)
OPTMIZ  = -g
INCDIR  = $(VPATH)../../include
INCS    = -I$(INCDIR)
INCS   += -I$(ANTELOPE)/include
DEFS    = -D$(OSNAME) -D$(OSTYPE) -D_REENTRANT
CFLAGS  = $(OPTMIZ) $(INCS) $(DEFS) $(SITEFLAGS)
ARCHIVE = libdccdb.a
OUTPUT  = $(LIBDIR)/$(ARCHIVE)
OBJS    = $(OUTPUT)(version.o)
OBJS   += $(OUTPUT)(abbrev.o)
OBJS   += $(OUTPUT)(cascade.o)
OBJS   += $(OUTPUT)(chan.o)
OBJS   += $(OUTPUT)(db.o)
OBJS   += $(OUTPUT)(lookup.o)
OBJS   += $(OUTPUT)(response.o)
OBJS   += $(OUTPUT)(seedloc.o)
OBJS   += $(OUTPUT)(site.o)
OBJS   += $(OUTPUT)(stage.o)
OBJS   += $(OUTPUT)(strings.o)
OBJS   += $(OUTPUT)(units.o)
OBJS   += $(OUTPUT)(util.o)

.PRECIOUS : $(OUTPUT)

all: OBJS/$(PLATFORM) $(LIBDIR) FORCE
	cd OBJS/$(PLATFORM); \
	$(MAKE) -f ../../Makefile VPATH=../../ makestamp

$(OUTPUT): makestamp

$(OBJS): $(INCDIR)/dccdb.h $(INCDIR)/mseed.h

makestamp: $(OBJS)
	$(RANLIB) $(OUTPUT)
	touch makestamp
 
clean: OBJS/$(PLATFORM) FORCE
	rm -f OBJS/$(PLATFORM)/*.o
 
remove: OBJS/$(PLATFORM) FORCE
	rm -f $(OUTPUT) OBJS/$(PLATFORM)/makestamp

REMOVE: OBJS FORCE
	rm -rf OBJS $(OUTPUT)

OBJS:
	mkdir $@

$(LIBDIR):
	mkdir -p $@

OBJS/$(PLATFORM):
	mkdir -p $@
 
FORCE:
