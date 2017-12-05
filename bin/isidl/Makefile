# $Id: Makefile,v 1.32 2016/02/12 21:55:30 dechavez Exp $
# Unix makefile for isidl

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
LIBS   += -ldbio
LIBS   += -lisi
LIBS   += -lida
LIBS   += -lisidb
LIBS   += -lida10
LIBS   += -lcssio
LIBS   += -lliss
LIBS   += -lparo
LIBS   += -liacp
LIBS   += -lq330
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
LIBS   += -ldmc
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
OBJS += bground.o
OBJS += exit.o
OBJS += help.o
OBJS += init.o
OBJS += intercept.o
OBJS += isi.o
OBJS += liss.o
OBJS += local.o
OBJS += log.o
OBJS += meta.o
OBJS += mseed.o
OBJS += q330.o
OBJS += qdplus.o
OBJS += scl.o
OBJS += signals.o
OBJS += write.o

OUTPUT  = isidl
 
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

$(OBJS): isidl.h $(INCDIR)/isi.h $(INCDIR)/nrts.h $(INCDIR)/util.h $(INCDIR)/ttyio.h $(INCDIR)/mseed.h $(INCDIR)/isi/dl.h
 
$(OUTPUT): $(OBJS) $(LIBDIR)/libisi.a $(LIBDIR)/libnrts.a $(LIBDIR)/libutil.a $(LIBDIR)/libqdp.a $(LIBDIR)/libqdplus.a $(LIBDIR)/libisidl.a $(LIBDIR)/libmseed.a
	$(CC) $(CFLAGS) -o $@ $(OBJS) $(LIBS)
