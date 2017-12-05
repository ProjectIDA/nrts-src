# $Id: Makefile,v 1.1 2014/07/10 20:30:41 dechavez Exp $
# Makefile for libdmc

VPATH   = 
RANLIB  = ranlib
LIBDIR  = $(VPATH)../../../lib/$(PLATFORM)
OPTMIZ  = -g
INCDIR  = $(VPATH)../../include
INCS    = -I$(INCDIR)/dmc -I$(INCDIR) -I$(VPATH)
DEFS    = -D$(OSNAME) -D$(OSTYPE) -D_REENTRANT
CFLAGS  = $(OPTMIZ) $(INCS) $(DEFS) $(SITEFLAGS)
ARCHIVE = libdmc.a
OUTPUT  = $(LIBDIR)/$(ARCHIVE)

OBJS    = $(OUTPUT)(fileutils.o)
OBJS   += $(OUTPUT)(genutils.o)
OBJS   += $(OUTPUT)(gswap.o)
OBJS   += $(OUTPUT)(lmplatform.o)
OBJS   += $(OUTPUT)(logging.o)
OBJS   += $(OUTPUT)(lookup.o)
OBJS   += $(OUTPUT)(msrutils.o)
OBJS   += $(OUTPUT)(pack.o)
OBJS   += $(OUTPUT)(packdata.o)
OBJS   += $(OUTPUT)(parseutils.o)
OBJS   += $(OUTPUT)(selection.o)
OBJS   += $(OUTPUT)(tracelist.o)
OBJS   += $(OUTPUT)(traceutils.o)
OBJS   += $(OUTPUT)(unpack.o)
OBJS   += $(OUTPUT)(unpackdata.o)
OBJS   += $(OUTPUT)(template.o)

.PRECIOUS : $(OUTPUT)

all: OBJS/$(PLATFORM) $(LIBDIR) FORCE
	cd OBJS/$(PLATFORM); \
	$(MAKE) -f ../../Makefile VPATH=../../ makestamp

$(OUTPUT): makestamp

$(OBJS): $(INCDIR)/dmc.h $(INCDIR)/dmc/libmseed.h $(INCDIR)/dmc/lmplatform.h

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
