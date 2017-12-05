#pragma ident "$Id: data.c,v 1.1 2010/09/10 22:56:30 dechavez Exp $"
/*======================================================================
 *
 *  Data thread.  One of these per remote source.
 *
 *====================================================================*/
#include "isimerge.h"

#define MY_MOD_ID ISIMERGE_MOD_DATA

#define INITIAL_RETRY_INTERVAL 10
#define MAX_RETRY_INTERVAL 80

static int ThreadCount = 0;
static MUTEX mutex;
static BOOL first = TRUE;

static void TerminateThread(char *ident)
{
    MUTEX_LOCK(&mutex);
        LogMsg(LOG_INFO, "**** NOTICE **** %s thread exits **** NOTICE ****", ident);
        if (--ThreadCount == 0) {
            LogMsg(LOG_INFO, "All data threads have terminated.  Aborting program.");
            Abort(MY_MOD_ID + 1);
        }
    MUTEX_UNLOCK(&mutex);
    THREAD_EXIT((void *) 0);
}

static THREAD_FUNC SiteThread(void *argptr)
{
SITE_PAR *site;
int ReadResult;
ISI *isi = NULL;
char ServerIdent[MAXPATHLEN+1];
UINT32 maxlen, RetryInterval = INITIAL_RETRY_INTERVAL;
static char *fid = "SiteThread";

    site = (SITE_PAR *) argptr;

    SEM_WAIT(&site->semaphore); /* Block until initialization is complete */
    LogMsg(LOG_DEBUG, "%s: thread started", fid);

/* Loop forever, reading packets and writing to local disk loop */

    while (1) {

    /* connect to server, if necessary */

        while (isi == NULL) {
            if ((isi = ConnectToServer(site, ServerIdent)) == NULL) {
                sleep(RetryInterval);
                RetryInterval *= 2;
                if (RetryInterval > MAX_RETRY_INTERVAL) RetryInterval = INITIAL_RETRY_INTERVAL;
            } else {
                LogMsg(LOG_INFO, "connected to %s, I/O timeout=%.3f sec", ServerIdent, (float) isi->iacp->attr.at_timeo / 1000.0);
                RetryInterval = INITIAL_RETRY_INTERVAL;
                site->first = TRUE;
            }
        }

    /* read next packet from server */

       switch (ReadResult = ReadRawPacket(isi, &site->raw, site->buf, maxlen)) {
          case ISI_OK:
            SavePacket(site, &site->raw);
            break;
          case ISI_DONE:
            LogMsg(LOG_INFO, "EOF received from %s", ServerIdent);
            isi = CloseConnection(isi, ServerIdent, site);
            TerminateThread(ServerIdent);
            break;
          case ISI_BREAK:
            LogMsg(LOG_INFO, "%s disconnected, cause code %d (%s)", ServerIdent, isi->alert, iacpAlertString(isi->alert));
            switch (isi->alert) {
              case IACP_ALERT_SERVER_FAULT:
              case IACP_ALERT_PROTOCOL_ERROR:
              case IACP_ALERT_ILLEGAL_DATA:
                TerminateThread(ServerIdent);
                break;
              default:
                isi = CloseConnection(isi, ServerIdent, site);
                break;
            }
            break;
          case ISI_TIMEDOUT:
            LogMsg(LOG_INFO, "%s: connection timed out", ServerIdent);
            isi = CloseConnection(isi, ServerIdent, site);
            break;
          case ISI_CONNRESET:
            LogMsg(LOG_INFO, "%s: connection reset", ServerIdent);
            isi = CloseConnection(isi, ServerIdent, site);
            break;
          case ISI_BADMSG:
           LogMsg(LOG_INFO, "%s: corrupt packet received", ServerIdent);
            isi = CloseConnection(isi, ServerIdent, site);
            break;
          case ISI_ERROR:
          default:
            LogMsg(LOG_INFO, "%s: UNEXPECTED ReadResult = %d", fid, ReadResult);
            isi = CloseConnection(isi, ServerIdent, site);
        }
    }
}

BOOL QueueSiteThread(LNKLST *head, char *srcstr)
{
THREAD tid;
SITE_PAR site, *this;
LNKLST_NODE *last;
static char *fid = "QueueSiteThread";

    InitializeSitePar(&site);

/* Get server details from the user supplied source string */

    if (!ParseSourceString(srcstr, &site)) return FALSE;

/* Add a new SITE_PAR to the list, and populate it */

    if (!listAppend(head, &site, sizeof(SITE_PAR))) {
        fprintf(stderr, "%s: listAppend: %s\n", fid, strerror(errno));
        return FALSE;
    }

    if ((last = listLastNode(head)) == NULL) {
        fprintf(stderr, "%s: listLastNode: %s\n", fid, strerror(errno));
        return FALSE;
    }
    this = (SITE_PAR *) last->payload;

    SEM_INIT(&this->semaphore, 0, 1);

/* Start the thread (it will block on the semaphore until all is ready) */

    if (!THREAD_CREATE(&tid, SiteThread, (void *) this)) {
        fprintf(stderr, "%s: THREAD_CREATE: SiteThread: %s\n", fid, strerror(errno));
        return FALSE;
    }

    return TRUE;
}

void InitDataThreadMutex()
{
    MUTEX_INIT(&mutex);
}

/* Revision History
 *
 * $Log: data.c,v $
 * Revision 1.1  2010/09/10 22:56:30  dechavez
 * initial release
 *
 */
