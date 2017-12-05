#pragma ident "$Id: qdp.c,v 1.6 2016/09/01 17:13:24 dechavez Exp $"
/*======================================================================
 *
 * Save QDP packets
 *
 *====================================================================*/
#include "qcal.h"

#define MY_MOD_ID QCAL_MOD_QDP

static MUTEX mutex;
static gzFile *gz = Z_NULL;
static BOOL CurrentCalibrationState = FALSE;
static UINT32 ReadySeqno = 0;
static QDP_TYPE_C1_STAT status;


QDP *RegisterWithConfigPort(QCAL *qcal)
{
QDP *qdp;
int errcode, suberr; 

    if ((qdp = q330Register(&qcal->addr, QDP_CFG_PORT, qcal->debug, qcal->lp, &errcode, &suberr)) == NULL) { 
        if (errcode == QDP_ERR_BUSY) {
            fprintf(stderr, "%s %s already in use\n", qcal->addr.connect.ident, qdpPortString(QDP_CFG_PORT));
            fprintf(stderr, "You wouldn't have a Willard session running, would you?\n");
        } else {
            fprintf(stderr, "Unable to register with configuration port: %s\n", qdpErrcodeString(errcode));
        }       
    }

    return qdp;
}

void SetCalibrationInitialized(UINT32 seqno)
{
    MUTEX_LOCK(&mutex);
        ReadySeqno = seqno;
    MUTEX_UNLOCK(&mutex);
}

static BOOL CalibrationInitialized(UINT32 seqno)
{
BOOL retval;

    MUTEX_LOCK(&mutex);
        if (ReadySeqno == 0) {
            retval = FALSE;
        } else if (seqno < ReadySeqno) {
            retval = FALSE;
        } else {
            retval = TRUE;
        }
    MUTEX_UNLOCK(&mutex);

    return retval;
}

BOOL CalInProgress(QDP_PKT *pkt)
{
UINT8 abort;
UINT16 bitmap;
BOOL cal_in_progress;
UINT32 seqno;

    utilUnpackUINT32(pkt->payload, &seqno);
    if (!CalibrationInitialized(seqno)) return FALSE;

    if (!qdpHaveCalNotifyBlockette(pkt->payload, pkt->hdr.dlen, &abort, &bitmap)) return CurrentCalibrationState;

    if (abort) {
        LogMsg("calibration aborted");
        SetExitStatus(0);
    }

    if ((cal_in_progress = (bitmap != 0)) != CurrentCalibrationState) {
        if (cal_in_progress) {
            LogMsg("calibration detected");
        } else {
            LogMsg("calibration finished");
            SetExitStatus(0);
        }
        CurrentCalibrationState = cal_in_progress;
    }

    return CurrentCalibrationState;
}

void CloseQDP(QCAL *unused)
{
    if (gz != Z_NULL) gzclose(gz);
}

void WriteQDP(ISI_RAW_PACKET *raw)
{
int errnum;

    if (gz == Z_NULL) return;

    if (gzwrite(gz, raw->payload, raw->hdr.len.used) <= 0) {
        LogMsg("ERROR: gzwrite: %s", gzerror(gz, &errnum));
        SetExitStatus(MY_MOD_ID + 3);
    }
}

BOOL OpenQDP(QCAL *qcal)
{
    MUTEX_INIT(&mutex);
    qdpInit_C1_STAT(&status);

    if (qcal->output & OUTPUT_QDP) {
        if ((gz = gzopen(qcal->name.qdp, "w")) == Z_NULL) {
            fprintf(stderr, "ERROR: gzopen: %s: %s\n", qcal->name.qdp, strerror(errno));
            return FALSE;
        }
   }

    return TRUE;
}

/*-----------------------------------------------------------------------+
 |                                                                       |
 | Copyright (C) 2010 Regents of the University of California            |
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
 * $Log: qdp.c,v $
 * Revision 1.6  2016/09/01 17:13:24  dechavez
 * introduced RegisterWithConfigPort()
 *
 * Revision 1.5  2015/12/07 18:59:09  dechavez
 * moved include flag from arguments to new "output" QCAL field
 *
 * Revision 1.4  2014/06/12 20:17:39  dechavez
 * only save QDP packets if enabled by OpenQDP() includeQDP argument
 *
 * Revision 1.3  2010/04/12 21:07:56  dechavez
 * removed console printing
 *
 * Revision 1.2  2010/04/01 20:25:53  dechavez
 * don't Exit(), set flag instead (avoids race conditions)
 *
 * Revision 1.1  2010/03/31 19:46:38  dechavez
 * initial release
 *
 */
