# Unix makefile for isi330

VPATH   = 
INCDIR  = $(VPATH)../../include
BINDIR  = $(VPATH)../../../bin/$(PLATFORM)
LIBDIR  = $(VPATH)../../../lib/$(PLATFORM)
LIBDIR3RDP = /ida/dev/lib

OPTMIZ  = -g
INCS   += -I$(INCDIR)
DEFS    = -D$(OSNAME) -DSVR4 -DOSVER=2.6 -D_REENTRANT
DEFS   += -D_POSIX_PTHREAD_SEMANTICS
CFLAGS  = $(OPTMIZ) $(INCS) $(DEFS) $(SITEFLAGS)


LIBS    = -L$(LIBDIR)
LIBS   += -L$(LIBDIR3RDP)
LIBS   += -l330
#LIBS   += -lida
#LIBS   += -lisidb
#LIBS   += -lida10
#LIBS   += -liacp
#LIBS   += -lq330
LIBS   += -llogio
LIBS   += -lutil
#LIBS   += -lmd5
#LIBS   += -lz
LIBS   += -lm
LIBS   += $(MTLIBS)
LIBS   += $(POSIX4LIB)
LIBS   += $(SOCKLIBS)

OBJS  = ReleaseNotes.o
OBJS += main.o
OBJS += callbacks.o
OBJS += q330db.o
OBJS += exit.o
OBJS += init.o
OBJS += log.o
OBJS += signals.o

OUTPUT  = isi330
 
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

$(OBJS): isi330.h
 
$(OUTPUT): $(OBJS) 
	$(CC) $(CFLAGS) -o $@ $(OBJS) $(LIBS)
