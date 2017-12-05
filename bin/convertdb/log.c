#pragma ident "$Id: log.c,v 1.2 2015/03/06 21:44:14 dechavez Exp $"
/*======================================================================
 *
 *  Logging facility
 *
 *====================================================================*/
#include "convertdb.h"

static LOGIO lp;
static int threshold = 1;

void LogRunParameters(int level, int argc, char **argv, char *old_path, char *new_schema, char *new_prefix)
{
int i;
char message[MAXPATHLEN+1];

    sprintf(message, "command line:  ");
    for (i = 0; i < argc; i++) sprintf(message+strlen(message), " %s", argv[i]);
    LogMsg(level, message);
    LogMsg(level, "input database: %s\n", old_path);
    LogMsg(level, "output schema:  %s\n", new_schema);
    LogMsg(level, "output prefix:  %s\n", new_prefix);
}

void LogMsg(int level, char *format, ...)
{
va_list marker;
char *ptr, msgbuf[LOGIO_MAX_MSG_LEN];

    if (level > threshold) return;

    ptr = msgbuf;
    va_start(marker, format);
    vsnprintf(ptr, LOGIO_MAX_MSG_LEN, format, marker);
    va_end(marker);

    logioMsg(&lp, LOG_INFO, msgbuf);
}

void LogErr(char *format, ...)
{
va_list marker;
char *ptr, msgbuf[LOGIO_MAX_MSG_LEN];

    ptr = msgbuf;
    va_start(marker, format);
    vsnprintf(ptr, LOGIO_MAX_MSG_LEN, format, marker);
    va_end(marker);

    logioMsg(&lp, LOG_ERR, msgbuf);
}

void LogMsgLevel(int newlevel)
{
    threshold = newlevel;
    logioMsg(&lp, threshold, "log level %d\n", threshold);
}

LOGIO *InitLogging(char *myname, char *path, int level)
{
static char *fid = "InitLogging";

    threshold = level;

    if (!logioInit(&lp, path, NULL, myname)) {
        fprintf(stderr, "%s: logioInit: %s\n", fid, strerror(errno));
        exit(1);
    }
    LogMsg(1, "%s - %s", myname, VersionIdentString);
    LogMsg(1, "Build %s %s", __DATE__, __TIME__);

    return &lp;
}

/* Revision History
 *
 * $Log: log.c,v $
 * Revision 1.2  2015/03/06 21:44:14  dechavez
 * initial production release
 *
 */
