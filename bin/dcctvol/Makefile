# $id$
# Unix makefile for dcctvol

CC = gcc

VPATH =

INCDIR = $(VPATH)../../include
BINDIR = $(VPATH)../../../bin/$(PLATFORM)
LIBDIR = $(VPATH)../../../lib/$(PLATFORM)

OPTMIZ  = -g -w
INCS    = -I$(INCDIR)
INCS   += -I$(IRISHOME)/include
INCS   += -I$(ANTELOPE)/include
INCS   += -I$(CSSHOME)/include
DEFS    = -D$(OSNAME) -D$(OSTYPE)
CFLAGS  = $(OPTMIZ) $(INCS) $(DEFS) $(SITEFLAGS)


LIBS  = -L$(LIBDIR)
LIBS += -lida10
LIBS += -L$(IRISHOME)/lib/sun5
LIBS += -L$(ANTELOPE)/lib
LIBS += -L$(CSSHOME)/lib
LIBS += -liris -lget -ltime -lutil
LIBS += -lds -ltrvltm -lresponse
LIBS += -lcoords -lbanner -lstock -ldeviants
LIBS +=	-lkstat -ldl -lposix4 -lnsl -lsocket -lsunmath
LIBS += -lz -lm

RLIB  = -R$(IRISHOME)/lib/sun5
RLIB += -R$(ANTELOPE)/lib
RLIB += -R$(CSSHOME)/lib

OBJS  = ReleaseNotes.o
OBJS += main.o
OBJS += help.o
OBJS += init.o
OBJS += get_str_datascope.o
OBJS += get_Krecmap.o
OBJS += figure_time.o
OBJS += tape_status.o
OBJS += check_logger_type.o
OBJS += day_split.o
OBJS += rdata.o
OBJS += histo.o
OBJS += mathcomp.o

OUTPUT = dcctvol

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
	$(INSTALL) -c -m 755 $(OUTPUT) $(BINDIR)

OBJS/$(PLATFORM):
	mkdir -p OBJS/$(PLATFORM)

FORCE:

$(OBJS):

$(OUTPUT): $(OBJS) 
	$(CC) $(CFLAGS) -o $@ $(OBJS) $(RLIB) $(LIBS)
