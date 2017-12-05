#pragma ident "$Id: day_split.c,v 1.1 2011/03/17 20:23:19 dechavez Exp $"
/*======================================================================
    day_split.c

    Routine to split wfdisc into individual days
=======================================================================*/
#include "globals.h"
#include "wfdisc30io.h"

#define DAY_FRACT 100

void
day_split( wf30, sample_rate, leap_second, fp )
	struct wfdisc	*wf30;
	double		sample_rate;
	double		leap_second;
	FILE		*fp;
{
	long		nominal;
	long      	bps, total, bytes_written, foff, nbytes ;
	double   	csstim, actual_sint;
	static long	css_bytes_left();

	struct wfdisc	new_wfd;
	extern void	finish_up();
	extern int	print_wd_double();

	bps = atoi( wf30->datatype + 1 );

	bytes_written = 0;
	csstim        = wf30->time;
	total         = wf30->nsamp * bps;
	foff          = wf30->foff;
	actual_sint   = 1. / sample_rate;

	while (bytes_written < total)
	{
		init_wfdisc(&new_wfd, wf30, csstim, foff, 1);
		nbytes = css_bytes_left(csstim, actual_sint, bps, DAY_FRACT);
		if ( (leap_second - csstim) < 86400.0 &&
		     (leap_second - csstim) > 0 )
		{
			csstim -= 1.0;
			nominal = (int)(sample_rate * 10 + 1.0) / 10;
			nbytes += (int)(nominal * bps);
		}
		if (nbytes >= (total - bytes_written)) nbytes = total - bytes_written;
		new_wfd.nsamp   = nbytes / bps;
		new_wfd.endtime = ((new_wfd.nsamp - 1) * actual_sint) + new_wfd.time;
		print_wd_double(new_wfd, sample_rate, fp);
		csstim += actual_sint * new_wfd.nsamp;
		foff   += nbytes;
		bytes_written += nbytes;
	}
}


/*======================================================================
    init_wfdisc                                           DAP 02/19/94
  This routine is used to fill initial values of new wfdisc entry
=======================================================================*/
static init_wfdisc(new, i_wfd, csstim, foff, commid)
struct wfdisc *new, *i_wfd;
double csstim;
int foff, commid;

/***********************************************************************
  Begin init_wfdisc
***********************************************************************/
{

/***********************************************************************
  Variable Declarations
***********************************************************************/
    double dbtime;
    int year, day;

/***********************************************************************
  Initialize the wfdisc
***********************************************************************/
    *new = wfdisc_null;

    new->time = csstim;
    new->samprate = i_wfd->samprate;
    new->foff = foff;
    year = css_to_dbltim(csstim, &dbtime);
    day  = dbtime / 86400.0;
    new->jdate   = 1000L * year + day;
    strncpy(new->chan, i_wfd->chan, strlen(i_wfd->chan));
    new->nsamp   = 0;
    strncpy(new->datatype, i_wfd->datatype, 2);
    strcpy(new->dir, i_wfd->dir);
    strcpy(new->dfile, i_wfd->dfile);
    strcpy(new->sta, i_wfd->sta);
    strcpy(new->segtype, "o");
    strncpy(new->instype, i_wfd->instype, 6);
    new->wfid   = i_wfd->wfid;
    new->chanid = i_wfd->chanid;
    new->calib  = i_wfd->calib;
    new->calper = i_wfd->calper;
    new->commid = commid;

/***********************************************************************
  End init_wfdisc
***********************************************************************/
}


/*======================================================================
    css_bytes_left                                        DAP 02/19/94
  Return the number of bytes between the current time and the end
  of the day fraction.
=======================================================================*/
static long css_bytes_left(csstim, sint, bps, day_fract)
double csstim;             /* epoch time of the first sample in file   */
double sint;               /* sample interval                          */
int    bps;                /* bytes per sample                         */
int    day_fract;          /* split time period, percent of day        */

/***********************************************************************
  Begin css_bytes_left
***********************************************************************/
{

/***********************************************************************
  Variable Declarations
***********************************************************************/
    int i, year, day;
    long  bytes_left, samps_left;
    double secs, secs_left, splitime;

/***********************************************************************
  Calculate number of seconds since begining of the year
***********************************************************************/
    year = css_to_dbltim(csstim, &secs);
    day  = secs/86400.0;

/***********************************************************************
  Find split time
***********************************************************************/
    splitime = ((double)day + day_fract/100.0) * 86400.0;

    secs_left  = splitime - secs + sint;
    samps_left = (long) (secs_left/sint);
    bytes_left = samps_left * bps;

    if (bytes_left <= 0) {
      fprintf(stderr, "ERROR: css_bytes_left (=%d) failed\n", bytes_left);
      exit(-1);
    }

/***********************************************************************
  Return normally
***********************************************************************/
    return (bytes_left);

/***********************************************************************
  End css_bytes_left
***********************************************************************/
}

/* Revision History
 *
 * $Log: day_split.c,v $
 * Revision 1.1  2011/03/17 20:23:19  dechavez
 * initial release, based on DCC time_vol sources
 *
 */
