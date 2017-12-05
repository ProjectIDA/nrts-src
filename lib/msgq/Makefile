# $Id: Makefile,v 1.7 2017/10/07 21:22:19 dechavez Exp $
# Makefile for msgq library

VPATH   = 
RANLIB  = ranlib
LIBDIR  = $(VPATH)../../../lib/$(PLATFORM)
OPTMIZ  = -g
INCDIR  = $(VPATH)../../include
INCS    = -I$(INCDIR)
#DEFS    = -D$(OSNAME) -D$(OSTYPE) -D_REENTRANT
DEFS    = -D$(OSNAME) -D$(OSTYPE)
CFLAGS  = $(OPTMIZ) $(INCS) $(DEFS) $(SITEFLAGS)
ARCHIVE = libmsgq.a
OUTPUT  = $(LIBDIR)/$(ARCHIVE)

OBJS    = $(OUTPUT)(version.o)
OBJS   += $(OUTPUT)(msgq.o)

DEBUG_LIBS    = -L$(LIBDIR)
DEBUG_LIBS   += -llogio
DEBUG_LIBS   += -lmsgq
DEBUG_LIBS  += $(MTLIBS)
DEBUG_LIBS  += $(SOCKLIBS)

.PRECIOUS : $(OUTPUT)

all: OBJS/$(PLATFORM) $(LIBDIR) FORCE
	cd OBJS/$(PLATFORM); \
	$(MAKE) -f ../../Makefile VPATH=../../ makestamp

$(OUTPUT): makestamp

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

test: msgq.c
	$(CC) $(CFLAGS) -L$(LIBDIR) -DDEBUG_TEST -o OBJS/$(PLATFORM)/$@ msgq.c $(DEBUG_LIBS)
