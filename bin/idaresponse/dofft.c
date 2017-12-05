#pragma ident "$Id: dofft.c,v 1.2 2015/12/08 21:10:43 dechavez Exp $"
#include "idaresponse.h"

void dofft(INT32 n, INT32 n_padded, REAL64 *s, INT32 ndir)
{
INT32	i;
REAL64	a;

	a = sqrt((double)(n_padded))/2.;

	if(abs(ndir) == 1) {
		for(i = 0; i < n; i++) s[i] *= a;
		fftl(s, n_padded, ndir);
	} else {
		fftl(s, n_padded, ndir);
		for(i = 0; i < n; i++) s[i] /= a;
	}
}

/* Revision History
 *
 * $Log: dofft.c,v $
 * Revision 1.2  2015/12/08 21:10:43  dechavez
 * declare as void since function has no return value
 *
 * Revision 1.1  2014/04/15 21:12:47  dechavez
 * initial release using sources modified from DCC version
 *
 */
