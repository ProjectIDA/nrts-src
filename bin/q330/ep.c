#pragma ident "$Id: ep.c,v 1.6 2017/10/11 23:16:34 dechavez Exp $"
/*======================================================================
 *
 *  Q330 Environmental processor control
 *
 *====================================================================*/
#include "q330cc.h"

void EpHelp(char *name, int argc, char **argv)
{
    printf("\n");
    printf("usage: %s { status | cnf | delay }\n", name);
    printf("\n");
    printf("status - environmental processor status (equivalent to the 'status ep' command)\n");
    printf("   cnf - print environmental processor configuration\n");
    printf(" delay - print environmental processor filter delays\n");
    printf("\n");
}

BOOL VerifyEp(Q330 *q330)
{
char *arg;

    if (q330->cmd.arglst->count != 1) return BadArgLstCount(q330);

    arg = (char *) q330->cmd.arglst->array[0];
    if (strcasecmp(arg, "status") == 0) {
        q330->cmd.code = Q330_CMD_STATUS;
        q330->cmd.p32 = QDP_SRB_EP;
        q330->cmd.update = FALSE;
    } else if (strcasecmp(arg, "cnf") == 0 || strcasecmp(arg, "cfg") == 0) {
        q330->cmd.code = Q330_CMD_EP_CNF;
        q330->cmd.update = FALSE;
    } else if (strcasecmp(arg, "delay") == 0) {
        q330->cmd.code = Q330_CMD_EP_DELAY;
        q330->cmd.update = FALSE;
    } else if (strcasecmp(arg, "help") == 0) {
        EpHelp(q330->cmd.name, 0, NULL);
        return FALSE;
    } else {
        return UnexpectedArg(q330, arg);
    }

    return TRUE;
}

void EpDelay(Q330 *q330)
{
    QDP_TYPE_C2_EPD epd;

    if (q330->qdp->flags & QDP_FLAGS_EP_SUPPORTED) {

        if (!qdp_C2_RQEPD(q330->qdp, &epd)) {
            printf("qdp_C2_RQEPD failed\n");
            return;
        }

        qdpPrint_C2_EPD(stdout, &epd);

    } else {
        qdpPrintUnsupportedQEP(stdout, q330->qdp);
    }
}

void EpCnf(Q330 *q330)
{
QDP_PKT pkt;
QDP_DP_TOKEN token;
QDP_TYPE_C2_EPCFG epcfg;

    if (q330->qdp->flags & QDP_FLAGS_EP_SUPPORTED) {

        if (!qdpRequestTokens(q330->qdp, &token, QDP_MEM_TYPE_CONFIG_DP1)) {
            printf("qdpRequestTokens failed\n");
            return;
        }

        if (!qdp_C2_RQEPCFG(q330->qdp, &epcfg)) {
            printf("qdp_C2_RQEPCFG failed\n");
        } else {
            qdpPrint_C2_EPCFG(stdout, &epcfg, &token);
        }

    } else {
        qdpPrintUnsupportedQEP(stdout, q330->qdp);
    }
}

/*-----------------------------------------------------------------------+
 |                                                                       |
 | Copyright (C) 2016 Regents of the University of California            |
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
 * $Log: ep.c,v $
 * Revision 1.6  2017/10/11 23:16:34  dechavez
 * sigh.  Didn't test my previous commit ('cause it was "trivial").  Works now.
 *
 * Revision 1.5  2017/10/11 22:44:01  dechavez
 * Print "library standard" message when reporting on EP firmware incompatibility
 *
 * Revision 1.4  2017/09/28 20:41:08  dauerbach
 * added version checks for EP commands
 *
 * Revision 1.3  2016/01/23 00:23:08  dechavez
 * include tokens in "ep cfg" output, added "ep delay" command
 *
 * Revision 1.2  2016/01/21 19:38:11  dechavez
 * made "help" a valid option
 *
 * Revision 1.1  2016/01/21 18:56:06  dechavez
 * created
 *
 */
