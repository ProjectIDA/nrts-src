#pragma ident "$Id: log.c,v 1.2 2011/02/25 18:54:58 dechavez Exp $"
/*======================================================================
 *
 *  Logging facility
 *
 *====================================================================*/
#include "dccdmx.h"

#define MY_MOD_ID DCCDMX_MOD_LOG

static LOGIO lp;

void LogMsg(char *format, ...)
{
va_list marker;
char *ptr, msgbuf[LOGIO_MAX_MSG_LEN];

    ptr = msgbuf;
    va_start(marker, format);
    vsnprintf(ptr, LOGIO_MAX_MSG_LEN, format, marker);
    va_end(marker);

    logioMsg(&lp, LOG_INFO, msgbuf);
}

void InitLog(char *myname)
{
static char *fid = "InitLog";

    if (!logioInit(&lp, NULL, NULL, myname)) {
        fprintf(stderr, "ERROR: %s: logioInit: %s\n", fid, strerror(errno));
        exit(MY_MOD_ID + 1);
    }
}

/* Revision History
 *
 * $Log: log.c,v $
 * Revision 1.2  2011/02/25 18:54:58  dechavez
 * initial release
 *
 */
