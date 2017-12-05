#pragma ident "$Id: parse.c,v 1.4 2014/09/05 22:13:16 dechavez Exp $"
/*======================================================================
 * 
 * Parse a raw SBD message
 *
 *====================================================================*/
#include "sbd.h"
#include "util.h"

int sbdParseMessageBody(SBD_MESSAGE *message)
{
UINT16 len;
UINT8 *ptr, *eom, iei;
static char *fid = "sbdParseMessageBody";

    if (message == NULL) return SBD_ERROR;
    sbdClearMessageIEIs(message);

    ptr = message->body;
    if ((message->protocol = *ptr++) != 1) return SBD_EPROTO;
    utilUnpackUINT16(ptr, &message->len);
    if (message->len > SBD_MAX_MSGLEN) return SBD_TOOBIG;

    ptr = message->body + SBD_PROTOCOL_1_PREAMBLE_LEN;
    eom = message->body + message->len;

    while (ptr < eom) {
        iei = *ptr++;
        switch (iei) {
          case SBD_IEI_MO_HEADER:
            ptr += sbdUnpackMO_HEADER(ptr, &message->mo.header);
            break;
          case SBD_IEI_MO_PAYLOAD:
            ptr += sbdUnpackPayload(ptr, &message->mo.payload);
            break;
          case SBD_IEI_MO_LOCATION:
            ptr += sbdUnpackMO_LOCATION(ptr, &message->mo.location);
            break;
          case SBD_IEI_MO_CONFIRM:
            ptr += sbdUnpackMO_CONFIRM(ptr, &message->mo.confirm);
            break;
          case SBD_IEI_MT_HEADER:
            ptr += sbdUnpackMT_HEADER(ptr, &message->mt.header);
            break;
          case SBD_IEI_MT_PAYLOAD:
            ptr += sbdUnpackPayload(ptr, &message->mt.payload);
            break;
          case SBD_IEI_MT_CONFIRM:
            ptr += sbdUnpackMT_CONFIRM(ptr, &message->mt.confirm);
            break;
          case SBD_IEI_MT_PRIORITY:
            ptr += sbdUnpackMT_PRIORITY(ptr, &message->mt.priority);
            break;
          default:
            ptr += utilUnpackUINT16(ptr, &len);
            ptr += len; /* skip over unsupported IEs */
        }
    }

    message->len += SBD_PROTOCOL_1_PREAMBLE_LEN; /* since we include the preamble in the body */

    return SBD_OK;
}

/* Revision History
 *
 * $Log: parse.c,v $
 * Revision 1.4  2014/09/05 22:13:16  dechavez
 * fixed typo spelling "unsupported" incorrectly
 *
 * Revision 1.3  2013/03/13 21:30:07  dechavez
 * added support for all previously unspported IEs
 *
 * Revision 1.2  2013/03/11 22:58:55  dechavez
 * added SBD_MT_CONFIRM support
 *
 * Revision 1.1  2013/03/07 21:01:11  dechavez
 * initial release
 *
 */

