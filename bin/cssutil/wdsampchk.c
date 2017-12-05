#pragma ident "$Id: wdsampchk.c,v 1.1 2016/11/16 22:01:15 dechavez Exp $"
/*======================================================================
 *
 *  Check for nsamp vs beg/endtime consistency for all channels present
 *  in a wfdsic file.
 *
 *====================================================================*/
#include "util.h"
#include "cssio.h"

typedef struct {
    char sta[CSSIO_STA_LEN+1];
    char chan[CSSIO_CHAN_LEN+1];
    REAL64 smprate;
    REAL64 time;
    REAL64 endtime;
    INT32 nsamp;
    INT32 nseg;
} INFO;

static void AddInfo(LNKLST *list, struct cssio_wfdisc *wfdisc)
{
INFO new;

    strncpy(new.sta,  wfdisc->sta,  CSSIO_STA_LEN+1 );
    strncpy(new.chan, wfdisc->chan, CSSIO_CHAN_LEN+1);
    new.smprate = (REAL64) wfdisc->smprate;
    new.time    = (REAL64) wfdisc->time;
    new.endtime = (REAL64) wfdisc->endtime;
    new.nsamp   = (INT32)  wfdisc->nsamp;
    new.nseg    = 1;

    if (!listAppend(list, &new, sizeof(INFO))) {
        perror("listAppend");
        exit(100);
    }
}

static void UpdateInfo(INFO *info, struct cssio_wfdisc *wfdisc)
{
    if (info->time > wfdisc->time) info->time = wfdisc->time;
    if (info->endtime < wfdisc->endtime) info->endtime = wfdisc->endtime;
    info->nsamp += wfdisc->nsamp;
    ++info->nseg;
}

static BOOL StreamMatch(INFO *info, struct cssio_wfdisc *wfdisc)
{
    if (strcmp(info->sta, wfdisc->sta) != 0) return FALSE;
    if (strcmp(info->chan, wfdisc->chan) != 0) return FALSE;
    if (info->smprate != wfdisc->smprate) return FALSE;
    return TRUE;
}

static INFO *LookupInfo(LNKLST *list, struct cssio_wfdisc *wfdisc)
{
INFO *info;
LNKLST_NODE *crnt;

    crnt = listFirstNode(list);
    while (crnt != NULL) {
        info = (INFO *) crnt->payload;
        if (StreamMatch(info, wfdisc)) return info;
        crnt = listNextNode(crnt);
    }

    return NULL;
}

static int WfdiscCompare(const void *aptr, const void *bptr)
{
int result;
struct cssio_wfdisc *a, *b;

    a = (struct cssio_wfdisc *) aptr;
    b = (struct cssio_wfdisc *) bptr;

    if ((result = strcmp(a->sta, b->sta)) != 0) return result;
    if ((result = strcmp(a->chan, b->chan)) != 0) return result;
    if (a->chan > b->chan) return 1;
    if (a->chan < b->chan) return -1;
    return 0;
}

static void CheckInfo(INFO *info)
{
REAL64 duration;
int expected, difference;

    duration = info->endtime - info->time;
    expected = (INT32) (duration * info->smprate) + 1;
    difference = expected - info->nsamp;

    printf("%4s", info->sta);
    printf(" %5s", info->chan);
    printf(" %6.2lf", info->smprate);
    printf(" %4d", info->nseg);
    printf(" %16.5lf %s", info->time, utilDttostr(info->time, 0, NULL));
    printf(" %16.5lf %s", info->endtime, utilDttostr(info->endtime, 0, NULL));
    printf(" %10d", expected);
    printf(" %10d", info->nsamp);
    printf(" %10d", difference);
    printf(" %10.3lf", difference / info->smprate);
    printf("\n");
}

static void process(FILE *fp)
{
INFO *info;
LNKLST head;
char tmpbuf[64];
INT32 i, nrec = 0;
LNKLST_NODE *crnt;
struct cssio_wfdisc *wfdisc = NULL;
static char *header = 
"                                                                                                          total samples           discrepancy\n"
" sta  chan   rate nseg -------- time of first sample -------- -------- time of first sample --------   expected     actual    samples    seconds";

    if ((nrec = rwfdisc(fp, &wfdisc)) < 0) {
        fprintf(stderr, "ERROR: rwfdisc error %d: %s\n", nrec, strerror(errno));
        exit(1);
    }

    qsort((void *) wfdisc, nrec, sizeof(struct cssio_wfdisc), WfdiscCompare);

    if (!listInit(&head)) {
        perror("ERROR: listInit");
        exit(2);
    }

    for (i = 0; i < nrec; i++) {
        if ((info = LookupInfo(&head, &wfdisc[i])) == NULL) {
            AddInfo(&head, &wfdisc[i]);
        } else {
            UpdateInfo(info, &wfdisc[i]);
        }
    }

    printf("%s\n", header);

    crnt = listFirstNode(&head);
    while (crnt != NULL) {
        CheckInfo((INFO *) crnt->payload);
        crnt = listNextNode(crnt);
    }
}

int main(int argc, char **argv)
{
FILE *fp;
char *fname;
int i;

    if (argc > 1) {
        for (i = 1; i < argc; i++) {
            fname = argv[i];
            if ((fp = fopen(fname, "r")) == NULL) {
                perror(fname);
            } else {
                printf("File: %s\n", fname);
                process(fp);
                fclose(fp);
            }
        }
    } else {
        process(stdin);
    }
}

/* Revision History
 *
 * $Log: wdsampchk.c,v $
 * Revision 1.1  2016/11/16 22:01:15  dechavez
 * initial release
 *
 */
