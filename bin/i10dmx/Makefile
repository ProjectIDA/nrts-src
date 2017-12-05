# $Id: Makefile,v 1.20 2017/10/20 00:17:36 dauerbach Exp $
# Makefile for i10dmx

VPATH  = 
INCDIR = $(VPATH)../../include
BINDIR = $(VPATH)../../../bin/$(PLATFORM)
LIBDIR = $(VPATH)../../../lib/$(PLATFORM)
NOTESDIR= $(VPATH)../../ReleaseNotes
OPTMIZ = -g
INCS   = -I$(INCDIR)
INCS  += $(SQLINCS)
DEFS   = -D$(OSNAME) -D$(OSTYPE)
DEFS  += $(SQLDEFS)
CFLAGS = $(OPTMIZ) $(INCS) $(DEFS) $(SITEFLAGS)
LIBS   = -L$(LIBDIR)
LIBS  += -lida10
LIBS  += -lisi
LIBS  += -lisidb
LIBS  += -ldbio
LIBS  += -lcssio
LIBS  += -llogio
LIBS  += -lq330
LIBS  += -lqdp
LIBS  += -ldetect
LIBS  += -lqio
LIBS  += -ludpio
LIBS  += -lttyio
LIBS  += -llogio
LIBS  += -lutil
LIBS  += -lmsgq
LIBS  += -lmseed
LIBS  += -ldmc
LIBS  += -lmd5
LIBS  += -lz
LIBS  += -lm
LIBS  += $(SQLLIBS)
LIBS  += $(SOCKLIBS)
LIBS  += $(MTLIBS)

OBJS   = ReleaseNotes.o
OBJS  += main.o
OBJS  += detect.o
OBJS  += exitcode.o
OBJS  += help.o
OBJS  += init.o
OBJS  += msglog.o
OBJS  += names.o
OBJS  += rdrec.o
OBJS  += tsrec_ok.o
OBJS  += ttag.o
OBJS  += unpackii.o
OBJS  += unpacklm.o
OBJS  += wfdisc.o
OBJS  += wrtdat.o
OUTPUT = i10dmx

all: OBJS/$(PLATFORM) $(BINDIR) FORCE
	cd OBJS/$(PLATFORM); \
	$(MAKE) -f ../../Makefile VPATH=../../ $(OUTPUT)

release: FORCE
	vi ReleaseNotes.c; $(MAKE) remove install

install: OBJS/$(PLATFORM) $(BINDIR) FORCE
	cd OBJS/$(PLATFORM); \
	$(MAKE) -f ../../Makefile VPATH=../../ doinstall notes

clean: OBJS/$(PLATFORM) FORCE
	rm -f OBJS/$(PLATFORM)/*.o

remove: OBJS/$(PLATFORM) $(BINDIR) FORCE
	rm -f OBJS/$(PLATFORM)/$(OUTPUT) $(BINDIR)/$(OUTPUT)

doinstall: $(OUTPUT)
	$(INSTALL) -c -m 755 $(OUTPUT) $(BINDIR)

notes: $(VPATH)ReleaseNotes.c
	$(INSTALL) -c -m 664 $(VPATH)ReleaseNotes.c $(NOTESDIR)/$(OUTPUT)

REMOVE: $(BINDIR)
	rm -rf OBJS
	cd $(BINDIR); rm -f $(OUTPUT)

$(BINDIR):
	mkdir -p $@

OBJS/$(PLATFORM):
	mkdir -p $@

FORCE:

$(OUTPUT): $(OBJS) $(LIBDIR)/libida10.a $(LIBDIR)/libutil.a $(LIBDIR)/libcssio.a $(LIBDIR)/libdetect.a
	$(CC) $(CFLAGS) -o $@ $(OBJS) $(LIBS)

$(OBJS): $(INCDIR)/ida10.h $(INCDIR)/detect.h i10dmx.h
