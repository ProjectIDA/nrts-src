#pragma ident "$Id: b58.c,v 1.2 2015/07/15 17:07:55 dechavez Exp $"
/*======================================================================
 * 
 * Blockette 58 - Channel Sensitivity/Gain Blockette
 *
 *====================================================================*/
#include "mseed.h"

BOOL mseedWriteBlockette58(FILE *fp, MSEED_B58 *b58)
{
int i, len, history = 0;

    len = 3                       /* blockette type - 058 */
        + 4                       /* length of blockette */
        + 2                       /* stage sequence number */
        + 12                      /* sensitivity/gain */
        + 12                      /* frequency */
        + 2                       /* number of history values (always 0) */
        ;

    fprintf(fp, "058");
    fprintf(fp, "%4d", len);

    fprintf(fp,    "%2d", b58->stageid);
    fprintf(fp, "%12.5E", b58->gain);
    fprintf(fp, "%12.5E", b58->freq);
    fprintf(fp,    "%2d", history);

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
 * $Log: b58.c,v $
 * Revision 1.2  2015/07/15 17:07:55  dechavez
 * moved semicolon placement in len calculation to match other files
 *
 * Revision 1.1  2015/07/10 17:54:01  dechavez
 * created
 *
 */
