#pragma ident "$Id: misc.c,v 1.3 2011/10/10 19:53:08 dechavez Exp $"
/*======================================================================
 *
 *  Miscellaneous stuff
 *
 *====================================================================*/
#include "ida10.h"

/* determine offset to 32 bit sequence number for those packets that have it */

int ida10SeqnoOffset(UINT8 *packet)
{
    if (packet == NULL) return -1;
    if (packet[2] != 10) return -1;

    switch (packet[3]) {
      case 2:
      case 3:
      case 6:
      case 7: return 28;
      case 4: return 36;
      case 8: return 20;
    }

    return -1;
}

/* determine offset to 32 bit time stamp for those packets that have it */

int ida10TstampOffset(UINT8 *packet)
{
    if (packet == NULL) return -1;
    if (packet[2] != 10) return -1;

    switch (packet[3]) {
      case 2:
      case 3:
      case 6:
      case 7: return 32;
      case 4: return 40;
      case 8: return 24;
    }

    return -1;
}

/* insert a 32 bit sequence number into those packets that support it */

BOOL ida10InsertSeqno32(UINT8 *packet, UINT32 seqno)
{
int offset;

    if (packet == NULL) {
        errno = EINVAL;
        return FALSE;
    }

/* make sure it is an IDA10 packet */

    if (packet[2] != 10) {
        errno = EINVAL;
        return FALSE;
    }

/* find the appropriate offset, if any */

    switch (packet[3]) {
      case 2:
      case 3: offset = 28; break;
      case 4: offset = 36; break;
      default:
        return TRUE;
    }

    utilPackUINT32(&packet[offset], seqno);

    return TRUE;
}

BOOL ida10HaveOrigSeqno(IDA10_CMNHDR *hdr)
{
    if (hdr == NULL) return FALSE;

    switch (hdr->subformat) {
      case IDA10_SUBFORMAT_6:
      case IDA10_SUBFORMAT_7:
        return TRUE;
    }

    return FALSE;
}

/* Revision History
 *
 * $Log: misc.c,v $
 * Revision 1.3  2011/10/10 19:53:08  dechavez
 * added ida10SeqnoOffset() and ida10TstampOffset()
 *
 * Revision 1.2  2010/09/10 22:53:44  dechavez
 * added ida10HaveOrigSeqno()
 *
 * Revision 1.1  2006/12/22 02:46:59  dechavez
 * initial release
 *
 */
