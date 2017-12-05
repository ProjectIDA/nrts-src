#pragma ident "$Id: datatype.c,v 1.3 2015/10/02 15:32:14 dechavez Exp $"
/*======================================================================
 *
 *  Given a word size and util_order() byte order descriptor, return the
 *  appropriate datatype string.
 * 
 * - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 * Copyright (c) 1997 Regents of the University of California.
 * All rights reserved.
 *====================================================================*/
#include <sys/types.h>
#include "cssio.h"
#include "util.h"

char *cssio_datatype(int wrdsiz, UINT32 order, int integer)
{
static char *s2 = "s2";
static char *s4 = "s4";
static char *t4 = "t4";
static char *i2 = "i2";
static char *i4 = "i4";
static char *f4 = "f4";
static char *undefined = "-";

    if ( integer && order == BIG_ENDIAN_ORDER && wrdsiz == 2) return s2;
    if ( integer && order == BIG_ENDIAN_ORDER && wrdsiz == 4) return s4;
    if (!integer && order == BIG_ENDIAN_ORDER && wrdsiz == 4) return t4;

    if ( integer && order == LTL_ENDIAN_ORDER && wrdsiz == 2) return i2;
    if ( integer && order == LTL_ENDIAN_ORDER && wrdsiz == 4) return i4;
    if (!integer && order == LTL_ENDIAN_ORDER && wrdsiz == 4) return f4;

    return undefined;
}

int cssio_samplesize(char *datatype)
{
    if (strcmp(datatype, "i2") == 0 || strcmp(datatype, "s2") == 0) return sizeof(INT16);
    if (strcmp(datatype, "i4") == 0 || strcmp(datatype, "s4") == 0) return sizeof(INT32);
    if (strcmp(datatype, "f4") == 0 || strcmp(datatype, "t4") == 0) return sizeof(REAL32);
    if (strcmp(datatype, "f8") == 0 || strcmp(datatype, "t8") == 0) return sizeof(REAL64);

    return -1;
}

/* Revision History
 *
 * $Log: datatype.c,v $
 * Revision 1.3  2015/10/02 15:32:14  dechavez
 * added cssio_samplesize()
 *
 * Revision 1.2  2012/02/14 20:04:49  dechavez
 * use portable integer types (eg, long to INT32, unsigned long to UINT32)
 *
 * Revision 1.1.1.1  2000/02/08 20:20:23  dec
 * import existing IDA/NRTS sources
 *
 */
