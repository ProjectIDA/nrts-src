# Unix makefile for isi330

VPATH   = 
SRC3RD  = $(VPATH)../../../src3rd
INCDIR  = $(VPATH)../../include
BINDIR  = $(VPATH)../../../bin/$(PLATFORM)
LIBDIR  = $(VPATH)../../../lib/$(PLATFORM)

OPTMIZ  = -g
INCS   += -I$(INCDIR)
INCS   += -I$(SRC3RD)/lib330
DEFS    = -D$(OSNAME) -D$(OSTYPE) -D_REENTRANT
DEFS   += -D_POSIX_PTHREAD_SEMANTICS
CFLAGS  = $(OPTMIZ) $(INCS) $(DEFS) $(SITEFLAGS) ###-std=c99


LIBS    = -L$(LIBDIR)
#LIBS   += -lida
#LIBS   += -lisidb
LIBS   += -lisi
#LIBS   += -lida10
LIBS   += -liacp
LIBS   += -lq330
LIBS   += -lqdp
LIBS   += -lqio
LIBS   += -ludpio
LIBS   += -lttyio
LIBS   += -llogio
LIBS   += -lmsgq
LIBS   += -lutil
LIBS   += -lmd5
LIBS   += -lz
LIBS   += -lm
LIBS   += $(MTLIBS)
LIBS   += $(POSIX4LIB)
LIBS   += $(SOCKLIBS)
# putting 3rd party libs last
LIBS   += -l330

OBJS  = ReleaseNotes.o
OBJS += bground.o
OBJS += main.o
OBJS += callbacks.o
OBJS += q330.o
OBJS += exit.o
OBJS += init.o
OBJS += log.o
OBJS += signals.o
OBJS += string.o

OUTPUT  = isi330

CC = cc -std=c89 ###clang
 
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
 
$(OUTPUT): $(OBJS) $(LIBDIR)/liblogio.a $(LIBDIR)/libutil.a $(LIBDIR)/lib330.a
	$(CC) $(CFLAGS) -o $@ $(OBJS) $(LIBS)
