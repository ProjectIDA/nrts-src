#pragma ident "$Id: wfdisc.c,v 1.1 2015/10/02 15:46:39 dechavez Exp $"
/*======================================================================
 *
 *  Wfdisc related routines.
 *
 *====================================================================*/
#include "msdmx.h"

#define MY_MOD_ID MSDMX_MOD_WFDISC

void InitWfdisc(WFDISC *wfdisc, MSEED_HDR *hdr)
{
char *datatype;
static char *fid = "InitWfdisc";

    if ((datatype = mseedCSSDatatypeString(hdr)) == NULL) {
        LogErr("%s: unsupported datatype '%d'\n", fid, hdr->format);
        GracefulExit(MY_MOD_ID + 1);
    }

    *wfdisc = cssio_wfdisc_null;
    strncpy(wfdisc->sta, hdr->staid, CSSIO_STA_LEN+1); util_lcase(wfdisc->sta);
    strncpy(wfdisc->chan, mseedChnloc(hdr, NULL), CSSIO_CHAN_LEN+1); util_lcase(wfdisc->chan);
    wfdisc->time = utilConvertFrom1999NsecTo1970Secs(hdr->tstamp);
    wfdisc->jdate = utilYearDay(wfdisc->time);
    wfdisc->endtime = utilConvertFrom1999NsecTo1970Secs(hdr->endtime);
    wfdisc->nsamp = 0; /* will get set after record is written to disk */
    wfdisc->smprate = (REAL64) NANOSEC_PER_SEC / hdr->sint;
    strncpy(wfdisc->datatype, datatype, CSSIO_DATATYPE_LEN+1);
    strncpy(wfdisc->dir, MSDMX_DATA_DIR_NAME, CSSIO_DIR_LEN+1);
    snprintf(wfdisc->dfile, CSSIO_DFILE_LEN+1, "%s-%s.w", wfdisc->sta, wfdisc->chan);
}

/* Revision History
 *
 * $Log: wfdisc.c,v $
 * Revision 1.1  2015/10/02 15:46:39  dechavez
 * initial production release
 *
 */
