#pragma ident "$Id: b33.c,v 1.3 2015/09/30 20:16:18 dechavez Exp $"
/*======================================================================
 * 
 * Blockette 33 - Generic Abbreviation Blockette
 *
 *====================================================================*/
#include "mseed.h"

LNKLST *mseedDestroyBlockette33List(LNKLST *list)
{
LNKLST_NODE *crnt;
MSEED_B33 *b33;

    if (list == NULL) return NULL;
    listDestroy(list);
    return NULL;
}

BOOL mseedWriteBlockette33(FILE *fp, MSEED_B33 *b33)
{
int i, len, nkeys;

    len = 3                       /* type */
        + 4                       /* length */
        + 3                       /* code */
        + strlen(b33->desc)       /* variable length description */
        + 1                       /* tilde */
        ;

    fprintf(fp, "033");
    fprintf(fp, "%4d", len);
    fprintf(fp, "%3d", b33->code);
    fprintf(fp, "%s~", b33->desc);
    fprintf(fp, "\n");

    return TRUE;
}

BOOL mseedWriteBlockette33List(FILE *fp, LNKLST *list)
{
LNKLST_NODE *crnt;

    if (fp == NULL || list == NULL) {
        errno = EINVAL;
        return FALSE;
    }

    crnt = listFirstNode(list);
    while (crnt != NULL) {
        mseedWriteBlockette33(fp, (MSEED_B33 *) crnt->payload);
        crnt = listNextNode(crnt);
    }

    return TRUE;
}

/*-----------------------------------------------------------------------+
 |                                                                       |
 | Copyright (C) 2014 Regents of the University of California            |
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
 * $Log: b33.c,v $
 * Revision 1.3  2015/09/30 20:16:18  dechavez
 * removed dead code (AddBlockette33() no longer being used)
 *
 * Revision 1.2  2015/07/15 17:02:04  dechavez
 * remove leading zeros on integers to improve readability
 *
 * Revision 1.1  2014/10/29 21:27:12  dechavez
 * created
 *
 */
