#pragma ident "$Id: b53.c,v 1.2 2015/07/15 17:07:37 dechavez Exp $"
/*======================================================================
 * 
 * Blockette 53 - Response (Poles & Zeros) Blockette
 *
 *====================================================================*/
#include "mseed.h"

static void WritePZ(FILE *fp, int num, MSEED_COMPLEX_PZ *pz)
{
int i;

    fprintf(fp, "%3d", num);
    for (i = 0; i < num; i++) {
        fprintf(fp, "%12.5E", pz[i].value.r);
        fprintf(fp, "%12.5E", pz[i].value.i);
        fprintf(fp, "%12.5E", pz[i].error.r);
        fprintf(fp, "%12.5E", pz[i].error.i);
    }
}

BOOL mseedWriteBlockette53(FILE *fp, MSEED_B53 *b53)
{
int len;

    len = 3                       /* blockette type - 053 */
        + 4                       /* length of blockette */
        + 1                       /* response type */
        + 2                       /* stage sequence number */
        + 3                       /* signal input units */
        + 3                       /* signal output units */
        + 12                      /* normalization factor */
        + 12                      /* normalization frequency */
        + 3                       /* number of zeros */
        + (b53->nzero) * 48       /* zeros (and errors) */
        + 3                       /* number of poles */
        + (b53->npole) * 48       /* poles (and errors) */
        ;

    fprintf(fp, "053");
    fprintf(fp, "%4d", len);

    fprintf(fp,       "%c", b53->type);
    fprintf(fp,      "%2d", b53->stageid);
    fprintf(fp,      "%3d", b53->iunits);
    fprintf(fp,      "%3d", b53->ounits);
    fprintf(fp,   "%12.5E", b53->a0);
    fprintf(fp,   "%12.5E", b53->freq);
    WritePZ(fp, b53->nzero, b53->zero);
    WritePZ(fp, b53->npole, b53->pole);

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
 * $Log: b53.c,v $
 * Revision 1.2  2015/07/15 17:07:37  dechavez
 * fixed len bug
 *
 * Revision 1.1  2015/07/10 17:54:01  dechavez
 * created
 *
 */
