# $Id: Makefile,v 1.16 2017/09/20 19:27:20 dauerbach Exp $
# Unix makefile for qmon

VPATH   = 
INCDIR  = $(VPATH)../../include
BINDIR  = $(VPATH)../../../bin/$(PLATFORM)
LIBDIR  = $(VPATH)../../../lib/$(PLATFORM)

OPTMIZ  = -g
INCS   += -I$(INCDIR) -I$(OPTDIR)/include -I$(XML2INCS)
DEFS    = -D$(OSNAME) -D$(OSTYPE) -DOSVER=$(OSVER) -D_REENTRANT -D_POSIX_PTHREAD_SEMANTICS
CFLAGS  = $(OPTMIZ) $(INCS) $(DEFS) $(SITEFLAGS)

LIBS    = -L$(LIBDIR) -L$(OPTDIR)/lib
LIBS   += -lisidl
LIBS   += -lnrts
LIBS   += -ldbio
LIBS   += -lisi
LIBS   += -lida
LIBS   += -lisidb
LIBS   += -lcssio
LIBS   += -lliss
LIBS   += -lparo
LIBS   += -liacp
LIBS   += -lq330
LIBS   += -le300
LIBS   += -lqdplus
LIBS   += -lqdp
LIBS   += -lseed
LIBS   += -lqio
LIBS   += -ludpio
LIBS   += -lttyio
LIBS   += -llogio
LIBS   += -lutil
LIBS   += -ldali
LIBS   += -lmseed
LIBS   += -lida10
LIBS   += -ldmc
LIBS   += -lmsgq
LIBS   += -lmd5
LIBS   += -lz
LIBS   += -lm
LIBS   += $(CURSELIB)
LIBS   += $(XML2LIBS)
LIBS   += $(MTLIBS)
LIBS   += $(SOCKLIBS)

OBJS  = ReleaseNotes.o
OBJS += main.o
OBJS += boot.o
OBJS += calib.o
OBJS += cmd.o
OBJS += debug.o
OBJS += display.o
OBJS += exit.o
OBJS += flush.o
OBJS += gps.o
OBJS += inbitmap.o
OBJS += input.o
OBJS += isi.o
OBJS += limit.o
OBJS += list.o
OBJS += nrts.o
OBJS += pop.o
OBJS += process.o
OBJS += preamp.o
OBJS += pulse.o
OBJS += q330.o
OBJS += qio.o
OBJS += resync.o
OBJS += save.o
OBJS += sc.o
OBJS += signals.o

OUTPUT  = qmon
 
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
	chmod 4755 $(BINDIR)/$(OUTPUT)
 
OBJS/$(PLATFORM):
	mkdir -p OBJS/$(PLATFORM)

FORCE:

$(OBJS): qmon.h $(INCDIR)/q330.h $(INCDIR)/isi/dl.h Makefile
 
$(OUTPUT): $(OBJS) $(LIBDIR)/libq330.a $(LIBDIR)/libqdp.a $(LIBDIR)/libisidl.a
	$(CC) $(CFLAGS) -o $@ $(OBJS) $(LIBS)
