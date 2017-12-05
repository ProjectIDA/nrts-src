#pragma ident "$Id: wdchk.c,v 1.4 2015/12/08 20:43:40 dechavez Exp $"
/*======================================================================
 *
 *  Given a wfdisc and time window investigate whether a specified
 *  time window is all or partially present.
 *  
 *====================================================================*/
#include "util.h"
#include "cssio.h"

enum {ALL, SOME};

struct l_list {
    int  flag;
    INT32 key;
    struct l_list *next;
};

struct ch_list {
    int  flag;
    char *key;
    struct ch_list *next;
};

struct selection {
    struct l_list  *date;
    struct ch_list *sta;
    struct ch_list *chan;
    double beg, end, dur;
};

int Match = FALSE;

static BOOL lmatch(INT32 test, struct l_list *list)
{
struct l_list *crnt;

    crnt = list->next;
    while (crnt != NULL) {
        if (test == crnt->key) return TRUE;
        crnt = crnt->next;
    }

    return FALSE;
}

static int cmatch(char *test, struct ch_list *list)
{
struct ch_list *crnt;

    util_ucase(test);

    crnt = list->next;
    while (crnt != NULL) {
        util_ucase(crnt->key);
        if (strcmp(test, crnt->key) == 0) return TRUE;
        crnt = crnt->next;
    }

    return FALSE;
}

static void check(FILE *wf, struct selection *want)
{
int ignore, trace = 0;
struct cssio_wfdisc wfdisc;
double beg, end, dur, percent, left, rite;

    while (rwfdrec(wf, &wfdisc) == 0) {
        ++trace;

        ignore = (
            (want->date->flag != ALL && !lmatch(wfdisc.jdate, want->date)) ||
            (want->sta->flag  != ALL && !cmatch(wfdisc.sta,   want->sta )) ||
            (want->chan->flag != ALL && !cmatch(wfdisc.chan,  want->chan)) ||
            (wfdisc.nsamp <= 0)
        );

        if (!ignore) {
            beg = wfdisc.time;
            end = wfdisc.endtime;
            dur = end - beg;
            if (end < want->beg || beg > want->end) {
                percent = 0.0;
            } else if (want->beg >= beg && want->end <= end) {
                percent = 100.0;
            } else {
                left = (want->beg > beg) ? want->beg : beg;
                rite = (want->end < end) ? want->end : end;
                percent = ((rite - left) / want->dur) * 100.0;
            }
            if (percent > 0.0) {    
                printf("%3.3d: %s/%s ", trace, wfdisc.dir, wfdisc.dfile);
                printf("(%s - ",   util_dttostr(beg, 0));
                printf("%s) -> ", util_dttostr(end, 0));
                printf("%7.3lf %%\n", percent);
                Match = TRUE;
            }
        }
    }
}

static void add_char(struct ch_list *head, char *token)
{
struct ch_list *new, *crnt;

    if ((new = (struct ch_list *) malloc(sizeof(struct ch_list))) == NULL) {
        perror("wdchk: malloc");
        exit(1);
    }

    new->key  = token;
    new->next = NULL;

    crnt = head;
    while (crnt->next != NULL) crnt = crnt->next;
    crnt->next = new;

}

static void ch_reinit( struct ch_list *head, char *string, int  offset)
{
char *token;
struct ch_list *crnt, *next;

    crnt = head->next;
    while (crnt != NULL) {
        next = crnt->next;
        free(crnt->key);
        free(crnt);
        crnt = next;
    }
    head->flag = SOME;

    if ((token = strtok(string+offset, ",")) == NULL) {
        fprintf(stderr,"wdchk: syntax error in `%s`\n", string);
        exit(1);
    }

    add_char(head, token);

    while ((token = strtok(NULL, ",")) != NULL) add_char(head, token);
}

static void add_INT32(struct l_list *head, char *token)
{
struct l_list *new, *crnt;

    if ((new = (struct l_list *) malloc(sizeof(struct l_list))) == NULL) {
        perror("wdchk: malloc");
        exit(1);
    }

    new->key  = atol(token);
    new->next = NULL;

    crnt = head;
    while (crnt->next != NULL) crnt = crnt->next;
    crnt->next = new;
}

static void l_reinit(struct l_list *head, char *string, int  offset)
{
char *token;
struct l_list *crnt, *next;

    crnt = head->next;
    while (crnt != NULL) {
        next = crnt->next;
        free(crnt);
        crnt = next;
    }
    head->flag = SOME;

    if ((token = strtok(string+offset, ",")) == NULL) {
        fprintf(stderr,"wdchk: syntax error in `%s`\n", string);
        exit(1);
    }

    add_INT32(head, token);

    while ((token = strtok(NULL, ",")) != NULL) add_INT32(head, token);
}

int main(int argc, char **argv)
{
FILE *wf;
int i;
char *fname;
struct l_list head1;
struct ch_list head2, head3;
struct selection want;

    want.date = &head1;
    want.date->flag = ALL;
    want.date->next = NULL;

    want.sta  = &head2;
    want.sta->flag  = ALL;
    want.sta->next  = NULL;

    want.chan = &head3;
    want.chan->flag = ALL;
    want.chan->next = NULL;

    if (argc < 4) {
        fprintf(stderr,"usage: wdchk beg end ");
        fprintf(stderr,"[sta=sta1,sta2,... chan=ch1,ch2,...] ");
        fprintf(stderr,"wfdisc1 wfdisc2 ...\n");
        exit(1);
    }

    want.beg = util_attodt(argv[1]);
    want.end = util_attodt(argv[2]);
    want.dur = want.end - want.beg;

    if (want.beg >= want.end) {
        fprintf(stderr,"wdchk: beg >= end\n");
        exit(1);
    }

    printf("Desired window: %s ", util_dttostr(want.beg, 0));
    printf("until %s\n", util_dttostr(want.end, 0));

    for (i = 3; i < argc; i++) {
        if (strncmp(argv[i], "sta=", strlen("sta=")) == 0) {
            ch_reinit(want.sta, argv[i], strlen("sta="));
        } else if (strncmp(argv[i], "chan=", strlen("chan=")) == 0) {
            ch_reinit(want.chan, argv[i], strlen("chan="));
        } else {
            fname = argv[i];
            if ((wf = fopen(fname, "r")) == NULL) {
                fprintf(stderr,"wdchk: ");
                perror(fname);
                fprintf(stderr,"Execution continuing.\n");
            } else {
                Match = FALSE;
                printf("\nWfdisc file '%s':\n", fname);
                check(wf, &want);
                if (!Match) printf("No data for this window found.\n");
            }
        }
    }

    exit(0);
}

/* Revision History
 *
 * $Log: wdchk.c,v $
 * Revision 1.4  2015/12/08 20:43:40  dechavez
 * various minor tweaks to get clean Mac OS X builds
 *
 * Revision 1.3  2011/03/17 20:47:38  dechavez
 * use the new CSSIO_, cssio_, prefixed names from libcssio 2.2.0
 *
 * Revision 1.2  2003/12/10 06:15:38  dechavez
 * various superficial changes in order to calm solaris cc
 *
 * Revision 1.1.1.1  2000/02/08 20:19:57  dec
 * import existing IDA/NRTS sources
 *
 */
