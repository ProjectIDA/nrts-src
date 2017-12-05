# $Id: Makefile,v 1.33 2017/09/13 23:42:37 dauerbach Exp $
# Unix makefile for q330

VPATH   = 
INCDIR  = $(VPATH)../../include
BINDIR  = $(VPATH)../../../bin/$(PLATFORM)
LIBDIR  = $(VPATH)../../../lib/$(PLATFORM)

OPTMIZ  = -g
INCS   += -I$(INCDIR) -I$(OPTDIR)/include -I$(XML2INCS)
DEFS    = -D$(OSNAME) -D$(OSTYPE) -DOSVER=$(OSVER) -D_REENTRANT
CFLAGS  = $(OPTMIZ) $(INCS) $(DEFS) $(SITEFLAGS)

LIBS    = -L$(LIBDIR) -L$(OPTDIR)/lib
LIBS   += -lq330 -lqdp -lqdpxml -lmyxml -lida10 -le300 -lmsgq -lqio -ludpio -lttyio -lutil -llogio -lmd5 -lm
LIBS   += -lz
LIBS   += -lm
LIBS   += $(XML2LIBS)
LIBS   += $(MTLIBS)
LIBS   += $(SOCKLIBS)

OBJS  = ReleaseNotes.o
OBJS += main.o
OBJS += amass.o
OBJS += calib.o
OBJS += checkout.o
OBJS += cmd.o
OBJS += combo.o
OBJS += config.o
OBJS += dcp.o
OBJS += ep.o
OBJS += exit.o
OBJS += fixvar.o
OBJS += flush.o
OBJS += glob.o
OBJS += gps.o
OBJS += help.o
OBJS += id.o
OBJS += ifconfig.o
OBJS += init.o
OBJS += input.o
OBJS += list.o
OBJS += man.o
OBJS += pulse.o
OBJS += preamp.o
OBJS += reboot.o
OBJS += resync.o
OBJS += save.o
OBJS += sc.o
OBJS += set.o
OBJS += spp.o
OBJS += status.o
OBJS += tokens.o
OBJS += xml.o

OUTPUT  = q330
 
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

$(OBJS): q330cc.h $(INCDIR)/q330.h $(INCDIR)/qdp.h $(INCDIR)/qdp/lcq.h $(INCDIR)/qdp/status.h
 
$(OUTPUT): $(OBJS) $(LIBDIR)/libqdp.a $(LIBDIR)/libmyxml.a $(LIBDIR)/libqdpxml.a $(LIBDIR)/libutil.a $(LIBDIR)/libq330.a $(LIBDIR)/libqio.a $(LIBDIR)/libudpio.a
	$(CC) $(CFLAGS) -o $@ $(OBJS) $(LIBS)
