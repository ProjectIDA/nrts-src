#pragma ident "$Id: udp.c,v 1.3 2014/08/11 18:01:20 dechavez Exp $"
/*========================================================================
 *
 * UDP header stuff
 *
 *======================================================================*/
#include "qio.h"

int qioPackUdpHeader(UINT8 *start, QIO *qio, UINT32 destinationIP, int destinationPort, UINT8 *payload, int len)
{
UINT8 *ptr;

    ptr = start;
    ptr += utilPackUINT16(ptr, qio->my.port);
    ptr += utilPackUINT16(ptr, destinationPort);
    ptr += utilPackUINT16(ptr, len + QIO_UDP_HDRLEN);
    ptr += utilPackUINT16(ptr, htons(utilUdpChecksum(start, qio->my.ip, destinationIP, payload, len)));

    return (int) (start - ptr);
}

BOOL qioVerifyUdpHeader(UINT8 *start, UINT8 *iphdr, UINT8 *payload, int len)
{
#define UDP_CHECKSUM_OFFSET 6
UINT16 expected, actual;
UINT32 srcip, dstip;

    srcip = qioIpHeaderSrcIP(iphdr); /* source IP taken from IP header */
    dstip = qioIpHeaderDstIP(iphdr); /* destination IP taken from IP header */

    utilUnpackUINT16(&start[UDP_CHECKSUM_OFFSET], &expected);
    actual = htons(utilUdpChecksum(start, srcip, dstip, payload, len));

    return (actual == expected) ? TRUE : FALSE;
}

UINT16 qioUdpHeaderSrcPort(UINT8 *start)
{
#define SRCPORT_OFFSET 0
UINT16 port;

    utilUnpackUINT16(&start[SRCPORT_OFFSET], &port);
    return port;
}

UINT16 qioUdpHeaderDstPort(UINT8 *start)
{
#define DSTPORT_OFFSET 4
UINT16 port;

    utilUnpackUINT16(&start[DSTPORT_OFFSET], &port);
    return port;
}

/*-----------------------------------------------------------------------+
 |                                                                       |
 | Copyright (C) 2011 Regents of the University of California            |
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

/* Revision History
 *
 * $Log: udp.c,v $
 * Revision 1.3  2014/08/11 18:01:20  dechavez
 *  MAJOR CHANGES TO SUPPORT Q330 DATA COMM OVER SERIAL PORT (see 8/11/2014 comments in version.c)
 *
 * Revision 1.2  2011/01/31 18:16:16  dechavez
 * removed checksum fail message
 *
 * Revision 1.1  2011/01/25 18:35:12  dechavez
 * initial release, checksum mismatches debugged to stdout
 *
 * Revision 1.1  2011/01/12 23:19:34  dechavez
 * created
 *
 */
