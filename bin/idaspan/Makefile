# $Id: Makefile,v 1.4 2017/10/30 22:35:32 dechavez Exp $
# Makefile for idaspan

VPATH   = 
INCDIR  = $(VPATH)../../include
BINDIR  = $(VPATH)../../../bin/$(PLATFORM)
LIBDIR  = $(VPATH)../../../lib/$(PLATFORM)

OPTMIZ  = -g
INCS   += -I$(INCDIR)
DEFS    = -D$(OSNAME) -D$(OSTYPE)
CFLAGS  = $(OPTMIZ) $(INCS) $(DEFS) $(SITEFLAGS)

LIBS  = -L$(LIBDIR)
LIBS += -lida10
LIBS += -lida
LIBS += -lisidb
LIBS += -ldbio
LIBS += -lutil
LIBS += -llogio
LIBS += -lmseed -ldmc
LIBS += -lz
LIBS += -lm
LIBS += $(SOCKLIBS)

OBJS  = ReleaseNotes.o
OBJS += main.o

OUTPUT  = idaspan
 
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

$(OBJS): $(INCDIR)/ida10.h $(INCDIR)/util.h
 
$(OUTPUT): $(OBJS) $(LIBDIR)/libida10.a $(LIBDIR)/libutil.a $(LIBDIR)/libdmc.a
	$(CC) $(CFLAGS) -o $@ $(OBJS) $(LIBS)
