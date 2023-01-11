#pragma ident "$Id: header.c,v 1.4 2015/11/13 20:53:16 dechavez Exp $"
/*======================================================================
 * 
 * Pack MiniSEED header (FSDH + B1000 + (B1001) + (B100))
 *
 *====================================================================*/
#include "mseed.h"

static int PackSeqno(UINT8 *start, INT32 seqno)
{
UINT8 *ptr;
char tmptxt[7];

    sprintf(tmptxt, "%06d", seqno);

    ptr = start;
    ptr += utilPackBytes(ptr, (UINT8 *) tmptxt, strlen(tmptxt));

    return (int) (ptr - start);
}

static int PackStaId(UINT8 *start, char *staid)
{
UINT8 *ptr;
char tmptxt[MSEED_SNAMLEN+1];

    sprintf(tmptxt, "%-5s", staid);
    utilPadString(tmptxt, MSEED_SNAMLEN, ' ');

    ptr = start;
    ptr += utilPackBytes(ptr, (UINT8 *) tmptxt, strlen(tmptxt));

    return (int) (ptr - start);
}

static int PackLocId(UINT8 *start, char *locid)
{
UINT8 *ptr;
char tmptxt[MSEED_LNAMLEN+1];

    sprintf(tmptxt, "%-2s", locid);
    utilPadString(tmptxt, MSEED_LNAMLEN, ' ');

    ptr = start;
    ptr += utilPackBytes(ptr, (UINT8 *) tmptxt, strlen(tmptxt));

    return (int) (ptr - start);
}

static int PackChnId(UINT8 *start, char *chnid)
{
UINT8 *ptr;
char tmptxt[MSEED_CNAMLEN+1];

    sprintf(tmptxt, "%-3s", chnid);
    utilPadString(tmptxt, MSEED_CNAMLEN, ' ');

    ptr = start;
    ptr += utilPackBytes(ptr, (UINT8 *) tmptxt, strlen(tmptxt));

    return (int) (ptr - start);
}

static int PackNetId(UINT8 *start, char *netid)
{
UINT8 *ptr;
char tmptxt[MSEED_NNAMLEN+1];

    sprintf(tmptxt, "%-2s", netid);
    utilPadString(tmptxt, MSEED_NNAMLEN, ' ');

    ptr = start;
    ptr += utilPackBytes(ptr, (UINT8 *) tmptxt, strlen(tmptxt));

    return (int) (ptr - start);
}

static int PackBtime(UINT8 *start, int year, int day, int hr, int mn, int sc, int frac)
{
UINT8 *ptr;

    ptr = start;
    ptr += utilPackUINT16(ptr, (UINT16) year);
    ptr += utilPackUINT16(ptr, (UINT16) day);
    *ptr++ = (UINT8) hr;
    *ptr++ = (UINT8) mn;
    *ptr++ = (UINT8) sc;
    *ptr++ = 0;
    ptr += utilPackUINT16(ptr, (UINT16) frac);

    return (int) (ptr - start);
}

BOOL mseedPackHeader(MSEED_HANDLE *handle, MSEED_PACKED *packed, MSEED_RECORD *record, int nsamp, int nframe, int format)
{
int b100, b1001;
INT32 seqno;
UINT8 *ptr, nblockettes;
UINT16 next;
REAL32 actual;
int fact, mult, lenp2;
char *staid, *netid, *chnid, *locid;
int year, day, hr, mn, sc, nsec, microsec, frac;
INT8 usec; // for blockette 1001 microsecond offset
static char space = ' ';
static char *fid = "mseedPackHeader";

    if (handle == NULL || packed == NULL || record == NULL) {
        errno = EINVAL;
        return FALSE;
    }

/* Sequence numbers will increment either globally (if handle seqno is defined) or by channel */

    seqno = record->hdr.seqno = mseedIncrementSeqno(handle, record->hdr.ident);

/* The station and network names can be superceded by what's in the handle */

    staid = (handle->staid != NULL) ? handle->staid : record->hdr.staid;
    netid = (handle->netid != NULL) ? handle->netid : record->hdr.netid;

/* Derive the factor/multilpier values for the nominal sample interval */

    mseedNsintToFactMult(record->hdr.nsint, &fact, &mult);

/* Split time time stamp into BTIME components */

    utilDecomposeTimestamp(record->hdr.tstamp, &year, &day, &hr, &mn, &sc, &nsec);
    microsec = nsec / USEC_PER_NANOSEC;
    frac = microsec / 100; // microsecond fraction value but with only tenths of millisecond precision, i.e last 2 digits are 0
    usec = microsec - frac * 100; // usec for blockette 1001 is the OFFSET from toms (tenths of milliseconds) timestamp in FSDH. Should be positive (0-99)

/* We will always include blockette 1000.  Blockette 1001 is included if we have
 * time quality and blockette 100 is included if we have actual sample interval.
 */

    b1001 = (record->hdr.tqual != -1) ? 1 : 0;
    b100  = (record->hdr.asint  >  0) ? 1 : 0;

    nblockettes = 1 + b1001 + b100;

/* Pack the FSDH */

    ptr = packed->data;
    ptr += PackSeqno(ptr, seqno);                       /* Sequence number */
    *ptr++ = handle->qc;                                /* Data header/quality indicator */
    *ptr++ = space;                                     /* Reserved byte */
    ptr += PackStaId(ptr, staid);                       /* Station identifier code */
    ptr += PackLocId(ptr, record->hdr.locid);           /* Location identifier code */
    ptr += PackChnId(ptr, record->hdr.chnid);           /* Channel identifier code */
    ptr += PackNetId(ptr, netid);                       /* Network identifier code */
    ptr += PackBtime(ptr, year, day, hr, mn, sc, frac); /* Record start time */
    ptr += utilPackUINT16(ptr, nsamp);                  /* Number of samples */
    ptr += utilPackUINT16(ptr, (UINT16) fact);          /* Sample rate factor */
    ptr += utilPackUINT16(ptr, (UINT16) mult);          /* Sample rate multiplier */
    *ptr++ = record->hdr.flags.act;                     /* Activity flags */
    *ptr++ = record->hdr.flags.ioc;                     /* I/O and clock flags */
    *ptr++ = record->hdr.flags.dat;                     /* Data quality flags */
    *ptr++ = nblockettes;                               /* Number of blockettes that follow */
    ptr += utilPackUINT32(ptr, record->hdr.tcorr);      /* Time correction */
    ptr += utilPackUINT16(ptr, record->hdr.bod);        /* Begining of data */
    ptr += utilPackUINT16(ptr, (UINT16) (ptr - packed->data) + sizeof(UINT16)); /* First blockette follows immediately */

/* Blockette 1000 is required */

    next = (nblockettes > 1) ? (UINT16) (ptr - packed->data) + MSEED_B1000_LEN : 0;
    lenp2 = util_powerof(packed->len, 2);

    ptr += utilPackUINT16(ptr, 1000);             /* Blockette type - 1000 */
    ptr += utilPackUINT16(ptr, next);             /* Next blockette's byte number */
    *ptr++ = (UINT8) format;                      /* Encoding format */
    *ptr++ = MSEED_BIG_ENDIAN;                    /* Word order */
    *ptr++ = (UINT8) lenp2;                       /* Data record length */
    *ptr++ = 0;                                   /* Reserved */

/* If applicable, derive and pack in blockette 1001 */

    if (b1001) {
        next = (record->hdr.asint > 0) ? (UINT16) (ptr - packed->data) + MSEED_B1001_LEN : 0;

        ptr += utilPackUINT16(ptr, 1001);             /* Blockette type - 1001 */
        ptr += utilPackUINT16(ptr, next);             /* Next blockette's byte number */
        *ptr++ = (UINT8) record->hdr.tqual;           /* Timing quality */
        *ptr++ = usec;                                /* usec */
        *ptr++ = 0;                                   /* reserved */
        *ptr++ = (UINT8) nframe;                      /* gag */
    }

/* If applicable, derive and pack in blockette 100 */

    if (b100) {
        actual = (REAL32) NANOSEC_PER_SEC / (REAL32) (record->hdr.asint);

        ptr += utilPackUINT16(ptr, 100);              /* Blockette type - 100 */
        ptr += utilPackUINT16(ptr, 0);                /* No more blockettes to follow */
        ptr += utilPackREAL32(ptr, actual);           /* Actual sample rate */
        *ptr++ = 0;                                   /* Flags are undefined */
        *ptr++ = 0;                                   /* padding */
        *ptr++ = 0;                                   /* padding */
        *ptr++ = 0;                                   /* padding */
    }

    return TRUE;
}

/*-----------------------------------------------------------------------+
 |                                                                       |
 | Copyright (C) 2014 Regents of the University of California            |
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
 * $Log: header.c,v $
 * Revision 1.4  2015/11/13 20:53:16  dechavez
 * fixed up some sprintf format vs type inconsitencies, added some casts to calm Darwin compile
 *
 * Revision 1.3  2014/08/26 17:30:58  dechavez
 * fixed blockette 100 bug (had been setting sample interval instead of rate)
 *
 * Revision 1.2  2014/08/19 18:02:08  dechavez
 * rework to use MSEED_PACKED
 *
 * Revision 1.1  2014/08/11 18:18:01  dechavez
 * initial release
 *
*/
