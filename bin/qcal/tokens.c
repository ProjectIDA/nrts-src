#pragma ident "$Id: tokens.c,v 1.4 2016/09/01 17:08:34 dechavez Exp $"
/*======================================================================
 *
 *  Set or restore data port configuration
 *
 *====================================================================*/
#include "qcal.h"

static UINT32 delay = 250;
static int ntry = 10;

BOOL ConfigureDataPort(QDP *ConfigPortConnection, QCAL *qcal)
{
int errcode, suberr;
UINT16 type;
Q330_LCQ *lcq;
static char *fid = "ConfigureDataPortTokens";

    if (qcal->dport.tokens == NULL) {
        LogMsg("%s configuration left unchanged\n", qdpPortString(qcal->port));
        return TRUE;
    }

    switch (qcal->port) {
      case QDP_LOGICAL_PORT_1: type = QDP_MEM_TYPE_CONFIG_DP1; break;
      case QDP_LOGICAL_PORT_2: type = QDP_MEM_TYPE_CONFIG_DP2; break;
      case QDP_LOGICAL_PORT_3: type = QDP_MEM_TYPE_CONFIG_DP3; break;
      case QDP_LOGICAL_PORT_4: type = QDP_MEM_TYPE_CONFIG_DP4; break;
      default:
        fprintf(stderr, "*** UNEXPECTED ERROR *** port %d is not a valid data port!\n", qcal->port);
        return FALSE;
    }

    if ((lcq = q330LookupLcq(qcal->dport.tokens, qcal->cfg)) == NULL) {
        fprintf(stderr, "ERROR: no such tokenset '%s'\n", qcal->dport.tokens);
        return FALSE;
    }

/* Register with the configuration port */

/* Save a copy of the exisiting configuration */

    if (!qdp_C1_RQFIX(ConfigPortConnection, &qcal->fix)) {
        fprintf(stderr, "ERROR: %s: qdp_C1_RQFIX: %s \n", fid, strerror(errno));
        return FALSE;
    }

    if (!qdpRequestDportConfig(ConfigPortConnection, &qcal->dport.saved, qcal->port)) {
        fprintf(stderr, "ERROR: %s: qdpRequestDportConfig: %s \n", fid, strerror(errno));
        return FALSE;
    }

/* Get a working copy to modify */

    if (!qdpRequestDportConfig(ConfigPortConnection, &qcal->dport.work, qcal->port)) {
        fprintf(stderr, "ERROR: %s: qdpRequestDportConfig: %s \n", fid, strerror(errno));
        return FALSE;
    }

/* Modify the tokens and update the configuration */

    if (!q330SetTokenLcq(&qcal->dport.work.token, lcq, qcal->dport.sta, qcal->dport.net)) {
        fprintf(stderr, "ERROR: %s: q330SetTokenLcq: %s \n", fid, strerror(errno));
        return FALSE;
    }

    if (!qdpConfigureDataPort(ConfigPortConnection, &qcal->dport.work, qcal->port, &qcal->fix, delay, ntry)) {
        fprintf(stderr, "ERROR: %s: qdpConfigureDataPort: %s \n", fid, strerror(errno));
        return FALSE;
    }

/* Success */

    LogMsg("%s configured with %s tokens\n", qdpPortString(qcal->port), qcal->dport.tokens);
    return TRUE;
}

BOOL RestoreDataPort(QDP *ConfigPortConnection, QCAL *qcal)
{
QDP *qdp;
int errcode, suberr;
static char *fid = "RestoreDataPort";

    if (qcal->dport.tokens == NULL) return TRUE; /* never changed */

    if (!qdpConfigureDataPort(ConfigPortConnection, &qcal->dport.saved, qcal->port, &qcal->fix, delay, ntry)) {
        fprintf(stderr, "ERROR: %s: qdpConfigureDataPort: %s \n", fid, strerror(errno));
        return FALSE;
    }

    LogMsg("%s restored\n", qdpPortString(qcal->port));
    return TRUE;
}

/*-----------------------------------------------------------------------+
 |                                                                       |
 | Copyright (C) 2011 Regents of the University of California            |
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
 * $Log: tokens.c,v $
 * Revision 1.4  2016/09/01 17:08:34  dechavez
 * don't call q330Register() directly anymore, instead use previously opened
 * ConfigPortConnection handle passed via arguments
 *
 * Revision 1.3  2016/08/04 22:30:39  dechavez
 * libqdp 3.13.1 and libq330 3.7.1 changes (basically added regerr to qdpConnect and q330Register calls)
 *
 * Revision 1.2  2012/06/24 18:28:56  dechavez
 * Modified to use libqdp 3.4.0 where data port and tokens are bound together
 * and main channel output frequencies are forced to agree with token set
 *
 * Revision 1.1  2011/04/14 19:22:35  dechavez
 * created
 *
 */
