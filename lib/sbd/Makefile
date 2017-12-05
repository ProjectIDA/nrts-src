# $Id: Makefile,v 1.2 2013/03/13 21:27:16 dechavez Exp $
# Makefile for sbd library

VPATH   = 
RANLIB  = ranlib
LIBDIR  = $(VPATH)../../../lib/$(PLATFORM)
OPTMIZ  = -g
INCDIR  = $(VPATH)../../include
INCS    = -I$(INCDIR)
DEFS    = -D$(OSNAME) -D$(OSTYPE) -D_REENTRANT
CFLAGS  = $(OPTMIZ) $(INCS) $(DEFS) $(SITEFLAGS)
ARCHIVE = libsbd.a
OUTPUT  = $(LIBDIR)/$(ARCHIVE)

OBJS    = $(OUTPUT)(version.o)
OBJS   += $(OUTPUT)(accept.o)
OBJS   += $(OUTPUT)(close.o)
OBJS   += $(OUTPUT)(connect.o)
OBJS   += $(OUTPUT)(free.o)
OBJS   += $(OUTPUT)(get.o)
OBJS   += $(OUTPUT)(open.o)
OBJS   += $(OUTPUT)(pack.o)
OBJS   += $(OUTPUT)(parse.o)
OBJS   += $(OUTPUT)(print.o)
OBJS   += $(OUTPUT)(read.o)
OBJS   += $(OUTPUT)(recv.o)
OBJS   += $(OUTPUT)(send.o)
OBJS   += $(OUTPUT)(server.o)
OBJS   += $(OUTPUT)(set.o)
OBJS   += $(OUTPUT)(unpack.o)
OBJS   += $(OUTPUT)(util.o)

.PRECIOUS : $(OUTPUT)

all: OBJS/$(PLATFORM) $(LIBDIR) FORCE
	cd OBJS/$(PLATFORM); \
	$(MAKE) -f ../../Makefile VPATH=../../ makestamp

$(OUTPUT): makestamp

$(OBJS): $(INCDIR)/sbd.h

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
