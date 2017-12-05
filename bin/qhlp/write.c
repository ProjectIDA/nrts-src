#pragma ident "$Id: write.c,v 1.1 2014/08/28 21:35:43 dechavez Exp $"
/*======================================================================
 *
 *  Write a packet to disk.  Mutex protected because this can get called
 *  by SaveIDA10() as well as the libisidl push client.
 *
 *====================================================================*/
#include "qhlp.h"

#define MY_MOD_ID QHLP_MOD_WRITE

BOOL WritePacketToDisk(ISI_DL *dl, ISI_RAW_PACKET *raw)
{
BOOL ok;
static char *fid = "WritePacketToDisk";

    BlockShutdown(fid);
        if (dl->options.flags & ISI_OPTION_INSERT_32BIT_SEQNO)  dl->options.offset.seqno  = ida10SeqnoOffset(raw->payload);
        if (dl->options.flags & ISI_OPTION_INSERT_32BIT_TSTAMP) dl->options.offset.tstamp = ida10TstampOffset(raw->payload);
        ok = isidlWriteToDiskLoop(dl, raw);
    UnblockShutdown(fid);

    if (!ok) {
        LogMsg("%s: isidlWriteToDiskLoop failed: %s\n", fid, strerror(errno));
        return FALSE;
    }

    return TRUE;
}

/*-----------------------------------------------------------------------+
 |                                                                       |
 | Copyright (C) 2014 Regents of the University of California            |
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
 * $Log: write.c,v $
 * Revision 1.1  2014/08/28 21:35:43  dechavez
 * created
 *
 */
