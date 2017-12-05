#pragma ident "$Id: swap.c,v 1.8 2014/04/15 20:47:29 dechavez Exp $"
/*======================================================================
 *
 *  Byte-swapping utilities.
 *
 *====================================================================*/
#include "util.h"

VOID utilSwapUINT16(UINT16 *input, long count)
{
UINT8 ctmp[2];
UINT8 temp;
long i;

    for (i = 0; i < count; i++) {
        memcpy(ctmp, input + i, 2);
        temp    = ctmp[0];
        ctmp[0] = ctmp[1];
        ctmp[1] = temp;
        memcpy(input + i, ctmp, 2);
    }
}

VOID utilSwapUINT32(UINT32 *input, long count)
{
UINT16 stmp[2];
UINT16 temp;
long i;

    utilSwapUINT16((UINT16 *) input, count*2);
    for (i = 0; i < count; i++) {
        memcpy(stmp, input + i, 4);
        temp    = stmp[0];
        stmp[0] = stmp[1];
        stmp[1] = temp;
        memcpy(input + i, stmp, 4);
    }
}

VOID utilSwapUINT64(UINT64 *input, long count)
{
UINT32 ltmp[2];
UINT32 temp;
long i;

    utilSwapUINT32((UINT32 *) input, count*2);
    for (i = 0; i < count; i++) {
        memcpy(ltmp, input + i, 8);
        temp    = ltmp[0];
        ltmp[0] = ltmp[1];
        ltmp[1] = temp;
        memcpy(input + i, ltmp, 8);
    }
}

VOID utilSwapREAL32(REAL32 *input, long count)
{
    utilSwapUINT32((UINT32 *) input, count);
}

VOID utilSwapREAL64(REAL64 *input, long count)
{
    utilSwapUINT64((UINT64 *) input, count);
}

#define MANTISSA_MASK ((UINT32)(0x00FFFFFF))
#define MANTISSA_SIZE (24)

void util_iftovf(UINT32 *input, long number)
{
static int native_order = -1;
UINT32  mantissa, exponent;
long i;

    if (native_order == -1) native_order = util_order();
    if (native_order == LTL_ENDIAN_ORDER) util_lswap((long *) input, number);
    for (i = 0; i < number; i++) {
        mantissa = input[i] & MANTISSA_MASK;
        exponent = (((input[i] >> MANTISSA_SIZE) + 1) << MANTISSA_SIZE);
        input[i] = mantissa | exponent;
    }
    util_sswap((short *)input, number*2);
    if (native_order == LTL_ENDIAN_ORDER) util_lswap((long *)input, number);
}

void util_vftoif(UINT32 *input, long number)
{
static int native_order = -1;
UINT32  mantissa, exponent;
long i;

    if (native_order == -1) native_order = util_order();
    if (native_order == LTL_ENDIAN_ORDER) util_lswap((long *)input, number);
    util_sswap((short *)input, number*2);
    for (i = 0; i < number; i++) {
        if (input[i] != 0) {
            mantissa = input[i] & MANTISSA_MASK;
            exponent = (((input[i]>>MANTISSA_SIZE)-1)<<MANTISSA_SIZE);
            input[i] = mantissa | exponent;
        }
    }
    if (native_order == LTL_ENDIAN_ORDER) util_lswap((long *)input, number);
}

UINT32 util_order()
{
union {
    UINT8 character[4];
    UINT32 integer;
} test4;

/* Construct a 4-byte word of known contents - 0x76543210 */
/* Result will be 0x10325476 for little endian machines (eg Vax, PC) */
/*                0x76543210 for big    endian machines (eg Sun)     */
/*                0x54761032 for PDP-11's                            */
/* The include file "util.h" defines the constants BIG_ENDIAN_ORDER  */
/* and LTL_ENDIAN_ORDER to correspond to output of this routine.     */

    test4.character[0] = 0x76;
    test4.character[1] = 0x54;
    test4.character[2] = 0x32;
    test4.character[3] = 0x10;

    return test4.integer;
}

/* Revision History
 *
 * $Log: swap.c,v $
 * Revision 1.8  2014/04/15 20:47:29  dechavez
 * added utilSwapREAL32(), utilSwapREAL64()
 *
 * Revision 1.7  2012/02/14 19:55:02  dechavez
 * use portable integer types (eg, long to INT32, unsigned long to UINT32)
 *
 * Revision 1.6  2005/05/25 22:41:46  dechavez
 * mods to calm Visual C++ warnings
 *
 * Revision 1.5  2003/12/10 06:07:00  dechavez
 * renamed swappers to UINTx, INTZx's now being util.h macros
 *
 * Revision 1.4  2003/11/19 23:29:19  dechavez
 * ade util_lswap and util_sswap macros (in util.h)
 *
 * Revision 1.3  2003/06/09 23:56:07  dechavez
 * added ESSW equivalents and replaced originals with calls to the new forms
 *
 * Revision 1.2  2001/05/07 22:40:13  dec
 * ANSI function declarations
 *
 * Revision 1.1.1.1  2000/02/08 20:20:42  dec
 * import existing IDA/NRTS sources
 *
 */
