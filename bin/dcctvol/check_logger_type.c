#pragma ident "$Id: check_logger_type.c,v 1.1 2011/03/17 20:23:19 dechavez Exp $"
#include "globals.h"
#include "db.h"

int
check_logger_type(time)
	double	time;
{
	int	n_recs;
	int	status;
	char	str[128];
	char	lgtype[8];
	Dbptr   db, dbconfig ;

  /* frame query for Antelope and run it */

	 /* open Antelope database */

        if ( dbopen ( prefix, "r", &db ) != 0 )
        {
		sprintf(str, "ANTELOPE problem: cannot open table %s.", prefix);
                die ( 0, str ) ;
        }

  /* frame query and set up joins */

        db = dblookup ( db, 0, "conmap", 0, 0 ) ;
        sprintf(str, "sta =='%s' && time <= %lf && endtime >= %lf",
		stat_name, time, time);
        db = dbsubset ( db, str, 0 );
        dbconfig = dblookup ( db, 0, "config", 0, 0 ) ;
        db = dbjoin ( db, dbconfig, 0, 0, 0, 0, 0 ) ;

  /* execute query */

        dbquery ( db, dbRECORD_COUNT, &n_recs ) ;
	db.record = 0 ;
	status = dbgetv ( db, 0,
		"lgtype", lgtype,
		0 ) ;
	dbclose(db);

	if (status != FALSE)
	{
		fprintf(stderr, "db_getv config query error return.\n");
		exit(1);
	}
	
	if ( !strcmp( lgtype, "MK6b"))
	{
		logger_type = MK6B;
		return(0);
	}
	if ( !strcmp( lgtype, "MK7a"))
	{
		logger_type = MK7A;
		return(0);
	}
	if ( !strcmp( lgtype, "MK7b"))
	{
		logger_type = MK7B;
		return(0);
	}
	if ( !strcmp( lgtype, "MK7c"))
	{
		logger_type = MK7C;
		return(0);
	}
	if ( !strcmp( lgtype, "MK8"))
	{
		logger_type = MK8;
		return(0);
	}
	if ( !strcmp( lgtype, "Q330"))
	{
		logger_type = Q330;
		return(0);
	}
	if ( !strcmp( lgtype, "FEMTO"))
	{
		logger_type = FEMTO;
		return(0);
	}

	return(1);
}

/* Revision History
 *
 * $Log: check_logger_type.c,v $
 * Revision 1.1  2011/03/17 20:23:19  dechavez
 * initial release, based on DCC time_vol sources
 *
 */
