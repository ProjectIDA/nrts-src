#pragma ident "$Id: wf28to30.c,v 1.5 2013/12/02 21:10:58 dechavez Exp $"
/*======================================================================
 *
 *  Convert a 2.8 wfdisc record to 3.0.
 *
 * - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 * Copyright (c) 1997 Regents of the University of California.
 * All rights reserved.
 *====================================================================*/
#include "cssio.h"

void wf28to30(wfdisc30, wfdisc28)
struct cssio_wfdisc *wfdisc30;
struct cssio_wfdisc28 *wfdisc28;
{
    *wfdisc30 = cssio_wfdisc_null;

    strlcpy(wfdisc30->sta,      wfdisc28->sta, CSSIO_STA_LEN+1);
    strlcpy(wfdisc30->chan,     wfdisc28->chan, CSSIO_CHAN_LEN+1);
            wfdisc30->time    = wfdisc28->time;
            wfdisc30->wfid    = wfdisc28->wfid;
            wfdisc30->chanid  = wfdisc28->chid;
            wfdisc30->jdate   = wfdisc28->date;
            wfdisc30->nsamp   = wfdisc28->nsamp;
            wfdisc30->smprate = wfdisc28->smprat;
            wfdisc30->calib   = wfdisc28->calib;
            wfdisc30->calper  = wfdisc28->calper;
    strlcpy(wfdisc30->instype,  wfdisc28->instyp, CSSIO_INSTYPE_LEN+1);
            wfdisc30->segtype = wfdisc28->segtyp;
    strlcpy(wfdisc30->datatype, wfdisc28->dattyp, CSSIO_DATATYPE_LEN+1);
            wfdisc30->clip    = wfdisc28->clip;
    strlcpy(wfdisc30->dir,      wfdisc28->dir, CSSIO_DIR_LEN+1);
    strlcpy(wfdisc30->dfile,    wfdisc28->file, CSSIO_DFILE_LEN+1);
            wfdisc30->foff    = wfdisc28->foff;

    wfdisc30->endtime = wfdisc30->time + (double) (wfdisc30->nsamp-1)/(double) wfdisc30->smprate;
}

/* Revision History
 *
 * $Log: wf28to30.c,v $
 * Revision 1.5  2013/12/02 21:10:58  dechavez
 * compute endtime using double precision arithmitic
 *
 * Revision 1.4  2011/03/17 17:46:30  dechavez
 * fixed some overlooked wfdisc_null occurences, changing them to cssio_wfdisc_null
 *
 * Revision 1.3  2011/03/17 17:21:09  dechavez
 * changed all macros and constants to use names with CSSIO_ or cssio_ prefixes
 * in order to avoid conflicts with IDA DCC source code
 *
 * Revision 1.2  2007/01/07 17:33:09  dechavez
 * strlcpy() instead of strcpy()
 *
 * Revision 1.1.1.1  2000/02/08 20:20:23  dec
 * import existing IDA/NRTS sources
 *
 */
