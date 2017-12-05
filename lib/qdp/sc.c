#pragma ident "$Id: sc.c,v 1.1 2010/03/31 20:24:56 dechavez Exp $"
/*======================================================================
 * 
 * Update sensor control bitmap
 *
 *====================================================================*/
#include "qdp.h"

BOOL qdpUpdateSensorControlBitmap(UINT16 *bitmap, UINT32 *sc, int line)
{
int i;

    for (i = 0; i < QDP_NSC; i++) {
        if ((sc[i] & QDP_SC_MASK) == line) {
            if (sc[i] & QDP_SC_ACTIVE_HIGH_BIT) {
                *bitmap |= (1 << i);
            } else {
                *bitmap &= ~(1 << i);
            }
            return TRUE;
        }
    }

    return FALSE;
}

/*-----------------------------------------------------------------------+
 |                                                                       |
 | Copyright (C) 2010 Regents of the University of California            |
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
 * $Log: sc.c,v $
 * Revision 1.1  2010/03/31 20:24:56  dechavez
 * initial release
 *
 */
