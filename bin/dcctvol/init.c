#pragma ident "$Id: init.c,v 1.1 2011/03/17 20:23:21 dechavez Exp $"
/*======================================================================
 *
 *  init.c
 *
 *  Initialization routine.
 *
 *====================================================================*/
#include "globals.h"
#include <time.h>
#include <sys/param.h>
#include <string.h>

extern char *VersionIdentString;

extern int	get_str_datascope();
extern int	get_Krecmap();

/*====================================================================*/
void init(int argc, char **argv)
{
	int	i, j, nc;
	char	fname[128];
	char	switches[32];
	char	stat_tmp[32];
	char	klist[128], rejlist[128], discard_list[1024], force_list[128];
	char	use_stream[6];
	void	set_keeplist(), setup_tmpfiles(), set_discardlist(), set_forcelist();
	void	memhandler();
	void	config_summary();
	void	check_config();
	char	*get_station_name();

	if (argc < 2) help();

  /*  Initialize several variables */

	memcpy(&cnt, &cnt_null, sizeof(struct counters));
	strcpy(klist, "");
	strcpy(rejlist, "");
	strcpy(force_drift_file, "");
	strcpy(use_stream, "");
	strcpy(discard_list, "");
	strcpy(force_list, "");
	strcpy(switches, "");
	strcpy(stat_tmp, "");
	memset(buffer, (unsigned char) 0, IRIS_BUFSIZ);
	for (i=0; i<MAXSEGS; i++)
	{
		memcpy(&tsegs[i],    &seg_null, sizeof(SEGMENTS));
		memcpy(&das_segs[i], &seg_null, sizeof(SEGMENTS));
		memcpy(&ars_segs[i], &seg_null, sizeof(SEGMENTS));
	}

  /*  Read command line arguments */
	setpar(argc, argv);
	getpar("keep",   "s", klist);
	getpar("reject", "s", rejlist);
	getpar("discard","s", discard_list);
	getpar("force",  "s", force_list);
	getpar("tol",    "d", &time_tolerance);
	getpar("use",    "s", use_stream);
	getpar("skip",   "d", &nskip);
	getpar("maxrec", "d", &n_maxrec);
	getpar("timcor", "F", &time_correction);
	getpar("timcoef", "s", force_drift_file);
	getpar("year",   "d", &set_year);
	getpar("station","s", stat_tmp);
	getpar("SWITCH", "s", &switches);
	endpar();

	strcpy(prefix, argv[1]);

  /* set flags depending upon switch settings */

	print_Lrecs = FALSE;
	print_Hrecs = FALSE;

	if ( strchr(switches, 'p') != NULL ) RunPlotxy   = TRUE;
	if ( strchr(switches, 'a') != NULL ) antelope    = TRUE;
	if ( strchr(switches, 'l') != NULL ) print_Lrecs = TRUE;
	if ( strchr(switches, 'h') != NULL ) print_Hrecs = TRUE;
	if ( strchr(switches, 'f') != NULL ) fit_ext_jumps = TRUE;
	if ( strchr(switches, 'c') != NULL ) connect_segment = TRUE;
	if ( strlen(force_drift_file) != 0 ) time_coef = TRUE;

  /* open headers file for processing */
	strcpy(fname, prefix); strcat(fname, ".hdr");
 
	if ((fpheaders = fopen(fname, "rb")) == NULL) {
		fprintf(stderr, "*** ERROR *** fopen: %s: %s\n", fname, strerror(errno));
		finish_up(1);
	}

  /* fish for channel name from common area of first record */
	if ( strlen(stat_tmp) > 0 )
	{
		strcpy(stat_name, stat_tmp);
	}
	else
	{
		strcpy(stat_name, get_station_name());
	}

  /*  Output to user important info */

	printf("====================");
	printf("DATA PROCESSING PASS #2 SUMMARY  ");
	printf("====================");
	printf("\ndcctvol [%s]\n\n", VersionIdentString);
	printf("arguments:\n");

	for (i = 1; i < argc; i++) printf("    %s\n", argv[i]); printf("\n");

  /*  Build mapping of streams to channels, then set stream map array */

	if ( antelope ) /* build the map with an Antelope query */
	{
		get_str_datascope(stat_name, prefix);
	}
	else /* build the map by reading K records */
	{
		get_Krecmap(stat_name);
	}

	if ( !n_configs && !antelope ) /* no configuration information */
	{
		fprintf(stderr, "*** No configuration (K) records present.  Rerun\n");
		fprintf(stderr, "*** dcctvol with -a flag to use information\n");
		fprintf(stderr, "*** about configuration held in ANTELOPE.\n");
		finish_up(-1);
	}

	if ( !n_configs )
	{
		fprintf(stderr, "*** No configuration information obtained from ANTELOPE\n");
		fprintf(stderr, "*** Exiting dcctvol.\n");
		finish_up(-1);
	}

  /* reconcile various indices */
	if ( n_configs > 1 )
	{
		for (i=n_configs-1; i>0; i--)
		{
			idaconfig[i-1].recend = idaconfig[i].recbeg - 1;
		}
	}
	idaconfig[0].recbeg = 1;  /* by fiat */

  /* parse keep/reject lists */
	set_keeplist(klist, rejlist);

  /* parse discard list */
	set_discardlist(discard_list);

  /* parse force list */
	set_forcelist(force_list);

  /* use for timing a different stream other than first one encountered? */
	if ( strlen(use_stream) > 0 )
	{
		if ( strlen(use_stream) < 2 )
		{
			stream2use = atoi(use_stream);
		}
		else /* map this name to a stream number */
		{
			for (i=0; i<MAXSTRM; i++)
			{
				if ( !strcasecmp( idaconfig[0].strms[i].chan, use_stream) )
				{
					stream2use = i;
					break;
				}
			}
			if ( i == MAXSTRM )
			{
				fprintf(stderr, "Could not find stream %s\n", use_stream);
				finish_up(-1);
			}
		}

		printf("\nUsing stream %s (%d) for timing purposes.\n\n",
			use_stream,stream2use);
	}



  /* print summary of configurations */
	config_summary(antelope);

  /* check the configuration for reasonable values */
	check_config();
 
  /* open files for output */
	setup_tmpfiles();
}

void
set_keeplist(klist, rejlist)
	char	*klist, *rejlist;
{
	int	i, j;
	int	nk = 0, nr = 0;
	IDACONFIG	*p;
  /*
   * default is set to keep channel, that is,
   */
	nk = strlen(klist);
	nr = strlen(rejlist);
	if ( nk ) util_lcase(klist);  /* if nk > 0, check keep list */
	if ( nr ) util_lcase(rejlist);  /* if nr > 0, check reject list */

  /* if lists are zero length, return */

	if ( !nk && !nr ) return;

	for (i=0; i<n_configs; i++)
	{
		p = &idaconfig[i];
		for (j=0; j<MAXSTRM; j++)
		{
			if ( nk && strstr(klist, util_lcase(p->strms[j].chan)) == NULL)
			{
				p->strms[j].keep = 0;
			}
			if ( nr && strstr(rejlist, util_lcase(p->strms[j].chan)) != NULL)
			{
				p->strms[j].keep = 0;
			}
		}
	}
}

#include <stddef.h>

void
set_discardlist(discard_list)
	char	*discard_list;
{
	int	i, j;
	long	k;
	char	*token, *ptr;
	char	pair[32];

	if ( !strlen(discard_list) ) return;

	discards = (long *)malloc(2 * sizeof(long) );
	if ( discards == NULL )
	{
		fprintf(stderr, "Could not malloc discard list.\n");
		exit(1);
	}

  /* parse this string and compute number of pairs required */

	token = strtok( discard_list, "," );
	while ( token != NULL )
	{
		n_discards++;
		i = 2 * n_discards - 2;
		j = i + 1;

		discards = (long *)realloc(discards, (i+2)* sizeof(long) );
		if ( discards == NULL )
		{
			fprintf(stderr, "Could not realloc discard list.\n");
			exit(1);
		}

		strcpy(pair, token);
		if ( (ptr = strchr(pair, '-')) == NULL ) /* this is a single number */
		{
			discards[i] = atoi(pair);
			discards[j] = atoi(pair);
		}
		else /* this is a pair */
		{
			strncpy(ptr, " ",1);
			sscanf(pair, "%ld", &k);      discards[i] = k;
			sscanf(pair, "%*ld %ld", &k); discards[j] = k;
		}
		token = strtok( NULL, "," );
	}
}

void
set_forcelist(force_list)
	char	*force_list;
{
	int	i, j;
	long	k;
	char	*token, *ptr;
	char	number[32];

	if ( !strlen(force_list) ) return;

	forcerecs = (long *)malloc( sizeof(long) );
	if ( forcerecs == NULL )
	{
		fprintf(stderr, "Could not malloc force record list.\n");
		exit(1);
	}

  /* parse this string and compute number */

	token = strtok( force_list, "," );
	while ( token != NULL )
	{
		n_forcerecs++;

		forcerecs = (long *)realloc(forcerecs, (n_forcerecs)* sizeof(long) );
		if ( forcerecs == NULL )
		{
			fprintf(stderr, "Could not realloc forced rec list.\n");
			exit(1);
		}

		strcpy(number, token);
		forcerecs[n_forcerecs-1] = atoi(number);
		token = strtok( NULL, "," );
	}
}

void
setup_tmpfiles()
{
	int		i, j, openit;
	IDACONFIG	*p;
	FILE		*fp, *mytmpfile();
	char		myfilename[64];

	for (i=0; i<MAXSTRM; i++)
	{
		openit = 0;
		fp = mytmpfile( &myfilename );

		for (j=0, p=&idaconfig[0]; j<n_configs; j++,p++)
		{
			if ( p->strms[i].inchan > -1 &&
			     p->strms[i].keep )
			{
				p->strms[i].fp = fp;
				strcpy(p->strms[i].filename, myfilename);
				openit = 1;
			}
		}
		if ( !openit )
		{
			fclose(fp);
			unlink(myfilename);
		}
	}
}

FILE *
mytmpfile(fname)
	char		*fname;
{
	char	*getenv();
	char	*tname;
	FILE	*fp;

	strcpy(fname, "./");
	if ( (getenv("IRIS_CSS_PATH")) != NULL )
	{
		sprintf(fname, "%s/%s", getenv("IRIS_CSS_PATH"), prefix);
	}

	tname = tempnam(fname, NULL);

	if ( tname == NULL )
	{
		fprintf(stderr, "problem setting up tmpfile name. exiting.\n");
		finish_up(1);
	}
	strcpy( fname, tname );

	fp = fopen(tname, "wb+");
	if ( fp == NULL )
	{
		fprintf(stderr, "problem opening tmpfile. exiting.\n");
		finish_up(1);
	}
	return(fp);
}

void
memhandler(type, n)
	char	*type;
	int	n;
{
  /* routine to perform a malloc/realloc of any of several structures
   * currently given global scope
   */
	int	i;
	int	nsz;

	if ( !strcmp(type, "idaconfig") )
	{
		nsz = sizeof(IDACONFIG) * n;
		if ( idaconfig == NULL )
	        {
			idaconfig = (IDACONFIG *)malloc(nsz);
		}
		else
		{
			idaconfig = (IDACONFIG *)realloc(idaconfig, nsz );
		}
 
		if ( idaconfig == NULL )
		{
			fprintf(stderr,
				"Could not malloc/realloc idaconfig struct.\n");
			finish_up(1);
		}
	}
	else if ( !strcmp(type, "segs") )
	{
		nsz = sizeof(SEGMENTS) * n;
		if ( segs == NULL )
	        {
			segs = (SEGMENTS *)malloc(nsz);
		}
		else
		{
			segs = (SEGMENTS *)realloc(segs, nsz );
		}
 
		if ( segs == NULL )
		{
			fprintf(stderr,
				"Could not malloc/realloc segs struct.\n");
			finish_up(1);
		}
	}
	else
	{
		fprintf(stderr, "memhandler: %s not recognized. Exiting.\n",
			type);
		finish_up(1);
	}
}
void
config_summary(antelope)
	int	antelope;
{
	void	config_summary_single();
	void	config_summary_general();

	if ( n_configs == 1 )
	{
		config_summary_single(antelope);
	}
	else
	{
		config_summary_general(antelope);
	}
}

void
config_summary_single(antelope)
	int	antelope;
{
	int	i ;
	IDACONFIG *p;

	printf("Configuration summary:\t single configuration based upon ");
	if ( antelope ) printf("Antelope\n\n");
	              else printf("K record\n\n");

	p = &idaconfig[0];
	printf(" stream    chan_name  samprate   channel  filter  mode\n");
	printf(" ------    ---------  --------   -------  ------  ----\n");
	for (i = 0; i < MAXSTRM; i++)
	{
	   if ( p->strms[i].samprate > 0.0 )
	   {
		printf("   %02d         %-5.5s    %6.1lf      %2d      %2d       %d",
			i, p->strms[i].chan, p->strms[i].samprate,
			p->strms[i].inchan, p->strms[i].nfilter,
			p->strms[i].mode);
	   }
	   else
	   {
		printf("   %02d         %-5.5s      ---       %2d      %2d       %d",
			i, p->strms[i].chan,
			p->strms[i].inchan, p->strms[i].nfilter,
			p->strms[i].mode);
	   }

	   if ( p->strms[i].inchan > -1 && !strcmp(p->strms[i].chan, "-") )
	   {
		printf("  ??\n");
	   }
	   else
	   {
		printf("\n");
	   }
	}
	printf("\n\n");
}

#define WIDLIM 4
void
config_summary_general(antelope)
	int	antelope;
{
	int	i, j, k ;
	IDACONFIG *p;

	k = ( n_configs < WIDLIM ) ? n_configs : WIDLIM;

	printf("Configuration summary:\t");
	printf("# of [unique] configurations = %d\n\n Basis:      ",n_configs);
	for (i = 0; i < k; i++)
	{
		if (!i && antelope ) printf("   Antelope   ");
		              else printf("         K        ");
	}
	printf("\n             ");
	for (i = 0; i < k; i++)  printf("beg     end       ");
	printf("\n Recs:  ");
	for (i = 0; i < k; i++)
	{
		printf("%8ld%8ld  ", idaconfig[i].recbeg, idaconfig[i].recend);
	}
	printf("\n Stream:");
	for (i = 0; i < MAXSTRM; i++)
	{
		printf("\n   %02d  ", i);
		for (j = 0; j < k; j++)
		{
			p = &idaconfig[j];
			
			if ( p->strms[i].inchan < 0 )
			{
				printf("       ---   ---- ");
			}
			else if ( p->strms[i].inchan > 0 &&
			     p->strms[i].samprate < 0.)
			{
				/* printf("       ???   ???? "); */
				printf("       [%d %d %d]?? ",
					p->strms[i].inchan,
					p->strms[i].nfilter,
					p->strms[i].mode);
			}
			else
			{
				printf("       %-3.3s %6.1lf ",
					p->strms[i].chan,
					p->strms[i].samprate);
			}
		}
	}
	printf("\n\n");
	if ( n_configs > WIDLIM )
	{
		printf("\t[Only first %d configurations listed.]\n\n",
			WIDLIM);
	}
}

/*
 * check configuration for internal consistency
 */

void
check_config()
{
	int		i, j, k;
	IDACONFIG	*p ;

	for (i=0; i<n_configs; i++)
	{
	   p = &idaconfig[i];
	   for ( j=0; j<MAXSTRM; j++)
	   {
		for ( k=j+1; k<MAXSTRM; k++)
		{
		   if ( strlen(p->strms[j].chan) > 0 &&
			strlen(p->strms[k].chan) > 0 &&
			!(strcmp(p->strms[j].chan, p->strms[k].chan)) &&
			(strcmp(p->strms[j].chan, str_null.chan)) )
		   {
			printf("\nWARNING: config %d - %s on streams %d and %d\n",
				i+1, p->strms[j].chan, j, k);
		   }

		   if ( p->strms[j].inchan > -1 &&
			p->strms[j].nfilter == p->strms[k].nfilter &&
			p->strms[j].inchan  == p->strms[k].inchan  &&
			p->strms[j].mode    == p->strms[k].mode )
		   {
			printf("\nWARNING: config %d - streams %d and %d settings match!\n",
				i+1, j, k);
		   }
		}
	   }
	}
	printf("\n\n");
}

char *
get_station_name()
{
	static char	statmp[9];
	short		iyr;

	fread(buffer, 1, 34, fpheaders);
	memcpy(&statmp, buffer + 26, 8);

  /*
   * handle the exception of NVS data logger being
   * moved to KURK in 1995.
   */
	if ( !strcmp(statmp, "NVS") )
	{
		memcpy(&iyr, buffer + 6, 2);	
		if ( iyr >= 1995 ) strcpy(statmp, "KURK"); 
	}
	if ( !strcmp(statmp, "NRI") )
	{
		strcpy(statmp, "NRIL");  /* gotcha! */
	}
	rewind(fpheaders);

	return(statmp);
}

/* Revision History
 *
 * $Log: init.c,v $
 * Revision 1.1  2011/03/17 20:23:21  dechavez
 * initial release, based on DCC time_vol sources
 *
 */
