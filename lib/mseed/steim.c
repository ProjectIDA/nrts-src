#pragma ident "$Id: steim.c,v 1.3 2015/12/04 23:29:08 dechavez Exp $"
/************************************************************************
 *
 *    Steim1 and Steim2 compression, using DMC library routines
 *
 ************************************************************************/
#include "mseed.h"
extern int msr_pack_steim1 (UINT8*, INT32*, int32_t, int, int, int, int*, int*, int);
extern int msr_pack_steim2 (UINT8*, INT32*, int32_t, int, int, int, int*, int*, int);

int mseedPackSteim1(
    UINT8 *output,   /* ptr to data frames */
    INT32 *data,     /* ptr to unpacked data array */
    INT32 d0,        /* first difference value */
    int   ns,        /* number of samples to pack */
    int   nf,        /* total number of data frames */
    int   pad,       /* flag to specify padding to nf */
    int   *pnframes, /* number of frames actually packed */
    int   *pnsamples /* number of samples actually packed */
) {
int        swapflag;
  
  swapflag = (NATIVE_BYTE_ORDER == BIG_ENDIAN_BYTE_ORDER) ? 0 : 1;

  return msr_pack_steim1(output, data, d0, ns, nf, pad, pnframes, pnsamples, swapflag);
}

int mseedPackSteim2(
    UINT8 *output,   /* ptr to data frames */
    INT32 *data,     /* ptr to unpacked data array */
    INT32 d0,        /* first difference value */
    int   ns,        /* number of samples to pack */
    int   nf,        /* total number of data frames */
    int   pad,       /* flag to specify padding to nf */
    int   *pnframes, /* number of frames actually packed */
    int   *pnsamples /* number of samples actually packed */
) {
int        swapflag;
  
  swapflag = (NATIVE_BYTE_ORDER == BIG_ENDIAN_BYTE_ORDER) ? 0 : 1;

  return msr_pack_steim2(output, data, d0, ns, nf, pad, pnframes, pnsamples, swapflag);
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
 * $Log: steim.c,v $
 * Revision 1.3  2015/12/04 23:29:08  dechavez
 * fixed msr_pack_steimX prototypes to calm OS X compiles
 *
 * Revision 1.2  2015/11/13 20:49:54  dechavez
 * declare ms_pack_steim[12]() prototypes
 *
 * Revision 1.1  2014/08/11 18:18:02  dechavez
 * initial release
 *
 */
