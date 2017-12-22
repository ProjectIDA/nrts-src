#pragma ident "$Id: version.c,v 1.26 2017/11/21 21:09:24 dechavez Exp $"
/*======================================================================
 *
 * library version management
 *
 *====================================================================*/
#include "mseed.h"

static VERSION version = {2, 7, 1};

/* mseed library release notes

2.7.1  11/21/2017
       mseed.h: changed mseed512ToIDA1012() to return UINT8 * instead of BOOL
       ida1012.c: changed mseed512ToIDA1012() to return pointer to IDA10.12 packet instead of BOOL

2.7.0  11/21/2017
       mseed.h: added mseedChnLocToChnloc() and mseed512ToIDA1012() prototypes
       chnloc.c: initial release
       ida1012.c: initial release

2.6.4  10/30/2017
       ida10.c: remove libisi dependency by using a local version of ChnlocToChnLoc()

2.6.3  06/20/2017
       unpack.c: fixed sample interval vs sample rate bug in mseedUnpackB100()

2.6.2  11/10/2016
       util.c:   change mseedSetIdent() packet identifier string to be of the form NN_SSS_LL_CCC/MSEED
                 (drop the _Q field so that the Nanometrics Apollo client won't get confused)

2.6.1  12/05/2015
       steim.c:  fixed msr_pack_steimX prototypes to calm OS X compiles
       unpack.c: fixed msr_unpack_steimX prototypes to calm OS X compiles
       util.c:   fixed bug in mseedTimeTearInSamples() (was returning time tear in nsec!)

2.6.0  11/17/2015
       mseed.h: added options field to MSEED_HANDLE
       handle.c: added mseedSetOptions(), initialze (new) options field in InitHandle()
       record.c: modified ContinuousRecord() to check for breaks on tqual or flag transitions depending on options

2.5.5  11/13/2015
       header.c: fixed up some sprintf format vs type inconsitencies, added some casts to calm Darwin compile
       history.c: replaced some incorrect return FALSE statements with return NULL
       pack.c: corrected mess-up comment delimiters around Revision History
       read.c: corrected mess-up comment delimiters around Revision History
       steim.c: declare ms_pack_steim[12]() prototypes
       unpack.c: cast some UNIT8 ptrs to const char ptrs to calm Darwin compiles
       util.c: removed trailing % from sprintf format string

2.5.4  10/30/2015
       util.c: fixed bug in mseedTimeTearInSamples()

2.5.3  10/02/2015
       mseed.h:  added sig field to MSEED_HDR, mseedTimeTearInSamples() and
                 mseedCSSDatatypeString() prototypes
       unpack.c: moved mseedSetIdent() call to the end of mseedUnpackHeader()
                 in order to capture the format and sint needed for sig
       util.c:   introduced mseedSignaturesMatch(), mseedTimeTearInSamples(),
                 mseedCSSDatatypeString(), added code to mseedSetIdent() to also set the new sig field

2.5.2  09/30/2015
       mseed.h: changed MSEED_B52_CLOCK_TOLERANCE() macro to avoid division by zero
                added mseedChnloc() protoype
       b33.c:   removed dead code (AddBlockette33() no longer being used)
       chan.c:  introducing mseedChnloc()

2.5.1  09/15/2015
       mseed.h: changed MSEED_HDR nsamp from INT16 to INT32 and updated mseedCopyX() protoypes accordingly
       copy.c: changed nsamp arg to INT32
       read.c: introduced mseedReadPackedRecord() and modifed mseedReadRecord() to use it
       record: fixed serious bug in ContinuousRecord() where it could return without setting value
       unpack.c: accomodate change of nsamp to INT32 in mseedUnpackFSDH()

2.5.0  09/15/2015
       mseed.h: changed MSEED_FORMAT_x defines to match SEED specification, updated prototypes
       print.c: include format code together with format string in mseedHdrString()
       read.c:  created, introducing mseedReadRecord()
       unpack.c: split FSDH decoding out of mseedUnpackHeader() into mseedUnpackFSDH(), made
          the previously static "internal" decoders public so that they could be used in read.c


2.4.1  09/03/2015
       util.c: fixed bug assinging type in mseedStageType with FILTER_TYPE_IIR_PZ

2.4.0  08/24/2015
       mseed.h: reworked MSEED_B30_type_CODEs to use MSEED_FORMAT_types, fixed some DDL typos,
                ifdef'd out my made up MSEED_B30_FAMILY_IEEEFLOAT
       b30.c:   ifdef out MSEED_B30_FAMILY_IEEEFLOAT from mseedDefaultBlockette30List()
       b52.c:   just print dip as it comes, no conversion

2.3.2  07/16/2015
       b30.c: fixed extra tilde problem
       b52.c: convert 0 to 180 dips to -90 to 90 (might be the right thing)

2.3.1  07/14/2015
       mseed.h: fixed MSEED_B30_STEIM2_CODE (wasn't unique), fixed MSEED_B50_STA_LEN (was too big)
       b30.c: remove leading zeros on integers to improve readability
       b33.c: remove leading zeros on integers to improve readability
       b34.c: fixed len bug, remove leading zeros on integers to improve readability
       b50.c: fixed len bug, added missing tildes
       b52.c: fixed len bug
       b53.c: fixed len bug
       b57.c: removed spurious "history" field left over from sloppy copy and paste
       b58.c: moved semicolon placement in len calculation to match other files

2.3.0  07/10/2015
       mseed.h:
       b50.c: remove leading zeros on integers to improve readability
       b52.c: remove leading zeros on integers to improve readability
       b53.c: created
       b54.c: created
       b57.c: created
       b58.c: created
       util.c: added mseedStageType()

2.2.1  06/30/2015
       mseed.h: MSEED_B52 defined, MSEED_B50 lat/lon/elev changed to REAL64, mseedSetTIMEstring() prototype updated
       b50.c: changed printf to use %lf for now REAL64 lat/lon/elev, add full=TRUE to mseedSetTIMEstring() calls
       b52.c: created
       time.c: added "BOOL full" arg to mseedSetTIMEstring

2.2.0  10/29/2014
       New additions: b30.c, b33.c, b34.c, b50.c, time.c
       mseed.h: MSEED_B30, MSEED_B33, MSEED_B34, MSEED_B50 typedefs defined, MSEED_B52 in progress
       data.c: check for failed Steim compression (prevents seg fault) and repack natively
       log.c: changed name of "format" argument to "message", for better clarity
       pack.c: mseedPackData() "format" argument is now a pointer
       print.c: mseedDataFormatString() changed to only recognize MSEED_FORMAT_X codes
       record.c: removed tabs

2.1.2  08/26/2014
       record.c: flush records if time tears exceed 0.5 samples

2.1.1  08/26/2014
       data.c: fixed bug packing non-Steim data
       header.c: fixed blockette 100 bug (had been setting sample interval instead of rate)
       ida10.c: use new IDA10_TTAG nepoch field to set hdr.tstamp
       record.c: rework time tear logic
       util.c: use hdr->sint in mseedSetEndtime(), fixed terrible bug computing hdr->asint in mseedSetActualSint()

2.1.0  08/19/2014
       mseed.h: removed packed and len fields from MSEED_RECORD and introduced MSEED_PACKED
       data.c:   rework to use MSEED_PACKED
       generic.c: set new sint field
       handle.c: added QC sanity check, removed log messages where handle was still unitialized
       header.c: rework to use MSEED_PACKED
       ida10.c:  set new sint field, fixed missing QC field in ident string bug
       print.c: changed how time quality percentage is displayed
       record.c: keep packing header as long as available data exceed minsamp (major bug fix)
       util.c: added mseedValidQC()

2.0.1  08/12/2015
        record.c: removed incorrect (and misleading) test on gross difference between time stamps

2.0.0   08/11/2014
        initial release
        This is a complete rewrite of the DMC libmseed (which I have now renamed libdmc).
        I had to modify the DMC version of libmseed in order to add support for setting
        time quality (blockette 1001), absolute sample interval (blockette 100) and the
        various FSDH status bytes, but when all was done I was left with something that
        sort of worked but with gaps.  I lacked sufficient understanding of the internals
        of that library so my efforts at debugging were full of confusion but no confidence.
        This library was designed with that missing support explicilty included, and it
        appears to be working.
 */

char *mseedVersionString()
{
static char string[] = "mseed library version 100.100.100 and slop";

    snprintf(string, strlen(string), "mseed library version %d.%d.%d",
        version.major, version.minor, version.patch
    );

    return string;
}

VERSION *mseedVersion()
{
    return &version;
}
