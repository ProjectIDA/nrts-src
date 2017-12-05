#pragma ident "$Id: register.c,v 1.14 2016/08/24 22:34:52 dechavez Exp $"
/*======================================================================
 * 
 * Connect to a Q330
 *
 *====================================================================*/
#include "q330.h"

QDP *q330Register(Q330_ADDR *addr, int port, int debug, LOGIO *lp, int *errcode, int *suberr)
{
QDP *qdp;
QDP_PAR par;
static char *fid = "q330Register";

    if (addr == NULL) {
        errno = EINVAL;
        return NULL;
    }

    if (!qdpValidPort(port)) {
        errno = EINVAL;
        return NULL;
    }

    qdpInitPar(&par, port);
    qdpSetConnect(&par, &addr->connect);
    qdpSetSerialno(&par, addr->serialno);
    qdpSetAuthcode(&par, addr->authcode);
    qdpSetWatchdogInterval(&par, addr->watchdog);
    qdpSetDebug(&par, debug);

    if ((qdp = qdpConnect(&par, lp, errcode, suberr)) == NULL) return NULL;
    addr->serialno = qdp->par.serialno;

    return qdp;
}

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
 * $Log: register.c,v $
 * Revision 1.14  2016/08/24 22:34:52  dechavez
 * don't log qdpConnect() failures (leave that up to the caller)
 *
 * Revision 1.13  2016/08/15 19:57:05  dechavez
 * log qdpConnect() failure messages
 *
 * Revision 1.12  2016/08/04 22:06:55  dechavez
 * set QDP_PAR_OPTION_QUICK_EXITS option
 *
 * Revision 1.11  2016/07/20 22:04:39  dechavez
 * Removed the 3.6.12 hack prompted by pre-libqdp 3.13.0 state machines.
 * Added suberr argument for compatibility libqdp 3.13.0 version of qdpConnect().
 *
 * Revision 1.10  2016/06/23 20:08:23  dechavez
 * added *temporary* hack to q330Register() to avoid automaton infinite loop
 * when attempting to register with a data port that doesn't produce data
 * (detected by checking for no buffer memory)
 *
 * Revision 1.9  2016/02/11 19:08:07  dechavez
 * changed maxsrq to maxtry (to follow libqdp 3.11.3 names)
 *
 * Revision 1.8  2016/02/05 16:54:58  dechavez
 * added initialization for new Q330_ADDR maxsrq field in q330Register()
 *
 * Revision 1.7  2011/02/01 19:45:08  dechavez
 * update calling addr with serial number after registration
 *
 * Revision 1.6  2011/01/25 18:07:26  dechavez
 * qdpSetConnect() replaces qdpSetIdent()
 *
 * Revision 1.5  2011/01/14 00:28:53  dechavez
 * qdpSetHost() to qdpSetIdent(), qdpConnectWithPar() to qdpConnect()
 *
 * Revision 1.4  2010/12/23 21:44:31  dechavez
 * use qdpValidPort() to test for input validity
 *
 * Revision 1.3  2010/03/31 20:23:42  dechavez
 * don't set client side port numbers (qdpSetMyXPort functions gone)
 *
 * Revision 1.2  2010/03/22 21:43:07  dechavez
 * added errcode arg required by libqdp 1.11.0
 *
 * Revision 1.1  2007/10/31 17:16:31  dechavez
 * initial release
 *
 */
