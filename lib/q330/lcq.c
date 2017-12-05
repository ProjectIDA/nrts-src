#pragma ident "$Id: lcq.c,v 1.3 2012/06/24 18:20:12 dechavez Exp $"
/*======================================================================
 * 
 * Logical channel queue utilities
 *
 *====================================================================*/
#include "q330.h"

BOOL q330SetTokenLcq(QDP_DP_TOKEN *token, Q330_LCQ *input, char *sta, char *net)
{
LNKLST_NODE *crnt;
QDP_TOKEN_LCQ token_lcq;
UINT8 count = 0;
Q330_LCQ_ENTRY *entry;

    if (sta != NULL) {
        strlcpy(token->site.sname, sta, QDP_SNAME_LEN+1);
        util_ucase(token->site.sname);
    }

    if (net != NULL) {
        strlcpy(token->site.nname, net, QDP_NNAME_LEN+1);
        util_ucase(token->site.nname);
    }

    strlcpy(token->setname, input->name, QDP_MAX_TOKENSET_NAME);
    listClear(&token->lcq);

    crnt = listFirstNode(input->list);
    while (crnt != NULL) {
        entry = (Q330_LCQ_ENTRY *) crnt->payload;
        strlcpy(token_lcq.loc, entry->loc, QDP_LNAME_LEN+1);
        strlcpy(token_lcq.chn, entry->chn, QDP_CNAME_LEN+1);
        token_lcq.ident.code = count++; token_lcq.ident.name[0] = 0;
        token_lcq.src[0] = entry->src[0];
        token_lcq.src[1] = entry->src[1];
        token_lcq.rate = entry->rate;
        qdpConvertSampleRate(entry->rate, &token_lcq.frate, &token_lcq.dsint, &token_lcq.nsint);
        token_lcq.options = 0;
        if (!listAppend(&token->lcq, &token_lcq, sizeof(QDP_TOKEN_LCQ))) {
            listClear(&token->lcq);
            return FALSE;
        }
        crnt = listNextNode(crnt);
    }

    if (!listSetArrayView(&token->lcq)) return FALSE;

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
 * $Log: lcq.c,v $
 * Revision 1.3  2012/06/24 18:20:12  dechavez
 * q330SetTokenLcq() sets derived frate, dsint, sint fields with qdpConvertSampleRate()
 *
 * Revision 1.2  2011/04/14 19:26:25  dechavez
 * moved q330LcqList() to lookup.c:q330LookcupLcq(), added sta, net support to q330SetTokenLcq()
 *
 * Revision 1.1  2011/04/12 20:49:36  dechavez
 * created
 *
 */
