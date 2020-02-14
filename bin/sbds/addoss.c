#pragma ident "$Id: addoss.c,v 1.3 2015/07/17 18:58:09 dechavez Exp $"
/*======================================================================
 *
 *  Deal with ADDOSS payloads
 *
 *====================================================================*/
#include "sbds.h"
#include "addoss.h"

static DBIO *db;
static BOOL seedlink;

static void SaveIDA10(ISI_DL *dl, UINT8 *ida10)
{
ISI_RAW_PACKET pkt;
IDA10_TS ts;
static char *fid = "SaveIDA10";

    isiInitRawPacket(&pkt, ida10, ADDOSS_IDA10_PKTLEN);
    strcpy(pkt.hdr.site, dl->sys->site);
    pkt.hdr.desc.type   = ISI_TYPE_IDA10;
    pkt.hdr.desc.comp   = ISI_COMP_NONE;
    pkt.hdr.desc.order  = ISI_ORDER_UNDEF;
    pkt.hdr.desc.size   = sizeof(UINT8);
    pkt.hdr.len.payload = pkt.hdr.len.used = pkt.hdr.len.native = ADDOSS_IDA10_PKTLEN;

    if (!AppSuppliedWritePacketToDisk(dl, &pkt)) {
        LogMsg(LOG_INFO, "%s: AppSuppliedWritePacketToDisk failed: %s\n", fid, strerror(errno));
    } else {    
        ida10UnpackTS(ida10, &ts);
        LogMsg(LOG_INFO, "TS %s\n", ida10TStoString(&ts, NULL));
    }
}

static void SaveSensorData(ADDOSS_SENSOR_DATA_INT24 *sensor, SBD_MO_HEADER *header, UINT64 zerotime, ISI_DL *dl)
{
int len;
UINT8 ida10[IDA10_MAXRECLEN];
static char *fid = "SaveSensorData";

    if ((len = addossIDA10(ida10, sensor, header, (INT32) (zerotime / NANOSEC_PER_SEC))) < 0) {
        LogMsg(LOG_INFO, "*** WARNING *** addossIDA10 error %d, frame dropped\n", len);
        return;
    }

    SaveIDA10(dl, ida10);
}

void ProcessADDOSS(SBD_MESSAGE *message)
{
UINT8 *ptr;
UINT16 type;
ADDOSS_SENSOR_DATA_INT24 addoss;
char site[IDA_SNAMLEN+1];
char net[IDA_NNAMLEN+1];
UINT64 zerotime;
ISI_DL *dl;
LNKLST *list;
LNKLST_NODE *crnt;
static char *fid = "ProcessADDOSS";

/* This check should already have been done by caller */

    if (!message->mo.header.valid || !message->mo.payload.valid) return;

/* Ignore message if we don't have site info for this IMEI */

    if (!isidbLookupSiteByIMEI(db, message->mo.header.imei, site, net, &zerotime)) {
        LogMsg(LOG_INFO, "*** WARNING *** %s: no site info for IMEI '%s'", fid, message->mo.header.imei);
        return;
    }

/* Ignore payload if we don't have an IDA10 disk loop for this site */

    if ((dl = GetIDA10dl(site, net, seedlink)) == NULL) {
        LogMsg(LOG_INFO, "*** WARNING *** %s: no disk loop for site '%s' (IMEI '%s')", fid, site, message->mo.header.imei);
        return;
    }

/* Note WG mailbox checks (empty payloads) */

    if (message->mo.payload.len == 0) {
        LogMsg(LOG_INFO, "empty message received (mailbox check)\n");
        return;
    }

/* Ingore invalid payloads */

    if (message->mo.payload.len % ADDOSS_FRAME_LEN) {
        LogMsg(LOG_INFO, "*** WARNING *** %s: SBD payload is not a multiple of %d (ignored)", fid, ADDOSS_FRAME_LEN);
        return;
    }
    LogMsg(LOG_INFO, "%d %d-byte ADDOSS frames received\n", message->mo.payload.len / ADDOSS_FRAME_LEN, ADDOSS_FRAME_LEN);
    
/* Extract the frames into a time ordered list */
     
    if ((list = addossExtractSensorDataInt24(message)) == NULL) {
        fprintf(stderr, "*** ERROR *** %s: addossExtractSensorDataInt24: %s\n", fid, strerror(errno));
        exit(1);
    }
             
/* Convert each frame to IDA10 and write to disk */
     
    crnt = listFirstNode(list);
    while (crnt != NULL) {
        SaveSensorData((ADDOSS_SENSOR_DATA_INT24 *) crnt->payload, &message->mo.header, zerotime, dl);
        crnt = listNextNode(crnt);
    }
             
    listDestroy(list);
}

void InitADOSSprocessor(PARAM *par)
{
    db = par->glob.db;
    seedlink = par->seedlink;
}

/* Revision History
 *
 * $Log: addoss.c,v $
 * Revision 1.3  2015/07/17 18:58:09  dechavez
 * rework to use user supplied function for writing data,
 * presort ADDOSS frames into time order to reduce artificial time tears
 * introduced when missed packets come in out of order.
 *
 * Revision 1.2  2013/07/19 18:00:09  dechavez
 * Log mailbox check messages, decode and print TS headers
 *
 * Revision 1.1  2013/05/11 23:03:58  dechavez
 * initial release
 *
 */
