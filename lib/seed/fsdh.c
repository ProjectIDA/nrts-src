#pragma ident "$Id: fsdh.c,v 1.5 2015/12/04 22:50:06 dechavez Exp $"
/*======================================================================
 *
 *  Load a Fixed Section of Data Header to memory and vice-versa.
 *
 * - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 * Copyright (c) 1997 Regents of the University of California.
 * All rights reserved.
 *====================================================================*/
#include <assert.h>
#include "seed.h"
#include "util.h"

void seed_loadfsdh(UINT8 *pdest, struct seed_fsdh *src, UINT32 order)
{
int swap;
char *dest;
INT16 stmp;
INT32 ltmp;
static char *fid = "seed_loadfsdh";

    assert(sizeof(INT16) == 2);
    assert(sizeof(INT32)  == 4);

    dest = (char *) pdest; /* calms DARWIN compiles */

    swap = (order != util_order());

    if (src->seqno > 999999) src->seqno = 1;

/* The front is all ASCII stuff... no swapping required */

    sprintf(dest   , "%06dD             ", src->seqno);
    sprintf(dest+ 8, "%s",                 src->staid);
    sprintf(dest+13, "%s",                 src->locid);
    sprintf(dest+15, "%s",                 src->chnid);
    sprintf(dest+18, "%s",                 src->netid);

/* Time stamp is byte swapped internally (if required) */

    seed_dtconv((UINT8 *) dest+20, src->start, order);

/* Byte swap the remaining stuff into the desired order, if required */

    /* Numer of samples */
    stmp = src->nsamp; if (swap) util_sswap(&stmp, 1);
    memcpy(dest+30, &stmp,  2);

    /* sample rate factor */
    stmp = src->srfact; if (swap) util_sswap(&stmp, 1);
    memcpy(dest+32, &stmp, 2);

    /* sample rate multipler */    
    stmp = src->srmult; if (swap) util_sswap(&stmp, 1);
    memcpy(dest+34, &stmp, 2);

    /* Activity flags */
    memcpy(dest+36, &src->active, 1);

    /* I/O and clock flags */
    memcpy(dest+37, &src->ioclck, 1);

    /* Data quality flags */
    memcpy(dest+38, &src->qual,   1);

    /* Number of blockettes that follow */
    memcpy(dest+39, &src->more, 1);

    /* Time correction */
    ltmp = src->tcorr; if (swap) util_lswap(&ltmp, 1);
    memcpy(dest+40,                      &src->tcorr,  4);

    /* Begining of data */
    stmp = src->bod; if (swap) util_sswap(&stmp, 1);
    memcpy(dest+44, &stmp, 2);

    /* First blockette */
    stmp = src->first; if (swap) util_sswap(&stmp, 1);
    memcpy(dest+46, &stmp, 2);
}

int seed_fsdh(struct seed_fsdh *dest, UINT8 *psrc)
{
char *src;

    assert(sizeof(INT16) == 2);
    assert(sizeof(INT32)  == 4);

    src = (char *) psrc; /* calms DARWIN compiles */

    sscanf(src, "%06dD             ", &dest->seqno);

    memcpy(dest->staid, src+ 8, 5); dest->staid[5] = 0;
    util_strtrm(dest->staid);
    memcpy(dest->locid, src+13, 2); dest->locid[2] = 0;
    util_strtrm(dest->locid);
    memcpy(dest->chnid, src+15, 3); dest->chnid[3] = 0;
    util_strtrm(dest->chnid);
    memcpy(dest->netid, src+18, 2); dest->netid[2] = 0;
    util_strtrm(dest->netid);

    dest->start = seed_ttodt((UINT8 *) src+20, &dest->order);
    memcpy(&dest->nsamp,  src+30, 2);
    memcpy(&dest->srfact, src+32, 2);
    memcpy(&dest->srmult, src+34, 2);
    memcpy(&dest->active, src+36, 1);
    memcpy(&dest->ioclck, src+37, 1);
    memcpy(&dest->qual,   src+38, 1);
    memcpy(&dest->more,   src+39, 1);
    memcpy(&dest->tcorr,  src+40, 4);
    memcpy(&dest->bod,    src+44, 2);
    memcpy(&dest->first,  src+46, 2);

    if (dest->order != util_order()) {
        dest->swap = TRUE;
        util_sswap(&dest->nsamp,  1);
        util_sswap(&dest->srfact, 1);
        util_sswap(&dest->srmult, 1);
        util_lswap(&dest->tcorr,  1);
        util_sswap(&dest->bod,    1);
        util_sswap(&dest->first,  1);
    } else {
        dest->swap = FALSE;
    }

    return 0;
}

#ifdef DEBUG_TEST

main(int argc, char **argv)
{
struct seed_fsdh dest;
UINT8 src[48];

    if (fread(src, 1, 48, stdin) != 48) {
        perror("fread");
        exit(1);
    }

    if (seed_fsdh(&dest, src) != 0) {
        fprintf(stderr, "seed_fsdh failed\n");
        exit(1);
    }

    printf("seqno  = %ld\n", dest.seqno);
    printf("staid  = %s\n",  dest.staid);
    printf("locid  = %s\n",  dest.locid);
    printf("chnid  = %s\n",  dest.chnid);
    printf("netid  = %s\n",  dest.netid);
    printf("start  = %s\n",  util_dttostr(dest.start, 0));
    printf("nsamp  = %hd\n", dest.nsamp);
    printf("srfact = %hd\n", dest.srfact);
    printf("srmult = %hd\n", dest.srmult);
    printf("active = %d\n",  (int) dest.active);
    printf("ioclck = %d\n",  (int) dest.ioclck);
    printf("qual   = %d\n",  (int) dest.qual);
    printf("more   = %d\n",  (int) dest.more);
    printf("tcorr  = %ld\n", dest.tcorr);
    printf("bod    = %hd\n", dest.bod);
    printf("first  = %hd\n", dest.first);
    printf("order  = %s\n",  dest.order == LTL_ENDIAN_ORDER ? "little endian" : "big endian");
    printf("swap   = %d\n",  dest.swap);
    exit(0);
}

#endif /* DEBUG_TEST */

/* Revision History
 *
 * $Log: fsdh.c,v $
 * Revision 1.5  2015/12/04 22:50:06  dechavez
 * casts and format fixes to calm OS X compiles
 *
 * Revision 1.4  2012/02/14 20:22:07  dechavez
 * use portable integer types (eg, long to INT32, unsigned long to UINT32)
 *
 * Revision 1.3  2005/05/25 22:40:22  dechavez
 * mods to calm Visual C++ warnings
 *
 * Revision 1.2  2002/02/14 22:22:22  dec
 * fixe byte order bug
 *
 * Revision 1.1.1.1  2000/02/08 20:20:39  dec
 * import existing IDA/NRTS sources
 *
 */
