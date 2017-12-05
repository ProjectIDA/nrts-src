#pragma ident "$Id: main.c,v 1.1 2014/10/29 22:21:23 dechavez Exp $"
/*
 *	jd
 *  
 *     This program converts Julian date (day of year - doy) to
 *	conventional date and visa versa.
 *      Usage: jd [-n] yeardoy
 *	       jd [-n] doy year
 *	       jd month day year
 *      Month is 3 character abbreviation or month number and
 *      the year argument should be given as "1978", not "78".
 */

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <strings.h>
#ifndef TRUE
#define TRUE 1
#endif
#ifndef FALSE
#define FALSE 0
#endif

extern char *VersionIdentString;

char *mname[12] = 
	{"jan","feb","mar","apr","may","jun","jul","aug","sep","oct","nov","dec"};

main(argc,argv)
	int argc;
	char **argv;
{
	int i,month,day,year,dofy;
	char *name;
	static int mnum=FALSE;

	if((argc <  2) || (argc > 4)) usage();

	for( argc--,argv++; *argv[0]=='-' && argc; argc--,argv++) {
		switch (argv[0][1]) {
		case 'n':			/* output month number */
			mnum = TRUE;
			break;
		default:
			printf("Unrecognizable option %s\n",*argv);
			usage();
			break;
		}
	}
	if (argc < 3) {				/* to month day year */
		if(argc == 1) {			/* given yeardoy */
			dofy = year = atoi(*argv);
			year /= 1000;
			dofy -= year*1000;
		}
		else {				/* given doy year */
			dofy = atoi(*argv++);
			year = atoi(*argv);
		}
			if(!dom(dofy,&month,&day,year)) usage();
			if(mnum)
				printf("%02d %02d %d\n",month,day,year);
			else
				printf("%s %02d %d\n",mname[month-1],day,year);
	}
	else {					/* to Julian day */
		if( isalpha(*argv[0]) ) {	/* given month name */
			name = *argv;		/* get month number */
			for(i=0; i<3; i++) {
				name[i] = (isupper(name[i])) ?
					tolower(name[i]) : name[i];
			}
			for (month = 0; month < 12; month++)  {
				if( !strncmp(name,mname[month],3) ) 
					break;
			}
			if(++month > 12) usage();
		}
		else				/* given month number */
			month = atoi(*argv);
		day = atoi(*++argv);
		year = atoi(*++argv);
		if( (dofy = doy(month,day,year)) == 0) usage();
		else printf("day of year = %d\n",dofy);
	}
}

usage()
{
	printf("usage: jd [-n] yeardoy\n");
	printf("       jd [-n] doy year\n");
	printf("       jd month day year\n");
	printf("	month is alphabetic or numeric\n");
	exit(1);
}
#define OK	01
#define ERROR	00

/* define time vector subscripts */

#define YR	0
#define DY	1
#define HR	2
#define MN	3
#define SC	4
#define MS	5

/*
 * time - a subroutine package to deal with time vectors
 *
 *	A time vector is a 6-element integer array in which the elements
 * represent years, days, hours, minutes, seconds, and milliseconds,
 * in that order.
 *
 *	Absolute time vector:
 *		0 <= year <= 9999
 *		1 <= day  <= 366
 *		0 <= hour <= 23
 *		0 <= min  <= 59
 * 		0 <= sec  <= 59
 *		0 <= msec <= 999
 *
 *	Interval time vector:
 *		     year = 0
 *		0 <= day  <= 364
 *		0 <= hour <= 23
 *		0 <= min  <= 59
 *		0 <= sec  <= 59
 *		0 <= msec <= 999
 */

/******************************************************************************/

/* SAMPTM
 *
 * Given a starting time vector (strttv[]), a sample number (isamp), a
 * sampling period in seconds (samper) this subroutine calculates the time
 * vector (samptv) for the sample time. A ERROR (0) is returned if subroutine
 * multm returns an error, otherwise OK (1) is returned.
 */

samptm(strttv,isamp,samper,samptv)
	short strttv[], samptv[];
	float isamp, samper;
{
	int i, erflag;
	short temptv[6];

	for (i=YR; i<=MS; i++) {
		temptv[i] = 0;
	}
	temptv[SC] = 1;
	if (isamp >= 1) {
		erflag = multm(temptv, (samper * (isamp-1)));
	}
	else {
		erflag = multm(temptv, (samper * (1-isamp)));
	}
	if (erflag != 1) {
		printf("MULTM RETURNED ERROR TO SAMPTM\n");
		return(ERROR);
	}

	settm(samptv, strttv);

	if (isamp >= 1) {
		erflag = addtm(temptv,samptv);
	}
	else {
		erflag = subtm(temptv,samptv);
	}
	if (erflag != 1) {
		printf("ADDTM RETURNED ERROR TO SAMPTM\n");
		return(ERROR);
	}
	return(OK);
}

/******************************************************************************/

/* SAMPLE
 *
 * Given a starting time vector (strttv[]), a time vector for the sample time
 * (samptv[]) and the sampling period in seconds (samper), this subroutine 
 * calculates the sample number (isamp) closest to the specified sample time.
 */

double sconv[6]         = {0.0, 86400.0, 3600.0, 60.0, 1.0, 0.001};

sample(strttv,samptv,samper,isamp)
	short strttv[], samptv[];
	float samper, *isamp;
{
	int sign, i, erflag;
	short difftv[6];
	double secs;
	
	/* determine the difference between start and sample times */
	erflag = difftm(samptv,strttv,difftv,&sign);


	if(erflag != 1)  return(ERROR);

	/* convert time interval difference into seconds */
	secs = 0;
	for (i=MS; i>=DY; i--) {
		secs += difftv[i] * sconv[i];
	}

	/* determine closest sample number to specified sample time */
	*isamp = sign * (secs/samper) + 1;

	return(OK);
}

/******************************************************************************/

/* ADDTM
 *
 * Subroutine addtm adds a time interval vector (inttv[]) to an absolute time
 * vector (abstv[]) and leaves the result in abstv[].  If overflow occurs, the
 * subroutine returns 0 and inttv is set to the correct result mod 10,000 years.
 * Otherwise, the function returns 1.
 */

int limit[6] 	= {10000,366,24,60,60,1000};
int low[6]	= {0,1,0,0,0,0};
int borow[6]    = {0,365,24,60,60,1000};

addtm(inttv,abstv)
	short inttv[], abstv[];
{
	int i, rem;
	if (lpyr(abstv[YR])) limit[DY] = 367;
	else limit[DY] = 366;

	rem = 0;
	for (i=5; i >= 0; i--) {
		abstv[i] += inttv[i] + rem;
		rem = 0;
		while (abstv[i] >= limit[i]) {
			abstv[i] -= (limit[i] - low[i]);
			rem = rem++;
		}
	}
	if (rem == 0) return(OK);
	else return(ERROR);
}

/******************************************************************************/

/* SUBTM
 *
 * The subroutine subtracts a time interval (inttv) from the time vector
 * abstv and leaves the result in abstv.  If underflow occurs, subtm returns
 * a 0 and abstv is set to the correct result mod 10,000 years.  Otherwise,
 * the subroutine returns 1.
 */

subtm(inttv,abstv)
	short inttv[], abstv[];
{
	int rem, i;
	if (lpyr(abstv[YR]-1))  limit[DY] = 367;
	else limit[DY] = 366;

	rem = 0;
	for (i=5; i>=0; i--) {
		abstv[i] -= (inttv[i] + rem);
		rem = 0;
		while(abstv[i] < low[i]) {
			abstv[i] += (limit[i] - low[i]);
			rem++;
		}
	}

	if (rem == 0) return(OK);
	else return(ERROR);
}

/******************************************************************************/

/* DIFFTM
 *
 * Given two absolute time vectors, this subroutine wiil find the interval time
 * difference (inttv[]) between them and the sign of the difference (sign)
 *     +1 if abstv1 > abstv2
 *      0 if abstv1 = abstv2
 *     -1 if abstv1 < abstv2
 */

difftm(abstv1,abstv2,inttv,sign)
	short abstv1[], abstv2[], inttv[];
	int *sign;
{
	int i, yrdiff, rem;

	rem = 0;

	/* zero the difference time vector */
	for (i=YR; i<=MS; i++) {
		inttv[i] = 0;
	}
	
	/* determine sign of difference and return if zero */
	*sign = ieqtm(abstv1,abstv2);
	if (*sign == 0)   return(OK);

	/* determine difference between years , if any */
	yrdiff = (abstv1[0] - abstv2[0]) * *sign;
	if (yrdiff > 0) {
		if (yrdiff > 1) return(5);
		/* correct day limit if smaller year is a leap year */
		if (lpyr(abstv1[YR] < abstv2[YR] ? abstv1[YR] : abstv2[YR]))
			borow[DY] = 366;
		else borow[DY] = 365;
	}

	if (*sign == 1) {
		/* abstv1 greater than abstv2 */
		for (i=5; i>=1; i--) {
			inttv[i] = abstv1[i] - (abstv2[i] + rem);
			rem = 0;
			while (inttv[i] < 0) {
				inttv[i] += borow[i];
				rem++;
			}
		}
	}
	else {
		/* abstv1 less than abstv2 */
		for (i=5; i>=1; i--) {
			inttv[i] = abstv2[i] - (abstv1[i] + rem);
			rem = 0;
			while (inttv[i] < 0) {
				inttv[i] += borow[i];
				rem++;
			}
		}
	}

	return(OK);
}

/******************************************************************************/

/* MULTM
 *
 * Multm multiplies a time interval vector by a non-negative floating point 
 * number and returns the time interval vector product in the original vector
 *
 * If flval is less than 0.0, or there is overflow, multm will return ERROR (0);
 * otherwise it returns OK (1).
 */

double conv[6] = {0.0, 86400000.0, 3600000.0, 60000.0, 1000.0, 1.0};

multm(inttv,flval)
	short inttv[];
	float flval;
{
	int i;
	short tinttv[6];
	double msecs;

	msecs = 0;
	for (i=0; i<6; i++) {
		tinttv[i] = 0;
	}

	if ((inttv[YR] != 0) || (flval < 0)) {
		return(ERROR);
	}

	for (i=MS; i>=DY; i--) {
		msecs += inttv[i] * conv[i];
	}

  	msecs *= flval;
	msecs += .5;

	for (i=DY; i <= MS; i++) {
		if(conv[i] <= msecs) {
			tinttv[i] = msecs/conv[i];
			msecs -= (tinttv[i] * conv[i]);
		}
	}
	settm(inttv,tinttv);
	return(OK);
}

/******************************************************************************/

/* CHKTMA
 *
 * This function checks whether abstv is a legal absolute time vector, 
 * returning TRUE (1) if it is and FALSE (0) if it is not.
 */

chktma(abstv)
	short abstv[];
{
	int maxdays;
	maxdays = 365;
	if (lpyr(abstv[YR])) maxdays = 366;

	if (abstv[YR]<0 || abstv[YR]>9999) return(FALSE);
	if (abstv[DY]<1 || abstv[DY]>maxdays) return(FALSE);
	if (abstv[HR]<0 || abstv[HR]>23) return(FALSE);
	if (abstv[MN]<0 || abstv[MN]>59) return(FALSE);
	if (abstv[SC]<0 || abstv[SC]>59) return(FALSE);
	if (abstv[MS]<0 || abstv[MS]>999) return(FALSE);
	return(TRUE);
}

/******************************************************************************/

/* CHKTMI
 *
 * This fundtion checks whether inttv is a legal interval time vector,
 * returning TRUE (1) if it is and FALSE (0) if it is not.
 */

chktmi(inttv)
	short inttv[];
{
	if (inttv[YR] != 0) return(FALSE);
	if (inttv[DY]<0 || inttv[DY]>364) return(FALSE);
	if (inttv[HR]<0 || inttv[HR]>23) return(FALSE);
	if (inttv[MN]<0 || inttv[MN]>59) return(FALSE);
	if (inttv[SC]<0 || inttv[SC]>59) return(FALSE);
	if (inttv[MS]<0 || inttv[MS]>999) return(FALSE);
	return(TRUE);
}

/******************************************************************************/

/* DOY
 * 
 * This function will convert mon and day within the year and return it as
 * the value of the function.  If mon or day are illegal values, the value 
 * of the function will be FALSE (0).
 */

int ndays[13] = {0,0,31,59,90,120,151,181,212,243,273,304,334};

doy(mon,day,year)
	int mon, day, year;
{
	int inc;
	if (mon < 1 || mon > 12) return(FALSE);
	if (day < 1 || day > 31) return(FALSE);
	if (lpyr(year) && mon > 2) inc = 1;
	else inc = 0;
	
	return(ndays[mon] + day + inc);
}

/******************************************************************************/

/* DOM
 *
 * This function converts a day within the year (dofy) to the month and
 * day of the month.  If dofy is an illegal value, mon and day will be 
 * returned as 0.
 */

int mdays[13] = {0,31,28,31,30,31,30,31,31,30,31,30,31};

dom(dofy,mon,day,year)
	int dofy,*mon,*day,year;
{
	int iday;
	if (dofy < 1) {
		*mon = 0;
		*day = 0;
		return(0);
	}

	if (lpyr(year)) mdays[2] = 29;
	else mdays[2] = 28;

	for (*mon = 1; *mon <= 12; (*mon)++) {
		*day = dofy;
		if ((dofy -= mdays[*mon]) <= 0) return(OK);
	}

	*mon = 0;
	*day = 0;
	return(ERROR);
}

/******************************************************************************/

/* LPYR
 * 
 * This function returns TRUE if year is a leap year.
 */

lpyr(year)
	int year;
{
	if (year%400 == 0) return(TRUE);
	if (year%4 != 0) return(FALSE);
	if (year%100 == 0) return(FALSE);
	else return(TRUE);
}

/******************************************************************************/

/* SETTM
 *
 * Subroutine settm sets time vector tv1 equal to tv2.
 */

settm(tv1,tv2)
	short tv1[], tv2[];
{
	int i;
	for (i=0; i<6; i++) {
		tv1[i] = tv2[i];
	}
}

/******************************************************************************/

/* IEQTM
 *
 * This subroutine compares tv1 to tv2 and returns:
 *       1 if tv1 > tv2
 *       0 if tv1 = tv2
 *      -1 if tv1 < tv2
 */

ieqtm(tv1,tv2)
	short tv1[], tv2[];
{
	int i;
	for (i=0; i<=5; i++) {
		if (tv1[i] < tv2[i]) return(-1);
		if (tv1[i] > tv2[i]) return (1);
	}
	return(0);
}

/******************************************************************************/

/* INTTM
 *
 * Inttm converts an integer representing the designated units to an interval
 * time vector (inttv[6]).  If there is overflow, the function returns ERROR (0) and
 * inttv is set to {0,0,0,0,0,0}.  Otherwise the subroutine returns OK (1).
 *
 * Values fo units:
 *	0  milliseconds
 *	1  hundredths of seconds
 *	2  tenths of seconds
 *	3  seconds
 *	4  minutes
 *	5  hours
 *	6  days
 */

int factor[7][5]	= {   0,   0,   0,1000,   1,
			    0,   0,6000, 100,   1,
			    0,   0, 600,  10,   1,
			    0,3600,  60,   1,   0,
			 1440,  60,   1,   0,   0,
			   24,   1,   0,   0,   0,
			    1,   0,   0,   0,   0};

inttm(integer,inttv,units)
	int integer, units;
	short inttv[];
{
	int i, ifactor, rem;
	ifactor = 0;
	for (i=0; i < 6; i++) {
		inttv[i] = 0;
	}
	if ((units < 0) || (units > 6)) return(ERROR);
	if (integer < 0) return(ERROR);
	rem = integer;
	for (i=0; (i < 5) && (ifactor != 1); i++) {
		ifactor = factor[units][i];
		if (ifactor != 0) {
			inttv[i+1] = rem/ifactor;
			rem -= inttv[i+1]*ifactor;
		}
	}
	if (units == 1) inttv[MS] = inttv[MS]*10;
	if (units == 2) inttv[MS] = inttv[MS]*100;

	if (inttv[1] <= 364) return(OK);
	else {
		for (i=0; i < 6; i++) {
			inttv[i] = 0;
		}
		return(ERROR);
	}
}

/* $Log: main.c,v $
/* Revision 1.1  2014/10/29 22:21:23  dechavez
/* copied from Center for Seismic Studies source tree
/*
 */
