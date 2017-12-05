#pragma ident "$Id: pt.c,v 1.3 2015/12/04 23:15:12 dechavez Exp $"
/*========================================================================
 *
 * Pack token data
 *
 *======================================================================*/
#include "qdp.h"

static int PackIgnore(UINT8 *start)
{
UINT8 *ptr;

    ptr = start;
    *ptr++ = QDP_TOKEN_TYPE_IGNORE;

    return (int) (ptr - start);
}


static int PackSite(UINT8 *start, QDP_TOKEN_SITE *site)
{
UINT8 *ptr;

    ptr = start;

    *ptr++ = QDP_TOKEN_TYPE_SITE;
    ptr += utilPackBytes(ptr, (UINT8 *) site->nname, QDP_NNAME_LEN);
    ptr += utilPackBytes(ptr, (UINT8 *) site->sname, QDP_SNAME_LEN);

    return (int) (ptr - start);
}

static int PackDss(UINT8 *start, QDP_TOKEN_DSS *dss)
{
UINT8 *ptr;

    ptr = start;

    *ptr++ = QDP_TOKEN_TYPE_DSS;
    ptr += utilPackBytes(ptr, (UINT8 *) dss->passwd.hi, QDP_DSS_PASSWD_LEN);
    ptr += utilPackBytes(ptr, (UINT8 *) dss->passwd.mi, QDP_DSS_PASSWD_LEN);
    ptr += utilPackBytes(ptr, (UINT8 *) dss->passwd.lo, QDP_DSS_PASSWD_LEN);
    ptr += utilPackUINT32(ptr, dss->timeout);
    ptr += utilPackUINT32(ptr, dss->maxbps);
    *ptr++ = dss->verbosity;
    *ptr++ = dss->maxcpu;
    ptr += utilPackUINT16(ptr, dss->port);
    ptr += utilPackUINT16(ptr, dss->maxmem);
    

    return (int) (ptr - start);
}

static int PackClock(UINT8 *start, QDP_TOKEN_CLOCK *clock)
{
UINT8 *ptr;

    ptr = start;

    *ptr++ = QDP_TOKEN_TYPE_CLOCK;
    ptr += utilPackINT32(ptr, clock->offset);
    ptr += utilPackUINT16(ptr, clock->maxlim);
    *ptr++ = clock->pll.locked;
    *ptr++ = clock->pll.track;
    *ptr++ = clock->pll.hold;
    *ptr++ = clock->pll.off;
    *ptr++ = clock->pll.spare;
    *ptr++ = clock->maxhbl;
    *ptr++ = clock->minhbl;
    *ptr++ = clock->nbl;
    ptr += utilPackUINT16(ptr, clock->clkqflt);

    return (int) (ptr - start);
}

static int PackLogid(UINT8 *start, QDP_TOKEN_LOGID *logid)
{
UINT8 *ptr;

    ptr = start;

    *ptr++ = QDP_TOKEN_TYPE_LOGID;
    ptr += utilPackBytes(ptr, (UINT8 *) logid->mesg.loc, QDP_LNAME_LEN);
    ptr += utilPackBytes(ptr, (UINT8 *) logid->mesg.chn, QDP_CNAME_LEN);
    ptr += utilPackBytes(ptr, (UINT8 *) logid->time.loc, QDP_LNAME_LEN);
    ptr += utilPackBytes(ptr, (UINT8 *) logid->time.chn, QDP_CNAME_LEN);

    return (int) (ptr - start);
}

static int PackCnfid(UINT8 *start, QDP_TOKEN_CNFID *cnfid)
{
UINT8 *ptr;

    ptr = start;

    *ptr++ = QDP_TOKEN_TYPE_CNFID;
    ptr += utilPackBytes(ptr, (UINT8 *) cnfid->loc, QDP_LNAME_LEN);
    ptr += utilPackBytes(ptr, (UINT8 *) cnfid->chn, QDP_CNAME_LEN);
    *ptr++ = cnfid->flags;
    ptr += utilPackUINT16(ptr, cnfid->interval);

    return (int) (ptr - start);
}

static int PackSrvr(UINT8 *start, UINT16 port, int key)
{
UINT8 *ptr;

    ptr = start;

    *ptr++ = key;
    ptr += utilPackUINT16(ptr, port);

    return (int) (ptr - start);
}

static int PackLcqAvepar(UINT8 *start, QDP_LCQ_AVEPAR *avepar)
{
UINT8 *ptr;

    ptr = start;
    ptr += utilPackUINT32(ptr, avepar->len);
    *ptr++ = avepar->filt.code;

    return (int) (ptr - start);

    return (int) (ptr - start);
}

static int PackLcqDecim(UINT8 *start, QDP_LCQ_DECIM *decim)
{
UINT8 *ptr;

    ptr = start;

    *ptr++ = decim->src.code;
    *ptr++ = decim->fir.code;

    return (int) (ptr - start);
}

static int PackLcqDetect(UINT8 *start, QDP_LCQ_DETECT *detect)
{
UINT8 *ptr;

    ptr = start;

    *ptr++ = detect->base.code;
    *ptr++ = detect->use;
    *ptr++ = detect->options;

    return (int) (ptr - start);
}

static int PackLcq(UINT8 *start, QDP_TOKEN_LCQ *lcq)
{
UINT8 *ptr, *plen;

    ptr = start;

    *ptr++ = QDP_TOKEN_TYPE_LCQ;
    plen = ptr++; /* note location of length byte */
    ptr += utilPackBytes(ptr, (UINT8 *) lcq->loc, QDP_LNAME_LEN);
    ptr += utilPackBytes(ptr, (UINT8 *) lcq->chn, QDP_CNAME_LEN);
    *ptr++ = lcq->ident.code;
    *ptr++ = lcq->src[0];
    *ptr++ = lcq->src[1];
    ptr += utilPackUINT32(ptr, lcq->options);

    ptr += utilPackUINT16(ptr, lcq->rate);
    if (lcq->options & QDP_LCQ_HAVE_PRE_EVENT_BUFFERS) ptr += utilPackUINT16(ptr, lcq->pebuf);
    if (lcq->options & QDP_LCQ_HAVE_GAP_THRESHOLD)     ptr += utilPackREAL32(ptr, lcq->gapthresh);
    if (lcq->options & QDP_LCQ_HAVE_CALIB_DELAY)       ptr += utilPackUINT16(ptr, lcq->caldly);
    if (lcq->options & QDP_LCQ_HAVE_FRAME_COUNT)       *ptr++ = lcq->comfr;
    if (lcq->options & QDP_LCQ_HAVE_FIR_MULTIPLIER)    ptr += utilPackREAL32(ptr, lcq->firfix);
    if (lcq->options & QDP_LCQ_HAVE_AVEPAR)            ptr += PackLcqAvepar(ptr, &lcq->ave);
    if (lcq->options & QDP_LCQ_HAVE_CNTRL_DETECTOR)    *ptr++ = lcq->cntrl.code;
    if (lcq->options & QDP_LCQ_HAVE_DECIM_ENTRY)       ptr += PackLcqDecim(ptr, &lcq->decim);
    if (lcq->options & QDP_LCQ_HAVE_DETECTOR_1)        ptr += PackLcqDetect(ptr, &lcq->detect[0]);
    if (lcq->options & QDP_LCQ_HAVE_DETECTOR_2)        ptr += PackLcqDetect(ptr, &lcq->detect[1]);
    if (lcq->options & QDP_LCQ_HAVE_DETECTOR_3)        ptr += PackLcqDetect(ptr, &lcq->detect[2]);
    if (lcq->options & QDP_LCQ_HAVE_DETECTOR_4)        ptr += PackLcqDetect(ptr, &lcq->detect[3]);
    if (lcq->options & QDP_LCQ_HAVE_DETECTOR_5)        ptr += PackLcqDetect(ptr, &lcq->detect[4]);
    if (lcq->options & QDP_LCQ_HAVE_DETECTOR_6)        ptr += PackLcqDetect(ptr, &lcq->detect[5]);
    if (lcq->options & QDP_LCQ_HAVE_DETECTOR_7)        ptr += PackLcqDetect(ptr, &lcq->detect[6]);
    if (lcq->options & QDP_LCQ_HAVE_DETECTOR_8)        ptr += PackLcqDetect(ptr, &lcq->detect[7]);

    *plen = (ptr - plen);

    return (int) (ptr - start);
}

static int PackIIRDatum(UINT8 *start, QDP_IIR_DATA *datum)
{
UINT8 *ptr;

    ptr = start;

    ptr += utilPackREAL32(ptr, datum->ratio);
    *ptr++ = (datum->type == QDP_IIR_HIPASS) ? (datum->npole | 0x80) : datum->npole;

    return (int) (ptr - start);
}

static int PackIir(UINT8 *start, QDP_TOKEN_IIR *iir)
{
int i;
UINT8 *ptr, *plen;


    ptr = start;

    *ptr++ = QDP_TOKEN_TYPE_IIR;
    plen = ptr++; /* note location of length byte */
    *ptr++ = iir->id;
    ptr += utilPackPascalString(ptr, iir->name, strlen(iir->name));
    *ptr++ = iir->nsection;
    ptr += utilPackREAL32(ptr, iir->gain);
    ptr += utilPackREAL32(ptr, iir->refreq);
    for (i = 0; i < iir->nsection; i++) ptr += PackIIRDatum(ptr, &iir->data[i]);

    *plen = (ptr - plen);

    return (int) (ptr - start);
}

static int PackFir(UINT8 *start, QDP_TOKEN_FIR *fir)
{
UINT8 *ptr, *plen;

    ptr = start;

    *ptr++ = QDP_TOKEN_TYPE_FIR;
    plen = ptr++; /* note location of length byte */
    *ptr++ = fir->id;
    ptr += utilPackPascalString(ptr, fir->name, strlen(fir->name));

    *plen = (ptr - plen);

    return (int) (ptr - start);
}

static int PackCds(UINT8 *start, QDP_TOKEN_CDS *cds)
{
int i;
UINT8 *ptr, *plen;

    ptr = start;

    *ptr++ = QDP_TOKEN_TYPE_CDS;
    plen = ptr++; /* note location of length byte */
    *ptr++ = cds->id;
    *ptr++ = cds->options;
    ptr += utilPackPascalString(ptr, cds->name, strlen(cds->name));
    for (i = 0; i < cds->nentry; i++) *ptr++ = cds->equation[i].code;
    *ptr++ = QDP_CDS_EQ_DONE;

    *plen = (ptr - plen);

    return (int) (ptr - start);
}

static int PackDet(UINT8 *start, QDP_TOKEN_DET *det)
{
static UINT16 spare = 0;
UINT8 *ptr, *plen;

    ptr = start;

    *ptr++ = det->type;
    plen = ptr++; /* note location of length byte */
    if (det->type == QDP_TOKEN_TYPE_MHD) {
        *ptr++ = det->id;
        *ptr++ = det->detf.code;
        *ptr++ = det->iw;
        *ptr++ = det->nht;
        ptr += utilPackUINT32(ptr, det->fhi);
        ptr += utilPackUINT32(ptr, det->flo);
        ptr += utilPackUINT16(ptr, det->wa);
        ptr += utilPackUINT16(ptr, spare);
        ptr += utilPackUINT16(ptr, det->tc);
        *ptr++ = det->x1;
        *ptr++ = det->x2;
        *ptr++ = det->x3;
        *ptr++ = det->xx;
        *ptr++ = det->av;
        ptr += utilPackPascalString(ptr, det->name, strlen(det->name));
    } else {
        *ptr++ = det->id;
        *ptr++ = det->detf.code;
        *ptr++ = det->iw;
        *ptr++ = det->nht;
        ptr += utilPackUINT32(ptr, det->fhi);
        ptr += utilPackUINT32(ptr, det->flo);
        ptr += utilPackUINT16(ptr, det->wa);
        ptr += utilPackUINT16(ptr, spare);
        ptr += utilPackPascalString(ptr, det->name, strlen(det->name));
    }

    *plen = (ptr - plen);

    return (int) (ptr - start);
}

static int PackCenList(UINT8 *start, LNKLST *list)
{
int i;
UINT8 *ptr, *plen;
QDP_TOKEN_CEN *cen;
char DefaultName[] = "COMM:xx + slop";

    ptr = start;

    *ptr++ = QDP_TOKEN_TYPE_CEN;
    plen = ptr; ptr += 2; /* note location of length byte */
    for (i = 0; i < list->count; i++) {
        cen = (QDP_TOKEN_CEN *) list->array[i];
        sprintf(DefaultName, "COMM:%d", i+1);
        if (strcmp(DefaultName, cen->name) == 0) continue; //* ignore default names */
        *ptr++ = cen->id;
        ptr += utilPackPascalString(ptr, cen->name, strlen(cen->name));
    }
    utilPackUINT16(plen, (UINT16) (ptr - plen));

    return (int) (ptr - start);
}

static int PackNoncomp(UINT8 *start)
{
UINT8 *ptr;

    ptr = start;

    *ptr++ = QDP_TOKEN_TYPE_NONCOMP;
    *ptr++ = 0; /* zero length */

    return (int) (ptr - start);
}

int qdpPackTokenMemblk(QDP_MEMBLK *dest, QDP_DP_TOKEN *token, UINT16 type)
{
int i;
UINT8 *ptr;

    if (dest == NULL || token == NULL) {
        errno = EINVAL;
        return -1;
    }

    dest->nbyte = 0;
    dest->type = type;
    ptr = dest->data;

    *ptr++ = QDP_TOKEN_TYPE_VER;
    *ptr++ = QDP_TOKEN_VERSION_ZERO;

    if (token->site.valid)      ptr += PackSite(ptr, &token->site);
    if (token->srvr.net.valid)  ptr += PackSrvr(ptr, token->srvr.net.port, QDP_TOKEN_TYPE_NETSERVER);
    if (token->dss.valid)       ptr += PackDss(ptr, &token->dss);
    ptr += PackIgnore(ptr); /* for Willard compatibility */
    ptr += PackIgnore(ptr); /* for Willard compatibility */
    ptr += PackIgnore(ptr); /* for Willard compatibility */
    ptr += PackIgnore(ptr); /* for Willard compatibility */
    ptr += PackIgnore(ptr); /* for Willard compatibility */
    if (token->srvr.web.valid)  ptr += PackSrvr(ptr, token->srvr.web.port, QDP_TOKEN_TYPE_WEBSERVER);
    if (token->srvr.data.valid) ptr += PackSrvr(ptr, token->srvr.data.port, QDP_TOKEN_TYPE_DATSERVER);
    if (token->clock.valid)     ptr += PackClock(ptr, &token->clock);
    if (token->logid.valid)     ptr += PackLogid(ptr, &token->logid);
    if (token->cnfid.valid)     ptr += PackCnfid(ptr, &token->cnfid);
    ptr += PackCenList(ptr, &token->cen);
    for (i = 0; i < token->iir.count; i++) ptr += PackIir(ptr, (QDP_TOKEN_IIR *) token->iir.array[i]);
    for (i = 0; i < token->fir.count; i++) ptr += PackFir(ptr, (QDP_TOKEN_FIR *) token->fir.array[i]);
    for (i = 0; i < token->det.count; i++) ptr += PackDet(ptr, (QDP_TOKEN_DET *) token->det.array[i]);
    for (i = 0; i < token->lcq.count; i++) ptr += PackLcq(ptr, (QDP_TOKEN_LCQ *) token->lcq.array[i]);
    for (i = 0; i < token->cds.count; i++) ptr += PackCds(ptr, (QDP_TOKEN_CDS *) token->cds.array[i]);
    if (token->noncomp) ptr += PackNoncomp(ptr);

    dest->nbyte = (INT16) (ptr - dest->data);
    if (dest->nbyte != (dest->nbyte / 2) * 2) ptr += PackIgnore(ptr);
    dest->nbyte = (INT16) (ptr - dest->data);

    return dest->nbyte;

}

/*-----------------------------------------------------------------------+
 |                                                                       |
 | Copyright (C) 2009 Regents of the University of California            |
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
 * $Log: pt.c,v $
 * Revision 1.3  2015/12/04 23:15:12  dechavez
 * casts, format fixes and the like, all to calm OS X compiles
 *
 * Revision 1.2  2009/11/05 18:32:42  dechavez
 * implemented qdpPackTokenMemblk()
 *
 * Revision 1.1  2009/10/29 17:43:36  dechavez
 * initial release
 *
 */
