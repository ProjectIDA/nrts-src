#pragma ident "$Id: main.c,v 1.1 2011/03/17 20:23:21 dechavez Exp $"
int print_summary = 1;        /* flag used for suppressing summary */
int newconfig = 0;            /* flag indicating if configuration has changed */
int klast = -1, new_segment;
char *words[] = {"          ", "new config"}; /* summary words */
/*======================================================================
 *
 *  main.c
 * 
 *  Program to list and/or check the contents of IGPP/IRIS data logger 
 *  output.  Produces wfdisc from header file contents. Writes to
 *  standard output.
 *
 *====================================================================*
 *  modifications: 30/04/97   M.S. O'Brien
 *  tcheck() -- formerly reset the data stream counter each time a new 
 *              configuration was encountered.  This has been corrected.
 *  check external() -- rewritten to explicitly look for large gaps and
 *              jumps in the external clock.
 *  form_segment() -- new case included for new conficuration but same
 *              stream
 *====================================================================*/
#define IRISLST_MAIN
#include "globals.h"

static PrintTsummary(int i, IDATIME *t)
{
static int count = 0;
	printf("%6d:", i);
	printf(" %6ld",  t->rec_num);
	printf(" %.3lf", t->sys_start);
	printf(" %.3lf", t->pulse_ext_start);
	printf(" %.3lf", t->pulse_sys_start);
	printf(" %.3lf", t->sys_end);
	printf(" %.3lf", t->pulse_ext_end);
	printf(" %.3lf", t->pulse_sys_end);
	printf(" %hd",   t->qf_start);
	printf(" %hd",   t->qf_end);
	printf(" %.3f",  t->clock_int);
	printf(" %hd",   t->nsamp);
	printf(" %hd",   t->year);
	printf("\n");
}

main(argc, argv)
int    argc;
char    *argv[];
{
	int		i, j, iloop = 1;
	int		process_stream();
	double		time1, time2;
	double		dnumber;
	double		calc_time();
	double		leap_second, check_leap();
	char		st1[32], st2[32];
	char        command[80];
	extern void	check_drifts();
	extern void	update_drift();
	IDACONFIG	*p, *test_con();
	SEGMENTS	*s;
	void		make_CSS_files();

  /* initialize */

	init(argc, argv);
	if ( print_Lrecs ) fplogsfile = tmpfile();
	p = &idaconfig[0];

  /* first pass - segregate data header records into temporary files */

   	while ( iloop )
	{
	   switch ( rdata(fpheaders) )
	   {
       	     case AREC:
       	     case DREC:   iloop = doDrec(p); break; /* this for loggers before IDA10 */
       	     case HREC:   doHrec();  break;
       	     case LREC:   doLrec();  break;
       	     case CREC:
       	     case KREC:
       	     case EREC:
       	     case PREC:
       	     case RREC:   break;
       	     case TREC:   iloop = doTrec(p);  break; /* this for IDA10 */
	     case UREC:   cnt.urec++; break;
	     case ZREC:   cnt.zrec++; break;
	     case MYREC:  break;

       	     case DONE:   iloop = 0; break;
	     case ERR_RET:
			fprintf(stderr,"dcctvol: incomplete read at rec %d\n",
				cnt.rec);
			finish_up(1);

	     default:   fprintf(stderr,
				"dcctvol: bad (main) rdata return -- rec %d\n",
					cnt.rec);
			finish_up(1);
           }
	   p = test_con(cnt.rec);
	}

	printf("Input contains\t%7ld 'H' records\n", cnt.Hrec);
	printf("\t\t%7ld 'R' records\n", cnt.Rrec);
	printf("\t\t%7ld 'P' records\n", cnt.Prec);
	printf("\t\t%7ld 'C' records\n", cnt.Crec);
	printf("\t\t%7ld 'K' records\n", cnt.Krec);
	printf("\t\t%7ld 'E' records\n", cnt.Erec);
	printf("\t\t%7ld 'L' records\n", cnt.Lrec);
	printf("\t\t%7ld 'A' records\n", cnt.Arec);
	printf("\t\t%7ld 'D' records\n", cnt.Drec);
	printf("\t\t%7ld 'T' records\n", cnt.Trec);
	printf("\t\t%7ld unrecognized records\n", cnt.urec);
	printf("\t\t%7ld zero-length records\n", cnt.zrec);
	printf("\t\t-------\n\t\t%7ld total\n\n",cnt.rec);


  /* second pass - process separate data channels */
	print_summary = 1;

    /* if stream2use set, use it as time code instead */
	if ( stream2use > -1 )
	{
		if ( process_stream(stream2use) )
		{
			printf("\nWARNING: Data from stream %d unrecognized.\n", stream2use);
		}
       		print_summary = 0;
	}

	for (i=0; i<MAXSTRM; i++)
	{
		if ( i == stream2use ) continue;

		if ( process_stream(i) )
		{
			printf("\nWARNING: Data from stream %d unrecognized.\n", i);
		}
		print_summary = 0;
	}

	/* RUN PLOTXY ON THE COMMANDS THAT HAVE BEEN GENERATED */
	if (RunPlotxy) {
    	printf ("running plotxy...\n");
		sprintf(command,"plotxy < /tmp/tv.%d.plotxy > /tmp/pxyjunk",getpid());
		system(command);
        printf ("cleaning up...\n");
	    sprintf(command,"rm /tmp/tv.%d.plotxy /tmp/pxyjunk",getpid());
	    system(command);
	}

  /* if there is a time correction to be applied, apply it here */

	if ( time_correction != 0.0 )
	{
		printf("\nApplying time correction of %lf seconds.\n",
			time_correction);
		for(i=0; i<n_dassegs; i++) das_segs[i].intercept += time_correction;
		for(i=0; i<n_arssegs; i++) ars_segs[i].intercept += time_correction;
		for(i=0; i<nsegs; i++) segs[i].intercept += time_correction;
	}

  /* summarize fitting */

	printf("\n\t\tSegment time fitting summary:\n\n");
	printf("    Seg    Drift        Intercept\n");
	printf("    --- ------------- ---------------------------------\n");
	for(i=0; i<n_dassegs; i++)
	{
		strcpy(st1, dbltim_to_ascii(das_segs[i].intercept) );
		if ( abs(das_segs[i].intercept) < 1e-5 )
		{
			if ( (das_segs[i].intercept == 0) && (das_segs[i].drift == 0) )
				/* Q330 will always fall into here */
				printf("DAS  %2d %e %13.3lf (%s) - a perfect clock\n",
					i+1, das_segs[i].drift, das_segs[i].intercept, st1);
			else
				printf("DAS  %2d   NULL           NULL\n",i+1);
		}
		else
		{
			printf("DAS  %2d %e %13.3lf (%s)\n",
				i+1, das_segs[i].drift, das_segs[i].intercept, st1);
		}
	}
	for(i=0; i<n_arssegs; i++)
	{
		strcpy(st1, dbltim_to_ascii(ars_segs[i].intercept) );
		if ( abs(ars_segs[i].intercept) < 1e-5 )
		{
			if ( (ars_segs[i].intercept == 0) && (ars_segs[i].drift == 0) )
				printf("ARS  %2d %e %13.3lf (%s) - a perfect clock\n",
					i+1, ars_segs[i].drift, ars_segs[i].intercept, st1);
			else
				printf("ARS  %2d   NULL           NULL\n",i+1);
		}
		else
		{
			printf("ARS  %2d %e %13.3lf (%s)\n",
				i+1, ars_segs[i].drift, ars_segs[i].intercept, st1);
		}
	}

  /* show comparable results from other volumes from same station */
	if ( antelope ) check_drifts();

  /* summarize segmentation */

	printf("\n\n\t\tSegmentation summary:  (triggered channels omitted)\n\n");
	printf("cha         tape recs  reboot       times");
	printf("                          str recs   drift\n");
	printf("        start      end flag   start            end");
	printf("              start     end assigned\n");
	printf("-----  -------  ------- - ----------------- ");
	printf("----------------- ------- ------- ----------\n");

	for (i=0, s=&segs[0] ; i<nsegs; i++, s++)
	{
		clock_factor = ( s->das_flag ) ? clock_factor_DAS : clock_factor_ARS;
	   /* finish loading segment structure here */
		s->clock_factor = clock_factor;

		time1 = calc_time( s->start, s->drift, s->intercept, s->clock_factor );
		time2 = calc_time( s->end  , s->drift, s->intercept, s->clock_factor );
		strcpy(st1, dbltim_to_ascii(time1) );
		strcpy(st2, dbltim_to_ascii(time2) );
		if ( s->m_end > 0  &&  !s->stream.mode )
		{
		    printf("%-5.5s %8ld %8ld %d %s %s %7d %7d %8.3e\n", 
			s->stream.chan, s->m_start, s->m_end,
			s->reboot, st1, st2, s->n_start + 1, s->n_end + 1,
			s->drift);
		}
	}

 /* check leap second file for possible leap second */
	leap_second = check_leap( segs[0].year );

 /* if a leap second is expected, check to see if one was detected */
	for (i=0 ; i<nsegs-1 && !(segs[i+1].str_number - segs[i].str_number) ; i++)
	{
	  /* if drift similar and intercept off by integral 1 sec ... */
		if ( (segs[i].drift/segs[i+1].drift) < 1.1 &&
		     (segs[i].drift/segs[i+1].drift) > 0.9 )
		{
		   dnumber = ( segs[i].intercept * segs[i+1].intercept > 0.0 ) ?
			fabs( segs[i].intercept - segs[i+1].intercept ) :
			fabs( dysize(segs[i].year) * 86400. - segs[i].intercept + segs[i+1].intercept) ;

		   if ( dnumber < 1.01 && dnumber > 0.99 )
		   {
			printf("\n**************************************************\n");
			printf("*** Leap second detected between segments %d and %d\n",
				i+1, i+2);
			printf("**************************************************\n\n");
		   }
		}
	}	

 /* produce final CSS files */

	make_CSS_files( leap_second );


	printf("\n\nClock unlocked on %ld records (%.1lf%%).\n",
		cnt.unlocks, 100. * (double)cnt.unlocks/cnt.rec);

  /* if connected to Antelope, update Antelope record accordingly */
	if ( antelope ) update_drift();

  /* print out abbreviated log files if requested */
	if ( print_Lrecs ) printLrecords();

	finish_up(0);
}

IDACONFIG *
test_con(n)
	int	n;
{
	int		i;
	IDACONFIG	*p;

	for (i=0, p=&idaconfig[i]; i<n_configs; i++, p++)
	{
		if ( n >= idaconfig[i].recbeg &&
		     n <= idaconfig[i].recend )
		{
			return(p);
		}
	}
	return(p);
}

int
doDrec(p)
	IDACONFIG	*p;
{
	int		i;
	int		ibytes;
	static int	first_point_reached = FALSE;
	short		str_id;
	short		data_format, nsamples;

  /* determine the stream ... */

	memcpy(&str_id, buffer + 56, 2); /* read stream id */
	if ( str_id < 0 || str_id > MAXSTRM )
	{
		fprintf(stderr, "Corrupt record %ld\n", cnt.rec);
		cnt.urec++;
		return(TRUE);
	}

  /* write record number into buffer in place of station code */
	memcpy(buffer + 26, &cnt.rec, 4);

  /*
   * update sample size of the appropriate config record by reading data format
   * in record and then looking at state of first bit
   */
	memcpy(&data_format, buffer + FORMAT_OFFSET, 2);
	p->strms[str_id].samp_size = ( data_format & 1 ) ? 2 : 4 ;
	if (p->strms[str_id].samp_size == 4)
	    strcpy(p->strms[str_id].datatype, "s4");
	else
	    strcpy(p->strms[str_id].datatype, "s2");

  /* if this is outside the bounds of nskip, n_maxrec, return */
	if ( cnt.rec > n_maxrec ) return(FALSE);

	if ( cnt.rec <= nskip )
	{
		memcpy(&nsamples, buffer + 40, 2);
		p->strms[str_id].foff0 += nsamples * p->strms[str_id].samp_size ;
		return(TRUE);
	}

  /* write to appropriate tmpfile */

	p->strms[str_id].count++;
	if ( p->strms[str_id].fp == NULL )
	{
		cnt.urec++;
		return(TRUE);
	}
	else /* write out buffer */
	{
		ibytes = fwrite(&buffer, 1, IRIS_HEAD_SIZE, p->strms[str_id].fp);
		if ( ibytes != IRIS_HEAD_SIZE )
		{
			fprintf(stderr, "dcctvol: write error to tmpfile. exiting.\n");
			finish_up(1);
		}
	}

	first_point_reached = TRUE;
	return(TRUE);
}

int doTrec(IDACONFIG *p)
{
IDA10_TSHDR hdr;
	int		i;
	int		ibytes;
	static int	first_point_reached = FALSE;
	short		str_id;
	short		data_format, nsamples, xx;
	char		data_format_tmp;

	if (!ida10UnpackTSHdr(buffer, &hdr)) {
		fprintf(stderr, "dcctvol: ida10UnpackTSHdr failed for record %d, skipped\n", cnt.rec);
		return FALSE;
	}

  /* determine the stream ... */

	for ( i=0, str_id = -1; i<MAXSTRM; i++ )
        {
		if (!strcmp(hdr.cname,"pha") || !strcmp(hdr.cname,"ple") || 
		    !strcmp(hdr.cname,"pll") || !strcmp(hdr.cname,"pps") 
		    )

		{
		return (99);
		}

		if ( !strcmp( p->strms[i].chan, hdr.cname ))
		{
			str_id = i;
			continue;
		}
        }
	if ( str_id < 0 )
	{
		fprintf(stderr, "dcctvol: IDA10 data stream `%s' not recognized. Exiting.\n", hdr.cname);
		exit(1);
	}

  /* write record number into buffer in place of channel (stream) code for now */

	memcpy(buffer + IDA10_STREAM_OFFSET, &cnt.rec, 4);

   /* update sample size of the appropriate config record */

	switch (hdr.datatype) {
	  case IDA10_DATA_INT8:   p->strms[str_id].samp_size = 1; break;
	  case IDA10_DATA_INT16:  p->strms[str_id].samp_size = 2; break;
	  case IDA10_DATA_INT32:  p->strms[str_id].samp_size = 4; break;
	  case IDA10_DATA_REAL32: p->strms[str_id].samp_size = 4; break;
	  case IDA10_DATA_REAL64: p->strms[str_id].samp_size = 8; break;
	  default:
		fprintf(stderr, "dcctvol: IDA10 data type %d not recognized. Exiting.\n", hdr.datatype);
		exit(1);
	}

	strcpy(p->strms[str_id].datatype, ida10DataTypeString(&hdr));

  /* if this is outside the bounds of nskip, n_maxrec, return */

	if ( cnt.rec > n_maxrec ) return(FALSE);

	if ( cnt.rec <= nskip )
	{
		p->strms[str_id].foff0 += hdr.nsamp * p->strms[str_id].samp_size ;
		return(TRUE);
	}

  /* write to appropriate tmpfile */

	p->strms[str_id].count++;
	if ( p->strms[str_id].fp == NULL )
	{
		cnt.urec++;
		return(TRUE);
	}
	else /* write out buffer */
	{
		ibytes = fwrite(&buffer, 1, IRIS_HEAD_SIZE, p->strms[str_id].fp);
		if ( ibytes != IRIS_HEAD_SIZE )
		{
			fprintf(stderr, "dcctvol: write error to tmpfile. exiting.\n");
			finish_up(1);
		}
	}

	first_point_reached = TRUE;
	return(TRUE);
}

int
doHrec()
{
	int	das_record;
	short	stemp;
	long	ltemp;
	char	ctemp[8];

	if ( !print_Hrecs ) return ;
 
	memcpy(&stemp, buffer + UNITID_OFFSET, 2);
	das_record = (stemp == 0) ? FALSE : TRUE;

	if ( das_record ) printf("DAS ident (H) read at record %d\n", cnt.rec);
	else              printf("ARS ident (H) read at record %d\n", cnt.rec);

	memcpy(ctemp, buffer + SNAME_OFFSET, 7);
	ctemp[7] = 0;
	printf("Station code      = %s\n", ctemp);

	if ( !das_record )
	{
		memcpy(&stemp, buffer + ARSID_OFFSET,  2);
		printf("CPU ident         = %hd\n", stemp);
		memcpy(&stemp, buffer + ARSREV_OFFSET, 2);
		printf("Firmware revision = %hd\n", stemp);
	}
	else
	{
		memcpy(&stemp, buffer + DASID_OFFSET,  2);
		printf("CPU ident         = %hd\n", stemp);
		memcpy(&stemp, buffer + DASREV_OFFSET, 2);
		printf("Firmware revision = %hd\n", stemp);
	}

	memcpy(&stemp, buffer + YEAR_OFFSET, 2);
	printf("Year number       = %hd\n", stemp);

	memcpy(&ltemp,  buffer +  EXT_OFFSET, 4);
	printf("Time stamp        = %s\n", dbltim_to_ascii( (double)ltemp ));

	if ( das_record )
	{
		printf("\n");
		return;
	}

	memcpy(&ltemp,  buffer +  LASTBEG_OFFSET, 4);
	if ( ltemp != 0 ) ltemp -= 86400;
	printf("Prev tape start   = %s\n", dbltim_to_ascii( (double)ltemp ));
 
	memcpy(&ltemp,  buffer +  LASTEND_OFFSET, 4);
	if ( ltemp != 0 ) ltemp -= 86400;
	printf("Prev tape end     = %s\n", dbltim_to_ascii( (double)ltemp ));

	printf("\n");
}

int
doLrec()
{
	int		ibytes;

	if ( !print_Lrecs ) return(0);

  /* write record number into buffer in place of station code */
	memcpy(buffer + 26, &cnt.rec, 4);

  /* write to appropriate tmpfile */
	ibytes = fwrite(&buffer, 1, IRIS_BUFSIZ, fplogsfile);
	if ( ibytes != IRIS_BUFSIZ )
	{
		fprintf(stderr, "dcctvol: write error to tmpfile. exiting.\n");
		finish_up(1);
	}
}

static int ConvertIda10ttag(IDA10_TTAG *ttag, double *sys, double *pulse_sys, double *pulse_ext)
{
unsigned long tmpext;
int iyr, ida, ihr, imn, isc, unused;

	*sys = ttag->sys / 10000;
	*pulse_sys = (double) (ttag->hz  / 10000);

     /* calculate year and adjust external time to #secs since Jan 1 of current year */

	tmpext = (ttag->ext / NANOSEC_PER_SEC) + SAN_EPOCH_TO_1970_EPOCH;
	utilTsplit(tmpext, &iyr, &ida, &ihr, &imn, &isc, &unused);
	*pulse_ext  = ida * SEC_PER_DAY;
	*pulse_ext += ihr * SEC_PER_HOUR;
	*pulse_ext += imn * SEC_PER_MIN;
	*pulse_ext += isc;

	return iyr;
}

int
process_stream(n)
	int	n;
{
	int		i, j, k, keepit = 0;
	int		iread;
	int		nrecs = 0;
	static int	first_iteration = TRUE;
	static short	yr, year_diff;
	short		year ;
	float		ci ;
	char		cqual;
	double		nsr, tincr;
	double		nsrf, nsrm ;
	IDACONFIG	*p;
	FILE		*fp;
	IDATIME		*t;
	unsigned long int	ul, getul();
	unsigned long long int	ull, getull();
	unsigned short int	us, getus();
	int		tcheck();
	int		das_stream;
	int		time_it = 0;
	int		s0 = nsegs;
	static int	das_done = 0; /* TRUE if DAS drift, intercept calculated */
	static int	ars_done = 0; /* TRUE if ARS drift, intercept calculated */
	extern int	figure_time();
	long int	getli();
	long long	getll();
	extern short	check_year(), getss();
	char		getbyte();
	STREAMS		*s;
    IDA10_TSHDR tshdr;

  /* is this a stream we want to keep? */

	for (i=0, p=&idaconfig[0]; i<n_configs; i++, p++)
	{
		if ( p->strms[n].keep ) keepit = 1;
		if ( p->strms[n].fp != NULL ) fp = p->strms[n].fp;
		nrecs += p->strms[n].count;
	}
	if ( !keepit || nrecs <= 0 ) return(0);

  /* stream unrecognized */
	if ( !strcmp(idaconfig[0].strms[n].chan, "-") ) return(1);

  /* read data into memory */

	rewind( fp );
	t = (IDATIME *)malloc(nrecs * sizeof(IDATIME));
	if ( t == NULL )
	{
		fprintf(stderr, "Could not malloc time structure.\n");
		finish_up(1);
	}

	for (i=0; i<nrecs; i++)
	{
		iread = fread(buffer, 1, IRIS_HEAD_SIZE, fp);
		if ( iread != IRIS_HEAD_SIZE )
		{
			fprintf(stderr, "tmpfile not re-read correctly.\n");
			finish_up(1);
		}

	  /* Treat the offsets differently, depending upon data logger type: */

		if ( buffer[0] == 'T' )		/* IDA10 format */
		{

			if (!ida10UnpackTSHdr(buffer, &tshdr)) {
			    fprintf(stderr, "Error unpacking from tmpfile.\n");
			    finish_up(1);
			}

			year = ConvertIda10ttag(&tshdr.cmn.ttag.beg, &t[i].sys_start, &t[i].pulse_sys_start, &t[i].pulse_ext_start);
			       ConvertIda10ttag(&tshdr.cmn.ttag.end, &t[i].sys_end,   &t[i].pulse_sys_end,   &t[i].pulse_ext_end  );
			ci = t[i].clock_int = 0.010;
			t[i].rec_num = (long)getul( IDA10_STREAM_OFFSET ); /* this was saved here in rdata.c */
			if ((t[i].qf_start = t[i].qf_end = tshdr.cmn.ttag.beg.status.locked ? 1 : 0) != 1) cnt.unlocks++;
			t[i].nsamp = tshdr.nsamp;
//t[i].year = year;
//PrintTsummary(i, &t[i]);

		}
		else  /* not IDA10 */
		{
			t[i].sys_start =       (double)getul(10);
			t[i].pulse_ext_start = (double)getul(14);
			t[i].pulse_sys_start = (double)getul(18);
	
			t[i].sys_end =       (double)getul(42);
			t[i].pulse_ext_end = (double)getul(46);
			t[i].pulse_sys_end = (double)getul(50);

			t[i].nsamp     = getus(40);
			t[i].clock_int = (float) getus(8);


			memcpy(&cqual, buffer + 22, 1);
			t[i].qf_start = iris_tqual(cqual);

			memcpy(&cqual, buffer + 54, 1);
			t[i].qf_end = iris_tqual(cqual);

			t[i].rec_num = (long)getul(26);

			if ( t[i].qf_start != 1 ) cnt.unlocks++;

			year = (short)getus(6);
			ci   = (int)getus(SYS_CLOCK_INT);
		}
		
	  /* on the first iteration, check the year value */
		if ( first_iteration )
		{
			yr = check_year( year );
			year_diff = yr - year;

			first_iteration = FALSE;
		}

	  /* check clock factor */

		clock_factor = (double) (1000. / ci);

	  /* assign year value; this will be wrong over a year change! */

		t[i].year =  yr;
		if ( (yr - year) == (year_diff - 1)  && ( set_year <= 0 ) )
		{
			t[i].year =  yr + 1;
		}
	}
   /*
    * establish if this stream is timed by the DAS or ARS
    */
	if (das_stream != (buffer[0] == 'D' || buffer[0] == 'T')) print_summary = 1;
	das_stream = (buffer[0] == 'D' || buffer[0] == 'T') ? TRUE : FALSE;
   /*
    *  if this is a femtometer met stream, force it to be ARS:
    */
	if ((short) buffer[3] == 5) das_stream = FALSE;

   /*
    * define differing clock factors for ARS and DAS
    */
	if (das_stream) clock_factor_DAS = clock_factor;
	else		clock_factor_ARS = clock_factor;

   /* perform consistency checking and segmentations */
        print_summary = 0;
	tcheck(t, n, das_stream);
        print_summary = 0;

   /* establish whether or not to use this stream for calculating drift */
	if ( !das_done  &&  das_stream ) time_it = TRUE;
	if ( !ars_done  && !das_stream ) time_it = TRUE;

	s = &idaconfig[0].strms[n];
	if ( time_it )
	{
		figure_time(t, nrecs, s->samprate, s->chan, das_stream, s0);
		if (  das_stream ) das_done = TRUE ;
		if ( !das_stream ) ars_done = TRUE ;
	}

    /* assign drift, intercept to current segments */
	map_drift_stream(t, das_stream, n);

	free(&t[0]);
	return(0);
}

/* convenience utilities: */

unsigned long
getul(offset)
	int	offset;
{
	static unsigned long	ul;

	memcpy(&ul, buffer + offset, 4);
	return(ul);
}

long int
getli(offset)
	int	offset;
{
	static long int li;

	memcpy(&li, buffer + offset, 4);
	return(li);
}

long long
getll(offset)
	int	offset;
{
	static long long ll;

	memcpy(&ll, buffer + offset, 8);
	return(ll);
}

unsigned long long
getull(offset)
	int	offset;
{
	static unsigned long long ull;

	memcpy(&ull, buffer + offset, 8);
	return(ull);
}

unsigned short
getus(offset)
	int	offset;
{
	static unsigned short	us;

	memcpy(&us, buffer + offset, 2);
	return(us);
}


short
getss(offset)
	int	offset;
{
	static short	ss;

	memcpy(&ss, buffer + offset, 2);
	return(ss);
}

char
getbyte(offset)
	int	offset;
{
	static char	mybyte;

	memcpy(&mybyte, buffer + offset, 1);
	return(mybyte);
}

/*
 * function to check discards list; if nrec in list, return TRUE
 */
 
int
check_discards(nrec, reset)
	long    nrec;
	int	reset;
{
	static int      i0 = 0;
	int             i;

	if ( reset ) i0 = 0;
 
	if ( !n_discards || nrec < discards[2*i0]) return(FALSE);
 
	for(i=i0; i<n_discards; i++)
	{
		if ( nrec >= discards[2*i] && nrec <= discards[2*i+1] )
		{
			i0 = i;
			return(TRUE);
		}
	}
	return(FALSE);
}


/*
 * function to map drift and intercept of control stream to current stream
 */

int
map_drift_stream(t, das_stream, nstream)
	IDATIME		*t;
	int		das_stream;
	int		nstream;
{
	int		i, j, j0;
	static int	i0 = 0;
	long		s1, s2, t1, t2;
	SEGMENTS	*s;

	if ( das_stream )
	{
		memcpy(&tsegs[0], &das_segs[0], n_dassegs * sizeof(SEGMENTS));
	}
	else
	{
		memcpy(&tsegs[0], &ars_segs[0], n_arssegs * sizeof(SEGMENTS));
	}

	for (i=i0, j0=0, s=&segs[i0] ; i<nsegs; i++, s++, i0++)
	{
		s1 = s->m_start ;
		s2 = s->m_end ;

		for (j=j0; j<n_tsegs; j++)
		{
			t1 = tsegs[j].m_start;
			t2 = tsegs[j].m_end;

			s->drift     = tsegs[j].drift;
			s->intercept = tsegs[j].intercept;

			if ( s1 <= t2 && s->year == tsegs[j].year ) break;
		}

	  /* assign stream number */
		s->str_number = nstream;
	}
}

double
calc_time(t, drift, intercept, clock_factor )
	double		t;
	double		drift;
	double		intercept;
	double		clock_factor;
{
	double	tt, ttt;
	
	ttt = t / clock_factor;
	tt = ttt * (drift + 1.0) + intercept;
	return(tt);
}

int
tcheck(t, n, das_stream)
	IDATIME	*t;
	int	n, das_stream;
{
	int		i, j, k;
	int		cn_return;
	int		check_discards();
	float		tps;
	long		foff  = 0;
	long		nsamp = 0;
	double		sr;
	IDACONFIG	*p;

	if (print_summary)
	{
		if (das_stream) printf("\n    DAS\n");
		else	        printf("\n    ARS\n");
		printf("  Segment    origin              start       end\n");
		printf("  -------    ------              -----      -----\n");
	}

	for (i=0, k=0, klast=0, p=&idaconfig[0]; i<n_configs; i++, p++)
	{
	    /* DO NOTHING IF THE STREAM HAS NO DATA  */
        	if (p->strms[n].count==0) continue;

	    /* initialize for this stream */
	        newconfig = 1;
		foff += (long)p->strms[n].foff0;
		form_segment(p->strms[n], -k, 0, t[k].sys_start, t[k].sys_end,
		             t[k].rec_num, 0, foff, nsamp, t[k].year, das_stream);

		if (k==0 && print_summary)
			printf("* %7d   initial   new config%6d",nsegs,k);

	    /* calculate tps = tics/sample */
		sr = p->strms[n].samprate / 1000. ;
		tps = (float)( 1 / (sr*t[k].clock_int) );

		for(j=0; j<p->strms[n].count; j++, k++)
		{
		   /* track foff */
			foff  += t[k].nsamp * p->strms[n].samp_size;

			new_segment = 0;

		   /* check if this point to be discarded */
			if ( check_discards(t[k].rec_num, !j) ) continue;

		   /* track nsamp */
			nsamp += t[k].nsamp;

		   /* 1. check if internal time within record is consistent */
            		  /* DISCARD RECORD IF LENGTH INCONSISTENT */
			if ( check_internal(t,k,tps,p->strms[n],foff,nsamp) )
			{
				newconfig = 0;
				continue;
			}
            		if (klast==-1) continue;

		   /* 2. check for year change -- REBOOT RESULT */
            			/* REBOOT SEGMENT IF NEW YEAR */
			if ( check_yrchange(t,k,tps,p->strms[n],foff,nsamp,das_stream) )
			{
				nsamp = 0;
				newconfig = 0;
				klast = k;
				continue;
			}

		   /* 3. check for time tear between consecutive records */

            /* FITTING SEGMENT IF > 1 DAY TIME GAP -- SHOWS UP AS OFFSET */
            /* REBOOT SEGMENT IF NEXT TIME LESS THAN PRESENT TIME */
            /* PASSIVE SEGMENT IF RECORDED AND CALCULATED TIMES TOO DIFF */

			cn_return = check_next(t,k,tps,p->strms[n],foff,nsamp,das_stream);
			if ( cn_return == 1 )        /* A REBOOT */
            		{
                		nsamp = t[k].nsamp;
                		newconfig = 0;
                		continue;
            		}
            		else if ( cn_return == 2)    /* AN UNPROFOUND SEGMENT */
	    		{
                		new_segment = 1;
				nsamp = 0;
	    		}
	   		else if ( cn_return == -1 )  /* AN ABORTIVE SEGMENT */
              		  nsamp -= t[k].nsamp;
                

		  /* 4. check for internal/external time tear */
            		/* FITTING SEGMENT IF EXTERNAL OFFSET W/O INTERNAL OFFSET */
			if ( strcmp("wdo",p->strms[n].chan) && strcmp("wdx",p->strms[n].chan) )  /* don't do this check for MK7ISP wdo chans */ 
			{
			  if ( check_external(t,k,tps,p->strms[n],foff,nsamp,das_stream) )
			  {
				nsamp = 0;
				newconfig = 0;
				klast = k;
				continue;
			  }
			}

			newconfig = 0;
			klast = k;
		}

		segs[nsegs-1].n_end = klast;
		segs[nsegs-1].m_end = t[klast].rec_num;
		segs[nsegs-1].end = t[klast].sys_end;
		segs[nsegs-1].year = t[klast].year;
		segs[nsegs-1].nsamp = nsamp;
	}
	if (print_summary)
        printf("%11d\n     * indicates start of data fitting segment\n",k-1);
}

int
check_internal( t, k, tps, s, foff, nsamp )
	IDATIME	*t;
	int	k;
	float	tps;
	STREAMS	*s;
	long	foff, nsamp;
{
	long		t1, t2;
	long		ns;

	t1 = (long)( t[k].sys_end - t[k].sys_start );
	t2 = (long)( tps * (t[k].nsamp - 1) );

/*	printf("%f %f\n", (float)t1,(float)t2);*/

	if ( abs( t1 - t2 ) > time_tolerance )
	{
	  /* discard this record */

		ns = nsamp - t[k].nsamp;
		if ( k==0 || k==segs[nsegs-1].n_start)
		{
            segs[nsegs-1].n_start++;
            segs[nsegs-1].start = t[k].sys_start;
            segs[nsegs-1].m_start = t[k].rec_num;
            segs[nsegs-1].foff = foff ;
            if (print_summary) printf("\b\b\b\b\b\b%6d",segs[nsegs-1].n_start);
			return(0);
		}
		else
		{
           /*  segs[nsegs-1].n_start++; */
           /*  segs[nsegs-1].start = t[k].sys_start; */
           /*  segs[nsegs-1].m_start = t[k].rec_num; */
           /*  segs[nsegs-1].foff = foff ; */
           /*  if (print_summary) printf("\b\b\b\b\b\b%6d",segs[nsegs-1].n_start); */
			return(1);
		}
	}
	return(0);
}

#define SECS_IN_DAY   86400
#define GAP_TOL    10.0

int
check_next( t, k, tps, s, foff, nsamp, das_stream ) 
	IDATIME	*t;
	int	k;
	float	tps;
	STREAMS	*s;
	long	foff, nsamp;
	int	das_stream;
{
	int	reboot;
	long	t1, t2;
	long	ns, nfoff;

	if ( k==0 || k==s->count) return(0);

	/* use "nint" here should not affect MK7 or MK8, since these numbers are
	   already in integers, 5 tics/ms for MK7 and 1 tic/ms for MK8 */
	t1 = (long) (rint) ( t[k].sys_start - t[klast].sys_start ) ;
	t2 = (long)( tps * t[k].nsamp ) ;

   /* CHECK FOR A SINGLE GLITCH */

	if ((t[k].pulse_sys_start > t[klast].pulse_sys_start &&
             t[k].pulse_sys_start > t[k+1].pulse_sys_start) ||
            (t[k].pulse_sys_start < t[klast].pulse_sys_start &&
             t[klast].pulse_sys_start < t[k+1].pulse_sys_start))
	{
		return(0);
	}
          

	if (t1/clock_factor/SECS_IN_DAY > GAP_TOL)  /* GAP GREATER THAN 10 DAYS */
	{
		ns = nsamp - t[k].nsamp;
		nfoff = foff - t[k].nsamp * s->samp_size;
		if ( form_segment(s, k, -2, t[k].sys_start, t[klast].sys_end,
			t[k].rec_num, t[klast].rec_num, nfoff, ns, t[k].year, das_stream) )
		{
			return(-1);
		}
		if (print_summary) printf("%11d\n* %7d   time gap  %10.10s%6d",
			klast,nsegs,words[newconfig],k);
		return(1);
	}

	else if ( abs(t1 - t2) > time_tolerance )
	{
	  /* segment */

		reboot = ( t[klast].sys_start > t[k].sys_start) ? TRUE : FALSE ;
		ns = nsamp - t[k].nsamp;
		nfoff = foff - t[k].nsamp * s->samp_size;
		if ( form_segment(s, k, reboot, t[k].sys_start, t[klast].sys_end,
			     t[k].rec_num, t[klast].rec_num, nfoff, ns, t[k].year, das_stream) )
		{
			return(-1);
		}

	        if (reboot)
	        {
	            if (print_summary) printf("%11d\n* %7d    reboot   %10.10s%6d",
                                      klast,nsegs,words[newconfig],k);
		    return(1);
	        }
	        else
	        {
	            if (print_summary) printf("%11d\n  %7d   int tear  %10.10s%6d",
                                      klast,nsegs,words[newconfig],k);
		    return(2);
	        }
	}
	return(0);
}

#define SECS_IN_YEAR 31536000

int
check_yrchange( t, k, tps, s, foff, nsamp, das_stream ) 
	IDATIME	*t;
	int	k;
	float	tps;
	STREAMS	*s;
	long	foff, nsamp;
	int	das_stream ;
{
	long		nfoff;
	long		ns;
	double		tdiff;
	double		secs_in_year;

	tdiff = (double)( t[k].pulse_ext_start ) -
                (double)( t[klast].pulse_ext_start );

   /* if time jump is close to one year +/- one day, resegment */
	if ( fabs( tdiff + SECS_IN_YEAR ) < 100000 )
	{
		ns = nsamp - t[k].nsamp;
		nfoff = foff - t[k].nsamp * s->samp_size;
		form_segment(s, k, 1, t[k].sys_start, t[klast].sys_end,
			t[k].rec_num, t[klast].rec_num, nfoff, ns, t[k].year, das_stream);
		if (print_summary) printf("%11d\n  %7d   new year  %10.10s%6d",
                    klast,nsegs,words[newconfig],k);
		return(TRUE);
	}
	return(FALSE);
	
}

#define   max(a,b)    ((a) > (b) ? (a) : (b))
#define   min(a,b)    ((a) < (b) ? (a) : (b))

int
check_external( t, k, tps, s, foff, nsamp, das_stream ) 
	IDATIME	*t;
	int	k;
	float	tps;
	STREAMS	*s;
	long	foff, nsamp;
	int	das_stream;
{
	int		i, k0;
	int		iret = FALSE;
	static int	np = 0;
	static double	x1, x2, y1, y2;
	static double	z, z0, zlast;
	double		z_test[100];
	double		dz = 0.0;
	double		zlimit;
	long		nfoff;
	long		ns;

	if ( t[k].qf_start != 1 ) return(iret);
	x2 = (double)( t[k].pulse_sys_start ) / clock_factor;
	y2 = (double)( t[k].pulse_ext_start );
	z = (y2 - y1) - (x2 - x1);

	if ( x1 == x2 ) return(iret);
	 
	if ( np == 1 )
	{
		z0 = z;
		zlast = z;
	}

	/* if ( fabs(z-z0) > 5.0 )  try changing this - JPD */
	if ( fabs(z-zlast) > 5.0 )
	{
		x1 = x2;
		y1 = y2;
		zlast = z;
		return(iret);
	}
 
  /* test for offset jumps */

	zlimit = 1.0;
	if ( clock_factor >= 200. && clock_factor < 1000.) zlimit = 0.5;

	if ( fabs(z-zlast) > zlimit )
	{
	   /* build up test set to check consistency */

		for( i=0, k0=max(k-49,1); i<100 && t[k0].pulse_ext_start > 0; i++, k0++)
		{
			z_test[i] = (double)( t[k0].pulse_ext_start ) -
				(double)( t[k0].pulse_sys_start ) / clock_factor;
		}
		k0 = i-2;
		for( i=0; i<k0; i++)
		{
			dz += z_test[i+1] - z_test[i];
		}
	}


	if ( (fabs(dz) > 1.0) && (! connect_segment) ) 
	{
		ns = nsamp - t[k].nsamp;
		nfoff = foff - t[k].nsamp * s->samp_size;
		form_segment(s, k, fit_ext_jumps, t[k].sys_start, t[klast].sys_end,
			t[k].rec_num, t[klast].rec_num, nfoff, ns, t[k].year, das_stream);
		iret = TRUE;
		if (print_summary) printf("%11d\n  %7d   ext jump  %10.10s%6d",
                    klast,nsegs,words[newconfig],k);
	}
 
	x1 = x2;
	y1 = y2;
 
	zlast = z;
	np++;

	return(iret);
}

int
form_segment(s, nend, reboot_flag, tstart, tend, m_start, m_end, foff, nsamp, year, das_stream)
	STREAMS	*s;
	int	nend;
	int	reboot_flag;
	double	tstart, tend;
	long	m_start, m_end;
	long	foff, nsamp;
	short	year;
	int	das_stream;
{
	int		ns, i;
	extern void	memhandler();

    /* case 0: new configuration, old stream -- no initialization required */
    /* THIS CASE NOT CONSIDERED IN OLD CODE */
	if ( nend < 0 )
	{
		segs[ns].new_config = TRUE;
		return(0);
	}

    /* case 1: initialize for a new stream */
	if ( nend == 0 )
	{
		nsegs++;
		memhandler("segs", nsegs);
		ns = nsegs - 1;
		memcpy(&segs[ns], &seg_null, sizeof(SEGMENTS));
		memcpy(&segs[ns].stream, s, sizeof(STREAMS) );
		segs[ns].n_start = 0;
		segs[ns].m_start = m_start;
		segs[ns].start = tstart;
		segs[ns].new_config = TRUE;
		segs[ns].foff = foff ;
		segs[ns].nsamp = nsamp ;
		segs[ns].year = (long)year ;
		segs[ns].das_flag = das_stream ;

		return(0);
	}

	ns = nsegs - 1;

    /* case 2: this really should not form new segment */

	if ( segs[ns].n_end == -1 && segs[ns].n_start == nend-1 )
	{
		segs[ns].n_start++;
		segs[ns].start = tstart;
		segs[ns].m_start = m_start;
		segs[ns].foff = foff ;
		return(1);
	}

  /* case 3: close out the preceding segment and start a new one */

	segs[ns].n_end = klast;
	segs[ns].m_end = m_end;
	segs[ns].end = tend;
	segs[ns].nsamp = nsamp ;

	nsegs++;
	memhandler("segs", nsegs);
	ns = nsegs - 1;
	memcpy(&segs[ns], &seg_null, sizeof(SEGMENTS));

	memcpy(&segs[ns].stream, s, sizeof(STREAMS) );
	segs[ns].n_start = nend;
	segs[ns].m_start = m_start;
	segs[ns].start = tstart;
	segs[ns].reboot = reboot_flag;
	segs[ns].foff = foff ;
	segs[ns].year = (long)year ;
	segs[ns].das_flag = das_stream ;

	return(0);
}


#define LL	64
#define CR 0x0d
#define LF 0x0a

int
printLrecords()
{
	int	i, nrec;
	short	stemp;
	long	ext_time;
	char	str[LL];

	rewind(fplogsfile);
	printf("\nLog record synopsis:  (time quality change messages suppressed)\n\n");
	printf(" REC #  UNIT MESSAGE\n");
	printf("------- ---- ------------------------------------\n");

	while ( fread(buffer, 1, IRIS_BUFSIZ, fplogsfile) )
	{
		if ( feof(fplogsfile) ) return (0);

		memcpy(&nrec, buffer + 26, 4);
		memcpy(&stemp, buffer + UNITID_OFFSET, 2);
		memcpy(str, buffer + 34, LL-1);
		str[LL-1] = '\0';

		if ( strstr(str, "time quality change") == NULL )
		{
			for(i=0; i<LL; i++) /* strip junk off */
			{
				if ( str[i] == '\n' ) str[i] = ' ';
				if ( str[i] == CR   ) str[i] = ' ';
				if ( str[i] == LF   ) str[i] = ' ';
			}

			if ( stemp )
			{
				printf("%7d DAS  %s\n", nrec, str);
			}
			else
			{
				printf("%7d ARS  %s\n", nrec, str);
			}
		}
	}
}

#include "wfdisc30io.h"

void
make_CSS_files(leap_second)
	double	leap_second;
{
	int	i;
	int	n_written = 0;
	char	filename[64];
	FILE	*fp;
	FILE	*fp_aux;
	SEGMENTS	*s;

  /* write wfdisc files */

	sprintf(filename, "%s_aux.wfdisc", prefix);
	fp_aux = fopen(filename,"wt");
	if (fp_aux == NULL )
	{
		fprintf(stderr, "Could not open wfdisc file. Exiting.\n");
		finish_up(-1);
	}
	sprintf(filename, "%s.wfdisc", prefix);
	fp = fopen(filename,"wt");
	if (fp == NULL )
	{
		fprintf(stderr, "Could not open wfdisc file. Exiting.\n");
		finish_up(-1);
	}
	for (i=0, s=&segs[0] ; i<nsegs; i++, s++)
	{
		n_written += write_wfdisc_record(fp, fp_aux, s, leap_second);
	}
	fclose(fp);
	fclose(fp_aux);

  /* check result */
	if ( !n_written )
	{
		unlink(filename);
		fprintf(stderr, "dcctvol: No wfdisc records written. Exiting.\n");
		finish_up(-1);
	}

  /* write corresponding remark files */

	sprintf(filename, "%s_aux.remark", prefix);
	fp_aux = fopen(filename,"wt");
	if (fp_aux == NULL )
	{
		fprintf(stderr, "Could not open remark file. Exiting.\n");
		finish_up(-1);
	}
	fprintf(fp_aux, "%8ld %8ld %-80.80s %-17.17s\n", 1, 1, "QF=00", "1996180");
	fclose(fp_aux);

	sprintf(filename, "%s.remark", prefix);
	fp = fopen(filename,"wt");
	if (fp == NULL )
	{
		fprintf(stderr, "Could not open remark file. Exiting.\n");
		finish_up(-1);
	}
	fprintf(fp, "%8ld %8ld %-80.80s %-17.17s\n", 1, 1, "QF=00", "1996180");
	fclose(fp);
}

/*
 * write_wfdisc_record writes output to wfdisc file
 */

int
write_wfdisc_record(fpwfdisc, fpwfdisc_aux, s, leap_second)
	FILE		*fpwfdisc;
	FILE		*fpwfdisc_aux;
	SEGMENTS	*s;
	double		leap_second;
{
	int		i, n, yr, ndays;
	long		nsamp, jdate, foff;
	static int	first_iteration = TRUE;
	extern int	check_logger_type();
	double		time, endtime;
	double		srate;
	double		leap;
	struct wfdisc	wf30;
	static int	wfid = 1;
	extern void	day_split();

  /* if this segment is untimed, return */
/* xxxx
	if (  fabs(s->intercept) < 1e-4  &&  fabs(s->drift) < 1e-10 )
	{
		return(0);
	}
*/

  /* initialize wfdisc structure */
	memcpy(&wf30, &wfdisc_null, sizeof(struct wfdisc));

  /* fill in elements */

	strcpy(wf30.sta,	stat_name);
	strcpy(wf30.chan,	s->stream.chan) ;
	strcpy(wf30.datatype,	s->stream.datatype);
	strcpy(wf30.dir,	".");
	strcpy(wf30.segtype,	"o");

	sprintf(wf30.dfile, "%s-%02d.w", prefix, s->str_number);
	sprintf(wf30.lddate, "%ld", todaysdate() );

	wf30.nsamp = s->nsamp ;
	wf30.wfid = wfid++ ;
	wf30.foff = s->foff ;
	wf30.commid = 1;
  /* take care to preserve precision here */
	srate = s->stream.samprate ;
	srate -= s->stream.samprate * s->drift ;
	wf30.samprate = (float)srate;

   /* calculate time functions */

	time = calc_time( s->start, s->drift, s->intercept, s->clock_factor );
	wf30.jdate = s->year * 1000L + (int)(time / 86400.) ;
	time -= 86400.; /* time had been in secs since Jan 0, not Jan 1 */
	if ( time < 0.0 ) /* whoops - falls back over year boundary! */
	{
	        yr = s->year - 1;
		wf30.jdate = yr*1000L + (long)(dysize(yr));
	}

	yr = s->year;
	for (i = 1970, ndays=0; i < yr; i++) ndays += dysize(i);
	time += (double)ndays * 86400.;
	wf30.time = time ;
	wf30.endtime = time + (double)(wf30.nsamp - 1) / srate;

  /* store these times for possible insertion into Antelope tape_status table */

	if ( !strncmp("bh",s->stream.chan,2) || !strncmp("sl",s->stream.chan,2) ||
	     !strncmp("sh",s->stream.chan,2) || !strncmp("eh",s->stream.chan,2))
	{
		if ( sb_time < 0.0 ) sb_time = wf30.time;
		if ( wf30.time < sb_time ) sb_time = wf30.time;
		if ( sb_endtime < wf30.endtime ) sb_endtime = wf30.endtime;
	}

  /* set logger type */
	if ( antelope && first_iteration )
	{
		if ( check_logger_type(wf30.time) )
		{
			fprintf(stderr, "Data logger type unknown. Exiting.\n");
			finish_up(1);
		}
	}

	strcpy(wf30.instype,"IDAMK7");
	if ( logger_type == MK6B ) strcpy(wf30.instype,"IDAMK6");
	if ( logger_type == MK7A || logger_type == MK7B || logger_type == MK7C )
	{
		strcpy(wf30.instype,"IDAMK7");
	}
	if ( logger_type == MK8 )
	{
		strcpy(wf30.instype,"IDAMK8");
		sprintf(wf30.dfile, "%s-%s.w", prefix, s->stream.chan);
	}
	if ( logger_type == Q330 )
	{
		strcpy(wf30.instype,"Q330");
		sprintf(wf30.dfile, "%s-%s.w", prefix, s->stream.chan);
	}
	if ( logger_type == FEMTO )
	{
		strcpy(wf30.instype,"FEMTO");
		sprintf(wf30.dfile, "%s-%s.w", prefix, s->stream.chan);
	}
  /*
   * check if these times fall over leap second introduction:
   * if so, process this in a special fashion and then return
   */
	if ( leap_second >= wf30.time  && leap_second <= wf30.endtime  )
	{
		printf("Leap second found for %s\n", wf30.chan);

	  /* hold key wfdisc values that will change across leap second */

		time = wf30.time;
		endtime = wf30.endtime;
		foff = wf30.foff;
		nsamp = wf30.nsamp;
		jdate = wf30.jdate;

	   /* compute how many points to leap second */

	    /* note addition of 1.0 is for POSITIVE leap seconds (as all have
	     * been since 1972.  If leap is negative, we must reconsider here...
	     */
		leap = +1.0;
		n = 1 + (int)( (leap_second - time + leap) * srate );

	   /* correct record leading up to leap second */

		wf30.nsamp = n;
		wf30.endtime = time + (double)(wf30.nsamp - 1) / srate;
		print_wd_double(wf30, srate, fpwfdisc_aux);
		day_split( wf30, srate, leap_second, fpwfdisc );

	   /* correct record following leap second */

		wf30.nsamp = nsamp - n;
		wf30.time = time - leap + (double)n / srate;
		wf30.foff += ( !strcmp(wf30.datatype, "s2") ) ? 2 * n : 4 * n;
		wf30.endtime = wf30.time + (double)(wf30.nsamp - 1) / srate;
	     /*
	      * leap seconds are only introduced at end of quarters of years
	      * so only problem in incrementing jdate is at year boundary
	      */
		if ( (wf30.jdate - (yr*1000L)) > 360 )
		{
			wf30.jdate = (yr+1)*1000L + 1;
		}
		else
		{
			wf30.jdate += 1;
		}

		print_wd_double(wf30, srate, fpwfdisc_aux);
		day_split( wf30, srate, leap_second, fpwfdisc );

		first_iteration = FALSE;
		return(1);
	}

  /* output */
	print_wd_double(wf30, srate, fpwfdisc_aux);

  /* output a second wfdisc split by day */
	day_split( wf30, srate, leap_second, fpwfdisc );

	first_iteration = FALSE;

	return(1);
}

int
print_wd_double(wf30, sample_rate, fp)
	struct wfdisc *wf30;
	double	sample_rate;
	FILE *fp;
{

    int bytes;

    bytes = fprintf(fp, WFDISC_WCS2,
        wf30->sta, wf30->chan, wf30->time, wf30->wfid, wf30->chanid,
        wf30->jdate, wf30->endtime, wf30->nsamp, sample_rate,
        wf30->calib, wf30->calper, wf30->instype, wf30->segtype,
        wf30->datatype, wf30->clip, wf30->dir, wf30->dfile,
        wf30->foff, wf30->commid, wf30->lddate );

    if (bytes <= 0)
      return(FALSE);
    else
      return(TRUE);
}

/* close tmpfiles and exit gracefully */

void
finish_up(code)
	int	code;
{
	int		i, j;
	IDACONFIG       *p;

	for (i=0; i<MAXSTRM; i++)
	{
		for (j=0, p=&idaconfig[0]; j<n_configs; j++, p++)
		{
			if ( p->strms[i].fp != NULL )
			{
				fclose(p->strms[i].fp);
				unlink(p->strms[i].filename);
				break;
			}
		}
	}

	exit(code);
}

double
check_leap( year )
	long	year;
{
	int	yr, dy;
	long	jdate;
	char	*getenv();
	char	fname[MAXPATHLEN];
	char	line[128];
	double	null_result = -999.0;
	double	time;
	extern double	dtoepoch();
	FILE	*fp;

   /* first, open and read the leap second table file if possible */

	if ( (getenv("IRISHOME")) == NULL )
	{
		return( null_result );
	}
	sprintf(fname, "%s/include/leapsec.table", getenv("IRISHOME"));
	fp = fopen(fname,"rt");
	if (fp == NULL )
	{
		fprintf(stderr, "Could not open leap second table. Going on...\n");
		return( null_result );
	}

	while ( (fgets(line, 127, fp)) != NULL )
	{
		if ( line[0] == '#' ) continue;
		sscanf(line, "%d %d", &yr, &dy);

	  /* test for correct year */
		if ( year == (long)yr )
		{
			fclose(fp);

		  /* calculate the double precision value for leap second */
			jdate = yr * 1000L + dy;
			time = dtoepoch(jdate) + 86400.;
			return( time );
		}
	}

	fclose(fp);
	return( null_result );
}

/* Revision History
 *
 * $Log: main.c,v $
 * Revision 1.1  2011/03/17 20:23:21  dechavez
 * initial release, based on DCC time_vol sources
 *
 */
