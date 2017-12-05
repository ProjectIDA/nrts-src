#pragma ident "$Id: fft.c,v 1.1 2014/04/15 21:12:48 dechavez Exp $"
#include "idaresponse.h"

#define mod(i,j) (i-((i)/(j))*(j))

static INT32 factor(INT32 n)
{
/*
* factor determines the smallest even number >= n that can be
* factored into products of primes in the array np. the result is
* returned as factor. only the first four primes are used. this
* routine is called before using fftl.
*/
	static INT32 np[4] = {2, 3, 5, 7};
	INT32 i, nf, m;

	if((m = n) <= 0) return(0);
	if(m != 2*(m/2)) m++;

	for(;;)
	{
		nf = m;
		for(i = 0; i < 4; i++)
		{
		         while(mod(nf, np[i]) == 0) nf /= np[i];
		}
		if(nf == 1) return(m);
		m += 2;
	}
}

static INT32 facdwn(INT32 n)
{
/*
* facdwn determines the largest even number <= n that can be factored
* into products of primes in the array np. facdwn returns the result.
* only the first four primes are used. this routine is called before
* using fftl.
*/
	static INT32 np[4] = {2, 3, 5, 7};
	INT32 i, nf, m;

	if((m = n) <= 0) return(0);
	if(m != 2*(m/2)) m--;

	for(;;)
	{
		nf = m;
		for(i = 0; i < 4; i++)
		{
			while(mod(nf, np[i]) == 0) nf /= np[i];
		}
		if(nf == 1) return(m);
		m -= 2;
	}  
}

static void realtr(REAL64 *a, REAL64 *b, INT32 n, INT32 isn)
{
/*
   title - realtr = real tranform
   fourier transform of real series from output of fft


                ---astract---
     realtr (a,b,n,isn)
        if isn=1, this subroutine completes the fourier trans-
        form of 2*n real data values, where the original data
        values are stored alternatively in arrays a and b, and are
        first transformed by a complex fourier transform of
        dimension n.
        the cosine coefficients are in a[0],a[1],...a[n] and
        the sine coefficients are in b[0],b[1],...b[n].
        a typical calling sequence is
        fft (a,b,n,1)
        realtr (a,b,n,1)
        the results should be muliplied by 0.5/n to give the
        usual scaling of coefficients.
        if isn=1, the inverse transformation is done, the
        first step in evaluating a real fourier series.
        a typical calling sequence is
        realtr (a,b,n,-1)
        fft (a,b,n,-1)
        the results should be multiplied by 0.5 to give the
        usual scaling, and the time domain results alternate
        in arrays a and b, ie. a[0],b[0], a[1],b[1],
        ...a[n-1],b[n-1].
        the data may alternatively be stored in a single complex
        array a, then the magnitude of isn changed to two to
        give the correct indexing increment and a[1] used to pass
        the initial address for the sequence of imaginary values:
        fft(a,a+1,n,2)
        realtr(a,a+1,n,2)
        in this case, the cosine and sine coefficients alternate
        in a.
*/
   INT32 j,k,nk,nh,inc ;
   REAL64 sd,cd,sn,cn,aa,ab,ba,bb,xx,yy ;

   if(n <= 1) return ;
   inc = isn ;
   if(inc < 0) inc = -inc ;
   nk = n*inc + 2 ;
   nh = nk/2 ;
   sd = 3.1415926535898/(2.*n) ;
   cd = 2.*pow(sin(sd),2.) ;
   sd = sin(sd+sd) ;
   sn = 0. ;
   if(isn < 0){
      cn = -1. ;
      sd = -sd ;
   }
   else {
      cn = 1. ;
      a[nk-2] = a[0] ;
      b[nk-2] = b[0] ;
   }
   for(j=0; j<nh; j+=inc){
      k = nk - j - 2 ;
      aa = a[j] + a[k] ;
      ab = a[j] - a[k] ;
      ba = b[j] + b[k] ;
      bb = b[j] - b[k] ;
      xx = cn*ba + sn*ab ;
      yy = sn*ba - cn*ab ;
      b[k] = yy - bb ;
      b[j] = yy + bb ;
      a[k] = aa - xx ;
      a[j] = aa + xx ;
      aa = cn - (cd*cn + sd*sn) ;
      sn = (sd*cn - cd*sn) + sn ;
      cn = aa ;
   }
}


INT32 fftl(REAL64 *x, INT32 n, INT32 ndir)
{
/*
 *  if iabs(ndir)==1 fftl fourier transforms the n point real time
 *  series in array x. the result overwrites x stored as (n+2)/2
 *  complex numbers (non-negative frequencies only). if iabs(ndir)
 *  ==2, fftl fourier transforms the (n+2)/2 complex fourier
 *  coefficients (non-negative frequencies only) in array x
 *  (assuming the series is hermitian). the resulting n point real
 *  time series overwrites x.  if ndir>0 the forward transform
 *  uses the sign convention exp(i*w*t). if ndir <0 the forward
 *  transform uses the sign convention exp(-i*w*t). The forward
 *  transform is normalized such that a sine wave of unit amplitude
 *  is transformed into delta functions of unit amplitude. the 
 *  backward transform is the original series. ierr is normally zero.
 *  if ierr==1 then fftl has not been able to factor the series.
 *  however, x has been scrambled by realtr. note that if n is odd
 *  the last point will not be used in the transform.
 *
 *                                                -rpb
 */
   INT32 i, n1, n2, ierr;
   REAL64 scale;

   n2 = n/2 ;
   if(abs(ndir) == 1){
      /* do forward transform (ie. time to frequency). */
      fft(x,x+1,n2,n2,n2,2,&ierr) ;
      realtr(x,x+1,n2,2) ;
      n1 = 2*n2 + 2 ;
      scale = 1./n ;
      if(ndir < 0) for(i=3; i<n; i+=2) x[i] = -x[i] ;
   }
   else if(abs(ndir) == 2){
      /* do backward transform (ie. frequency to time). */
      if(ndir < 0) for(i=3; i<n; i+=2) x[i] = -x[i] ;
      n1 = 2*n2 ;
      x[1] = 0. ;
      x[n1+1] = 0. ;
      realtr(x,x+1,n2,-2) ;
      fft(x,x+1,n2,n2,n2,-2,&ierr) ;
      scale = .5 ;
   }
   else {
       return(-1) ;
   }
   /* normalize the transform */
   for(i=0; i<n1; i++) x[i] *= scale ;
   return(ierr);
}

void fft(REAL64 *a, REAL64 *b, INT32 ntot, INT32 n, INT32 nspan, INT32 isn, INT32 *ierr)
{
/*
   multivariate complex fourier tranform, computed in place
   using mixed-radix fast fourier tranform algorithm.
   by r.c. singleton, stanford research institute, sept. 1968
   arrays a and b originally hold the real and imaginary
   components of the data, and return the real and imaginary
   components of the resulting fourier coefficients.
   multivariate data is indexed according to the fortran
   array element successor function, without limit on the
   number of implied muliple subscripts.
   the subroutine is called once for each variate
   the calls for a multivariate transform may be in any order.
   ntot is the total number of complex data values.
   n is the dimension of the current variable.
   nspan/n is the spacing of consecutive data values while
           indexing the current variable.
   the integer ierr is an error return indicator.  it is normally
   zero, but is set to 1 if the number of terms cannot be
   factored in the space available.  if it is permissible, the
   appropriate action at this stage is to enter fft again after
   reducing the length of the series by one term. the sign of isn
   determines the sign of the complex exponential, and the
   magnitude of isn is normally one. a tri-variate transform with
   a(n1,n2,n3), b(n1,n2,n3) is computed by
      call fft(a,b,n1*n2*n3,n1,n1,1)
      call fft(a,b,n1*n2*n3,n2,n1*n2,1)
      call fft(a,b,n1*n2*n3,n3,n1*n2*n3,1)
   for a single-variate transform,
      ntot = n = nspan = (number of complex data values), e.g.
      call fft(a,b,n,n,n,1)
   with most fortran compilers the data can alternatively be
   stored in a single complex array a, then the maginitude of isn
   changed to two to give the correct indexing increment and
   a[1] used to pass the initial address for the sequence of
   imaginary values, e.g.
      call fft(a,a+1,ntot,n,nspan,2)
   arrays at(maxf), ck(maxf), bt(maxf), sk(maxf), and np(maxf)
   are used for temporary storage. if the available storage is
   insufficient, the program is terminated by the error return
   option. maxf must be >= the maximum prime factor of n.
   maxp must be > the number of prime factors of n. in
   addition, if the square-free portion k of n has two or more
   prime factors, then maxp must be >= k-1. array storage in
   nfac for a maximum of 11 prime factors of n. if n has more
   than one square-free factor, the product of the square-free
   factors must be <= 210  (2**5*7=210)
*/

   /* array storage for maximum prime factor of 23 */
#define MAXF 23
#define MAXP 209
   INT32 nfac[11],np[MAXP] ;
   REAL64 at[MAXF],ck[MAXF],bt[MAXF],sk[MAXF] ;
   INT32 i,j,k,l,m,jj,jc,jf,kk,k1,k2,k3,k4,ks,kspan,kspnn,inc,nn,nt,
       maxf,maxp,kt,le ; 
   REAL64 c72,s72,s120,rad,radf,sd,cd,ak,bk,c1,s1,bjp,bjm,c2,s2,c3,s3,
         akp,akm,ajp,ajm,bkp,bkm,aj,bj,aa,bb,aai,bbi,aki,bki,aji,bji ;

   maxf = MAXF ;
   maxp = MAXP ;
   *ierr = 0 ;
   if(n < 2) return ;
   inc = isn ;
   c72 = 0.30901699437494 ;
   s72 = 0.95105651629515 ;
   s120 = 0.86602540378443 ;
   rad = 6.2831853071796 ;
   if(isn < 0){
      s72 = -s72 ;
      s120 = -s120 ;
      rad = -rad ;
      inc = -inc ;
   }
   nt = inc*ntot ;
   ks = inc*nspan ;
   kspan = ks ;
   nn = nt - inc ;
   jc = ks/n ;
   radf = rad*(jc*.5) ;
   i = 0 ;
   jf = 0  ;
   /* determine the factors of n */
   m = 0 ;
   k = n ;
   while(k-16*(k/16) == 0){
      m = m + 1 ;
      nfac[m-1] = 4 ;
      k = k/16 ;
   }
   j = 3 ;
   jj = 9 ;
   do {
      while(mod(k,jj) == 0){
         m = m + 1 ;
         nfac[m-1] = j ;
         k = k/jj ;
      }
      j = j + 2 ;
      jj = j*j ;
   } while(jj <= k) ;
   if(k <= 4){
      kt = m ;
      nfac[m+1-1] = k ;
      if(k != 1) m = m + 1 ;
   }
   else {
      if(k-4*(k/4) == 0){
         m = m + 1 ;
         nfac[m-1] = 2 ;
         k = k/4 ;
      }
      kt = m ;
      j = 2 ;
      do {
         if(mod(k,j) == 0){
            m = m + 1 ;
            nfac[m-1] = j ;
            k = k/j ;
         }
         j = ((j+1)/2)*2 + 1 ;
      } while(j <= k) ;
   }
   if(kt != 0){
      j = kt ;
      do {
         m = m + 1 ;
         nfac[m-1] = nfac[j-1] ;
         j = j - 1 ;
      } while(j != 0) ;
   }
   /* compute fourier transform */
 L100:sd = radf/(kspan) ;
      cd = 2.*pow(sin(sd),2.) ;
      sd = sin(sd+sd) ;
      kk = 1 ;
      i = i + 1 ;
      if(nfac[i-1] != 2) goto L400 ;
      /* transform for factor of 2 (including rotation factor) */
      kspan = kspan/2 ;
      k1 = kspan + 2 ;
 L210:k2 = kk + kspan ;
      ak = a[k2-1] ;
      bk = b[k2-1] ;
      a[k2-1] = a[kk-1] - ak ;
      b[k2-1] = b[kk-1] - bk ;
      a[kk-1] = a[kk-1] + ak ;
      b[kk-1] = b[kk-1] + bk ;
      kk = k2 + kspan ;
      if(kk <= nn) goto L210 ;
      kk = kk - nn ;
      if(kk <= jc) goto L210 ;
      if(kk > kspan) goto L800 ;
 L220:c1 = 1. - cd ;
      s1 = sd ;
 L230:k2 = kk + kspan ;
      ak = a[kk-1] - a[k2-1] ;
      bk = b[kk-1] - b[k2-1] ;
      a[kk-1] = a[kk-1] + a[k2-1] ;
      b[kk-1] = b[kk-1] + b[k2-1] ;
      a[k2-1] = c1*ak - s1*bk ;
      b[k2-1] = s1*ak + c1*bk ;
      kk = k2 + kspan ;
      if(kk < nt) goto L230 ;
      k2 = kk - nt ;
      c1 = -c1 ;
      kk = k1 - k2 ;
      if(kk > k2) goto L230 ;
      ak = cd*c1 + sd*s1 ;
      s1 = (sd*c1 - cd*s1) + s1 ;
      c1 = c1 - ak ;
      kk = kk + jc ;
      if(kk < k2) goto L230 ;
      k1 = k1 + inc+inc ;
      kk = (k1-kspan)/2 + jc ;
      if(kk <= jc+jc) goto L220 ;
      goto L100 ;
      /* transform for factor of 3 (optional code) */
 L320:k1 = kk + kspan ;
      k2 = k1 + kspan ;
      ak = a[kk-1] ;
      bk = b[kk-1] ;
      aj = a[k1-1] + a[k2-1] ;
      bj = b[k1-1] + b[k2-1] ;
      a[kk-1] = ak + aj ;
      b[kk-1] = bk + bj ;
      ak = -.5*aj + ak ;
      bk = -.5*bj + bk ;
      aj = (a[k1-1]-a[k2-1])*s120 ;
      bj = (b[k1-1]-b[k2-1])*s120 ;
      a[k1-1] = ak - bj ;
      b[k1-1] = bk + aj ;
      a[k2-1] = ak + bj ;
      b[k2-1] = bk - aj ;
      kk = k2 + kspan ;
      if(kk < nn) goto L320 ;
      kk = kk - nn ;
      if(kk <= kspan) goto L320 ;
      goto L700 ;
      /* transform for factor of 4 */
 L400:if(nfac[i-1] != 4) goto L600 ;
      kspnn = kspan ;
      kspan = kspan/4 ;
 L410:c1 = 1. ;
      s1 = 0. ;
 L420:k1 = kk + kspan ;
      k2 = k1 + kspan ;
      k3 = k2 + kspan ;
      if(isn >= 0){
         if(s1 != 0){
            akp = a[kk-1] + a[k2-1] ;
            akm = a[kk-1] - a[k2-1] ;
            ajp = a[k1-1] + a[k3-1] ;
            ajm = a[k1-1] - a[k3-1] ;
            a[kk-1] = akp + ajp ;
            ajp = akp - ajp ;
            bkp = b[kk-1] + b[k2-1] ;
            bkm = b[kk-1] - b[k2-1] ;
            bjp = b[k1-1] + b[k3-1] ;
            bjm = b[k1-1] - b[k3-1] ;
            b[kk-1] = bkp + bjp ;
            bjp = bkp - bjp ;
            akp = akm - bjm ;
            akm = akm + bjm ;
            bkp = bkm + ajm ;
            bkm = bkm - ajm ;
            a[k1-1] = akp*c1 - bkp*s1 ;
            b[k1-1] = akp*s1 + bkp*c1 ;
            a[k2-1] = ajp*c2 - bjp*s2 ;
            b[k2-1] = ajp*s2 + bjp*c2 ;
            a[k3-1] = akm*c3 - bkm*s3 ;
            b[k3-1] = akm*s3 + bkm*c3 ;
         }
         else {
            akp = a[kk-1] + a[k2-1] ;
            akm = a[kk-1] - a[k2-1] ;
            ajp = a[k1-1] + a[k3-1] ;
            ajm = a[k1-1] - a[k3-1] ;
            a[kk-1] = akp + ajp ;
            ajp = akp - ajp ;
            bkp = b[kk-1] + b[k2-1] ;
            bkm = b[kk-1] - b[k2-1] ;
            bjp = b[k1-1] + b[k3-1] ;
            bjm = b[k1-1] - b[k3-1] ;
            b[kk-1] = bkp + bjp ;
            bjp = bkp - bjp ;
            akp = akm - bjm ;
            akm = akm + bjm ;
            bkp = bkm + ajm ;
            bkm = bkm - ajm ;
            a[k1-1] = akp ;
            b[k1-1] = bkp ;
            a[k2-1] = ajp ;
            b[k2-1] = bjp ;
            a[k3-1] = akm ;
            b[k3-1] = bkm ;
         }
      }
      else {
         if(s1 != 0){
            akp = a[kk-1] + a[k2-1] ;
            akm = a[kk-1] - a[k2-1] ;
            ajp = a[k1-1] + a[k3-1] ;
            ajm = a[k1-1] - a[k3-1] ;
            a[kk-1] = akp + ajp ;
            ajp = akp - ajp ;
            bkp = b[kk-1] + b[k2-1] ;
            bkm = b[kk-1] - b[k2-1] ;
            bjp = b[k1-1] + b[k3-1] ;
            bjm = b[k1-1] - b[k3-1] ;
            b[kk-1] = bkp + bjp ;
            bjp = bkp - bjp ;
            akp = akm + bjm ;
            akm = akm - bjm ;
            bkp = bkm - ajm ;
            bkm = bkm + ajm ;
            a[k1-1] = akp*c1 - bkp*s1 ;
            b[k1-1] = akp*s1 + bkp*c1 ;
            a[k2-1] = ajp*c2 - bjp*s2 ;
            b[k2-1] = ajp*s2 + bjp*c2 ;
            a[k3-1] = akm*c3 - bkm*s3 ;
            b[k3-1] = akm*s3 + bkm*c3 ;
         }
         else {
            akp = a[kk-1] + a[k2-1] ;
            akm = a[kk-1] - a[k2-1] ;
            ajp = a[k1-1] + a[k3-1] ;
            ajm = a[k1-1] - a[k3-1] ;
            a[kk-1] = akp + ajp ;
            ajp = akp - ajp ;
            bkp = b[kk-1] + b[k2-1] ;
            bkm = b[kk-1] - b[k2-1] ;
            bjp = b[k1-1] + b[k3-1] ;
            bjm = b[k1-1] - b[k3-1] ;
            b[kk-1] = bkp + bjp ;
            bjp = bkp - bjp ;
            akp = akm + bjm ;
            akm = akm - bjm ;
            bkp = bkm - ajm ;
            bkm = bkm + ajm ;
            a[k1-1] = akp ;
            b[k1-1] = bkp ;
            a[k2-1] = ajp ;
            b[k2-1] = bjp ;
            a[k3-1] = akm ;
            b[k3-1] = bkm ;
         }
      }
      kk = k3 + kspan ;
      if(kk <= nt) goto L420 ;
      c2 = cd*c1 + sd*s1 ;
      s1 = (sd*c1 - cd*s1) + s1 ;
      c1 = c1 - c2 ;
      c2 = c1*c1 - s1*s1 ;
      s2 = 2.*c1*s1 ;
      c3 = c2*c1 - s2*s1 ;
      s3 = c2*s1 + s2*c1 ;
      kk = kk - nt + jc ;
      if(kk <= kspan) goto L420 ;
      kk = kk - kspan + inc ;
      if(kk <= jc) goto L410 ;
      if(kspan == jc) goto L800 ;
      goto L100 ;
      /* transform for factor of 5 (optional code) */
 L510:c2 = c72*c72 - s72*s72 ;
      s2 = 2.*c72*s72 ;
 L520:k1 = kk + kspan ;
      k2 = k1 + kspan ;
      k3 = k2 + kspan ;
      k4 = k3 + kspan ;
      akp = a[k1-1] + a[k4-1] ;
      akm = a[k1-1] - a[k4-1] ;
      bkp = b[k1-1] + b[k4-1] ;
      bkm = b[k1-1] - b[k4-1] ;
      ajp = a[k2-1] + a[k3-1] ;
      ajm = a[k2-1] - a[k3-1] ;
      bjp = b[k2-1] + b[k3-1] ;
      bjm = b[k2-1] - b[k3-1] ;
      aa = a[kk-1] ;
      bb = b[kk-1] ;
      a[kk-1] = aa + akp + ajp ;
      b[kk-1] = bb + bkp + bjp ;
      ak = akp*c72 + ajp*c2 + aa ;
      bk = bkp*c72 + bjp*c2 + bb ;
      aj = akm*s72 + ajm*s2 ;
      bj = bkm*s72 + bjm*s2 ;
      a[k1-1] = ak - bj ;
      a[k4-1] = ak + bj ;
      b[k1-1] = bk + aj ;
      b[k4-1] = bk - aj ;
      ak = akp*c2 + ajp*c72 + aa ;
      bk = bkp*c2 + bjp*c72 + bb ;
      aj = akm*s2 - ajm*s72 ;
      bj = bkm*s2 - bjm*s72 ;
      a[k2-1] = ak - bj ;
      a[k3-1] = ak + bj ;
      b[k2-1] = bk + aj ;
      b[k3-1] = bk - aj ;
      kk = k4 + kspan ;
      if(kk < nn) goto L520 ;
      kk = kk - nn ;
      if(kk <= kspan) goto L520 ;
      goto L700 ;
      /* transform for odd factors */
 L600:k = nfac[i-1] ;
      kspnn = kspan ;
      kspan = kspan/k ;
      if(k == 3) goto L320 ;
      if(k == 5) goto L510 ;
      if(k ==jf) goto L640 ;
      jf = k ;
      s1 = rad/k ;
      c1 = cos(s1) ;
      s1 = sin(s1) ;
      if(jf > maxf) goto L998 ;
      ck[jf-1] = 1. ;
      sk[jf-1] = 0. ;
      j = 1 ;
 L630:ck[j-1] = ck[k-1]*c1 + sk[k-1]*s1 ;
      sk[j-1] = ck[k-1]*s1 - sk[k-1]*c1 ;
      k = k - 1 ;
      ck[k-1] = ck[j-1] ;
      sk[k-1] = -sk[j-1] ;
      j = j + 1 ;
      if(j < k) goto L630 ;
 L640:k1 = kk ;
      k2 = kk + kspnn ;
      aai = a[kk-1] ;
      bbi = b[kk-1] ;
      aki = aai ;
      bki = bbi ;
      j = 1 ;
      k1 = k1 + kspan ;
 L650:k2 = k2 - kspan ;
      j = j + 1 ;
      at[j-1] = a[k1-1] + a[k2-1] ;
      aki = at[j-1] + aki ;
      bt[j-1] = b[k1-1] + b[k2-1] ;
      bki = bt[j-1] + bki ;
      j = j + 1 ;
      at[j-1] = a[k1-1] - a[k2-1] ;
      bt[j-1] = b[k1-1] - b[k2-1] ;
      k1 = k1 + kspan ;
      if(k1 < k2) goto L650 ;
      a[kk-1] = aki ;
      b[kk-1] = bki ;
      k1 = kk ;
      k2 = kk + kspnn ;
      j = 1 ;
 L660:k1 = k1 + kspan ;
      k2 = k2 - kspan ;
      jj = j ;
      aki = aai ;
      bki = bbi ;
      aji = 0. ;
      bji = 0. ;
      k = 1 ;
 L670:k = k + 1 ;
      aki = at[k-1]*ck[jj-1] + aki ;
      bki = bt[k-1]*ck[jj-1] + bki ;
      k = k + 1 ;
      aji = at[k-1]*sk[jj-1] + aji ;
      bji = bt[k-1]*sk[jj-1] + bji ;
      jj = jj + j ;
      if(jj > jf) jj = jj - jf ;
      if(k < jf) goto L670 ;
      k = jf - j ;
      a[k1-1] = aki - bji ;
      b[k1-1] = bki + aji ;
      a[k2-1] = aki + bji ;
      b[k2-1] = bki - aji ;
      j = j + 1 ;
      if(j < k) goto L660 ;
      kk = kk + kspnn ;
      if(kk <= nn) goto L640 ;
      kk = kk - nn ;
      if(kk <= kspan) goto L640 ;
      /* multiply by rotation factor (except for factors of 2 and 4) */
 L700:if(i == m) goto L800 ;
      kk = jc + 1 ;
 L710:c2 = 1. - cd ;
      s1 = sd ;
 L720:c1 = c2 ;
      s2 = s1 ;
      kk = kk + kspan ;
 L730:ak = a[kk-1] ;
      a[kk-1] = c2*ak - s2*b[kk-1] ;
      b[kk-1] = s2*ak + c2*b[kk-1] ;
      kk = kk + kspnn ;
      if(kk <= nt) goto L730 ;
      ak = s1*s2 ;
      s2 = s1*c2 + c1*s2 ;
      c2 = c1*c2 - ak ;
      kk = kk - nt + kspan ;
      if(kk <= kspnn) goto L730 ;
      c2 = c1 - (cd*c1 + sd*s1) ;
      s1 = s1 + (sd*c1 - cd*s1) ;
      kk = kk - kspnn + jc ;
      if(kk <= kspan) goto L720 ;
      kk = kk - kspan + jc + inc ;
      if(kk <= jc+jc) goto L710 ;
      goto L100 ;
      /* permute the results to normal order --- done in two stages 
         permutation for square factors of n */
 L800:np[1-1] = ks ;
      if(kt == 0) goto L890 ;
      k = kt + kt + 1 ;
      if(m < k) k = k - 1 ;
      j = 1 ;
      np[k+1-1] = jc ;
 L810:np[j+1-1] = np[j-1]/nfac[j-1] ;
      np[k-1] = np[k+1-1]*nfac[j-1] ;
      j = j + 1 ;
      k = k - 1 ;
      if(j < k) goto L810 ;
      k3 = np[k+1-1] ;
      kspan = np[2-1] ;
      kk = jc + 1 ;
      k2 = kspan + 1 ;
      j = 1 ;
      if(n != ntot) goto L850 ;
      /* permutation for single-variate transform (optional code) */
 L820:ak = a[kk-1] ;
      a[kk-1] = a[k2-1] ;
      a[k2-1] = ak ;
      bk = b[kk-1] ;
      b[kk-1] = b[k2-1] ;
      b[k2-1] = bk ;
      kk = kk + inc ;
      k2 = kspan + k2 ;
      if(k2 < ks) goto L820 ;
 L830:k2 = k2 - np[j-1] ;
      j = j + 1 ;
      k2 = np[j+1-1] + k2 ;
      if(k2 > np[j-1]) goto L830 ;
      j = 1 ;
 L840:if(kk < k2) goto L820 ;
      kk = kk + inc ;
      k2 = kspan + k2 ;
      if(k2 < ks) goto L840 ;
      if(kk < ks) goto L830 ;
      jc = k3 ;
      goto L890 ;
      /* permutation for multivariate transform */
 L850:k = kk + jc ;
 L860:ak = a[kk-1] ;
      a[kk-1] = a[k2-1] ;
      a[k2-1] = ak ;
      bk = b[kk-1] ;
      b[kk-1] = b[k2-1] ;
      b[k2-1] = bk ;
      kk = kk + inc ;
      k2 = k2 + inc ;
      if(kk < k) goto L860 ;
      kk = kk + ks - jc ;
      k2 = k2 + ks - jc ;
      if(kk < nt) goto L850 ;
      k2 = k2 - nt + kspan ;
      kk = kk - nt + jc ;
      if(k2 < ks) goto L850 ;
 L870:k2 = k2 - np[j-1] ;
      j = j + 1 ;
      k2 = np[j+1-1] + k2 ;
      if(k2 > np[j-1]) goto L870 ;
      j = 1 ;
 L880:if(kk < k2) goto L850 ;
      kk = kk + jc ;
      k2 = kspan + k2 ;
      if(k2 < ks) goto L880 ;
      if(kk < ks) goto L870 ;
      jc = k3 ;
 L890:if(2*kt+1 >= m) return ;
      kspnn = np[kt+1-1] ;
      /* permutation for square-free factors of n */
      j = m - kt ;
      nfac[j+1-1] = 1 ;
 L900:nfac[j-1] = nfac[j-1]*nfac[j+1-1] ;
      j = j - 1 ;
      if(j != kt) goto L900 ;
      kt = kt + 1 ;
      nn = nfac[kt-1] - 1 ;
      if(nn > maxp) goto L998 ;
      jj = 0 ;
      j = 0 ;
      goto L906 ;
 L902:jj = jj - k2 ;
      k2 = kk ;
      k = k + 1 ;
      kk = nfac[k-1] ;
 L904:jj = kk + jj ;
      if(jj >= k2) goto L902 ;
      np[j-1] = jj ;
 L906:k2 = nfac[kt-1] ;
      k = kt + 1 ;
      kk = nfac[k-1] ;
      j = j + 1 ;
      if(j <= nn) goto L904 ;
      /* determine the permutations cycles of length greater than 1 */
      j = 0 ;
      goto L914 ;
 L910:k = kk ;
      kk = np[k-1] ;
      np[k-1] = -kk ;
      if(kk != j) goto L910 ;
      k3 = kk ;
 L914:j = j + 1 ;
      kk = np[j-1] ;
      if(kk < 0) goto L914 ;
      if(kk != j) goto L910 ;
      np[j-1] = -j ;
      if(j != nn) goto L914 ;
      maxf = inc*maxf ;
      /* reorder a and b, following the permutation cycles */
      goto L950 ;
 L924:j = j - 1 ;
      if(np[j-1] < 0) goto L924 ;
      jj = jc ;
 L926:kspan = jj ;
      if(jj > maxf) kspan = maxf ;
      jj = jj - kspan ;
      k = np[j-1] ;
      kk = jc*k + i + jj ;
      k1 = kk + kspan ;
      k2 = 0 ;
 L928:k2 = k2 + 1 ;
      at[k2-1] = a[k1-1] ;
      bt[k2-1] = b[k1-1] ;
      k1 = k1 - inc ;
      if(k1 != kk) goto L928 ;
 L932:k1 = kk + kspan ;
      k2 = k1 - jc*(k+np[k-1]) ;
      k = -np[k-1] ;
 L936:a[k1-1] = a[k2-1] ;
      b[k1-1] = b[k2-1] ;
      k1 = k1 - inc ;
      k2 = k2 - inc ;
      if(k1 != kk) goto L936 ;
      kk = k2 ;
      if(k != j) goto L932 ;
      k1 = kk + kspan ;
      k2 = 0 ;
 L940:k2 = k2 + 1 ;
      a[k1-1] = at[k2-1] ;
      b[k1-1] = bt[k2-1] ;
      k1 = k1 - inc ;
      if(k1 != kk) goto L940 ;
      if(jj != 0) goto L926 ;
      if(j != 1) goto L924 ;
 L950:j = k3 + 1 ;
      nt = nt - kspnn ;
      i = nt - inc + 1 ;
      if(nt >= 0) goto L924 ;
      return ;
      /* error finish, insufficient array storage */
 L998:*ierr = 1 ;
      return ;
}

/* Revision History
 *
 * $Log: fft.c,v $
 * Revision 1.1  2014/04/15 21:12:48  dechavez
 * initial release using sources modified from DCC version
 *
 */
