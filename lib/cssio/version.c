#pragma ident "$Id: version.c,v 1.20 2016/06/23 20:17:26 dechavez Exp $"
/*======================================================================
 * 
 * library version management
 *
 *====================================================================*/
#include "cssio.h"

static VERSION version = {2, 5, 4};

/* cssio library release notes

2.5.4   06/23/2016
        togse.c: fixed long present error in testing WriteHeader() return status

2.5.3   11/04/2015
        readata.c: fixed a whole bunch of missing return values for the ReadXX() subfunctions
        togse.c: fixed some format strings to call OS X compiler
        css/2.8/wfdiscio.h: changed %ld in CSSIO_WFDISC28_SCS macro to %d (to calm OS X)

2.5.2   10/02/2015
        cssio.h: added cssio_samplesize() prototype
        datatype.c: added cssio_samplesize()

2.5.1   08/24/2015
        jdate.c: use gmtime_r() instead of gmtime_().  Must have been a typo???

2.5.0   04/18/2014
        cssio.h: added CSSIO_DATATYPE_x enums, updated prototypes
        readata.c: reworked to use CSSIO_DATATYPE_x enums instead of datatype strcmp's
        string.c:  added cssioDatatypeString(), cssioDatatypeListString().  Changed code from UINT8 to int.

2.4.5   04.15.2014
        readata.c: created, with cssioReadDataIntoReal64()
        wwfdisc.c: use MT unsafe buffer if cssioWfdiscString() is called without user supplied space

2.4.0   04.14.2014
        include/css/3.0/wfdiscio.h: defined WFDISC_C type and CSSIO_FIELD_x macros
        rwfdrec.c: added cssioReadWfdiscC().  added arg check to rwfdrec()
        string.c: new file, introduces cssioFieldString(), cssioFieldCode(), cssioFieldMatch()

2.3.0   12/12/2013
        css2sac.c: zero path the numeric extension for duplicate sta.chan paths

2.2.3   12/02/2013
        include/3.0/wfdiscio.h: changed all %ld to %d (same reason as for 2.2.2)
        wf28to30.c: compute endtime using double precision arithmitic

2.2.2   11/01/2013
        wwfdisc.c: change all %ld to %d (on 64-bit Linux this was causing the INT32s to print as garbage)

2.2.1   02/20/2012
        css2sac.c, datatype.c, togse.c:
            use portable integer types (eg, long to INT32, unsigned long to UINT32)

2.2.0   03/17/2011
        changed all macros and constants to use names with CSSIO_ or cssio_
        prefixes in order to avoid conflicts with IDA DCC source code

2.1.6   01/07/2007
        css2sac.c: strlcpy() instead of strcpy()
        togse.c: strlcpy() instead of strcpy()
        wf28to30.c: strlcpy() instead of strcpy()
        wwfdisc.c: strlcpy() instead of strcpy()
        version.c: snprintf() instead of sprintf()

2.1.5   10/11/2005
        desc.c: initial release

2.1.4   03/31/2005 (cvs rtag libcssio_2_1_4 libcssio)
        css2sac.c: fixed bug generating sac time stamp

2.1.3   02/10/2005 (cvs rtag libcssio_2_1_3 libcssio)
        css2sac.c, togse.c: aap win32 portability mods

2.1.2   08/24/2003 (cvs rtag libcssio_2_1_2 libcssio)
        togse.c: split long chan names into chan and auxid

2.1.1   11/13/2003 (cvs rtag libcssio_2_1_1 libcssio)
        css2sac.c: moved SAC specific stuff into css2sac to allow exporting the
        rest of the library without having to drag the sac stuff as well.

2.1.0   10/15/2003 (cvs rtag libcssio_2_1_0 libcssio)
        wwfdisc.c: added reentrant cssioWfdiscString() and recast
            wdtoa() as a call to it with a static (gag) buffer

2.0.0   06/19/2003
        ESSW imported release
  
 */

char *cssioVersionString(VOID)
{
static char string[] = "cssio library version 100.100.100 and slop";

    snprintf(string, strlen(string), "cssio library version %d.%d.%d",
        version.major, version.minor, version.patch
    );

    return string;
}

VERSION *cssioVersion(VOID)
{
    return &version;
}
