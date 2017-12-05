#pragma ident "$Id: version.c,v 1.13 2015/12/04 22:18:26 dechavez Exp $"
/*======================================================================
 * 
 * library version management
 *
 *====================================================================*/
#include "liss.h"

static VERSION version = {1, 3, 7};

/* liss library release notes

1.3.7   12/07/2015
        pack.c:   fixed format strings to calm OS X compiles
        strint.c: fixed format strings to calm OS X compiles

1.3.6   11/09/2011
        pack.c: pad short strings with spaces in lissPackMseedHdr()

1.3.5   11/02/2011
        liss.h: replaced fsdh and b1000 in LISS_PKT with LISS_MSEED_HDR hdr
        pack.c: adjust for fsdh and b1000 moving into LISS_MSEED_HDR hdr field
        string.c: lissDataFormatString(), lissMiniSeedHdrString(), lissPrintMiniSeedHdr()

1.3.0   10/28/2011
        liss.h: added LISS_MSEED_HDR type, updated prototypes
        pack.c: added lissPackMseedHdr()

1.2.4   11/19/2009
        pack.c: LISS_HEARTBEAT support (aap)

1.2.3   02/03/2009
        steim.c: fixed usigned to signed int conversion bug in lissDecompressSteim1

1.2.2   02/03/2008
        pack.c: lissUnpackMiniSeed() made smarter about decoding non-data
            packets, return value switched from BOOL to decode status
        read.c: lissRead() changed to receive all packets of the expected
            size, regardless of contents (let lissUnpackMiniSeed() decide
            if it can be interpreted)

1.2.1   01/10/2008
        steim.c: deal with bus errors associated with pointer casts directly
                 on input byte stream

1.2.0   01/07/2008
        liss.h: added various constants, added support for optional data
                decoding in LISS_PKT handle
        read.c: removed selection tests and data decode operations from
                lissRead() which were moved to lissUnpackMiniSeed()
        steim.c: replaced hardcoded error return codes with liss.h constants
        time.c: added lissSint()
        unpack.c: added selection tests and optional data decoding

1.1.1   10/31/2007
        pack.c: replaced string memcpy with strlcpy

1.1.0   05/02/2007
        read.c: fixed uninitialized "ident" bug

1.0.1   01/08/2007
        open.c: snprintf() instead of sprintf()
        version.c: snprintf() instead of sprintf()

1.0.0   09/30/2005
        initial release
        
 */

char *lissVersionString()
{
static char string[] = "liss library version 100.100.100 and slop";

    snprintf(string, strlen(string), "liss library version %d.%d.%d",
        version.major, version.minor, version.patch
    );

    return string;
}

VERSION *lissVersion()
{
    return &version;
}
