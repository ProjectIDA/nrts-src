#pragma ident "$Id: mon.c,v 1.3 2014/08/28 21:27:36 dechavez Exp $"
/*======================================================================
 *
 *  Monitor ISI and NRTS disk loops
 *
 *====================================================================*/
#include "isi/dl.h"
#include "nrts.h"

#define BUFLEN 256
static INT64 SetTimeStamps(ISI_DL *dl, INT64 *uptime, INT64 *update)
{
INT64 now;

    now = utilTimeStamp();

    if (dl->sys->state != ISI_INACTIVE) {
        *uptime = now - dl->sys->tstamp.start;
    } else {
        *uptime = UTIL_UNDEFINED_TIMESTAMP;
    }

    if (dl->sys->tstamp.write != UTIL_UNDEFINED_TIMESTAMP) {
        *update = now - dl->sys->tstamp.write;
    } else {
        *update = UTIL_UNDEFINED_TIMESTAMP;
    }

    return now;
}

static void UpdateISI(WINDOW *win, ISI_DL *dl, int row, int col)
{
INT64 uptime, update;
int i, parent, buflen = BUFLEN;
char buf[buflen];

    if (dl == NULL) {
        for (i = 0; i < 14; i++) {
            wmove(win, row+i, col);
            wclrtoeol(win);
        }
        return;
    }

    SetTimeStamps(dl, &uptime, &update);
    parent = (dl->sys->parent != 0 && kill(dl->sys->parent, 0) == 0) ? dl->sys->parent : 0;

    mvwprintw(win, row++, col, "%s [ %d ]        ", dl->sys->site, parent);
    mvwprintw(win, row++, col, "%s ",               utilTimeString(uptime, 108, buf, buflen));
    mvwprintw(win, row++, col, "%s ",               utilTimeString(update, 108, buf, buflen));
    mvwprintw(win, row, col, "                            ");
    mvwprintw(win, row++, col, "%llu        ",      dl->sys->count);
    mvwprintw(win, row++, col, "%s",                isiSeqnoString2(&dl->sys->seqno, buf));
    if (dl->tee.disabled) {
        mvwprintw(win, row++, col, "disabled                ");
    } else {
        mvwprintw(win, row++, col, "%08lx%016llx", dl->sys->seqno.signature, (dl->sys->seqno.counter / dl->glob->trecs) * dl->glob->trecs);
    }
    mvwprintw(win, row, col, "                            ");
    mvwprintw(win, row++, col, "%s",   isidlStateString(dl->sys->state));

    ++row;
    mvwprintw(win, row++, col, "%s        ",   dl->base);
    mvwprintw(win, row++, col, "%s        ",   utilLttostr(dl->sys->seqno.signature, 0, buf));
    mvwprintw(win, row++, col, "%lu        ",  dl->sys->maxlen);
    mvwprintw(win, row++, col, "%lu        ",  dl->sys->hdrlen);
    mvwprintw(win, row++, col, "%lu        ",  dl->sys->numpkt);
    mvwprintw(win, row++, col, "%lu        ",  dl->glob->trecs);

}

void isidlMonUpdateISI(WINDOW *win, ISI_MON *mon, int row)
{
    UpdateISI(win, mon->isi.show[0], row, 17);
    UpdateISI(win, mon->isi.show[1], row, 52);
}

void isidlMonStaticISI(WINDOW *win, int row)
{
    mvwprintw(win, row++, 0, "loop [ pid ]");
    mvwprintw(win, row++, 0, "uptime");
    mvwprintw(win, row++, 0, "last update");
    mvwprintw(win, row++, 0, "pkts written");
    mvwprintw(win, row++, 0, "crnt seqno");
    mvwprintw(win, row++, 0, "crnt teefile");
    mvwprintw(win, row++, 0, "state");

    ++row;
    mvwprintw(win, row++, 0, "base dir");
    mvwprintw(win, row++, 0, "dl build date");
    mvwprintw(win, row++, 0, "max pktlen");
    mvwprintw(win, row++, 0, "ISI hdrlen");
    mvwprintw(win, row++, 0, "dl len (pkts)");
    mvwprintw(win, row++, 0, "recs/teefile");
}

void isidlSetMonISI(ISI_MON *mon, char *target)
{
int i;
char target330[MAXPATHLEN+1];

    for (i = 0; i < 2; i++) {
        mon->isi.show[i] = NULL;
        mon->isi.index[i] = -1;
    }

    for (i = 0; mon->isi.show[0] == NULL && i < mon->isi.ndl; i++) {
        if (strcmp(mon->isi.dl[i]->sys->site, target) == 0) {
            mon->isi.show[0] = mon->isi.dl[i];
            mon->isi.index[0] = i;
        }
    }

    if (mon->isi.show[0] == NULL) {
        mon->isi.show[0] = mon->isi.dl[0];
        mon->isi.index[0] = 0;
    } else {
        sprintf(target330, "%s330", mon->isi.show[0]->sys->site);
        for (i = 0; mon->isi.show[1] == NULL && i < mon->isi.ndl; i++) {
            if (strcmp(mon->isi.dl[i]->sys->site, target330) == 0) {
                mon->isi.show[1] = mon->isi.dl[i];
                mon->isi.index[1] = i;
            }
        }
    }
}

void isidlIncrMonISI(ISI_MON *mon)
{
int index;

    if (mon->isi.index[1] < 0) {
        if ((index = ++mon->isi.index[0]) == mon->isi.ndl) index = 0;
    } else if ((index = ++mon->isi.index[1]) == mon->isi.ndl) {
        index = 0;
    }
    isidlSetMonISI(mon, mon->isi.dl[index]->sys->site);
}

void isidlDecrMonISI(ISI_MON *mon)
{
    if (--mon->isi.index[0] < 0) mon->isi.index[0] = mon->isi.ndl - 1;
    isidlSetMonISI(mon, mon->isi.dl[mon->isi.index[0]]->sys->site);
}


static void UpdateNRTS(WINDOW *win, int row, time_t now, NRTS_CHN *chn)
{
char *status;
char begstr[BUFLEN];
char endstr[BUFLEN];
char updstr[BUFLEN];

    if (chn->beg != (double) NRTS_UNDEFINED_TIMESTAMP) {
        sprintf(begstr, "%s", utilDttostr(chn->beg, 0, NULL));
    } else {
        sprintf(begstr, "                     ");
    }

    if (chn->end != (double) NRTS_UNDEFINED_TIMESTAMP) {
        sprintf(endstr, "%s", utilDttostr(chn->end, 0, NULL));
    } else {
        sprintf(endstr, "                     ");
    }

    if (chn->tread != NRTS_UNDEFINED_TIMESTAMP) {
        sprintf(updstr, "%s", utilLttostr(now - chn->tread, 8, NULL));
    } else {
        sprintf(updstr, "            ");
    }

    switch (chn->status) {
      case NRTS_IDLE:    status = "idle    "; break;
      case NRTS_STABLE:  status = "stable  "; break;
      case NRTS_BUSY1:   status = "busy (1)"; break;
      case NRTS_BUSY2:   status = "busy (2)"; break;
      case NRTS_BUSY3:   status = "busy (3)"; break;
      case NRTS_CORRUPT: status = "CORRUPT!"; break;
      default:           status = "????????"; break;
    }

    mvwprintw(win, row,  0, "%s", chn->name);
    mvwprintw(win, row,  6, "%s", begstr);
    mvwprintw(win, row, 28, "%s", endstr);
    mvwprintw(win, row, 50, "%6d", chn->nseg);
    mvwprintw(win, row, 59, "%s", status);
    mvwprintw(win, row, 68, "%s", updstr);
}

void isidlMonUpdateNRTS(WINDOW *win, ISI_MON *mon, int start)
{
int i, parent, row, remain, end;
time_t now, uptime;
NRTS_DL *nrts;
#define NUM_HEADER_ROWS 3
#define MAX_CHN         15

    row = start;

    nrts = mon->nrts.show;
    parent = (nrts->sys->pid != 0 && kill(nrts->sys->pid, 0) == 0) ? nrts->sys->pid : 0;
    
    mvwprintw(win, start, 6, "%s", nrts->sys->sta[0].name);
    wprintw(win, " [ %d ]              ", parent);

    now = time(NULL);
    uptime = now - nrts->sys->start;
    mvwprintw(win, start, 63, "%17s", nrts_latency(uptime));

    row = start + NUM_HEADER_ROWS;
    for (i = 0; i < MAX_CHN; i++) {
        wmove(win, row+i, 0);
        wclrtoeol(win);
    }

    remain = nrts->sys->sta[0].nchn - mon->nrts.beg;
    end = mon->nrts.beg + (remain > MAX_CHN ? MAX_CHN : remain);

    row = start + NUM_HEADER_ROWS;
    for (i = mon->nrts.beg; i < end; i++) {
        UpdateNRTS(win, row++, now, &nrts->sys->sta[0].chn[i]);
        mon->nrts.end = i;
    }
}

void isidlMonStaticNRTS(WINDOW *win, int row)
{
    mvwprintw(win, row, 0, "loop: ");
    mvwprintw(win, row++, 55, "Uptime: ");

    ++row;
    mvwprintw(win, row++, 0, " chan     earliest datum         latest datum       nseg   status      update");
}

void isidlSetMonNRTS(ISI_MON *mon, char *target)
{
int i;

    mon->nrts.show = NULL;

    for (i = 0; i < mon->nrts.ndl; i++) {
        if (strcmp(mon->nrts.dl[i]->sys->sta[0].name, target) == 0) {
            mon->nrts.show = mon->nrts.dl[i];
            mon->nrts.index = i;
            mon->nrts.beg   = 0;
            mon->nrts.nchn = mon->nrts.show->sys->sta[0].nchn;
            return;
        }
    }
    mon->nrts.show = mon->nrts.dl[0];
    mon->nrts.index = 0;
    mon->nrts.beg   = 0;
    mon->nrts.nchn  = mon->nrts.show->sys->sta[0].nchn;
}

void isidlIncrMonNRTSchn(ISI_MON *mon)
{
    mon->nrts.beg = ++mon->nrts.end;
    if (mon->nrts.beg >= mon->nrts.nchn) mon->nrts.beg = 0;
}

void isidlIncrMonNRTS(ISI_MON *mon)
{
    if (++mon->nrts.index == mon->nrts.ndl) mon->nrts.index = 0;
    mon->nrts.show = mon->nrts.dl[mon->nrts.index];
    mon->nrts.nchn = mon->nrts.show->sys->sta[0].nchn;
    mon->nrts.beg  = 0;
}

void isidlDecrMonNRTS(ISI_MON *mon)
{
    if (--mon->nrts.index < 0) mon->nrts.index = mon->nrts.ndl - 1;
    mon->nrts.show = mon->nrts.dl[mon->nrts.index];
    mon->nrts.nchn = mon->nrts.show->sys->sta[0].nchn;
    mon->nrts.beg  = 0;
}

BOOL isidlInitMON(char *myname, ISI_MON *mon, char *dbspec, LOGIO *lp)
{
int i;
ISI_DL *isi;
NRTS_DL *nrts;
static UINT32 options = ISI_OPTION_NONE;
static char *fid = "isidlInitMON";

    if (myname == NULL || mon == NULL) {
        errno = EINVAL;
        return FALSE;
    }

    if (!isidlSetGlobalParameters(dbspec, myname, &mon->glob)) return FALSE;

    if ((mon->master = isidlOpenDiskLoopSet(NULL, &mon->glob, lp, ISI_RDONLY, options)) == NULL) return FALSE;

    if (mon->master->ndl < 1) {
        errno = ENOENT;
        return FALSE;
    }

    mon->isi.dl = mon->master->dl;
    mon->isi.ndl = mon->master->ndl;

    if ((mon->nrts.dl = (NRTS_DL **) malloc(sizeof(NRTS_DL *) * mon->isi.ndl)) == NULL) return FALSE;
    mon->nrts.ndl = 0;

    for (i = 0; i < mon->isi.ndl; i++) {
        isi = mon->isi.dl[i];
        if (isi->nrts != NULL) mon->nrts.dl[mon->nrts.ndl++] = isi->nrts;
    }

    return TRUE;
}

/* Revision History
 *
 * $Log: mon.c,v $
 * Revision 1.3  2014/08/28 21:27:36  dechavez
 * support for new options argument to isidlOpenDiskLoopSet()
 *
 * Revision 1.2  2011/07/28 20:58:31  dechavez
 * removed debug() calls, fixed +++ typo and added uptime to NRTS display.
 *
 * Revision 1.1  2011/07/27 23:39:43  dechavez
 * created
 *
 */
