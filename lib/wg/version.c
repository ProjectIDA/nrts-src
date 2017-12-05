#pragma ident "$Id: version.c,v 1.1 2012/07/03 16:07:13 dechavez Exp $"
/*======================================================================
 * 
 * library version management
 *
 *====================================================================*/
#include "wg.h"

static VERSION version = {0, 0, 0};

/* wg library release notes

0.0.0  Initial release
  
 */

char *wgVersionString()
{
static char string[] = "wg library version 100.100.100 and slop";

    snprintf(string, strlen(string), "wg library version %d.%d.%d",
        version.major, version.minor, version.patch
    );

    return string;
}

VERSION *wgVersion()
{
    return &version;
}
