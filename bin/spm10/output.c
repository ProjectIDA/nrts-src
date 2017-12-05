#pragma ident "$Id: output.c,v 1.1 2016/04/28 23:01:50 dechavez Exp $"
/*======================================================================
 *
 *  Handle output, either to a remote disk loop server or stdout
 *
 *====================================================================*/
#define INCLUDE_IACP_DEFAULT_ATTR
#include "spm10.h"

static ISI_PUSH *ph = NULL;

static BOOL ParseServerPortDepth(char **server, int *port, int *depth, char *arg)
{
#define MAX_TOKEN 3
#define MIN_TOKEN 2
char *token[MAX_TOKEN];
#define DELIMITERS ":"
int ntoken;

    if ((ntoken = utilParse(arg, token, DELIMITERS, MAX_TOKEN, 0)) < MIN_TOKEN) {
        fprintf(stderr, "bad server:port[:depth] string '%s'\n", arg);
        return FALSE;
    }

    if ((*server = strdup(token[0])) == NULL) {
        perror("strdup");
        return FALSE;
    }

    if ((*port = atoi(token[1])) <= 0) {
        fprintf(stderr, "illegal port number from '%s'\n", arg);
        return FALSE;
    }

    if (ntoken == MAX_TOKEN) {
        if ((*depth = atoi(token[MAX_TOKEN-1])) <= 0) {
            fprintf(stderr, "illegal depth from '%s'\n", arg);
            return FALSE;
        }
    }

    return TRUE;
}

BOOL SetDlOutput(char *string, LOGIO *lp)
{
BOOL block = FALSE;
char *server;
int port = 0;
int depth = DEFAULT_DEPTH;
IACP_ATTR attr = IACP_DEFAULT_ATTR;

    if (!ParseServerPortDepth(&server, &port, &depth, string)) return FALSE;

    if ((ph = isiPushInit(server, port, &attr, lp, LOG_INFO, IDA10_FIXEDRECLEN, depth, block)) == NULL) {
        perror("isiPushInit");
        return FALSE;
    }

    return TRUE;
}

void FlushPacket(PROTO_PACKET *pkt)
{
    if (pkt->nsamp > 0) {
        utilPackINT16(&pkt->buf[pkt->offset.nsamp], pkt->nsamp);
        if (ph == NULL) {
            fwrite(pkt->buf, 1, IDA10_FIXEDRECLEN, stdout);
        } else {
            isiPushRawPacket(ph, pkt->buf, IDA10_FIXEDRECLEN, ISI_TYPE_IDA10);
        }
    }
    ResetPacket(pkt);
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
 * $Log: output.c,v $
 * Revision 1.1  2016/04/28 23:01:50  dechavez
 * initial release
 *
 */
