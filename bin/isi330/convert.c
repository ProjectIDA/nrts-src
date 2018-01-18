/*======================================================================
 *
 *  Convert tonesec_call structure to MSEED_RECORD
 *
 *=+===================================================================*/
#include "isi330.h"

static char *ExtractNetid(char *station_name, char *dest)
{
    memcpy(dest, station_name, 2);
    dest[MSEED_NNAMLEN] = 0;

    return dest;
}

static UINT64 SampleIntervalInNsec(int rate)
{
UINT64 nsint;

    if (rate > 0) {
        nsint = NANOSEC_PER_SEC / rate;
    } else {
        nsint = NANOSEC_PER_SEC * 10; /* ASSUME 0.1 sps for ALL sub-Hz sample rates */
    }

    return nsint;
}

void OneSecPacketToMSEED_RECORD(MSEED_HANDLE *handle, MSEED_RECORD *dest, tonesec_call *src)
{
char *staid, *netid, netidbuf[MSEED_NNAMLEN+1];

    dest->seqno = 0; // let somebody else figure that out
    dest->hdr.asint = 0; // no "actual" sint since no drift available (unless cl_offset is it)

/* station and network names are either as-is from the data or user specified via the handle */
    staid = (handle->staid != NULL) ? handle->staid : (src->station_name + 3); /* skip over NN- */
    netid = (handle->netid != NULL) ? handle->netid : ExtractNetid(src->station_name, netidbuf);

    strncpy(dest->hdr.staid, staid, MSEED_SNAMLEN);
    strncpy(dest->hdr.netid, netid, MSEED_NNAMLEN);

/* presumably the QC field will be MSEED_QC_RAW ('R'), but the app gets to decide */

    dest->hdr.flags.qc = handle->qc;

/* evertything else is direct from the data */

    strncpy(dest->hdr.chnid, src->channel, MSEED_CNAMLEN);
    strncpy(dest->hdr.locid, src->location, MSEED_LNAMLEN);
    dest->hdr.nsamp = (src->rate > 0) ? src->rate : 1;
    dest->hdr.nsint = dest->hdr.sint = SampleIntervalInNsec(src->rate);
    dest->hdr.tstamp = utilConvertFrom2000SecsTo1999Nsec(src->timestamp);
    dest->hdr.tqual = src->qual_perc;
    dest->hdr.flags.act = src->activity_flags;
    dest->hdr.flags.ioc = src->io_flags;
    dest->hdr.flags.dat = src->data_quality_flags;
    dest->hdr.format = MSEED_FORMAT_INT_32;
    mseedCopyINT32(dest->dat.int32, src->samples, dest->hdr.nsamp);

    mseedSetEndtime(&dest->hdr);
    mseedSetIdent(&dest->hdr);
}
