#pragma ident "$Id: version.c,v 1.8 2016/08/26 20:09:33 dechavez Exp $"
/*======================================================================
 * 
 * library version management
 *
 *====================================================================*/
#include "udpio.h"

static VERSION version = {1, 1, 2};

/* udpio library release notes

1.1.2  08/26/2016
       udpio.c: added a (got == 0) branch to RECVFROM() return value check in udpioRecv(),
                include got in logging errors (got < 0)

1.1.1  07/20/2016
       udpio.c: added a missing errno assignment to one of the input sanity checks in udpioRecv()

1.1.0  12/05/2015
       udpio.c: use FD_SETSIZE instead of gettablesize() for select "width" arg (for OS X portability)

1.0.4  04/02/2010
       udpio.c: include port number in bind() failure log message

1.0.3  01/07/2007
       version.c: snprintf() instead of sprintf()

1.0.2  06/26/2006
       udpio.c: removed unreferenced local variables

1.0.1  05/18/2006
       udpio.c: fixed incorrect return value in udpioInit()

1.0.0  05/05/2006
       udpio.c: initial release

 */

char *udpioVersionString()
{
static char string[] = "udpio library version 100.100.100 and slop";

    snprintf(string, strlen(string), "udpio library version %d.%d.%d",
        version.major, version.minor, version.patch
    );

    return string;
}

VERSION *udpioVersion()
{
    return &version;
}
