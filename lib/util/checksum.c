#pragma ident "$Id: checksum.c,v 1.1 2011/01/25 18:14:56 dechavez Exp $"
/*======================================================================
 *
 * utilIpHeaderChecksum() - IP header checksum
 * utilUdpChecksum() - UDP checksum
 * util_fletcher16()
 * util_fletcher32()
 * Fletcher's checksums, adapted from
 * http://en.wikipedia.org/wiki/Fletcher's_checksum
 *
 *====================================================================*/
#include "util.h"

UINT16 utilIpHeaderChecksum(UINT8 *hdr, int len)
{
int i;
INT32 sum = 0;
UINT16 value;

    while (len > 0) {
        value = *((UINT16 *) hdr);
        sum += *((UINT16 *) hdr);
        if (sum & 0x80000000) sum = (sum & 0xFFFF) + (sum >> 16);
        len -= 2;
        hdr += 2;
    }

    if (len > 0) sum += (UINT16) *hdr; /* take care of odd byte */

/* Keep only the last 16 bits of the sum, and add the carries */

    while (sum >> 16) sum = (sum & 0xFFFF) + (sum >> 16);

/* Return the one's complement */

    return (UINT16) ~sum;
}

UINT16 utilUdpChecksum(UINT8 *udphdr, UINT32 srcip, UINT32 dstip, UINT8 *payload, int len)
{
int i;
UINT16 *p16;
UINT32 sum = 0;
#define UDP_HDR_LEN 8
#define UDP_FLAG    0x11
#define PSUEDO_BUFLEN 12
UINT8 *ptr, psuedo[PSUEDO_BUFLEN];

/* Psuedo header first */

    ptr = psuedo;
    ptr += utilPackUINT32(ptr, srcip);
    ptr += utilPackUINT32(ptr, dstip);
    ptr += utilPackUINT16(ptr, UDP_FLAG);
    ptr += utilPackUINT16(ptr, len + UDP_HDR_LEN);

    for (i = 0, p16 = (UINT16 *) psuedo; i < PSUEDO_BUFLEN/2; i++) sum += p16[i];

/* Now the UDP header, skipping the checksum */

    for (i = 0, p16 = (UINT16 *) udphdr; i < (UDP_HDR_LEN - 2)/ 2; i++) sum += p16[i];

/* Now the data */

    for (i = 0, p16 = (UINT16 *) payload; i < len/2; i++) sum += p16[i];
    if (len % 2) sum += (UINT16) payload[len-1] << 8; /* take care of the odd byte */

/* Keep only the last 16 bits of the sum, and add the carries */

    while (sum >> 16) sum = (sum & 0xFFFF) + (sum >> 16);

/* Return the one's complement */

    return ~sum;
}

UINT32 util_fletcher32(UINT16 *data, size_t len)
{
uint32_t sum1 = 0xffff, sum2 = 0xffff;
 
    while (len) {
            unsigned tlen = len > 360 ? 360 : len;
            len -= tlen;
            do {
                    sum1 += *data++;
                    sum2 += sum1;
            } while (--tlen);
            sum1 = (sum1 & 0xffff) + (sum1 >> 16);
            sum2 = (sum2 & 0xffff) + (sum2 >> 16);
    }

    /* Second reduction step to reduce sums to 16 bits */

    sum1 = (sum1 & 0xffff) + (sum1 >> 16);
    sum2 = (sum2 & 0xffff) + (sum2 >> 16);

    return sum2 << 16 | sum1;
}

void util_fletcher16(UINT8 *checkA, UINT8 *checkB, UINT8 *data, size_t len)
{
UINT16 sum1 = 0xff, sum2 = 0xff;
 
    while (len) {
            size_t tlen = len > 21 ? 21 : len;
            len -= tlen;
            do {
                    sum1 += *data++;
                    sum2 += sum1;
            } while (--tlen);
            sum1 = (sum1 & 0xff) + (sum1 >> 8);
            sum2 = (sum2 & 0xff) + (sum2 >> 8);
    }

    /* Second reduction step to reduce sums to 8 bits */

    sum1 = (sum1 & 0xff) + (sum1 >> 8);
    sum2 = (sum2 & 0xff) + (sum2 >> 8);
    *checkA = (UINT8)sum1;
    *checkB = (UINT8)sum2;

    return;
}

/* Revision History
 *
 * $Log: checksum.c,v $
 * Revision 1.1  2011/01/25 18:14:56  dechavez
 * created
 *
 */
