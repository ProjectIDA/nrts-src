#pragma ident "$Id: read.c,v 1.1 2013/03/07 21:01:11 dechavez Exp $"
/*======================================================================
 * 
 * Read SBD messages from stdin (gzip compressed OK)
 *
 *====================================================================*/
#include "sbd.h"

BOOL sbdReadMessage(gzFile *gz, SBD_MESSAGE *message)
{
UINT8 *ptr;
static char *fid = "sbdReadMessage";

    if (message == (SBD_MESSAGE *) NULL) {
        errno = EINVAL;
        return FALSE;
    }

/* NULL handle is OK, we'll just read from standard in */

    if (gz == NULL && (gz = gzdopen(fileno(stdin), "r")) == NULL) return FALSE;

/* Read the preamble */

    if (gzread(gz, message->body, SBD_PROTOCOL_1_PREAMBLE_LEN) != SBD_PROTOCOL_1_PREAMBLE_LEN) return FALSE;

    ptr = message->body;
    message->protocol = *ptr++;
    ptr += utilUnpackUINT16(ptr, &message->len);

    if (message->protocol != 1) {
        errno = EINVAL;
        return FALSE;
    }

    if (message->len > SBD_MAX_MSGLEN) {
        errno = EINVAL;
        return FALSE;
    }

/* Read the contents */

    if (gzread(gz, message->body + SBD_PROTOCOL_1_PREAMBLE_LEN, message->len) != message->len) return FALSE;
    message->len += SBD_PROTOCOL_1_PREAMBLE_LEN; /* since we include the preamble in the body */

/* Parse the message */

    sbdParseMessageBody(message);

    return TRUE;
}

/* Revision History
 *
 * $Log: read.c,v $
 * Revision 1.1  2013/03/07 21:01:11  dechavez
 * initial release
 *
 */
