#pragma ident "$Id: compress.c,v 1.6 2015/11/13 21:20:47 dechavez Exp $"
/*======================================================================
 *
 *  Misc simple-minded compression/decompression routines:
 *
 *  util_lcomp: compress an array of longs
 *  util_ldcmp: decompress into an array of longs  (inverse of lcmp)
 *  util_scomp: compress an array of shorts;
 *  util_sdcmp: decompress into an array of shorts (inverse of scmp)
 *
 * - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 * Copyright (c) 1997 Regents of the University of California.
 * All rights reserved.
 *====================================================================*/
#include "util.h"

#define B00000001 0x01
#define B10000000 0x80
#define B01111110 0x7e
#define B00111111 0x3f
#define B01111111 0x7f

UINT32 utilIdaCompressINT32(UINT8 *dest, INT32 *src, UINT32 nsamp)
{
UINT8 sign;
INT32 diff;
UINT32 i, index = 0;

/*  Do first sample  */

    diff = src[0];
    if ((sign = (diff < 0) ? B00000001 : 0) != 0) diff = -diff;
    if (diff < 0) return -1L; /* Can't deal with clipped data */
    dest[index] = ((diff & B00111111) << 1) | sign;
    diff = diff >> 6;
    while (diff) {
        dest[index] |= B10000000;
        ++index;
        dest[index] = diff & B01111111;
        diff = diff >> 7;
    }

/*  Do remaining samples  */
        
    for (i = 1; i < nsamp; i++) {
        ++index;
        diff = src[i] - src[i-1];
        if ((sign = (diff < 0) ? B00000001 : 0) != 0) diff = -diff;
        if (diff < 0) return -1L; /* Can't deal with clipped data */
        dest[index] = ((diff & B00111111) << 1) | sign;
        diff = diff >> 6;
        while (diff) {
            dest[index] |= B10000000;
            ++index;
            dest[index] = diff & B01111111;
            diff = diff >> 7;
        }
    }

/* Return the number of output bytes */
    
    return ++index;
}

UINT32 utilIdaExpandINT32(INT32 *dest, UINT8 *src, UINT32 nsamp)
{
int negative, shift;
INT32 diff;
UINT32 i, index = 0;

    negative = src[index] & B00000001;
    dest[0] = (src[index] & B01111110) >> 1;
    shift = 6;
    while (src[index] & B10000000) {
        ++index;
        dest[0] |= ((src[index] & B01111111) << shift);
        shift += 7;
    }
    if (negative) dest[0] = -dest[0];

    for (i = 1; i < nsamp; i++) {
        ++index;
        negative = src[index] & B00000001;
        diff = (src[index] & B01111110) >> 1;
        shift = 6;
        while (src[index] & B10000000) {
            ++index;
            diff |= ((src[index] & B01111111) << shift);
            shift += 7;
        }
        if (negative) diff = -diff;
        dest[i] = dest[i-1] + diff;
    }

/* Return the value of the last sample */

    return dest[nsamp-1];
}

INT32 util_lcomp(UINT8 *dest, INT32 *src, INT32 nsamp)
{
    return (INT32) utilIdaCompressINT32((UINT8*) dest, (INT32*) src, (UINT32) nsamp);
}

INT32 util_ldcmp(INT32 *dest, UINT8 *src, INT32 nsamp)
{
    return (INT32) utilIdaExpandINT32((INT32*) dest, (UINT8*) src, (UINT32) nsamp);
}

INT32 util_scomp(UINT8 *dest, INT16 *src, INT32 nsamp)
{
UINT8 sign;
INT16 diff;
INT32 i, index  = 0;


/*  Do first sample  */

    diff = src[0];
    if ((sign = (diff < 0) ? B00000001 : 0) != 0) diff = -diff;
    if (diff < 0) return -1L; /* Can't deal with clipped data */
    dest[index] = ((diff & B00111111) << 1) | sign;
    diff = diff >> 6;
    while (diff) {
        dest[index] |= B10000000;
        ++index;
        dest[index] = diff & B01111111;
        diff = diff >> 7;
    }

/*  Do remaining samples  */
        
    for (i = 1; i < nsamp; i++) {
        ++index;
        diff = src[i] - src[i-1];
        if ((sign = (diff < 0) ? B00000001 : 0) != 0) diff = -diff;
        if (diff < 0) return -1L; /* Can't deal with clipped data */
        dest[index] = ((diff & B00111111) << 1) | sign;
        diff = diff >> 6;
        while (diff) {
            dest[index] |= B10000000;
            ++index;
            dest[index] = diff & B01111111;
            diff = diff >> 7;
        }
    }
    
    return ++index;
}

INT16 util_sdcmp(INT16 *dest, UINT8 *src, INT32 nsamp)
{
INT16 diff;
int negative, shift;
INT32 i, index = 0;

    negative = src[index] & B00000001;
    dest[0] = (src[index] & B01111110) >> 1;
    shift = 6;
    while (src[index] & B10000000) {
        ++index;
        dest[0] |= ((src[index] & B01111111) << shift);
        shift += 7;
    }
    if (negative) dest[0] = -dest[0];

    for (i = 1; i < nsamp; i++) {
        ++index;
        negative = src[index] & B00000001;
        diff = (src[index] & B01111110) >> 1;
        shift = 6;
        while (src[index] & B10000000) {
            ++index;
            diff |= ((src[index] & B01111111) << shift);
            shift += 7;
        }
        if (negative) diff = -diff;
        dest[i] = dest[i-1] + diff;
    }

    return dest[nsamp-1];
}

/* Revision History
 *
 * $Log: compress.c,v $
 * Revision 1.6  2015/11/13 21:20:47  dechavez
 *  added some parentheses to stop Darwin from whining about
 *  "using the result of an assignment as a condition without parentheses"
 *
 * Revision 1.5  2012/02/14 19:55:02  dechavez
 * use portable integer types (eg, long to INT32, unsigned long to UINT32)
 *
 * Revision 1.4  2003/12/04 23:29:07  dechavez
 * removed tabs
 *
 * Revision 1.3  2003/06/10 00:03:49  dechavez
 * utilIdaCompressINT32() and utilIdaExpandINT32() and made util_lcomp and util_ldcmp
 * calls to them
 *
 * Revision 1.2  2001/05/07 22:40:12  dec
 * ANSI function declarations
 *
 * Revision 1.1.1.1  2000/02/08 20:20:41  dec
 * import existing IDA/NRTS sources
 *
 */
