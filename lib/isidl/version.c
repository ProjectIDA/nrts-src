/*======================================================================
 *
 * library version management
 *
 *====================================================================*/
#include "isi/dl.h"

static VERSION version = {1, 26, 3};

/* isidl library release notes

1.26.3 05/31/2018 dauerbach
       slink.c: moved init of slink queue semaphore and startup of RingServerWriteThread back to
                isiFeedSeedLink() from isidlSetSeedLinkOption() to be sure(?) that semaphore is
                created after main process is forked when run in background. Discovered problem in qhlp.

1.26.2 01/08/2018
       push.c: changed format of incoming connection message
       slink.c: debugged IDA1012Branch() (renamed from IDA1012BranchTaken)

1.26.1 01/08/2018 dauerbach
       slink.c:isiFeedSeedLink:  moved call to IDA1012BranchTaken() to after check for raw == NULL

1.26.0 12/20/2017
       dl.c: added NRTS support for IDA10.12 (in NRTSCompatiblePacket()) (untested)
       slink.c: added IDA1012BranchTaken() (untested)

1.25.3 08/26/2016
       dl.c: commented out the disk loop lock/unlock debug messages in LockDiskLoop()

1.25.2 07/20/2016
       isi/dl.h: changed ISI_DL_DEFAULT_HOME_DIR and ISI_DL_DEFAULT_DBID to /Users/nrts for MacOS builds

1.25.1 12/07/2015
       dl.c:    fixed sloppy block comment delimiter
       meta.c:  fixed format strings to calm OS X compiles
       pack.c:  utilUnpackINT64() instead of utilUnpackUINT64() where needed (benign, calms OS X)
       slink.c: added missing return value to InitRecordQ()
       tee.c:   fixed format strings to calm OS X compiles
       print.c: fixed format strings to calm OS X compiles

1.25.0 08/28/2014
       isi/dl.h: removed mseed and bud from ISI_DL_OPTIONS, and made it a part of the ISI_DL handle
       bud.c:    removed
       dl.c:     support move of options from its own entity to a member of the ISI_DL handle
       master.c: support for new options argument to isidlOpenDiskLoopSet()
       mon.c:    support for new options argument to isidlOpenDiskLoopSet()
       push.c:   rework to use user supplied function for writing data, and move of options into ISI_DL handle
       slink.c:  include dmc/libmseed.h since it is now gone from isi/dl.h
       utils.c:  removed mseed and bud from ISI_DL_OPTIONS

1.24.1 08/19/2014
       isi/dl.h: added debug field to ISI_DL_SEEDLINK
       slink.c:  reworked to use MSEED_PACKED instead of MSEED_RECORD (libmseed 2.1.0)

1.24.0 08/11/2014
       isi/dl.h: reworked ISI_DL_SEEDLINK to support new (IDA) libmseed
       dl.c:     calls to isidlFeedSeedLink() changed to support new inteface in modified slink.c
       slink.c:  major rework to use redesigned libmseed

1.23.0 04/30/2014
       isi/dl.h: removed MSEED_HANDLE, made ISI_DL_SEEDLINK::mseed a void pointer (cast to MSTemplateHandle *)
       dl.c:     mods to support change of slink.mseed from MSEED_HANDLE to void pointer
       slink.c:  rework to use MSTemplateHandle clock quality and status bit support

1.22.2 07/19/2013
       push.c: fixed bug that caused core dump when serving more than one push client

1.22.1 03/08/2013
       slink.c: fixed bug with timeout logic during queue flush

1.22.0 03/08/2013
       isi/dl.h: moved ISI_DL_SEEDLINK from options to ISI_DL (to make for easy exit handling)
       dl.c:     isidlCloseDiskLoop() flushes partial MiniSEED packets
       slink.c:  added ability to flush partial MiniSEED packets
       utils.c:  removed ISI_DL_SEEDLINK from isidlInitOptions()

1.21.2 02/07/2013
       isi/dl.h: added MUTEX, LOGIO and dropped count to ISI_DL_SEEDLINK
       slink.c: Memory leak was NOT fixed in 1.21.1 (sigh).  Reworked buffering. Again.

1.21.1 02/07/2013
       slink.c: fixed LNKLST_MODE memory leak in PopMiniSEED

1.21.0 02/07/2013
       isi/dl.h: changed ISI_DL_SEEDLINK to use (new) RECORDQ instead of MSGQ_BUF
       slink.c: reworked to use RECORDQ for buffering output data

1.20.4 02/05/2013
       slink.c: fixed time stamp truncation error (MAJOR BUG!)

1.20.3 01/23/2013
       isi/dl.h: updated isidlLogSeedLinkOption() prototype
       slink.c: fixed uninitialized slink->mseed.msr BUG, added extra sanity checks, improved(?) logging

1.20.2 01/22/2013
       slink.c: don't print message when tossing full MiniSEED packets after queue buffer is full

1.20.1 01/22/2013
       slink.c: fixed memory link in SendToRingServer(), set msgq log pointer

1.20.0 01/14/2013
       isi/dl.h: initial structures and prototypes for SeedLink support
       dl.c: tee to SeedLink option enabled, BUD option now ignored (will prune later)
       utils.c: added SeedLink support to isidlInitOptions()
       slink.c: created

1.19.1 07/30/2012
       isi/dl.h: defined ISI_DL_FLAGS_ASYNC_WRITE option flag
       dl.c: isidlOpenDiskLoop() to NOT use O_SYNC for ISI writes when ISI_DL_FLAGS_ASYNC_WRITE selected

1.19.0 06/27/2012
       utils.c: added isidlLocateDiskLoopBySite()

1.18.1 12/15/2011
       dl.c: insert host time stamp in IDA packet header before updating NRTS
             (that allows it to be there for use by the NRTS sanity checks)

1.18.0 11/07/2011
       isi/dl.h: created ISI_DL_BUD
       bud.c: created: isidlWriteToBud()
       dl.c: if ISI_DL_OPTION mseed is set, create equivalent MiniSEED header before writing
           packets to ISI disk loop, and tee those files to BUD if ISI_DL_BUD is set
       push.c: use ISI_DL_OPTIONS
       utils.c: added isidlInitOptions()

1.17.0 10/12/2011
       isi/dl.h: introduced ISI_DL_OPTIONS
       dl.c: use ISI_DL_OPTIONS to insert 32-bit seqno and timestamps into raw packets
       push.c: created

1.16.1 07/28/2011
       mon.c: removed debug() calls, fixed +++ typo and added uptime to NRTS display.

1.16.0 07/27/2011
       isi/dl.h: added ISI_MON and related prototypes
       mon.c: created

1.15.1 11/10/2010
       dl.c: check ISI_DL_SYS hdrlen in isidlOpenDiskLoop() to ensure existing
         disk loop isn't incompatible. Add site name to LockDiskLoop() error messages.

1.15.0 11/04/2010
       isi/dl.h: add hdrlen to ISI_DL_SYS
       dl.c: ISI_DL_SYS hdrlen support
       print.c: ISI_DL_SYS hdrlen support

1.14.0 08/27/2010
       pack.c: initial release
       report.c: initial release

1.12.3 06/23/2010
       include/platform.h: asynchronous MSYNC (MS_ASYNC)

1.12.2 01/25/2010
       print.c: added build time (seqno signature) to isidlPrintDL()

1.12.1 11/29/2009
       dl.c: changed LockDiskLoop to ignore locks when lock FILE is null (ie, when creating disk loop)

1.12.0 03/17/2009
       dl.c: reworked disk loop locks to use libutil utilWriteLockWait()
             and utilReadLockWait(), removed mutex in ISI_DL handle

1.11.4  02/23/2009
        dl.c: restored MUTEX_INIT to isidlSnapshot() (dealing with memory
            at the application level)

1.11.3  02/03/2009
        dl.c: added debug messages in isidlReadDiskLoop()
        glob.c: added isidlLoadDebugFlags()

1.11.2  01/26/2009
        dl.c: removed unwise MUTEX_INIT in isidlSnapshot() (cased memory leaks)

1.11.1  12/15/2008
        dl.c: restored O_SYNC when writing NRTS files, set sync flag in all isidlSysSetX calls

1.11.0  01/07/2008 *** First version with automatic MiniSEED to IDA10 ***
        dl.c: automatically convert MiniSEED to IDA10/NRTS, if possible

1.10.4  11/05/2007
        dl.c: add NRTSCompatiblePacket() test before attempting to
              update NRTS disk loop

1.10.3  11/05/2007
        glob.c: backed out short sighted "fix" added in 1.10.2

1.10.2  10/31/2007
        glob.c: removed memory leak in isidlSetGlobalParameters()
        print.c: added buffer length to utilTimeString() calls

1.10.1  10/05/2007
        glob.c: added support for NRTS_HOME environment variable

1.10.0  09/14/2007
        dl.c: removed ISI_DL_FLAGS_TIME_DL_OPS_ENABLED related code,
              improved debug and log messages, added support for both
              partial and complete overlaps, fixed problem with plugged
              NRTS diskloops following first rejected packet

1.9.5   06/12/2007
        wfdisc.c: added EnforceSampleMaximum() to prevent illegal wfdisc
                  records (more than 99,999,999 samples) from being generated

1.9.4   04/18/2007
        dl.c: note state file in dl flags, if present
        print.c: note state file, if present

1.9.3   02/08/2007
        dl.c: disk loop writers keep files open, added conditional support
        for ISI_DL_FLAGS_TIME_DL_OPS (not compiled in this build)

1.9.2   01/31/2007
        wfdisc.c: allow for missing NRTS disk loop

1.9.1   01/22/2007
        dl.c: added support for ISI_DL_FLAGS_IGNORE_LOCKS flag, and changed
        isiReadDiskLoop not choke on indices explicitly set as undefined.
        glob.c: added flags field

1.9.0   01/11/2007
        Renamed all functions to use isidl prefix instead of isi.

1.8.2   01/11/2007
        datreq.c: renamed all the "stream" requests to the more accurate "twind" (time window)

1.8.1   01/08/2007
        datreq.c: switch to size-bounded string operations
        dl.c: switch to size-bounded string operations
        tee.c: switch to size-bounded string operations
        version.c: switch to size-bounded string operations

1.8.0   12/12/2006
        dl.c: added metadata directory support, isidlCrntSeqno()
        meta.c: initial release
        print.c: print availability of metadata

1.7.1   12/08/2006
        print.c: include sequence number of oldest packet in isiPrintDL()

1.7.0   11/08/2006
        dl.c: flock locking only if ENABLE_DISKLOOP_LOCKS defined (and
              it isn't in this build)
        glob.c: added ttag to ISI_DEBUG

1.6.7   09/29/2006
        dl.c: fixed typo in fcntl error reports in LockDiskLoop/UnlockDiskLoop,
        don't attempt to unlock a non-locked file
        util.c: cleared tabs

1.6.6   07/10/2006
        tee.c: prevent open tee files from being copied under Windows

1.6.5   06/26/2006
        cnf.c: use dummy "when" for isiLookupCoords
        master.c: fixed signed/unsigned compares, removed unreferenced local variables

1.6.4   06/20/2006
        utils.c: allow for ISI systems w/o NRTS disk loops in isiLocateNrtsDiskloop()

1.6.3   06/12/2006
        dl.c: added sync arguments to isidlSysX functions such that
            disk syncs didn't happen everytime the structure was
            changed, just after critical updates.
            Added O_BINARY to OpenAndSeek flags for Win32 compatibility (aap)
        sys.c: made sync to disk optional via new "BOOL sync" argument

1.6.2   06/02/2006
        cnf.c: allow for missing NRTS disk loop
        dl.c: wrappped all changes to mapped ISI sys structure with calls
              to isidlSysXXX functions that use msync() to flush to disk
        soh.c: allow for missing NRTS disk loop
        sys.c: initial release

1.6.1   04/20/2006
        dl.c: added isidlUpdateParentID(), check for matching locks

1.6.0   04/07/2006
        dl.c: removed references to obsolete fields in tee handle
        tee.c: remove gzip support (was causing data loss when plug was pulled)

1.5.0   03/15/2006
        dl.c: O_SYNC on write (this was a major oversight(!))
        glob.c: major rework to use libisidb lookups instead of flat file
        print.c: include tee file name in isiPrintDL() output

        tee.c: support changed field names in updated ISI_GLOB

1.4.0   02/08/2006
        cnf.c: isiLookupSitechan() instead of isiLookupInst()
        dl.c: fixed race condition in LockDiskLoop()
        glob.c: pass LOGIO to dbioOpen
        master.c: warn instead of fail when unable to open a master list site

1.3.4   12/09/2005
        dl.c: recognize NRTS_NOTDATA status from decoder, changed isiWriteDiskLoop()
              to take flags argument and added support for new ISI_REJECT_DUPLICATES option

1.3.3   10/19/2005
        utils.c: fixed uninitialized variable bug in isiLocateNrtsDiskloop()

1.3.2   10/18/2005
        dl.c: log failed fcntl in LockDiskloop closer to event

1.3.1   10/09/2005
        tee.c: gzip by default, require nozip flag file to disable

1.3.0   09/30/2005
        dl.c: changes to tee handle (for gzip), changed name of ReadRaw for Win32 sanity
        glob.c: initialize db isiInitDefaultGlob()
        search.c: deal with requests for packets outside the bounds of the disk loop
        tee.c: support on the fly enable/disable and gzip on/off

1.2.0   09/09/2005
        dl.c: major rework to use explicit hdr and raw file I/O instead of
              mapped memory operations
        glob.c: added support for lock debugger
        print.c: ISI_DL_SYS instead of ISI_DL_HDR

1.1.0   08/25/2005
        dl.c: added tee support
        glob.c: added tee (trecs) support

1.0.2   07/27/2005
        dl.c: only write NRTS_ACCEPTed packets to NRTS disk loop

1.0.1   07/26/2005
        db.c: removed deadlock in isiLookupInst()
        dl.c: include mutex lock in disk loop lock, lock diskloop while closing,
              removed deadlock in isiOpenDiskLoop()

1.0.0   07/25/2005
        cnf.c: initial release
        datreq.c: initial release
        db.c: initial release
        dl.c: initial release
        glob.c: initial release
        master.c: initial release
        print.c: initial release
        search.c: initial release
        soh.c: initial release
        string.c: initial release
        utils.c: initial release
        wfdisc.c: initial release
 */

char *isidlVersionString()
{
static char string[] = "isi library version 100.100.100 and slop";

    snprintf(string, strlen(string), "isi library version %d.%d.%d",
        version.major, version.minor, version.patch
    );

    return string;
}

VERSION *isidlVersion()
{
    return &version;
}
