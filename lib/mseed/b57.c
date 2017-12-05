#pragma ident "$Id: b57.c,v 1.2 2015/07/15 17:07:46 dechavez Exp $"
/*======================================================================
 * 
 * Blockette 57 - Decimation Blockette
 *
 *====================================================================*/
#include "mseed.h"

BOOL mseedWriteBlockette57(FILE *fp, MSEED_B57 *b57)
{
int i, len;

    len = 3                       /* blockette type - 057 */
        + 4                       /* length of blockette */
        + 2                       /* stage sequence number */
        + 10                      /* input sample rate */
        + 5                       /* decimation factor */
        + 5                       /* decimation offset */
        + 11                      /* estimated delay */
        + 11                      /* correction applied */
        ;

    fprintf(fp, "057");
    fprintf(fp, "%4d", len);

    fprintf(fp,    "%2d", b57->stageid);
    fprintf(fp, "%10.4E", b57->irate);
    fprintf(fp,    "%5d", b57->factor);
    fprintf(fp,    "%5d", b57->offset);
    fprintf(fp, "%11.4E", b57->estdelay);
    fprintf(fp, "%11.4E", b57->correction);

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
 * $Log: b57.c,v $
 * Revision 1.2  2015/07/15 17:07:46  dechavez
 * removed spurious "history" field left over from sloppy copy and paste
 *
 * Revision 1.1  2015/07/10 17:54:01  dechavez
 * created
 *
 */
