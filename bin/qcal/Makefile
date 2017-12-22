# $Id: Makefile,v 1.14 2017/10/30 22:27:17 dechavez Exp $
# Unix makefile for qcal

VPATH   = 
INCDIR  = $(VPATH)../../include
BINDIR  = $(VPATH)../../../bin/$(PLATFORM)
LIBDIR  = $(VPATH)../../../lib/$(PLATFORM)

OPTMIZ  = -g
INCS   += -I$(INCDIR)
INCS   += $(SQLINCS)
DEFS    = -D$(OSNAME) -D$(OSTYPE) -DOSVER=$(OSVER) -D_REENTRANT
DEFS   += -D_POSIX_PTHREAD_SEMANTICS
DEFS   += $(SQLDEFS)
DEFS   += -DINCLUDE_Q330
CFLAGS  = $(OPTMIZ) $(INCS) $(DEFS) $(SITEFLAGS)


LIBS    = -L$(LIBDIR)
LIBS   += -lq330
LIBS   += -le300
LIBS   += -lqdplus
LIBS   += -lqdp
LIBS   += -lmseed
LIBS   += -lida10
LIBS   += -lisi
LIBS   += -lliss
LIBS   += -lida
LIBS   += -lisidl
LIBS   += -lisidb
LIBS   += -ldbio
LIBS   += -lqio
LIBS   += -ludpio
LIBS   += -lttyio
LIBS   += -llogio
LIBS   += -lutil
LIBS   += -lmseed -ldmc
LIBS   += -lmsgq
LIBS   += -lmd5
LIBS   += -lz
LIBS   += -lm
LIBS   += $(MTLIBS)
LIBS   += $(POSIX4LIB)
LIBS   += $(SOCKLIBS)
LIBS   += $(SQLLIBS)

OBJS  = ReleaseNotes.o
OBJS += main.o
OBJS += calib.o
OBJS += data.o
OBJS += e300.o
OBJS += exit.o
OBJS += help.o
OBJS += ida.o
OBJS += log.o
OBJS += mseed.o
OBJS += names.o
OBJS += qdp.o
OBJS += signals.o
OBJS += tokens.o

OUTPUT  = qcal
 
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
	chmod 755 $(BINDIR)/$(OUTPUT)
 
OBJS/$(PLATFORM):
	mkdir -p OBJS/$(PLATFORM)

FORCE:

$(OBJS): qcal.h $(INCDIR)/isi.h $(INCDIR)/nrts.h $(INCDIR)/util.h $(INCDIR)/ttyio.h $(INCDIR)/q330.h
 
$(OUTPUT): $(OBJS) $(LIBDIR)/libisi.a $(LIBDIR)/libnrts.a $(LIBDIR)/libutil.a $(LIBDIR)/libqdp.a $(LIBDIR)/libq330.a
	$(CC) $(CFLAGS) -o $@ $(OBJS) $(LIBS)
