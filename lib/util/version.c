#pragma ident "$Id: version.c,v 1.79 2015/12/22 21:37:55 dechavez Exp $"
/*======================================================================
 * 
 * library version management
 *
 *====================================================================*/
#include "util.h"

static VERSION version = {3, 20, 2};

/* util library release notes

3.20.2 12/22/2015
       rwtimeo.c: use FD_SETSIZE for width in all select() calls.  Fixes all sorts of weird downstream problems in FreeBSD 10.1

3.20.1 12/07/2015
       misc.c: removed buggy LinuxSetHostAddr() in favor of working UnixSetHostAddr().  Ie, removed ifdef LINUX test.

3.20.0 11/24/2015 (mysemX() semphores added)
       mysem.h: created
       mysem.c: created

3.19.2 11/13/2015
       compress.c: added some parens to turn off Darwin "using the result of an assignment as a condition without parenthese" whines
       connect.c: declare addrlen as unsigned int instead of int
       email.c: added missing return value
       gsecmp.c: added some parens to turn off Darwin "using the result of an assignment as a condition without parenthese" whines
       list.c: cast some [s]printf() arguments to calm Darwin compiles
       log.c: added explicit format string to syslog() to calm Darwin compiles

3.19.1 08/24/2015
       util.h: defined UTIL_COMPLEX_1 and UTIL_COMPLEX_0 constants, added utilYearDay() prototype
       complex.c: utilComplexMultiply() to use temporary variable (not sure why I felt it was needed)
       timefunc.c: introduced utilYearDay()

3.19.0 07/10/2015
       util.h: added prototypes for complex.c functions
       complex.c: initial release

3.18.0 03/06/2015
       util.h: added prototypes for new functions
       chncmp.c: initial release
       copy.c: initial release
       dir.c: introduced utilRemoveDirectoryTree()
       timefunc.c: introduced utilConvertFrom1970SecsTo1999Nsec(), utilConvertFrom1999NsecTo1970Secs()

3.17.0 08/11/2014
       util.h: added USEC_PER_NANOSEC constant and updated prototypes
       sockinfo.c: changed optlen type from int to socklen_t
       timefunc.c: added utilTsplit2(), utilDecomposeTimestamp(), utilComposeTimestamp()

3.16.5 04/28/2014
       util.h: added UTIL_CPU_TIMER and related prototypes
       timer.c: added utilInitCPUtimer(), utilStartCPUtimer(), utilStopCPUtimer()

3.16.0 04/18/2014
       timefunc.c: added format code 16 (yyyyddd mm dd, yyyy hh:mm:ss)

3.15.5 04/15/2014
       swap.c: added utilSwapREAL32(), utilSwapREAL64()

3.15.0 04/14/2014
       myip.c: return result in network byte order
       steim.c: at some time I added some explicit type casts... don't see
           they make any difference.  Committing the beast anyway.
       timefunc.c: added format code 10 (Day Mon hh:mm:ss year)

3.14.0 09/20/2013
       targ.c: initial release

3.13.0 03/07/2013
       kcap.c: initial release

3.12.0 02/07/2013
       list.h: added listUnlinkNode() and listCount() prototypes
       list.c: added listUnlinkNode() and listCount() (lightly tested, I think they work)

3.11.3 12/13/2012
       util.h: added utilCurrentSecond() prototype
       timer.c: added utilCurrentSecond()

3.11.2 12/13/2012
       parse.c: changed benign but incorrect return FALSE to return NULL in utilStringTokenList()

3.11.1 05/02/2012
       timer.c: added utilMsecToTimeval()

3.11.0 04/25/2012
       parse.c: added utilParseServer()

3.10.1 02/15/2012
       compress.c: gsecmp.c: steim.c: swap.c: timefunc.c:
	   use portable integer types (eg, long to INT32, unsigned long to UINT32)

3.10.0 11/02/2011
       pack.c: added utilPackUINT8Array(), utilUnpackUINT8Array()

3.9.3  10/12/2011
       timefunc.c: added support for SAN epoch time values

3.9.2  03/17/2011
       etoh.c, htoe.c: timefunc.c: changed "struct date" to "struct css_date"
          (to avoid conficts with Antelope software)

3.9.1  01/23/2011
       checksum.c: created w/utilIpHeaderChecksum(), utilUdpChecksum(), and
          util_fletcher32() and util_fletcher16() lifted from fletcher.c
       fletcher.c: removed

3.9.0  09/10/2010
       crc.c: initial release

3.8.5  11/03/2009
       fletcher.c: initial release

3.8.4  09/28/2009
       binprint.c: added utilBinString() changed utilBinString() to use same
       pack.c: added utilPackPascalString(), utilUnpackPascalString()
       string.c: added utilTrimNewline(), utilIsBlankString()

3.8.0  03/17/2009
       lock.c: added utilWriteLockWait(), utilWriteLockTry(), utilReadLockWait(),
               utilReadLockTry(), utilReleaseLock(),
               removed utilLockFileWait() and utilUnlockFile()
       log.c: allow specifying log facility with syslog option

3.7.0  10/02/2008
       binprint.c: added utilPrintBinUINT16
       string.c: added utilDotDecimalString

3.6.0  03/13/2008
       lock.c: added utilLockFileWait() and utilUnlockFile()

3.5.0  01/25/2008
       list.c: added listCopy()

3.4.8  01/07/2008
       binprint.c: little-endian fix for utilPrintBinUINT32()
       list.c: made formerly static DestroyNode() public listDestroyNode()

3.4.7  10/30/2007
       peer.c: replace string memcpy w/ strlcpy
       timefunc.c: replace string memcpy w/ strlcpy, add length to utilTimestring() args

3.4.6  10/17/2007
       pack.c: added support for Slate ARM5 cpu REAL64s (fshelly)

3.4.5  09/25/2007
       mkfile.c: allow for zero length files 

3.4.4  09/14/2007
       misc.c: added utilDeleteFile() to unix builds

3.4.3  06/28/2007
       misc.c: removed extra *hp from linux builds
       peer.c: use socklen_t instead of int
       string.c: added strlcpy for systems without HAVE_STRLCPY defined

3.4.2  01/31/2007
       log.c: fixed long standing bug handling "syslogd" specifier

3.4.1  01/07/2007
       Makefile: removed isfloat.o
       etoh.c: strlcpy() instead of strcpy()
       log.c: strlcpy() instead of strcpy(), vsnprintf() instead of vsprintf()
       mkpath.c: strlcpy() instead of strcpy()
       peer.c: strlcpy() instead of strcpy()
       timefunc.c: removed some strcpy()'s, some remain
       version.c: snprintf() instead of sprintf()

3.4.0  12/12/2006
       dir.c: initial release
       io.c: initial release

3.3.0  12/06/2006
       binprint.c: intial release of utilPrintBinUINT8() and utilPrintBinUINT32()
       timefunc.c: allow NULL destination buffers (use mt-unsafe local storage)

3.2.3  11/08/2006
       timefunc.c: cosmetic changes to utilTimeString()

3.2.2  06/26/2006
       lock.c: made separate unix and windows (dummy) functions

3.2.1  06/14/2006
       misc.c: fixed error in Linux gethostbyaddr_r call

3.2.0  05/15/2006
       misc.c: allow for NULL hostname in utilSetHostAddr() (INADDR_ANY)
       timer.c: changed utilStartTimer() interval parameter from int to UINT64

3.1.0  04/27/2006
       timer.c: added utilStartTimer(), utilInitTimer(), utilElapsedTime()
                and utilTimerExpired()

3.0.4  11/03/2005
       list.c: removed unreferenced local variables

3.0.3  09/30/2005
       mkpath.c: 09-30-2005 aap win32 port
       string.c: utilPadString(), utilTrimString()

3.0.2  09/12/2005
       mmap.c: removed MAP_ALIGN option

3.0.1  08/25/2005
       misc.c: added utilDirectoryExists()
       timefunc.c: fixed typo in UTIL_LONGTIME_FACTOR

3.0.0  07/25/2005 (bumped full rev just because nrts and isi went full rev today)
       list.c: added listRemoveNode() and listClear()

2.9.1  07/06/2005
       log.c: added hook to permit use of logio routines
       setid.c: initial release

2.9.0  06/29/2005
       misc.c: set ENOENT errno when utilSetHostAddr() unable to determine address
       timefunc.c: INT64 timestamp support
       timer.c: INT64 timestamp support

2.8.0  05/26/2005
       rwtimeo.c: added utilRead() and utilWrite() (msec timeouts)

2.7.0  05/24/2005 (cvs rtag libutil_2_7_0  libutil)
       mkfile.c: initial release
       mmap.c: initial production release
       query.c: utilPause() to return char that was pressed
 
2.6.3  05/05/2005 (cvs rtag libutil_2_6_3  libutil)
       peer.c: fixed benign #endif;

2.6.2  04/03/2005 (cvs rtag libutil_2_6_2  libutil)
       timefunc.c: oops, didn't get all the gmtimes in 2.6.1.  OK now.

2.6.1  03/31/2005 (cvs rtag libutil_2_6_1  libutil)
       timefunc.c: make utilTsplit and utilDttostr reentrant throught the 
                   use of gmtime_r instead of gmtime.

2.6.0  03/23/2005 (cvs rtag libutil_2_6_0  libutil)
       misc.c: Linux portability mods (aap)
       timefunc.c: added type 14 and 15 strings to utilDttostr (aap)

2.5.1  02/07/2005 (cvs rtag libutil_2_5_1  libutil)
       BufferedStream.c: changed lstrcpy to STRCPY macro for unix portablity

2.5.0  02/07/2005 (cvs rtag libutil_2_5 0  libutil)
       BufferedStream.c: initial release

2.4.1  12/20/2004 (cvs rtag libutil_2_4_1  libutil)
       rwtimeo.c: aap win32 mods

2.4.0  12/09/2004 (cvs rtag libutil_2_4_0  libutil)
       timefunc.c: add support for SAN epoch timestamps to utilDttostr()

2.3.0  09/28/2004 (cvs rtag libutil_2_3_0  libutil)
       connect.c: allow for blocking sockets via negative port numbers in util_connect()

2.2.10 08/24/2004 (cvs rtag libutil_2_2_10 libutil)
       timefunc.c: fixed utilYdhmstod bug in setting up date_time parameters for util_htoe

2.2.9 06/30/2004 (cvs rtag libutil_2_2_9 libutil)
      log.c: remove "obnoxious "logging suspended" message in util_logclose()

2.2.8 06/24/2004 (cvs rtag libutil_2_2_8 libutil)
      list.c: various AAP windows portability modifications
      lock.c: various AAP windows portability modifications
      log.c: various AAP windows portability modifications
      parse.c: various AAP windows portability modifications
      syserr.c: various AAP windows portability modifications

2.2.7 06/10/2004 (cvs rtag libutil_2_2_7 libutil)
      list.c: various AAP windows portability modifications
      log.c: various AAP windows portability modifications
      misc.c: various AAP windows portability modifications
      parse.c: various AAP windows portability modifications
      peer.c: various AAP windows portability modifications

2.2.6 01/29/2004 (cvs rtag libutil_2_2_6 libutil)
      peer.c: fix memory leak in utilPeerAddr()

2.2.5 12/09/2003 (cvs rtag libutil_2_2_5 libutil)
      isfloat.c: added includes to calm solaris cc
      swap.c: renamed swappers to UINTx, INTZx's now being util.h macros

2.2.4 12/04/2003 (cvs rtag libutil_2_2_4 libutil)
      timefunc.c: fixed bizarre bug with utilAttodt return value
          getting lost on x86 gcc builds
      bcd.c compress.c getline.c log.c: removed tabs

2.2.3 11/26/2003 (cvs rtag libutil_2_2_3 libutil)
      bground.c: do not close all open file descriptors

2.2.2 11/21/2003 (cvs rtag libutil_2_2_2 libutil)
      removed fnmatch.c

2.2.1 11/19/2003 (cvs rtag libutil_2_2_1 libutil)
      list.c: included util.h to calm certain compilers
      parse.c: added explicit strtok_r declaration
      swap.c: made util_lswap and util_sswap macros
      
2.2.0 11/14/2003 (cvs rtag libutil_2_2_0 libutil)
      bground.c: recast util_bground() into call to utilBground()
      dump.c: obsoleted
      hexdump.c: initial version
      log.c: removed Sigfunc casts
      parse.c: removed dead code, recast util_parse into call() to utilParse()
      signal.c: removed util_signal and made util_sigtoa() almost reentrant
      timefunc.c: merged in old style functions from timefuncOLD.c
      timefuncOLD.c: removed

2.1.1 11/02/2003 (cvs rtag libutil_2_1_1 libutil)
      list.c: improved error checking in listCreate() and listSetArrayView()
      misc.c: utilCloseSocket() changed to return INVALID_SOCKET

2.1.0
        bground.c: utilBackGround() to return an ENOSYS failure for
            non-unix builds
        dump.c: removed utilLogHexDump() (moved to logio library as
            logioHexDump())
        list.c: imported from ex-liblist
        misc.c: added tests for invalid arguments
        pack.c: added utilPackREAL64(), utilUnpackREAL64(), fixed case
            sensitivity error in various function names revealed for
            LTL_ENDIAN_HOST builds
        parse.c: added utilStringTokenList(), made other functions 
            reentrant via strtok_r instead of strtok
        timer.c: initial release

2.0.0   06/09/2003 (cvs rtag libutil_0_0_0 libutil)
        Initial release
  
 */

char *utilVersionString()
{
static char string[] = "util library version 100.100.100 and slop";

    snprintf(string, strlen(string), "util library version %d.%d.%d",
        version.major, version.minor, version.patch
    );

    return string;
}

VERSION *utilVersion()
{
    return &version;
}
