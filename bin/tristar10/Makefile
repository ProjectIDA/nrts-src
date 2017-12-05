# $Id: Makefile,v 1.5 2014/09/04 21:31:56 dechavez Exp $
# Unix makefile for tristar10

VPATH   = 
INCDIR  = $(VPATH)../../include
BINDIR  = $(VPATH)../../../bin/$(PLATFORM)
LIBDIR  = $(VPATH)../../../lib/$(PLATFORM)

OPTMIZ  = -g
INCS   += -I$(INCDIR) -I$(INCDIR)/modbus
DEFS    = -D$(OSNAME) -D$(OSTYPE) -DOSVER=$(OSVER) -D_REENTRANT
DEFS   += -D_POSIX_PTHREAD_SEMANTICS
CFLAGS  = $(OPTMIZ) $(INCS) $(DEFS) $(SITEFLAGS)

LIBS  = -L$(LIBDIR)
LIBS += -lisi
LIBS += -lida10
LIBS += -lida
LIBS += -liacp
LIBS += -lmsgq
LIBS += -lisidb
LIBS += -ldbio
LIBS += -lliss
LIBS += -llogio
LIBS += -lutil
LIBS += -ltristar
LIBS += -lmodbus
LIBS += -lmseed
LIBS += -lm
LIBS += -lz
LIBS += $(MTLIBS)
LIBS += $(POSIX4LIB)
LIBS += $(SOCKLIBS)

OBJS  = ReleaseNotes.o
OBJS += main.o
OBJS += bground.o
OBJS += exit.o
OBJS += log.o
OBJS += moxa.o
OBJS += packet.o
OBJS += signals.o

OUTPUT  = tristar10
 
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

$(OBJS): tristar10.h $(INCDIR)/tristar.h
 
$(OUTPUT): $(OBJS) $(LIBDIR)/libtristar.a $(LIBDIR)/libmodbus.a
	$(CC) $(CFLAGS) -o $@ $(OBJS) $(LIBS)
