#pragma ident "$Id: tear.c,v 1.2 2012/02/14 20:22:07 dechavez Exp $"
/*======================================================================
 *
 *  Look for time tears.
 *
 * - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 * Copyright (c) 1997 Regents of the University of California.
 * All rights reserved.
 *====================================================================*/
#include <errno.h>
#include "seed.h"

int seed_timetear(struct seed_minipacket *prev, struct seed_minipacket *crnt, INT32 *error)
{
INT32 expected;
static char *fid = "seed_timetear";

    if (prev == (struct seed_minipacket *) NULL) {
        errno = EINVAL;
        return -1;
    }

    if (crnt == (struct seed_minipacket *) NULL) {
        errno = EINVAL;
        return -2;
    }

    if (strcasecmp(prev->sname, crnt->sname) != 0) {
        util_log(1, "%s: sname mismatch: %s != %s",
            fid, prev->sname, crnt->sname
        );
        errno = EINVAL;
        return -3;
    }

    if (strcasecmp(prev->cname, crnt->cname) != 0) {
        util_log(1, "%s: cname mismatch: %s != %s",
            fid, prev->cname, crnt->cname
        );
        errno = EINVAL;
        return -4;
    }

    if (prev->sint != crnt->sint) {
        util_log(1, "%s: sint mismatch: %.3f != %.3f",
            fid, prev->sint, crnt->sint
        );
        errno = EINVAL;
        return -5;
    }

    expected = (INT32) ((crnt->beg - prev->beg) / (double) prev->sint);
    *error = expected - prev->nsamp;

    return 0;
}

/* Revision History
 *
 * $Log: tear.c,v $
 * Revision 1.2  2012/02/14 20:22:07  dechavez
 * use portable integer types (eg, long to INT32, unsigned long to UINT32)
 *
 * Revision 1.1.1.1  2000/02/08 20:20:39  dec
 * import existing IDA/NRTS sources
 *
 */
