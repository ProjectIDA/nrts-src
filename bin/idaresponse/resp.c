#pragma ident "$Id: resp.c,v 1.2 2015/11/18 20:25:51 dechavez Exp $"
/*======================================================================
 *
 *  Given information from wfdisc record, reads data, determines response,
 *  and operates on data accordingly.
 *
 *====================================================================*/
#include "idaresponse.h"

#define PI 3.14159265358979

static INT32 write_data(REAL64 *data, WFDISC *outwd, char *prefix)
{
INT32 i, np;
REAL32 f;
static BOOL  first = TRUE;
static INT32 foff ;
static char  filename[64], fname[128];
static FILE  *fpout;

/* if necessary, open the file */

    if (first) {
        sprintf(filename, "%s.w", prefix);
        sprintf(fname, "%s/%s", outwd->dir, filename);
        if ((fpout = fopen(fname, "wb")) == NULL) {
            fprintf(stderr, "Could not open file %s\n", fname);
            perror(fname);
            return(1);
        }
        foff = 0;
        first = FALSE;
    }

    outwd->foff = foff;
    outwd->calib = 1.0;
    strcpy(outwd->dfile, filename);
#ifdef BIG_ENDIAN_HOST
    strcpy(outwd->datatype, "t4");
#else
    strcpy(outwd->datatype, "f4");
#endif

    for (i=0; i<outwd->nsamp; i++) {
        f = (REAL32)data[i];
        np = fwrite(&f, sizeof(REAL32), 1, fpout);
        if (np == 1) foff += sizeof(REAL32);
    }

    return(0);
}

INT32 resp(WFDISC *wd, BOOL convolve, BOOL deconvolve, BOOL acc, BOOL displ, BOOL ida, BOOL s1only, char *respath, REAL64 res_limit, char *prefix)
{
INT32  i, j, k, ns, len;
INT32  iret ;
INT32  n, nf, nf1, npad, npp, nn;
REAL64 mean;
REAL64 df, nyquist;
REAL64 *data;
REAL64 *amp = NULL, *phase = NULL;
REAL64 ins_r, ins_i, r_r, r_i, rlength;
REAL64 max_amp ;
REAL64 dynlimit ;

/* if these data are compressed, skip them */

    if ( !strcmp(wd->datatype, "c2") || !strcmp(wd->datatype, "c4") ) {
        fprintf(stderr, "Skipping compressed data..\n");
        return(1);
    }

/* read data into a REAL64 array */

    npad = 2;
    while ( npad < wd->nsamp ) npad *= 2;
    npp = npad + 2;
    len = npp * sizeof(REAL64);
    if ((data = (REAL64 *) malloc(len)) == NULL) {
        perror("malloc");
        return(1);
    }
    memset(data, 0, len);

    if (!cssioReadDataIntoReal64(wd, data)) {
		perror("cssioReadDataIntoReal64");
        free(data);
        return(1);
    }

/* remove mean from data */

    mean = 0.0;
    for (i=0; i<wd->nsamp; i++) mean += data[i];
    mean /= (REAL64)wd->nsamp;
    for (i=0; i<wd->nsamp; i++) data[i] = data[i] - mean;;

/* get response */

    df      = wd->smprate / (REAL64)(npad);
    nyquist = wd->smprate / 2.;
    nf      = (npad + 2) / 2;
    nf1     = nf - 1 ;
    amp     = (REAL64 *) malloc(nf1 * sizeof(REAL64));
    phase   = (REAL64 *) malloc(nf1 * sizeof(REAL64));

    if (getresp(wd, amp, phase, nf1, df, nyquist, acc, displ, ida, s1only, respath) != 0) {
        free(amp);
        free(phase);
        return(1);
    }

/* if deconvolving, curb upper freqs */

    if ( deconvolve ) {
        nn = (int)( 0.95 * nf1 );
        for (i=nn; i<nf1; i++) amp[i] *= exp( 5 * (i - nn) / (nf1 - nn ) ); 
    }

/* limit dynamic range, if desired */

    if ( res_limit > 0. ) {
        for (i=0, max_amp=0; i<nf1; i++) max_amp = (amp[i] > max_amp) ? amp[i] : max_amp;
        dynlimit = max_amp * pow(10, (-res_limit/10.0));
        for (i=0; i<nf1; i++) if ( amp[i] < dynlimit ) amp[i] = dynlimit;
    }

/* tranform data, convolve/deconvolve IR, transform back */

    dofft(wd->nsamp, npad, data, -1);

    for(i=0; i < nf1; i ++) {
        ins_r = amp[i] * cos(phase[i] * PI / 180.) ;
        ins_i = amp[i] * sin(phase[i] * PI / 180.) ;
        j = 2 * i + 2 ;
        k = j + 1;
        if ( convolve ) {
            r_r = (REAL64)data[j] * ins_r - (REAL64)data[k] * ins_i ;
            r_i = (REAL64)data[j] * ins_i + (REAL64)data[k] * ins_r ;
        } else {
            rlength = ins_r * ins_r + ins_i * ins_i ;
            r_r =  (REAL64)data[j] * ins_r + (REAL64)data[k] * ins_i ;
            r_i = -(REAL64)data[j] * ins_i + (REAL64)data[k] * ins_r ;
            r_r /= rlength;
            r_i /= rlength;
        }
        data[j] = r_r ;
        data[k] = r_i ;
    }

    free(amp);
    free(phase);

    dofft(wd->nsamp, npad, data, -2);

/* write out output data */

    if ( write_data(data, wd, prefix) != 0) {
        free(data);
        return(1);
    }

    free(data);
    return(0);
}

/* Revision History
 *
 * $Log: resp.c,v $
 * Revision 1.2  2015/11/18 20:25:51  dechavez
 * added support for "s1" and "verbose" options
 *
 * Revision 1.1  2014/04/15 21:12:48  dechavez
 * initial release using sources modified from DCC version
 *
 */
