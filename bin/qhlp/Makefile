# $Id: Makefile,v 1.19 2014/08/29 20:38:46 dechavez Exp $
# Unix makefile for qhlp

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
CFLAGS  = $(OPTMIZ) $(INCS) $(DEFS) $(SITEFLAGS)


LIBS    = -L$(LIBDIR)
LIBS   += -lisidl
LIBS   += -lnrts
LIBS   += -lisidb
LIBS   += -ldbio
LIBS   += -lisi
LIBS   += -liacp
LIBS   += -lqdplus
LIBS   += -lqdp
LIBS   += -lliss
LIBS   += -lida
LIBS   += -lisidb
LIBS   += -lq330
LIBS   += -lqdp
LIBS   += -ldetect
LIBS   += -llogio
LIBS   += -lmsgq
LIBS   += -lqio
LIBS   += -ludpio
LIBS   += -lttyio
LIBS   += -lmd5
LIBS   += -lutil
LIBS   += -ldali
LIBS   += -lmseed
LIBS   += -lida10
LIBS   += -ldmc
LIBS   += -lz
LIBS   += -lm
LIBS   += $(MTLIBS)
LIBS   += $(POSIX4LIB)
LIBS   += $(SOCKLIBS)
LIBS   += $(SQLLIBS)

OBJS  = ReleaseNotes.o
OBJS += main.o
OBJS += bground.o
OBJS += debug.o
OBJS += exit.o
OBJS += ida10.o
OBJS += init.o
OBJS += log.o
OBJS += process.o
OBJS += read.o
OBJS += signals.o
OBJS += trigger.o
OBJS += write.o

OUTPUT  = qhlp
 
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

$(OBJS): qhlp.h $(INCDIR)/isi.h $(INCDIR)/nrts.h $(INCDIR)/util.h $(INCDIR)/qdplus.h $(INCDIR)/qdp.h $(INCDIR)/detect.h $(INCDIR)/mseed.h $(INCDIR)/isi/dl.h
 
$(OUTPUT): $(OBJS) $(LIBDIR)/libisi.a $(LIBDIR)/libnrts.a $(LIBDIR)/libutil.a $(LIBDIR)/libqdplus.a $(LIBDIR)/libqdp.a $(LIBDIR)/libdetect.a $(LIBDIR)/libisidl.a $(LIBDIR)/libmseed.a $(LIBDIR)/libida10.a
	$(CC) $(CFLAGS) -o $@ $(OBJS) $(LIBS)
