# @(#)Makefile    1.9  06/20/06
##########################################################
# Makefile: migcmp                          AJR 05/21/92 #
##########################################################
PROGRAM        = migcmp

# Establish version of Sun OS

ARCH:           sh = uname -r | awk -F. '{printf "sun%d",$1}
UPARCH:         sh = uname -r | awk -F. '{printf "SUN%d",$1}

# Directory Paths needed for Makefile
IRIS_HOME    = /ida/00_PRE_LINUX_TRANSITION_00/iris
CSSHOME        = /opt/antelope/5.7
DESTDIR        = $(IRISHOME)/bin/$(ARCH)
INCDIR1        = $(IRISHOME)/include
INCDIR2        = $(CSSHOME)/include
INCDIRSSUN4     = -I$(INCDIR1) -I$(INCDIR2)
INCDIRSSUN5     = -I$(INCDIR1) -I$(INCDIR2)

LIBDIR1        = $(IRISHOME)/lib/$(ARCH)
LIBDIR2        = $(CSSHOME)/lib
LIBDIRSSUN4    = -L$(LIBDIR1) -L$(LIBDIR2)
LIBDIRSSUN5    = -L$(LIBDIR1) -L$(LIBDIR2)
RUNLIBDIRS    = -R$(LIBDIR1) -R$(LIBDIR2)

# Source Code lists

SRCS        = $(PROGRAM).c
INCS        =
OBJS        = $(PROGRAM).o
LIBS        =

# Command macros

CFLAGS        = -O -w $(INCDIRS$(UPARCH))
LINKLIBSSUN4    = $(LIBDIRSSUN4) -liris -lutil -lmisc -ltime -lseis \
          -lget 
LINKLIBSSUN5    = $(LIBDIRSSUN5) -liris -lutil -lmisc -ltime -lseis \
          -lget -lelf -lbsdmalloc

COMMAND         = cc $(CFLAGS) -o $(PROGRAM) $(OBJS) $(RUNLIBDIRS) $(LINKLIBS$(UPARCH))
PMODE        = 775
CLEAN        = rm -f $(OBJS) $(PROGRAM) core

# Makefile executables

all:        $(PROGRAM)

$(OBJS):    $(SRCS) $(INCS) $(LIBS)

$(PROGRAM):    $(OBJS)
        $(COMMAND)

install:    all
        install -m $(PMODE) $(PROGRAM) $(DESTDIR) 
        install -m 664 $(PROGRAM).1 $(IRISHOME)/doc/man/man1
        $(CLEAN)

sccs:
        sccs get $(SRCS) $(INCS) Makefile

clean:
        $(CLEAN)
