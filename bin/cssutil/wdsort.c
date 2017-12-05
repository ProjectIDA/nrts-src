#pragma ident "$Id: wdsort.c,v 1.1 2014/04/15 20:59:51 dechavez Exp $"
/*======================================================================
 *
 *  Sort wfdiscs records. With no options the records are sorted into
 *  station/channel/time order, with alhpanumerical sorts on the station
 *  and channel names and numerical sort on the time.  With the -t option
 *  the records are sorted in time/station/channel order.  In either case
 *  the -r option will cause the channel names to be sorted in reverse
 *  order.
 *
 *====================================================================*/
#include "cssio.h"

#define OPTION_DEFAULT_ORDER 0x00
#define OPTION_TIME_ORDER    0x01
#define OPTION_REVERSE_CHAN  0x02

static void help(char *myname)
{
    fprintf(stderr, "usage: %s [ -r -t ] < CSS_wfdisc\n", myname);
    fprintf(stderr, "\n");
    fprintf(stderr, "Options: -t -> sort by time/sta/chan\n");
    fprintf(stderr, "         -r => sort channel names in reverse order\n");
    exit(0);
}

static int SortByStaChanTime(const void *aptr, const void *bptr)
{
int result;
struct cssio_wfdisc *a, *b;
        
    a = (struct cssio_wfdisc *) aptr;
    b = (struct cssio_wfdisc *) bptr;

/* Sort by station */

    if ((result = strcmp(a->sta, b->sta)) != 0) return result;

/* Sort by channel */

    if ((result = strcmp(a->chan, b->chan)) != 0) return result;

/* Sort by time */

    if (a->time == b->time) return 0;
    return (a->time < b->time) ? -1 : 1;
}

static int SortByTimeStaChan(const void *aptr, const void *bptr)
{
int result;
struct cssio_wfdisc *a, *b;
        
    a = (struct cssio_wfdisc *) aptr;
    b = (struct cssio_wfdisc *) bptr;

/* Sort by time */

    if (a->time != b->time) return (a->time < b->time) ? -1 : 1;

/* Sort by station */

    if ((result = strcmp(a->sta, b->sta)) != 0) return result;

/* Sort by channel */

    return strcmp(a->chan, b->chan);
}

static int SortByStaNachTime(const void *aptr, const void *bptr)
{
int result;
struct cssio_wfdisc *a, *b;
        
    a = (struct cssio_wfdisc *) aptr;
    b = (struct cssio_wfdisc *) bptr;

/* Sort by station */

    if ((result = strcmp(a->sta, b->sta)) != 0) return result;

/* Sort by reverse channel */

    if ((result = strcmp(b->chan, a->chan)) != 0) return result;

/* Sort by time */

    if (a->time == b->time) return 0;
    return (a->time < b->time) ? -1 : 1;
}

static int SortByTimeStaNach(const void *aptr, const void *bptr)
{
int result;
struct cssio_wfdisc *a, *b;
        
    a = (struct cssio_wfdisc *) aptr;
    b = (struct cssio_wfdisc *) bptr;

/* Sort by time */

    if (a->time != b->time) return (a->time < b->time) ? -1 : 1;

/* Sort by station */

    if ((result = strcmp(a->sta, b->sta)) != 0) return result;

/* Sort by reverse channel */

    return strcmp(b->chan, a->chan);
}

int main(int argc, char **argv)
{
int i, nrec, options = 0;
WFDISC *wfdisc;

    for (i = 1; i < argc; i++) {
        if (strcasecmp(argv[i], "-r") == 0) {
            options |= OPTION_REVERSE_CHAN;
        } else if (strcasecmp(argv[i], "-t") == 0) {
            options |= OPTION_TIME_ORDER;
        } else if (strcasecmp(argv[i], "-h") == 0 || strcasecmp(argv[i], "-help") == 0) {
            help(argv[0]);
        } else {
            fprintf(stderr, "unrecognized option '%s'\n", argv[i]);
            help(argv[0]);
        }
    }

    if ((nrec = rwfdisc(stdin, &wfdisc)) <= 0) {
        fprintf(stderr,"%s: bad or empty wfdisc file\n", argv[0]);
        exit(2);
    }

    if (options == 0) {
        qsort(wfdisc, nrec, sizeof(WFDISC), SortByStaChanTime);
    } else if (options == OPTION_REVERSE_CHAN) {
        qsort(wfdisc, nrec, sizeof(WFDISC), SortByStaNachTime);
    } else if (options == OPTION_TIME_ORDER) {
        qsort(wfdisc, nrec, sizeof(WFDISC), SortByTimeStaChan);
    } else if (options == (OPTION_TIME_ORDER | OPTION_REVERSE_CHAN)) {
        qsort(wfdisc, nrec, sizeof(WFDISC), SortByTimeStaNach);
    }

    for (i = 0; i < nrec; i++) wwfdisc(stdout, &wfdisc[i]);
}

/* Revision History
 *
 * $Log: wdsort.c,v $
 * Revision 1.1  2014/04/15 20:59:51  dechavez
 * initial release
 *
 */
