# $Id: Makefile,v 1.12 2014/08/11 19:34:03 dechavez Exp $
# Unix makefile for isildl

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
LIBS   += -lisi
LIBS   += -lliss
LIBS   += -lida
LIBS   += -lisidb
LIBS   += -ldbio
LIBS   += -liacp
LIBS   += -llogio
LIBS   += -lutil
LIBS   += -ldali
LIBS   += -lmseed
LIBS   += -lida10
LIBS   += -ldmc
LIBS   += -lmsgq
LIBS   += -lz
LIBS   += -lm
LIBS   += $(MTLIBS)
LIBS   += $(SOCKLIBS)
LIBS   += $(SQLLIBS)

OBJS  = ReleaseNotes.o
OBJS += main.o

OUTPUT  = isildl
 
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

$(OBJS): $(INCDIR)/isi.h $(INCDIR)/iacp.h $(INCDIR)/util.h
 
$(OUTPUT): $(OBJS) $(LIBDIR)/libisi.a $(LIBDIR)/libisidl.a $(LIBDIR)/libiacp.a $(LIBDIR)/libutil.a $(LIBDIR)/libdbio.a
	$(CC) $(CFLAGS) -o $@ $(OBJS) $(LIBS)
