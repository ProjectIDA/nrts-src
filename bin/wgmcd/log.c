#pragma ident "$Id: log.c,v 1.1 2012/07/03 16:15:15 dechavez Exp $"
/*======================================================================
 *
 *  Logging facility
 *
 *====================================================================*/
#include "wgmcd.h"

#define MY_MOD_ID WGMCD_MOD_LOG

static LOGIO *lp = (LOGIO *) NULL;

VOID LogMsgLevel(int level)
{
    logioSetThreshold(lp, level);
}

VOID LogMsg(int level, char *format, ...)
{
va_list marker;
char *ptr, msgbuf[LOGIO_MAX_MSG_LEN];

    ptr = msgbuf;
    va_start(marker, format);
    vsnprintf(ptr, LOGIO_MAX_MSG_LEN, format, marker);
    va_end(marker);

    logioMsg(lp, level, msgbuf);
}

BOOL InitLogging(char *myname, PARAM *par)
{
    if (!logioInit(&par->lp, par->log, NULL, myname)) {
        return FALSE;
    } else {
        lp = &par->lp;
    }

    if (par->debug) logioSetThreshold(lp, LOG_DEBUG);
    logioMsg(lp, LOG_INFO, "Wave Glider Mission Control Daemon - %s", VersionIdentString);
    logioMsg(lp, LOG_INFO, "Build %s %s", __DATE__, __TIME__);

    return TRUE;
}

/* Revision History
 *
 * $Log: log.c,v $
 * Revision 1.1  2012/07/03 16:15:15  dechavez
 * initial (barely working) release
 *
 */
