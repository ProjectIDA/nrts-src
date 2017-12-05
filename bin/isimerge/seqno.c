#pragma ident "$Id: seqno.c,v 1.1 2010/09/10 22:56:46 dechavez Exp $"
/*======================================================================
 *
 *  Update sequence numbers
 *
 *====================================================================*/
#include "isimerge.h"

#define MY_MOD_ID ISIMERGE_MOD_SEQNO

static void UpdateIda9(ISI_SEQNO *new, UINT8 *buf)
{
}

static void UpdateIda10(ISI_SEQNO *new, UINT8 *buf)
{
int SeqnoOffset, SavedOffset, NewSubformat;
UINT32 old32, new32;
static char *fid = "UpdateIda10";

    if (ida10Type(buf) != IDA10_TYPE_TS) return;

    new32 = (UINT32) new->counter;
    switch (ida10SubFormatCode(buf)) {
      case IDA10_SUBFORMAT_2:
        SeqnoOffset = 28;
        SavedOffset = 36;
        NewSubformat = IDA10_SUBFORMAT_6;
        break;
      case IDA10_SUBFORMAT_3:
        SeqnoOffset = 28;
        SavedOffset = 36;
        NewSubformat = IDA10_SUBFORMAT_7;
        break;
      case IDA10_SUBFORMAT_4:
        SeqnoOffset = 36;
        SavedOffset = -1;
        NewSubformat = IDA10_SUBFORMAT_4;
        break;
      case IDA10_SUBFORMAT_5:
        SeqnoOffset = 20;
        SavedOffset = -1;
        NewSubformat = IDA10_SUBFORMAT_8;
        break;
      default:
        return;
    }
/* Save the original sequence number, if possible */

    if (SavedOffset > 0) {
        utilUnpackUINT32(&buf[SeqnoOffset], &old32);
        utilPackUINT32(&buf[SavedOffset], old32);
    }

/* Insert the new sequence number */

    if (SeqnoOffset > 0) {
        utilPackUINT32(&buf[SeqnoOffset], new32);
    }

/* Update the subformat type */

    buf[3] = NewSubformat;
}

void UpdateSequenceNumbers(ISI_SEQNO *new, ISI_RAW_PACKET *raw)
{
static char *fid = "UpdateSequenceNumbers";

/* save the original sequence number in the raw packet header */

    raw->hdr.oldseqno = raw->hdr.seqno;
    raw->hdr.seqno = *new;

/* and the 32bit versions in the payload, if they exist */

    switch (idaPacketFormat(raw->payload)) {
      case 9:
        UpdateIda9(new, raw->payload);
        break;
      case 10:
        UpdateIda10(new, raw->payload);
        break;
      default:
        break;
    }
}

/* Revision History
 *
 * $Log: seqno.c,v $
 * Revision 1.1  2010/09/10 22:56:46  dechavez
 * initial release
 *
 */
