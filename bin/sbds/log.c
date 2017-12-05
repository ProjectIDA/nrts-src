#pragma ident "$Id: log.c,v 1.3 2013/05/11 23:08:20 dechavez Exp $"
/*======================================================================
 *
 *  Logging facility
 *
 *====================================================================*/
#include "sbds.h"

#define MY_MOD_ID SBDS_MOD_LOG

static LOGIO *lp = (LOGIO *) NULL;

LOGIO *LogPointer(void)
{
    return lp;
}

void LogMsgLevel(int level)
{
    logioSetThreshold(lp, level);
}

void LogMsg(int level, char *format, ...)
{
va_list marker;
char *ptr, msgbuf[LOGIO_MAX_MSG_LEN];

    ptr = msgbuf;
    va_start(marker, format);
    vsnprintf(ptr, LOGIO_MAX_MSG_LEN, format, marker);
    va_end(marker);

    logioMsg(lp, level, msgbuf);
}

void HexDump(int level, UINT8 *data, int len)
{
    logioHexDump(lp, level, data, len);
}

BOOL InitLogging(char *myname, PARAM *par)
{
    if (!logioInit(&par->lp, par->log, NULL, myname)) {
        return FALSE;
    } else {
        lp = &par->lp;
    }

    if (par->debug) logioSetThreshold(lp, LOG_DEBUG);
    logioMsg(lp, LOG_INFO, "Iridium Short Burst Data Server - %s", VersionIdentString);
    logioMsg(lp, LOG_INFO, "Build %s %s", __DATE__, __TIME__);

    return TRUE;
}

/* Revision History
 *
 * $Log: log.c,v $
 * Revision 1.3  2013/05/11 23:08:20  dechavez
 * VOID -> void
 *
 * Revision 1.2  2013/03/13 21:34:53  dechavez
 * added HexDump()
 *
 * Revision 1.1  2013/03/11 23:04:16  dechavez
 * initial release
 *
 */
