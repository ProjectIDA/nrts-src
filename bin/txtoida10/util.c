#pragma ident "$Id: util.c,v 1.1 2011/10/04 19:49:28 dechavez Exp $"
/*======================================================================
 *
 *  Various common convenience functions
 *
 *====================================================================*/
#include "txtoida10.h"

void BuildIdentString(char *dest, char *nname, char *sname, char *cname)
{
static char *hash = "#";

    if (nname == NULL) nname = hash;
    if (sname == NULL) sname = hash;
    if (cname == NULL) cname = hash;

    sprintf(dest, "%s-%s-%s", nname, sname, cname);
}

void BuildGentag(IDA10_GENTAG *dest, TIMESTAMP *tstamp)
{
int doy, sec, msec;
REAL64 dtime;

    doy = utilYmdtojd(tstamp->year, tstamp->month, tstamp->day);
    sec = (int) tstamp->second;
    msec = (int) ((tstamp->second - (float) sec) * 1000.0);
    dtime = utilYdhmsmtod(tstamp->year, doy, tstamp->hour, tstamp->minute, sec, msec);
    dtime -= SAN_EPOCH_TO_1970_EPOCH; /* convert secs since 1/1/1970 to secs since 1/1/1999 */
    dest->tstamp = (UINT64) (dtime * (REAL64) NANOSEC_PER_SEC);
    dest->status.receiver = dest->status.generic = tstamp->status;
}

/* Revision History
 *
 * $Log: util.c,v $
 * Revision 1.1  2011/10/04 19:49:28  dechavez
 * initial release
 *
 */
