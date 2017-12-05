#pragma ident "$Id: chan.c,v 1.3 2015/03/06 22:46:04 dechavez Exp $"
/*======================================================================
 *
 *  Build the new chan table, from the old
 *
 *====================================================================*/
#include "convertdb.h"

#define MY_MOD_ID MOD_CHAN

typedef struct {
    char *chn;
    char flag;
} FLAG_MAP;

static FLAG_MAP map[] = {
    {"ba1", MSEED_B52_FLAG_GEOPHYSICAL},
    {"bae", MSEED_B52_FLAG_GEOPHYSICAL},
    {"ban", MSEED_B52_FLAG_GEOPHYSICAL},
    {"bdf", MSEED_B52_FLAG_GEOPHYSICAL},
    {"bf1", MSEED_B52_FLAG_GEOPHYSICAL},
    {"bf2", MSEED_B52_FLAG_GEOPHYSICAL},
    {"bfe", MSEED_B52_FLAG_GEOPHYSICAL},
    {"bfn", MSEED_B52_FLAG_GEOPHYSICAL},
    {"bfz", MSEED_B52_FLAG_GEOPHYSICAL},
    {"bg1", MSEED_B52_FLAG_GEOPHYSICAL},
    {"bg2", MSEED_B52_FLAG_GEOPHYSICAL},
    {"bh1", MSEED_B52_FLAG_GEOPHYSICAL},
    {"bh2", MSEED_B52_FLAG_GEOPHYSICAL},
    {"bhe", MSEED_B52_FLAG_GEOPHYSICAL},
    {"bhn", MSEED_B52_FLAG_GEOPHYSICAL},
    {"bhz", MSEED_B52_FLAG_GEOPHYSICAL},
    {"bl1", MSEED_B52_FLAG_GEOPHYSICAL},
    {"bl2", MSEED_B52_FLAG_GEOPHYSICAL},
    {"ble", MSEED_B52_FLAG_GEOPHYSICAL},
    {"bln", MSEED_B52_FLAG_GEOPHYSICAL},
    {"blz", MSEED_B52_FLAG_GEOPHYSICAL},
    {"bsa", MSEED_B52_FLAG_GEOPHYSICAL},
    {"bsb", MSEED_B52_FLAG_GEOPHYSICAL},
    {"bsc", MSEED_B52_FLAG_GEOPHYSICAL},
    {"ehe", MSEED_B52_FLAG_GEOPHYSICAL},
    {"ehn", MSEED_B52_FLAG_GEOPHYSICAL},
    {"ehz", MSEED_B52_FLAG_GEOPHYSICAL},
    {"ele", MSEED_B52_FLAG_GEOPHYSICAL},
    {"eln", MSEED_B52_FLAG_GEOPHYSICAL},
    {"elz", MSEED_B52_FLAG_GEOPHYSICAL},
    {"en1", MSEED_B52_FLAG_GEOPHYSICAL},
    {"en2", MSEED_B52_FLAG_GEOPHYSICAL},
    {"ene", MSEED_B52_FLAG_GEOPHYSICAL},
    {"enn", MSEED_B52_FLAG_GEOPHYSICAL},
    {"enz", MSEED_B52_FLAG_GEOPHYSICAL},
    {"hh1", MSEED_B52_FLAG_GEOPHYSICAL},
    {"hh2", MSEED_B52_FLAG_GEOPHYSICAL},
    {"hhe", MSEED_B52_FLAG_GEOPHYSICAL},
    {"hhn", MSEED_B52_FLAG_GEOPHYSICAL},
    {"hhz", MSEED_B52_FLAG_GEOPHYSICAL},
    {"hle", MSEED_B52_FLAG_GEOPHYSICAL},
    {"hln", MSEED_B52_FLAG_GEOPHYSICAL},
    {"hlz", MSEED_B52_FLAG_GEOPHYSICAL},
    {"la1", MSEED_B52_FLAG_GEOPHYSICAL},
    {"lae", MSEED_B52_FLAG_GEOPHYSICAL},
    {"lan", MSEED_B52_FLAG_GEOPHYSICAL},
    {"ldf", MSEED_B52_FLAG_GEOPHYSICAL},
    {"ldi", MSEED_B52_FLAG_WEATHER},
    {"ldo", MSEED_B52_FLAG_WEATHER},
    {"lf1", MSEED_B52_FLAG_GEOPHYSICAL},
    {"lf2", MSEED_B52_FLAG_GEOPHYSICAL},
    {"lfe", MSEED_B52_FLAG_GEOPHYSICAL},
    {"lfn", MSEED_B52_FLAG_GEOPHYSICAL},
    {"lfz", MSEED_B52_FLAG_GEOPHYSICAL},
    {"lg1", MSEED_B52_FLAG_GEOPHYSICAL},
    {"lg2", MSEED_B52_FLAG_GEOPHYSICAL},
    {"lgz", MSEED_B52_FLAG_GEOPHYSICAL},
    {"lh1", MSEED_B52_FLAG_GEOPHYSICAL},
    {"lh2", MSEED_B52_FLAG_GEOPHYSICAL},
    {"lhe", MSEED_B52_FLAG_GEOPHYSICAL},
    {"lhn", MSEED_B52_FLAG_GEOPHYSICAL},
    {"lhz", MSEED_B52_FLAG_GEOPHYSICAL},
    {"ll1", MSEED_B52_FLAG_GEOPHYSICAL},
    {"ll2", MSEED_B52_FLAG_GEOPHYSICAL},
    {"lle", MSEED_B52_FLAG_GEOPHYSICAL},
    {"lln", MSEED_B52_FLAG_GEOPHYSICAL},
    {"llz", MSEED_B52_FLAG_GEOPHYSICAL},
    {"ln1", MSEED_B52_FLAG_GEOPHYSICAL},
    {"ln2", MSEED_B52_FLAG_GEOPHYSICAL},
    {"lne", MSEED_B52_FLAG_GEOPHYSICAL},
    {"lnn", MSEED_B52_FLAG_GEOPHYSICAL},
    {"lnz", MSEED_B52_FLAG_GEOPHYSICAL},
    {"lsa", MSEED_B52_FLAG_GEOPHYSICAL},
    {"lsb", MSEED_B52_FLAG_GEOPHYSICAL},
    {"lsc", MSEED_B52_FLAG_GEOPHYSICAL},
    {"mhe", MSEED_B52_FLAG_HEALTH},
    {"mhn", MSEED_B52_FLAG_HEALTH},
    {"mhz", MSEED_B52_FLAG_HEALTH},
    {"mle", MSEED_B52_FLAG_HEALTH},
    {"mln", MSEED_B52_FLAG_HEALTH},
    {"mlz", MSEED_B52_FLAG_HEALTH},
    {"sh1", MSEED_B52_FLAG_GEOPHYSICAL},
    {"sh2", MSEED_B52_FLAG_GEOPHYSICAL},
    {"she", MSEED_B52_FLAG_GEOPHYSICAL},
    {"shn", MSEED_B52_FLAG_GEOPHYSICAL},
    {"shz", MSEED_B52_FLAG_GEOPHYSICAL},
    {"sl1", MSEED_B52_FLAG_GEOPHYSICAL},
    {"sl2", MSEED_B52_FLAG_GEOPHYSICAL},
    {"sle", MSEED_B52_FLAG_GEOPHYSICAL},
    {"sln", MSEED_B52_FLAG_GEOPHYSICAL},
    {"slz", MSEED_B52_FLAG_GEOPHYSICAL},
    {"uae", MSEED_B52_FLAG_GEOPHYSICAL},
    {"uan", MSEED_B52_FLAG_GEOPHYSICAL},
    {"ugz", MSEED_B52_FLAG_GEOPHYSICAL},
    {"va1", MSEED_B52_FLAG_GEOPHYSICAL},
    {"vf1", MSEED_B52_FLAG_GEOPHYSICAL},
    {"vg1", MSEED_B52_FLAG_GEOPHYSICAL},
    {"vg2", MSEED_B52_FLAG_GEOPHYSICAL},
    {"vae", MSEED_B52_FLAG_GEOPHYSICAL},
    {"van", MSEED_B52_FLAG_GEOPHYSICAL},
    {"vfz", MSEED_B52_FLAG_GEOPHYSICAL},
    {"vfn", MSEED_B52_FLAG_GEOPHYSICAL},
    {"vfe", MSEED_B52_FLAG_GEOPHYSICAL},
    {"vgz", MSEED_B52_FLAG_GEOPHYSICAL},
    {"vh1", MSEED_B52_FLAG_GEOPHYSICAL},
    {"vh2", MSEED_B52_FLAG_GEOPHYSICAL},
    {"vhe", MSEED_B52_FLAG_GEOPHYSICAL},
    {"vhn", MSEED_B52_FLAG_GEOPHYSICAL},
    {"vhz", MSEED_B52_FLAG_GEOPHYSICAL},
    {"vle", MSEED_B52_FLAG_GEOPHYSICAL},
    {"vln", MSEED_B52_FLAG_GEOPHYSICAL},
    {"vlz", MSEED_B52_FLAG_GEOPHYSICAL},
    {"vm1", MSEED_B52_FLAG_HEALTH},
    {"vm2", MSEED_B52_FLAG_HEALTH},
    {"vme", MSEED_B52_FLAG_HEALTH},
    {"vmn", MSEED_B52_FLAG_HEALTH},
    {"vmu", MSEED_B52_FLAG_HEALTH},
    {"vmv", MSEED_B52_FLAG_HEALTH},
    {"vmw", MSEED_B52_FLAG_HEALTH},
    {"vmz", MSEED_B52_FLAG_HEALTH},
    {"vsa", MSEED_B52_FLAG_GEOPHYSICAL},
    {"vsb", MSEED_B52_FLAG_GEOPHYSICAL},
    {"vsc", MSEED_B52_FLAG_GEOPHYSICAL},
    {"wdi", MSEED_B52_FLAG_WEATHER},
    {"wdo", MSEED_B52_FLAG_WEATHER},
    {NULL,   0 }
};

static char TypeFlag(char *chn)
{
int i = 0;

    while (map[i].chn != NULL) {
        if (strcasecmp(chn, map[i].chn) == 0) return map[i].flag;
        i++;
    }

    return 0;
}

static char *ChanFlag(char *descrip, char *chn)
{
char mode, type;
static char flag[DCCDB_CHAN_FLAG_LEN+1];

    if (strcasecmp(descrip, "continuous") == 0) {
        mode = MSEED_B52_FLAG_CONTINUOUS;
    } else if (strcasecmp(descrip, "triggered") == 0) {
        mode = MSEED_B52_FLAG_TRIGGERED;
    } else {
        LogErr("ERROR: unrecognized sitechan descrip field '%s'\n", descrip);
        exit(1);
    }

    if ((type = TypeFlag(chn)) != 0) {
        sprintf(flag, "%c%c", mode, type);
    } else {
        LogErr("ERROR: can't derive channel type flag from chn='%s'\n", chn);
        exit(1);
    }

    return flag;
}


static int ChanCompare(const void *aptr, const void *bptr)
{
int result; 
DCCDB_CHAN *a, *b; 

    a = (DCCDB_CHAN *) aptr;
    b = (DCCDB_CHAN *) bptr;

    if ((result = strcmp(a->sta, b->sta)) != 0) return result; 
    if (a->begt != b->begt) return (a->begt < b->begt) ? -1 : 1; 
    if ((result = utilChnCompare(a->chn, b->chn)) != 0) return result; 
    if ((result = strcmp(a->loc, b->loc)) != 0) return result; 
    if (a->endt != b->endt) return (a->endt < b->endt) ? -1 : 1; 

    return 0;
}

static BOOL AddNewChanEntry(OLD *old, DCCDB *new, OLD_SITECHAN *sitechan, OLD_SEEDLOC *seedloc, LNKLST *head)
{
DCCDB_CHAN entry;
char *iunits, *ounits;
static char *fid = "BuildChan:AddNewChanEntry";

    memset(&entry, 0, sizeof(DCCDB_CHAN));
    strncpy(entry.sta, sitechan->sta, DCCDB_CHAN_STA_LEN+1);
    strncpy(entry.chn, seedloc->newchan, DCCDB_CHAN_CHN_LEN+1);
    strncpy(entry.loc, seedloc->loccode, DCCDB_CHAN_LOC_LEN+1);
    strncpy(entry.flag, ChanFlag(sitechan->descrip, entry.chn), DCCDB_CHAN_FLAG_LEN+1);
    entry.begt = dccdbOndateToBegt(sitechan->ondate);
    entry.endt = dccdbOffdateToEndt(sitechan->offdate);
    entry.edepth = sitechan->edepth;
    entry.hang = sitechan->hang;
    entry.vang = sitechan->vang;
    LookupCalperInstype(old, new, sitechan, &entry.ncalper, entry.instype);
    entry.isduplicate = FALSE;

    return listAppend(head, &entry, sizeof(DCCDB_CHAN));
}

static BOOL IsDuplicateChan(DCCDB_CHAN *a, DCCDB_CHAN *b)
{
static char *fid = "IsDuplicateChan";

    if (strcmp(a->sta,     b->sta    ) != 0) return FALSE;
    if (strcmp(a->chn,     b->chn    ) != 0) return FALSE;
    if (strcmp(a->loc,     b->loc    ) != 0) return FALSE;
    if ((long) a->begt != (long) b->begt) return FALSE;
    if ((long) a->endt != (long) b->endt) return FALSE;

    LogMsg(2, "%s: A = %s\n", fid, dccdbChanString(a, NULL)); 
    LogMsg(2, "%s: B = %s\n", fid, dccdbChanString(b, NULL)); 
    LogMsg(2, "%s: A and B match:\n", fid);

    return TRUE;
}

void BuildChan(OLD *old, DCCDB *new, char *prefix)
{
int i, j;
FILE *fp;
OLD_SEEDLOC *seedloc;
LNKLST *nlist, *slist;
char path[MAXPATHLEN+1];
static char *fid = "BuildChan";

/* We don't know in advance how many new chan records we'll need, so we'll append to a linked list first */

    if ((slist = listCreate()) == NULL) {
        LogErr("%s: listCreate: %s", fid, strerror(errno));
        GracefulExit(MY_MOD_ID + 1);
    }

/* Generate one new chan record for every sta/chn/loc match with seedloc */

    new->nchan = old->nsitechan;
    for (i = 0; i < old->nsitechan; i++) {
        if ((nlist = GenerateStaChnLocTuplesForNewChan(old, &old->sitechan[i])) != NULL) {
            for (j = 0; j < nlist->count; j++) {
                seedloc = (OLD_SEEDLOC *) nlist->array[j];
                if (!AddNewChanEntry(old, new, &old->sitechan[i], seedloc, slist)) {
                    LogErr("%s: AddNewChanEntry: %s", fid, strerror(errno));
                    GracefulExit(MY_MOD_ID + 2);
                }
            }
            listDestroy(nlist);
        }
    }
    if (!listSetArrayView(slist)) {
        LogErr("%s: listSetArrayView: %s", fid, strerror(errno));
        GracefulExit(MY_MOD_ID + 2);
    }

/* Now we can build our new->chan array */

    if ((new->chan = (DCCDB_CHAN *) malloc (sizeof(DCCDB_CHAN) * slist->count)) == NULL) {
        LogErr("%s: malloc: %s", fid, strerror(errno));
        GracefulExit(MY_MOD_ID + 3);
    }

    for (i = 0; i < slist->count; i++) new->chan[i] = *(DCCDB_CHAN *) slist->array[i];
    new->nchan = slist->count;

/* Sort it into sta/begt/chn/loc/endt order */

    qsort(new->chan, new->nchan, sizeof(DCCDB_CHAN), ChanCompare);

/* Identify any duplicate records */

    for (i = 0; i < new->nchan-1; i++) {
        for (j = i+1; !new->chan[i].isduplicate && j < new->nchan; j++) {
            new->chan[i].isduplicate = IsDuplicateChan(&new->chan[i], &new->chan[j]);
        }
    }

/* And write out the table */

    sprintf(path, "%s.%s", prefix, DCCDB_CHAN_TABLE_NAME);
    if ((fp = fopen(path, "w")) == NULL) {
        fprintf(stderr, "fopen: ");
        perror(path);
        GracefulExit(MY_MOD_ID + 4);
    }

    for (i = 0; i < new->nchan; i++) {
        if (!new->chan[i].isduplicate) {
            LogMsg(2, "%s: DROP record %d as duplicate: %s\n", fid, i, dccdbChanString(&new->chan[i], NULL));
        } else {
            dccdbPrintChanRecord(fp, &new->chan[i]);
        }
    }
    fclose(fp);
    LogMsg(1, "%s created OK\n", path);
}

/* Revision History
 *
 * $Log: chan.c,v $
 * Revision 1.3  2015/03/06 22:46:04  dechavez
 * bumped up the debug level on some log messages to make default less verbose
 *
 * Revision 1.2  2015/03/06 21:44:14  dechavez
 * initial production release
 *
 */
