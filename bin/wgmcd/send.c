#pragma ident "$Id: send.c,v 1.1 2012/07/03 16:15:15 dechavez Exp $"
/*======================================================================
 *
 *  Code to send things to the wave glider (one day)
 *
 *====================================================================*/
#include "wgmcd.h"

VOID SendOutboundCommands(CLIENT *client)
{
    iacpSendNull(client->iacp);
}

/* Revision History
 *
 * $Log: send.c,v $
 * Revision 1.1  2012/07/03 16:15:15  dechavez
 * initial (barely working) release
 *
 */
