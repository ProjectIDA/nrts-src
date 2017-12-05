#pragma ident "$Id: b34.c,v 1.3 2015/09/08 19:22:55 dechavez Exp $"
/*======================================================================
 * 
 * Blockette 34 - Units Abbreviation Blockette
 *
 *====================================================================*/
#include "mseed.h"

BOOL mseedWriteBlockette34(FILE *fp, MSEED_B34 *b34)
{
int i, len, nkeys;

    len = 3                       /* type */
        + 4                       /* length */
        + 3                       /* code */
        + strlen(b34->unit)       /* unit */
        + 1                       /* tilde */
        + strlen(b34->desc)       /* variable length description */
        + 1                       /* tilde */
        ;

    fprintf(fp, "034");
    fprintf(fp, "%4d", len);
    fprintf(fp, "%3d", b34->code);
    fprintf(fp, "%s~", b34->unit);
    fprintf(fp, "%s~", b34->desc);
    fprintf(fp, "\n");

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
 * $Log: b34.c,v $
 * Revision 1.3  2015/09/08 19:22:55  dechavez
 * fixed description of blockette type in comments
 *
 * Revision 1.2  2015/07/15 17:02:53  dechavez
 * fixed len bug, remove leading zeros on integers to improve readability
 *
 * Revision 1.1  2014/10/29 21:27:12  dechavez
 * created
 *
 */
