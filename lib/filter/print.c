#pragma ident "$Id: print.c,v 1.3 2015/11/04 23:15:46 dechavez Exp $"
/*======================================================================
 *
 *  Print FILTERs
 *
 *====================================================================*/
#include "filter.h"

static void PrintPandZ(FILE *fp, FILTER_PANDZ *pandz)
{
int i;

    fprintf(fp, "%-3d     # number of zeros\n", pandz->nzero);
    fprintf(fp, "%-3d     # number of poles\n", pandz->npole);
    fprintf(fp, "\n");
    fprintf(fp, "# zeros\n");
    for (i = 0; i < pandz->nzero; i++) fprintf(fp, "%12.5E, %12.5E\n", pandz->zero[i].r, pandz->zero[i].i);
    fprintf(fp, "\n");
    fprintf(fp, "# poles\n");
    for (i = 0; i < pandz->npole; i++) fprintf(fp, "%12.5E, %12.5E\n", pandz->pole[i].r, pandz->pole[i].i);
}

static void PrintCoeff(FILE *fp, FILTER_COEFF *coeff)
{
int i;

    fprintf(fp, "%-3d     # number of coefficients\n", coeff->ncoef);
    if (coeff->delay != 0.0) {
        fprintf(fp, "%7.4lf # group delay\n", coeff->delay);
    } else {
        fprintf(fp, "0.0     # group delay\n");
    }
    fprintf(fp, "\n");
    for (i = 0; i < coeff->ncoef; i++) fprintf(fp, "%12.5E\n", coeff->coef[i]);
}

void filterPrint(FILE *fp, FILTER *filter)
{
    if (fp == NULL || filter == NULL) return;

    fprintf(fp, "0x%04x  # type 0x%04x = %s\n", filter->type, filter->type, filterTypeString(filter->type));

    if (filter->type & FILTER_TYPE_PANDZ) {
        PrintPandZ(fp, &filter->data.pz);
    } else if (filter->type & FILTER_TYPE_COEFF) {
        PrintCoeff(fp, &filter->data.cf);
    }
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
 * $Log: print.c,v $
 * Revision 1.3  2015/11/04 23:15:46  dechavez
 * removed extraneous (and benign) printf argument in PrintCoeff()
 *
 * Revision 1.2  2015/08/24 18:41:55  dechavez
 * change printf format from %lf to %12.5E (compatible with SEED)
 *
 * Revision 1.1  2015/07/15 17:29:40  dechavez
 * created
 *
 */
