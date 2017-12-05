#pragma ident "$Id: kcap.c,v 1.1 2013/03/07 21:12:02 dechavez Exp $"
/*======================================================================
 *
 * Kcap/unpack various things into/out of little-endian byte order.
 *
 * These are intended for dealing with data frames whose contents
 * are in little-endian byte order (such as with ADDOSS, at least for
 * now).  For network I/O, use utilPackX and utilUnpackX from pack.c.
 *
 *====================================================================*/
#include "util.h"

/* Pack/Unpack 16 bit integer values */

int utilKcapUINT16(UINT8 *ptr, UINT16 value)
{
UINT16 sval;

    sval = value;
#ifdef BIG_ENDIAN_HOST
    utilSwapUINT16(&sval, 1);
#endif /* BIG_ENDIAN_HOST */
    memcpy(ptr, &sval, 2);

    return 2;
}

int utilUnkcapUINT16(UINT8 *ptr, UINT16 *out)
{
UINT16 sval;

    memcpy(&sval, ptr, 2);
#ifdef BIG_ENDIAN_HOST
    utilSwapUINT16(&sval, 1);
#endif /* BIG_ENDIAN_HOST */
    *out = sval;

    return 2;
}

int utilKcapINT16(UINT8 *ptr, INT16 value)
{
INT16 sval;

    sval = value;
#ifdef BIG_ENDIAN_HOST
    utilSwapINT16(&sval, 1);
#endif /* BIG_ENDIAN_HOST */
    memcpy(ptr, &sval, 2);

    return 2;
}

int utilUnkcapINT16(UINT8 *ptr, INT16 *out)
{
INT16 sval;

    memcpy(&sval, ptr, 2);
#ifdef BIG_ENDIAN_HOST
    utilSwapINT16(&sval, 1);
#endif /* BIG_ENDIAN_HOST */
    *out = sval;

    return 2;
}

int utilKcapUINT32(UINT8 *ptr, UINT32 value)
{
UINT32 lval;

    lval = value;
#ifdef BIG_ENDIAN_HOST
    utilSwapUINT32(&lval, 1);
#endif /* BIG_ENDIAN_HOST */
    memcpy(ptr, &lval, 4);

    return 4;
}

int utilUnkcapUINT32(UINT8 *ptr, UINT32 *out)
{
UINT32 lval;

    memcpy(&lval, ptr, 4);
#ifdef BIG_ENDIAN_HOST
    utilSwapUINT32(&lval, 1);
#endif /* BIG_ENDIAN_HOST */
    *out = lval;

    return 4;
}

int utilKcapINT32(UINT8 *ptr, INT32 value)
{
INT32 lval;

    lval = value;
#ifdef BIG_ENDIAN_HOST
    utilSwapINT32(&lval, 1);
#endif /* BIG_ENDIAN_HOST */
    memcpy(ptr, &lval, 4);

    return 4;
}

int utilUnkcapINT32(UINT8 *ptr, INT32 *out)
{
INT32 lval;

    memcpy(&lval, ptr, 4);
#ifdef BIG_ENDIAN_HOST
    utilSwapINT32(&lval, 1);
#endif /* BIG_ENDIAN_HOST */
    *out = lval;

    return 4;
}

/* Pack/Unpack 64 bit integer values */

int utilKcapUINT64(UINT8 *ptr, UINT64 value)
{
UINT64 lval, *ptr64;

#ifdef BIG_ENDIAN_HOST
    lval = value;
    ptr64 = &lval;
    utilSwapINT64(ptr64, 1);
#else
    ptr64 = &value;
#endif

    memcpy(ptr, ptr64, 8);

    return 8;
}

int utilUnkcapUINT64(UINT8 *ptr, UINT64 *out)
{
UINT64 lval;

    memcpy(&lval, ptr, 8);
#ifdef BIG_ENDIAN_HOST
    utilSwapINT64(&lval, 1);
#endif
    *out = lval;

    return 8;
}

int utilKcapINT64(UINT8 *ptr, INT64 value)
{
INT64 lval, *ptr64;

#ifdef BIG_ENDIAN_HOST
    lval = value;
    ptr64 = &lval;
    utilSwapINT64((UINT64 *) ptr, 1);
#else
    ptr64 = &value;
#endif

    memcpy(ptr, ptr64, 8);

    return 8;
}

int utilUnkcapINT64(UINT8 *ptr, INT64 *out)
{
INT64 lval;

    memcpy(&lval, ptr, 8);
#ifdef BIG_ENDIAN_HOST
    utilSwapINT64((UINT64 *) &lval, 1);
#endif
    *out = lval;

    return 8;
}

/* Pack/Unpack 32 bit float values */

int utilKcapREAL32(UINT8 *ptr, REAL32 value)
{
union {
    REAL32 f;
    UINT32 l;
} val;

    val.f = value;
    return utilKcapUINT32(ptr, val.l);
}

int utilUnkcapREAL32(UINT8 *ptr, REAL32 *out)
{
union {
    REAL32 f;
    UINT32 l;
} val;

    utilUnkcapUINT32(ptr, &val.l);
    *out = val.f;

    return 4;
}

/* Pack/Unpack 64 bit float values */

int utilKcapREAL64(UINT8 *ptr, REAL64 value)
{
union {
    REAL64 f;
    UINT64 l;
} val;

    val.f = value;
    return utilKcapUINT64(ptr, val.l);
}

int utilUnkcapREAL64(UINT8 *ptr, REAL64 *out)
{
union {
    REAL64 f;
    UINT64 l;
} val;

    utilUnkcapUINT64(ptr, &val.l);
    *out = val.f;

    return 8;
}

/* Revision History
 *
 * $Log: kcap.c,v $
 * Revision 1.1  2013/03/07 21:12:02  dechavez
 * initial release
 *
 */
