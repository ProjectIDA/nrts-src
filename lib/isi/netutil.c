#pragma ident "$Id: netutil.c,v 1.2 2012/06/27 15:22:12 dechavez Exp $"
/*======================================================================
 *
 *  Additional convenience functions useful in networked apps
 *
 *====================================================================*/
#include "isi.h"
#include "util.h"

/* Set/Get ISI handle flag */

VOID isiSetFlag(ISI *isi, UINT8 value)
{
    if (isi == NULL) return;
    isi->flag = value;
}

UINT8 isiGetFlag(ISI *isi)
{
    if (isi == NULL) return ISI_FLAG_NOP;
    return isi->flag;
}

BOOL isiGetIacpStats(ISI *isi, IACP_STATS *send, IACP_STATS *recv)
{
    if (isi == NULL || send == NULL || recv == NULL) return FALSE;
    if (!iacpGetSendStats(isi->iacp, send)) return FALSE;
    if (!iacpGetRecvStats(isi->iacp, recv)) return FALSE;

    return TRUE;
}

/* Revision History
 *
 * $Log: netutil.c,v $
 * Revision 1.2  2012/06/27 15:22:12  dechavez
 * removed isiInitIncoming() and isiResetIncoming() (now part of libiacp)
 *
 * Revision 1.1  2006/02/09 00:09:20  dechavez
 * initial release
 *
 */
