h28800
s 00005/00004/00058
d D 1.9 06/06/20 16:36:45 jchang 9 8
c add RUNLIBDIRS to eliminate the use of LD_LIBRARY_PATH
e
s 00002/00003/00060
d D 1.8 99/12/14 18:54:39 jchang 8 7
c rm ucb library link
e
s 00021/00009/00042
d D 1.7 94/05/12 14:53:11 jburford 7 6
c Changed to produce SunOS/Solaris-specific targets automatically
e
s 00001/00000/00050
d D 1.6 94/03/10 08:59:53 dap 6 5
c added instalation of man page during general install
e
s 00000/00000/00050
d D 1.5 93/12/13 15:24:49 iris 5 4
c none
e
s 00001/00001/00049
d D 1.4 93/11/10 14:24:10 iris 4 3
c linked in bsdmalloc
e
s 00002/00005/00048
d D 1.3 93/10/19 15:20:45 iris 3 2
c cleaned up
e
s 00009/00005/00044
d D 1.2 93/10/15 15:41:58 iris 2 1
c changed for solaris(nupu)
e
s 00049/00000/00000
d D 1.1 92/08/05 18:14:55 reynolds 1 0
c date and time created 92/08/05 18:14:55 by reynolds
e
u
U
f e 0
t
T
I 1
# %W%  %G%
##########################################################
# Makefile: migcmp                          AJR 05/21/92 #
##########################################################
PROGRAM		= migcmp

I 7
# Establish version of Sun OS

ARCH:           sh = uname -r | awk -F. '{printf "sun%d",$1}
UPARCH:         sh = uname -r | awk -F. '{printf "SUN%d",$1}

E 7
# Directory Paths needed for Makefile

D 7
DESTDIR		= ../../../bin
E 7
I 7
D 9
DESTDIR		= ../../../bin/$(ARCH)
E 7
D 2
INCDIR1		= ../../../include
INCDIR2		= /usr/local/css/include
E 2
I 2
D 3
# INCDIR1		= ../../../include
INCDIR1		= /opt/css/include
# INCDIR2		= /usr/local/css/include
INCDIR2		= $(INCDIR1)
E 3
I 3
INCDIR1		= ../../../include
E 9
I 9
DESTDIR		= $(IRISHOME)/bin/$(ARCH)
INCDIR1		= $(IRISHOME)/include
E 9
D 7
INCDIR2		= /opt/css/include
E 3
E 2
LIBDIR1		= ../../../lib
D 2
LIBDIR2		= /usr/local/css/lib
LIBDIRS		= -L$(LIBDIR1) -L$(LIBDIR2)
E 2
I 2
D 3
# LIBDIR2		= /usr/local/css/lib
E 3
LIBDIR2		= /opt/css/lib
E 7
I 7
INCDIR2		= $(CSSHOME)/include
INCDIRSSUN4     = -I$(INCDIR1) -I$(INCDIR2)
INCDIRSSUN5     = -I$(INCDIR1) -I$(INCDIR2)

D 9
LIBDIR1		= ../../../lib/$(ARCH)
E 9
I 9
LIBDIR1		= $(IRISHOME)/lib/$(ARCH)
E 9
LIBDIR2		= $(CSSHOME)/lib
E 7
D 8
LIBDIR3		= /usr/ucblib
E 8
D 7
LIBDIRS		= -L$(LIBDIR1) -L$(LIBDIR2) -L$(LIBDIR3)
E 2
INCDIRS		= -I$(INCDIR1) -I$(INCDIR2)
E 7
I 7
LIBDIRSSUN4	= -L$(LIBDIR1) -L$(LIBDIR2)
D 8
LIBDIRSSUN5	= -L$(LIBDIR1) -L$(LIBDIR2) -L$(LIBDIR3)
E 8
I 8
LIBDIRSSUN5	= -L$(LIBDIR1) -L$(LIBDIR2)
I 9
RUNLIBDIRS	= -R$(LIBDIR1) -R$(LIBDIR2)
E 9
E 8
E 7

# Source Code lists

SRCS		= $(PROGRAM).c
INCS		=
OBJS		= $(PROGRAM).o
LIBS		=

# Command macros

D 7
CFLAGS		= -O -w $(INCDIRS)
LINKLIBS	= $(LIBDIRS) -liris -lutil -lmisc -ltime -lseis \
E 7
I 7
CFLAGS		= -O -w $(INCDIRS$(UPARCH))
LINKLIBSSUN4	= $(LIBDIRSSUN4) -liris -lutil -lmisc -ltime -lseis \
		  -lget 
LINKLIBSSUN5	= $(LIBDIRSSUN5) -liris -lutil -lmisc -ltime -lseis \
E 7
D 2
		  -lcenter2_8 -lget
E 2
I 2
D 4
		  -lget -lucb -lelf
E 4
I 4
D 8
		  -lget -lucb -lelf -lbsdmalloc
E 8
I 8
		  -lget -lelf -lbsdmalloc
E 8
I 7

D 9
COMMAND         = cc $(CFLAGS) -o $(PROGRAM) $(OBJS) $(LINKLIBS$(UPARCH))
E 9
I 9
COMMAND         = cc $(CFLAGS) -o $(PROGRAM) $(OBJS) $(RUNLIBDIRS) $(LINKLIBS$(UPARCH))
E 9
E 7
E 4
E 2
PMODE		= 775
CLEAN		= rm -f $(OBJS) $(PROGRAM) core

# Makefile executables

all:		$(PROGRAM)

$(OBJS):	$(SRCS) $(INCS) $(LIBS)

$(PROGRAM):	$(OBJS)
D 7
		cc $(CFLAGS) -o $(PROGRAM) $(OBJS) $(LINKLIBS)
E 7
I 7
		$(COMMAND)
E 7

install:	all
		install -m $(PMODE) $(PROGRAM) $(DESTDIR) 
I 6
		install -m 664 $(PROGRAM).1 $(IRISHOME)/doc/man/man1
E 6
		$(CLEAN)

sccs:		
		sccs get $(SRCS) $(INCS) Makefile

clean:
		$(CLEAN)
E 1
