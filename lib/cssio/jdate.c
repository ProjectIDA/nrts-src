#pragma ident "$Id: jdate.c,v 1.4 2015/08/24 19:03:57 dechavez Exp $"
/*======================================================================
 *
 *  Given an epoch time, return the equivalent jdate.
 * 
 * - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 * Copyright (c) 1997 Regents of the University of California.
 * All rights reserved.
 *====================================================================*/
#include "cssio.h"

#ifdef HAVE_GMTIME_R

INT32 cssio_jdate(double dtime)
{
time_t ltime;
struct tm tm;

    ltime = (time_t) dtime;
    gmtime_r(&ltime, &tm);
    return ((1900 + tm.tm_year) * 1000) + tm.tm_yday + 1;
}

#else

INT32 cssio_jdate(double dtime)
{
INT32  ltime;
struct tm *tiempo;

    ltime = (INT32) dtime;
    tiempo = gmtime(&ltime);
    return ((1900 + tiempo->tm_year) * 1000) + tiempo->tm_yday + 1;
}

#endif /* !HAVE_GMTIME_R */

/* Revision History
 *
 * $Log: jdate.c,v $
 * Revision 1.4  2015/08/24 19:03:57  dechavez
 * use gmtime_r() instead of gmtime_().  Must have been a typo???
 *
 * Revision 1.3  2012/02/14 21:45:30  dechavez
 * use portable integer types (eg, long to INT32, unsigned long to UINT32)
 *
 * Revision 1.2  2007/01/04 23:33:39  dechavez
 * Changes to accomodate OpenBSD builds
 *
 * Revision 1.1.1.1  2000/02/08 20:20:23  dec
 * import existing IDA/NRTS sources
 *
 */
