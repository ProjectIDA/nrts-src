#pragma ident "$Id: histo.c,v 1.1 2011/03/17 20:23:20 dechavez Exp $"
#define   max(a,b)    ((a) > (b) ? (a) : (b))
#define   min(a,b)    ((a) < (b) ? (a) : (b))
#define   HIST_LINE  19000
#define   NMAX        2500
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
void histo(int, double*, FILE*);

void histo(nn, xx, pxy_file)
int nn;
double *xx;
FILE *pxy_file;

{
    int		nbin, ii, kk, binmax;
    const int	nmax = 2500;
    long	ibin[NMAX];
    double	xmin, xmax, xb, db;

    /* INITIALIZE */
    for (ii=0;  ii<NMAX;  ii++) ibin[ii] = 0;

    /* SET THE NUMBER OF BINS */
    nbin = 100;

    /* CALCULATE THE RANGE, MEAN, STD */

    xmin = xx[0];
    xmax = xx[0];
    for (ii=1;  ii<nn;  ii++)
    {
        xmin = min(xx[ii], xmin);
        xmax = max(xx[ii], xmax);
    }
    xmin = -10;
    xmax = 10;

    /* CALCULATE THE BIN SIZE AND DIVI UP THE SAMPLE */

    db = (xmax - xmin)/(nbin - 1);
    for (ii=0;  ii<nn;  ii++)
    {
        kk = (int)(0.000001 + (xx[ii] - xmin)/db);
        ibin[kk] = ibin[kk] + 1;
    }
    binmax = 0;
    for (ii=0;  ii<nbin;  ii++)
        binmax = max(binmax,ibin[ii]);

    /* PRINT COORDINATES OF HISTOGRAM BARS */

    for (ii=0, kk=0;  ii<nbin;  ii++)
        if (ibin[ii] > 0) kk += 3;
    fprintf(pxy_file,"color %d\n",HIST_LINE);
    fprintf(pxy_file,"read %d\n",kk);
    for (ii=0;  ii<nbin;  ii++)
        if (ibin[ii] > 0)
        {
            xb = xmin + (ii+0.5)*db;
            fprintf(pxy_file,"%f %d\n",xb,0);
            fprintf(pxy_file,"%f %d\n",xb,ibin[ii]);
            fprintf(pxy_file,"-99 -99\n");
        }
    fprintf(pxy_file,"ylimit 2.25 %d %d\n",-(int)(binmax*0.00),(int)(binmax*1.05));
    fprintf(pxy_file,"color 6000\n");
}

/* Revision History
 *
 * $Log: histo.c,v $
 * Revision 1.1  2011/03/17 20:23:20  dechavez
 * initial release, based on DCC time_vol sources
 *
 */
