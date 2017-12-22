# $Id: Makefile,v 1.35 2017/10/30 22:31:17 dechavez Exp $
# Makefile for misc programs

VPATH   =
INCDIR  = $(VPATH)../../include
BINDIR  = $(VPATH)../../../bin/$(PLATFORM)
LIBDIR  = $(VPATH)../../../lib/$(PLATFORM)
OPTMIZ  = -g
INCS    = -I$(INCDIR)
DEFS    = -D$(OSNAME) -D$(OSTYPE) -D_REENTRANT
CFLAGS  = $(OPTMIZ) $(INCS) -L$(LIBDIR) $(DEFS) $(SITEFLAGS)
OUTPUT += attol
OUTPUT += basefs
OUTPUT += dos2unix
OUTPUT += etoh
OUTPUT += filelock
OUTPUT += filetimes
OUTPUT += gmt
OUTPUT += lcase
OUTPUT += lttoa
OUTPUT += ucase
OUTPUT += unix2dos
#OUTPUT += cd2
#OUTPUT += cd2
#OUTPUT += i4echo
#OUTPUT += idadd
#OUTPUT += idadtc
#OUTPUT += idasync
#OUTPUT += modtime
#OUTPUT += netread
#OUTPUT += pingwait
#OUTPUT += revcode
#OUTPUT += s4echo
#OUTPUT += selrec
#OUTPUT += stability
#OUTPUT += ts
#OUTPUT += validate
#OUTPUT += ydtodmdy

all: OBJS/$(PLATFORM) $(BINDIR) FORCE
	cd OBJS/$(PLATFORM); \
	$(MAKE) -f ../../Makefile VPATH=../../ $(OUTPUT)

install: OBJS/$(PLATFORM) $(BINDIR) FORCE
	cd OBJS/$(PLATFORM); \
	$(MAKE) -f ../../Makefile VPATH=../../ doinstall

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

attol: $(VPATH)attol.c $(LIBDIR)/libutil.a
	$(CC) $(CFLAGS) -o $@ $(VPATH)attol.c -lutil -llogio

basefs: $(VPATH)basefs.c
	$(CC) $(CFLAGS) -o $@ $(VPATH)basefs.c

calib: $(VPATH)calib.c $(LIBDIR)/libutil.a $(LIBDIR)/libida.a
	$(CC) $(CFLAGS) -o $@ $(VPATH)calib.c -lida -ldbio -lutil -llogio $(SQLLIBS)

cd2: $(VPATH)cd2.c $(LIBDIR)/libutil.a
	$(CC) $(CFLAGS) -o $@ $(VPATH)cd2.c -lutil -llogio $(SOCKLIBS)

dos2unix: $(VPATH)dos2unix.c
	$(CC) $(CFLAGS) -o $@ $(VPATH)dos2unix.c

etoh: $(VPATH)etoh.c $(LIBDIR)/libutil.a
	$(CC) $(CFLAGS) -o $@ $(VPATH)etoh.c -lutil -llogio

filelock: $(VPATH)filelock.c
	$(CC) $(CFLAGS) -o $@ $(VPATH)filelock.c -lutil

filetimes: $(VPATH)filetimes.c
	$(CC) $(CFLAGS) -o $@ $(VPATH)filetimes.c -lutil -llogio

gmt: $(VPATH)gmt.c $(LIBDIR)/libutil.a
	$(CC) $(CFLAGS) -o $@ $(VPATH)gmt.c -lutil -llogio

i4echo: $(VPATH)i4echo.c $(LIBDIR)/libutil.a
	$(CC) $(CFLAGS) -o $@ $(VPATH)i4echo.c -lutil

idadd: $(VPATH)idadd.c $(LIBDIR)/libutil.a
	$(CC) $(CFLAGS) -o $@ $(VPATH)idadd.c -lutil -llogio

idadtc: $(VPATH)idadtc.c $(LIBDIR)/libmtio.a $(LIBDIR)/libutil.a
	$(CC) $(CFLAGS) -o $@ $(VPATH)idadtc.c -lmtio -lutil -llogio

idasync: $(VPATH)idasync.c $(LIBDIR)/libutil.a $(LIBDIR)/libida.a
	$(CC) $(CFLAGS) -o $@ $(VPATH)idasync.c -lida -ldbio -lutil -llogio $(SOCKLIBS) $(MTLIBS)

lcase: $(VPATH)lcase.c
	$(CC) $(CFLAGS) -o $@ $(VPATH)lcase.c

locate: $(VPATH)locate.c $(LIBDIR)/libutil.a $(LIBDIR)/libida.a
	$(CC) $(CFLAGS) -o $@ $(VPATH)locate.c -lida -ldbio -lutil -llogio $(SQLLIBS)

lttoa: $(VPATH)lttoa.c $(LIBDIR)/libutil.a
	$(CC) $(CFLAGS) -o $@ $(VPATH)lttoa.c -lutil -llogio

modtime: $(VPATH)modtime.c
	$(CC) $(CFLAGS) -o $@ $(VPATH)modtime.c

netread: $(VPATH)netread.c
	$(CC) $(CFLAGS) -o $@ $(VPATH)netread.c -lutil -llogio $(SOCKLIBS) $(MTLIBS)

pingwait: $(VPATH)pingwait.c
	$(CC) $(CFLAGS) -o $@ $(VPATH)pingwait.c

revcode: $(VPATH)revcode.c $(LIBDIR)/libutil.a $(LIBDIR)/libida.a
	$(CC) $(CFLAGS) -o $@ $(VPATH)revcode.c -lida -lida10 -lisidb -ldbio -lutil -llogio -ldmc $(SQLLIBS) $(SOCKLIBS) $(MTLIBS)

s4echo: $(VPATH)s4echo.c $(LIBDIR)/libutil.a
	$(CC) $(CFLAGS) -o $@ $(VPATH)s4echo.c -lutil

selrec: $(VPATH)selrec.c
	$(CC) $(CFLAGS) -o $@ $(VPATH)selrec.c

stability: $(VPATH)stability.c $(LIBDIR)/libutil.a
	$(CC) $(CFLAGS) -o $@ $(VPATH)stability.c -lutil -llogio

ts: $(VPATH)ts.c
	$(CC) $(CFLAGS) -o $@ $(VPATH)ts.c -lutil -llogio $(SOCKLIBS) $(MTLIBS)

ucase: $(VPATH)ucase.c
	$(CC) $(CFLAGS) -o $@ $(VPATH)ucase.c

unix2dos: $(VPATH)unix2dos.c
	$(CC) $(CFLAGS) -o $@ $(VPATH)unix2dos.c

validate: $(VPATH)validate.c
	$(CC) $(CFLAGS) -o $@ $(VPATH)validate.c

ydtodmdy: $(VPATH)ydtodmdy.c $(LIBDIR)/libutil.a
	$(CC) $(CFLAGS) -o $@ $(VPATH)ydtodmdy.c -lutil -llogio
