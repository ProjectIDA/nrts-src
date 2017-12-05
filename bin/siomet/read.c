#pragma ident "$Id: read.c,v 1.1 2017/09/28 18:20:44 dauerbach Exp $"
/*======================================================================
 *
 * Thread to read scans from the device, as fast as it can emit them
 * (which is roughly 1 sps)
 *
 * Repeated calls to GetScan() will return TRUE the first time a
 * scan has been read, but will return FALSE on all subsequent calls
 * until a fresh scan has been read.
 *
 * Debug verbosity is managed via a signal handler (see signals.c)
 *
 *====================================================================*/
#include "siomet.h"

#define MY_MOD_ID SIOMET_MOD_READ

static SEMAPHORE Semaphore;

static BOOL NewScan = FALSE;
static BOOL Debug = FALSE;
static MUTEX mutex;
static SIOMET_SCAN SavedScan;

static void SaveScan(SIOMET_SCAN *scan)
{
    MUTEX_LOCK(&mutex);
        SavedScan = *scan;
        NewScan = TRUE;
    MUTEX_UNLOCK(&mutex);
}

BOOL GetScan(SIOMET_SCAN *dest)
{
BOOL retval = FALSE;

    MUTEX_LOCK(&mutex);
        if (NewScan) {
            *dest = SavedScan;
            NewScan = FALSE; // This flags the scan as read, so we won't reuse it
            retval = TRUE;
        }
    MUTEX_UNLOCK(&mutex);

    return retval;
}

void ToggleReadThreadVerbosity(void)
{
    Debug = !Debug;
    LogMsg("packet logging %s", Debug ? "enabled" : "disabled");
}

static REAL64 DecodeToken(char *token)
{
REAL64 result;

    token[strlen(token)-1] = 0; /* clobber units field */
    result = atof(token);
    return result;
}

static int ParseMetString(char *string, REAL64 *pTa, REAL64 *pUa, REAL64 *pPa)
{
int i, ntoken;
#define DELIMITERS ",="
#define NTOKEN     15
#define QUOTE      0
char *token[NTOKEN], *msg;
char copy[SIOMET_BUFLEN+1];

/* work with a copy avoid corrupting input string for possible forensics */

    strncpy(copy, string, SIOMET_BUFLEN);

/* reject corrupt strings */

    if (!ChecksumOK(copy)) return SIOMET_CRC_ERROR;

/* parse it */

    if ((ntoken = utilParse(copy, token, DELIMITERS, NTOKEN, QUOTE)) != NTOKEN) return SIOMET_BAD_STRING;

    if (strcmp(token[5], "Ta") != 0) return SIOMET_BAD_STRING;
    *pTa = DecodeToken(token[6]);

    if (strcmp(token[7], "Ua") != 0) return SIOMET_BAD_STRING;
    *pUa = DecodeToken(token[8]);

    if (strcmp(token[9], "Pa") != 0) return SIOMET_BAD_STRING;
    *pPa = DecodeToken(token[10]);

     if (Debug) LogMsg("T=%.2fC H=%.1f%% P=%.4fbar\n", *pTa, *pUa, *pPa);

    return SIOMET_OK;
}

static BOOL ReadLine(TTYIO *tty, char *buffer)
{
int count = 0;

    while (1) {

        if (ttyioRead(tty, &buffer[count], 1) != 1) {
            switch (errno) {
              case ETIMEDOUT:
                LogMsg("WARNING: timeout reading serial port");
                return FALSE;
              default:
                LogMsg("ERROR: ReadLine: ttyioRead: %s", strerror(errno));
                GracefulExit(MY_MOD_ID + 1);
            }
        }

        /* over-write CRLF with NULLs */

        if (buffer[count] == 0x0d) {
            buffer[count] = 0;
        } else if (buffer[count] == 0x0a) {
            buffer[count] = 0;
            if (Debug) LogMsg("%s", buffer);
            return TRUE;
        }

        if (++count == SIOMET_BUFLEN) {
            LogMsg("ERROR: ReadLine: Buffer overrun, SIOMET_BUFLEN=%d\n", SIOMET_BUFLEN);
            GracefulExit(MY_MOD_ID + 2);
        }
    }
}

void ReadScan(TTYIO *tty, SIOMET_SCAN *scan)
{
static BOOL FirstScan = TRUE;
int result;
#define CMDLEN 4
char cmd[CMDLEN] = {'0', 'R', '0', 0xd};

    while (1) {
        if (ttyioWrite(tty, cmd, CMDLEN) != CMDLEN) {
            LogMsg("ERROR: ttyioWrite: %s", strerror(errno));
            GracefulExit(MY_MOD_ID + 3);
        }

        if (ReadLine(tty, scan->string)) {
            if ((result = ParseMetString(scan->string, &scan->Ta, &scan->Ua, &scan->Pa)) == SIOMET_OK) {
                scan->iTa = scan->Ta * SIOMET_TA_FACTOR;
                scan->iUa = scan->Ua * SIOMET_UA_FACTOR;
                scan->iPa = scan->Pa * SIOMET_PA_FACTOR;
                if (FirstScan) {
                    SEM_POST(&Semaphore);
                    FirstScan = FALSE;
                }
                return;
            }
            LogMsg("WARNING: ReadScan: corrupt met string ignored: '%s'", scan->string);
        }
    }
}

static THREAD_FUNC ReadThread(void *arg)
{
TTYIO *tty;
SIOMET_INFO *info;
SIOMET_SCAN scan;
static char *fid = "ReadThread";

    info = (SIOMET_INFO *) arg;

    if ((tty = ttyioOpen(
        info->device,
        TRUE,
        info->speed,
        info->speed,
        TTYIO_PARITY_NONE,
        TTYIO_FLOW_NONE,
        1,
        SIOMET_FIXED_TO,
        0,
        info->lp)) == NULL
    ){
        LogMsg("%s: ttyioOpen(%s, %d,...) failed: %s", fid, info->device, info->speed, strerror(errno));
        GracefulExit(MY_MOD_ID + 4);
    }

    while (1) {
        ReadScan(tty, &scan);
        SaveScan(&scan);
    }
}

void StartDeviceReader(SIOMET_INFO *info, LOGIO *lp)
{
THREAD tid;
static char *fid = "StartDeviceReader";

    MUTEX_INIT(&mutex);
    SEM_INIT(&Semaphore, 0, 0);

    if (!THREAD_CREATE(&tid, ReadThread, (void *) info)) {
        LogMsg("%s: THREAD_CREATE: ReadThread: %s", fid, strerror(errno));
        GracefulExit(MY_MOD_ID + 6);
    }
    THREAD_DETACH(tid);

    SEM_WAIT(&Semaphore);
}

/*-----------------------------------------------------------------------+
 |                                                                       |
 | Copyright (C) 2017 Regents of the University of California            |
 |                                                                       |
 | This software is provided 'as-is', without any express or implied     |
 | warranty.  In no event will the authors be held liable for any        |
 | damages arising from the use of this software.                        |
 |                                                                       |
 | Permission is granted to anyone to use this software for any purpose, |
 | including commercial applications, and to alter it and redistribute   |
 | it freely, subject to the following restrictions:                     |
 |                                                                       |
 | 1. The origin of this software must not be misrepresented; you must   |
 |    not claim that you wrote the original software. If you use this    |
 |    software in a product, an acknowledgment in the product            |
 |    documentation of the contribution by Project IDA, UCSD would be    |
 |    appreciated but is not required.                                   |
 | 2. Altered source versions must be plainly marked as such, and must   |
 |    not be misrepresented as being the original software.              |
 | 3. This notice may not be removed or altered from any source          |
 |    distribution.                                                      |
 |                                                                       |
 +-----------------------------------------------------------------------*/

/* Revision History
 *
 * $Log: read.c,v $
 * Revision 1.1  2017/09/28 18:20:44  dauerbach
 * initial release
 *
 */
