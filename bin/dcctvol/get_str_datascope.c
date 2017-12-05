#pragma ident "$Id: get_str_datascope.c,v 1.1 2011/03/17 20:23:20 dechavez Exp $"
/***********************************************************************
 *
 * get_str_datascope.c       
 *
 * This routine will be used to get the stream->channel map from Antelope.
 *
  ***********************************************************************/
#include "globals.h"
#include "db.h"

extern void	memhandler();
 
int
get_str_datascope(sta, prefix)
	char	*sta;
	char	*prefix;
{
	int	i, j, n_channels ;
	long	lstream, linchan, lmode;
	double	lsamprate;
	char	lchan[9], lnfilter[4];
	char	str[80];
	Dbptr	db, dbconmap, dbconfig ; 
	Tbl	*sortkeys ;
	IDACONFIG	*p;


  /* open Antelope database, named prefix by convention */

	if ( dbopen ( prefix, "r", &db ) != 0 )
	{
		sprintf(str, "ANTELOPE problem: cannot open table %s.", prefix);
		die ( 0, str ) ;
	}

  /* frame query and set up joins */

	db = dblookup ( db, 0, "tape_login", 0, 0 ) ;
	sprintf(str, "sta =='%s' && idaid == %s", sta, prefix+6);
	db = dbsubset ( db, str, 0 );
	dbconmap = dblookup ( db, 0, "conmap", 0, 0 ) ;
	db = dbjoin ( db, dbconmap, 0, 0, 0, 0, 0 ) ;
	dbconfig = dblookup ( db, 0, "config", 0, 0 ) ;
	db = dbjoin ( db, dbconfig, 0, 0, 0, 0, 0 ) ;
	sortkeys = strtbl("config.stream", 0 ) ;
	db = dbsort ( db, sortkeys, 0, 0 ) ;

  /* execute query and load into array chconfig */

	dbquery ( db, dbRECORD_COUNT, &n_channels ) ;
	
	if ( n_channels < 1 )
	{
		fprintf(stderr, "Antelope configuration not found for sta %s\n", sta);
		exit(1);
	}

   /* create the idaconfig array and load it; note here that
    * there is an implicit assumption that when using Antelope info,
    * that info will be loaded into the first config structure record
    */
	memhandler("idaconfig", n_configs+1);

	p = &idaconfig[n_configs] ;
	memcpy(p, &idaconfignull, sizeof(IDACONFIG));
	memcpy(&p->counts, &cnt_null, sizeof(COUNTERS));

	idaconfig[n_configs].recbeg = 1;
	idaconfig[n_configs].nactive = n_channels;
	for ( i=0; i<MAXSTRM; i++ ) /* null out new streams */
	{
		memcpy(&p->strms[i], &str_null, sizeof(STREAMS));
	}
	for ( db.record = 0 ; db.record < n_channels ; db.record++ )
	{
		dbgetv ( db, 0,
			"chan", lchan,
			"stream", &lstream,
			"inchan", &linchan,
			"nfilter", &lnfilter,
			"mode", &lmode,
			"samprate", &lsamprate,
			0 ) ;

		j = lstream;
		strcpy(p->strms[j].chan, lchan);
		p->strms[j].inchan   = (short)linchan;
		p->strms[j].mode     = (short)lmode;
		p->strms[j].nfilter  = atoi(lnfilter);
		p->strms[j].samprate = (double)lsamprate;
	}

	n_configs++;
	dbclose(db);

	return(0);
}

/* Revision History
 *
 * $Log: get_str_datascope.c,v $
 * Revision 1.1  2011/03/17 20:23:20  dechavez
 * initial release, based on DCC time_vol sources
 *
 */
