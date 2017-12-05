#pragma ident "$Id: misc.c,v 1.7 2015/12/03 18:06:56 dechavez Exp $"
/*======================================================================
 *
 * Miscellaneous convenience functions
 *
 *====================================================================*/
#include "qdp.h"

/* Convert LCQ sample rate (positive => Hz, negative => 1/rate (Hz)) to convenient equivalents */

void qdpConvertSampleRate(INT16 rate, REAL64 *pfrate, REAL64 *pdsint, UINT64 *pnsint)
{
REAL64 frate, dsint;
UINT64 nsint;

    if (rate == 0 || (pfrate == NULL && pdsint == NULL && pnsint == NULL)) return;

    frate = rate < 0 ? (REAL64) -1.0 / (REAL64) rate : (REAL64) rate;
    dsint = (REAL64) 1.0 / frate;
    nsint = (UINT64) (NANOSEC_PER_SEC * dsint);

    if (pfrate != NULL) *pfrate = frate;
    if (pdsint != NULL) *pdsint = dsint;
    if (pnsint != NULL) *pnsint = nsint;
}

/* Given a src[0] "channel" byte, determine the input channel number.
 * Returns 0-5 for main digitizer inputs, -1 for anything else.
 */

int qdpMainDigitizerChanNum(UINT8 channel)
{
int result;

    switch (channel & QDP_DCM) {
      case QDP_DC_D32:
      case QDP_DC_COMP:
      case QDP_DC_MULT:
        result = channel & (UINT8) ~QDP_DCM;
        break;
      default:
        result = -1;
    }

    return result;
}

/* Ensure consistency between DP tokens and channels emitted on each data port */

static BOOL UpdateDataPortChannelFreqs(QDP_TYPE_C1_LOG *log, QDP_TOKEN_LCQ *lcq, QDP_TYPE_C1_FIX *fix)
{
int i, chan;

/* Do nothing if this token is for something other than a main digitizer channel */

    if ((chan = qdpMainDigitizerChanNum(lcq->src[0])) < 0) return TRUE;

/* Find the frequency bit for the specified sample interval */

    for (i = 0; i < QDP_NFREQ; i++) {
        if (fix->freq[i].nsint == lcq->nsint) {
            log->freqs[chan] |= (UINT8) (1 << i);
            return TRUE;
        }
    }
    return FALSE;
}

static void PrintDportFreqs(UINT16 *freqs)
{
int i;

    for (i = 0; i < QDP_NCHAN; i++) {
        printf("freqs[%d] = ", i); utilPrintBinUINT16(stdout, freqs[i]); printf("\n");
    }
}

void qdpUpdateDataPortChannelFreqs(QDP_TYPE_DPORT *dport, QDP_TYPE_C1_FIX *fix)
{
int i;
LNKLST_NODE *crnt;

/* Clear all outputs for all channels */

    for (i = 0; i < QDP_NCHAN; i++) dport->log.freqs[i] = 0;
    dport->error &= ~QDP_DPORT_ERROR_NOSINT; /* clear any missing sint error */

/* Search the DP tokens and turn on the appropriate frequencies for defined channels */

    crnt = listFirstNode(&dport->token.lcq);
    while (crnt != NULL) {
        if (!UpdateDataPortChannelFreqs(&dport->log, (QDP_TOKEN_LCQ *) crnt->payload, fix)) dport->error |= QDP_DPORT_ERROR_NOSINT;
        crnt = listNextNode(crnt);
    }
}

int qdpCalState(QDP_PKT *pkt, UINT8 *abort)
{
#define CAL_ENABLED_MASK 0x5555 /* 01010101 01010101 */
#define CAL_RUNNING_MASK 0xaaaa /* 10101010 10101010 */
UINT16 bitmap;
static char *fid = "qdpCalState";
        
    if (!qdpHaveCalNotifyBlockette(pkt->payload, pkt->hdr.dlen, abort, &bitmap)) return QDP_CAL_UNCHANGED;

    if (bitmap & CAL_RUNNING_MASK) return QDP_CAL_RUNNING;
    if (bitmap & CAL_ENABLED_MASK) return QDP_CAL_ENABLED;

    return QDP_CAL_IDLE;
}

/*-----------------------------------------------------------------------+
 |                                                                       |
 | Copyright (C) 2012 Regents of the University of California            |
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
 * $Log: misc.c,v $
 * Revision 1.7  2015/12/03 18:06:56  dechavez
 * Added additional input checks and immediately return if no output defined
 *
 * Revision 1.6  2015/08/24 19:08:04  dechavez
 * introduced qdpCalState()
 *
 * Revision 1.5  2012/06/24 18:16:52  dechavez
 * created: qdpConvertSampleRate(), qdpMainDigitizerChanNum(), qdpUpdateDataPortChannelFreqs()
 *
 */
