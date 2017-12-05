#pragma ident "$Id: get_strmap.c,v 1.1 2011/03/17 20:23:20 dechavez Exp $"
/***********************************************************************
 *
 * get_strmap.c       
 *
 * This routine will be used to get the stream->channel map from Sybase.
 *
  ***********************************************************************/
#include "globals.h"

struct configr  *chconfig = NULL;

extern void	memhandler();
 
int
get_strmap(sta)
	char	*sta;
{
	int	i, j, status, n_channels ;
	char	where_cls[MAXSQLCMD];
	char	str[80];
	IDACONFIG	*p;

  /* frame query for Sybase */

	strcpy(where_cls, "where configid=(select configid from conmap where ");
	sprintf(str,"sta ='%s' and endtime > 1262303999.) ", sta);
		strcat(where_cls, str);
	sprintf(str,"and stream > -1 order by stream");
		strcat(where_cls, str);

  /* execute query and load into array chconfig */

	status = db_get(NULL, where_cls, CONFIG, &chconfig, &n_channels);
 
	if (status == FALSE)
	{
		fprintf(stderr, "db_get error return.\n");
		exit(1);
	}

	if ( n_channels < 1 )
	{
		fprintf(stderr, "Sybase configuration not found for sta %s\n", sta);
		exit(1);
	}

   /* create the idaconfig array and load it; note here that
    * there is an implicit assumption that when using Sybase info,
    * that info will be loaded into the first config structure record
    */
	memhandler("idaconfig", n_configs+1);

	p = &idaconfig[n_configs] ;
	memcpy(p, &idaconfignull, sizeof(IDACONFIG));
	memcpy(p->counts, &cnt_null, sizeof(COUNTERS));
	idaconfig[n_configs].recbeg = 1;
	idaconfig[n_configs].nactive = n_channels;
	for ( i=0; i<MAXSTRM; i++ ) /* null out new streams */
	{
		memcpy(&p->strms[i], &str_null, sizeof(STREAMS));
	}
	for ( i=0; i<n_channels; i++ )
	{
		j = chconfig[i].stream;
		strcpy(p->strms[j].chan, chconfig[i].chan);
		p->strms[j].inchan   = (short)chconfig[i].inchan;
		p->strms[j].mode     = (short)chconfig[i].mode;
		p->strms[j].nfilter  = atoi(chconfig[i].nfilter);
		p->strms[j].samprate = (double)chconfig[i].samprate;
	}

	n_configs++;

	for (i=n_channels-1;i<=0;i--)
	{
		free(&chconfig[i]);
	}

	return(0);
}

/* Revision History
 *
 * $Log: get_strmap.c,v $
 * Revision 1.1  2011/03/17 20:23:20  dechavez
 * initial release, based on DCC time_vol sources
 *
 */
