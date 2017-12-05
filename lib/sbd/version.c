#pragma ident "$Id: version.c,v 1.7 2015/12/04 22:53:12 dechavez Exp $"
/*======================================================================
 * 
 * library version management
 *
 *====================================================================*/
#include "sbd.h"

static VERSION version = {1, 3, 3};

/* sbd library release notes

1.3.3   12/05/2015
        accept.c, connect.c, pack.c, print.c, unpack.c, util.c:
        casts, format fixes and the like, all to calm OS X compiles

1.3.2   09/05/2014
        parse.c: fixed typo spelling "unsupported" incorrectly
        recv.c:  ditto
        send.c:  ditto

1.3.1   01/29/2014
        unpack.c: rework handling of unexpected lengths (to avoid Linux build complaints)

1.3.0   03/15/2013
        include/sbd.h: added SBD_MO_LOCATION_MASK_x defines, SBD_IMEI_LEN, tidied up comments
        pack.c: added sbdPackMO_LOCATION(), sbdPackMO_CONFIRM(), sbdPackMT_PRIORITY()
        print.c: fixed bug with mo.location.cep formatting
        unpack.c: use SBD_MO_LOCATION_MASK_x from sbd.h, changed length in sbdUnpackMO_LOCATION()
        from 20 (what the manual says) to 11 (what it really is)

1.2.0   03/13/2013
        include/sbd.h: introduced SBD_MO_LOCATION, SBD_MO_CONFIRM, SBD_MT_PRIORITY, SBD_MO and SBD_MT
        pack.c:  accomodate separate MO and MT payloads
        parse.c: added support for all previously unsupported IEs 
        print.c: created, introduces sbdPrintMessageIE()
        unpack.c: added sbdUnpackMO_LOCATION(), sbdUnpackMO_CONFIRM(), sbdUnpackMT_PRIORITY()
        util.c: introduced sbdClearMessageIEIs()

1.1.0   03/11/2013
        include/sbd.h: added SBD_MT_CONFIRM support, fixed SBD_MINTIMEO
        pack.c: added SBD_MT_CONFIRM support
        parse.c: added SBD_MT_CONFIRM support
        util.c: added sbdClearMessage()
        unpack.c: added sbdUnpackMT_CONFIRM()

1.0.0   03/07/2013
        initial release
 */

char *sbdVersionString()
{
static char string[] = "sbd library version 100.100.100 and slop";

    snprintf(string, strlen(string), "sbd library version %d.%d.%d",
        version.major, version.minor, version.patch
    );

    return string;
}

VERSION *sbdVersion()
{
    return &version;
}
