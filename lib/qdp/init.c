#pragma ident "$Id: init.c,v 1.32 2016/08/04 21:40:15 dechavez Exp $"
/*======================================================================
 * 
 * Initialization routine.  Completes the handle and starts the machine.
 *
 *====================================================================*/
#include "qdp.h"
#include "qdp/fsa.h"

static void InitReorderBuffer(QDP *qp)
{
int i;

    qp->recv.count = 0;
    qp->recv.last_packet = 0;
    for (i = 0; i < 4; i++) qp->recv.ack[i] = 0;
    for (i = 0; i < QDP_MAX_WINDOW_NBUF; i++) qp->recv.msg[i] = NULL;
    utilInitTimer(&qp->recv.timer);
}

static BOOL InitPktQ(QDP *qp)
{
    if (!msgqInitBuf(&qp->Q.pkt, qp->par.nbuf, sizeof(QDP_PKT))) return FALSE;
    msgqSetLog(&qp->Q.pkt.heap, qp->lp);
    msgqSetLog(&qp->Q.pkt.full, qp->lp);

    return TRUE;
}

static BOOL InitEventQ(QDP *qp)
{
    if (!msgqInitBuf(&qp->Q.event, 1, sizeof(QDP_EVENT))) return FALSE;
    msgqSetLog(&qp->Q.event.heap, qp->lp);
    msgqSetLog(&qp->Q.event.full, qp->lp);

    return TRUE;
}

static BOOL InitCmdQ(QDP *qp)
{
MSGQ_MSG *msg;
QDP_CMD *cmd;

    if (!msgqInitBuf(&qp->Q.cmd, 1, sizeof(QDP_PKT))) return FALSE;
    msgqSetLog(&qp->Q.event.heap, qp->lp);
    msgqSetLog(&qp->Q.event.full, qp->lp);

    SEM_INIT(&qp->cmd.sem, 0, 1);
    qp->cmd.msg = NULL;

    return TRUE;
}

static BOOL InitConnections(QDP *qp)
{
char *iostr;
struct sockaddr_in addr;
int debug = QIO_DEBUG_OFF;
char tmpstr[QDP_IDENT_LEN + 1];
static char *fid = "qdpInit:InitConnections";

    if (qp->par.options & QDP_PAR_OPTION_QIO_TERSE) debug = QIO_DEBUG_TERSE;
    if (qp->par.options & QDP_PAR_OPTION_QIO_VERBOSE) debug = QIO_DEBUG_VERBOSE;

    if (qp->par.connect.type == QIO_UDP) {
        if (debug != QIO_DEBUG_OFF) logioMsg(qp->lp, LOG_INFO, "%s: qp->par.connect.type == QIO_UDP\n", fid);
        if (!utilSetHostAddr(&addr, qp->par.connect.ident, 0)) {
            logioMsg(qp->lp, LOG_ERR, "%s: utilSetHostAddr: %s: %s", fid, qp->par.connect.ident, strerror(errno));
            return FALSE;
        }
        qp->peer.addr = ntohl(addr.sin_addr.s_addr);
        snprintf(qp->peer.ident, QDP_IDENT_LEN + 1, "%016llX", qp->par.serialno);
        iostr = NULL;
    } else {
        if (debug != QIO_DEBUG_OFF) logioMsg(qp->lp, LOG_INFO, "%s: qp->par.connect.type == QIO_TTY\n", fid);
        qp->peer.addr = QDP_BROADCAST_IP;
        iostr = qp->par.connect.iostr;
        strncpy(qp->peer.ident, iostr, QDP_IDENT_LEN + 1);
    }

    if (!qioInit(&qp->ctrl, 0, qp->par.port.ctrl.value, iostr, qp->par.timeout.ctrl, qp->lp, debug)) {
        logioMsg(qp->lp, LOG_ERR, "%s: qioInit (ctrl port %d): %s", fid, qp->par.port.ctrl.value, strerror(errno));
        qp->errcode = qp->par.connect.type == QIO_UDP ? QDP_ERR_UDPIO : QDP_ERR_TTYIO;
        return FALSE;
    }

    if (qp->peer.port.data != 0) {
        if (!qioInit(&qp->data, 0, qp->par.port.data.value, iostr, qp->par.timeout.data, qp->lp, debug)) {
            logioMsg(qp->lp, LOG_ERR, "%s: udpioInit (data port %d): %s", fid, qp->par.port.data.value, strerror(errno));
            qp->errcode = qp->par.connect.type == QIO_UDP ? QDP_ERR_UDPIO : QDP_ERR_TTYIO;
            return FALSE;
        }
    }

    return TRUE;
}

static void InitFSA(QDP_FSA *fsa, QDP_PAR *par)
{
    MUTEX_INIT(&fsa->mutex);

    fsa->nthread = 0;

    SEM_INIT(&fsa->semaphore.Automaton, 0, 1);
    SEM_INIT(&fsa->semaphore.WatchdogThread, 0, 1);

    fsa->state.current = fsa->state.initial = (par->connect.type == QIO_UDP) ? QDP_STATE_UNREG : QDP_STATE_NEEDSN;
    fsa->state.previous = QDP_STATE_OFF;

    fsa->tld = FALSE;
}

BOOL qdpInit(QDP *qp, QDP_PAR *par, LOGIO *lp)
{
static char *fid = "qdpInit";

    if (qp == NULL || par == NULL ) {
        logioMsg(lp, LOG_ERR, "%s: NULL input not allowed", fid);
        errno = EINVAL;
        return FALSE;
    }

    qp->dbgpkt = NULL;
    qp->lp = lp;
    qp->par = *par;
    qp->suberr = QDP_CERR_NOERR;
    qp->errcode = QDP_ERR_NO_ERROR;
    qp->reboots = 0;
    InitReorderBuffer(qp);
    utilInitTimer(&qp->open);
    qdpInitMemBlk(&qp->meta.raw.token, QDP_MEM_TYPE_INVALID);

/* define the UDP port number(s) we will be using */

    switch (qp->par.port.link) {
      case QDP_CFG_PORT:
        qp->peer.port.ctrl = qp->par.port.base + QDP_PORT_CONFIG;
        qp->peer.port.data = 0;
        qp->dataLink = FALSE;
        break;
      case QDP_SFN_PORT:
        qp->peer.port.ctrl = qp->par.port.base + QDP_PORT_SPECIAL;
        qp->peer.port.data = 0;
        qp->dataLink = FALSE;
        break;
      case QDP_LOGICAL_PORT_1:
        qp->peer.port.ctrl = qp->par.port.base + QDP_PORT_1_CTRL;
        qp->peer.port.data = qp->par.port.base + QDP_PORT_1_DATA;
        qp->par.hbeat.bitmap |= QDP_STATUS_DATA_PORT_1;
        qp->meta.raw.token.type = QDP_MEM_TYPE_CONFIG_DP1;
        qp->dataLink = TRUE;
        break;
      case QDP_LOGICAL_PORT_2:
        qp->peer.port.ctrl = qp->par.port.base + QDP_PORT_2_CTRL;
        qp->peer.port.data = qp->par.port.base + QDP_PORT_2_DATA;
        qp->par.hbeat.bitmap |= QDP_STATUS_DATA_PORT_2;
        qp->meta.raw.token.type = QDP_MEM_TYPE_CONFIG_DP2;
        qp->dataLink = TRUE;
        break;
      case QDP_LOGICAL_PORT_3:
        qp->peer.port.ctrl = qp->par.port.base + QDP_PORT_3_CTRL;
        qp->peer.port.data = qp->par.port.base + QDP_PORT_3_DATA;
        qp->par.hbeat.bitmap |= QDP_STATUS_DATA_PORT_3;
        qp->meta.raw.token.type = QDP_MEM_TYPE_CONFIG_DP3;
        qp->dataLink = TRUE;
        break;
      case QDP_LOGICAL_PORT_4:
        qp->peer.port.ctrl = qp->par.port.base + QDP_PORT_4_CTRL;
        qp->peer.port.data = qp->par.port.base + QDP_PORT_4_DATA;
        qp->par.hbeat.bitmap |= QDP_STATUS_DATA_PORT_4;
        qp->meta.raw.token.type = QDP_MEM_TYPE_CONFIG_DP4;
        qp->dataLink = TRUE;
        break;
      default:
        logioMsg(lp, LOG_ERR, "%s: illegal link port '%d'", fid, qp->par.port.link);
        errno = EINVAL;
        return FALSE;
    }

/* initialize the physical connection(s) */

    if (!InitConnections(qp)) {
        logioMsg(lp, LOG_ERR, "%s: InitConnections: %s", fid, strerror(errno));
        return FALSE;
    }

/* initialize the automaton semaphores and state */

    InitFSA(&qp->fsa, par);

/* init the circular buffer message queues */

    if (!InitEventQ(qp)) {
        logioMsg(lp, LOG_ERR, "%s: InitEventQ: %s", fid, strerror(errno));
        return FALSE;
    }
    if (!InitPktQ(qp)) {
        logioMsg(lp, LOG_ERR, "%s: InitPktQ: %s", fid, strerror(errno));
        return FALSE;
    }
    if (!InitCmdQ(qp)) {
        logioMsg(lp, LOG_ERR, "%s: InitCmdQ: %s", fid, strerror(errno));
        return FALSE;
    }

/* init the stats */

    qdpInitStats(qp);

/* init the outbound sequence number */

    qp->seqno = 0;

/* init the timer */

    qp->start = utilTimeStamp();

/* done */

    return TRUE;
}

/*-----------------------------------------------------------------------+
 |                                                                       |
 | Copyright (C) 2006 Regents of the University of California            |
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
 * $Log: init.c,v $
 * Revision 1.32  2016/08/04 21:40:15  dechavez
 * got rid of unused EventThread and CtrlThread semaphores, added reboots counter
 *
 * Revision 1.31  2016/07/20 17:10:46  dechavez
 * support for new QDP_FSA field added to handle in qdp.h rev 1.89
 *
 * Revision 1.30  2016/06/15 21:29:27  dechavez
 * QDP_PAR_OPTION_QIO_x based debug messages added
 *
 * Revision 1.29  2016/02/03 17:35:19  dechavez
 * only the one automaton to assign to qp->fsa
 *
 * Revision 1.28  2015/12/23 20:24:04  dechavez
 * set errno to be either QDP_ERR_UDPIO or QDP_ERR_TTYIO depending on connection type
 *
 * Revision 1.27  2014/08/11 18:07:39  dechavez
 * MAJOR CHANGES TO SUPPORT Q330 DATA COMM OVER SERIAL PORT (see 8/11/2014 comments in version.c)
 *
 * Revision 1.26  2011/01/31 21:13:13  dechavez
 * set destination IP to broadcast when not using network I/O
 *
 * Revision 1.25  2011/01/25 18:22:37  dechavez
 * replaced InitIO with InitConnections(), and use QDP_CONNECT "connect"
 *
 * Revision 1.24  2011/01/14 00:25:01  dechavez
 * added InitIO to handle the details of I/O set up and to hide knowledge about
 * console connection or not
 *
 * Revision 1.23  2011/01/13 20:28:42  dechavez
 * initial state for UDP connections changed to QDP_STATE_UNREG
 *
 * Revision 1.22  2011/01/11 17:31:06  dechavez
 * all libudpio structures and library calls changed to qdpio equivalents, but
 * no support for anything other than UDP yet provided.
 *
 * Revision 1.21  2010/04/02 18:24:07  dechavez
 * set return ports to port.ctrl.value, port.data.value, set errcode to QDP_ERR_UDPIO in the event of udpioInit() failures
 *
 * Revision 1.20  2010/03/31 20:33:28  dechavez
 * get OS assigned return port numbers in udioInit() (finally!)
 *
 * Revision 1.19  2009/11/05 18:35:30  dechavez
 * new type argument for qdpInitMemBlk()
 *
 * Revision 1.18  2009/10/29 17:37:37  dechavez
 * nitialize QDP dbgpkt
 *
 * Revision 1.17  2008/10/02 22:48:30  dechavez
 * used QDP_STATUS_DATA_PORT_x instead of QDP_LOGICAL_PORT_x_STATUS
 *
 * Revision 1.16  2007/10/31 17:14:18  dechavez
 * replaced sprintf with snprintf
 *
 * Revision 1.15  2007/09/06 18:28:17  dechavez
 * include port number in udpioInit failure messages
 *
 * Revision 1.14  2007/05/18 18:00:57  dechavez
 * initial production release
 *
 */
