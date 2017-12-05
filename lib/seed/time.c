#pragma ident "$Id: time.c,v 1.3 2012/02/14 20:22:07 dechavez Exp $"
/*======================================================================
 *
 *  seed_dtconv()
 *  Given a epoch (double) time and a 10 byte character array, load the
 *  array with the split time.
 *
 *  seed_ttodt()
 *  Given an array with data in seed time format, convert to double.
 *  This routine also examines the raw data in order to guess the
 *  native byte order of the rest of the header (something which the
 *  SEED format fails to take into account... duh).  It does this by
 *  looking at the year, and if it is negative or greater than 2038
 *  (the year all this code falls apart due to Unix time overflow!)
 *  then it is assumed that the header is ordered opposite of this
 *  host (there is the further assumption that there are only two
 *  types of word order, little-endian and big-endian).  There are
 *  a few years where this test will not work, in which case we further
 *  do a test on the day of year field, and that narrows down the
 *  possibility of failure to a few days in the late 18th century for
 *  which there is likely to be little data.
 *
 * - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 * Copyright (c) 1997 Regents of the University of California.
 * All rights reserved.
 *====================================================================*/
#include <assert.h>
#include <memory.h>
#include "seed.h"
#include "util.h"

UINT8 *seed_dtconv(UINT8 *output, double dtime, UINT32 order)
{
int swap, tmp1, tmp2, tmp3, tmp4, tmp5, tmp6;
INT16 yr, da, ms;
UINT8 hr, mn, sc, un = 0;

    assert(sizeof(INT16) == 2);
    swap = (order != util_order());

    util_tsplit(dtime, &tmp1, &tmp2, &tmp3, &tmp4, &tmp5, &tmp6);
    yr = (INT16) tmp1;      if (swap) util_sswap(&yr, 1);
    da = (INT16) tmp2;      if (swap) util_sswap(&da, 1);
    hr = (UINT8) tmp3;
    mn = (UINT8) tmp4;
    sc = (UINT8) tmp5;
    ms = (INT16) tmp6 * 10; if (swap) util_sswap(&ms, 1);

    memcpy(output+0, (void *) &yr, 2);
    memcpy(output+2, (void *) &da, 2);
    memcpy(output+4, (void *) &hr, 1);
    memcpy(output+5, (void *) &mn, 1);
    memcpy(output+6, (void *) &sc, 1);
    memcpy(output+7, (void *) &un, 1);
    memcpy(output+8, (void *) &ms, 2);

    return output;
}

double seed_ttodt(UINT8 *seedtime, UINT32 *order)
{
int swap;
UINT32 native_order, other_order;
INT16 yr, da, ms;
UINT8 hr, mn, sc;

    assert(sizeof(INT16) == 2);

    if ((native_order = util_order()) == LTL_ENDIAN_ORDER) {
        other_order = BIG_ENDIAN_ORDER;
    } else {
        other_order = LTL_ENDIAN_ORDER;
    }

    memcpy((void *) &yr, seedtime+0, 2);
    memcpy((void *) &da, seedtime+2, 2);
    memcpy((void *) &hr, seedtime+4, 1);
    memcpy((void *) &mn, seedtime+5, 1);
    memcpy((void *) &sc, seedtime+6, 1);
    memcpy((void *) &ms, seedtime+8, 2);

/* Try to guess the byte ordering used */

    if (yr < 0 || yr > 2038 || da < 0 || da > 366) {
        *order = other_order;
        swap = 1;
    } else {
         *order = native_order;
        swap = 0;
    }

/* Re-order if necessary, and then convert */

    if (swap) {
        util_sswap(&yr, 1);
        util_sswap(&da, 1);
        util_sswap(&ms, 1);
    }
    ms /= 10;

    return util_ydhmsmtod(
        (int)yr, (int)da, (int)hr, (int)mn, (int)sc, (int)ms
    );
}

#ifdef DEBUG_TEST

#include <time.h>
#include <sys/types.h>

main()
{
UINT32 order;
INT16 yr, da, ms;
UINT8 hr, mn, sc;
UINT8 seedtime[10];
double dtime = 0.123, stime;
time_t now;

    assert(sizeof(INT16) == 2);

    now = time(NULL);
    dtime += (double) now;

    printf("current time = %.3lf (%s)\n", dtime, util_dttostr(dtime, 0));
    seed_dtconv(seedtime, dtime);
    memcpy((void *) &yr, seedtime+0, 2);
    memcpy((void *) &da, seedtime+2, 2);
    memcpy((void *) &hr, seedtime+4, 1);
    memcpy((void *) &mn, seedtime+5, 1);
    memcpy((void *) &sc, seedtime+6, 1);
    memcpy((void *) &ms, seedtime+8, 2);
    ms /= 10;
    printf("seedtime yr = %d\n", (int) yr);
    printf("         da = %d\n", (int) da);
    printf("         hr = %d\n", (int) hr);
    printf("         mn = %d\n", (int) mn);
    printf("         sc = %d\n", (int) sc);
    printf("         ms = %d\n", (int) ms);
    printf("\n");

    stime = seed_ttodt(seedtime);
    printf("convert back: %.3lf (%s)\n", stime, util_dttostr(stime, 0));

    printf("difference = %.3lf\n", stime - dtime);

    exit(0);
}

#endif /* DEBBUG_TEST */

/* Revision History
 *
 * $Log: time.c,v $
 * Revision 1.3  2012/02/14 20:22:07  dechavez
 * use portable integer types (eg, long to INT32, unsigned long to UINT32)
 *
 * Revision 1.2  2007/01/11 17:48:03  dechavez
 * added platform.h and stdtypes.h stuff
 *
 * Revision 1.1.1.1  2000/02/08 20:20:39  dec
 * import existing IDA/NRTS sources
 *
 */
