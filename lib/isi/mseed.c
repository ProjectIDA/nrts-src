#pragma ident "$Id: mseed.c,v 1.1 2011/11/07 17:35:00 dechavez Exp $"
/*======================================================================
 *
 *  "convert" an ISI_RAW_PACKET payload to MiniSEED, if possible.
 *
 *====================================================================*/
#include "isi.h"

int isiRawToMiniSeed(ISI_RAW_PACKET *raw)
{
/* Nothing to do if already MiniSEED */

    if (raw->hdr.desc.type == ISI_TYPE_MSEED) return ISI_TYPE_MSEED;

/* Nothing to do if we don't have a MiniSEED equivalent already available */

    if (raw->hdr.mseed[0] == 0) return raw->hdr.desc.type;

/* IDA packets can get converted by replacing the header */

    switch (raw->hdr.desc.type) {
      case ISI_TYPE_IDA5:
      case ISI_TYPE_IDA6:
      case ISI_TYPE_IDA7:
      case ISI_TYPE_IDA8:
      case ISI_TYPE_IDA9:
      case ISI_TYPE_IDA10:
        memcpy(raw->payload, raw->hdr.mseed, ISI_MSEED_HDR_LEN);
        raw->hdr.desc.type = ISI_TYPE_MSEED;
        break;
    }
    return raw->hdr.desc.type;
}

/* Revision History
 *
 * $Log: mseed.c,v $
 * Revision 1.1  2011/11/07 17:35:00  dechavez
 * created: isiRawToMiniSeed()
 *
 */
