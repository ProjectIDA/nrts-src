#pragma ident "$Id: b54.c,v 1.1 2015/07/10 17:54:01 dechavez Exp $"
/*======================================================================
 * 
 * Blockette 54 - Response (Coefficients) Blockette
 *
 *====================================================================*/
#include "mseed.h"

static void WriteCoeff(FILE *fp, int num, MSEED_COEFF *coeff)
{
int i;

    for (i = 0; i < num; i++) {
        fprintf(fp, "%12.5E", coeff[i].value);
        fprintf(fp, "%12.5E", coeff[i].error);
    }
}

BOOL mseedWriteBlockette54(FILE *fp, MSEED_B54 *b54)
{
int len;

    len = 3                       /*  1) blockette type - 054 */
        + 4                       /*  2) length of blockette */
        + 1                       /*  3) response type */
        + 2                       /*  4) stage sequence number */
        + 3                       /*  5) signal input units */
        + 3                       /*  6) signal output units */
        + 4                       /*  7) number of numerators */
        + (b54->nnum) * 12        /*  8) numerators */
        + (b54->nnum) * 12        /*  9) numerator errors */
        + 4                       /* 10) number of denomenators */
        + (b54->nden) * 12        /* 11) denomenators */
        + (b54->nden) * 12        /* 12) denomenator errors */
        ;

    fprintf(fp, "054");
    fprintf(fp, "%4d", len);

    fprintf(fp,     "%c", b54->type);
    fprintf(fp,    "%2d", b54->stageid);
    fprintf(fp,    "%3d", b54->iunits);
    fprintf(fp,    "%3d", b54->ounits);
    fprintf(fp,    "%4d", b54->nnum); WriteCoeff(fp, b54->nnum, b54->num);
    fprintf(fp,    "%4d", b54->nden); WriteCoeff(fp, b54->nden, b54->den);

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
 * $Log: b54.c,v $
 * Revision 1.1  2015/07/10 17:54:01  dechavez
 * created
 *
 */
