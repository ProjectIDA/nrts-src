# $Id: Makefile,v 1.3 2016/01/21 17:47:25 dechavez Exp $
# Makefile for qdpxml library

VPATH   = 
RANLIB  = ranlib
LIBDIR  = $(VPATH)../../../lib/$(PLATFORM)
OPTMIZ  = -g
INCDIR  = $(VPATH)../../include
INCS    = -I$(INCDIR) -I$(OPTDIR)/include -I$(XML2INCS)
DEFS    = -D$(OSNAME) -D$(OSTYPE) -D_REENTRANT
CFLAGS  = $(OPTMIZ) $(INCS) $(DEFS) $(SITEFLAGS)
ARCHIVE = libqdpxml.a
OUTPUT  = $(LIBDIR)/$(ARCHIVE)
OBJS    = $(OUTPUT)(version.o)
OBJS   += $(OUTPUT)(announce.o)
OBJS   += $(OUTPUT)(automass.o)
OBJS   += $(OUTPUT)(balecfg.o)
OBJS   += $(OUTPUT)(cds.o)
OBJS   += $(OUTPUT)(clock.o)
OBJS   += $(OUTPUT)(config.o)
OBJS   += $(OUTPUT)(data.o)
OBJS   += $(OUTPUT)(detect.o)
OBJS   += $(OUTPUT)(dss.o)
OBJS   += $(OUTPUT)(envproc.o)
OBJS   += $(OUTPUT)(glob.o)
OBJS   += $(OUTPUT)(gps.o)
OBJS   += $(OUTPUT)(iir.o)
OBJS   += $(OUTPUT)(interfaces.o)
OBJS   += $(OUTPUT)(lcq.o)
OBJS   += $(OUTPUT)(murdock.o)
OBJS   += $(OUTPUT)(phy2.o)
OBJS   += $(OUTPUT)(pll.o)
OBJS   += $(OUTPUT)(print.o)
OBJS   += $(OUTPUT)(sensctrl.o)
OBJS   += $(OUTPUT)(slave.o)
OBJS   += $(OUTPUT)(thresh.o)
OBJS   += $(OUTPUT)(tokens.o)
OBJS   += $(OUTPUT)(webpage.o)
OBJS   += $(OUTPUT)(writer.o)

.PRECIOUS : $(OUTPUT)

all: OBJS/$(PLATFORM) $(LIBDIR) FORCE
	cd OBJS/$(PLATFORM); \
	$(MAKE) -f ../../Makefile VPATH=../../ makestamp

$(OUTPUT): makestamp
$(OBJS): $(INCDIR)/qdp.h $(INCDIR)/qdp/xml.h $(INCDIR)/myxml.h $(VPATH)/protos.h

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
