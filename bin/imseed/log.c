#pragma ident "$Id: log.c,v 1.4 2014/10/29 21:35:47 dechavez Exp $"
/*======================================================================
 *
 *  Logging facility
 *
 *====================================================================*/
#include "imseed.h"

static LOGIO lp;
static int level = LOG_INFO;

void LogCommandLine(int argc, char **argv)
{
int i;
char message[MAXPATHLEN+1];

    sprintf(message, "working directory: ");
    getcwd(message+strlen(message), MAXPATHLEN-strlen(message));
    LogMsg(message);

    sprintf(message, "command line:     ");
    for (i = 0; i < argc; i++) sprintf(message+strlen(message), " %s", argv[i]);
    LogMsg(message);
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

void LogMsg(char *format, ...)
{
va_list marker;
char *ptr, msgbuf[LOGIO_MAX_MSG_LEN];

    ptr = msgbuf;
    va_start(marker, format);
    vsnprintf(ptr, LOGIO_MAX_MSG_LEN, format, marker);
    va_end(marker);

    logioMsg(&lp, level, msgbuf);
}

void LogMsgLevel(int newlevel)
{
    level = newlevel;
    logioMsg(&lp, level, "log level %d\n", level);
}

void InterceptLogger(char *string)
{
    logioMsg(&lp, LOG_INFO, string);
}

LOGIO *InitLogging(char *myname, char *path, BOOL verbose)
{
static char *fid = "InitLogging";

    if (!verbose) level = LOG_DEBUG;

    if (!logioInit(&lp, path, NULL, myname)) {
        fprintf(stderr, "%s: logioInit: %s\n", fid, strerror(errno));
        exit(1);
    }
    logioMsg(&lp, level, "%s - %s", myname, VersionIdentString);
    logioMsg(&lp, level, "Build %s %s", __DATE__, __TIME__);

    ms_loginit(InterceptLogger, "ms_log: ", InterceptLogger, "ms_diag: ");

    return &lp;
}

/* Revision History
 *
 * $Log: log.c,v $
 * Revision 1.4  2014/10/29 21:35:47  dechavez
 * added support to intercept and print ms_log() messages coming from libdmc
 *
 * Revision 1.3  2014/08/11 20:42:43  dechavez
 * changed InitLogging from void to LOGIO*
 *
 * Revision 1.2  2014/03/04 21:08:36  dechavez
 * include working directory in output of LogCommandLine(), added LogErr()
 *
 * Revision 1.1  2014/02/05 21:16:25  dechavez
 * initial release
 *
 */
