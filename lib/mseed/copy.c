#pragma ident "$Id: copy.c,v 1.2 2015/09/24 22:12:15 dechavez Exp $"
/*======================================================================
 * 
 * Convert an IDA10 packet into an MSEED_RECORD
 *
 *====================================================================*/
#include "mseed.h"

int mseedCopyINT16(INT16 *dest, INT16 *src, INT32 nsamp)
{
int i;

    for (i = 0; i < nsamp; i++) dest[i] = src[i];
    return MSEED_FORMAT_INT_16;
}

int mseedCopyINT32(INT32 *dest, INT32 *src, INT32 nsamp)
{
int i;

    for (i = 0; i < nsamp; i++) dest[i] = src[i];
    return MSEED_FORMAT_INT_32;
}

int mseedCopyREAL32(REAL32 *dest, REAL32 *src, INT32 nsamp)
{
int i;

    for (i = 0; i < nsamp; i++) dest[i] = src[i];
    return MSEED_FORMAT_IEEE_F;
}

int mseedCopyREAL64(REAL64 *dest, REAL64 *src, INT32 nsamp)
{
int i;

    for (i = 0; i < nsamp; i++) dest[i] = src[i];
    return MSEED_FORMAT_IEEE_D;
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
 * $Log: copy.c,v $
 * Revision 1.2  2015/09/24 22:12:15  dechavez
 * changed nsamp arg to INT32
 *
 * Revision 1.1  2014/08/11 18:18:01  dechavez
 * initial release
 *
 */
