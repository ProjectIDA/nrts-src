#pragma ident "$Id: response.c,v 1.4 2015/09/04 00:49:07 dechavez Exp $"
/*====================================================================
 *
 * Calculate complex response for IRIS-IDA stage at specified freq
 *
 *==================================================================*/
#include "filter.h"
#include <math.h>

#define    PI (REAL64) 3.141592654
#define TWOPI (REAL64) 6.283185307

static void AnalogTrans(FILTER_PANDZ *pandz, REAL64 freq, UTIL_COMPLEX *b)
{
int i;
REAL64 mod = 1.0, pha = 0.0;
REAL64 R, I;

    for (i = 0; i < pandz->nzero; i++) {
        R = -pandz->zero[i].r;
        I = -pandz->zero[i].i;
        if (R == 0.0) {
            mod *= freq;
            pha += PI/2.0;
        } else {
            mod *= sqrt((freq+I)*(freq+I) + R*R);
            pha += atan2(freq+I, R);
        }
    }

    for (i = 0; i < pandz->npole; i++) {
        R = -pandz->pole[i].r;
        I = -pandz->pole[i].i;
        if (R == 0.0) {
            mod /= freq;
            pha -= PI/2.0;
        } else {
            mod /= sqrt((freq+I)*(freq+I) + R*R);
            pha -= atan2(freq+I, R);
        }
    }

    b->r = mod * cos(pha);
    b->i = mod * sin(pha);
}

static void IIRtrans(FILTER_PANDZ *pandz, REAL64 wsint, UTIL_COMPLEX *b)
{
int i;
REAL64 mod = 1.0, pha = 0.0;
REAL64 c, s, R, I;

    c = cos(wsint);
    s = sin(wsint);

    for (i = 0; i < pandz->nzero; i++) {
        R = c + pandz->zero[i].r;
        I = s + pandz->zero[i].i;
        mod *= sqrt(R*R + I*I);
        if (R == 0.0 && I == 0.0) {
            pha += 0.0;
        } else {
            pha += atan2(I, R);
        }
    }

    for (i = 0; i < pandz->npole; i++) {
        R = c + pandz->pole[i].r;
        I = s + pandz->pole[i].i;
        mod /= sqrt(R*R +I*I);
        if (R == 0.0 && I == 0.0) {
            pha += 0.0;
        } else {
            pha -= atan2(I, R);
        }
    }

    b->r = mod * cos(pha);
    b->i = mod * sin(pha);
}

static void RunMean(FILTER_COEFF *coeff, REAL64 wsint, UTIL_COMPLEX *b)
{
    b->r = (wsint == 0.0) ? 1.0 : sin(wsint/2.*coeff->ncoef) / sin(wsint/2.);
    b->i = 0.0;
}

static void FIRSymTrans(FILTER_COEFF *coeff, REAL64 wsint, UTIL_COMPLEX *b)
{
int i, n0;
REAL64 R = 0.0;

    n0 = (coeff->ncoef - 1) / 2;
    for (i = 1; i < coeff->ncoef - n0; i++) R += coeff->coef[i+n0] * cos(wsint * i);
    b->r = (coeff->coef[n0] + 2 * R);
    b->i = 0.0;
}

static void FIRAsymTrans(FILTER_COEFF *coeff, REAL64 wsint, UTIL_COMPLEX *b)
{
int i;
REAL64 R = 0.0, I = 0.0, y, mod, pha;

    for (i = 1; i < coeff->ncoef; i++) if (coeff->coef[i] != coeff->coef[0]) break;

    if (i == coeff->ncoef) {    /* filter is a comb (running mean) */

        if (wsint == 0.0) {
            b->r = 1.0;
        } else {
            b->r = (sin(wsint/2.*coeff->ncoef) / sin(wsint/2.)) * coeff->coef[0];
        }
        b->i = 0;

    } else {

        for (i = 0; i < coeff->ncoef; i++) {
            y = wsint * i;
            R += coeff->coef[i] * cos(y);
            I += coeff->coef[i] * -sin(y);
        }

        mod = sqrt(R*R + I*I);
        pha = atan2(I,R);
        R = mod * cos(pha);
        I = mod * sin(pha);

        b->r = R;
        b->i = I;
    }
}

BOOL filterResponse(FILTER *filter, REAL64 freq, int units, REAL64 srate, UTIL_COMPLEX *out)
{
REAL64 w, wsint;
UTIL_COMPLEX a=UTIL_COMPLEX_1, b=UTIL_COMPLEX_1;
static char *fid = "filterResponse";

    w = TWOPI * freq;
    wsint = w / srate ;

    switch (units) {
      case FILTER_UNITS_DIS: a.r = 0.0; a.i = -1/w; break; /* convert displacement to velocity */
      case FILTER_UNITS_ACC: a.r = 0.0; a.i =    w; break; /* convert acceleration to velocity */
    }

    switch (filter->type) {     /* FILTER_TYPE_NULL will just fall through, yielding unity response */
      case FILTER_TYPE_ANALOG:
        AnalogTrans(&filter->data.pz, freq, &b);
        break;
      case FILTER_TYPE_LAPLACE:
        AnalogTrans(&filter->data.pz, freq, &b);
        break;
      case FILTER_TYPE_IIR_PZ:
        IIRtrans(&filter->data.pz, wsint, &b);
        break;
      case FILTER_TYPE_COMB:
        RunMean(&filter->data.cf, wsint, &b);
        break;
      case FILTER_TYPE_FIR_SYM:
        FIRSymTrans(&filter->data.cf, wsint, &b);
        break;
      case FILTER_TYPE_FIR_ASYM:
        FIRAsymTrans(&filter->data.cf, wsint, &b);
        break;
    }

    utilComplexMultiply(&a, &b, out);
    return TRUE;
}

BOOL filterA0(FILTER *filter, REAL64 freq, REAL64 srate, REAL64 *result)
{
UTIL_COMPLEX resp;

    if (filter == NULL || freq <= 0.0 || result == NULL) {
        errno = EINVAL;
        return FALSE;
    }

    if (!filterResponse(filter, freq, FILTER_UNITS_NULL, srate, &resp)) return FALSE;
    *result = 1.0 / utilComplexMagnitude(&resp);

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
 * Revision 1.5  2019/04/15 18:00:00  dauerbach
 * Add switch clause for FILTER_TYPE_LAPLACE so
 * it doesn't fall through resulting in a0==1.0
 *
 * Revision 1.4  2015/09/04 00:49:07  dechavez
 * fixed bug in IIRtrans()
 *
 * Revision 1.3  2015/09/01 18:48:16  dechavez
 * fixed bug in RunMean()
 *
 * Revision 1.2  2015/08/24 18:43:47  dechavez
 * removed factor from filterResponse(), added filterA0()
 *
 * Revision 1.1  2015/07/15 17:29:40  dechavez
 * created
 *
 */
