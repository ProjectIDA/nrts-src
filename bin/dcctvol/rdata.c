#pragma ident "$Id: rdata.c,v 1.1 2011/03/17 20:23:21 dechavez Exp $"
/*======================================================================
 *
 *  rdata.c
 *
 *  Read the next record.
 *
 *====================================================================*/
#include "globals.h"


int rdata(FILE *fp)
{
    int				bytes, status;
    int				i, type;
    static int			first = TRUE;
    static unsigned char	nullrec[IDA10_MAXHEADLEN];

/*  The first time through, initialize the null record  */

    if (first)
    {
        memset(nullrec, (unsigned char) 0, IDA10_MAXHEADLEN);
        first = FALSE;
    }

    if ((bytes = ida10ReadRecord(fp, buffer, IDA10_MAXHEADLEN, &type, TRUE)) > 0) {
    	switch (buffer[0]) {
          case 'D': type = DREC; ++cnt.Drec; break;
          case 'A': type = AREC; ++cnt.Arec; break;
          case 'C': type = CREC; ++cnt.Crec; break;
          case 'H': type = HREC; ++cnt.Hrec; break;
          case 'K': type = KREC; ++cnt.Krec; break;
          case 'E': type = EREC; ++cnt.Erec; break;
          case 'L': type = LREC; ++cnt.Lrec; break;
          case 'P': type = PREC; ++cnt.Prec; break;
          case 'R': type = RREC; ++cnt.Rrec; break;
          case 'T': type = TREC; ++cnt.Trec; break;
          default:  type =
	     (memcmp(buffer, nullrec, IDA10_MAXHEADLEN) == 0) ? ZREC : UREC;
        }
		++cnt.rec;
		return type;
	}

	status = bytes;
	if (status == IDA10_EOF) {
		return DONE;
	} else {
		fprintf(stderr, "ida10ReadGz failed at record no. %d: %s\n", ++cnt.rec, ida10ErrorString(status));
		return ERR_RET;
	}
}

/* Revision History
 *
 * $Log: rdata.c,v $
 * Revision 1.1  2011/03/17 20:23:21  dechavez
 * initial release, based on DCC time_vol sources
 *
 */
