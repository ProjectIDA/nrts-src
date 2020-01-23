# $Id: Makefile,v 1.2 2016/06/06 21:49:44 dechavez Exp $
# Makefile for tristar library

VPATH   = 
RANLIB  = ranlib
LIBDIR  = $(VPATH)../../../lib/$(PLATFORM)
OPTMIZ  = -g
INCDIR  = $(VPATH)../../include
INCS    = -I$(INCDIR) -I$(INCDIR)/modbus
DEFS    = -D$(OSNAME) -D$(OSTYPE)
CFLAGS  = $(OPTMIZ) $(INCS) $(DEFS) $(SITEFLAGS)
ARCHIVE = libtristar.a
OUTPUT  = $(LIBDIR)/$(ARCHIVE)

OBJS    = $(OUTPUT)(version.o)
OBJS   += $(OUTPUT)(cmd.o)
OBJS   += $(OUTPUT)(connect.o)
OBJS   += $(OUTPUT)(device.o)
OBJS   += $(OUTPUT)(handle.o)
OBJS   += $(OUTPUT)(log.o)
OBJS   += $(OUTPUT)(read.o)
OBJS   += $(OUTPUT)(soh.o)
OBJS   += $(OUTPUT)(string.o)

.PRECIOUS : $(OUTPUT)

all: OBJS/$(PLATFORM) $(LIBDIR) FORCE
	cd OBJS/$(PLATFORM); \
	$(MAKE) -f ../../Makefile VPATH=../../ makestamp

$(OUTPUT): makestamp

$(OBJS): $(INCDIR)/tristar.h $(VPATH)/private.h $(INCDIR)/modbus/modbus.h

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
