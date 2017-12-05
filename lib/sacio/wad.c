#pragma ident "$Id: wad.c,v 1.2 2012/02/14 21:45:40 dechavez Exp $"
/*======================================================================
 *
 *  Write SAC data to disk as ascii.
 *
 *====================================================================*/
#include <stdio.h>
#include "sacio.h"

int sacio_wad(FILE  *fp, float *buffer, INT32  npts, INT32 *count)
{
int i;

    clearerr(fp);

    for (i = 0; i < npts; i++, *count += 1) {
        fprintf(fp, "%15.6f", buffer[i]);
        if (((*count + 1) % 5) == 0 && *count > 0) fprintf(fp, "\n");
    }

    return ferror(fp) ? -1 : npts;
}

/* Revision History
 *
 * $Log: wad.c,v $
 * Revision 1.2  2012/02/14 21:45:40  dechavez
 * use portable integer types (eg, long to INT32, unsigned long to UINT32)
 *
 * Revision 1.1.1.1  2000/02/08 20:20:37  dec
 * import existing IDA/NRTS sources
 *
 */
