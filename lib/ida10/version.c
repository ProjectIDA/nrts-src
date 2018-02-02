#pragma ident "$Id: version.c,v 1.59 2018/01/18 23:31:20 dechavez Exp $"
/*======================================================================
 *
 * library version management
 *
 *====================================================================*/
#include "ida10.h"

static VERSION version = {2, 25, 1};

/* ida10 library release notes

2.25.1 02/02/2018
       ida10.h: added ident[] to TSHDR, changed convention wrt to boxid and serialno (0 means not set)
       misc.c: added support for IDA10.12 offsets
       string.c: support for ident[] field added to TSHDR
       unpack.c: support for ident[] field added to TSHDR

2.25.0 01/18/2018
       ida10.txt reworked IDA10.12 definition to include serial number, SEED_TAG, and longer station name
       ida10.h:  changes to support rework of IDA10.12 definition
       ttag.c:   IDA10_TIMER_SEED support (part of IDA10.12 redefinition)
       unpack.c: changes to support rework of IDA10.12 definition

2.24.0 10/19/2017
       Makefile  Removed pack.c from build since not referenced anywhere. Leaving source code here for now.
       pack.c    inserted comments about being this source file being unreferenced
       read.c    added IDA10_SUBFORMAT_12 to ReadRecord()
       unpack.c  Added support for subformat 10.12 which has mseed 512 records as it's payload

2.23.2 12/07/2015
       pack.c:   casts and format fixes to calm OS X compiles
       rename.c: fixed == typo in assigning errno
       ttag.c:   casts and format fixes to calm OS X compiles
       unpack.c: casts and format fixes to calm OS X compiles

2.23.1 06/17/2015
       string.c: improved clock quality in ida10TSHDRtoString()

2.23.0 08/29/2014
       string.c: added ida10PacketString()
       util.c:   use mseedFactMultToNsint() instead of ms_nomsampinterval() to compute nsint

2.22.0 08/26/2014
       ida10.h:  added nepoch to IDA10_TTAG
       ttag.c:   added nepoch support to ida10CompleteTimeTag(), ida10TtagIncrErr()
                 reports error for all time tears (earlier was lax about Q330 tears
                 that were less than one sample... don't recall why I did that)

2.21.0 08/08/2014
       rename.c: added ida10Cname()

2.20.0 04/30/2014
       ida10.c:  added percent field to IDA10_CLOCK_STATUS
       ttag.c:   for Q330s, set IDA10_CLOCK_STATUS percent to Q330 clock quality

2.19.0 04/14/2014
       ttag.c:   set locked and suspect generic clock status for Q330 based on q330.qual.percentage
                 (using hard-coded thresholds)
       unpack.c: use libmseed's ms_nomsampinterval() to get nominal sample interval

2.18.1 09/20/2013
       read.c:   added IDA10_SUBFORMAT_11 to ReadRecord() (worked before because unpack.c was incorrect)
       ttag.c:   added IDA10_TIMER_OBS2 support to PrintHeader()
       unpack.c: IDA10_SUBFORMAT_10 replaced with IDA10_SUBFORMAT_11 in UnpackCmnHdr11()

2.18.0 07/10/2013
       ida10.h, pack.c, ttag.c, unpack.c: IDA10_OBSTAG2 and IDA10.11 support

2.17.0 05/11/2013
       ida10.h, pack.c, read.c, ttag.c, unpack.c: IDA10.10 support

2.16.0 02/07/2013
       ida10.h: added ida10PackTSHdr() and ida10PackTS() prototypes
       pack.c:  created

2.15.0 12/13/2012
       ida10.h: added IDA10_NAME_MAP and rename.c prototypes
       rename.c: created, with ida10ReplaceCname(), ida10ReplaceSname(), ida10BuildChannelMap()

2.14.4 01/11/2012
       ttag.c: include human readable tofs, host tstamp, and seqno when printing OFIS ttag

2.14.3 11/09/2011
       mseed.c: back out 2.14.2 change (done instead in libliss 1.3.6)

2.14.2 11/09/2011
       mseed.c: split illegal (too long) station names into sta+net

2.14.1 10/28/2011
       mseed.c: changed ida10ToMseed() to populate a LISS_MSEED_HDR structure

2.14.0 10/24/2011
       mseed.c initial release of ida10ToMseed()

2.13.1 10/10/2011 (fixed 10.8 support)
       ida10.h: defined IDA10x_RESERVED_BYTES
       misc.c:  added ida10SeqnoOffset() and ida10TstampOffset()
       unpack.c: use global IDA10x_RESERVED_BYTES instead of local SUBFORMAT_x_RESERVED_BYTES,
           set extra.valid FALSE for 10.5, added tstamp decode to 10.8 (and set extra.valid TRUE)

2.13.0 07/01/2011
       ttag.c: set IDA10_TTAG ext and hz fields from Q330 and GENERIC time tags
          to be equal to sys field (instead of 0 which is equivalent, but which
          caused some other programs to complain).

2.12.1 03/17/2011
       read.c: fixed bug in ReadRecord() that assumed caller allocated space
         for entire packet, even when only headers were being read
       unpack.c: moved setting of datatype from ida10UnpackTS() to ida10UnpackTSHdr()

2.12.0 02/25/2011
       read.c: added TSheader option to ida10ReadGz() and ida10ReadRecord()

2.11.0 02/24/2011
       read.c: added ida10ReadGz()

2.10.0 01/25/2011
       ida10.h: IDA10_MAXDATALEN increased to 16320, fixed nasty bug in
         defining IDA10_TSDAT (multiplied instead of divided by datum size!)

2.9.0  09/10/2010
       misc.c: added ida10HaveOrigSeqno()
       string.c: added support for optional origseqno in IDA_EXTRA
       unpack.c: added 10.6, 10.7, and 10.8 support

2.8.1  05/10/2010
       ttag.c: avoid division by zero error when a bad header results in nsint == 0
       unpack.c: avoid division by zero error when a bad header results in nsint == 0

2.8.0  03/11/2009
       unpack.c: supprt for IDA10.4 LCQ src bytes

2.7.5  02/03/2009
       string.c: added alert characters for suspect time PrintStatusFlag() and unused bytes in ida10TSHDRtoString()
       unpack.c: fixed sign problem in sint() (aap)

2.7.4  08/20/2008
       ttag.c: set new "fresh" bit in generic ttag status

2.7.3  04/03/2008
       ttag.c: fixed core dumping typo in ttag.c when printing usupported
        time tags (IDA10_TIMER_GENERIC in this case, which are still unsupported)

2.7.2  04/02/2008
       read.c: added ida10.5 support

2.7.1  01/07/2008
       unpack.c: added ida10PackGenericTtag()

2.7.0  12/21/2007
       unpack.c: added ida10.5 support
       ttag.c:   added IDA10_TIMER_GENERIC support

2.6.9  06/21/2007
       ttag.c: added support for 'derived' time status field

2.6.8  06/01/2007
       unpack.c: use IDA_DEFAULT_EXTRA to initialize ida10CMNHDR "extra"

2.6.7  05/11/2007
       ttag.c: fixed Q330 filter delay compensation

2.6.6  04/18/2007
       string.c:  use static buffer if output not supplied in calls to
        ida10CMNHDRtoString(), ida10TSHDRtoString(), ida10TStoString()

2.6.5  03/06/2007
       ttag.c: fixed bug printing offset in PrintTtag()

2.6.4  01/08/2007
       unpack.c: strlcpy() instead of strncpy()
       version.c: snprintf() instead of sprintf()

2.6.3  01/04/2006
       ttag.c: made ida10TtagIncrErr debug messages a compile time option,
               added support for incerr struct in TS header
       unpack.c: support for nanosecond sint and incerr struct in TS header

2.6.2  12/21/2006
       misc.c: initial release
       string.c: update 10.4 support
       ttag.c: update 10.4 support
       read.c: update 10.4 support
       unpack.c: update 10.4 support

2.6.1  12/08/2006
       string.c: 10.4 support
       ttag.c: preliminary 10.4 support
       unpack.c: preliminary 10.4 support

2.6.0  11/08/2006
       read.c: removed some debug printfs
       reorder.c: added REAL64 support
       string.c: added REAL64 support, generic clock status
       ttag.c: generic timetag support
       unpack.c: updated 10.3 support, added proper REAL64 support

2.5.1  08/18/2006
       read.c: added 10.3 support
       unpack.c: added 10.3 support

2.5.0  08/14/2006
       reorder.c: removed premature 64 bit data type support
       string.c: removed premature 64 bit data type support, added OFIS ttag support
       ttag.c:
       unpack.c:

2.4.1  10/06/2005
       string.c: include format.subformat in ida10CMNHDRtoString()
       unpack.c: include format and subformat in common header, work
                 around bogus nbuytes in early 10.2 CF records

2.4.0  08/25/2005
       read.c: added 10.2 support
       string.c: added 10.2 support
       unpack.c: added 10.2 support

2.3.0  03/23/2005 (cvs rtag libida10_2_3_0 libida10)
       reorder.c: added (untested) support for 64-bit data types
       string.c: added (untested) support for 64-bit data types
       unpack.c: added (untested) support for 64-bit data types

2.2.0   12/09/2004 (cvs rtag libida10_2_2_0 libida10)
        string.c: added ida10TtagToString(), ida10ClockStatusString()
        ttag.c: added ida10SystemTime(), ida10PhaseToUsec(), modified
            ida101HzTime() to use latter

2.1.4   06/25/2004 (cvs rtag libida10_2_1_4 libida10)
        unpack.c: fixed dangling comment in WIN32 ifdef causing nasty ComputeEndTimeTS bug

2.1.3   06/24/2004 (cvs rtag libida10_2_1_3 libida10)
        read.c: removed unnecessary includes (aap)
        sint.c: removed unnecessary includes (aap)
        string.c: removed unnecessary includes (aap)
        ttag.c: removed unnecessary includes (aap)
        unpack.c: removed unnecessary includes, WIN32 port (aap)

2.1.2   12/09/2003 (cvs rtag libida10_2_1_2 libida10)
        sint.c: included math.h
        unpack.c: use sign/unsigned specific utilUnpack...s

2.1.1   11/13/2003 (cvs rtag libida10_2_1_1 libida10)
        ida10.h: removed uneeded includes

2.1.0   10/16/2003 (cvs rtag libida10_2_1_0 libida10)
        reorder.c: initial release
        string.c: added ida10DataTypeString(), and type specific max/min
            in ida10TStoString()
        unpack.c: added support for all (uncompressed) data types in
            ida10UnpackTS()
        version.c: initial release

2.0.0   Initial release

 */

char *ida10VersionString()
{
static char string[] = "ida10 library version 100.100.100 and slop";

    snprintf(string, strlen(string), "ida10 library version %d.%d.%d",
        version.major, version.minor, version.patch
    );

    return string;
}

VERSION *ida10Version()
{
    return &version;
}
