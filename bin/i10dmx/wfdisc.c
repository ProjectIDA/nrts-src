#pragma ident "$Id: wfdisc.c,v 1.20 2015/12/07 19:55:33 dechavez Exp $"
/*======================================================================
 *
 *  Wfdisc related routines.
 *
 *----------------------------------------------------------------------
 *
 *  get_wfdisc()
 *
 *  Get the most recent wfdisc list entry which corresponds to the
 *  given output file.  If there is not already a wfdisc record in 
 *  the list, one is created and added to it.  Returns a pointer to 
 *  the list element if successful, otherwise aborts.
 *
 *  If a time stamp increment error was detected then we log it and
 *  create a new wfdisc record, regardless of the size of the error.
 *
 *----------------------------------------------------------------------
 *
 *  mkwfdisc()
 *
 *  Create a new wfdisc record, and add it to the list.
 *
 *----------------------------------------------------------------------
 *
 *  fill_wfdisc()
 *
 *  Fill wfdisc field of wflist element.
 *
 *====================================================================*/
#include <stdio.h>
#include <errno.h>
#include <assert.h>
#include "i10dmx.h"
#include "isi.h"
#include "isi/db.h"

extern char *Buffer;

static struct wflist listhead;

static struct {
    IDA10_TSHDR hdr;
    UINT32 recno;
    UINT32 count;
} save[IDA10_MAXSTREAMS];

static DBIO *db = NULL;

static struct counter *count;
static int cssformat;
static int maxdur = 0;

int needToSwap;

static void UpdateHistory(int index, IDA10_TSHDR *hdr, UINT32 recno)
{
    ++save[index].count;
    save[index].recno = recno;
    save[index].hdr = *hdr;
}

static void LogFreshClock(IDA10_TSHDR *hdr, int index, UINT32 recno)
{
    sprintf(Buffer, "Stream %s clock became valid between recs %u and %u, time break forced",
        hdr->cname, save[index].recno, recno
    );
    logmsg(Buffer);
}

static void LogTimeTear(
    int sign, UINT64 err, IDA10_TSHDR *hdr, int index, UINT32 recno,  REAL64 errsec, REAL64 errsmp
){
    sprintf(Buffer, "Time jump of %.9lf sec (%.9lf samples), stream %s between recs %u and %u",
        errsec, errsmp, hdr->cname, save[index].recno, recno
    );
    logmsg(Buffer);
}

static void LogSintChange(IDA10_TSHDR *hdr, int index, UINT32 recno)
{
    sprintf(Buffer, "*** WARNING: stream %s sample interval changed from %.3lf to %.3lf between recs %u and %u",
        hdr->cname, save[index].hdr.sint, hdr->sint,
        save[index].recno, recno
    );
    logmsg(Buffer);
}

static BOOL IncrError(IDA10_TSHDR *hdr)
{
int i, status, sign;
UINT64 err;
REAL64 errsec, errsmp;
static BOOL retval, first = TRUE;

    if (first) {
        for (i = 0; i < IDA10_MAXSTREAMS; i++) save[i].count = 0;
        first = FALSE;
    }

    for (i = 0; i < IDA10_MAXSTREAMS; i++) {

        if (save[i].count == 0) {
            save[i].hdr = *hdr;
            save[i].recno = count->rec;
            ++save[i].count;
            return FALSE;
        } else if (strcmp(save[i].hdr.sname, hdr->sname) == 0 && strcmp(save[i].hdr.cname, hdr->cname) == 0) {
            status = ida10TtagIncrErr(&save[i].hdr, hdr, &sign, &err);
            switch (status) {
              case IDA10_OK:
                if (hdr->incerr.fresh) {
                    LogFreshClock(hdr, i, count->rec);
                    retval = TRUE;
                } else if (hdr->incerr.ok) {
                    retval = FALSE;
                } else if (err > hdr->nsint / 2) { /* break record for tears of more than 1/2 the sample interval */
                    errsec = (REAL64) (err) / (REAL64) NANOSEC_PER_SEC;
                    errsmp = errsec / hdr->sint;
                    if (sign < 0) errsec = -errsec;
                    LogTimeTear(sign, err, hdr, i, count->rec, errsec, errsmp);
                    retval = TRUE;
                } else {
                    retval = FALSE;
                }
                break;
              case IDA10_ESRATE:
                LogSintChange(hdr, i, count->rec);
                retval = TRUE;
                break;
              default:
                fprintf(stderr, "\n*** ERROR: ida10TtagIncrErr: error %d\n",
                    status
                );
                exit(1);
            }
            UpdateHistory(i, hdr, count->rec);
            return retval;
        }
    }

    fprintf(stderr, "\nIncrease IDA10_MAXSTREAMS (%d)\n", IDA10_MAXSTREAMS);
    exit(1);
}

static void fill_wfdisc(struct wflist *new, BufferedStream  *fp, IDA10_TSHDR *hdr)
{
ISI_STREAM_NAME name;
static ISI_INST inst = ISI_UNDEFINED_INST;
int yr, da, hr, mn, sc, ms;

    strcpy(name.sta, sname(hdr->sname));
    strcpy(name.chnloc, hdr->cname);

/*  Initialize the wfdisc  */

    new->wfdisc = wfdisc28_null;

    util_tsplit(hdr->tofs, &yr, &da, &hr, &mn, &sc, &ms);
    new->wfdisc.date   = (1000L * (INT32) yr) + (INT32) da;
    new->wfdisc.time   = hdr->tofs;
    new->wfdisc.nsamp  = 0;
    new->wfdisc.smprat = (float)hdr->srate;
    strcpy(new->wfdisc.dattyp, ida10DataTypeString(hdr));
    strcpy(new->wfdisc.dir,  crnt_dname());
    strcpy(new->wfdisc.file, crnt_fname());
    strcpy(new->wfdisc.sta, sname(hdr->sname));
    sprintf(new->wfdisc.chan, "%s", hdr->cname);
    new->wfdisc.segtyp = 'o';
    new->wfdisc.foff = fp->nTotalBytes;
    if (db != NULL) {
        isidbLookupInst(db, &name, (UINT32) hdr->tofs, &inst);
        new->wfdisc.calib = inst.calib;
        new->wfdisc.calper = inst.calper;
        strcpy(new->wfdisc.instyp, inst.type);
    }
}

static struct wflist *mkwfdisc(char *path, BufferedStream  *fp, IDA10_TSHDR *hdr)
{
struct wflist *new, *crnt, *next;

/*  Create and initialize new element  */

    if ((new = (struct wflist *) malloc(sizeof(struct wflist))) == NULL) {
        sprintf(Buffer,"mkwfdisc: malloc: %s", syserrmsg(errno));
        logmsg(Buffer);
        die(ABORT);
    }

    strcpy(new->name, path);
    fill_wfdisc(new, fp, hdr);

/*  Add it to the list  */

    crnt = &listhead;
    next = crnt->next;
    while (next != NULL) {
        crnt = crnt->next;
        next = next->next;
    }    

    crnt->next = new;
    new->next  = NULL;

/*  Done, return pointer to new element  */

    return new;
}    

void init_wfdisc(int format, struct counter *countptr, char *dbspec, int maxsec)
{
    maxdur = maxsec;
    cssformat = format;
    needToSwap = (util_order() != BIG_ENDIAN_ORDER);
    listhead.next = NULL;
    count = countptr;
    if (dbspec != NULL) {
        if ((db = dbioOpen(dbspec, NULL)) == NULL) {
            sprintf(Buffer, "warning: %s database unavailable, will use default (null) calib/calper\n", dbspec);
            logmsg(Buffer);
        }
    } else {
        sprintf(Buffer, "Warning: no database specified, will use default (null) calib/calper/inst\n");
        logmsg(Buffer);
        fprintf(stderr, "%s", Buffer);
    }
}

void wrt_wfdisc()
{
FILE *fp;
char fname[MAXPATHLEN];
struct wflist *crnt;
struct cssio_wfdisc wfdisc30;
static char *prefix = "css";

    sprintf(fname,"%s%c%s.wfdisc", basedir(), PATH_DELIMITER, prefix);

    if ((fp = fopen(fname, "w")) == NULL) {
        sprintf(Buffer,"wrt_wfdisc: %s: %s", fname, syserrmsg(errno));
        logmsg(Buffer);
        return;
    }
#ifdef WIN32
/***********************************************************************/
/*  Do not remove!                                                     */
/*  WinIdadmx uses this output to get the wfdisc file name from idadmx */
/***********************************************************************/
    printf("\nOutput file name:%s\n", fname);
#endif

    crnt = listhead.next;
    while (crnt != NULL) {
        if (crnt->wfdisc.nsamp != 0) {
            if (cssformat == CSS_28) {
                wwfdisc28(fp, &crnt->wfdisc);
            } else {
                wf28to30(&wfdisc30, &crnt->wfdisc);
                wwfdisc(fp, &wfdisc30);
            }
        }
        crnt = crnt->next;
    }
    fclose(fp);

    //cssioWriteDescriptor(basedir(), prefix);
}

static int duration(int nsamp, double srate)
{
int secs;

    if (nsamp <= 0) return 0;

    secs = (int) ((double) nsamp / srate);

    return secs;
}

struct wflist *get_wfdisc(char *path, BufferedStream *fp, IDA10_TSHDR *hdr)
{
struct wflist *crnt, *match;
BOOL NeedNewRecord;
int TotalDuration, RecordDuration;

/*  See if time incremented OK between records for this stream */

    NeedNewRecord = IncrError(hdr);

/*  If no time tear, locate the current wfdisc for this stream and check for duration threshold */

    if (!NeedNewRecord) {
        match = NULL;
        crnt  = listhead.next;
        while (crnt != NULL) {
            if (strcmp(crnt->name, path) == 0) match = crnt;
            crnt = crnt->next;
        }
        if (match == NULL) {
            NeedNewRecord = TRUE;
        } else if (maxdur > 0) {
            TotalDuration = duration(match->wfdisc.nsamp, (double) match->wfdisc.smprat);
            RecordDuration = duration(hdr->nsamp, hdr->srate);
            if (TotalDuration + RecordDuration > maxdur) NeedNewRecord = TRUE;
        }
    }

/* provider either the current wfdisc record or generate a new one, as required */

    return NeedNewRecord ? mkwfdisc(path, fp, hdr) : match;
}

/* Revision History
 *
 * $Log: wfdisc.c,v $
 * Revision 1.20  2015/12/07 19:55:33  dechavez
 * all sorts of changes to calm OS X builds
 *
 * Revision 1.19  2015/03/09 22:01:22  dechavez
 * fixed bug causing extra wfdisc records to be created
 *
 * Revision 1.18  2014/08/29 18:57:38  dechavez
 * split records only if time tears exceed 1/2 sample interval
 *
 * Revision 1.17  2014/08/26 23:42:29  dechavez
 * accept ida10TtagIncrErr() results as gospel (don't check for size of error)
 *
 * Revision 1.16  2013/12/12 19:06:44  dechavez
 * added support for maxdur (force new wfdisc record every 'maxdur' seconds)
 *
 * Revision 1.15  2013/05/11 22:35:56  dechavez
 * ignore time tears that are less than one sample
 *
 * Revision 1.14  2011/03/17 20:47:41  dechavez
 * use the new CSSIO_, cssio_, prefixed names from libcssio 2.2.0
 *
 * Revision 1.13  2008/08/20 18:30:43  dechavez
 * force a new wfdisc record each time ida10TtagIncrErr() notes a "fresh" clock
 *
 * Revision 1.12  2007/09/26 23:11:17  dechavez
 * Fixed spurious time tears bug when multi-station file has duplicate channel names on different stations
 *
 * Revision 1.11  2007/01/11 22:02:12  dechavez
 * switch to isidb and/or isidl prefix on functions in isidb and isidl libraries
 *
 * Revision 1.10  2007/01/04 18:10:40  dechavez
 * Use new incerr header struct to decide to neglect a time tear or not
 *
 * Revision 1.9  2006/12/22 02:34:41  dechavez
 * Warn about missing database.
 * Don't generate new wfdisc records if errors are less than one sample.
 *
 * Revision 1.8  2006/11/13 23:54:44  dechavez
 * INT64 time error
 *
 * Revision 1.7  2006/02/09 20:12:38  dechavez
 * libisidb database support
 *
 * Revision 1.6  2005/10/11 22:52:10  dechavez
 * generate descriptor file
 *
 * Revision 1.5  2005/05/13 19:46:14  dechavez
 * switched to BufferedStream I/O
 *
 * Revision 1.4  2003/10/16 18:13:38  dechavez
 * Added (untested) support for types other than INT32
 *
 * Revision 1.3  2002/01/25 19:27:17  dec
 * generate new wfdisc record on IDA10_ESRATE errors when checking time tears
 *
 * Revision 1.2  2001/12/20 22:25:27  dec
 * Fixed error printing size of data gaps in samples.
 *
 * Revision 1.1.1.1  2000/02/08 20:20:01  dec
 * import existing IDA/NRTS sources
 *
 */
