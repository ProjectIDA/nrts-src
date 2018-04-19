#pragma ident "$Id: packet.c,v 1.3 2018/01/18 02:45:08 dauerbach Exp $"
/*======================================================================
 *
 *  proto-packet functions
 *
 *====================================================================*/
#define INCLUDE_IACP_DEFAULT_ATTR
#include "isi330.h"

#define MY_MOD_ID ISI330_MOD_PACKET

static ISI330_CONFIG *lcfg = NULL;
static ISI_PUSH *ph = NULL;
static int isi_que_depth = DEFAULT_PACKET_QUEUE_DEPTH;


static void FlushRecordToISISVR(UINT8 *rawmseed)
{
    static char *fid = "FlushRecordToISISVR";
    static BOOL FirstRecord = TRUE;
    UINT8 ida1012[IDA10_FIXEDRECLEN];

    if (mseed512ToIDA1012(rawmseed, ida1012, lcfg->sta, lcfg->netname, lcfg->q330->sn) == NULL) {

        LogMsg("ERROR: %s: mseed512ToIDA1012: %s", fid, strerror(errno));
        return;
    }

    if (!isiPushRawPacket(ph, ida1012, IDA10_FIXEDRECLEN, ISI_TYPE_IDA10)) {
        LogMsg("ERROR: %s: isiPushRawPacket: %s", strerror(errno));
        GracefulExit(MY_MOD_ID + 1);
    }

    if (FirstRecord) {
        LogMsg("initial packet enqueued to ISI push server@%s:%d\n", ph->server, ph->port);
        LogMsg("initial packet sta=%s  net=%s  sn=%016llx\n", lcfg->sta, lcfg->netname, lcfg->q330->sn);
        FirstRecord = FALSE;
    }
}

static void FlushRecordToFile(FILE *outfl, UINT8 *rawmseed)
{
    static char *fid = "FlushRecordToFile";
    MSEED_HDR mshdr;
    int outcnt;

    if (mseedUnpackFSDH(&mshdr, rawmseed)) {
         mseedPrintHeader(stderr, &mshdr);
    }
    outcnt = fwrite(rawmseed, 1, 512, outfl);
    if (outcnt != 512) {
        LogMsg("ERROR: write expected to write %d, wrote %d: ", 512, outcnt);
        GracefulExit(MY_MOD_ID + 2);
    }

}

static void InitOutputISISVR()
{
   static char *fid = "InitOutputISISVR";
   IACP_ATTR attr = IACP_DEFAULT_ATTR;

   if ((ph = isiPushInit(lcfg->server, lcfg->port, &attr, lcfg->lp, LOG_INFO, IDA10_FIXEDRECLEN, isi_que_depth, FALSE)) == NULL) {
      LogMsg("ERROR: %s:isiPushInit: %s", fid, strerror(errno));
      GracefulExit(MY_MOD_ID + 2);
   }
}

static void InitOutputFile()
{
    static char *fid = "InitOutputFile";

    // Do nothing for stdout

    /* open file, get handle when/ifwe allow filename specified on cmdline. */
}

void FlushRecord(UINT8 *rawmseed)
{
    static char *fid = "FlushRecord";

    if (rawmseed == NULL) return;

    switch (lcfg->outputType) {

        case ISI330_OUTPUT_TYPE_ISISVR:
            FlushRecordToISISVR(rawmseed);
            break;

        case ISI330_OUTPUT_TYPE_STDOUT:
            FlushRecordToFile(lcfg->outfl, rawmseed);
            break;

        default:
            break;


    }

    return;
}

void InitOutput(ISI330_CONFIG *cfg)
{
    static char *fid = "InitOutput";

    lcfg = cfg;

    switch (lcfg->outputType) {

        case ISI330_OUTPUT_TYPE_ISISVR:
            InitOutputISISVR();
            break;

        case ISI330_OUTPUT_TYPE_STDOUT:
            InitOutputFile();
            break;

        default:
            break;
    }
}

/*-----------------------------------------------------------------------+
 |                                                                       |
 | Copyright (C) 2017, 2018 Regents of the University of California      |
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
