#pragma ident "$Id: log.c,v 1.1 2010/09/10 22:56:31 dechavez Exp $"
/*======================================================================
 *
 *  Logging facility
 *
 *====================================================================*/
#include "isimerge.h"

#define MY_MOD_ID ISIMERGE_MOD_LOG

static LOGIO lp;

LOGIO *LogHandle()
{
    return &lp;
}

void LogMsgLevel(int level)
{
    logioSetThreshold(&lp, level);
}

void LogMsg(int level, char *format, ...)
{
va_list marker;
char *ptr, msgbuf[LOGIO_MAX_MSG_LEN];

    ptr = msgbuf;
    va_start(marker, format);
    vsprintf(ptr, format, marker);
    va_end(marker);

    logioMsg(&lp, level, msgbuf);
}

LOGIO *InitLogging(char *myname, char *spec, char *prefix, BOOL debug)
{
    if (!logioInit(&lp, spec, NULL, myname)) return NULL;
    logioSetPrefix(&lp, prefix);

    if (debug) logioSetThreshold(&lp, LOG_DEBUG);
    LogMsg(LOG_INFO, "isimerge - %s (%s %s)", VersionIdentString, __DATE__, __TIME__);

    return &lp;
}

/* Revision History
 *
 * $Log: log.c,v $
 * Revision 1.1  2010/09/10 22:56:31  dechavez
 * initial release
 *
 */
