#pragma ident "$Id: utils.c,v 1.10 2014/08/28 21:22:32 dechavez Exp $"
/*======================================================================
 *
 *  various convenience functions
 *
 *====================================================================*/
#include "isi/dl.h"

/* Find the NRTS stuff for given stream */

NRTS_DL *isidlLocateNrtsDiskloop(ISI_DL_MASTER *master, ISI_STREAM_NAME *target, NRTS_STREAM *result)
{
NRTS_DL *nrts;
int index, i, j;
ISI_STREAM_NAME test;
NRTS_SYS *sys;
NRTS_STA *sta;
NRTS_CHN *chn;

    if (master == NULL || target == NULL || result == NULL) {
        errno = EINVAL;
        return FALSE;
    }

    for (index = 0; index < master->ndl; index++) {
        if ((nrts = master->dl[index]->nrts) != NULL) {
            sys = nrts->sys;
            for (i=0; i < nrts->sys->nsta; i++) {
                sta = &nrts->sys->sta[i];
                for (j = 0; j < sta->nchn; j++) {
                    chn = &sta->chn[j];
                    isiStaChnToStreamName(sta->name, chn->name, &test);
                    if (isiStreamNameMatch(&test, target)) {
                        result->sys = sys;
                        result->sta = sta;
                        result->chn = chn;
                        sprintf(result->ident, "%s:%s", sta->name, chn->name);
                        return nrts;
                    }
                }
            }
        }
    }

    return NULL;
}

/* Test to see if a particular stream exists is available in any of the supported disk loops */

BOOL isidlStreamExists(ISI_DL_MASTER *master, ISI_STREAM_NAME *target)
{
static NRTS_STREAM unused;

    return isidlLocateNrtsDiskloop(master, target, &unused) != NULL ? TRUE : FALSE;
}

/* Find a particular ISI disk loop in the master list */

static int CompareDL(const void *aptr, const void *bptr)
{
ISI_DL *a, *b;

    a = *((ISI_DL **)aptr);
    b = *((ISI_DL **)bptr);

    if (a == NULL && b == NULL) return 0;
    if (a == NULL) return -1;
    if (b == NULL) return  1;

    return strcmp(a->sys->site, b->sys->site);
}

ISI_DL *isidlLocateDiskLoopBySite(ISI_DL_MASTER *master, char *target)
{
int i;

    for (i = 0; i < master->ndl; i++) {
        if (strcmp(master->dl[i]->sys->site, target) == 0) return master->dl[i];
    }

    return NULL;
}

/* Initialize options structure */

void isidlInitOptions(ISI_DL_OPTIONS *dest, UINT32 flags)
{
    if (dest == NULL) return;
    dest->flags = flags;
    dest->offset.seqno = dest->offset.tstamp = -1;
}

/* Revision History
 *
 * $Log: utils.c,v $
 * Revision 1.10  2014/08/28 21:22:32  dechavez
 * removed mseed and bud from ISI_DL_OPTIONS
 *
 * Revision 1.9  2013/03/08 18:28:47  dechavez
 * removed ISI_DL_SEEDLINK from isidlInitOptions()
 *
 * Revision 1.8  2013/01/14 22:11:52  dechavez
 * added SeedLink support to isidlInitOptions()
 *
 * Revision 1.7  2012/06/27 15:29:47  dechavez
 * added isidlLocateDiskLoopBySite()
 *
 * Revision 1.6  2011/11/07 17:28:53  dechavez
 * added isidlInitOptions()
 *
 * Revision 1.5  2007/01/11 21:59:27  dechavez
 * use new isidl and/or isidb function prefixes
 *
 * Revision 1.4  2006/09/29 19:43:08  dechavez
 * cleared tabs
 *
 * Revision 1.3  2006/06/21 18:33:11  dechavez
 * allow for ISI systems w/o NRTS disk loops in isiLocateNrtsDiskloop()
 *
 * Revision 1.2  2005/10/19 23:31:14  dechavez
 * fixed uninitialized variable bug in isiLocateNrtsDiskloop()
 *
 * Revision 1.1  2005/07/26 00:25:20  dechavez
 * initial release
 *
 */
