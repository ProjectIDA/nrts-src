#pragma ident "$Id: tokens.c,v 1.6 2011/10/13 18:21:33 dechavez Exp $"
/*======================================================================
 *
 *  Request or set DP tokens
 *
 *====================================================================*/
#include "q330cc.h"

void TokensHelp(char *name, int argc, char **argv)
{
    printf("usage: %s [ 1 2 3 4 ] [ tokenset [sta=name net=name] ] \n", name);
    printf("Naming a tokenset will configure the tokens on specified data port.\n");
    printf("The tokenset argument must match a tag entry in the lcq database.\n");
    exit(1);
}

BOOL VerifyTokens(Q330 *q330)
{
int i;
char *arg;

    q330->cmd.p32 = 0;
    q330->cmd.token.lcq = NULL;
    q330->cmd.token.sta = NULL;
    q330->cmd.token.net = NULL;

    q330->cmd.code = Q330_CMD_TOKENS;
    if (q330->cmd.arglst->count == 0) {
        q330->cmd.p32 = QDP_MEM_TYPE_CONFIG_DP1;
        return TRUE;
    }
    for (i = 0; i < q330->cmd.arglst->count; i++) {
        arg = (char *) q330->cmd.arglst->array[i];
        if (q330->cmd.p32 == 0) {
            if (strcmp(arg, "0") == 0 || strcmp(arg, "1") == 0) {
                q330->cmd.p32 = QDP_MEM_TYPE_CONFIG_DP1;
                q330->cmd.token.port = QDP_LOGICAL_PORT_1;
            } else if (strcmp(arg, "2") == 0) {
                q330->cmd.p32 = QDP_MEM_TYPE_CONFIG_DP2;
                q330->cmd.token.port = QDP_LOGICAL_PORT_2;
            } else if (strcmp(arg, "3") == 0) {
                q330->cmd.p32 = QDP_MEM_TYPE_CONFIG_DP3;
                q330->cmd.token.port = QDP_LOGICAL_PORT_3;
            } else if (strcmp(arg, "4") == 0) {
                q330->cmd.p32 = QDP_MEM_TYPE_CONFIG_DP4;
                q330->cmd.token.port = QDP_LOGICAL_PORT_4;
            } else {
                TokensHelp(q330->cmd.name, 0, NULL);
            }
        } else if (strncasecmp(arg, "sta=", strlen("sta=")) == 0) {
            q330->cmd.token.sta = arg + strlen("sta=");
        } else if (strncasecmp(arg, "net=", strlen("net=")) == 0) {
            q330->cmd.token.net = arg + strlen("net=");
        } else if (q330->cmd.token.lcq == NULL) {
            if ((q330->cmd.token.lcq = q330LookupLcq(arg, q330->cfg)) == NULL) {
                printf("No such tokenset '%s'\n", arg);
                GracefulExit(q330, 1);
            }
        } else {
            TokensHelp(q330->cmd.name, 0, NULL);
        }
    }

    return TRUE;
}

static BOOL SetTokenLcq(Q330 *q330, QDP_MEMBLK *blk, QDP_DP_TOKEN *token)
{
static UINT32 delay = 250;
static int ntry = 10;

    if (!q330SetTokenLcq(token, q330->cmd.token.lcq, q330->cmd.token.sta, q330->cmd.token.net)) {
        printf("q330SetTokenLcq failed\n");
        return FALSE;
    }
    qdpPackTokenMemblk(blk, token, q330->cmd.p32);
    printf("Loading %s tokens\n", qdpPortString(q330->cmd.token.port));
    if (!qdpPostMemBlk(q330->qdp, blk, delay, ntry, NULL)) {
        printf("qdpPostMemBlk failed\n");
        return FALSE;
    }
    if (!qdpRequestTokens(q330->qdp, token, (UINT16) q330->cmd.p32)) {
        printf("qdpRequestTokens (after reset) failed\n");
        return FALSE;
    }

    return TRUE;
}

void tokens(Q330 *q330)
{
QDP_MEMBLK blk;
QDP_DP_TOKEN token;

    if (!qdpRequestTokens(q330->qdp, &token, (UINT16) q330->cmd.p32)) {
        printf("qdpRequestTokens failed\n");
        return;
    }

    if (q330->cmd.token.lcq != NULL && !SetTokenLcq(q330, &blk, &token)) return;

    printf("%s\n", qdpMemTypeString((UINT8) q330->cmd.p32));
    printf("\n");
    qdpPrintTokenReport(stdout, &token);

    if (q330->cmd.token.lcq != NULL) {
        printf("\n");
        printf("The %s configuration change takes place immediately,\n", qdpPortString(q330->cmd.token.port));
        printf("however it will not persist across reboots until it is saved.\n");
    }
}

/*-----------------------------------------------------------------------+
 |                                                                       |
 | Copyright (C) 2009 Regents of the University of California            |
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
 * Revision 1.6  2011/10/13 18:21:33  dechavez
 * graceful exit on errors
 *
 * Revision 1.5  2011/04/14 21:01:46  dechavez
 * added sta and net options
 *
 * Revision 1.4  2011/04/12 21:00:44  dechavez
 * Added tokenset option to tokens command, to permit configuring tokens on
 * a specific data port using token sets defined in the lcq database.
 *
 * Revision 1.3  2009/11/13 00:39:20  dechavez
 * corrected error message printed when qdpRequestTokens() fails
 *
 * Revision 1.2  2009/09/28 22:15:20  dechavez
 * use qdpUnpackTokens() instead of explicit request and unpack calls
 *
 * Revision 1.1  2009/09/15 23:14:42  dechavez
 * initial release
 *
 */
