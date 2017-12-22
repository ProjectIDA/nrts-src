# $Id: Makefile,v 1.8 2017/11/21 18:06:27 dechavez Exp $
# Makefile for mseed library

VPATH   = 
RANLIB  = ranlib
LIBDIR  = $(VPATH)../../../lib/$(PLATFORM)
OPTMIZ  = -g
INCDIR  = $(VPATH)../../include
INCS    = -I$(INCDIR)
DEFS    = -D$(OSNAME) -D$(OSTYPE) -D_REENTRANT
CFLAGS  = $(OPTMIZ) $(INCS) $(DEFS) $(SITEFLAGS)
ARCHIVE = libmseed.a
OUTPUT  = $(LIBDIR)/$(ARCHIVE)
OBJS    = $(OUTPUT)(version.o)
OBJS   += $(OUTPUT)(b30.o)
OBJS   += $(OUTPUT)(b33.o)
OBJS   += $(OUTPUT)(b34.o)
OBJS   += $(OUTPUT)(b50.o)
OBJS   += $(OUTPUT)(b53.o)
OBJS   += $(OUTPUT)(b54.o)
OBJS   += $(OUTPUT)(b52.o)
OBJS   += $(OUTPUT)(b57.o)
OBJS   += $(OUTPUT)(b58.o)
OBJS   += $(OUTPUT)(chnloc.o)
OBJS   += $(OUTPUT)(convert.o)
OBJS   += $(OUTPUT)(copy.o)
OBJS   += $(OUTPUT)(data.o)
OBJS   += $(OUTPUT)(header.o)
OBJS   += $(OUTPUT)(generic.o)
OBJS   += $(OUTPUT)(handle.o)
OBJS   += $(OUTPUT)(history.o)
OBJS   += $(OUTPUT)(ida9.o)
OBJS   += $(OUTPUT)(ida10.o)
OBJS   += $(OUTPUT)(ida1012.o)
OBJS   += $(OUTPUT)(log.o)
OBJS   += $(OUTPUT)(pack.o)
OBJS   += $(OUTPUT)(print.o)
OBJS   += $(OUTPUT)(read.o)
OBJS   += $(OUTPUT)(record.o)
OBJS   += $(OUTPUT)(steim.o)
OBJS   += $(OUTPUT)(time.o)
OBJS   += $(OUTPUT)(unpack.o)
OBJS   += $(OUTPUT)(util.o)

.PRECIOUS : $(OUTPUT)

all: OBJS/$(PLATFORM) $(LIBDIR) FORCE
	cd OBJS/$(PLATFORM); \
	$(MAKE) -f ../../Makefile VPATH=../../ makestamp

$(OUTPUT): makestamp

$(OBJS): $(INCDIR)/mseed.h

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
