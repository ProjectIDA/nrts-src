#pragma ident "$Id: version.c,v 1.5 2016/07/20 23:15:06 dechavez Exp $"
/*======================================================================
 * 
 * library version management
 *
 *====================================================================*/
#include "tristar.h"

static VERSION version = {2, 0, 1};

/* tristar library release notes

2.0.1   07/20/2016
        soh.c: replaced the 'Device =' strings in the various tristarPrintSOH:PrintXXXX() functions
               with device specific 'Charge controller =' or 'Load controller =' as appropriate

2.0.0   06/06/2016 (TS-MPPT support)
        tristar.h - complete redesign, TS-MPPT support added
        read.c - completely rewritten, now with just tristarReadItem()
        private.h - initial release
        cmd.c - initial release
        connect.c - initial release
        handle.c - initial release
        log.c  - initial release
        soh.c - initial release
        string.c - initial release
        
1.1.1   12/05/2015
        print.c: removed unused argument from a printf()

1.1.0   05/02/2012
        tristar.h: added valid field to TRISTAR_REG and serialno
        print.c: tristarPrintRam() prints "n/a" if register reading is not valid
        read.c: set valid field in tristarReadRegister(), changed tristarReadRam()
            to void now that valid field is used, added tristarReadRamLogset()

1.0.0   04/25/2012
        misc.c: initial release
        print.c: initial release
        read.c: initial release
  
 */

char *tristarVersionString()
{
static char string[] = "tristar library version 100.100.100 and slop";

    snprintf(string, strlen(string), "tristar library version %d.%d.%d",
        version.major, version.minor, version.patch
    );

    return string;
}

VERSION *tristarVersion()
{
    return &version;
}
