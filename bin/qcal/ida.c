#pragma ident "$Id: ida.c,v 1.6 2016/08/26 20:24:01 dechavez Exp $"
/*======================================================================
 *
 * Generate IDA10 packets (via HLP callbacks).
 *
 *====================================================================*/
#include "qcal.h"

#define MY_MOD_ID QCAL_MOD_IDA

static gzFile *gz = Z_NULL;

static void Ida10Callback(void *arg, QDP_HLP *hlp)
{
int errnum;
static UINT32 seqno = 0;
UINT8 buf[IDA10_FIXEDRECLEN];
MSEED_RECORD record;

/* Convert the HLP into IDA10 */

    if (!qdpHlpToIDA10(buf, hlp, ++seqno)) {
        LogMsg("qdpHlpToIDA10: %s\n", strerror(errno));
        SetExitStatus(MY_MOD_ID + 1);
        return;
    }

/* Write it out if IDA10 output is desired */

    if (gz != Z_NULL && gzwrite(gz, buf, IDA10_FIXEDRECLEN) <= 0) {
        LogMsg("ERROR: gzwrite: %s", gzerror(gz, &errnum));
        SetExitStatus(MY_MOD_ID + 2);
    }

/* Hand the IDA10 packet off to the MiniSEED facility */

    GenerateMSEED(buf);
}

void CloseIDA(QCAL *qcal)
{
    qdpFlushLCQ(&qcal->lcq);
    if (gz != Z_NULL) gzclose(gz);
}

BOOL OpenIDA(QCAL *qcal, UINT32 options)
{
QDP_LCQ_PAR par;

    if (qcal->output & OUTPUT_IDA10) {
        if ((gz = gzopen(qcal->name.ida, "w")) == Z_NULL) {
            fprintf(stderr, "ERROR: gzopen: %s: %s\n", qcal->name.ida, strerror(errno));
            return FALSE;
        }
    }

    par.lp = qcal->lp;
    par.debug = qcal->debug;

    if (!qdpInitHLPRules(
        &par.rules,
        IDA10_DEFDATALEN,
        QDP_HLP_FORMAT_NOCOMP32,
        Ida10Callback,
        (void *) NULL,
        options
    )) return FALSE;

    return qdpInitLCQ(&qcal->lcq, &par);
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
 * $Log: ida.c,v $
 * Revision 1.6  2016/08/26 20:24:01  dechavez
 * removed tabs
 *
 * Revision 1.5  2015/12/07 19:03:34  dechavez
 * Only output IDA10 if selected, but in all cases hand off packet to MiniSEED generator
 *
 * Revision 1.4  2014/06/12 20:19:45  dechavez
 * use caller supplied options
 *
 * Revision 1.3  2014/01/27 18:21:15  dechavez
 * removed "strict" option for decoder (ie, will unpack OK even if tokens are missing)
 *
 * Revision 1.2  2010/04/01 20:27:59  dechavez
 * rename SetExitFlag() to SetExitStatus()
 *
 * Revision 1.1  2010/03/31 19:46:38  dechavez
 * initial release
 *
 */
