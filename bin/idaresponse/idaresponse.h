#pragma ident "$Id: idaresponse.h,v 1.4 2015/12/08 21:10:56 dechavez Exp $"
#ifndef idaresponse_h_included
#define idaresponse_h_included
#include "platform.h"
#include "util.h"
#include "cssio.h"

extern char *VersionIdentString;

#define DEFAULT_RESP_PATH  "/ida/dcc/response/RESP"
#define DEFAULT_NETWORK    "II"
#define BS(value) utilBoolToString(value)

/* Function prototypes */

/* main.c */
int main(int argc, char **argv);

/* dofft.c */
void dofft(INT32 n, INT32 n_padded, REAL64 *s, INT32 ndir);

/* fft.c */
INT32 fftl(REAL64 *x, INT32 n, INT32 ndir);
void fft(REAL64 *a, REAL64 *b, INT32 ntot, INT32 n, INT32 nspan, INT32 isn, INT32 *ierr);

/* getresp.c */
INT32 getresp(WFDISC *outwd, REAL64 *amp, REAL64 *phase, INT32 nf, REAL64 df, REAL64 nyquist, INT32 acc, INT32 displ, INT32 ida, BOOL s1only, char *filenm);

/* resp.c */
INT32 resp(WFDISC *outwd, BOOL convolve, BOOL deconvolve, BOOL acc, BOOL displ, BOOL ida, BOOL s1only, char *filenm, REAL64 res_limit, char *prefix);

#endif /* idaresponse_h_included */

/* Revision History
 *
 * $Log: idaresponse.h,v $
 * Revision 1.4  2015/12/08 21:10:56  dechavez
 * updated prototypes
 *
 * Revision 1.3  2015/11/18 20:25:51  dechavez
 * added support for "s1" and "verbose" options
 *
 * Revision 1.2  2015/11/02 21:01:03  dechavez
 * changed default response file directory to /ida/dcc/response/RESP
 *
 * Revision 1.1  2014/04/15 21:12:48  dechavez
 * initial release using sources modified from DCC version
 *
 */
