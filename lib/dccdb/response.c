/*======================================================================
 *
 *  Compute the stage 0 sensitivity for a given DCCDB_CASCADE
 *
 *====================================================================*/
#include "dccdb.h"

BOOL dccdbSetCascadeA0Freq(DCCDB_CASCADE *cascade, REAL64 freq)
{
int i, units;
DCCDB_STAGE *stage;
UTIL_COMPLEX StageResponse, CumulativeResponse = UTIL_COMPLEX_1, ComplexOne = UTIL_COMPLEX_1;
static char *fid = "dccdbSetCascadeA0Freq";
char str1[1024];

    if (cascade == NULL) return FALSE;

    cascade->freq = freq;
    cascade->a0 = 1.0;

    for (i = 0; i < cascade->nentry; i++) {
        stage = &cascade->entry[i];
        units = filterUnitsCode(stage->iunits); /* NULL return OK */
        cascade->a0 *= (stage->gnom * stage->gcalib);
        if ((stage->filter.type != FILTER_TYPE_ANALOG) && (stage->filter.type != FILTER_TYPE_LAPLACE)) {
            if (!filterResponse(&stage->filter, freq, units, stage->srate, &StageResponse)) {
                cascade->errcode = DCCDB_ERRCODE_RESPONSE_ERROR;
                return FALSE;
            }
        } else {
            StageResponse = ComplexOne; /* by definition/convention */
        }
        utilComplexMultiply(&StageResponse, &CumulativeResponse, &CumulativeResponse);
    }

    cascade->a0 *= utilComplexMagnitude(&CumulativeResponse);

    cascade->errcode = DCCDB_ERRCODE_NO_ERROR;
    return TRUE;
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
 * Revision 1.4  2019/04/15 18:00:00  dauerbach
 * Check for FILTER_TYPE_LAPLACE poles and zeros response to skip call to
 * filterResponse() same as for FILTER_TYPE_ANALOG poles and zeros response
 * This fixes problem with stage 0 A0 calculation for LAPLACE responses
 *
 * Revision 1.3  2015/12/01 22:15:16  dechavez
 * Don't fail if units lookup returns NULL. The return value is only used in
 * the call to filterResponse(), and doesn't mind.
 *
 * Revision 1.2  2015/08/26 23:15:49  dechavez
 * fixed bug in setting cascade->a0 (multiply by mag(resp), NOT the inverse!)
 *
 * Revision 1.1  2015/08/24 18:24:14  dechavez
 * initial release
 *
 */
