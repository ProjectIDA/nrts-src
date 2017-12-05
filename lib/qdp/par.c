#pragma ident "$Id: par.c,v 1.27 2016/07/20 17:17:41 dechavez Exp $"
/*======================================================================
 * 
 * Manipulate the parameter structure
 *
 *====================================================================*/
#include "qdp.h"

BOOL qdpValidPort(int port)
{
    switch (port) {
      case QDP_LOGICAL_PORT_1:
      case QDP_LOGICAL_PORT_2:
      case QDP_LOGICAL_PORT_3:
      case QDP_LOGICAL_PORT_4:
      case QDP_CFG_PORT:
      case QDP_SFN_PORT:
        return TRUE;
    }

    return FALSE;
}

int qdpDataPortNumber(int port)
{
    switch (port) {
      case 1: return QDP_LOGICAL_PORT_1;
      case 2: return QDP_LOGICAL_PORT_2;
      case 3: return QDP_LOGICAL_PORT_3;
      case 4: return QDP_LOGICAL_PORT_4;
    }

    return QDP_UNDEFINED_PORT;
}

/* Initilize the structure */

void qdpInitConnect(QDP_CONNECT *connect)
{
    memset(connect->ident, 0, MAXPATHLEN+1);
    connect->type = QIO_UDP;
    connect->tag = 0xFFFF;
}

void qdpInitPar(QDP_PAR *par, int linkport)
{
    if (par == NULL) return;

    MUTEX_INIT(&par->mutex);
    qdpInitConnect(&par->connect);
    par->port.base = QDP_DEFAULT_BASEPORT;
    par->port.link = linkport;
    par->port.ctrl.base = QDP_DEFAULT_CTRL_BASEPORT;
    par->port.ctrl.value = 0;
    par->port.data.base = QDP_DEFAULT_DATA_BASEPORT;
    par->port.data.value = 0;
    par->authcode = 0;
    par->serialno = QDP_UNDEFINED_SERIALNO;
    par->timeout.ctrl = QDP_DEFAULT_CTRL_TIMEOUT;
    par->timeout.data = QDP_DEFAULT_DATA_TIMEOUT;
    par->interval.retry = QDP_DEFAULT_RETRY_INTERVAL;
    par->interval.open = QDP_DEFAULT_OPEN_INTERVAL;
    par->interval.watchdog = QDP_DEFAULT_WATCHDOG_INTERVAL;
    par->hbeat.bitmap = QDP_DEFAULT_HBEAT_BITMAP;
    par->hbeat.forward = QDP_DEFAULT_HBEAT_FORWARD;
    par->debug = QDP_DEFAULT_DEBUG;
    par->user.arg = QDP_DEFAULT_USER_ARG;
    par->user.func = QDP_DEFAULT_USER_FUNC;
    par->meta.arg = QDP_DEFAULT_META_ARG;
    par->meta.func = QDP_DEFAULT_META_FUNC;
    par->options = QDP_DEFAULT_OPTIONS;

    switch (par->port.link) {
      case QDP_CFG_PORT:
      case QDP_SFN_PORT:
        par->hbeat.interval = QDP_DEFAULT_CMD_HBEAT_INTERVAL;
        par->nbuf = QDP_DEFAULT_CMD_NBUF;
        break;
      default:
        par->hbeat.interval = QDP_DEFAULT_DATA_HBEAT_INTERVAL;
        par->nbuf = QDP_DEFAULT_DATA_NBUF;
    }
}

/* Set various fields */

BOOL qdpSetConnect(QDP_PAR *par, QDP_CONNECT *connect)
{
    if (par == NULL || connect == NULL) return FALSE;
    par->connect = *connect;
    return TRUE;
}

BOOL qdpSetBaseport(QDP_PAR *par, int value)
{
    if (par == NULL) return FALSE;
    par->port.base = value;
    return TRUE;
}

BOOL qdpSetCtrlBaseport(QDP_PAR *par, int value)
{
    if (par == NULL) return FALSE;
    par->port.ctrl.base = value;
    return TRUE;
}

BOOL qdpSetDataBaseport(QDP_PAR *par, int value)
{
    if (par == NULL) return FALSE;
    par->port.data.base = value;
    return TRUE;
}

BOOL qdpSetMyCtrlPort(QDP_PAR *par, int value)
{
    if (par == NULL) return FALSE;
    par->port.ctrl.value = par->port.ctrl.base + value;
    return TRUE;
}

BOOL qdpSetMyDataPort(QDP_PAR *par, int value)
{
    if (par == NULL) return FALSE;
    par->port.data.value = par->port.data.base + value;
    return TRUE;
}

BOOL qdpSetAuthcode(QDP_PAR *par, UINT64 value)
{
    if (par == NULL) {
        errno = EINVAL;
        return FALSE;
    }
    par->authcode = value;
    return TRUE;
}

BOOL qdpSetSerialno(QDP_PAR *par, UINT64 value)
{
    if (par == NULL) {
        errno = EINVAL;
        return FALSE;
    }
    par->serialno = value;
    return TRUE;
}

BOOL qdpSetCtrlTimeout(QDP_PAR *par, UINT32 value)
{
    if (par == NULL) {
        errno = EINVAL;
        return FALSE;
    }
    par->timeout.ctrl = value;
    return TRUE;
}

BOOL qdpSetDataTimeout(QDP_PAR *par, UINT32 value)
{
    if (par == NULL) {
        errno = EINVAL;
        return FALSE;
    }
    par->timeout.data = value;
    return TRUE;
}

BOOL qdpSetRetryInterval(QDP_PAR *par, UINT32 value)
{
    if (par == NULL) {
        errno = EINVAL;
        return FALSE;
    }
    par->interval.retry = value;
    return TRUE;
}

BOOL qdpSetOpenInterval(QDP_PAR *par, UINT32 value)
{
    if (par == NULL) {
        errno = EINVAL;
        return FALSE;
    }
    par->interval.open = value;
    return TRUE;
}

BOOL qdpSetWatchdogInterval(QDP_PAR *par, UINT32 value)
{
    if (par == NULL) {
        errno = EINVAL;
        return FALSE;
    }
    par->interval.watchdog = value;
    return TRUE;
}

BOOL qdpSetHeartbeat(QDP_PAR *par, UINT32 interval, UINT32 bitmap, BOOL forward)
{
    if (par == NULL) {
        errno = EINVAL;
        return FALSE;
    }
    par->hbeat.interval = interval;
    par->hbeat.bitmap = bitmap;
    par->hbeat.forward = forward;
    return TRUE;
}

BOOL qdpSetNbuf(QDP_PAR *par, int value)
{
    if (par == NULL) {
        errno = EINVAL;
        return FALSE;
    }
    par->nbuf = value;
    return TRUE;
}

BOOL qdpSetDebug(QDP_PAR *par, int value)
{
    if (par == NULL) {
        errno = EINVAL;
        return FALSE;
    }
    if (value < QDP_TERSE) value = QDP_TERSE;
    if (value > QDP_DEBUG3) value = QDP_DEBUG3;

    switch (value) {
      case QDP_DEBUG2:
        qdpAddOption(par, QDP_PAR_OPTION_QIO_TERSE);
        value = QDP_DEBUG;
        break;
      case QDP_DEBUG3:
        qdpAddOption(par, QDP_PAR_OPTION_QIO_VERBOSE);
        value = QDP_DEBUG;
        break;
    }

    MUTEX_LOCK(&par->mutex);
        par->debug = value;
    MUTEX_UNLOCK(&par->mutex);
    return TRUE;
}

BOOL qdpSetUser(QDP_PAR *par, void *arg, QDP_USER_FUNC func)
{
    if (par == NULL) {
        errno = EINVAL;
        return FALSE;
    }
    par->user.arg = arg;
    par->user.func = func;
    return TRUE;
}

BOOL qdpSetMeta(QDP_PAR *par, void *arg, QDP_META_FUNC func)
{
    if (par == NULL) {
        errno = EINVAL;
        return FALSE;
    }
    par->meta.arg = arg;
    par->meta.func = func;
    return TRUE;
}

BOOL qdpSetOptionFlags(QDP_PAR *par, UINT32 options)
{
    if (par == NULL) {
        errno = EINVAL;
        return FALSE;
    }
    par->options = options;
    return TRUE;
}

BOOL qdpAddOption(QDP_PAR *par, UINT32 options)
{
    if (par == NULL) {
        errno = EINVAL;
        return FALSE;
    }
    par->options |= options;
    return TRUE;
}

/* Log contents */

void qdpLogPar(QDP *qp, QDP_PAR *par)
{
char *forward;

    if (par == NULL) return;

    qdpInfo(qp, "            ident = %s", par->connect.ident);
    qdpInfo(qp, "         serialno = %016llX%", par->serialno);
    qdpInfo(qp, "         authcode = %016llX%", par->authcode);
    qdpInfo(qp, "        Q330 port = %s", qdpPortString(par->port.link));
    qdpInfo(qp, "    UDP base port = %d", par->port.base);
    qdpInfo(qp, "    UDP ctrl port = %d", par->port.ctrl.value);
    qdpInfo(qp, "     ctrl timeout = %.2f sec", (float) par->timeout.ctrl / MSEC_PER_SEC);
    qdpInfo(qp, "   retry interval = %.2f sec", (float) par->interval.retry / MSEC_PER_SEC);
    if (qp->dataLink) {
        qdpInfo(qp, "    UDP data port = %d", par->port.data.value);
        qdpInfo(qp, "     data timeout = %.2f sec", (float) par->timeout.data / MSEC_PER_SEC);
        qdpInfo(qp, "    open interval = %.2f sec", (float) par->interval.open / MSEC_PER_SEC);
    }
    if (par->hbeat.interval) {
        qdpInfo(qp, "   hbeat interval = %.2f sec", (float) par->hbeat.interval / MSEC_PER_SEC);
        forward = par->hbeat.forward ? "forwarded" : "not forwarded";
        qdpInfo(qp, "    status bitmap = 0x%04x, %s", par->hbeat.bitmap, forward);
    } else {
        qdpInfo(qp, "   hbeat interval = n/a (no automatic heartbeat)");
    }
    if (par->interval.watchdog) {
        qdpInfo(qp, "handshake timeout = %.2f sec", (float) par->interval.watchdog / MSEC_PER_SEC);
    } else {
        qdpInfo(qp, "handshake timeout = n/a (disabled)");
    }
    qdpInfo(qp, "             nbuf = %d", par->nbuf);
    qdpInfo(qp, "            debug = %s", qdpDebugString(par->debug));
    qdpInfo(qp, "     pkt function = %s", par->user.func == NULL ? "disabled" : "enabled");
    qdpInfo(qp, "    meta function = %s", par->meta.func == NULL ? "disabled" : "enabled");
    qdpInfo(qp, "          options = 0x%08x", par->options);
}

/* Print contents */

void qdpPrintPar(FILE *fp, QDP_PAR *par)
{
char *forward;

    if (par == NULL) return;

    fprintf(fp, "         ident = %s\n", par->connect.ident);
    fprintf(fp, "      serialno = %016llX\n", par->serialno);
    fprintf(fp, "      authcode = %016llX\n", par->authcode);
    fprintf(fp, "     base port = %d\n", par->port.base);
    fprintf(fp, "          port = %s\n", qdpPortString(par->port.link));
    fprintf(fp, "     ctrl port = %d\n", par->port.ctrl.value);
    fprintf(fp, "  ctrl timeout = %llu\n", par->timeout.ctrl / MSEC_PER_SEC);
    fprintf(fp, "     data port = %d\n", par->port.data.value);
    fprintf(fp, "  data timeout = %llu\n", par->timeout.data / MSEC_PER_SEC);
    fprintf(fp, "retry interval = %llu\n", par->interval.retry / MSEC_PER_SEC);
    fprintf(fp, " open interval = %llu\n", par->interval.open / MSEC_PER_SEC);
    if (par->hbeat.interval) {
        fprintf(fp, "hbeat interval = %llu\n", par->hbeat.interval / MSEC_PER_SEC);
        forward = par->hbeat.forward ? "forwarded" : "not forwarded";
        fprintf(fp, " status bitmap = 0x%04x, %s\n", par->hbeat.bitmap, forward);
    } else {
        fprintf(fp, "hbeat interval = n/a (no automatic heartbeat)\n");
    }
    fprintf(fp, "          nbuf = %d\n", par->nbuf);
    fprintf(fp, "         debug = %s\n", qdpDebugString(par->debug));
    fprintf(fp, "  pkt function = %s\n", par->user.func == NULL ? "disabled" : "enabled");
    fprintf(fp, " meta function = %s\n", par->meta.func == NULL ? "disabled" : "enabled");
    fprintf(fp, "       options = 0x%08x", par->options);
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
 * $Log: par.c,v $
 * Revision 1.27  2016/07/20 17:17:41  dechavez
 * removed qdpSetMaxtry(), added qdpSetWatchdogInterval() and related stuff
 * stemming from replacing maxtry with watchdog interval
 *
 * Revision 1.26  2016/06/15 21:27:10  dechavez
 * overloaded qdpSetDebug() to use new QDP_DEBUGx states to set QDP_PAR_OPTION_QIO_x options
 *
 * Revision 1.25  2016/02/11 19:02:20  dechavez
 * changed maxresp to maxtry, qdpSetMaxsrq() to qdpSetMaxtry()
 *
 * Revision 1.24  2016/02/05 17:03:48  dechavez
 * fixed up some format string vs. data type discrepancies
 *
 * Revision 1.23  2016/02/05 16:48:52  dechavez
 * introduced qdpSetMaxsrq(), include 'maxsrq' and 'options' to qdpLogPar() and qdpPrintPar() output
 *
 * Revision 1.22  2015/12/04 23:15:12  dechavez
 * casts, format fixes and the like, all to calm OS X compiles
 *
 * Revision 1.21  2011/01/25 18:18:36  dechavez
 * added qdpInitConnect(), qdpSetConnect(), removed qdpSetIdent()
 *
 * Revision 1.20  2011/01/14 00:23:08  dechavez
 * par "host" to "ident", qdpSetHost() to qdpSetIdent()
 *
 * Revision 1.19  2010/12/23 21:45:41  dechavez
 * dded qdpValidPort()
 *
 * Revision 1.18  2010/12/22 22:55:11  dechavez
 * added qdpDataPortNumber()
 *
 * Revision 1.17  2010/10/20 18:33:09  dechavez
 * added qdpAddOption(), removed unused arg arg from qdpSetOptionFlags()
 *
 * Revision 1.16  2010/04/02 18:22:20  dechavez
 * restored qdpSetMyCtrlPort() and qdpSetMyDataPort().  Changed the default
 * vaues for port.ctrl.value and port.data.value to 0 to still permit OS
 * assigned return ports, but also have the ability to assign predictable
 * ports for those applications that want it (ie, isidl)
 *
 * Revision 1.15  2010/03/31 20:30:23  dechavez
 * removed trash link parameters and functions, removed unused qdpParseArgString(),
 * qdpSetMyCtrlPort() and qdpSetMyDataPort() functions, changed qdpLogPar() and
 * qdpPrintPar() to take QDP_PAR as the argument
 *
 * Revision 1.14  2010/03/22 21:35:14  dechavez
 * added qdpSetOptionFlags(), defined options flag in qdpInitPar()
 *
 * Revision 1.13  2007/05/18 18:00:57  dechavez
 * initial production release
 *
 */
