# $Id: Makefile,v 1.12 2017/10/30 22:25:19 dechavez Exp $
# Unix makefile for qpp

VPATH   = 
INCDIR  = $(VPATH)../../include
BINDIR  = $(VPATH)../../../bin/$(PLATFORM)
LIBDIR  = $(VPATH)../../../lib/$(PLATFORM)

OPTMIZ  = -g
INCS   += -I$(INCDIR)
DEFS    = -D$(OSNAME) -D$(OSTYPE) -DOSVER=$(OSVER) -D_REENTRANT
DEFS   += -D_POSIX_PTHREAD_SEMANTICS
CFLAGS  = $(OPTMIZ) $(INCS) $(DEFS) $(SITEFLAGS)

LIBS    = -L$(LIBDIR)
LIBS   += -lqdplus
LIBS   += -lqdp
LIBS   += -lisi
LIBS   += -lliss
LIBS   += -lida
LIBS   += -lida10
LIBS   += -lq330
LIBS   += -ldetect
LIBS   += -lisidb
LIBS   += -ldbio
LIBS   += -lmsgq
LIBS   += -lqio
LIBS   += -ludpio
LIBS   += -lttyio
LIBS   += -lutil
LIBS   += -lmseed -ldmc
LIBS   += -llogio
LIBS   += -lmd5
LIBS   += -lz
LIBS   += -lm
LIBS   += $(MTLIBS)
LIBS   += $(POSIX4LIB)
LIBS   += $(SOCKLIBS)
LIBS   += $(SQLLIBS)

OBJS  = ReleaseNotes.o
OBJS += main.o
OBJS += log.o
OBJS += tee.o

OUTPUT  = qpp
 
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

$(OBJS): qpp.h $(INCDIR)/qdplus.h $(INCDIR)/qdp.h $(INCDIR)/util.h
 
$(OUTPUT): $(OBJS) $(LIBDIR)/libqdplus.a $(LIBDIR)/libqdp.a $(LIBDIR)/libutil.a
	$(CC) $(CFLAGS) -o $@ $(OBJS) $(LIBS)
