#pragma ident "$Id: qdplus.c,v 1.10 2016/02/12 21:56:27 dechavez Exp $"
/*======================================================================
 *
 * QDPLUS packet support
 *
 *====================================================================*/
#include "isidl.h"

#define MY_MOD_ID ISIDL_MOD_QDPLUS

/* Offsets to QDPLUS_PKT header fields */

#define SERIALNO_OFFSET 0
#define SEQNO_OFFSET    8
#define PACKET_OFFSET   QDPLUS_HDRLEN

/* Add digitizer serial to QDPLUS payload */

void InsertQdplusSerialno(ISI_RAW_PACKET *raw, UINT64 serialno)
{
static char *fid = "InsertQdplusSerialno";

    utilPackUINT64(&raw->payload[SERIALNO_OFFSET], serialno);
}

/* Add sequence number to QDPLUS payload */

void CompleteQdplusHeader(ISI_RAW_PACKET *raw, ISI_DL *dl)
{
UINT8 *ptr;

    ptr = &raw->payload[SEQNO_OFFSET];
    ptr += utilPackUINT32(ptr, dl->sys->seqno.signature);
    ptr += utilPackUINT64(ptr, dl->sys->seqno.counter);
}

/* Copy QDP packet into QDPLUS payload */

void CopyQDPToQDPlus(ISI_RAW_PACKET *raw, QDP_PKT *src)
{
static char *fid = "CopyQDPToQDPlus";

    memcpy(&raw->payload[QDPLUS_HDRLEN], src->raw, src->len);
    raw->hdr.len.used = src->len + QDPLUS_HDRLEN;
}

/* Revision History
 *
 * $Log: qdplus.c,v $
 * Revision 1.10  2016/02/12 21:56:27  dechavez
 * removed old code related to barometer, ars, and pre-IDA10 packet support
 *
 * Revision 1.9  2015/12/09 18:34:54  dechavez
 * various minor tweaks to get clean Mac OS X builds
 *
 * Revision 1.8  2011/04/07 22:50:25  dechavez
 * build QDPLUS_DT_USER_PAROSCI packets
 *
 * Revision 1.7  2007/02/08 22:52:46  dechavez
 * use LOCALPKT handle
 *
 * Revision 1.6  2006/12/12 23:12:39  dechavez
 * removed all the misguided metadata stuff
 *
 * Revision 1.5  2006/12/08 17:36:48  dechavez
 * support for streamlined QDPLUS_PKT
 *
 * Revision 1.4  2006/06/30 18:18:02  dechavez
 * replaced message queues for processing locally acquired data with static buffers
 *
 * Revision 1.3  2006/06/23 18:29:40  dechavez
 * changed thread initialization message to debug
 *
 * Revision 1.2  2006/06/07 22:37:58  dechavez
 * fixed incorrect maxsamp assignment and a pointer problem in FinishPacket
 *
 * Revision 1.1  2006/06/02 21:09:07  dechavez
 * initial release
 *
 */
