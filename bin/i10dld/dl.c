#pragma ident "$Id: dl.c,v 1.4 2018/01/11 19:06:26 dechavez Exp $"
/*======================================================================
 *
 * Disk loop I/O
 * This is where the isidlPacketServer() callback is defined.
 *
 *====================================================================*/
#include "i10dld.h"

#define MY_MOD_ID MOD_DL

static ISI_DL *dl = NULL;
static UINT64 count = 0;
static BOOL Debug = DEFAULT_VERBOSE;

void ToggleDLWriteVerbosity(void)
{
    Debug = !Debug;
    LogMsg("incoming packet logging %s", Debug ? "enabled" : "disabled");
}

ISI_DL *OpenDiskLoop(char *dbspec, char *myname, char *site, LOGIO *lp, int flags, char *seedlink, BOOL verbose)
{
static ISI_GLOB glob;
static UINT32 options = ISI_OPTION_GENERATE_SEQNO | ISI_OPTION_INSERT_32BIT_SEQNO | ISI_OPTION_INSERT_32BIT_TSTAMP;
static char *fid = "OpenDiskLoop";

    if (!isidlSetGlobalParameters(dbspec, myname, &glob)) {
        LogMsg("%s: isidlSetGlobalParameters failed: %s\n", fid, strerror(errno));
        Exit(MY_MOD_ID + 1);
    }

    if ((dl = isidlOpenDiskLoop(&glob, site, lp, ISI_RDWR, options)) == NULL) {
        LogMsg("%s: isidlOpenDiskLoop failed: %s\n", fid, strerror(errno));
        Exit(MY_MOD_ID + 2);
    }
    LogMsg("%s disk loop opened", dl->sys->site);

    if (dl->nrts != NULL) {
        dl->nrts->flags |= NRTS_DL_FLAGS_STATION_SYSTEM;
        dl->options.flags |= ISI_OPTION_REJECT_COMPLETE_OVERLAPS;
    }
    if (flags & OPTION_LAXNRTS) dl->nrts->flags |= NRTS_DL_FLAGS_LAX_TIME;

    if (seedlink != NULL && !isidlSetSeedLinkOption(dl, seedlink, myname, 0)) {
        LogMsg("ERROR: %s: isidlSetSeedLinkOption failed", fid);
        Exit(MY_MOD_ID + 3);
    }
    isidlLogSeedLinkOption(lp, LOG_INFO, dl);

    if (verbose) Debug = TRUE;

    return dl;
}

void CloseDiskLoop(void)
{
    if (dl != NULL) {
        LogMsg("%s disk loop closed, %llu packets acquired this session", dl->sys->site, count);
        isidlCloseDiskLoop(dl);
    }
    dl = NULL;
}

BOOL USER_SUPPLIED_WRITE_FUNCTION(ISI_DL *dl, ISI_RAW_PACKET *raw)
{
BOOL ok;
char msgbuf[128];
static char *fid = "WritePacketToDisk";

    if (raw->hdr.desc.type != ISI_TYPE_IDA10) {
        LogMsg("unexpected packet type %s dropped", isiDescString(&raw->hdr.desc, msgbuf));
        return TRUE;
    }

    GrabGlobalMutex(fid);
        if (dl->options.flags & ISI_OPTION_INSERT_32BIT_SEQNO)  dl->options.offset.seqno  = ida10SeqnoOffset(raw->payload);
        if (dl->options.flags & ISI_OPTION_INSERT_32BIT_TSTAMP) dl->options.offset.tstamp = ida10TstampOffset(raw->payload);
        ok = isidlWriteToDiskLoop(dl, raw);
        ++count;
    ReleaseGlobalMutex(fid);

    if (!ok) {
        LogMsg("%s: isidlWriteToDiskLoop failed: %s\n", fid, strerror(errno));
        return FALSE;
    }

    if (Debug) LogIDA10PacketHeader(raw->payload, count);

    return TRUE;
}

/*-----------------------------------------------------------------------+
 |                                                                       |
 | Copyright (C) 2018 Regents of the University of California            |
 |                                                                       |
 | This software is provided 'as-is', without any express or implied     |
 | warranty.  In no event will the authors be held liable for any        |
 | damages arising from the use of this software.                        |
 |                                                                       |
 | Permission is granted to anyone to use this software for any purpose, |
 | including commercial applications, and to alter it and redistribute   |
 | it freely, subject to the following restrictions:                     |
 |                                                                       |
 | 1. The origin of this software must not be misrepresented; you must   |
 |    not claim that you wrote the original software. If you use this    |
 |    software in a product, an acknowledgment in the product            |
 |    documentation of the contribution by Project IDA, UCSD would be    |
 |    appreciated but is not required.                                   |
 | 2. Altered source versions must be plainly marked as such, and must   |
 |    not be misrepresented as being the original software.              |
 | 3. This notice may not be removed or altered from any source          |
 |    distribution.                                                      |
 |                                                                       |
 +-----------------------------------------------------------------------*/

/* Revision History
 *
 * $Log: dl.c,v $
 * Revision 1.4  2018/01/11 19:06:26  dechavez
 * added support for verbose option
 *
 * Revision 1.3  2018/01/11 00:05:41  dechavez
 * removed log message about first packet (library does that already)
 *
 * Revision 1.2  2018/01/10 23:51:55  dechavez
 * first production release
 *
 * Revision 1.1  2018/01/10 21:20:18  dechavez
 * created
 *
 */
