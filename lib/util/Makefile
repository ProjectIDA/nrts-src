# $Id: Makefile,v 1.31 2015/11/24 23:06:57 dechavez Exp $
# Makefile for util library routines

VPATH   = 
RANLIB  = ranlib
LIBDIR  = $(VPATH)../../../lib/$(PLATFORM)
OPTMIZ  = -g
INCDIR  = $(VPATH)../../include
INCS    = -I$(INCDIR)
DEFS    = -D$(OSNAME) -D$(OSTYPE) -D_REENTRANT
CFLAGS  = $(OPTMIZ) $(INCS) $(DEFS) $(SITEFLAGS)
DFLAGS  = -g $(INCS) $(DEFS)
ARCHIVE = libutil.a
OUTPUT  = $(LIBDIR)/$(ARCHIVE)

OBJS    = $(OUTPUT)(version.o)
OBJS   += $(OUTPUT)(atolk.o)
OBJS   += $(OUTPUT)(BufferedStream.o)
OBJS   += $(OUTPUT)(basename.o)
OBJS   += $(OUTPUT)(bcd.o)
OBJS   += $(OUTPUT)(bground.o)
OBJS   += $(OUTPUT)(binprint.o)
OBJS   += $(OUTPUT)(case.o)
OBJS   += $(OUTPUT)(cat.o)
OBJS   += $(OUTPUT)(cfgpair.o)
OBJS   += $(OUTPUT)(checksum.o)
OBJS   += $(OUTPUT)(chncmp.o)
OBJS   += $(OUTPUT)(complex.o)
OBJS   += $(OUTPUT)(compress.o)
OBJS   += $(OUTPUT)(connect.o)
OBJS   += $(OUTPUT)(copy.o)
OBJS   += $(OUTPUT)(crc.o)
OBJS   += $(OUTPUT)(dir.o)
OBJS   += $(OUTPUT)(email.o)
OBJS   += $(OUTPUT)(etoh.o)
OBJS   += $(OUTPUT)(getline.o)
OBJS   += $(OUTPUT)(gsecmp.o)
OBJS   += $(OUTPUT)(hexdump.o)
OBJS   += $(OUTPUT)(htoe.o)
OBJS   += $(OUTPUT)(io.o)
#OBJS   += $(OUTPUT)(isfloat.o)
OBJS   += $(OUTPUT)(isint.o)
OBJS   += $(OUTPUT)(kcap.o)
OBJS   += $(OUTPUT)(ldump.o)
OBJS   += $(OUTPUT)(lenprt.o)
OBJS   += $(OUTPUT)(list.o)
OBJS   += $(OUTPUT)(lock.o)
OBJS   += $(OUTPUT)(log.o)
OBJS   += $(OUTPUT)(misc.o)
OBJS   += $(OUTPUT)(mkfile.o)
OBJS   += $(OUTPUT)(mkpath.o)
OBJS   += $(OUTPUT)(mmap.o)
OBJS   += $(OUTPUT)(myip.o)
OBJS   += $(OUTPUT)(mysem.o)
OBJS   += $(OUTPUT)(move.o)
OBJS   += $(OUTPUT)(pack.o)
OBJS   += $(OUTPUT)(parse.o)
OBJS   += $(OUTPUT)(peer.o)
OBJS   += $(OUTPUT)(pow.o)
OBJS   += $(OUTPUT)(query.o)
OBJS   += $(OUTPUT)(rwtimeo.o)
OBJS   += $(OUTPUT)(setid.o)
OBJS   += $(OUTPUT)(signal.o)
OBJS   += $(OUTPUT)(sockinfo.o)
OBJS   += $(OUTPUT)(steim.o)
OBJS   += $(OUTPUT)(string.o)
OBJS   += $(OUTPUT)(swap.o)
OBJS   += $(OUTPUT)(syserr.o)
OBJS   += $(OUTPUT)(targ.o)
OBJS   += $(OUTPUT)(timefunc.o)
OBJS   += $(OUTPUT)(timer.o)

.PRECIOUS : $(OUTPUT)

all: OBJS/$(PLATFORM) $(LIBDIR) FORCE
	cd OBJS/$(PLATFORM); \
	$(MAKE) -f ../../Makefile VPATH=../../ makestamp

$(OUTPUT): makestamp

$(OBJS): $(INCDIR)/util.h

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
