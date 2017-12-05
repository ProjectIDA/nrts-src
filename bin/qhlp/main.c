#pragma ident "$Id: main.c,v 1.13 2016/01/19 23:18:01 dechavez Exp $"
/*======================================================================
 *
 *  QDP disk loop to HLP disk loop
 *
 *====================================================================*/
#include "qhlp.h"

LOGIO_WATCHDOG *Watch = NULL;

#define MY_MOD_ID QHLP_MOD_MAIN

/* Main program loop */

static THREAD_FUNC MainThread(MainThreadParams *cmdline)
{
QDP_LCQ *lcq;
QHLP_PAR *qhlp;
QDPLUS_PKT pkt;
static UINT64 count = 0;
static char tmpbuf[64];
static char *fid = "MainThread";

    qhlp = init(cmdline->myname, cmdline->argc, cmdline->argv);

    while (1) {

        QuitOnShutdown(MY_MOD_ID + 0);

        ReadNextPacket(qhlp, &pkt);
        ++count;

        if ((lcq = qdplusProcessPacket(qhlp->qdplus, &pkt)) == NULL) {
            LogMsg("%s: qdplusProcessPacket failed", fid);
            Exit(MY_MOD_ID + 1);
        }

        if (lcq->event != QDP_LCQ_EVENT_OK) {
            isiSeqnoString(&pkt.seqno, tmpbuf);
            if (lcq->event & QDP_LCQ_EVENT_NO_META)      LogMsg("%s: QDP_LCQ_EVENT_NO_META (0x%02x)", tmpbuf, lcq->meta.state);
          //if (lcq->event & QDP_LCQ_EVENT_UNSUPPORTED)  LogMsg("%s: QDP_LCQ_EVENT_UNSUPPORTED",  tmpbuf);
            if (lcq->event & QDP_LCQ_EVENT_UNDOCUMENTED) LogMsg("%s: QDP_LCQ_EVENT_UNDOCUMENTED", tmpbuf);
          //if (lcq->event & QDP_LCQ_EVENT_NO_TOKENS)    LogMsg("%s: QDP_LCQ_EVENT_NO_TOKENS",    tmpbuf);
            if (lcq->event & QDP_LCQ_EVENT_DECOMP_ERR)   LogMsg("%s: QDP_LCQ_EVENT_DECOMP_ERR",   tmpbuf);
            if (lcq->event & QDP_LCQ_EVENT_SINT_CHANGE)  LogMsg("%s: QDP_LCQ_EVENT_SINT_CHANGE",  tmpbuf);
            if (lcq->event & QDP_LCQ_EVENT_FATAL) {
                LogMsg("%s: DP_LCQ_EVENT_FATAL", tmpbuf);
                Exit(MY_MOD_ID + 2);
            }
        }

        if (count % qhlp->SyncInterval == 0) SaveState(qhlp->qdplus);

    }
}

#ifdef WIN32_SERVICE
#   include "win32svc.c"
#else

/* Or just run it like a regular console app or Unix program */

#ifdef unix
int main(int argc, char **argv)
#else
void main(int argc, char **argv)
#endif
{
MainThreadParams cmdline;

    cmdline.argc   = argc;
    cmdline.argv   = argv;
    cmdline.myname = argv[0];

    MainThread(&cmdline);
}

#endif /* WIN32_SERVICE */

/*-----------------------------------------------------------------------+
 |                                                                       |
 | Copyright (C) 2007 Regents of the University of California            |
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
 * $Log: main.c,v $
 * Revision 1.13  2016/01/19 23:18:01  dechavez
 * print the value of the meta state when found to be incomplete
 *
 * Revision 1.12  2014/08/19 18:15:56  dechavez
 * Remove QDP_LCQ_IS_DT_USER support (we never used this).
 *
 * Revision 1.11  2011/04/07 22:58:50  dechavez
 * generate IDA10 barometer packets in response to QDP_LCQ_IS_DT_USER lcq actions
 * Implemented in a lazy, but not really dangerous fashion... see comments.
 *
 * Revision 1.10  2010/03/22 21:54:50  dechavez
 * Removed all special handling of calibration monitor data
 *
 * Revision 1.9  2010/03/12 00:47:09  dechavez
 * rename CheckCalibrationStatus() to UpdateCalibrationState()
 *
 * Revision 1.8  2009/07/09 18:36:51  dechavez
 * switch to lcq instead of c1_stat for determining calibration state
 *
 * Revision 1.7  2009/07/08 16:04:18  dechavez
 * pass serial number to SetCalibrationStatus (multiple DAS calibration trigger bug fix)
 *
 * Revision 1.6  2009/07/02 23:09:27  dechavez
 * track calibration status (for automatic trigger support)
 *
 * Revision 1.5  2009/05/14 18:08:50  dechavez
 * include sequence number in warning messages
 *
 * Revision 1.4  2009/01/26 21:18:55  dechavez
 * switched to liblogio 2.4.1 syntax for watchdog loggin
 *
 * Revision 1.3  2009/01/05 17:42:28  dechavez
 * added logioUpdateWatchdog() tracers
 *
 * Revision 1.2  2007/05/15 20:43:02  dechavez
 * use QuitOnShutdown() instead of ShutdownInProgress() test
 *
 * Revision 1.1  2007/05/03 21:12:28  dechavez
 * initial release
 *
 */
