# $Id: Makefile,v 1.1 2016/01/21 19:42:26 dechavez Exp $
# Unix makefile for qxml

VPATH   = 
INCDIR  = $(VPATH)../../include
BINDIR  = $(VPATH)../../../bin/$(PLATFORM)
LIBDIR  = $(VPATH)../../../lib/$(PLATFORM)

OPTMIZ  = -g
INCS   += -I$(INCDIR) -I$(OPTDIR)/include -I$(XML2INCS)
DEFS    = -D$(OSNAME) -D$(OSTYPE) -DOSVER=$(OSVER) -D_REENTRANT
CFLAGS  = $(OPTMIZ) $(INCS) $(DEFS) $(SITEFLAGS)

LIBS    = -L$(LIBDIR) -L$(OPTDIR)/lib
LIBS   += -lq330
LIBS   += -lqdpxml
LIBS   += -lqdp
LIBS   += -lmyxml
LIBS   += -lida10
LIBS   += -le300
LIBS   += -lmsgq
LIBS   += -lqio
LIBS   += -ludpio
LIBS   += -lttyio
LIBS   += -lutil
LIBS   += -llogio
LIBS   += -lmd5
LIBS   += -lm
LIBS   += -lz
LIBS   += $(XML2LIBS)
LIBS   += $(MTLIBS)
LIBS   += $(SOCKLIBS)

OBJS  = ReleaseNotes.o
OBJS += main.o

OUTPUT  = qxml
 
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

$(OBJS): $(INCDIR)/qdp.h $(INCDIR)/qdp/lcq.h $(INCDIR)/qdp/status.h
 
$(OUTPUT): $(OBJS) $(LIBDIR)/libqdp.a $(LIBDIR)/libmyxml.a $(LIBDIR)/libqdpxml.a $(LIBDIR)/libutil.a $(LIBDIR)/libq330.a $(LIBDIR)/libqio.a $(LIBDIR)/libudpio.a
	$(CC) $(CFLAGS) -o $@ $(OBJS) $(LIBS)
