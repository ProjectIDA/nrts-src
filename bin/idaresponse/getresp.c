#pragma ident "$Id: getresp.c,v 1.3 2015/12/08 21:11:06 dechavez Exp $"
#include "idaresponse.h"

extern BOOL verbose;

static char SccsId[] = "%W%	%G%";

static char *getchannum(char *chan)
{
static char *names[] = {
	"BHZ", "BHN", "BHE",
	"BLZ", "BLN", "BLE",
	"LHZ", "LHN", "LHE",
	"VHZ", "VHN", "VHE",
	"SHZ", "SHN", "SHE",
	"SLZ", "SLN", "SLE",
	"EHZ", "EHN", "EHE",
	"ELZ", "ELN", "ELE",
	"LGZ", "LGN", "LGE",
	"EGZ", "EGN", "EGE",
    NULL

};
static char *nums[] =  {
	"11", "12", "13",
	"01", "02", "03",
	"14", "15", "16",
	"17", "18", "19",
	"54", "55", "56",
	"51", "52", "53",
	"54", "55", "56",
	"51", "52", "53",
	"24", "25", "26",
	"57", "58", "59",
	NULL
};
INT32 i = 0;

	while ( names[i] != NULL ) {
		if (strcmp(chan, names[i]) == 0) return(nums[i]);
		i++;
	}
	return("");
}

static INT32 getseedresp(WFDISC *outwd, REAL64 *amp, REAL64 *phase, INT32 nf, REAL64 df, REAL64 nyquist, INT32 acc, INT32 displ, char *respath, BOOL s1only)
{
INT32	i;
INT32	hr = 0, mn = 0, sec = 0;
REAL64	f, dum;
char	fname[128];
char	comline[256];
char	sta[8], chan[16];
char	locid[4];
char	tstr1[8], tstr2[8], tstr3[8];
char	rtype[4];
FILE	*fp;

   /* run evalresp */

	sprintf(tstr1, "%d", outwd->jdate);
	strncpy(tstr2, tstr1, 4); tstr2[4] = '\0';
	strncpy(tstr3, tstr1+4, 3); tstr3[3] = '\0';
	util_ucase(outwd->sta);  strcpy(sta,  outwd->sta);
	util_ucase(outwd->chan); strcpy(chan, outwd->chan);
	if ( strlen(chan) == 5 ) /* terminate chan at 3 chars, treat 4+5 as locid */
	{
		strcpy(locid, chan+3);
		chan[3] = '\0';
	}
	else
	{
		strcpy(locid, "00");
	}


	strcpy(rtype, "vel");
	if ( acc ) strcpy(rtype, "acc");
	if ( displ ) strcpy(rtype, "dis");

	comline[0] = 0;
	sprintf(comline, "evalresp");
	sprintf(comline+strlen(comline), " %s", sta);
	sprintf(comline+strlen(comline), " %s", chan);
	sprintf(comline+strlen(comline), " %s", tstr2);
	sprintf(comline+strlen(comline), " %s", tstr3);
	sprintf(comline+strlen(comline), " %le", df);
	sprintf(comline+strlen(comline), " %le", nyquist);
	sprintf(comline+strlen(comline), " %d", nf);
	sprintf(comline+strlen(comline), " -u");
	sprintf(comline+strlen(comline), " %s", rtype);
	sprintf(comline+strlen(comline), " -t %02d:%02d:%02d", hr, mn, sec);
	sprintf(comline+strlen(comline), " -f %s", respath);
	sprintf(comline+strlen(comline), " -s lin");
	sprintf(comline+strlen(comline), " -l %s", locid);
	sprintf(comline+strlen(comline), " -il");
	if (s1only) sprintf(comline+strlen(comline), " -stage 1");

	if (verbose) printf("%s\n", comline);
	system(comline);

    /* read the amp file */

	sprintf(fname, "AMP.%s.%s.%s.%s", DEFAULT_NETWORK, sta, locid, chan );
	if ((fp = fopen(fname, "r")) == NULL) {
		fprintf(stderr, "Could not open file %s\n", fname);
		perror(fname);
		return(1);
	}

	for (i=0; i<nf; i++) {
		fscanf(fp, "%lf %lf", &f, &dum);
		amp[i] = dum;
	}
	fclose(fp);

	if ( unlink(fname) ) fprintf(stderr, "WARNING: could not remove file %s\n", fname);

    /* read the phase file */

	sprintf(fname, "PHASE.%s.%s.%s.%s", DEFAULT_NETWORK, sta, locid, chan);
	if ((fp = fopen(fname, "r")) == NULL) {
		fprintf(stderr, "Could not open file %s\n", fname);
		perror(fname);
		return(1);
	}

	for (i=0; i<nf; i++) {
		fscanf(fp, "%lf %lf", &f, &dum);
		phase[i] = dum;
	}
	fclose(fp);
	if ( unlink(fname) ) fprintf(stderr, "WARNING: could not remove file %s\n", fname);
	util_lcase(outwd->chan);

	return(0);

}

static INT32 getidaresp(WFDISC *outwd, REAL64 *amp, REAL64 *phase, INT32 nf, REAL64 df, REAL64 nyquist, INT32 acc, INT32 displ, char *filenm)
{
	INT32	i;
	INT32	hr = 0, mn = 0, sec = 0;
	REAL32	dum;
	char	fname[128];
	char	nchan[4];
	char	*getchannum();
	char	comline[256];
	char	sta[16],chan[16];
	char	tstr1[8], tstr2[8], tstr3[8];
	char	rtype[4];
	FILE	*fp;

   /* run staresp */

	sprintf(tstr1, "%d", outwd->jdate);
	strncpy(tstr2, tstr1, 4); tstr2[4] = '\0';
	strncpy(tstr3, tstr1+4, 3); tstr3[3] = '\0';
	util_lcase(outwd->sta); strcpy(sta, outwd->sta);
	util_ucase(outwd->chan); strcpy(chan, outwd->chan);

	strcpy(nchan, getchannum(chan) );

	strcpy(rtype, "vel");
	if ( acc ) strcpy(rtype, "acc");
	if ( displ ) strcpy(rtype, "dis");

	sprintf(comline, "staresp %s %s %s %s %g %d %s", sta, tstr2, tstr3, nchan, nyquist/(float)nf, nf+1, rtype);
	if (verbose) printf("%s\n", comline);
	system(comline);

    /* read the amp file */

	sprintf(fname, "%s.%s.amp", sta, nchan);
	if ((fp = fopen(fname, "rb")) == NULL)
	{
		fprintf(stderr, "Could not open file %s\n", fname);
		perror(fname);
		return(1);
	}

	fread(&dum, sizeof(float), 1, fp);
	for (i=0; i<nf; i++)
	{
		fread(&dum, sizeof(float), 1, fp);
		amp[i] = dum;
	}
	fclose(fp);
	if ( unlink(fname) )
	{
		fprintf(stderr, "Could not remove file %s\n", fname);
	}

    /* read the phase file */

	sprintf(fname, "%s.%s.pha", sta, nchan);
	if ((fp = fopen(fname, "r")) == NULL)
	{
		fprintf(stderr, "Could not open file %s\n", fname);
		perror(fname);
		return(1);
	}

	fread(&dum, sizeof(float), 1, fp);
	for (i=0; i<nf; i++)
	{
		fread(&dum, sizeof(float), 1, fp);
		phase[i] = dum;
	}
	fclose(fp);
	if ( unlink(fname) )
	{
		fprintf(stderr, "Could not remove file %s\n", fname);
	}

	util_ucase(outwd->sta);
	util_lcase(outwd->chan);
	return(0);

}

INT32 getresp(WFDISC *outwd, REAL64 *amp, REAL64 *phase, INT32 nf, REAL64 df, REAL64 nyquist, INT32 acc, INT32 displ, INT32 ida, BOOL s1only, char *filenm)
{
INT32	iret;

	if ( ida ) {
		iret =  getidaresp(outwd,amp,phase,nf,df,nyquist,acc,displ,filenm);
	} else {
		iret = getseedresp(outwd,amp,phase,nf,df,nyquist,acc,displ,filenm, s1only);
	}

	return(iret);
}

/* Revision History
 *
 * $Log: getresp.c,v $
 * Revision 1.3  2015/12/08 21:11:06  dechavez
 * various minor tweaks to get clean Mac OS X builds
 *
 * Revision 1.2  2015/11/18 20:25:51  dechavez
 * added support for "s1" and "verbose" options
 *
 * Revision 1.1  2014/04/15 21:12:48  dechavez
 * initial release using sources modified from DCC version
 *
 */
