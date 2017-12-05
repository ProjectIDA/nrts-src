#pragma ident "$Id: ida10.c,v 1.1 2017/09/28 18:20:44 dauerbach Exp $"
/*======================================================================
 *
 *  Build IDA10 packet
 *
 *====================================================================*/
#include "siomet.h"

void BuildIda10(PROTO_PACKET *pkt, UINT8 *dest)
{
int i;
INT16 srfact, srmult;
UINT8 *ptr, descriptor;
IDA10_GENTAG gentag = {0, 0, IDA10_GENTAG_LOCKED};

    ptr = dest;

    /* 10.8 common header */

    ptr += utilPackBytes(ptr, (UINT8 *) "TS", 2);
    *ptr++ = 10; /* format 10 */
    *ptr++ = 8;  /* subformat 8 */
    ptr += utilPackBytes(ptr, (UINT8 *) pkt->sname, IDA105_SNAME_LEN);
    ptr += utilPackBytes(ptr, (UINT8 *) pkt->nname, IDA105_NNAME_LEN);
    gentag.tstamp = pkt->tofs;
    ptr += ida10PackGenericTtag(ptr, &gentag);
    ptr += utilPackUINT32(ptr, 0); /* seqno will get updated when written to disk */
    ptr += utilPackUINT32(ptr, 0); /* timestamp will get updated when written to disk */
    ptr += IDA108_RESERVED_BYTES;
    ptr += utilPackUINT16(ptr, IDA10_FIXED_NBYTES);

    /* end of common header */

    /* use chnlc for stream name */

    ptr += utilPackBytes(ptr, (UINT8 *) pkt->cname, IDA10_CNAMLEN);

    /* data format/status descriptor */

    descriptor = 0;
    descriptor |= IDA10_COMP_NONE;
    descriptor |= IDA10_A2D_24;
    descriptor |= IDA10_MASK_INT32;
    *ptr++ = descriptor;

    /* conversion gain */

    *ptr++ = 1;

    /* number of samples */

    ptr += utilPackINT16(ptr, pkt->nsamp);

    /* sample rate */

    ida10SintToFactMult(((REAL64) pkt->nsint / NANOSEC_PER_SEC), &srfact, &srmult);
    ptr += utilPackINT16(ptr, srfact);
    ptr += utilPackINT16(ptr, srmult);

    /* the first data */

    for (i = 0; i < pkt->nsamp; i++) ptr += utilPackINT32(ptr, pkt->data[i]);
}

/*-----------------------------------------------------------------------+
 |                                                                       |
 | Copyright (C) 2017 Regents of the University of California            |
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
 * $Log: ida10.c,v $
 * Revision 1.1  2017/09/28 18:20:44  dauerbach
 * initial release
 *
 */
