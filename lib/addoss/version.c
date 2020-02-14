#pragma ident "$Id: version.c,v 1.8 2015/11/04 22:01:19 dechavez Exp $"
/*======================================================================
 * 
 * library version management
 *
 *====================================================================*/
#include "addoss.h"

static VERSION version = {1, 2, 2};

/* addoss library release notes

1.2.2   11/04/2015
        ida10.c: explict cast of arguments needed to calm OS X gcc
        print.c: modify print format strings to calm OS X gcc

1.2.1   04/28/2014
        extract.c: fixed return typo in addossExtractSensorDataInt24() when failing on listCreate()

1.2.0   01/27/2014
		addoss.h: added extract.c prototypes and required includes (list.h, sbd.h)
        extract.c: created
        ida10.c: pre-fill working space with 0xee instead of 0x33 (cosmetic only)

1.1.1   09/20/2013
        ida10.c: changed IDA1010_x constants to IDA1011_x versions

1.1.0   07/10/2013
        addoss.h: changed addossIDA10() prototype to use 32-bit zerotime
        ida10.c: include hard-coded filter delay and emit IDA10.11 packets
        print.c: minor tweak of addossPrintOSG_STATUS() formatting

1.0.0   05/14/2013
        initial production release
 */

char *addossVersionString()
{
static char string[] = "addoss library version 100.100.100 and slop";

    snprintf(string, strlen(string), "addoss library version %d.%d.%d",
        version.major, version.minor, version.patch
    );

    return string;
}

VERSION *addossVersion()
{
    return &version;
}
