#pragma ident "$Id: figure_time.c,v 1.1 2011/03/17 20:23:19 dechavez Exp $"
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <float.h>
#include "globals.h"
#include <unistd.h>

#define   max(a,b)	((a) > (b) ? (a) : (b))
#define   min(a,b)	((a) < (b) ? (a) : (b))
#define   REG_LINE   7000      /* THICKNESS OF REGULAR PLOT LINES */
#define   HIST_LINE  19000     /* THICKNESS OF HISTOGRAM LINES */
#define   RES_SYM	 17        /* SYMBOL FOR RESIDUAL PLOTS -- circle */
#define   RES_SIZE  0.04       /* SIZE OF SYMBOL FOR RESIDUAL PLOTS */
#define   QUAL_SYM	 17        /* SYMBOL FOR QF PLOT -- circle */
#define   QUAL_SIZE  0.05      /* SIZE OF SYMBOL FOR QF PLOT */
#define   REBOOT_SYM	5      /* SYMBOL FOR REBOOTS -- asterisk */
#define   REBOOT_SIZE   0.08   /* SIZE OF SYMBOL FOR REBOOTS */
#define   OFFSET_SYM	18     /* SYMBOL FOR OFFSETS -- large circle */
#define   OFFSET_SIZE   0.05   /* SIZE OF SYMBOL FOR OFFSETS */
#define   PLOT_RATIO	500.0  /* 1 / (FRACTION OF PLOT SIZE BELOW WHICH 
                                       CONSECUTIVE POINTS ARE NOT PLOTTED) */

/* NEW SUBROUTINES ADDED   30/04/97 */
double slr(long, double*, double*, double*, double*, int, int);
void histo(int, double*, FILE*);


FILE *temp_file;

void
figure_time(t, nrecs, samprate, chan, das_stream, s0)
	IDATIME *t;
	int	 nrecs;
	double	samprate;
	char	*chan;
	int	das_stream;
	int	s0;
{
	int	nscale ;
	char	s[80], command[80];
    static int first_already_plotted = 0;



  /* plots on page 1:
	-------------------------
	|			|
	|			|
	|	 1		 3	|
	|			|
	|			|
	|			|
	|			|
	|	 2		 4	|
	|			|
	|			|
	|			|
	|	 X		  X	  |
	|			|
	|			|
	|			|
	-------------------------



  /* parts 1-4 appear only on first page; succeeding plots made by
   * routine plot_timefits may spill over onto later pages 
   */


/* printf("plotting...\n"); */
	sprintf(s,"/tmp/tv.%d.plotxy",getpid());
    if (first_already_plotted)
    {
		/* OPEN THE EXISTING PLOTXY FILE FOR APPENDING */
	    temp_file = fopen(s, "r+");
		/* CHANGE THE FINAL STOP COMMAND TO A PAGE BREAK */
        fseek(temp_file,-4L,SEEK_END);
	    fprintf(temp_file,"\npage\n");
    }
    else
    {
		/* OPEN A NEW PLOTXY FILE */
	    temp_file = fopen(s, "w");
	    fprintf(temp_file,"output %s.timefit.ps\n",prefix);
    }
	fprintf(temp_file,"note\n");
	fprintf(temp_file,"color %d\n",REG_LINE);
	fprintf(temp_file,"xlimit 2.75 0 0\n");
	fprintf(temp_file,"symbol -1\n");
	fprintf(temp_file,"ylimit 2.25 0 0\n");
	fprintf(temp_file,"character 0.08\n");
	fprintf(temp_file,"file *\n");

	plot_part1(t, nrecs, samprate, das_stream, chan);
	plot_part2(t, nrecs, samprate);
	plot_part3(t, nrecs, samprate);
	plot_part4(t, nrecs);
	plot_timefits(t, nrecs, s0, das_stream, chan);
	plot_summary(t, nrecs, s0, das_stream, chan);
	fprintf(temp_file,"stop\n");
	fclose(temp_file);

    first_already_plotted = 1;
/* printf("hmmm...\n"); */
	  
}



/* look at consistency of internal timing within records */
int
plot_part1(t, nrecs, samprate, das_stream, chan)
	IDATIME *t;
	int	 nrecs;
	double	samprate;
	int	das_stream;
	char	*chan;
{
	int	i, nn;
	int	ymin, ymax, dy;
	long	ll;
	double	*xm, *ym;
	double	*x, *y;
	char	s[80];

	dy = (int)( clock_factor / samprate );
	ymin = (t[0].nsamp - 1) * dy - dy/2 ;
	ymax = (t[0].nsamp - 1) * dy + dy/2 ;

	x = (double *)malloc( nrecs * sizeof(double) );
	y = (double *)malloc( nrecs * sizeof(double) );

	if ( x == NULL  ||  y == NULL )
	{
		fprintf(stderr, "Could not malloc for plotting.  Exit.\n");
		finish_up(1);
	}

	fprintf(temp_file,"color %d\n",REG_LINE);
	fprintf(temp_file,"ylimit 2.25 %d %d\n",ymin,ymax);
	fprintf(temp_file,"character 0.1\nxlabel record number\n");
	fprintf(temp_file,"ylabel internal [w/in rec] (ticks)\n");
	plot_labels(das_stream, chan, 0);
	/* SET THE ARRAYS */
	for (i=0; i<nrecs; i++)
	{
		x[i] = (double)i;
		y[i] = (long)( t[i].sys_end - t[i].sys_start);
		if ( y[i] > ymax ) y[i] = (double)ymax;
		if ( y[i] < ymin ) y[i] = (double)ymin;
	}
	/* COUNT HOW MANY WE REALLY HAVE TO PRINT */
	nn=0;
	for (i=0; i<nrecs; i++)
		if ( i==0 || i==nrecs-1 || y[i]!=y[i-1] || y[i]!=y[i+1] ) nn++;
	/* PRINT ONLY THEM */
	fprintf(temp_file,"read %d\n",nn);
	for (i=0; i<nrecs; i++)
		if ( i==0 || i==nrecs-1 || y[i]!=y[i-1] || y[i]!=y[i+1] )
			fprintf(temp_file,"%d %f\n",i, y[i]);

	fprintf(temp_file,"color %d\n",REG_LINE);
	fprintf(temp_file,"character 0.08\nplot %f %.14e\n",1.25, 7.5);
	fprintf(temp_file,"note\n");

	free( &x[0] );
	free( &y[0] );
}


/* look at continuity of internal timing from record to record */
int
plot_part3(t, nrecs, samprate)
	IDATIME *t;
	int	 nrecs;
	double	samprate;
{
	int	i, nr, nn;
	int	ymin, ymax, dy;
	long	ll;
	double	*xm, *ym;
	double	*x, *y;
	char	s[80];

	dy = (int)( clock_factor / samprate );
	ymin = (t[0].nsamp - 1) * dy ;
	ymax = (t[0].nsamp + 1) * dy ;

	nr = nrecs - 1;

	x = (double *)malloc( nr * sizeof(double) );
	y = (double *)malloc( nr * sizeof(double) );

	if ( x == NULL  ||  y == NULL )
	{
		fprintf(stderr, "Could not malloc for plotting.  Exit.\n");
		finish_up(1);
	}

	fprintf(temp_file,"cancel\n");
	fprintf(temp_file,"ylimit 2.25 %d %d\n",ymin,ymax);
	fprintf(temp_file,"character 0.1\nxlabel record number\n");
	fprintf(temp_file,"ylabel internal [rec-rec] (ticks)\n");
	for (i=0; i<nr; i++)
	{
		x[i] = (double)i;
		y[i] = (long)( t[i+1].sys_start - t[i].sys_start );
		if ( y[i] > ymax ) y[i] = (double)ymax;
		if ( y[i] < ymin ) y[i] = (double)ymin;
	}
	/* COUNT HOW MANY WE REALLY HAVE TO PRINT */
	nn = 0;
	for (i=0; i<nr; i++)
		if ( i==0 || i==nr-1 || y[i]!=y[i-1] || y[i]!=y[i+1] ) nn++;
	/* PRINT ONLY THEM */
	fprintf(temp_file,"read %d\n",nn);
	for (i=0; i<nr; i++)
		if ( i==0 || i==nr-1 || y[i]!=y[i-1] || y[i]!=y[i+1] )
			fprintf(temp_file,"%d %f\n",i, y[i]);

	fprintf(temp_file,"character 0.08\nplot -3.8 -3\n");

	free( &x[0] );
	free( &y[0] );
}


/* look at consistency of external timing within records */
int
plot_part2(t, nrecs, samprate)
	IDATIME *t;
	int	 nrecs;
	double	samprate;
{
	int	i, nr, nn;
	int	ymin, ymax, dy;
	long	ll;
	double	*xm, *ym;
	double	*x, *y;
	char	s[80];

	dy = (int)( t[0].nsamp / samprate );
	ymin = (int)( dy - 2.0 );
	ymax = (int)( dy + 2.0 );
	nr = nrecs - 1;

	x = (double *)malloc( nr * sizeof(double) );
	y = (double *)malloc( nr * sizeof(double) );

	if ( x == NULL  ||  y == NULL )
	{
		fprintf(stderr, "Could not malloc for plotting.  Exit.\n");
		finish_up(1);
	}

	fprintf(temp_file,"cancel\n");
	fprintf(temp_file,"ylimit 2.25 %d %d\n",ymin,ymax);
	fprintf(temp_file,"character 0.1\nxlabel record number\n");
	fprintf(temp_file,"ylabel external [rec-rec] (secs)\n");
	for (i=0; i<nr; i++)
	{
		x[i] = (double)i;
		y[i] = (long)( t[i+1].pulse_ext_start - t[i].pulse_ext_start);
		if ( y[i] > ymax ) y[i] = (double)ymax;
		if ( y[i] < ymin ) y[i] = (double)ymin;
	}
	/* COUNT HOW MANY WE REALLY HAVE TO PRINT */
	nn=0;
	for (i=0; i<nr; i++)
		if ( i==0 || i==nr-1 || y[i]!=y[i-1] || y[i]!=y[i+1] ) nn++;
	/* PRINT ONLY THEM */
	fprintf(temp_file,"read %d\n",nn);
	for (i=0; i<nr; i++)
		if ( i==0 || i==nr-1 || y[i]!=y[i-1] || y[i]!=y[i+1] )
			fprintf(temp_file,"%d %f\n",i, y[i]);

	fprintf(temp_file,"character 0.08\nplot 3.8 0\n");

	free( &x[0] );
	free( &y[0] );
}


/* look at continuity of offset from record to record */
int
plot_part4(t, nrecs)
	IDATIME *t;
	int	 nrecs;
{
	int	i, nr, nn ;
	static int	ymin = -5;
	static int	ymax =  5;
	double	*xm, *ym;
	double	*x, *y;
	double	y1, y2;
	char	s[80];

	nr = nrecs - 1;

	x = (double *)malloc( nr * sizeof(double) );
	y = (double *)malloc( nr * sizeof(double) );

	if ( x == NULL  ||  y == NULL )
	{
		fprintf(stderr, "Could not malloc for plotting.  Exit.\n");
		finish_up(1);
	}

	fprintf(temp_file,"cancel\n");
	fprintf(temp_file,"ylimit 2.25 %d %d\n",ymin,ymax);
	fprintf(temp_file,"character 0.1\nxlabel record number\n");
	fprintf(temp_file,"ylabel \\Delta\\ offset (secs)\n");

	for (i=0; i<nr; i++)
	{
		x[i]  = (double)i;
		y1  = (double)( t[i].pulse_ext_start ) ;
		y1 -= (double)( t[i].pulse_sys_start ) / clock_factor;
		y2  = (double)( t[i+1].pulse_ext_start ) ;
		y2 -= (double)( t[i+1].pulse_sys_start ) / clock_factor;

		y[i]  = y2 - y1;

		/* following is necessary because of scaling bug in MATLAB */

		if ( y[i] > (double)ymax ) y[i] = (double)ymax;
		if ( y[i] < (double)ymin ) y[i] = (double)ymin;
	}
	/* COUNT HOW MANY WE REALLY HAVE TO PRINT */
	nn=0;
	for (i=0; i<nr; i++)
		if ( i==0 || i==nr-1 || y[i]!=y[i-1] || y[i]!=y[i+1] ) nn++;
	/* PRINT ONLY THEM */
	fprintf(temp_file,"read %d\n",nn);
	for (i=0; i<nr; i++)
		if ( i==0 || i==nr-1 || y[i]!=y[i-1] || y[i]!=y[i+1] )
			fprintf(temp_file,"%d %f\n",i, y[i]);

	fprintf(temp_file,"character 0.08\nplot 3.8 0\n");

	free( &x[0] );
	free( &y[0] );
}

#include <time.h>

int
plot_labels(das_stream, chan, summary_flag)
	int	das_stream;
	char	*chan;
	int	summary_flag;
{
	char		s[80];
	char		ts[64];
	char		myprefix[64];
	char		mytitle[64];
	char		urlabel[64];
	char		*cuserid();
	char		namebuf[16];
	char		logname[16];
	struct timeval	tp;
	static int	pageno = 1;

	/* CONSTRUCT TOP LABEL FROM STATION VOLUME */
	strcpy(myprefix, prefix);
	if ( strrchr(prefix, '/') != NULL )
		strcpy(myprefix, strrchr(prefix, '/')+1);
	strcpy(mytitle, util_ucase(myprefix));

	/* CONSTRUCT LABEL FOR UPPER RIGHT HAND SIDE */
	strcpy(s, chan);
	sprintf(urlabel, "ARS (%s)", util_lcase(s) );
	if ( das_stream ) sprintf(urlabel, "DAS (%s)", util_lcase(s) );

	/* LABEL LOWER RIGHT WITH DATE, PAGE, AND USERID */
	gettimeofday(&tp, NULL);
	strcpy(ts, timestr((double)tp.tv_sec - 28800.) ); ts[14] = '\0';

	if ( cuserid(namebuf) != NULL )
	{
		strcpy(logname, cuserid(namebuf) );
	}
	else
	{	
		strcpy(logname, "DCC");
	}

	if ( !summary_flag )
	{
		fprintf(temp_file,"character 0.2\n");
		fprintf(temp_file,"note (2.25, 2.6 in) \\dup\\ %s\n",mytitle);
		fprintf(temp_file,"character 0.12\n");
		fprintf(temp_file,"note (5.0 2.65 in) %s\n",urlabel);
		fprintf(temp_file,"character 0.06\n");
		fprintf(temp_file,"note (3.75 -7 in) %s / %s / pg %d\n",
				ts, logname, pageno );
	}
	else
	{
		fprintf(temp_file,"character 0.2 90\n");
		fprintf(temp_file,"note (-0.85, -1.25 in) \\dup\\ %s\n",mytitle);
		fprintf(temp_file,"character 0.12 90\n");
		fprintf(temp_file,"note (-0.90 2.0 in) %s\n",urlabel);
		fprintf(temp_file,"character 0.06 90\n");
		fprintf(temp_file,"note (8.75 0.25 in) %s / %s / pg %d\n",
				ts, logname, pageno );
	}
	fprintf(temp_file,"character 0.10\n");

	pageno++;
}

/*
 * fit a time function to each segment requiring it
 */

/* NOTE: PLOTS_PER_PAGE CANNOT BE CHANGED OR BAD THINGS WILL HAPPEN */
#define PLOTS_PER_PAGE	3

int
plot_timefits(t, nrecs, s0, das_stream, chan)
	IDATIME *t;
	int	 nrecs;
	int	s0;
	int	das_stream;
	char	*chan;
{
	int		i, k, break_page, nplots;
	int		iret;
	int		check_forcefits();
	char		s[80];
	double	drift = 0.0, intercept = 0.0;


  /* 
   * The global variable structure tsegs holds information about the time
   * mapping.  This structure will be filled in this routine and accessed
   * elsewhere.
   */

  /*
   * determine how many time fits will be necessary and
   * fill part of the tsegs structure
   */

	for (i=s0, n_tsegs = -1; i<nsegs; i++)
	{
		if ( segs[i].reboot!=0 || segs[i].new_config == TRUE || 
			 check_forcefits(segs[i].n_start, segs[i].m_start) )
		{
			n_tsegs++;
			tsegs[n_tsegs].n_start = segs[i].n_start;
			tsegs[n_tsegs].m_start = segs[i].m_start;
			tsegs[n_tsegs].year	= segs[i].year;
			tsegs[n_tsegs].start = t[ segs[i].n_start ].sys_start;
		    tsegs[n_tsegs].reboot = segs[i].reboot;
 printf("reboot? %d %ld %ld %lf\n",tsegs[n_tsegs].reboot,tsegs[n_tsegs].n_end,tsegs[n_tsegs].m_end,tsegs[n_tsegs].end); 
		}
		if ( n_tsegs == MAXSEGS - 1)
		{
			fprintf(stderr, "Number of segments requiring separate");
			fprintf(stderr, " time fit are excessive.  Exiting.\n");
			finish_up(1);
		}
		tsegs[n_tsegs].n_end = segs[i].n_end;
		tsegs[n_tsegs].m_end = segs[i].m_end;
		tsegs[n_tsegs].end = t[ segs[i].n_end ].sys_end;
 printf("otherwise %ld %ld %lf\n",tsegs[n_tsegs].n_end,tsegs[n_tsegs].m_end,tsegs[n_tsegs].end); 
	}
	tsegs[n_tsegs].m_end = segs[nsegs-1].m_end;
	tsegs[n_tsegs].year  = segs[nsegs-1].year;
	n_tsegs = ++n_tsegs;
 printf("n_tsegs is %d %ld %ld %lf\n",n_tsegs, tsegs[n_tsegs].n_end,tsegs[n_tsegs].m_end,tsegs[n_tsegs].end); 

  /* fit time for each of these */

	printf("\n--------------------- fitting for ");
	if ( das_stream )
	{
		printf("DAS drift, intercept -------------\n");
	}
	else
	{
		printf("ARS drift, intercept -------------\n");
	}
	  nplots = 3;
	for (i=0, k=3; i < n_tsegs; i++, k++)
	{
		printf("\nSegment %d:  %d records\n\n",
			   i+1,tsegs[i].n_end-tsegs[i].n_start+1);

	   /* if drift, intercept are to be forced, get info from file */
		if ( time_coef &&
			 read_coefficient_file(i+1, das_stream, &drift, &intercept) )
		{
			tsegs[i].drift = drift;
			tsegs[i].intercept = intercept;
			printf("  drift, intercept forced to: %e %.3lf\n",
				drift, intercept);
			continue;
		}

		/* CHECK FOR LAST ROW OF PLOTS FOR PAGE, OR LAST SEGMENT */

		if (k-(k/3)*3==1) 
			break_page = 1;
		else
			break_page = 0;
		iret = find_time(t, nrecs, 0, &tsegs[i], k, PLOTS_PER_PAGE,
						&drift, &intercept,break_page, das_stream, chan);

		/* if negative return, no clock information */
		if ( iret < 0 )
		{
			printf("   *************************************************\n");
			printf("   *  Clock unlocked during all of this segment.   *\n");
			printf("   *************************************************\n");
			drift = intercept = 0.0;
		}

	   /* if positive return, then unable to fit for drift */
		if ( iret > 0 )
		{
			drift = tsegs[i-1].drift ;
		}

		tsegs[i].drift = drift;
		tsegs[i].intercept = intercept;
	}
	printf("\n--------------------- end fitting -----------------------------------\n");

  /*
   * transfer contents of tsegs to appropriate structure
   */
	if ( das_stream )
	{
		n_dassegs = n_tsegs;
		memcpy(&das_segs[0], &tsegs[0], n_tsegs * sizeof(SEGMENTS));
	}
	else
	{
		n_arssegs = n_tsegs;
		memcpy(&ars_segs[0], &tsegs[0], n_tsegs * sizeof(SEGMENTS));
	}
}

int
check_forcefits(n1, n2)
	long	n1, n2;
{
	int	i;

	if ( n_forcerecs < 0 )  return(FALSE);
	if ( n_forcerecs == 1 && forcerecs[0] == 0 ) return(TRUE);

	for ( i=0; i<n_forcerecs; i++)
	{
		if ( forcerecs[i] >= n1 && forcerecs[i] <= n2 ) return(TRUE);
	}
	return(FALSE);
}

int
read_coefficient_file(segment_number, das_stream, drift, intercept)
	int	segment_number;
	int	das_stream;
	double	*drift, *intercept;
{
	FILE	*fp;
	char	line[128];
	char	part[8];
	int	n, das;
	double	dr, it;

	fp = fopen(force_drift_file, "rt");
	if ( fp == NULL )
	{
		fprintf(stderr, "Could not open file %s. dcctvol exiting.\n",
			force_drift_file);
		finish_up(1);
	}

	while ( (fgets(line, 127, fp)) != NULL )
	{
		sscanf(line, "%s %d %le %lf", &part, &n, &dr, &it);
		das = ( !strcmp(part, "DAS") ) ? TRUE : FALSE ;
		if ( n == segment_number  &&  das == das_stream )
		{
			*drift = dr;
			*intercept = it;
			fclose(fp);
			return(TRUE);
		}
	}

	fclose(fp);
	return(FALSE);
}

int
find_time(t, nrecs, unlocked, tsegs, ny, nplots, drift, intercept, break_page,
		  das_stream, chan)
	IDATIME *t;
	int	 nrecs, break_page;
	int unlocked;			/* this provides the option of eliminating 
								  empty plots for unlocked sections */
	SEGMENTS	*tsegs;
	int	ny, nplots;
	double	*drift, *intercept;
	int	das_stream;
	char	*chan;
{
	int	i, k, iret, np1, np2 ;
	int	m, n;
	int	low_flag = 0;
	long	n_outliers = 0, *ix;
	long jj, kk, nn, irej, nrej, nnrej;
	double a0, a1, rej_level;
	static int	fit_count = 1;
	static int	yscale = 10;
	double	*xm, *ym;
	double	*x, *y, *res, *ypred, *xx, *yy;
	double	  xmin, xmax, ymin, ymax, resmin, resmax, d0;
	double	*out_real,*out_imag;
	double	res_median, res_std, m_drift, m_intercept;
	char	s[128];
	int		npass = 3;
	static float	sigmas[3] = {10., 5., 3.};

	if (unlocked)
	{
		fit_count++;
		return(0);
	}

	np1 = (int)( tsegs->n_end - tsegs->n_start ) + 1;
	if ( np1 < 1 ) return(-1);

	x = (double *)malloc( np1 * sizeof(double) );
	y = (double *)malloc( np1 * sizeof(double) );
	xx = (double *)malloc( np1 * sizeof(double) );
	yy = (double *)malloc( np1 * sizeof(double) );
	ix = (long*) malloc(np1 * sizeof(long));
	ypred = (long*) malloc(np1 * sizeof(double));
	res = (long*) malloc(np1 * sizeof(double));

	if ( x == NULL  ||  y == NULL )
	{
		fprintf(stderr, "Could not malloc for plotting.  Exit.\n");
		finish_up(1);
	}

	for (i=tsegs->n_start, k=0; i<tsegs->n_end; i++)
	{
		if ( t[i].qf_start == 1 )
		{
			x[k] = (double)( t[i].pulse_sys_start ) / clock_factor;
		/* 	x[k] = (double)( t[i].sys_start ) / clock_factor; */
			y[k] = (double)( t[i].pulse_ext_start ) ;
			k++;
		}
	}
	np2 = k;

	if ( np2 < 1 ) /* bail out */
	{
		fill_plot(break_page, ny, fit_count);
		fit_count++;
		return(-1);
	}

	if ( np2 < 10 ) /* don't fit drift */
	{
		low_flag = 1;
		for(i=0; i<np2; i++)
		{
			y[i] = y[i] - (1. - *drift) * x[i];
		}
	}
	  else
		  low_flag = 0;


	/* DO AN ITERATIVE REGRESSION AND OUTLIER REMOVAL */
	nn = np2;
	irej = 15;
	nnrej = 0;
	npass = 0;
	nrej = 0;

	while (nrej>0 || irej>3)
	{
		npass++;
		if ((clock_factor > 999.0 ) && (clock_factor <= 99999))
		{
			res_std = 1000.0 * sqrt(slr(nn, x, y, &a0, &a1, 0, npass)/(nn-1.0));
			if ( res_std < 0.1 ) break;
		}

		/* DO LINEAR REGRESSION ON SERIES AND GET STANDARD ERROR */
		res_std = 1000.0 * sqrt(slr(nn, x, y, &a0, &a1, !low_flag, npass)/(nn-1.0));
		if ( res_std < 0.1 ) break;

		/* ELIMINATE OUTLIERS */
		irej = max(3,irej/1.5);
		rej_level = res_std*irej;

	  /* for the MK8, the rej_level should be no more than one second */
		if (clock_factor > 999.0 ) rej_level = ( rej_level > 900.0 ) ? 900.0 : rej_level ;

		nrej = 0;
		for (jj=0;  jj<nn;  jj++)
		{
			ypred[jj] = a0 + a1*x[jj];
			res[jj] = (y[jj] - ypred[jj])*1000;
			if (fabs(res[jj]) > rej_level)
			/* INCREMENT THE REJECT COUNTER */
				nrej++;
			else
			/* MOVE UP THE CURRENT ELEMENT */
			{
				x[jj-nrej] = x[jj];
				y[jj-nrej] = y[jj];
			}
		}
		nnrej += nrej;
		nn -= nrej;
		printf("	pass %2d: %4d outliers rejected at %3.4f msec (%2d std)\n",
			   npass, nnrej, rej_level, irej);
	}

	/* express final residual in millisecs */
	/* COMPUTE THE FINAL RESIDUALS */
	resmax = -DBL_MAX;
	resmin = DBL_MAX;
	for (jj=0;  jj<nn;  jj++)
	{
		ypred[jj] = a0 + a1*x[jj];
	/* xxx	res[jj] = (y[jj] - ypred[jj])*1000; xxx */
		res[jj] = (y[jj] - ypred[jj]) * clock_factor; /* scale residuals to system ticks */
		resmin = min(resmin,res[jj]);
		resmax = max(resmax,res[jj]);
	}

	/* SET THE FINAL FIT PARAMETERS */
	m_intercept = a0;
	if (low_flag)
		m_drift = *drift;
	else
		m_drift = a1 - 1.0;

	/* DO A COMB SORT TO GET THE MEDIAN */
	for (jj=0;  jj<nn;  jj++)
		ix[jj] = jj;
	dind_combsort (nn, ix, res);
	res_median = res[ix[nn/2]];
	if (nn/2 != (int)((double)nn/2.0))
		res_median = (res_median + res[ix[nn/2+1]]) / 2.0;

	/* PRINT MESSAGES */
	printf("   original (final) number of points = %d (%d)\n", np2, nn);
	printf("   median of residuals = %.1lf ticks\n", res_median);
	printf("   std deviation of residuals = %.1lf ticks\n", res_std);
	printf("   drift	 = %e\n", m_drift);
	strcpy(s, dbltim_to_ascii(m_intercept) );
	printf("   intercept = %s\n", s);


	/* CLIP AT +- 2 TICKS */
	for (jj=0;  jj<nn;  jj++)
	{
		if (res[jj] < -2.0) res[jj] = -2.0;
		if (res[jj] >  2.0) res[jj] =  2.0;
	}

	/* SET THE MAX AND MIN FOR THE X-AXIS */
	xmin = (double)(1.05*ypred[0] - 0.05*ypred[nn-1])/86400.0;
	xmax = (double)(1.05*ypred[nn-1] - 0.05*ypred[0])/86400.0;
	if (xmin==xmax)
	{
		xmin = (double)(int)xmin;
		xmax = xmin + 1;
	}

	/* DETERMINE WHICH POINTS NEED TO BE PLOTTED */
	d0 = sqrt(pow(xmax-xmin,2.0) + pow(20,2.0));
	for (jj=0, kk=0; jj<nn; jj++)
	{
		xx[kk] = ypred[jj]/86400.0;
	/* 	xx[kk] = x[jj];  /* UNCOMMENT TO LOOK AT ORIGINAL SERIES */
		yy[kk] = res[jj];
	/* 	yy[kk] = y[jj];  /* UNCOMMENT TO LOOK AT ORIGINAL SERIES */
		/* DON'T PLOT IT IF IT'S EXCESSIVELY CLOSE TO THE LAST POINT */
		if (jj==0 || sqrt(pow(xx[kk]-xx[kk-1],2.0) + pow(yy[kk]-yy[kk-1],2.0))
			> d0/PLOT_RATIO) kk++;
	}

	/* PRINT THE PLOTXY COMMANDS FOR THIS SECTION */
	fprintf(temp_file,"symbol %d %.3f\n",RES_SYM,RES_SIZE);
	fprintf(temp_file,"character 0.1\nxlabel external time (days)\n");
	fprintf(temp_file,"ylabel residual time (ticks)\n");
	fprintf(temp_file,"character 0.10\nnote (0.25 2 in) sect %d, N=%d\n",
            fit_count,nn);
	fprintf(temp_file,"ylimit 2.25 -2 2\n");
/* 	fprintf(temp_file,"ylimit 2.25 0 0\n"); /* UNCOMMENT FOR ORIGINAL SERIES */
/* 	fprintf(temp_file,"xlimit 2.75 0 0\n"); /* UNCOMMENT FOR ORIGINAL SERIES */

	/* WRITE THE ACTUAL POINTS FOR PLOTTING */

	if ( check_leapyr_error( t[tsegs->n_start], m_drift, m_intercept, chan) )
	{
		fprintf(temp_file,"xlimit 2.75 %f %f\n",xmin+1, xmax+1);
		fprintf(temp_file,"read %d\n",kk);
		for (jj=0;  jj<kk;  jj++)
			fprintf(temp_file,"%f %f\n", xx[jj]+1, yy[jj]);
		m_intercept += 86400.; /* add a day */
	}
	else
	{
		fprintf(temp_file,"xlimit 2.75 %f %f\n",xmin, xmax);
		fprintf(temp_file,"read %d\n",kk);
		for (jj=0;  jj<kk;  jj++)
			fprintf(temp_file,"%f %f\n", xx[jj], yy[jj]);
	}

	/* INSERT PAGE BREAK IF REQUIRED */
	if (break_page)
	{
		fprintf(temp_file,"page\n");
	  plot_labels(das_stream, chan, 0);
		fprintf(temp_file,"character 0.08\nplot 1.25 7.5\n");
	}
	else
		fprintf(temp_file,"character 0.08\nplot -3.8 -3.0\n");


	/* HISTOGRAM THE RESIDUALS */
	fprintf(temp_file,"symbol -1\n");
	histo(nn, res, temp_file);

	fprintf(temp_file,"color %d\n",REG_LINE);
	fprintf(temp_file,"note\n");
	fprintf(temp_file,"xlimit 2.75 -5.5 5.5\n");
	fprintf(temp_file,"character 0.1\nxlabel residual time (ticks)\n");
	fprintf(temp_file,"ylabel number of records\n");
	fprintf(temp_file,"character 0.10\nnote (1.9 1.9 in) std=%.1f ms\n",res_std);
	fprintf(temp_file,"character 0.08\nplot 3.8 0\n");
	fprintf(temp_file,"note\n");

	*drift	 = m_drift;
	*intercept = m_intercept;

	free( x );
	free( y);
	free( xx );
	free( yy);
	free( ypred);
	free( res);
	free( ix );

	fit_count++;
	return(low_flag);
}

int
fill_plot(break_page, ny, fit_count)
	int break_page, ny, fit_count;
{
	/*  DO LEFT PLOT */
	fprintf(temp_file,"xlimit 2.75 0 1\n");
	fprintf(temp_file,"ylimit 2.25 0 1\n");
	fprintf(temp_file,"ylabel residual time (N/A)\n");
	fprintf(temp_file,"xlabel external time (N/A)\n");
	fprintf(temp_file,"read 2\n");
	fprintf(temp_file,"0 0\n1 1\n");
	fprintf(temp_file,"character 0.12\nnote (0.25 0.45) clock unlocked\n");
	fprintf(temp_file,"character 0.10\nnote (0.1 0.90) sect %d\n",fit_count);
	/* CHECK IF IT'S THE FIRST PLOT OF A NEW PAGE */
	if (break_page)
	{
		fprintf(temp_file,"page\n");
		fprintf(temp_file,"plot 1.25 7.5\n");
	}
	else
		fprintf(temp_file,"plot -3.8 -3.0\n");

	/*  DO RIGHT PLOT */
	fprintf(temp_file,"character 0.08\n");
	fprintf(temp_file,"xlabel residual time (N/A)\n");
	fprintf(temp_file,"ylabel number of records (N/A)\n");
	fprintf(temp_file,"plot 3.8 0\n");

	/* CLEAR NOTES AND RESET CHARACTER HEIGHT */
	fprintf(temp_file,"note\n");
	fprintf(temp_file,"character 0.08\n");

}

/*
 * following is to correct for the data logger leap day error of 1996
 */

int
check_leapyr_error(t, drift, intercept, chan)
	IDATIME		*t;
	double		drift;
	double		intercept;
	char		*chan;
{
	int		i;
	double		start_time;
	static char	*goof_list[] = {"ASCN", "CMLA", "EFI", "ERM",
					"JTS", "MSEY", "NIL", "SHEL",
					"SUR", "WRAB"};
	static char	*goof_list_2000[] = {"ASCN", "BORG", "EFI", "MSEY", "NIL",
				"SHEL", "SUR", "WRAB"};
	static char	*goof_list_2004[] = {"EFI", "NIL", "SHEL"};
	static char	*goof_list_2008[] = {"BFO", "COCO", "EFI", "ERM", "HOPE", "MSEY", "NIL", "SHEL"};

	int		n2000=8;
	int		n2004=3;
	int		n2008=8;

	if ( (t->year != 1996) && (t->year != 2000) && (t->year != 2004) && (t->year != 2008) ) return(FALSE) ;

	if (t->year == 1996)
	{
	    for (i=0; i<10; i++)
	    {
		if ( !strcmp( goof_list[i], stat_name ) )
		{
			start_time = (double)t->sys_start / clock_factor;
			start_time = start_time * (1.+ drift) + intercept;
			start_time /= 86400.;
			if ( start_time > 60. )
			{
			  /* MSEY was fixed 1996240 */

				if ( !strcmp( stat_name, "MSEY" ) && start_time > 238.)
				{
					return(FALSE);
				}
				else
				{
					return(TRUE);
				}
			}
			break;
		}
	    }
	}
	if (t->year == 2000)
	{
	    for (i=0; i<n2000 ; i++)
	    {
		if ( !strcmp( goof_list_2000[i], stat_name ) )
		{
			start_time = (double)t->sys_start / clock_factor;
			start_time = start_time * (1.+ drift) + intercept;
			start_time /= 86400.;
			if ( start_time > 60. )
			{
				/* MSEY was fixed again 2000225 */
				if ( !strcmp( stat_name, "MSEY" ) && (start_time > 224.))
				{
					return(FALSE);
				}
				/* ASCN was fixed 2000196 */
				else if ( !strcmp( stat_name, "ASCN" ) && (start_time > 195.))
				{
					return(FALSE);
				}
				/* WRAB was fixed 2000201 */
				else if ( !strcmp( stat_name, "WRAB" ) && (start_time > 200.))
				{
					return(FALSE);
				}
				else
				{
					return(TRUE);
				}
			}
			break;
		}
	    }
	}
	if (t->year == 2004)
	{
	    for (i=0; i<n2004 ; i++)
	    {
		if ( !strcmp( goof_list_2004[i], stat_name ) )
		{
			start_time = (double)t->sys_start / clock_factor;
			start_time = start_time * (1.+ drift) + intercept;
			start_time /= 86400.;
			if ( start_time > 60. )
			{
				/* SHEL was fixed 2004212 */
				if ( !strcmp( stat_name, "SHEL" ) && (start_time > 211.))
				{
					return(FALSE);
				}
				else
				{
					return(TRUE);
				}
			}
			break;
		}
	    }
	}
	if (t->year == 2008)
	{
	    for (i=0; i<n2008; i++)
	    {
		if ( !strcmp( goof_list_2008[i], stat_name ) && strcmp (chan, "wdx") )
		{
			start_time = (double)t->sys_start / clock_factor;
			start_time = start_time * (1.+ drift) + intercept;
			start_time /= 86400.;
			if ( start_time > 60. )
		  	{
				return(TRUE);
			}
			break;
		}
	    }
	}
	return(FALSE);
}

int
plot_summary(t, nrecs, s0, das_stream, chan)
	IDATIME *t;
	int	 nrecs;
	int	s0;
	int	das_stream;
	char	*chan;
{
	int	i, j, k, nr;
	int	xmin = 999999, xmax = -999999;
	int	ymin_top = -1, ymax_top = 8;
	int	ymin_bot = 0,  ymax_bot = 4;
	int	ypos_data = 1, ypos_reboot = 2, ypos_offset = 3;
	int	 this_year ;
	int	 year_cross = FALSE;

	double	*xm, *ym;
	double	*x, *y, *xx, *yy;
	double	x1, x2, d0;
	double	gap_tol, slope_tol;
	char	s[80];
	double	timex();
	extern double	calc_time();



	/* first plot */


	x = (double *)malloc( nrecs * sizeof(double) );
	y = (double *)malloc( nrecs * sizeof(double) );
	xx = (double *)malloc( nrecs * sizeof(double) );
	yy = (double *)malloc( nrecs * sizeof(double) );

	if ( x == NULL  ||  y == NULL )
	{
		fprintf(stderr, "Could not malloc for plotting.  Exit.\n");
		finish_up(1);
	}

	/* check first if year boundary has been crossed */
	year_cross = ( tsegs[0].year == tsegs[n_tsegs-1].year ) ? FALSE : TRUE ;

	for (i=0, k=0; i<n_tsegs; i++)
	{
		if (tsegs[i].drift==0.0 && tsegs[i].intercept==0) continue;
		for (j=tsegs[i].n_start; j<tsegs[i].n_end; j++, k++)
		{
			x[k] = timex(t[j].sys_start,tsegs[i].drift,tsegs[i].intercept);
			if (k > 0  &&  x[k] < 0.01 ) x[k] = x[k-1];

		   /* if this is a year crossing, adjust x values */
			if ( year_cross && tsegs[i].year == tsegs[0].year )
			{
				x[k] -= (double)dysize(tsegs[i].year);
			}

			y[k] = (double)t[j].qf_start ;
			if ( y[i] > (double)ymax_top ) y[i] = (double)ymax_top;
			if ( y[i] < (double)ymin_top ) y[i] = (double)ymin_top;
		}
		this_year = (int)tsegs[i].year;
	}
	nr = k;
	xmin = (int)x[0];
	if (x[0] < 0.0) xmin -= 1;
	xmax = (int)x[nr-1]+1;

	/* WRITE PLOTXY COMMANDS */
	fprintf(temp_file,"page\nseascape\n");
	fprintf(temp_file,"xlimit 8.25 %d %d\n",xmin, xmax);
	fprintf(temp_file,"ylimit 3 %d %d\n",ymin_top,ymax_top);
	fprintf(temp_file,"character 0.14\n");
	fprintf(temp_file,"xlabel time (days)  year = %d\n",this_year);
	fprintf(temp_file,"ylabel clock quality factor\n");
	plot_labels(das_stream, chan, 1);
	fprintf(temp_file,"symbol %d %.3f\n",QUAL_SYM,QUAL_SIZE);

	/* DETERMINE WHICH POINTS NEED TO BE PLOTTED */
	d0 = sqrt(pow(xmax-xmin,2.0) + pow(ymax_top-ymin_top,2.0));
	xx[0] = x[0];   yy[0] = y[0];
	for (i=1, k=0; i<nr; i++)
		/* DON'T PLOT IT IF IT'S EXCESSIVELY CLOSE TO THE LAST POINT */
		if (sqrt(pow(x[i]-xx[k-1],2.0) + pow(y[i]-yy[k-1],2.0)) > d0/PLOT_RATIO)
		{
			xx[k] = x[i];
			yy[k] = y[i];
			k++;
		}

    /* WRITE THEM */
	fprintf(temp_file,"read %d\n",k);
	for (i=0; i<k; i++)
		fprintf(temp_file,"%f %f\n",xx[i], yy[i]);
	fprintf(temp_file,"character 0.1\nplot %f %f\n",1.75, 4.75);


	/* second plot */

	fprintf(temp_file,"character 0.14\n");
	fprintf(temp_file,"ylabel Misc factors\n");
	fprintf(temp_file,"ylimit 3 0 4\n");
	for (i=0; i<n_tsegs; i++)
	{
	/* 	if ( abs( tsegs[i].intercept ) < 1e-4 ) continue; */

		/* coverage */
		x1 = timex(tsegs[i].start, tsegs[i].drift, tsegs[i].intercept);
		x2 = timex(tsegs[i].end,   tsegs[i].drift, tsegs[i].intercept);

		/* if this is a year crossing, adjust x values */
		if ( year_cross && tsegs[i].year == tsegs[0].year )
		{
			x1 -= (double)dysize(tsegs[i].year);
			x2 -= (double)dysize(tsegs[i].year);
		}


		fprintf(temp_file,"symbol -1\n");
		fprintf(temp_file,"read 2\n");
		fprintf(temp_file,"%f %d\n%f %d\n",x1,ypos_data,x2,ypos_data);

		/* reboots */
		j = nsegs - n_tsegs + i ;
		if ( tsegs[i].reboot==1 && i!=0)
		{
			fprintf(temp_file,"symbol %d %.3f\n",REBOOT_SYM,REBOOT_SIZE);
			fprintf(temp_file,"read 1\n");
			fprintf(temp_file,"%f %d\n",x1,ypos_reboot);
		}
        else if(tsegs[i].reboot<0 && i!=0)
        {
            fprintf(temp_file,"symbol %d %.3f\n", OFFSET_SYM, OFFSET_SIZE);
            fprintf(temp_file,"read 1\n");
            fprintf(temp_file,"%f %d\n",x1,ypos_offset);
        }
	}


/* CODE FOR INDEPENDENT RE-CHECK FOR OFFSET USED TO RESIDE HERE */
if (1==2){
	gap_tol = 0.1; /* tolerance for plotting this point */
	slope_tol = 0.01; /* tolerance for plotting this point */
	for (i=0; i<nrecs-1; i++)
	{
		x1  = (double)( t[i+1].pulse_ext_start - t[i].pulse_ext_start ) ;
		x2  = (double)( t[i+1].pulse_sys_start - t[i].pulse_sys_start ) ;
		x2 /= clock_factor;
	/* 	if ( fabs(x2 - x1) > gap_tol ) */
		if ( fabs(1 - x2/x1) > slope_tol )
		{
			for (j=0; j<n_tsegs; j++)
			{
				if ( i >= tsegs[j].n_start && i <= tsegs[j].n_end )
				{
					x1 = timex(t[i].sys_start, tsegs[j].drift, tsegs[j].intercept);
                    printf("%11d %12f %13f\n",
                           i+1,x1,x2 - x1);
                   /*  printf("%11d %12f %13f\n", */
                          /*  i+1,x1,fabs(1 - x2/x1)); */
					fprintf(temp_file,"symbol %d %.3f\n",
								  OFFSET_SYM, OFFSET_SIZE);
					fprintf(temp_file,"read 1\n");
					fprintf(temp_file,"%f %d\n",x1,ypos_offset);
				}
			}
		}
	}
	}

	x1 = xmin + 0.2;
	x1 = 0.99*xmin + 0.01*xmax;
	fprintf(temp_file,"note\n");
	fprintf(temp_file,"character 0.07 0\n");
	fprintf(temp_file,"note (%f %f) coverage\n", x1, ypos_data-0.15);
	fprintf(temp_file,"note (%f %f) reboots\n", x1, ypos_reboot-0.15);
	fprintf(temp_file,"note (%f %f) d(offset)\n", x1, ypos_offset-0.15);
	fprintf(temp_file,"character 0.1\nplot 0 -3.75\n");


	free( x );
	free( xx );
	free( y );
	free( yy );
}

double
timex(t, drift, intercept)
	double   t;
	double	drift;
	double	intercept;
{
	double	x;

	x = calc_time(t, drift, intercept, clock_factor) / 86400.;
	x -= 1.0;

	return(x);
}

/* Revision History
 *
 * $Log: figure_time.c,v $
 * Revision 1.1  2011/03/17 20:23:19  dechavez
 * initial release, based on DCC time_vol sources
 *
 */
