#pragma ident "$Id: liss.c,v 1.7 2011/11/07 17:49:20 dechavez Exp $"
/*======================================================================
 *
 *  MiniSEED packets from a LISS server
 *
 *====================================================================*/
#include "isidl.h"
#include "liss.h"
#define MY_MOD_ID ISIDL_MOD_LISS

#define INITIAL_RETRY_INTERVAL 10
#define MAX_RETRY_INTERVAL     80

static char *server;
static int port;
static int blksiz;
static int to;

BOOL ParseLissArgs(char *string)
{
LNKLST *TokenList;
static char *fid = "ParseLissArgs";

    if ((TokenList = utilStringTokenList(string, ":,", 0)) == NULL) {
        fprintf(stderr, "%s: utilStringTokenList: %s\n", fid, strerror(errno));
        return FALSE;
    }

    if (!listSetArrayView(TokenList)) {
        fprintf(stderr, "%s: listSetArrayView: %s\n", fid, strerror(errno));
        return FALSE;
    }

    if (TokenList->count != 4) {
        fprintf(stderr, "%s: incorrect token count (must be 4)\n", fid);
        return FALSE;
    }

    if ((server = strdup((char *) TokenList->array[0])) == NULL) {
        fprintf(stderr, "%s: strdup: %s\n", fid, strerror(errno));
        return FALSE;
    }

    if ((port = atoi((char *) TokenList->array[1])) <= 0) {
        fprintf(stderr, "%s: illegal LISS port number '%s'\n", fid, (char *) TokenList->array[1]);
        return FALSE;
    }

    if ((blksiz = atoi((char *) TokenList->array[2])) <= 0) {
        fprintf(stderr, "%s: illegal LISS block size '%s'\n", fid, (char *) TokenList->array[2]);
        return FALSE;
    }

    if ((to = atoi((char *) TokenList->array[3])) <= 0) {
        fprintf(stderr, "%s: illegal LISS I/O timeout '%s'\n", fid, (char *) TokenList->array[3]);
        return FALSE;
    }

    return TRUE;
}

/* Read MiniSEED packets from a LISS server */

static THREAD_FUNC LissThread(void *argptr)
{
ISIDL_PAR *par;
NRTS_DL *nrts;
LISS *liss = NULL;
LOCALPKT local;
LISS_PKT pkt;
int RetryInterval = INITIAL_RETRY_INTERVAL;
UINT32 flags = 0;
static char *fid = "LissThread";

    par = (ISIDL_PAR *) argptr;

    MUTEX_INIT(&local.mutex);
    local.dl = par->dl[0];
    strlcpy(local.raw.hdr.site, local.dl->sys->site, ISI_SITELEN+1);
    local.raw.hdr.desc.comp = ISI_COMP_NONE;;
    local.raw.hdr.desc.type = ISI_TYPE_MSEED;
    local.raw.hdr.desc.size = 1;
    local.raw.hdr.len.used = blksiz;
    local.raw.hdr.len.native = blksiz;
    local.options = &par->options;

    while (1) {

    /* Connect to server, if necessary */

        while (liss == NULL) {
            if ((liss = lissOpen(server, port, blksiz, to, par->lp)) == NULL) {
                sleep(RetryInterval);
                RetryInterval *= 2;
                if (RetryInterval > MAX_RETRY_INTERVAL) RetryInterval = INITIAL_RETRY_INTERVAL;
            } else {
                LogMsg(LOG_INFO, "connected to liss@%s:%d, blksiz=%d, to=%d", server, port, blksiz, to);
                local.raw.payload = liss->buf;
            }
        }

    /* Read next packet */

        if (lissRead(liss, &pkt, flags)) {
            RetryInterval = INITIAL_RETRY_INTERVAL;
            if (par->first[0]) {
                LogMsg(LOG_INFO, "initial %s packet received", local.dl->sys->site);
                par->first[0] = FALSE;
            }
            if (pkt.status == LISS_HEARTBEAT) {
                LogMsg(LOG_DEBUG, "%s liss heartbeat packet received", local.dl->sys->site);
                continue;
            }
            
            local.raw.hdr.desc.order = (pkt.hdr.fsdh.order == BIG_ENDIAN_BYTE_ORDER) ? ISI_ORDER_BIGENDIAN : ISI_ORDER_LTLENDIAN;
            ProcessLocalData(&local);
        } else {
            LogMsg(LOG_INFO, "liss@%s:%d: lissRead: %s", server, port, strerror(errno));
            liss = lissClose(liss);
        }
    }
}


void StartLissReader(ISIDL_PAR *par)
{
THREAD tid;
static char *fid = "StartLissReader";

    if (!THREAD_CREATE(&tid, LissThread, (void *) par)) {
        LogMsg(LOG_INFO, "%s: THREAD_CREATE: LissThread: %s", fid, strerror(errno));
        Hold(MY_MOD_ID + 1);
    }
}

/* Revision History
 *
 * $Log: liss.c,v $
 * Revision 1.7  2011/11/07 17:49:20  dechavez
 * accomodated move of mseed to ISI_DL_OPTIONS
 *
 * Revision 1.6  2011/11/03 18:02:52  dechavez
 * Populate ISI_RAW_HEADER mseed field (not tested)
 *
 * Revision 1.5  2010/04/01 21:51:09  dechavez
 * replaced all Exit() calls to either set exit status and continue, or set status and hold
 *
 * Revision 1.4  2009/11/19 19:27:39  dechavez
 * recognize (and ignore) LISS heartbeats (aap)
 *
 * Revision 1.3  2008/01/07 22:16:45  dechavez
 * backed out explicit LISS to IDA10 conversion (now handled in the library)
 *
 * Revision 1.2  2007/12/21 22:24:13  dechavez
 * Repackage MiniSEED into IDA10.5 and write to NRTS disk loop
 *
 * Revision 1.1  2007/05/03 20:26:58  dechavez
 * initial release
 *
 */
