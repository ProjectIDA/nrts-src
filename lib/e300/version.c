#pragma ident "$Id: version.c,v 1.2 2015/11/04 23:11:54 dechavez Exp $"
/*======================================================================
 * 
 * library version management
 *
 *====================================================================*/
#include "e300.h"

static VERSION version = {1, 0, 1};

/* e300 library release notes

1.0.1   11/04/2015
        util.c: cosmetic changes to calm OS X

1.0.0   09/29/2010
        e300.h: initial release
        util.c: initial release
 */

char *e300VersionString()
{
static char string[] = "e300 library version 100.100.100 and slop";

    snprintf(string, strlen(string), "e300 library version %d.%d.%d",
        version.major, version.minor, version.patch
    );

    return string;
}

VERSION *e300Version()
{
    return &version;
}
