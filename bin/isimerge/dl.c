#pragma ident "$Id: dl.c,v 1.3 2011/11/09 20:12:35 dechavez Exp $"
/*======================================================================
 *
 *  Local disk loop operations.
 *
 *====================================================================*/
#define INCLUDE_ISI_STATIC_SEQNOS
#include "isimerge.h"

#define MY_MOD_ID ISIMERGE_MOD_SAVE

static ISI_DL *dl = NULL;
static ISI_DL_OPTIONS options;

void UpdateParentID()
{
    LockGlobalMutex();
        isidlUpdateParentID(dl, getpid());
    ReleaseGlobalMutex();
}

BOOL UpdateSitePar(SITE_PAR *site)
{
char *mode;
static char *RPLUS = "r+b";
static char *WPLUS = "w+b";
static char *fid = "UpdateSitePar";

    if (!utilDirectoryExists(dl->path.meta)) util_mkpath(dl->path.meta, 0755);
    if (!utilDirectoryExists(dl->path.meta)) {
        LogMsg(LOG_ERR, "%s: can't mkdir %s: %s", fid, dl->path.meta, strerror(errno));
        return FALSE;
    }

    sprintf(site->mpath, "%s/%s.%s@%s", dl->path.meta, ISI_DL_MERGE_PREFIX, site->name, site->server);

    mode = utilFileExists(site->mpath) ? RPLUS : WPLUS;
    
    if ((site->mfp = fopen(site->mpath, mode)) == NULL) {
        LogMsg(LOG_ERR, "%s: ERROR: fopen: %s: %s", fid, site->mpath, strerror(errno));
        return FALSE;
    }

    if (isiIsUndefinedSeqno(&site->next) && !SetMetaStartSeqno(site)) return FALSE;
    if (isiIsUndefinedSeqno(&site->end)) site->end = ISI_KEEPUP_SEQNO;

    if ((site->buf = (UINT8 *) malloc(dl->sys->maxlen)) == NULL) {
        LogMsg(LOG_ERR, "%s: malloc(%lu): %s", fid, dl->sys->maxlen, strerror(errno));
        return FALSE;
    }
    if (!isiInitRawPacket(&site->raw, NULL, dl->sys->maxlen)) {
        LogMsg(LOG_ERR, "%s: InitRawPacket(%lu): %s", fid, dl->sys->maxlen, strerror(errno));
        return FALSE;
    }

}

void SavePacket(SITE_PAR *site, ISI_RAW_PACKET *raw)
{
char buf1[MAXPATHLEN], buf2[MAXPATHLEN];
static char *fid = "SavePacket";

    LockGlobalMutex();

    /* Packet size mis-match is fatal */

        if (raw->hdr.len.used > dl->sys->maxlen) {
            LogMsg(LOG_INFO, "*** FATAL ERROR *** %s@%s %d byte payload > %d!", site->name, site->server, raw->hdr.len.used, dl->sys->maxlen);
            Abort(MY_MOD_ID + 1);
        }

    /* Ensure that this is packet not a duplicate or overlap */

        if (isiIsUndefinedSeqno(&site->last)) {
            switch (isiCompareSeqno(&raw->hdr.seqno, &site->last)) {
              case 0:
                isiSeqnoString(&raw->hdr.seqno, buf1);
                LogMsg(LOG_INFO, "duplicate %s@%s packet dropped (seqno = %s)", site->name, site->server, buf1);
                return;
              case -1:
                isiSeqnoString(&raw->hdr.seqno, buf1);
                isiSeqnoString(&site->last, buf2);
                LogMsg(LOG_INFO, "overlapping %s@%s packet dropped (new = %s, dl = %s)", site->name, site->server, buf1, buf2);
                return;
            }
        }

    /* Replace the incoming site name with our name */

        strncpy(raw->hdr.site, dl->sys->site, ISI_SITELEN+1);

    /* Update sequence numbers in the packet headers */

        if (isiIsUndefinedSeqno(&raw->hdr.oldseqno)) UpdateSequenceNumbers(&dl->sys->seqno, raw);

/* Write it to disk */

        isidlWriteToDiskLoop(dl, raw, &options);

    /* Remember original sequence number for comparison and reconnects */

        site->last = raw->hdr.oldseqno;
        SaveLastSeqno(site);

    /* Update diskloop sequence number for the next packet */

        isiIncrSeqno(&dl->sys->seqno);

    /* done */

        if (site->first) {
            LogMsg(LOG_INFO, "initial %s@%s packet received", site->name, site->server);
            site->first = FALSE;
        }
    ReleaseGlobalMutex();

}

BOOL OpenDiskLoop(ISI_GLOB *glob, char *site, LOGIO *lp)
{
static char *fid = "OpenDiskLoop";

    if ((dl = isidlOpenDiskLoop(glob, site, lp, ISI_RDWR)) == NULL) {
        LogMsg(LOG_ERR, "%s: isidlOpenDiskLoop: %s\n", fid, strerror(errno));
        return FALSE;
    }
	isidlInitOptions(&options);

    return TRUE;
}

void CloseDiskLoop()
{
    if (dl != NULL) {
        LogMsg(LOG_INFO, "%s disk loop closed", dl->sys->site);
        isidlCloseDiskLoop(dl);
    }
}

/* Revision History
 *
 * $Log: dl.c,v $
 * Revision 1.3  2011/11/09 20:12:35  dechavez
 * Accomodate change of isidlWriteToDiskLoop options field from bitmask to structure
 *
 * Revision 1.2  2010/09/17 20:04:22  dechavez
 * reordered stuff in failed attempt to figure out why the threads hang when run as a daemon,
 * changed site@server specification to eliminate the isi= prefix
 *
 * Revision 1.1  2010/09/10 22:56:31  dechavez
 * initial release
 *
 */
