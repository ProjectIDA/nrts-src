#pragma ident "$Id: cmds.c,v 1.13 2016/08/04 21:26:46 dechavez Exp $"
/*======================================================================
 * 
 * Simple command utilities
 *
 *====================================================================*/
#include "qdp.h"

BOOL qdpPostCmd(QDP *qp, QDP_PKT *pkt, BOOL wait)
{
BOOL result;
QDP_PKT *cmd;
MSGQ_MSG *msg;

    if (qp == NULL || pkt == NULL) {
        errno = EINVAL;
        return FALSE;
    }

    msg = msgqGet(&qp->Q.cmd.heap, MSGQ_WAIT);
    cmd = (QDP_PKT *) msg->data;
    qdpCopyPkt(cmd, pkt);
    msgqPut(&qp->Q.cmd.full, msg);
    if (wait) {
        SEM_WAIT(&qp->cmd.sem);
        return qp->cmd.ok;
    } else {
        return TRUE;
    }
}

BOOL qdpDeregister(QDP *qp, BOOL wait)
{
QDP_PKT pkt;

    qdpEncode_C1_DSRV(&pkt, qp->par.serialno);
    return qdpPostCmd(qp, &pkt, wait);
}

BOOL qdpRqflgs(QDP *qp, BOOL wait)
{
QDP_PKT pkt;

    qdpEncode_C1_RQFLGS(&pkt, (UINT16) qp->par.port.link);
    return qdpPostCmd(qp, &pkt, wait);
}

BOOL qdpCtrl(QDP *qp, UINT16 flags, BOOL wait)
{
QDP_PKT pkt;

    qdpEncode_C1_CTRL(&pkt, flags);
    return qdpPostCmd(qp, &pkt, wait);
}

BOOL qdpNoParCmd(QDP *qp, int command, BOOL wait)
{
QDP_PKT pkt;

    qdpEncode_NoParCmd(&pkt, command);
    return qdpPostCmd(qp, &pkt, wait);
}

BOOL qdpRqstat(QDP *qp, UINT32 bitmap, BOOL wait)
{
QDP_PKT pkt;

    qdpEncode_C1_RQSTAT(&pkt, bitmap);
    return qdpPostCmd(qp, &pkt, wait);
}

BOOL qdpMemCmd(QDP *qp, QDP_PKT *pkt, UINT32 delay, int ntry, FILE *fp)
{
int count = 0;

    while (count++ < ntry) {
        if (qdpPostCmd(qp, pkt, TRUE)) return TRUE;
        if (qp->cmd.err == QDP_CERR_MEM) {
            if (fp != NULL) fprintf(fp, "%s: memory operation in progress (attempt %d/%d)\n", qdpCmdString(pkt->hdr.cmd), count, ntry);
        } else {
            if (fp != NULL) fprintf(fp, "ERROR: %s\n", qdpErrString(qp->cmd.err));
            return FALSE;
        }
        utilDelayMsec(delay);
    }

    if (fp != NULL) fprintf(fp, "Abandoning %s command after %d attempts\n", qdpCmdString(pkt->hdr.cmd), ntry);
    return FALSE;
}

BOOL qdpFlushDataPort(QDP *qdp, int port, BOOL wait)
{
QDP_PKT pkt;
QDP_TYPE_C1_LOG log;

    switch (port) {
      case QDP_LOGICAL_PORT_1:
      case QDP_LOGICAL_PORT_2:
      case QDP_LOGICAL_PORT_3:
      case QDP_LOGICAL_PORT_4:
        break;
      default:
        qdp->errcode = QDP_ERR_NOSUCH;
        return FALSE;
    }

    if (!qdp_C1_RQLOG(qdp, &log, port)) return FALSE;

    log.flags |= QDP_LOG_FLAG_MASK_FLUSH_BUFFER;
    log.olddata = 0;

    qdpEncode_C1_LOG(&pkt, &log, port);
    if (!qdpPostCmd(qdp, &pkt, wait)) return FALSE;

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
 * $Log: cmds.c,v $
 * Revision 1.13  2016/08/04 21:26:46  dechavez
 * removed qdpShutdown() (moved to fsa.c)
 *
 * Revision 1.12  2016/07/20 16:46:17  dechavez
 * qdpShutdown() calls qdpDestroyHandle() (addresses previously unrecognized memory leak, if it works)
 *
 * Revision 1.11  2012/06/24 18:02:39  dechavez
 * added port value to qdpEncode_C1_LOG() in qdpFlushDataPort()
 *
 * Revision 1.10  2010/12/23 21:48:27  dechavez
 * use only symbolic values for port numbers in qdpFlushDataPort()
 *
 * Revision 1.9  2010/11/15 23:09:21  dechavez
 * added qdpFlushDataPort()
 *
 * Revision 1.8  2010/03/31 20:40:08  dechavez
 * added qdpShutdown()
 *
 * Revision 1.7  2009/11/13 00:33:18  dechavez
 * added qdpMemCmd() (for memory busy retries)
 *
 * Revision 1.6  2008/10/02 22:51:57  dechavez
 * added qdpNoParCmd, qdpRqstat
 *
 * Revision 1.5  2007/05/18 18:00:57  dechavez
 * initial production release
 *
 */
