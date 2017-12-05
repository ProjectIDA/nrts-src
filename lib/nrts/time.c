#pragma ident "$Id: time.c,v 1.4 2012/02/14 20:19:05 dechavez Exp $"
/*======================================================================
 *
 *  Convert from double to nrts_times and back.
 *
 *====================================================================*/
#include "nrts.h"

struct nrts_time *nrts_time(dtime)
double dtime;
{
static struct nrts_time ntime;

    ntime.sec = (INT32) dtime;
    ntime.msec = (int) ((dtime - (double) ntime.sec) * (double) 1000.0);

    return &ntime;
}

double nrts_dtime(ntime)
struct nrts_time *ntime;
{
    return (double) ntime->sec + ((double) ntime->msec / (double) 1000.0);
}

/* Revision History
 *
 * $Log: time.c,v $
 * Revision 1.4  2012/02/14 20:19:05  dechavez
 * use portable integer types (eg, long to INT32, unsigned long to UINT32)
 *
 * Revision 1.3  2005/05/25 22:39:50  dechavez
 * mods to calm Visual C++ warnings
 *
 * Revision 1.2  2004/06/24 17:33:16  dechavez
 * removed unnecessary includes (aap)
 *
 * Revision 1.1.1.1  2000/02/08 20:20:32  dec
 * import existing IDA/NRTS sources
 *
 */
