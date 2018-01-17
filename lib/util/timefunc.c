#pragma ident "$Id: timefunc.c,v 1.29 2015/08/24 19:13:48 dechavez Exp $"
/*======================================================================
 *
 *  Misc. time related functions.
 *
 *----------------------------------------------------------------------
 *
 *  utilAttodt:
 *  Convert a string of the form "yyyy:ddd-hh:mm:ss:msc" to double time.
 *  Can truncate fields from the right.
 *
 *----------------------------------------------------------------------
 *
 *  utilDttostr:
 *  Given a double time value and format code, make a string of one of
 *  the following formats:
 *
 *  Format code   Output string
 *       0        yyyy:ddd-hh:mm:ss.msc
 *       1        Mon dd, year hh:mm:ss:msc
 *       2        yy:ddd-hh:mm:ss.msc, where input time is an interval
 *       3        yyyydddhhmmssmsc
 *       4        yyyyddd
 *       5        Day Mon dd, year
 *       6        yyyymmddhhmmss
 *       7        yyyy mm dd hh mm ss
 *       8        ddd-hh:mm:ss.msc, where input time is an interval
 *       9        yydddhh
 *       10       Day Mon hh:mm:ss year
 *       14       dd mnth yyyy (ddd)
 *       15       hh:mm:ss
 *       16       yyyyddd mm dd, yyyy hh:mm:ss
 *
 *  No newline is appended.
 *
 *----------------------------------------------------------------------
 *
 *  utilLttostr:
 *  Given a long time value and format code, make a string of one of
 *  the following formats:
 *
 *  Format code   Output string
 *       0        yyyy:ddd-hh:mm:ss
 *       1        Mon dd, year hh:mm:ss
 *       2        yy:ddd-hh:mm:ss, where input time is an interval
 *       3        yydddhhmmss
 *       4        yyyyddd
 *       5        Day Mon dd, year
 *       6        yyyymmddhhmmss
 *       7        yyyy mm dd hh mm ss
 *       8        ddd-hh:mm:ss, where input time is an interval
 *       9        yydddhh
 *       10       Day Mon hh:mm:ss year
 *       14       dd mnth yyyy (ddd)
 *       15       hh:mm:ss
 *       16       yyyyddd mm dd, yyyy hh:mm:ss
 *
 *  No newline is appended.
 *
 *----------------------------------------------------------------------
 *
 *  utilTsplit:
 *  Split a double time to yyyy, ddd, hh, mm, ss, msc.
 *
 *----------------------------------------------------------------------
 *
 *  utilYdhmsmtod:
 *  Given year, day, hour, minutes, seconds, and milliseconds, return
 *  a double containing the seconds since 00:00:00.000 Jan 1, 1970.
 *
 *  Only works for times after Jan 1, 1970!
 *
 *----------------------------------------------------------------------
 *
 *  utilJdtomd:
 *  Given year and day of year, determine month and day of month.
 *
 *----------------------------------------------------------------------
 *
 *  utilYmdtojd:
 *  Given year, month, and day determine day of year.
 *
 *----------------------------------------------------------------------
 *
 *  utilToday:
 *  Returns today's date in YYYYDDD form.
 *
 *====================================================================*/
#include "util.h"

#define UTIL_SAN_TIME_FACTOR 1000
#define UTIL_LONGTIME_FACTOR  100

#ifndef leap_year
#define leap_year(i) ((i % 4 == 0 && i % 100 != 0) || i % 400 == 0)
#endif
 
#ifndef daysize
#define daysize(i) (365 + leap_year(i))
#endif

static char daytab[2][13] = {
    {0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31},
    {0, 31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31},
};

static char *month_name[] = {
    "   ", /* cause we increment it after gmtime_r() */
    "Jan", "Feb", "Mar", "Apr", "May", "Jun", 
    "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
};

static char *day_name[] = {
    "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"
};

static char *UnsupportedCode = "<UNSUPPORTED FORMAT CODE>";

#define MINIMUM_CODE 0
#define MAXIMUM_CODE 16
static char   *EmptyString[MAXIMUM_CODE - MINIMUM_CODE + 1] = {
    "                     ",
    "                         ",
    "                   ",
    "                ",
    "       ",
    "                ",
    "              ",
    "                   ",
    "                ",
    "       "
};

#define SPM (       60L)
#define SPH (SPM *  60L)
#define SPD (SPH *  24L)
#define SPY (SPD * 365L)

static void TruncateMscPortion(int code, char *string)
{
    switch (code) {
      case 0:
      case 1:
      case 2:
      case 8:
        string[strlen(string)-strlen(".msc")] = 0;
        break;
      case 3:
        string[strlen(string)-strlen("msc")] = 0;
        break;
    }
}

/***********************************************************************/

#define COPYBUFLEN 32

double utilAttodt(char *string)
{
double result;
char copy[COPYBUFLEN];
char *token[6];
int errors, ntoken, yr, da, hr, mn, sc, ms, mon, dom;
static char *special_case = "yyyy/dd/mm hh:mm:ss";
static char *fid = "utilAttodt";

/*  Check for special case ("present")  */

    if (strcasecmp(string, "present") == 0) return (double) 2147483647;

/*  Parse (copy of) string  */

    strlcpy((void *) copy, (void *) string, (size_t) COPYBUFLEN);

    ntoken = utilParse(copy, token, "-/.:(),; ", 6, 0);

    yr = hr = mn = sc = ms = 0; da = 1;
        
/* Check for special case where string is of the form "year/mo/dom hr:mn:sc" */

    if (strlen(string) == strlen(special_case) && string[4] == '/' && string[7] == '/') {
        yr = atoi(token[0]);
        mon = atoi(token[1]);
        dom = atoi(token[2]);
        da  = utilYmdtojd(yr, mon, dom);
        hr  = atoi(token[3]);
        mn  = atoi(token[4]);
        sc  = atoi(token[5]);

/* otherwise decode as yyyy:ddd-hh:mm:ss.msc */

    } else {
        switch (ntoken) {

          case 6:
               ms = atoi(token[5]);
          case 5:
            sc = atoi(token[4]);
          case 4:
            mn = atoi(token[3]);
          case 3:
            hr = atoi(token[2]);
          case 2:
            da = atoi(token[1]);
          case 1:
            yr = atoi(token[0]) + ((strlen(token[0]) == 2) ? 1900 : 0);
            break;
          default:
            errno = EINVAL;
            return -1.0;
        }
    }

    errors = 0;
    if (yr < 1970)          ++errors;
    if (da < 1 || da > 366) ++errors;
    if (hr < 0 || hr >  23) ++errors;
    if (mn < 0 || mn >  59) ++errors;
    if (sc < 0 || sc >  59) ++errors;
    if (ms < 0 || ms > 999) ++errors;
    
    if (errors) {
        errno = EINVAL;
        return -2.0;
    }

    result = utilYdhmsmtod(yr, da, hr, mn, sc, ms);

    return result;

}

/***********************************************************************/
#define SAN_EPOCH_TO_1970_EPOCH 915148800 /* also defined in sanio.h */
#define SAN_EPOCH_TO_2000_EPOCH (-31536000) /* 2000 is 946684800 from 1970 */

char *utilDttostr(double dtime, int code, char *buf)
{
time_t ltime;
double ffrac;
int   ifrac, yr, da, hr, mn, sc, ms;
struct tm tm;
struct css_date_time dt;
static char mtunsafe[] = "xxxxxxxxxxxxxxxxxxxxxxxxx plus some extra";

    if (buf == NULL) buf = mtunsafe;

    if (code >= UTIL_SAN_TIME_FACTOR) {
        dtime += SAN_EPOCH_TO_1970_EPOCH;
        code -= UTIL_SAN_TIME_FACTOR;
    }

    if (code < MINIMUM_CODE || code > MAXIMUM_CODE) return UnsupportedCode;

    ltime = (long) dtime;
    ffrac = ((dtime - (double) ltime) * 1000.0); /* factional part, in msecs */
    ifrac = (int) ffrac;
    if (ffrac - (float) ifrac >= 0.5) ifrac++;

/* Deal with the intervals */

    if (code == 2 || code == 8) {
        yr = ltime / SPY; ltime -= yr * SPY;
        da = ltime / SPD; ltime -= da * SPD;
        hr = ltime / SPH; ltime -= hr * SPH;
        mn = ltime / SPM; ltime -= mn * SPM;
        sc = ltime;
        ms = ifrac;

        if (code == 2) {
            sprintf((char *) buf,"%2.2d:%3.3d-%2.2d:%2.2d:%2.2d.%3.3d",
                (int) yr, (int) da, (int) hr, (int) mn, (int) sc, (int) ms
            );
        } else {
            sprintf((char *) buf,"%3.3d-%2.2d:%2.2d:%2.2d.%3.3d",
                (int) da, (int) hr, (int) mn, (int) sc, (int) ms
            );
        }
        return buf;
    }

    dt.epoch = dtime;
    util_etoh(&dt);

    if (gmtime_r(&ltime, &tm) == NULL) return EmptyString[code];
    tm.tm_year += 1900;
    tm.tm_yday += 1;
    tm.tm_mon  += 1;

    switch (code) {
        case 0:
            sprintf((char *) buf,"%4.4d:%3.3d-%2.2d:%2.2d:%06.3f", dt.year, dt.doy, dt.hour, dt.minute, dt.second);
            break;
        case 1:
            sprintf((char *) buf, "%s %2.2d, %4.4d %2.2d:%2.2d:%06.3f", dt.mname, dt.day, dt.year, dt.hour, dt.minute, dt.second);
            break;
        case 3:
            sprintf((char *) buf,"%4.4d%3.3d%2.2d%2.2d%06.3f", dt.year, dt.doy, dt.hour, dt.minute, dt.second);
            break;
        case 4:
            sprintf((char *) buf,"%4.4d%3.3d", tm.tm_year, tm.tm_yday);
            break;
        case 5:
            sprintf((char *) buf, "%s %2.2d/%2.2d/%4.4d", day_name[tm.tm_wday], tm.tm_mon, tm.tm_mday, tm.tm_year);
            break;
        case 6:
            sprintf((char *) buf,"%4.4d%2.2d%2.2d%2.2d%2.2d%2.2d", tm.tm_year, tm.tm_mon, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);
            break;
        case 7:
            sprintf((char *) buf,"%4.4d %2.2d %2.2d %2.2d %2.2d %2.2d", tm.tm_year, tm.tm_mon, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);
            break;
        case 9:
            sprintf((char *) buf,"%2.2d%3.3d%2.2d", tm.tm_year - 2000, tm.tm_yday, tm.tm_hour);
            break;
        case 10:
            sprintf((char *) buf, "%s %s %2d %02d:%02d:%02d %04d", day_name[tm.tm_wday], dt.mname, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec, tm.tm_year);
            break;
        case 14:
            sprintf((char *) buf,"%02d %s %04d (%03d)", tm.tm_mday, month_name[tm.tm_mon], tm.tm_year, tm.tm_yday); break;
        case 15:
            sprintf((char *) buf,"%02d:%02d:%02d", tm.tm_hour, tm.tm_min, tm.tm_sec);
            break;
        case 16:
            sprintf((char *) buf,"%04d%03d %s %d, %04d %02d:%02d:%02d", tm.tm_year, tm.tm_yday, month_name[tm.tm_mon], tm.tm_mday, tm.tm_year, tm.tm_hour, tm.tm_min, tm.tm_sec);
            break;

        default: 
            return UnsupportedCode;
    }

    return buf;
}

/***********************************************************************/

char *utilLttostr(long ltime, int code, char *buf)
{
char *string;
double dtime;

    dtime = (double) ltime;
    string = utilDttostr(dtime, code, buf);
    if (code >= UTIL_SAN_TIME_FACTOR) code -= UTIL_SAN_TIME_FACTOR;
    switch (code) {
      case 0:
      case 1:
      case 2:
      case 3:
      case 8:
      case 9:
      case 10:
      case 14:
      case 15:
      case 16:
        TruncateMscPortion(code, string);
        break;
    }

    return string;
}

/***********************************************************************/

VOID utilTsplit(double dtime, int *yr, int *da, int *hr, int *mn, int *sc, int *ms)
{
time_t ltime;
int  imsc;
double dmsc;
struct tm tm;

    ltime = (long) dtime;
    dmsc = ((dtime - (double) ltime)) * (double) 1000.0;
    imsc = (int) dmsc;
    if (dmsc - (double) imsc >= (double) 0.5) imsc++;
    if (imsc == 1000) {
        ++ltime;
        imsc = 0;
    }

    gmtime_r(&ltime, &tm);
    *yr = 1900 + tm.tm_year;
    *da = ++tm.tm_yday;
    *hr = tm.tm_hour;
    *mn = tm.tm_min;
    *sc = tm.tm_sec;
    *ms = imsc;
}

VOID utilTsplit2(double dtime, int *yr, int *da, int *hr, int *mn, int *sc, int *us)
{
time_t ltime;
int  iusc;
double dusc;
struct tm tm;

    ltime = (time_t) dtime;
    dusc = ((dtime - (double) ltime)) * (double) 1000000.0;
    iusc = (int) dusc;
    if (dusc - (double) iusc >= (double) 0.5) iusc++;
    if (iusc == 1000000) {
        ++ltime;
        iusc = 0;
    }

    gmtime_r(&ltime, &tm);
    *yr = 1900 + tm.tm_year;
    *da = ++tm.tm_yday;
    *hr = tm.tm_hour;
    *mn = tm.tm_min;
    *sc = tm.tm_sec;
    *us = iusc;
}

INT32 utilYearDay(double dtime)
{
INT32 result;
int yr, da, unused;

    utilTsplit(dtime, &yr, &da, &unused, &unused, &unused, &unused);
    result = (yr * 1000) + da;

    return result;
}

void utilDecomposeTimestamp(INT64 tstamp, int *pyr, int *pda, int *phr, int *pmn, int *psc, int *pns)
{
int ns;
time_t ltime;
struct tm tm;

    ltime = (time_t) (tstamp / NANOSEC_PER_SEC); /* sec since 1/1/1999 */
    ns = (int) (tstamp - (ltime * NANOSEC_PER_SEC));   /* nsec fractional part of tstamp */

    ltime += SAN_EPOCH_TO_1970_EPOCH; /* ltime is now secs since 1970 */
    gmtime_r(&ltime, &tm);
    *pyr = 1900 + tm.tm_year;
    *pda = ++tm.tm_yday;
    *phr = tm.tm_hour;
    *pmn = tm.tm_min;
    *psc = tm.tm_sec;
    *pns = ns;
}

INT64 utilComposeTimestamp(int year, int day, int hr, int mn, int sc, int ns)
{
#ifndef SAN_EPOCH_YEAR
#define SAN_EPOCH_YEAR 1999
#endif
INT64 days = 0, secs;

    if (year > SAN_EPOCH_YEAR) {
        while (--year >= SAN_EPOCH_YEAR) days += ISLEAP(year) ? 366 : 365;
    } else if (year < SAN_EPOCH_YEAR) {
        while (year < SAN_EPOCH_YEAR) { days -= ISLEAP(year) ? 366 : 365; year++; };
    }
    days += (day-1); 

    secs = (days * 86400) + (hr * 3600) + (mn * 60) + sc;
    return (secs * NANOSEC_PER_SEC) + ns;
}

/***********************************************************************/

REAL64 utilYdhmsmtod(int yr, int da, int hr, int mn, int sc, int ms)
{
REAL64 result;
struct css_date_time dt;

    dt.date = yr * 1000 + (da == 0 ? 1 : da);
    dt.hour = hr;
    dt.minute = mn;
    dt.second = (float) sc + (float) (ms / 1000.0);

    util_htoe(&dt);
    result = (REAL64) dt.epoch;

    return result;
}

/***********************************************************************/

VOID utilJdtomd(int year, int day, int *m_no, int *d_no)
{
int i, leap;

    leap = leap_year(year);
    
    for (i = 1; day > daytab[leap][i]; i++) day -= daytab[leap][i];

    *m_no = i;
    *d_no = day;
}

/***********************************************************************/

UINT32 utilYmdtojd(int year, int mo, int da)
{
long jd, m, leap;

    leap = leap_year(year);
    for (jd = 0, m = 1; m < mo; m++) jd += daytab[leap][m];
    jd += da;

    return jd;
}

/***********************************************************************/

UINT32 utilToday(VOID)
{
time_t now;
struct tm *current;

    now = time(NULL);
    current = localtime(&now);
    current->tm_year += 1900;
    ++current->tm_yday;

    return (1000 * current->tm_year) + current->tm_yday;

}

/***********************************************************************/

double util_attodt(char *string)
{
    return utilAttodt(string);
}

char *util_dttostr(double dtime, int code)
{
static char string[] = "xxxxxxxxxxxxxxxxxxxxxxxxx";

    return utilDttostr(dtime, code, string);
}

char *util_lttostr(long ltime, int code)
{
static char string[] = "xxxxxxxxxxxxxxxxxxxxxxxxx";

    return utilLttostr(ltime, code, string);
}

void util_tsplit(double dtime, int *yr, int *da, int *hr, int *mn, int *sc, int *ms)
{
    utilTsplit(dtime, yr, da, hr, mn, sc, ms);
}

double util_ydhmsmtod(int yr, int da, int hr, int mn, int sc, int ms)
{
    return utilYdhmsmtod(yr, da, hr, mn, sc, ms);
}

int util_jdtomd(int year, int day, int *m_no, int *d_no)
{
    utilJdtomd(year, day, m_no, d_no);
    return 0;
}

int util_ymdtojd(int year, int mo, int da)
{
    return utilYmdtojd(year, mo, da);
}

UINT32 util_today(VOID)
{
    return utilToday();
}

char *utilTimeString(INT64 value, int code, char *buf, int buflen)
{
BOOL IntegerTime = FALSE;
char *result;
static char mtunsafe[] = "xxxxxxxxxxxxxxxxxxxxxxxxx plus some extra";

    if (buf == NULL) {
        buf = mtunsafe;
        buflen = strlen(buf);
    }

    if (code >= UTIL_SAN_TIME_FACTOR) {
        value += (SAN_EPOCH_TO_1970_EPOCH * NANOSEC_PER_SEC);
        code -= UTIL_SAN_TIME_FACTOR;
    }

    if (code >= UTIL_LONGTIME_FACTOR) {
        code -= UTIL_LONGTIME_FACTOR;
        IntegerTime = TRUE;
    }

    if (code < MINIMUM_CODE || code > MAXIMUM_CODE) return UnsupportedCode;
    if (value == UTIL_UNDEFINED_TIMESTAMP) {
        strlcpy(buf, EmptyString[code], buflen);
        if (IntegerTime) TruncateMscPortion(code, buf);
        return buf;
    }

    if (IntegerTime) {
        result = utilLttostr((INT32) (value / (UINT64) NANOSEC_PER_SEC), code, buf);
    } else {
        result = utilDttostr((REAL64) (value / (REAL64) NANOSEC_PER_SEC), code, buf);
    }

    return result;
}

INT64 utilConvertFrom1970SecsTo1999Nsec(double tstamp)
{
INT64 sec, frac, nsec;

    sec = (INT64) tstamp - SAN_EPOCH_TO_1970_EPOCH;
    frac = (tstamp - (double) ((INT64) tstamp)) * (double) NANOSEC_PER_SEC;
    nsec = (sec * NANOSEC_PER_SEC) + frac; 

    return nsec;
}

REAL64 utilConvertFrom1999NsecTo1970Secs(INT64 nsec)
{
REAL64 rsec;
INT64 isec, frac;

    isec = (nsec / NANOSEC_PER_SEC);
    frac = (nsec - (isec * NANOSEC_PER_SEC));
    rsec = isec + SAN_EPOCH_TO_1970_EPOCH + (frac / (REAL64) NANOSEC_PER_SEC);

    return rsec;
}

INT64 utilConvertFrom2000SecsTo1999Nsec(double tstamp)
{
INT64 sec, frac, nsec;

    sec = (INT64) tstamp - SAN_EPOCH_TO_1970_EPOCH;
    frac = (tstamp - (double) ((INT64) tstamp)) * (double) NANOSEC_PER_SEC;
    nsec = (sec * NANOSEC_PER_SEC) + frac; 

    return nsec;
}

/* Revision History
 *
 * $Log: timefunc.c,v $
 * Revision 1.29  2015/08/24 19:13:48  dechavez
 * introduced utilYearDay()
 *
 * Revision 1.28  2015/03/06 23:06:28  dechavez
 * introduced utilConvertFrom1970SecsTo1999Nsec(), utilConvertFrom1999NsecTo1970Secs()
 *
 * Revision 1.27  2014/08/11 17:50:06  dechavez
 * added utilTsplit2(), utilDecomposeTimestamp(), utilComposeTimestamp()
 *
 * Revision 1.26  2014/04/18 20:46:09  dechavez
 * added format code 16 (yyyyddd mm dd, yyyy hh:mm:ss)
 *
 * Revision 1.25  2014/04/14 18:12:17  dechavez
 * added format code 10
 *
 * Revision 1.24  2012/02/14 19:55:02  dechavez
 * use portable integer types (eg, long to INT32, unsigned long to UINT32)
 *
 * Revision 1.23  2011/10/12 17:31:59  dechavez
 * added support for SAN epoch time values
 *
 * Revision 1.22  2011/03/17 17:24:25  dechavez
 * changed "struct date" to "struct css_date" (to avoid conficts with Antelope software)
 *
 * Revision 1.21  2007/10/30 21:56:18  dechavez
 * replace string memcpy w/ strlcpy, add length to utilTimestring() args
 *
 * Revision 1.20  2007/01/07 17:41:04  dechavez
 * removed some strcpy()'s, some remain
 *
 * Revision 1.19  2007/01/04 23:34:27  dechavez
 * Changes to accomodate OpenBSD builds
 *
 * Revision 1.18  2006/12/06 23:50:54  dechavez
 * allow NULL destination buffers (use mt-unsafe local storage)
 *
 * Revision 1.17  2006/11/10 06:27:14  dechavez
 * cosmetic changes to utilTimeString()
 *
 * Revision 1.16  2005/08/26 18:12:11  dechavez
 * fixed typo in UTIL_LONGTIME_FACTOR
 *
 * Revision 1.15  2005/06/30 01:23:03  dechavez
 * INT64 timestamp support
 *
 * Revision 1.14  2005/05/25 22:41:46  dechavez
 * mods to calm Visual C++ warnings
 *
 * Revision 1.13  2005/04/04 20:11:14  dechavez
 * use gmtime_r in utilTsplit
 *
 * Revision 1.12  2005/03/23 21:29:28  dechavez
 * added type 14 and 15 strings to utilDttostr (aap
 *
 * Revision 1.11  2004/12/10 17:39:53  dechavez
 * add support for SAN epoch timestamps to utilDttostr()
 *
 * Revision 1.10  2004/08/24 17:19:43  dechavez
 * fixed utilYdhmstod bug in setting up date_time parameters for util_htoe
 *
 * Revision 1.9  2004/07/26 23:02:29  dechavez
 * use util_htoe and util_etoh for time conversions
 *
 * Revision 1.8  2003/12/17 20:07:30  dechavez
 * added support for time string format 9 (yydddhh), fixed utilLttostr truncation
 * logic
 *
 * Revision 1.7  2003/12/04 23:27:33  dechavez
 * fixed strange error in utilAttodt() where return value would not make
 * it back to caller under x86 gcc (return explict value rather than via
 * a call to utilYdhmsmtod())
 *
 * Revision 1.6  2003/11/13 19:34:32  dechavez
 * merged in old style functions from timefuncOLD.c
 *
 * Revision 1.5  2003/06/09 23:53:15  dechavez
 * updated with ESSW version
 *
 */
