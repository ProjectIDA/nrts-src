#pragma ident "$Id: tape_status.c,v 1.1 2011/03/17 20:23:21 dechavez Exp $"
/***********************************************************************
 *
 * tape_status.c       
 *
 * This routine will be used to get / put information in the appropriate
 * tape_status record of Antelope.
 *
  ***********************************************************************/
#include "globals.h"
#include "db.h"


short
check_year(year_tape)
	short	year_tape;
{
	int	n_recs;
	char	str[128];
	double	tape_time;
	short	ant_year;
	Dbptr	db ;


  /* if antelope flag is not set, just check if the year is set
   * on the command line (in variable set_year); if not return
   * the value passed down from the tape record */

	if ( !antelope )
	{
		if ( set_year >= 0 )
		{
			printf("******************************************************\n");
			printf("******************************************************\n");
			printf("*** Using command line entry to set year to %d\n", set_year);
			printf("******************************************************\n");
			printf("******************************************************\n");
			return( (short)set_year );
		}
		return( year_tape );
	}	

  /*
   * if here, check Antelope for consistency ....
   */
	/* open Antelope database */

        if ( dbopen ( prefix, "r", &db ) < 0 )
        {
		sprintf(str, "ANTELOPE problem: cannot open table %s.", prefix);
		die ( 0, str ) ;
        }

  /* frame query for Antelope and run it */

	db = dblookup ( db, 0, "tape_login", 0, 0 ) ;
        sprintf(str, "sta =='%s' && idaid == %s", stat_name, prefix+6);
        db = dbsubset ( db, str, 0 );
	dbquery ( db, dbRECORD_COUNT, &n_recs ) ;

	if ( n_recs < 1 )
	{
		fprintf(stderr, "tape_login query returned 0 records. exiting\n");
		dbclose(db);
		finish_up(-1);
	}
	if ( n_recs > 1 )
	{
		fprintf(stderr, "Antelope had %d records for sta %s tape %s\n",
			stat_name, prefix+6);
		dbclose(db);
		finish_up(1);
	}
	db.record = 0 ;
	dbgetv ( db, 0, "time", &tape_time, 0);
	dbclose(db);
	

   /* establish which year this represents */

	ant_year = find_antyear( tape_time );

	if ( ant_year < 0 )
	{
		fprintf(stderr, "tape_login information incomplete.  exiting.\n");
		finish_up(-1);
	}

   /* check consistency */

	if ( set_year >= 0 )
	{
		if ( year_tape != ant_year || year_tape != set_year )
		{
			printf("******************************************************\n");
			printf("******************************************************\n");
			printf("*** The following inconsistency has been noted:\n***\n");
			printf("***     from field volume data record: year = %d\n", year_tape);
			printf("***     from tape_login record:        year = %d\n", ant_year);
			printf("***     from command line:             year = %d\n", set_year);
			printf("***\n");
                        printf("*** Using command line entry to set year to %d\n", set_year);
			printf("***\n");
			printf("******************************************************\n");
			printf("******************************************************\n");

			return( (short)set_year );
		}
	}

	if ( year_tape != ant_year )
	{
		printf("********************************************************\n");
		printf("********************************************************\n");
		printf("*** The following inconsistency has been noted:\n***\n");
		printf("***     from field volume data record: year = %d\n", year_tape);
		printf("***     from tape_login record:        year = %d\n", ant_year);
		printf("***\n");
		printf("*** These data will be processed using the year from the\n");
		printf("*** tape_login record.  If this is incorrect, either\n");
		printf("*** correct the Antelope tape_login record OR use the\n");
		printf("*** year=correct_year cmd line option on dcctvol.\n");
		printf("********************************************************\n");
		printf("********************************************************\n");

		return( ant_year );
	}

	return( year_tape );
}

int
find_antyear(ttime)
	double	ttime;
{
	if ( ttime <  567993600.0 ) return (-1);
	if ( ttime <  599616000.0 ) return (1988);
	if ( ttime <  631152000.0 ) return (1989);
	if ( ttime <  662688000.0 ) return (1990);
	if ( ttime <  694224000.0 ) return (1991);
	if ( ttime <  725846400.0 ) return (1992);
	if ( ttime <  757382400.0 ) return (1993);
	if ( ttime <  788918400.0 ) return (1994);
	if ( ttime <  820454400.0 ) return (1995);
	if ( ttime <  852076800.0 ) return (1996);
	if ( ttime <  883612800.0 ) return (1997);
	if ( ttime <  915148800.0 ) return (1998);
	if ( ttime <  946684800.0 ) return (1999);
	if ( ttime <  978307200.0 ) return (2000);
	if ( ttime < 1009843200.0 ) return (2001);
	if ( ttime < 1041379200.0 ) return (2002);
	if ( ttime < 1072915200.0 ) return (2003);
	if ( ttime < 1104537600.0 ) return (2004);
	if ( ttime < 1136073600.0 ) return (2005);
	if ( ttime < 1167609600.0 ) return (2006);
	if ( ttime < 1199145600.0 ) return (2007);
	if ( ttime < 1230768000.0 ) return (2008);
	if ( ttime < 1262304000.0 ) return (2009);
	if ( ttime < 1293840000.0 ) return (2010);
	if ( ttime < 1325376000.0 ) return (2011);
	if ( ttime < 1356998400.0 ) return (2012);
	if ( ttime < 1388534400.0 ) return (2013);
	if ( ttime < 1420070400.0 ) return (2014);
	if ( ttime < 1451606400.0 ) return (2015);
	return(-1);
}

void
check_drifts()
{
	int	i;
	int	idaid, lidaid;
	int	n_recs;
	double	ltime, lendtime;
	double	ldasdrft, ldasint, larsdrft, larsint;
	char	str[128];
	char	st1[64], st2[64];
	char	*tstr();
	Tbl     *sortkeys ;
	Dbptr   db ;

	struct tape_status  *tpstatus = NULL;

	/* open Antelope database */

        if ( dbopen ( prefix, "r", &db ) < 0 )
        {
		sprintf(str, "ANTELOPE problem: cannot open table %s.", prefix);
		die ( 0, str ) ;
        }
	
  /* frame query for Antelope and run it */

	idaid = atoi(prefix+6);
	db = dblookup ( db, 0, "tape_status", 0, 0 ) ;
	sprintf(str, "sta =='%s' && idaid >= %d && idaid <= %d",
		stat_name, idaid-3, idaid+3);
        db = dbsubset ( db, str, 0 );
	sortkeys = strtbl("tape_status.idaid", 0 ) ;
	db = dbsort ( db, sortkeys, 0, 0 ) ;

	dbquery ( db, dbRECORD_COUNT, &n_recs ) ;

	if (n_recs < 0)
	{
		dbclose(db);
		fprintf(stderr, "db_get error return.\n");
		finish_up(1);
	}

  /* print out results */

	printf("\nDrift, intercept of other field volumes from %s:\n\n",
		stat_name);
	printf("   tape          coverage                  DAS                      ARS\n");
	printf("          start          end           drift     intercept      drift      intercept\n");
	printf("   ----  ------------- ------------- --------- -------------   --------- -------------\n");

	for ( db.record = 0 ; db.record < n_recs ; db.record++ )
	{
		dbgetv ( db, 0,
                        "idaid", &lidaid,
                        "time", &ltime,
                        "endtime", &lendtime,
                        "dasdrft", &ldasdrft,
                        "dasint", &ldasint,
                        "arsdrft", &larsdrft,
                        "arsint", &larsint,
                        0 ) ;

		if ( lidaid == idaid )
		{
			printf("\n");
			continue;
		}
		strcpy(st1, tstr(ltime) );
		strcpy(st2, tstr(lendtime) );
		printf("    %03d  %s %s %9.2le %13.3lf   %9.2le %13.3lf\n",
			lidaid, st1, st2, ldasdrft, ldasint, larsdrft, larsint);
	}
	printf("\n");
	dbclose(db);
}

#include "pfile.h"

char *tstr(t)
double t;
{
        intime_t it;
        static char string[12];
 
        strcpy(string,"-------------");
 
        /* convert to intermediate time */
        if (makeintt(t,&it))
 
        /* convert it to string */
        sprintf(string,"%04d%03d-%02d:%02d",it.tyr,it.tdoy,
          it.thr,it.tmin);
        return(string);
}

void
update_drift()
{
	int	idaid, lidaid;
	int	n_recs, lnumrec, status;
	short	ant_year;
	double	ltime, lendtime;
	double	ldasdrft, ldasint, larsdrft, larsint;
	char	str[128];
	Dbptr   db ;

	/* open Antelope database */

        if ( dbopen ( prefix, "r+", &db ) < 0 )
        {
		sprintf(str, "ANTELOPE problem: cannot open table %s.", prefix);
		die ( 0, str ) ;
        }
	
  /* frame query for Antelope and run it */

	idaid = atoi(prefix+6);
	db = dblookup ( db, 0, "tape_status", 0, 0 ) ;
	sprintf(str, "sta =='%s' && idaid == %d",
		stat_name, idaid);
        db = dbsubset ( db, str, 0 );
	dbquery ( db, dbRECORD_COUNT, &n_recs ) ;

	if ( n_recs < 0 )
	{
		fprintf(stderr, "Antelope error return.\n");
		dbclose(db);
		finish_up(1);
	}

	if ( n_recs > 1 )
	{
		fprintf(stderr, "Antelope had %d records for sta %s tape %d\n",
			stat_name, idaid);
		dbclose(db);
		finish_up(1);
	}

   /* reset values to appropriate numbers */

	db.record = 0 ;
	dbgetv ( db, 0,
		"idaid", &lidaid,
		"time", &ltime,
		"endtime", &lendtime,
		"dasdrft", &ldasdrft,
		"dasint", &ldasint,
		"arsdrft", &larsdrft,
		"arsint", &larsint,
		"numrec", &lnumrec,
		0 ) ;

	if ( n_dassegs > 0 )
	{
		ldasdrft = das_segs[n_dassegs - 1].drift;
		ldasint  = das_segs[n_dassegs - 1].intercept;
		lendtime = sb_endtime;
		ltime = sb_time;
	}
	if ( n_arssegs > 0 )
	{
		larsdrft = ars_segs[n_arssegs-1].drift;
		larsint = ars_segs[n_arssegs-1].intercept;
	}
	lnumrec = cnt.rec;

   /* update the old record */

	db.record = 0 ;
	status = dbputv( db, 0,
		"idaid", lidaid,
		"time", ltime,
		"endtime", lendtime,
		"dasdrft", ldasdrft,
		"dasint", ldasint,
		"arsdrft", larsdrft,
		"arsint", larsint,
		0 ) ;

	if (status != FALSE)
	{
		fprintf(stderr, "*********************************************\n");
		fprintf(stderr, "*** ERROR - Unable to update Antelope table.\n");
		fprintf(stderr, "*** station %s, tape %d record.\n",
			stat_name, idaid);
		fprintf(stderr, "*********************************************\n");
	}

	dbclose(db);
        return;
}

/* Revision History
 *
 * $Log: tape_status.c,v $
 * Revision 1.1  2011/03/17 20:23:21  dechavez
 * initial release, based on DCC time_vol sources
 *
 */
