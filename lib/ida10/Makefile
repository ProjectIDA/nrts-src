# $Id: Makefile,v 1.17 2017/10/19 23:49:58 dauerbach Exp $
# Makefile for ida10 library routines

VPATH   = 
RANLIB  = ranlib
LIBDIR  = $(VPATH)../../../lib/$(PLATFORM)
OPTMIZ  = -g
INCDIR  = $(VPATH)../../include
INCS    = -I$(INCDIR)
DEFS    = -D$(OSNAME) -D$(OSTYPE) -D_REENTRANT
CFLAGS  = $(OPTMIZ) $(INCS) $(DEFS) $(SITEFLAGS)
DFLAGS  = -g $(INCS) $(DEFS)
ARCHIVE = libida10.a
OUTPUT  = $(LIBDIR)/$(ARCHIVE)

OBJS    = $(OUTPUT)(version.o)
OBJS   += $(OUTPUT)(misc.o)
OBJS   += $(OUTPUT)(mseed.o)
OBJS   += $(OUTPUT)(read.o)
OBJS   += $(OUTPUT)(rename.o)
OBJS   += $(OUTPUT)(reorder.o)
OBJS   += $(OUTPUT)(sint.o)
OBJS   += $(OUTPUT)(string.o)
OBJS   += $(OUTPUT)(ttag.o)
OBJS   += $(OUTPUT)(unpack.o)
# OBJS   += $(OUTPUT)(pack.o) # UNREFERENCED CODE, BUT A GOOD READ!
#
.PRECIOUS : $(OUTPUT)

all: OBJS/$(PLATFORM) $(LIBDIR) FORCE
	cd OBJS/$(PLATFORM); \
	$(MAKE) -f ../../Makefile VPATH=../../ makestamp

$(OUTPUT): makestamp

$(OBJS): $(INCDIR)/ida10.h

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
