#pragma ident "$Id: rwfdrec.c,v 1.3 2014/04/14 18:41:42 dechavez Exp $"
/*======================================================================
 *
 *  Read a single wfdisc record.  2.8 format wfdiscs are detected and
 *  converted to 3.0 format silently and automatically.
 *
 *  Records with incorrect length are silently ignored.
 *
 * - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 * Copyright (c) 1997 Regents of the University of California.
 * All rights reserved.
 *====================================================================*/
#include <stdio.h>
#include <stdlib.h>
#include "cssio.h"
#include "util.h"

int rwfdrec(FILE *fp, WFDISC *wfdisc)
{
char line[CSSIO_WFDISC_SIZE+2];
struct cssio_wfdisc28 wfdisc28;

    if (fp == NULL || wfdisc == NULL) {
        errno = EINVAL;
        return -1;
    }

    if (SETMODE(fileno(fp), O_BINARY) == -1) {
        perror("library routine rwfdrec: setmode");
        return -1;
    }

    while (1) {

        if (fgets(line, CSSIO_WFDISC_SIZE+1, fp) == NULL) return -1;

        if (strlen(line) == CSSIO_WFDISC28_SIZE) {
            sscanf(line, CSSIO_WFDISC28_SCS, CSSIO_WFDISC28_RVL(&wfdisc28));
            CSSIO_WFDISC28_TRM(&wfdisc28);
            wf28to30(wfdisc, &wfdisc28);
            return 0;
        } else if (strlen(line) == CSSIO_WFDISC_SIZE) {
            sscanf(line, CSSIO_WFDISC_SCS, CSSIO_WFDISC_RVL(wfdisc));
            CSSIO_WFDISC_TRM(wfdisc);
            return 0;
        }
    }
}

/* Read in a wfdisc record as a set of opaque character strings */

int cssioReadWfdiscC(FILE *fp, WFDISC_C *wd, int flags)
{
int ntoken;
char line[CSSIO_WFDISC_SIZE+2];
char *token[CSSIO_WFDISC_NFIELD];

    if (fp == NULL || wd == NULL) {
        errno = EINVAL;
        return -1;
    }


    if (fgets(line, CSSIO_WFDISC_SIZE+1, fp) == NULL) return -1;

    if (strlen(line) != CSSIO_WFDISC_SIZE) {
        errno = EINVAL;
        return -1;
    }

/* save a copy of the orginal line */

    strncpy(wd->string, line, CSSIO_WFDISC_SIZE+1);

/* if application wants to ensure known case, apply it here */

    switch (flags) {
      case CSSIO_UCASE: util_ucase(line); break;
      case CSSIO_LCASE: util_lcase(line); break;
    }

/* parse the line into individual tokens */

    if ((ntoken = utilParse(line, token, " ", CSSIO_WFDISC_NFIELD, 0)) != CSSIO_WFDISC_NFIELD) {
        errno = EINVAL;
        return -2;
    }

    /* copy each token to the appropriate field */

    strncpy(wd->sta,      token[CSSIO_FIELD_STA],      CSSIO_STA_LEN     +1);
    strncpy(wd->chan,     token[CSSIO_FIELD_CHAN],     CSSIO_CHAN_LEN    +1);
    strncpy(wd->time,     token[CSSIO_FIELD_TIME],     CSSIO_TIME_LEN    +1);
    strncpy(wd->wfid,     token[CSSIO_FIELD_WFID],     CSSIO_WFID_LEN    +1);
    strncpy(wd->chanid,   token[CSSIO_FIELD_CHANID],   CSSIO_CHANID_LEN  +1);
    strncpy(wd->jdate,    token[CSSIO_FIELD_JDATE],    CSSIO_JDATE_LEN   +1);
    strncpy(wd->endtime,  token[CSSIO_FIELD_ENDTIME],  CSSIO_ENDTIME_LEN +1);
    strncpy(wd->smprate,  token[CSSIO_FIELD_SMPRATE],  CSSIO_SMPRATE_LEN +1);
    strncpy(wd->nsamp,    token[CSSIO_FIELD_NSAMP],    CSSIO_NSAMP_LEN   +1);
    strncpy(wd->calib,    token[CSSIO_FIELD_CALIB],    CSSIO_CALIB_LEN   +1);
    strncpy(wd->calper,   token[CSSIO_FIELD_CALPER],   CSSIO_CALPER_LEN  +1);
    strncpy(wd->instype,  token[CSSIO_FIELD_INSTYPE],  CSSIO_INSTYPE_LEN +1);
    strncpy(wd->segtype,  token[CSSIO_FIELD_SEGTYPE],  CSSIO_SEGTYPE_LEN +1);
    strncpy(wd->datatype, token[CSSIO_FIELD_DATATYPE], CSSIO_DATATYPE_LEN+1);
    strncpy(wd->clip,     token[CSSIO_FIELD_CLIP],     CSSIO_CLIP_LEN    +1);
    strncpy(wd->dir,      token[CSSIO_FIELD_DIR],      CSSIO_DIR_LEN     +1);
    strncpy(wd->dfile,    token[CSSIO_FIELD_DFILE],    CSSIO_DFILE_LEN   +1);
    strncpy(wd->foff,     token[CSSIO_FIELD_FOFF],     CSSIO_FOFF_LEN    +1);
    strncpy(wd->commid,   token[CSSIO_FIELD_COMMID],   CSSIO_COMMID_LEN  +1);
    strncpy(wd->lddate,   token[CSSIO_FIELD_LDDATE],   CSSIO_LDDATE_LEN  +1);

    sprintf(wd->path,     "%s/%s", wd->dir, wd->dfile);

    return 0;
}

/* Revision History
 *
 * $Log: rwfdrec.c,v $
 * Revision 1.3  2014/04/14 18:41:42  dechavez
 * added cssioReadWfdiscC().  added arg check to rwfdrec()
 *
 * Revision 1.2  2011/03/17 17:21:09  dechavez
 * changed all macros and constants to use names with CSSIO_ or cssio_ prefixes
 * in order to avoid conflicts with IDA DCC source code
 *
 * Revision 1.1.1.1  2000/02/08 20:20:23  dec
 * import existing IDA/NRTS sources
 *
 */
