#pragma ident "$Id: connect.c,v 1.11 2016/07/20 16:48:40 dechavez Exp $"
/*======================================================================
 * 
 * High level intialization
 *
 *====================================================================*/
#include "qdp.h"

QDP *qdpConnect(QDP_PAR *par, LOGIO *lp, int *errcode, int *suberr)
{
QDP *qp;
static char *fid = "qdpConnect";

    if (errcode != NULL) *errcode = QDP_ERR_NO_ERROR;
    if (suberr  != NULL) *suberr = QDP_CERR_NOERR;

    if (par == NULL) {
        if (errcode != NULL) *errcode = QDP_ERR_INVALID;
        return NULL;
    }

    if ((qp = (QDP *) malloc(sizeof(QDP))) == NULL) {
        if (errcode != NULL) *errcode = QDP_ERR_NOMEM;
        return NULL;
    }

    if (!qdpInit(qp, par, lp)) {
        if (errcode != NULL) *errcode = qp->errcode;
        free(qp);
        return NULL;
    }

    qdpLogPar(qp, par);

    if (!qdpStartFSA(qp)) {
        if (errcode != NULL) *errcode = qp->errcode;
        if (suberr != NULL) *suberr = qp->suberr;
        free(qp);
        return NULL;
    }

    return qp;

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
 * $Log: connect.c,v $
 * Revision 1.11  2016/07/20 16:48:40  dechavez
 * added suberr argument to qdpConnect()
 *
 * Revision 1.10  2011/01/14 00:26:14  dechavez
 * removed old qdpConnect() and gave name to qdpConnectWithPar()
 *
 * Revision 1.9  2010/03/31 21:39:44  dechavez
 * moved qdpLogPar() to after qdpInit() (par->mutex is not yet initialized otherwise... duh)
 *
 * Revision 1.8  2010/03/31 20:39:39  dechavez
 * moved qdpLogPar to before qdpInit
 *
 * Revision 1.7  2010/03/22 21:39:01  dechavez
 * added errcode arg to qdpConnect(), qdpConnectWithPar()
 *
 * Revision 1.6  2007/05/18 18:00:57  dechavez
 * initial production release
 *
 */
