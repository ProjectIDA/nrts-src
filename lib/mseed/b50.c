#pragma ident "$Id: b50.c,v 1.4 2015/07/15 17:03:38 dechavez Exp $"
/*======================================================================
 * 
 * Blockette 50 - Station Identifier Blockette
 *
 *====================================================================*/
#include "mseed.h"

BOOL mseedWriteBlockette50(FILE *fp, MSEED_B50 *b50)
{
int i, len, nkeys;
char beg[MSEED_TIMELEN+1], end[MSEED_TIMELEN+1];

    mseedSetTIMEstring(beg, b50->beg, TRUE);
    mseedSetTIMEstring(end, b50->end, TRUE);

    len = 3                       /* type */
        + 4                       /* length */
        + MSEED_B50_STA_LEN       /* name */
        + 10                      /* latitude */
        + 11                      /* longitude */
        + 7                       /* elevation */
        + 4                       /* number of channels */
        + 3                       /* number of station comments */
        + strlen(b50->desc)       /* variable length description */
        + 1                       /* tilde */
        + 3                       /* network code (numeric) */
        + 4                       /* 32-bit word order string */
        + 2                       /* 16-bit word order string */
        + strlen(beg)             /* start effective date */
        + 1                       /* tilde */
        + strlen(end)             /* end effective date */
        + 1                       /* tilde */
        + 1                       /* update flag */
        + MSEED_B50_NET_LEN       /* network code (string) */
        ;

    fprintf(fp, "050");
    fprintf(fp, "%4d", len);
    fprintf(fp, "%-5s", b50->name);
    fprintf(fp, "%10.6lf", b50->lat);
    fprintf(fp, "%11.6lf", b50->lon);
    fprintf(fp, "%7.1lf", b50->elev);
    fprintf(fp, "    "); /* number of channels, blank as per SEED recommendation */
    fprintf(fp, "   ");  /* number of station comments, blank as per SEED recommendation */
    fprintf(fp, "%-s~", b50->desc);
    fprintf(fp, "%3d", b50->code);
    fprintf(fp, "%-4s", MSEED_32_BIT_WORD_ORDER);
    fprintf(fp, "%-2s", MSEED_16_BIT_WORD_ORDER);
    fprintf(fp, "%s~", beg);
    fprintf(fp, "%s~", end);
    fprintf(fp, "%c",  b50->update);
    fprintf(fp, "%-2s",  b50->net);
    fprintf(fp, "\n");

    return ferror(fp) ? FALSE : TRUE;
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
 * $Log: b50.c,v $
 * Revision 1.4  2015/07/15 17:03:38  dechavez
 * fixed len bug, added missing tildes
 *
 * Revision 1.3  2015/07/10 17:52:39  dechavez
 * remove leading zeros on integers to improve readability
 * check for success before returning TRUE
 *
 * Revision 1.2  2015/06/30 19:16:38  dechavez
 * changed printf to use %lf for now REAL64 lat/lon/elev, add full=TRUE to mseedSetTIMEstring() call
 *
 * Revision 1.1  2014/10/29 21:27:12  dechavez
 * created
 *
 */
