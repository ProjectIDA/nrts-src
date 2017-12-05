#pragma ident "$Id: complex.c,v 1.2 2015/08/24 19:13:00 dechavez Exp $"
/*====================================================================
 *
 * Complex number stuff
 *
 *==================================================================*/
#include "util.h"

BOOL utilComplexMultiply(UTIL_COMPLEX *x, UTIL_COMPLEX *y, UTIL_COMPLEX *result)
{
UTIL_COMPLEX product;

    if (x == NULL || y == NULL || result == NULL) {
        errno = EINVAL;
        return FALSE;
    }

    product.r = (x->r * y->r) - (x->i * y->i);
    product.i = (x->r * y->i) + (x->i * y->r);

    *result = product;

    return TRUE;
}

REAL64 utilComplexMagnitude(UTIL_COMPLEX *complex)
{
REAL64 result;

    result = sqrt((complex->r * complex->r) + (complex->i * complex->i));

    return result;
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
 * $Log: complex.c,v $
 * Revision 1.2  2015/08/24 19:13:00  dechavez
 * utilComplexMultiply() to use temporary variable (not sure why I felt it was needed)
 *
 * Revision 1.1  2015/07/10 17:47:48  dechavez
 * initial release
 *
 */
