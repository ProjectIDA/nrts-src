#pragma ident "$Id: wdtime.c,v 1.1 2014/04/15 21:00:12 dechavez Exp $"
/*======================================================================
 *
 *  Print start/end times of records from a CSS wfdisc file (2.8 or 3.0)
 *
 *====================================================================*/
#include "cssio.h"

#define DUP_FORMAT -2
#define UNDEFINED  -1
#define EPOCH_TIME  0
#define JULIAN_TIME 1
#define HUMAN_TIME  2

static void help(char *myname, int format)
{
    if (format == DUP_FORMAT) fprintf(stderr, "%s: multiple output format specifications not allowed\n\n", myname);
    fprintf(stderr, "usage: %s [ -r | -j | -h ] < CSS_wfdisc\n", myname);
    fprintf(stderr, "\n");
    fprintf(stderr, "Options: -r => print raw (epcoch) time (eg. 526953600.000000)\n");
    fprintf(stderr, "         -j => print julian dates      (eg. 1986:256-00:00:00.000)\n");
    fprintf(stderr, "         -h => print human dates       (eg. Fri Sep 13 00:00:00 1986)\n");
    fprintf(stderr, "\n");
    fprintf(stderr, "         -r is assumed if no format option is specified\n");

    exit(format == DUP_FORMAT ? -1 : 0);
}

static void PrintRawTimes(WFDISC *wd)         
{
static char *NULL_TIME = "      (css null)   ";

    if (wd->time == cssio_wfdisc_null.time) {
        printf(" %s", NULL_TIME);
    } else {
        printf(" %20.6lf", wd->time);
    }
    if (wd->nsamp == cssio_wfdisc_null.nsamp || wd->smprate == cssio_wfdisc_null.smprate || wd->endtime == cssio_wfdisc_null.endtime) {
        printf("%s", NULL_TIME);
    } else {
        printf("%19.6lf", wd->endtime);
    }
}

static void PrintCookedTimes(WFDISC *wd, int format)
{
static char *NULL_TIME0 = "      (css null)     ";
static char *NULL_TIME1 = "        (css null)       ";
char *NULL_TIME;

    NULL_TIME = (format == 0) ? NULL_TIME0 : NULL_TIME1;

    if (wd->time == cssio_wfdisc_null.time) {
        printf(" %s", NULL_TIME);
    } else {
        printf(" %s", utilDttostr(wd->time, format, NULL));
    }
    if (wd->nsamp == cssio_wfdisc_null.nsamp || wd->smprate == cssio_wfdisc_null.smprate || wd->endtime == cssio_wfdisc_null.endtime) {
        printf("%s", NULL_TIME);
    } else {
        printf(" %s", utilDttostr(wd->endtime, format, NULL));
    }
}

int main(int argc, char **argv)
{
int i, format = UNDEFINED;
WFDISC wd;
static char *NULL_SMPRATE = "   (css null)";

    for (i = 1; i < argc; i++) {
        if (strcasecmp(argv[i], "-r") == 0) {
            if (format != UNDEFINED) help(argv[0], DUP_FORMAT);
            format = EPOCH_TIME;
        } else if (strcasecmp(argv[i], "-j") == 0) {
            if (format != UNDEFINED) help(argv[0], DUP_FORMAT);
            format = JULIAN_TIME;
        } else if (strcasecmp(argv[i], "-h") == 0) {
            if (format != UNDEFINED) help(argv[0], DUP_FORMAT);
            format = HUMAN_TIME;
        } else if (strcasecmp(argv[i], "-help") == 0) {
            help(argv[0], format);
        } else {
            fprintf(stderr, "%s: unrecognized option '%s'\n", argv[0], argv[i]);
            help(argv[0], format);
        }
    }

    while (rwfdrec(stdin, &wd) == 0) {
        printf("%5s", wd.sta);
        printf(" %6s", wd.chan);
        switch (format) {
          case JULIAN_TIME: PrintCookedTimes(&wd, 0);  break;
          case HUMAN_TIME:  PrintCookedTimes(&wd, 10); break;
          default:          PrintRawTimes(&wd);        break;
        }
        printf(" %13.7f\n", wd.smprate);
    }

    exit(0);
}

/* Revision History
 *
 * $Log: wdtime.c,v $
 * Revision 1.1  2014/04/15 21:00:12  dechavez
 * initial release
 *
 */
