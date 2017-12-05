#pragma ident "$Id: globals.h,v 1.1 2011/03/17 20:23:20 dechavez Exp $"
/*======================================================================
 *
 *  dcctvol/globals.h
 *
 *====================================================================*/
#include <stdio.h>
#include <sys/types.h>
#include <sys/file.h>
#include <math.h>
#include "ida10.h"

#define IRIS_HEAD_SIZE (64)
#define IRIS_DATA_SIZE (960)
#define IRIS_BUFSIZ    (IRIS_HEAD_SIZE + IRIS_DATA_SIZE)

#ifndef SEC_PER_MIN
#define SEC_PER_MIN 60
#endif

#ifndef MIN_PER_HOUR
#define MIN_PER_HOUR 60
#endif

#ifndef SEC_PER_HOUR
#define SEC_PER_HOUR (SEC_PER_MIN * MIN_PER_HOUR)
#endif

#ifndef HOUR_PER_DAY
#define HOUR_PER_DAY 24
#endif

#ifndef MIN_PER_DAY
#define MIN_PER_DAY (MIN_PER_HOUR * HOUR_PER_DAY)
#endif

#ifndef SEC_PER_DAY
#define SEC_PER_DAY (SEC_PER_MIN * MIN_PER_DAY)
#endif

#ifndef MSEC_PER_SEC
#define MSEC_PER_SEC (UINT64) 1000
#endif

#ifndef USEC_PER_SEC
#define USEC_PER_SEC (MSEC_PER_SEC * (UINT64) 1000)
#endif

#ifndef USEC_PER_MSEC
#define USEC_PER_MSEC (USEC_PER_SEC / MSEC_PER_SEC)
#endif

#ifndef NANOSEC_PER_SEC
#define NANOSEC_PER_SEC (USEC_PER_SEC * (UINT64) 1000)
#endif

#define utilNsToS(nsec) ((REAL64) (nsec) / NANOSEC_PER_SEC)

#ifndef NANOSEC_PER_MSEC
#define NANOSEC_PER_MSEC (NANOSEC_PER_SEC / MSEC_PER_SEC)
#endif

#ifndef NANOSEC_PER_USEC
#define NANOSEC_PER_USEC (NANOSEC_PER_SEC / USEC_PER_SEC)
#endif

#ifndef TRUE
#define TRUE  1
#define FALSE 0
#endif

#ifndef leap_year
#define leap_year(i) ((i % 4 == 0 && i % 100 != 0) || i % 400 == 0)
#endif
#ifndef dysize
#define dysize(i) (365 + leap_year(i))
#endif

#define MAXSTRM 100	/* max chans on a logger */
                        /* including channels with less sampling rate */
#define	MAXSEGS	100	/* maximum time fits to calculate */

enum {ERR_RET=-1, DONE, AREC, CREC, DREC, RREC, HREC, LREC,
      EREC, KREC, PREC, TREC, UREC, ZREC, MYREC };

enum {ANT, MK6B, MK7A, MK7B, MK7C, MK8, Q330, FEMTO};

/* variation to accommodate sample rates */

#define WFDISC_WCS2 "%-6.6s %-8.8s %17.5f %8ld %8ld %8ld %17.5f %8ld %11.7f %16.6f %16.6f %6.6s %1s %-2.2s %1s %-64.64s %-32.32s %10ld %8ld %-17.17s\n"

/* next needed to read H records */
 
#define YEAR_OFFSET     6
#define EXT_OFFSET     14
#define UNITID_OFFSET  24
#define SNAME_OFFSET   26
#define LASTBEG_OFFSET 34
#define LASTEND_OFFSET 38
#define ARSID_OFFSET   42
#define ARSREV_OFFSET  44
#define DASID_OFFSET   46
#define DASREV_OFFSET  48

/* next needed to read D or A records */
#define FORMAT_OFFSET	34 
#define SYS_CLOCK_INT	8

/* next needed to read T records */
#define IDA10_STREAM_OFFSET	50 

/* ---------------------------------------------------------------------- */
	/* structure definitions */
/* ---------------------------------------------------------------------- */

typedef struct counters {
    long rec ;        /* current record number                          */
    long Arec ;       /* number of 'A' records read                     */
    long Crec ;       /* number of 'C' records read                     */
    long Drec ;       /* number of 'D' records read                     */
    long Hrec ;       /* number of 'H' records read                     */
    long eHrec ;      /* number of "empty" 'H' records read             */
    long Lrec ;       /* number of 'L' records read                     */
    long Krec ;       /* number of 'K' records read                     */
    long Erec ;       /* number of 'E' records read                     */
    long Prec ;       /* number of 'P' records read                     */
    long Rrec ;       /* number of 'R' records read                     */
    long Trec ;       /* number of 'T' records read                     */
    long zrec ;       /* number of all zero records read                */
    long urec ;       /* number of unrecognized records read            */
    long trig ;       /* number of triggers detected                    */
    long unlocks;     /* number of records for which clock unlocked     */
} COUNTERS ;

static COUNTERS cnt_null = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};


typedef struct streams {
	short	inchan;		/* input channel number */
	short	nfilter;	/* filter number */
	short	mode;		/* 0 continuous, 1 triggered */
	short	keep ;		/* flag on whether to retain or not */
	long	samp_size;	/* sample size in bytes, either 2 or 4 or 8 or 1 */
	long	count ;		/* number of records for this stream */
	long	foff0;		/* initial file offset (for skipped recs) */
	double	samprate;	/* sample rate in Hz */
	FILE	*fp ;		/* file pointer to tmpfile holding recs */
	char	chan[9];	/* SEED channel name */
	char	filename[64];	/* tmp file name */
	char    datatype[2];	/* data type */
} STREAMS;

static STREAMS str_null = {
  -1,				/* input channel number */
  -1,				/* filter number */
  0,				/* mode: 0 continuous, 1 triggered */
  1,				/* keep flag set to TRUE */
  4,				/* sample size in bytes */
  0,				/* num records this stream */
  0,				/* initial file offset (for skipped recs) */
  -999.,			/* sample rate in Hz */
  NULL,				/* file pointer */
  "-",				/* SEED channel name */
  "",				/* tmp file name */
};

typedef struct ida_configuration {
	long	recbeg ;	/* beginning record number applicable */
	long	recend ;	/* ending record number applicable */
	int	nactive ;	/* number of active streams */
	STREAMS	strms[MAXSTRM];	/* structure holding streams content */
	COUNTERS	counts;	/* counters array at pt this K rec found */
} IDACONFIG;

static IDACONFIG idaconfignull = {
  0,				/* beginning record number applicable */
  9999999,			/* ending record number applicable */
  0,				/* number of active streams */
  NULL,				/* structure holding streams content */
  NULL				/* counters array at pt this K rec found */
};

typedef struct ida_times {
	double		sys_start;
	double		pulse_ext_start;
	double		pulse_sys_start;
	double		sys_end;
	double		pulse_ext_end;
	double		pulse_sys_end;
	short		qf_start;
	short		qf_end;	
	float		clock_int;
	short		nsamp;
	short		year;
	long		rec_num;
} IDATIME;


typedef struct segments {
	long	das_flag;	/* flag TRUE if DAS segment, FALSE if ARS segment */
	long	n_start;	/* tmpfile record number of segment start (chan coords) */
	long	n_end;		/* tmpfile record number of segment end (chan coords) */
	long	m_start;	/* tape record number of segment start (volume coords) */
	long	m_end;		/* tape record number of segment end (volume coords) */
	long	foff;		/* file offset as in wfdisc */
	long	nsamp;		/* number of samples in this segment */
	long	year;		/* year */
	double	start;		/* system time of segment start */
	double	end;		/* system time of segment end */
	double	drift;		/* clock drift */
	double	intercept;	/* clock intercept */
	double	clock_factor;	/* clock factor */
	int	reboot;		/* flag true if reboot occurs at start */
	int	new_config;	/* flag true if config changes at start */
	int	str_number;	/* stream number */
	STREAMS	stream;		/* stream for this segment */
} SEGMENTS;

static SEGMENTS seg_null = {
   TRUE,			/* assume DAS segment */
  -1,				/* tmpfile record number of segment start */
  -1,				/* tmpfile record number of segment end */
  -1,				/* tape record number of segment start */
  -1,				/* tape record number of segment end */
   0,				/* system time of segment start */
   0,				/* system time of segment end */
   0,				/* file offset as in wfdisc */
   0,				/* number of samples in this segment */
   0,				/* year */
  0.0,				/* clock drift */
  0.0,				/* clock intercept */
  0,				/* reboot flag */
  0,				/* new_config flag */
  -1,				/* stream number */
  NULL				/* pointer to stream */
};


/* ---------------------------------------------------------------------- */
	/* variables with program scope */
/* ---------------------------------------------------------------------- */

unsigned char	buffer[IDA10_MAXRECLEN];

#ifndef IRISLST_MAIN

int		n_configs ;
COUNTERS	cnt;
STREAMS		**st;
FILE		*fpheaders;
FILE		*fplogsfile;
IDACONFIG	*idaconfig;
int		nsegs ;
SEGMENTS	*segs;
int		time_tolerance;
char		prefix[64];
SEGMENTS	tsegs[MAXSEGS];
SEGMENTS	das_segs[MAXSEGS];
SEGMENTS	ars_segs[MAXSEGS];
int		n_tsegs;
int		n_dassegs;
int		n_arssegs;
int		logger_type;
int		nskip;
int		n_maxrec;
char		stat_name[8];
int		connect_segment;
int		time_coef;		/* flag for forcing drift/intercept */
int		fit_ext_jumps;		/* flag for forcing fit of external jumps */
char		force_drift_file[128];
int		print_Hrecs;		/* flag to print H records or not */
int		print_Lrecs;		/* flag to print L records or not */
double		time_correction;	/* time correction in seconds */
int		set_year ;		/* year to which data are to be set */
double		sb_time ;		/* for insertion to Antelope */
double		sb_endtime ;		/* for insertion to Antelope */
int		stream2use;		/* use this stream for time control */
long		*discards;		/* array of record numbers to discard */
int		n_discards;		/* number of pairs in discards array */
long		*forcerecs;		/* array of record numbers to force fits */
int		n_forcerecs;		/* number of pairs in forcerecs array */
void		finish_up();
double		clock_factor;		/* multiply int clock by this number to get ms */
double		clock_factor_DAS;	/* clock_factor for DAS */
double		clock_factor_ARS;	/* clock_factor for ARS */
int		antelope;		/* flag for using Antelope */
int		RunPlotxy;	/* flag for invoking plotxy */

#else

int		n_configs = 0;
COUNTERS	cnt;
STREAMS		**st = NULL;
FILE		*fpheaders  = NULL;
FILE		*fplogsfile = NULL;
IDACONFIG	*idaconfig  = NULL;
int		nsegs = 0;
SEGMENTS	*segs = NULL;
int		time_tolerance = 0;	/* tolerance in units of ticks */
char		prefix[64];
SEGMENTS	tsegs[MAXSEGS];		/* time segments fit */
SEGMENTS	das_segs[MAXSEGS];	/* DAS time segments */
SEGMENTS	ars_segs[MAXSEGS];	/* ARS time segments */
int		n_tsegs = 0;		/* number of time segment to fit */
int		n_dassegs = 0;
int		n_arssegs = 0;
int		logger_type = MK7B;
int		nskip = 0;
int		n_maxrec = 99999999;
char		stat_name[8];
int		connect_segment = FALSE;
int		time_coef = FALSE;	/* flag for forcing drift/intercept */
int		fit_ext_jumps = FALSE;	/* flag for forcing fit of external jumps */
char		force_drift_file[128];
int		print_Hrecs = FALSE;	/* flag to print H records or not */
int		print_Lrecs = FALSE;	/* flag to print L records or not */
double		time_correction;	/* time correction in seconds */
int		set_year = -1;		/* year to which data are to be set */
double		sb_time = -1. ;		/* for insertion to Antelope */
double		sb_endtime = -1.;	/* for insertion to Antelope */
int		stream2use = -1;	/* use this stream for time control */
long		*discards = NULL;	/* array of record numbers to discard */
int		n_discards = 0;		/* number of pairs in discards array */
long		*forcerecs = NULL;	/* array of record numbers to force fits */
int		n_forcerecs = 0;	/* number of pairs in forcerecs array */
void		finish_up();
double		clock_factor = 200.0;	/* multiply int clock by this number to get ms */
double		clock_factor_DAS = 200;	/* clock_factor for DAS */
double		clock_factor_ARS = 200;	/* clock_factor for ARS */
int		antelope = FALSE;	/* flag for using Antelope */
int		RunPlotxy = FALSE;	/* flag for invoking plotxy */

#endif

/* Revision History
 *
 * $Log: globals.h,v $
 * Revision 1.1  2011/03/17 20:23:20  dechavez
 * initial release, based on DCC time_vol sources
 *
 */
