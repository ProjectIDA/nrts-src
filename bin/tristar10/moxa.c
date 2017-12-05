#pragma ident "$Id: moxa.c,v 1.4 2015/12/09 18:44:43 dechavez Exp $"
/*======================================================================
 *
 *  Read the devices in a slow loop
 *
 *====================================================================*/
#include "tristar10.h"

#define MY_MOD_ID TRISTAR10_MOD_MOXA

static SCAN CurrentScan;
static MUTEX mutex;

static void PrintReading(char *buf, READING *reading)
{
    sprintf(buf+strlen(buf), "%s=", reading->chnloc);
    if (reading->live) {
        sprintf(buf+strlen(buf), "%d/%u/%s", reading->value, reading->missed, reading->fresh ? "fresh" : "STALE");
    } else {
        sprintf(buf+strlen(buf), "DEAD");
    }
}

void LogScan(SCAN *scan)
{
char tmpbuf[256];

    if (!DebugEnabled()) return;

    tmpbuf[0] = 0;
    PrintReading(tmpbuf, &scan->aerrs);
    LogDebug(tmpbuf);

    tmpbuf[0] = 0;
    PrintReading(tmpbuf, &scan->aes01); strcat(tmpbuf, " ");
    PrintReading(tmpbuf, &scan->aea01); strcat(tmpbuf, " ");
    PrintReading(tmpbuf, &scan->aef01); strcat(tmpbuf, " ");
    PrintReading(tmpbuf, &scan->aev01); strcat(tmpbuf, " ");
    PrintReading(tmpbuf, &scan->aec01); strcat(tmpbuf, " ");
    PrintReading(tmpbuf, &scan->ae101); strcat(tmpbuf, " ");
    PrintReading(tmpbuf, &scan->ae201);
    LogDebug(tmpbuf);

    tmpbuf[0] = 0;
    PrintReading(tmpbuf, &scan->aes02); strcat(tmpbuf, " ");
    PrintReading(tmpbuf, &scan->aea02); strcat(tmpbuf, " ");
    PrintReading(tmpbuf, &scan->aef02); strcat(tmpbuf, " ");
    PrintReading(tmpbuf, &scan->aev02); strcat(tmpbuf, " ");
    PrintReading(tmpbuf, &scan->aec02);
    LogDebug(tmpbuf);
}

SCAN *LockScan(void)
{
    MUTEX_LOCK(&mutex);
    return &CurrentScan;
}

void UnlockScan(void)
{
    MUTEX_UNLOCK(&mutex);
}

static int SaveRegister(READING *dest, TRISTAR_REG *src, BOOL convert)
{

    if (src->valid) {
        dest->value  = convert ? (INT32) (src->value * CONVERSION_GAIN) : src->raw;
        dest->missed = 0;
        dest->fresh  = TRUE;
        dest->live   = TRUE;
        return 0;
    }

    return (dest->fresh || !dest->live) ? 0 : 1; /* missed readings of live channels return 1 */
}

static THREAD_FUNC MoxaThread(void *arg)
{
SCAN *scan;
modbus_t *ctx;
SERVER *source;
TRISTAR_RAM ram[2];
BOOL first[2];

    source = (SERVER *) arg;
    LogMsg("moxa thread started, server=%s, port=%d\n", source->server, source->port);

    first[0] = first[1] = TRUE;

    while ((ctx = modbus_new_tcp(source->server, source->port)) == NULL) {
        LogMsg("ERROR: modbus_new_tcp(%s, %d): %s\n", source->server, source->port, strerror(errno));
        sleep(60);
    }

    if (source->bto.tv_sec != 0 || source->bto.tv_usec != 0) modbus_set_byte_timeout(ctx, &source->bto);
    if (source->rto.tv_sec != 0 || source->rto.tv_usec != 0) modbus_set_response_timeout(ctx, &source->rto);

    modbus_get_byte_timeout(ctx, &source->bto);
    LogMsg("modbus byte     timeout = %ld.%05d\n", source->bto.tv_sec, source->bto.tv_usec);
    modbus_get_response_timeout(ctx, &source->rto);
    LogMsg("modbus response timeout = %ld.%05d\n", source->rto.tv_sec, source->rto.tv_usec);

    while (1) {

        if ((modbus_connect(ctx)) == -1) {
            LogMsg("ERROR: modbus_connect(%s): %s\n", source->server, modbus_strerror(errno));
            sleep(60);
        } else {

            tristarReadRamLogset(ctx, 1, &ram[0]); /* charger registers */
            tristarReadRamLogset(ctx, 2, &ram[1]); /* load controller registers */

            scan = LockScan();
                scan->aerrs.value  = 0;
                scan->aerrs.value += SaveRegister(&scan->aes01, &ram[0].control_state, FALSE);
                scan->aerrs.value += SaveRegister(&scan->aea01, &ram[0].Alarm,         FALSE);
                scan->aerrs.value += SaveRegister(&scan->aef01, &ram[0].fault,         FALSE);
                scan->aerrs.value += SaveRegister(&scan->aev01, &ram[0].adc_vb_f,       TRUE);
                scan->aerrs.value += SaveRegister(&scan->aec01, &ram[0].adc_ipv_f,      TRUE);
                scan->aerrs.value += SaveRegister(&scan->ae101, &ram[0].adc_vs_f,       TRUE);
                scan->aerrs.value += SaveRegister(&scan->ae201, &ram[0].adc_vx_f,       TRUE);
                scan->aerrs.value += SaveRegister(&scan->aes02, &ram[1].control_state, FALSE);
                scan->aerrs.value += SaveRegister(&scan->aea02, &ram[1].Alarm,         FALSE);
                scan->aerrs.value += SaveRegister(&scan->aef02, &ram[1].fault,         FALSE);
                scan->aerrs.value += SaveRegister(&scan->aev02, &ram[1].adc_vx_f,       TRUE);
                scan->aerrs.value += SaveRegister(&scan->aec02, &ram[1].adc_iload_f,    TRUE);
                scan->aerrs.fresh = TRUE;
            UnlockScan();
        }

        modbus_close(ctx);
        sleep(5);
    }
}

static void InitReading(char *chnloc, READING *reading)
{
    strncpy(reading->chnloc, chnloc, IDA10_CNAMLEN);
    reading->value = reading->missed = 0;
    reading->fresh = reading->live = (strcmp(chnloc, AERRS) == 0) ? TRUE : FALSE;
}

void StartMoxaThread(SERVER *server)
{
int status;
THREAD tid;
static char *fid = "StartMoxaThread";
     
    MUTEX_INIT(&mutex);

    InitReading(AERRS, &CurrentScan.aerrs);
    InitReading(AES01, &CurrentScan.aes01);
    InitReading(AEA01, &CurrentScan.aea01);
    InitReading(AEF01, &CurrentScan.aef01);
    InitReading(AEV01, &CurrentScan.aev01);
    InitReading(AEC01, &CurrentScan.aec01);
    InitReading(AE101, &CurrentScan.ae101);
    InitReading(AE201, &CurrentScan.ae201);
    InitReading(AES02, &CurrentScan.aes02);
    InitReading(AEA02, &CurrentScan.aea02);
    InitReading(AEF02, &CurrentScan.aef02);
    InitReading(AEV02, &CurrentScan.aev02);
    InitReading(AEC02, &CurrentScan.aec02);

    if (!THREAD_CREATE(&tid, MoxaThread, (void *) server)) {
        LogMsg("%s: THREAD_CREATE: %s", fid, strerror(errno));
        Exit(MY_MOD_ID);
    }

}

/*-----------------------------------------------------------------------+
 |                                                                       |
 | Copyright (C) 2012 Regents of the University of California            |
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
 * $Log: moxa.c,v $
 * Revision 1.4  2015/12/09 18:44:43  dechavez
 * various minor tweaks to get clean Mac OS X builds
 *
 * Revision 1.3  2012/05/02 18:30:48  dechavez
 * *** initial production release ***
 *
 * Revision 1.2  2012/04/26 17:38:16  dechavez
 * split sample into slave1 and slave2, and added aex channel.  GetSample
 * returns false until first reading from either slave is obtained.  In the
 * event of read errors, the slave sample is preserved but the aex channel is
 * set to -1 after first read so that you can determine when the remaining
 * channels are valid.  This is to prevent the occasional timeout -1 spike
 * from rendering the time series unreadable.
 *
 * Revision 1.1  2012/04/25 21:21:04  dechavez
 * initial release
 *
 */
