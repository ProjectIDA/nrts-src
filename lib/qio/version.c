#pragma ident "$Id: version.c,v 1.10 2016/08/01 22:02:21 dechavez Exp $"
/*======================================================================
 * 
 * library version management
 *
 *====================================================================*/
#include "qio.h"

static VERSION version = {2, 1, 1};

/* qio library release notes

2.1.1  08/02/2016
       send.c, recv.c: make low-level debug code visible only if ENABLE_QIO_DEBUG_MESSAGES defined
       Makefile: define ENABLE_QIO_DEBUG_MESSAGES

2.1.0  06/15/2016
       qio.h: debug.c prototypes and added 'debug' field to QIO handle
       debug.c: initial release
       init.c: debug argument added to qioInit()
       recv.c, send.c: handle based debug message support added

2.0.1  12/05/2015
       send.c, recv.c: casts, format fixes and the like, all to calm OS X compiles

2.0.0  08/11/2014 MAJOR CHANGES TO SUPPORT Q330 DATA COMM OVER SERIAL PORT
       I made these changes in fall of 2013, and resisted committing them
       for nearly year because I wasn't confident I hadn't broken anything
       with the UDP I/O, and because the serial I/O wasn't perfect (I was
       able to get great results with one digitizer but not another and
       had no idea why).  This was all done to support the ADDOSS project
       but I then got sidetracked with the transition to the Linux VM after
       Judy was laid off and still have not gotten around to working on that
       anymore.  In the meantime I have deployed a lot of code built using
       the library in its current state so I have decided to freeze things
       as they are and use this as my baseline for when I finally get around
       to revisiting ADDOSS.

1.0.1  02/03/2011
       close.c: close UDP sockets

1.0.0  01/31/2011
       initial release (tty and network I/O tested)

 */

char *qioVersionString()
{
static char string[] = "qio library version 100.100.100 and slop";

    snprintf(string, strlen(string), "qio library version %d.%d.%d",
        version.major, version.minor, version.patch
    );

    return string;
}

VERSION *qioVersion()
{
    return &version;
}
