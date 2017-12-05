# $Id: Makefile,v 1.1 2016/04/28 23:01:49 dechavez Exp $
# Makefile for spm10

VPATH   = 
INCDIR  = $(VPATH)../../include
BINDIR  = $(VPATH)../../../bin/$(PLATFORM)
LIBDIR  = $(VPATH)../../../lib/$(PLATFORM)

OPTMIZ  = -g
INCS   += -I$(INCDIR)
DEFS    = -D$(OSNAME) -D$(OSTYPE)
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
LIBS += -lmseed
LIBS += -lm
LIBS += -lz
LIBS += $(MTLIBS)
LIBS += $(SOCKLIBS)

OBJS  = ReleaseNotes.o
OBJS += main.o
OBJS += exit.o
OBJS += log.o
OBJS += output.o
OBJS += pkt.o
OBJS += process.o
OBJS += signals.o
OBJS += util.o

OUTPUT  = spm10
 
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

$(OBJS): FORCE
 
$(OUTPUT): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $(OBJS) $(LIBS)
