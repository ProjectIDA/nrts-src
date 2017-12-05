# $Id: Makefile,v 1.13 2016/11/16 22:01:28 dechavez Exp $
# Makefile for CSS utilities

#INSTALL = install
SRCDIR  = ../..
INCDIR  = ../../../../include
BINDIR  = ../../../bin/$(PLATFORM)
LIBDIR  = ../../../../../lib/$(PLATFORM)

OPTMIZ  = -g
INCS    = -I$(INCDIR)
CFLAGS  = $(OPTMIZ) $(INCS) -D$(OSNAME) -D$(OSTYPE) -L$(LIBDIR) $(SITEFLAGS)

OUTPUT  = css2gse
OUTPUT += css2sac
OUTPUT += wdbytes
OUTPUT += wdc
OUTPUT += wdcat
OUTPUT += wdchk
OUTPUT += wdconvert
OUTPUT += wdcut
OUTPUT += wddel
OUTPUT += wdecho
OUTPUT += wdedit
OUTPUT += wdlist
OUTPUT += wdprint
OUTPUT += wdread
OUTPUT += wdscan
OUTPUT += wdsort
OUTPUT += wdspan
OUTPUT += wdtime
OUTPUT += wdtst
OUTPUT += wdjdays
OUTPUT += wdsampchk

all: OBJS/$(PLATFORM) $(BINDIR) FORCE
	cd OBJS/$(PLATFORM); \
	$(MAKE) -f ../../Makefile BINDIR=../../../../../bin/$(PLATFORM) $(OUTPUT)

install: OBJS/$(PLATFORM) $(BINDIR) FORCE
	cd OBJS/$(PLATFORM); \
	$(MAKE) -f ../../Makefile BINDIR=../../../../../bin/$(PLATFORM) doinstall

clean: OBJS/$(PLATFORM) FORCE
	rm -f OBJS/$(PLATFORM)/*.o

remove: OBJS/$(PLATFORM) $(BINDIR) FORCE
	cd OBJS/$(PLATFORM); rm -f $(OUTPUT)
	cd $(BINDIR); rm -f $(OUTPUT)

doinstall: $(OUTPUT)
	$(INSTALL) -c -m 755 $(OUTPUT) $(BINDIR)

REMOVE: $(BINDIR)
	rm -rf OBJS
	cd $(BINDIR); rm -f $(OUTPUT)

$(BINDIR):
	mkdir -p $@

OBJS/$(PLATFORM):
	mkdir -p $@

FORCE:

css2gse: $(SRCDIR)/css2gse.c $(LIBDIR)/libcssio.a \
                   $(LIBDIR)/libida.a $(LIBDIR)/libutil.a
	$(CC) $(CFLAGS) -o $@ $(SRCDIR)/css2gse.c -lcssio -lida -lutil -lm

css2sac: $(SRCDIR)/css2sac.c $(LIBDIR)/libcssio.a $(LIBDIR)/libsacio.a \
                   $(LIBDIR)/libida.a $(LIBDIR)/libutil.a
	$(CC) $(CFLAGS) -o $@ $(SRCDIR)/css2sac.c -lcssio -lsacio -lida -lutil  -lm

wdbytes: $(SRCDIR)/wdbytes.c $(LIBDIR)/libcssio.a
	$(CC) $(CFLAGS) -o $@ $(SRCDIR)/wdbytes.c -lcssio -lutil

wdc: $(SRCDIR)/wdc.c $(LIBDIR)/libcssio.a $(LIBDIR)/libutil.a
	$(CC) $(CFLAGS) -o $@ $(SRCDIR)/wdc.c -lcssio -lutil

wdchk: $(SRCDIR)/wdchk.c $(LIBDIR)/libcssio.a $(LIBDIR)/libutil.a
	$(CC) $(CFLAGS) -o $@ $(SRCDIR)/wdchk.c -lcssio -lutil

wdcut: $(SRCDIR)/wdcut.c $(LIBDIR)/libcssio.a $(LIBDIR)/libutil.a
	$(CC) $(CFLAGS) -o $@ $(SRCDIR)/wdcut.c -lcssio -lutil

wddel: $(SRCDIR)/wddel.c $(LIBDIR)/libcssio.a
	$(CC) $(CFLAGS) -o $@ $(SRCDIR)/wddel.c -lcssio -lutil

wdecho: $(SRCDIR)/wdecho.c $(LIBDIR)/libcssio.a
	$(CC) $(CFLAGS) -o $@ $(SRCDIR)/wdecho.c -lcssio -lutil

wdedit: $(SRCDIR)/wdedit.c $(LIBDIR)/libcssio.a $(LIBDIR)/libutil.a
	$(CC) $(CFLAGS) -o $@ $(SRCDIR)/wdedit.c -lcssio -lutil

wdlist: $(SRCDIR)/wdlist.c $(LIBDIR)/libcssio.a $(LIBDIR)/libutil.a
	$(CC) $(CFLAGS) -o $@ $(SRCDIR)/wdlist.c -lcssio -lutil

wdprint: $(SRCDIR)/wdprint.c $(LIBDIR)/libcssio.a
	$(CC) $(CFLAGS) -o $@ $(SRCDIR)/wdprint.c -lcssio -lutil

wdread: $(SRCDIR)/wdread.c $(LIBDIR)/libcssio.a
	$(CC) $(CFLAGS) -o $@ $(SRCDIR)/wdread.c -lcssio -lutil

wdconvert: $(SRCDIR)/wdconvert.c $(LIBDIR)/libcssio.a $(LIBDIR)/libutil.a
	$(CC) $(CFLAGS) -o $@ $(SRCDIR)/wdconvert.c -lcssio -lutil

wdcat: $(SRCDIR)/wdcat.c $(LIBDIR)/libcssio.a $(LIBDIR)/libutil.a
	$(CC) $(CFLAGS) -o $@ $(SRCDIR)/wdcat.c -lcssio -lutil -llogio -lm

wdscan: $(SRCDIR)/wdscan.c $(LIBDIR)/libcssio.a $(LIBDIR)/libutil.a
	$(CC) $(CFLAGS) -o $@ $(SRCDIR)/wdscan.c -lcssio -lutil

wdsort: $(SRCDIR)/wdsort.c $(LIBDIR)/libcssio.a $(LIBDIR)/libutil.a
	$(CC) $(CFLAGS) -o $@ $(SRCDIR)/wdsort.c -lcssio -lutil

wdspan: $(SRCDIR)/wdspan.c $(LIBDIR)/libcssio.a $(LIBDIR)/libutil.a
	$(CC) $(CFLAGS) -o $@ $(SRCDIR)/wdspan.c -lcssio -lutil

wdtst: $(SRCDIR)/wdtst.c $(LIBDIR)/libcssio.a $(LIBDIR)/libutil.a
	$(CC) $(CFLAGS) -o $@ $(SRCDIR)/wdtst.c -lcssio -lutil

wdtime: $(SRCDIR)/wdtime.c $(LIBDIR)/libcssio.a $(LIBDIR)/libutil.a
	$(CC) $(CFLAGS) -o $@ $(SRCDIR)/wdtime.c -lcssio -lutil

wdjdays: $(SRCDIR)/wdjdays.c $(LIBDIR)/libcssio.a $(LIBDIR)/libutil.a
	$(CC) $(CFLAGS) -o $@ $(SRCDIR)/wdjdays.c -lcssio -lutil

wdsampchk: $(SRCDIR)/wdsampchk.c $(LIBDIR)/libcssio.a $(LIBDIR)/libutil.a
	$(CC) $(CFLAGS) -o $@ $(SRCDIR)/wdsampchk.c -lcssio -lutil
