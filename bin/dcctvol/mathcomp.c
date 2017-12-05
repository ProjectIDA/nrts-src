#pragma ident "$Id: mathcomp.c,v 1.1 2011/03/17 20:23:21 dechavez Exp $"
#define   max(a,b)    ((a) > (b) ? (a) : (b))
#define   min(a,b)    ((a) < (b) ? (a) : (b))
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
double slr(long, double*, double*, double*, double*, int, int);


/******************************************************************/
/******************************************************************/
void dind_combsort (nx, ix, x1)
long nx, *ix;
double *x1;
/*    1-field index Combsort */
/*    $$$$ calls no other routines */
/*    Combsort: souped up version of bubble sort */
/*    (Lacey & Box, Byte 16, p315, 1991); optimizes better than sort. */
{
    register long ngap, ii, jj, isw, itemp, kk;

    ngap=nx;
    do
    {
        /* COMPUTE THE JUMP GAP */
        ngap = (int)max((int)(ngap/1.3), 1);
        if (ngap==9 || ngap==10) ngap=11;
        isw=0;
        for (ii=0;  ii<nx-ngap;  ii++)
        {
            jj = ii + ngap;
            /* THIS IS THE CONDITIONAL FOR A 1 FIELD SORT */
            if (x1[ix[ii]] > x1[ix[jj]])
            {
                itemp=ix[ii];
                ix[ii]=ix[jj];
                ix[jj]=itemp;
                isw=1;
            }
        }
    }
    while (isw==1 || ngap>1);
   /*  return() */
}



/******************************************************************/
/******************************************************************/
double slr(nn, xx, yy, a0, a1, order, iter)
int order, iter;
long nn;
double *xx, *yy, *a0, *a1;


/*  COMPUTES THE LINEAR REGREESION COEFFIECNTS a0 AND a1 FOR THE
    VECTORS xx and yy, SUCH THAT
         yy = a0 + a1*xx + e
    RETURNS: THE SUM OF THE SQUARED RESIDUALS */

{
      long kk;
      double sx, sy, sx2, sy2, sxy, b0, b1, tmp;
	double	*b0t;
	long	*idx;
	double	btest, stddev;
	double xx0, yy0;

  /* reduce dynamic range of data */
      xx0 = xx[0];
      yy0 = yy[0];
      for (kk=0;  kk<nn;  kk++)
      {
	xx[kk] -= xx0;
	yy[kk] -= yy0;
      }

      tmp = sx = sy = sx2 = sy2 = sxy = 0.0;
      if (order == 0)
      {
          for (kk=0;  kk<nn;  kk++)
              sy  += yy[kk] - xx[kk];
          *a0 = sy/nn;
          *a1 = 1;
          for (kk=0;  kk<nn;  kk++)
              sx += (yy[kk]-*a0 - xx[kk])*(yy[kk]-*a0 - xx[kk]);
        /* reapply transformation */
	   *a0 = *a0 + yy0 - *a1 * xx0;
           for (kk=0;  kk<nn;  kk++)
	   {
		xx[kk] += xx0;
		yy[kk] += yy0;
	   }
          return(sx);
      }
 
      for (kk=0;  kk<nn;  kk++)
      {
          sx  += xx[kk];
          sx2 += xx[kk]*xx[kk];
          sy  += yy[kk];
          sy2 += yy[kk]*yy[kk];
          sxy += xx[kk]*yy[kk];
      }

      b1 = (nn*sxy - sx*sy) / (nn*sx2 - sx*sx);
      b0 = (sy - b1*sx) / nn;

  /* test slope and breadth of distribution: if not reasonable, try alternative: */
	btest = fabs(b1 - 1.0) ;
        for (kk=0, stddev = 0.0;  kk<nn;  kk++)
          stddev += (yy[kk]-b0-b1*xx[kk])*(yy[kk]-b0-b1*xx[kk]);
	stddev = 1000.0 * sqrt( stddev / (nn-1) ); /* std in msecs */

	if ( iter == 1 && (btest > 1e-7 || stddev > 10.0) )
	{
		idx = (long *) malloc(nn * sizeof(long));
		b0t = (double *) malloc(nn * sizeof(double));
		b1 = 1.0; /* assume zero drift */
		for (kk=0;  kk<nn;  kk++)
		{
			idx[kk] = kk;
			b0t[kk] = yy[kk] - b1 * xx[kk];
		}
	   /* sort residuals to find preliminary b0 */
		dind_combsort (nn, idx, b0t);
		b0 = b0t[idx[nn/2]];

		free(idx);
		free(b0t);
	}

  /* reapply transformation */
      *a0 = b0 + yy0 - b1 * xx0;;
      *a1 = b1;
      for (kk=0;  kk<nn;  kk++)
      {
	xx[kk] += xx0;
	yy[kk] += yy0;
      }
      /* THIS IS THE CLOSED FORM FOR THE RESIDUALS BUT IT'S UNSTABLE IF THE  */
      /* RESIDUALS ARE SEVERAL ORDERS OF MAGNITUDE LESS THAN THE VALUES  */
      /*  std = sy2 + nn*b0*b0 + b1*b1*sx2 + 2.0*(b0*b1*sx - b1*sxy - b0*sy); */
      for (kk=0;  kk<nn;  kk++)
          tmp += (yy[kk]-b0-b1*xx[kk])*(yy[kk]-b0-b1*xx[kk]);
      return(tmp);
}


/* Revision History
 *
 * $Log: mathcomp.c,v $
 * Revision 1.1  2011/03/17 20:23:21  dechavez
 * initial release, based on DCC time_vol sources
 *
 */
