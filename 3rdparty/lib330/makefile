TARGET	= lib330.a

# need to fix this hardcoded path once soruce/build tree properly set up
VPATH   = 
RANLIB  = ranlib
LIBDIR  = $(VPATH)../3rdparty/lib/$(PLATFORM)
# LIBDIR  = /ida/dev/lib
OUTPUT  = $(LIBDIR)/$(TARGET)


SRCS	= libarchive.c libcvrt.c libnetserv.c libslider.c libverbose.c\
	libclient.c libdetect.c libopaque.c libstats.c q330cvrt.c\
	libcmds.c libfilters.c libpoc.c libstrucs.c q330io.c\
	libcompress.c liblogs.c libsampcfg.c libsupport.c\
	libcont.c libmd5.c libsample.c libtokens.c libdss.c\
	libctrldet.c libmsgs.c libseed.c libtypes.c\

HDRS =	libarchive.h libclient.h libcmds.h libcompress.h libcont.h libctrldet.h\
	libcvrt.h libdetect.h libfilters.h liblogs.h libmd5.h libmsgs.h\
	libnetserv.h libopaque.h libpoc.h libsampcfg.h libsampglob.h libdss.h\
 	libsample.h libseed.h libslider.h libstats.h libstrucs.h libsupport.h\
	libtokens.h libtypes.h libverbose.h pascal.h platform.h q330cvrt.h\
	q330io.h q330types.h



OBJS	= $(SRCS:%.c=%.o)

RANLIB	= ranlib

.PRECIOUS:	$(TARGET)

CC	= clang ### cc
CFLAGS  = -g -D$(OSTYPE) -std=c99
COPT	= -I./ 


$(TARGET)  := CFLAGS += $(COPT)

ALL	=  $(TARGET)

all:	$(ALL)

$(LIBDIR):
	mkdir -p $@

$(TARGET):	$(TARGET)($(OBJS))
	$(RANLIB) $@


clean:
	-rm -f *.o $(ALL)

install: $(LIBDIR)
	cp $(TARGET) $(LIBDIR)
