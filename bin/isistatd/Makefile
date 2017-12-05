# $Id: Makefile,v 1.3 2015/09/17 05:03:20 dauerbach Exp $
# Unix makefile for isistatd

VPATH   = 
INCDIR  = $(VPATH)../../include
BINDIR  = $(VPATH)../../../bin/$(PLATFORM)
LIBDIR  = $(VPATH)../../../lib/$(PLATFORM)
VENLIB  = /usr/local/lib
VENLIB64  = /usr/local/lib64

OPTMIZ  = -g
INCS   += -I$(INCDIR)
DEFS    = -D$(OSNAME) -D$(OSTYPE) -DOSVER=$(OSVER) -D_REENTRANT
DEFS   += -D_POSIX_PTHREAD_SEMANTICS
CFLAGS  = $(OPTMIZ) $(INCS) $(DEFS) $(SITEFLAGS)

LIBS    = -L$(LIBDIR) -L$(VENLIB)
LIBS   += -lisi
LIBS   += -lisidl
LIBS   += -lida
LIBS   += -lliss
LIBS   += -lisidb
LIBS   += -ldbio
LIBS   += -lcssio
LIBS   += -liacp
LIBS   += -llogio
LIBS   += -lida10
LIBS   += -lutil
LIBS   += -lmseed
LIBS   += -lttyio
LIBS   += -lz
LIBS   += -lm
LIBS   += $(MTLIBS)
LIBS   += $(POSIX4LIB)
# LIBS   += $(SQLLIBS)
LIBS   += $(SOCKLIBS)
LIBS   += -lcurl # libcurl for HTTP calls
# LIBS   += -ljansson # jansson json lib


OBJS  = ReleaseNotes.o
OBJS += main.o
OBJS += exit.o
OBJS += log.o
OBJS += signals.o
OBJS += soh2json.o

OUTPUT  = isistatd
 
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

$(OBJS): $(VPATH)/isistatd.h $(INCDIR)/isi.h $(INCDIR)/iacp.h $(INCDIR)/logio.h $(INCDIR)/util.h 
 
$(OUTPUT): $(OBJS) $(LIBDIR)/libisi.a $(LIBDIR)/libiacp.a $(LIBDIR)/liblogio.a $(LIBDIR)/libutil.a
	$(CC) $(CFLAGS) -o $@ $(OBJS) $(LIBS) $(VENLIB)/libjansson.a
