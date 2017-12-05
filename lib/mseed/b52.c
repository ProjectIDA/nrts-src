#pragma ident "$Id: b52.c,v 1.5 2015/08/24 18:59:19 dechavez Exp $"
/*======================================================================
 * 
 * Blockette 52 - Channel Identifier Blockette
 *
 *====================================================================*/
#include "mseed.h"

BOOL mseedWriteBlockette52(FILE *fp, MSEED_B52 *b52)
{
int i, len, nkeys;
char beg[MSEED_TIMELEN+1], end[MSEED_TIMELEN+1];

    mseedSetTIMEstring(beg, b52->beg, TRUE);
    mseedSetTIMEstring(end, b52->end, TRUE);

    len = 3                       /* blockette type - 052 */
        + 4                       /* length of blockette */
        + MSEED_B52_LOC_LEN       /* location identifier */
        + MSEED_B52_CHN_LEN       /* channel identifier */
        + 4                       /* subchannel identifier */
        + 3                       /* instrument identifier */
        + strlen(b52->comment)    /* variable length comment */
        + 1                       /* tilde */
        + 3                       /* units of signal response */
        + 3                       /* units of calibration input */
        + 10                      /* latitude */
        + 11                      /* longitude */
        + 7                       /* elevation */
        + 5                       /* local depth */
        + 5                       /* azimuth */
        + 5                       /* dip */
        + 4                       /* data format identifer code */
        + 2                       /* data record length */
        + 10                      /* sample rate */
        + 10                      /* max clock drift */
        + 4                       /* number of comments */
        + strlen(b52->flag)       /* channel flags */
        + 1                       /* tilde */
        + strlen(beg)             /* start date */
        + 1                       /* tilde */
        + strlen(end)             /* end date */
        + 1                       /* tilde */
        + 1                       /* update flag */
        ;

    fprintf(fp, "052");
    fprintf(fp, "%4d", len);

    fprintf(fp,    "%-2s", b52->loc);
    fprintf(fp,    "%-3s", b52->chn);
    fprintf(fp,    "%4d", b52->subchn);
    fprintf(fp,    "%3d", b52->instid);
    fprintf(fp,    "%-s~", b52->comment);
    fprintf(fp,    "%3d", b52->runitid);
    fprintf(fp,    "%3d", b52->cunitid);
    fprintf(fp, "%10.6lf", b52->lat);
    fprintf(fp, "%11.6lf", b52->lon);
    fprintf(fp,  "%7.1lf", b52->elev);
    fprintf(fp,  "%5.1lf", b52->depth);
    fprintf(fp,  "%5.1lf", b52->azimuth);
    fprintf(fp,  "%5.1lf", b52->dip);
    fprintf(fp,    "%4d", b52->dfcode);
    fprintf(fp,    "%2d", b52->drlen);
    fprintf(fp,  "%10.4E", b52->srate);
    fprintf(fp,  "%10.4E", b52->maxdrift);
    fprintf(fp,   "    "); /* number of channel comment blockettes, blank as per SEED recommendation */
    fprintf(fp,    "%-s~", b52->flag);
    fprintf(fp,     "%s~", beg);
    fprintf(fp,     "%s~", end);
    fprintf(fp,      "%c", b52->update);

    fprintf(fp, "\n");

    return ferror(fp) ? FALSE : TRUE;
}

/*-----------------------------------------------------------------------+
 |                                                                       |
 | Copyright (C) 2015 Regents of the University of California            |
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
 * $Log: b52.c,v $
 * Revision 1.5  2015/08/24 18:59:19  dechavez
 * just print dip as it comes, no conversion
 *
 * Revision 1.4  2015/07/16 19:49:02  dechavez
 * convert 0 to 180 dips to -90 to 90 (might be the right thing)
 *
 * Revision 1.3  2015/07/15 17:04:01  dechavez
 * fixed len bug
 *
 * Revision 1.2  2015/07/10 17:53:31  dechavez
 * remove leading zeros on integers to improve readability
 * check for success before returning TRUE
 *
 * Revision 1.1  2015/06/30 19:09:45  dechavez
 * created
 *
 */
