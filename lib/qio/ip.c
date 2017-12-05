#pragma ident "$Id: ip.c,v 1.3 2014/08/11 18:01:19 dechavez Exp $"
/*========================================================================
 *
 * IP header stuff
 *
 *======================================================================*/
#include "qio.h"

int qioPackIpHeader(UINT8 *start, QIO *qio, UINT32 destinationIP, int len)
{
UINT8 *ptr, *psum;

    ptr = start;
    *ptr++ = ((QIO_IP_VERSION << 4) | (QIO_IP_HDRLEN >> 2));
    *ptr++ = QIO_IP_DS;
    ptr += utilPackUINT16(ptr, len + QIO_IP_HDRLEN + QIO_UDP_HDRLEN);
    ptr += utilPackUINT16(ptr, qio->xmit.ident++);
    *ptr++ = QIO_IP_FLAGS;   /* we cheat because 5 bits of this belong next, but are always zero */
    *ptr++ = QIO_IP_FRAGOFF; /* always zero in this implementation */
    *ptr++ = QIO_IP_TTL;
    *ptr++ = QIO_IP_UDP;
    psum = ptr; ptr += utilPackUINT16(ptr, 0); /* leave checksum zero for now */
    ptr += utilPackUINT32(ptr, qio->my.ip);
    ptr += utilPackUINT32(ptr, destinationIP);
    utilPackUINT16(psum, htons(utilIpHeaderChecksum(start, QIO_IP_HDRLEN)));

    return (int) (ptr - start);
}

BOOL qioVerifyIpHeader(UINT8 *start)
{
#define IP_CHECKSUM_OFFSET 10
UINT16 expected, actual;
    
    utilUnpackUINT16(&start[IP_CHECKSUM_OFFSET], &expected);
    utilPackUINT16(&start[IP_CHECKSUM_OFFSET], 0);
    actual = htons(utilIpHeaderChecksum(start, QIO_IP_HDRLEN));

    return (actual == expected) ? TRUE : FALSE;
}

UINT32 qioIpHeaderSrcIP(UINT8 *start)
{
#define SRCIP_OFFSET 12
UINT32 ip;

    utilUnpackUINT32(&start[SRCIP_OFFSET], &ip);
    return ip;
}

UINT32 qioIpHeaderDstIP(UINT8 *start)
{
#define DSTIP_OFFSET 16
UINT32 ip;

    utilUnpackUINT32(&start[DSTIP_OFFSET], &ip);
    return ip;
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
 * $Log: ip.c,v $
 * Revision 1.3  2014/08/11 18:01:19  dechavez
 *  MAJOR CHANGES TO SUPPORT Q330 DATA COMM OVER SERIAL PORT (see 8/11/2014 comments in version.c)
 *
 * Revision 1.2  2011/02/08 21:56:34  dechavez
 * removed checksum fail message
 *
 * Revision 1.1  2011/01/25 18:35:11  dechavez
 * initial release, checksum mismatches debugged to stdout
 *
 */
