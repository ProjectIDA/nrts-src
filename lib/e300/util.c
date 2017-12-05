#pragma ident "$Id: util.c,v 1.4 2015/11/04 23:11:46 dechavez Exp $"
/*======================================================================
 *
 * Basic E300 commands
 *
 *====================================================================*/
#include "e300.h"

#define E300_BUFLEN 1024
#define MAXTOKEN 256
#define DELIMITERS "\t\r\n"

static int ReadResponse(int sd, char **token, int timeout)
{
int i, done, maxi;
UINT8 cval;
char buf[E300_BUFLEN+1];
static char *fid = "ReadLine";

    i = done = 0;
    maxi = E300_BUFLEN - 1;
    memset(buf, 0, E300_BUFLEN+1);
    while (!done) {

        cval = 0;
        if (utilRead(sd, &cval, 1, timeout) != 1) {
            if (errno != ETIMEDOUT) {
                return -1;
            } else {
                done = 1;
            }
        } else if (isascii(cval)) {
            buf[i++] = cval;
        }

        if (i == maxi) done = 1;

        if (done) buf[i] = 0;
    }

    return utilParse(buf, token, DELIMITERS, MAXTOKEN, 0);
}

static BOOL SendCommand(int sd, int timeout, char *command, ...)
{
int i, ntoken, nbytes;
static char CR[] = {0x0a, 0};
char *response[MAXTOKEN], *expect;
va_list ap;
static char *fid = "SendString";

    if (command != NULL) {
        nbytes = utilWrite(sd, (UINT8 *) command, strlen(command), timeout);
        if (nbytes != strlen(command)) return FALSE;
    }

    nbytes = utilWrite(sd, (UINT8 *) CR, strlen(CR), timeout);
    if (nbytes != strlen(CR)) return FALSE;

    if (command != NULL) {
        if ((ntoken = ReadResponse(sd, response, timeout)) < 1) return FALSE;

        i = 0;
        va_start(ap, command);
            while ((expect = va_arg(ap, caddr_t)) != NULL) {
                do {
                    if (strcmp(expect, response[i++]) == 0) break;
                    if (i == ntoken) return FALSE;
                } while (i < ntoken);
            }
        va_end(ap);
    }

    return TRUE;
}

typedef struct {
    int timeout;
    char *cmnd;
    char *resp;
} E300_COMMAND_LIST;

static BOOL ExecuteCommandSet(char *e300, E300_COMMAND_LIST *list)
{
int i, sd;
static char *fid = "ExecuteCommandSet";

    if ((sd = utilTcpSocket(e300, E300_DIGI_PASSTHRU_PORT)) <= 0) return FALSE;

    for (i = 0; list[i].timeout > 0; i++) {
        if (!SendCommand(sd, list[i].timeout, list[i].cmnd, list[i].resp, NULL)) {
            shutdown(sd, 2);
            utilCloseSocket(sd);
            return FALSE;
        }
    } 

    shutdown(sd, 2);
    utilCloseSocket(sd);

    return TRUE;
}

BOOL e300Safe(char *e300)
{
static E300_COMMAND_LIST list[] = {
    { 1000,          "safe", "System placed in Safe Mode" },
    { 0, NULL, NULL}
};
static char *fid = "e300Safe";

    return ExecuteCommandSet(e300, list);
}

BOOL e300Extcal(char *e300)
{
static E300_COMMAND_LIST list[] = {
    { 1000,          "safe", "System placed in Safe Mode"                            },
    { 1000,  "enableabcdef", "System Control Enabled"                                },
    { 1000,     "calibrate", "Calibration Control Menu:CALIBRATE> "                  },
    { 4000, "extcalconnect", "EXT CAL Input+/- Connected Directly to E/N/Z CAL Coils"},
    { 0, NULL, NULL}
};
static char *fid = "e300ExtCal";

    return ExecuteCommandSet(e300, list);
}

BOOL e300Ping(char *e300)
{
static E300_COMMAND_LIST list[] = {
    { 1000, NULL, NULL },
    {    0, NULL, NULL }
};
    return ExecuteCommandSet(e300, list);
}

/* Revision History
 *
 * $Log: util.c,v $
 * Revision 1.4  2015/11/04 23:11:46  dechavez
 * cosmetic changes to calm OS X
 *
 * Revision 1.3  2014/10/15 14:47:47  dechavez
 * changed return 0 to return FALSE
 *
 * Revision 1.2  2010/09/30 17:54:09  dechavez
 * removed debugging printfs, fixed bug with connection refused errors
 *
 * Revision 1.1  2010/09/29 21:19:44  dechavez
 * initial release
 *
 */
