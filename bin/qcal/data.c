#pragma ident "$Id: data.c,v 1.11 2016/09/01 17:26:45 dechavez Exp $"
/*======================================================================
 *
 * Handle data port connection
 *
 *====================================================================*/
#include "qcal.h"

#define MY_MOD_ID QCAL_MOD_DATA

static ISI_RAW_PACKET rawpkt;
static ISI_SEQNO seqno;
static QDP_PAR par;
static MUTEX mutex;
static QDP *qdp = NULL;
static BOOL CalReady = FALSE;
static SEMAPHORE sem;

void ShutdownDataConnection(QCAL *qcal)
{
    if (qdp != NULL) qdpShutdown(qdp);
}

/* called by qdp library each time it gets a packet for the app */

static void SaveQ330Packet(void *args, QDP_PKT *pkt)
{
QCAL *qcal;
UINT8 *ptr, *start;
static BOOL first = TRUE;
static char *fid = "SaveQ330Packet";

    if (ExitStatus() != QCAL_STATUS_RUNNING) return;

    if (pkt->hdr.cmd != QDP_DT_DATA || CalInProgress(pkt)) {

        qcal = (QCAL *) args;

    /* Add serial and sequence numbers to the header */

        ptr = start = rawpkt.payload;
        ptr += utilPackUINT64(ptr, qcal->addr.serialno);
        ptr += utilPackUINT32(ptr, seqno.signature);
        ptr += utilPackUINT64(ptr, seqno.counter++);

    /* And the data */

        ptr += utilPackBytes(ptr, pkt->raw, pkt->len);
        rawpkt.hdr.len.used = (ptr - start);

    /* Save it */

        WriteQDP(&rawpkt);
        qdpProcessPacket(&qcal->lcq, pkt); 

    }
}

static THREAD_FUNC DataThread(void *argptr)
{
QCAL *qcal;
int errcode, suberr;
static char *fid = "DataThread";

    qcal = (QCAL *) argptr;

    qdpInitPar(&par, qcal->port);
    qdpSetConnect(&par, &qcal->addr.connect);
    qdpSetSerialno(&par, qcal->addr.serialno);
    qdpSetAuthcode(&par, qcal->addr.authcode);
    qdpSetDebug(&par, qcal->debug);
    qdpSetUser(&par, qcal, SaveQ330Packet);

    if ((qdp = qdpConnect(&par, qcal->lp, &errcode, &suberr)) == NULL) {
        LogMsg("%s:%s registration failed: %s", par.connect.ident, qdpPortString(par.port.link), qdpErrcodeString(errcode));
        printf("%s:%s registration failed: %s\n", par.connect.ident, qdpPortString(par.port.link), qdpErrcodeString(errcode));
        SetDeleteFlag();
        Exit(MY_MOD_ID + 1);
    }

    LogMsg("data thread started");
    SEM_POST(&sem);

    while (1) pause(); /* All action will take place in the callback function */
}

/* Launch Q330 data thread */

BOOL StartDataThread(QCAL *qcal)
{
THREAD tid;
UINT32 interval = 5000; /* 5 second status interval */
static char *fid = "StartDataThread";

    SEM_INIT(&sem, 0, 1);
    MUTEX_INIT(&mutex);

    seqno.signature = time(NULL);
    seqno.counter = 0;

    if (!isiInitRawPacket(&rawpkt, NULL, QDPLUS_PKTLEN)) {
        fprintf(stderr, "%s: isiInitRawPacket: %s\n", fid, strerror(errno));
        return FALSE;
    }

    if (!THREAD_CREATE(&tid, DataThread, (void *) qcal)) {
        fprintf(stderr, "%s: THREAD_CREATE: DataThread: %s\n", fid, strerror(errno));
        return FALSE;
    }

    SEM_WAIT(&sem);

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
 * $Log: data.c,v $
 * Revision 1.11  2016/09/01 17:26:45  dechavez
 * Replace DeregisterQ330() with ShutdownDataConnection(), moved QDP_PAR initializations
 * from StartDataThread() to DataThread(), eliminated some unneeded variables in StartDataThread()
 *
 * Revision 1.10  2016/08/04 22:31:27  dechavez
 * libqdp 3.13.1 changes (basically added regerr to qdpConnect call)
 *
 * Revision 1.9  2016/06/23 20:27:59  dechavez
 * deregister without waiting for ACK (thinking that this could be the source
 * of some of the occasional hanging exits)
 *
 * Revision 1.8  2015/12/07 19:05:29  dechavez
 * fixed fprintf() format vs argument discrepancies
 *
 * Revision 1.7  2011/07/28 00:05:54  dechavez
 * renamed raw to rawpkt to avoid name clashes with curses that is creeping in via include files
 *
 * Revision 1.6  2011/01/25 18:47:20  dechavez
 * Use new QDP_CONNECT "connect" field in Q330_ADDR
 *
 * Revision 1.5  2011/01/14 00:32:34  dechavez
 * qdpConnectWithPar() to qdpConnect(), QDP_PAR "host" to "ident"
 *
 * Revision 1.4  2010/04/12 21:09:56  dechavez
 * removed console verbosity
 *
 * Revision 1.3  2010/04/01 20:30:30  dechavez
 * extensive rework to avoid race conditions
 *
 * Revision 1.2  2010/03/31 20:42:37  dechavez
 * removed qdpSetMyXPorts
 *
 * Revision 1.1  2010/03/31 19:46:38  dechavez
 * initial release
 *
 */
