#pragma ident "$Id: get_Krecmap.c,v 1.1 2011/03/17 20:23:20 dechavez Exp $"
/***********************************************************************
 *
 * get_Krecmap.c       
 *
 * This routine will be used to get the stream->channel map from K records.
 *
  ***********************************************************************/
#include "globals.h"

extern void memhandler();
 
int
get_Krecmap(sta)
	char	*sta;
{
	int	iloop = 1 ;
	int	set_streamchans();

   /* This code will not yet accommodate MK8 configuration records: Note below (JPD) */

   /* read through the headers and process all K (configuration) records */

   	while ( iloop )
	{
	   switch ( rdata(fpheaders) )
	   {
       	     case KREC: dokrec(buffer);  break;
       	     case AREC: break;
       	     case DREC: break;
       	     case CREC: break;
       	     case LREC: break;
       	     case EREC: break;
       	     case PREC: break;
       	     case HREC: break;
       	     case RREC: break;
       	     case TREC:
			fprintf(stderr,"dcctvol: processing MK8 data requires Antelope database.\n");
			exit(1);
	     case MYREC: break;

       	     case DONE: iloop = 0;       break;
	     case ERR_RET:
			fprintf(stderr,"dcctvol: incomplete read at rec %d\n",
				cnt.rec);
			exit(1);

	     default:   fprintf(stderr,"dcctvol: bad rdata return at rec %d\n",
				cnt.rec);
			exit(1);
           }
	   if ( cnt.rec > n_maxrec ) break;
	}

   /* fill in chan, samprate of various streams in all configurations */
	set_streamchans(sta);

   /* reset initial state of key variables */

	rewind(fpheaders);
	memcpy(&cnt, &cnt_null, sizeof(struct counters));
}

#define NSTR_OFFSET 38	/* dependent upon data logger type */

int
dokrec()
{
	int	i, j;
	int	ibeg, iend;
	int	num_streams;
	int	new_config = 1; /* assume it IS new */
	int	n_active = 0;
	int	compare_configs();
	IDACONFIG	*p;
	STREAMS		stream_list[MAXSTRM] ;

   /*
    * first read and decode stream map in this K record.
    * Note well!  This is dependent upon data logger type.
    */
	memcpy(&num_streams, buffer + NSTR_OFFSET, 4);
	if ( num_streams > MAXSTRM )
	{
		fprintf(stderr, "K record indicates too many streams.\n");
		exit(1);
	}
	for (i=0; i<num_streams; i++)
	{
		memcpy(&stream_list[i], &str_null, sizeof(STREAMS));
		j = NSTR_OFFSET + 4 + (i * 10);
		memcpy(&stream_list[i].inchan,  buffer+j,   2);
		memcpy(&stream_list[i].nfilter, buffer+j+2, 2);
		memcpy(&stream_list[i].mode,    buffer+j+4, 2);
		if (stream_list[i].inchan > -1) n_active++;
	}

	if ( n_configs > 0 ) /* compare this with previous config */
	{
		new_config = compare_configs(num_streams, stream_list);
	}

   /* if this is not new, simply return */

	if ( !new_config ) return;

   /* if reached this point, this is a new configuration.  create
    * new idaconfig record and realloc strm array as necessary
    */
	memhandler("idaconfig", n_configs+1);
	p = &idaconfig[n_configs] ;
        memcpy(p, &idaconfignull, sizeof(IDACONFIG));
        memcpy(&p->counts, &cnt, sizeof(COUNTERS));
        idaconfig[n_configs].recbeg  = cnt.rec;
        idaconfig[n_configs].nactive = n_active;
 
   /* transfer contents to working array after malloc/realloc */

	memcpy(&p->strms[0], &stream_list, MAXSTRM*sizeof(STREAMS));
        n_configs++;

	return(0);
}

int
compare_configs(num_streams, stream_list)
	int	num_streams;
	STREAMS *stream_list;
{
   /* function returns TRUE if it has found stream not in
    * previous configuration and FALSE if they match.
    */
	int	i, j, found;
	int	ibeg, iend;
	IDACONFIG	*p;

	p = &idaconfig[n_configs-1];

	for (i=0; i<num_streams; i++)
	{
	   /* if stream is inactive, go to next stream */
		if ( stream_list[i].inchan < 0 ) continue;

	   /* search for match */

		for(j=0, found = FALSE; j<MAXSTRM && !found; j++)
		{
			if ( p->strms[j].inchan  == stream_list[i].inchan  &&
			     p->strms[j].nfilter == stream_list[i].nfilter &&
			     p->strms[j].mode    == stream_list[i].mode )
			{
				found = TRUE;
			}
		}
		if ( !found ) return( TRUE );
	}

	return(FALSE);
}

/*
 * map streams to channel names and sample rates
 */


int
set_streamchans(sta)
	char		*sta;
{
	int		i ;
	int		test_lgtype();
	void		fill_strm();
	IDACONFIG	*p;

	for (i=0; i<n_configs; i++)
	{
		p = &idaconfig[i];
		logger_type = test_lgtype(p, sta);
		fill_strm(logger_type, p);
	}

}

int
test_lgtype(p, sta)
	IDACONFIG	*p;
	char		*sta;
{
   /* look at stream 0 for clue to type */

	if ( strlen(p->strms[0].chan) > 1 )
	{
		return(ANT); /* chan already filled */
	}
	else if ( p->strms[0].nfilter == 3 )
	{
		return(MK6B);
	}
	else if ( p->strms[0].nfilter == 11 )
	{
		if ( !strcmp(sta, "RAYN") )
		{
			return(MK7C);
		}
		else if (p->strms[12].nfilter == 10)
		{
			return(MK7A);
		}
		else
		{
			return(MK7B);
		}
	}
	return(MK7B); /* arbitrary default */
}

void
fill_strm(ltype, p)
	IDACONFIG	*p;
	int		ltype;
{
	int	fill_MK6B();
	int	fill_MK7A();
	int	fill_MK7B();
	int	fill_MK7C();

	switch(ltype)
	{
		case ANT:		      break;
		case MK6B:	fill_MK6B(p); break;
		case MK7A:	fill_MK7A(p); break;
		case MK7B:	fill_MK7B(p); break;
		case MK7C:	fill_MK7C(p); break;
		default:	break;
	}
}

int
fill_MK6B(p)
	IDACONFIG	*p;
{
	char	*codes[MAXSTRM] = {"bhz","bhn","bhe",
				   "blz","bln","ble",
				   "vhz","vhn","vhe",
				   "vmz","vmn","vme",
				   "wk1","ehz","ehn",
				   "ehe","elz","eln",
				   "ele","-",  "-",
				   "ae1","wk2","-",  "-"};

	static double	srates[MAXSTRM] = { 20.,   20.,   20.,
					    20.,   20.,   20.,
					     0.1,   0.1,   0.1,
					     0.1,   0.1,   0.1,
					     0.1, 200.,  200.,
					   200.,  200.,  200.,
					   200., -999., -999.,
				   	     0.1,   0.1,-999., -999.};
	int	i;

	for(i=0; i<MAXSTRM; i++)
	{
		if ( p->strms[i].inchan > -1 )
		{
			strcpy(p->strms[i].chan,  codes[i]);
			p->strms[i].samprate= srates[i];
		}
	}
	return;
}

int
fill_MK7A(p)
	IDACONFIG	*p;
{
	char	*codes[MAXSTRM] = {"bhz","bhn","bhe",
				   "lhz","lhn","lhe",
				   "vhz","vhn","vhe",
				   "vmz","vmn","vme",
				   "ehz","ehn","ehe",
				   "elz","eln","ele",
				   "-",  "-",  "-",
				   "-",  "-",  "-",  "-"};
	static double	srates[MAXSTRM] = {  20.,   20.,   20.,
				    	      1.,    1.,    1.,
					      0.1,   0.1,   0.1,
					      0.1,   0.1,   0.1,
					    100.,  100.,  100.,
					    100.,  100.,  100.,
					   -999., -999., -999.,
					   -999., -999., -999., -999.};
	int	i;

	for(i=0; i<MAXSTRM; i++)
	{
		if ( p->strms[i].inchan > -1 )
		{
			strcpy(p->strms[i].chan,  codes[i]);
			p->strms[i].samprate= srates[i];
		}
	}
	return;
}

int
fill_MK7B(p)
	IDACONFIG	*p;
{
	char	*codes[MAXSTRM] = {"bhz","bhn","bhe",
				   "lhz","lhn","lhe",
				   "vhz","vhn","vhe",
				   "vmz","vmn","vme",
				   "shz","shn","she",
				   "slz","sln","sle",
				   "egz","egn","ege",
				   "lgz","lgn","lge",  "-"};

	static double	srates[MAXSTRM] = { 20.,  20.,  20.,
					     1.,   1.,   1., 
					     0.1,  0.1,  0.1,
					     0.1,  0.1,  0.1,
					    40.,  40.,  40.,
					    40.,  40.,  40.,
					   100., 100., 100.,
					     1.,   1.,   1., -999.};
	int	i;

	for(i=0; i<MAXSTRM; i++)
	{
		if ( p->strms[i].inchan > -1 )
		{
			strcpy(p->strms[i].chan,  codes[i]);
			p->strms[i].samprate= srates[i];
		}
	}
	return;
}

int
fill_MK7C(p)
	IDACONFIG	*p;
{
	char	*codes[MAXSTRM] = {"slz","sln","sle",
				   "lhz","lhn","lhe",
				   "vhz","vhn","vhe",
				   "vmz","vmn","vme",
				   "shz","shn","she",
				   "-","-","-",
				   "egz","egn","ege",
				   "lgz","lgn","lge",  "-"};

	static double	srates[MAXSTRM] = { 40.,  40.,  40.,
					     1.,   1.,   1.,
					     0.1,  0.1,  0.1,
					     0.1,  0.1,  0.1,
					    40.,  40.,  40.,
					  -999.,-999.,-999.,
					   100., 100., 100.,
					     1.,   1.,   1., -999.};
	int	i;

	for(i=0; i<MAXSTRM; i++)
	{
		if ( p->strms[i].inchan > -1 )
		{
			strcpy(p->strms[i].chan,  codes[i]);
			p->strms[i].samprate= srates[i];
		}
	}
	return;
}

/* Revision History
 *
 * $Log: get_Krecmap.c,v $
 * Revision 1.1  2011/03/17 20:23:20  dechavez
 * initial release, based on DCC time_vol sources
 *
 */
