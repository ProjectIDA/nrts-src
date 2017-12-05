#pragma ident "$Id: server.c,v 1.17 2015/12/04 22:31:17 dechavez Exp $"
/*======================================================================
 *
 * IDA data exchange protocols, server side routines.
 *
 * - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 * Copyright (c) 1997 Regents of the University of California.
 * All rights reserved.
 *====================================================================*/
#include "platform.h"
#include "xfer.h"
#include "util.h"

static int slen = sizeof(UINT16);
static int llen = sizeof(UINT32);
UINT16 sval;
UINT32 lval;

static size_t len;

static LNKLST deny;

/*============================ internal use ==========================*/

/* Compress waveform data */

static int xfer_Compress(cnf, wav)
struct xfer_cnf *cnf; /* disk loop configuration             */
struct xfer_wav *wav; /* waveform record, complete with data */
{
UINT32 order;
int cbyte, nsamp, wrdsiz, standx, chnndx, datlen;
INT32 *nbyte;
int *comp;
UINT8 *data, *output;
static UINT8 cdata[XFER_MAXDAT*4];
static union {
   INT16 s[XFER_MAXDAT];
   INT32 l[XFER_MAXDAT];
} buffer;
IDA ida;
static char *fid = "xfer_Compress";

/* Get all the required info from the config and waveform records */

    switch (wav->format) {

      case XFER_WAVGEN1:

        if (*(comp = &wav->type.gen1.comp) == XFER_CMPNONE) return XFER_OK;

        standx =  wav->type.gen1.standx;
        chnndx =  wav->type.gen1.chnndx;
        nsamp  =  wav->type.gen1.nsamp;
        nbyte  = &wav->type.gen1.nbyte;
        data   =  wav->type.gen1.data;
        break;

      case XFER_WAVRAW:
      case XFER_WAVSEED:
      case XFER_WAVPASSCAL:

        return XFER_OK;

#ifdef IDA_SUPPORT
      case XFER_WAVIDA:

        if (cnf->format != XFER_CNFNRTS) {
            util_log(1, "%s: cnf format must be NRTS for WAVIDA!", fid);
            xfer_errno = XFER_EINVAL;
            return XFER_ERROR;
        }
        datlen = cnf->type.nrts.sta[0].chn[0].dlen;


        if (*(comp = &wav->type.ida.comp) == XFER_CMPNONE) return XFER_OK;

        data  =  wav->type.ida.data;
        nbyte = &wav->type.ida.nbyte;
        data  =  wav->type.ida.data;

        idaInitHandle(&ida); ida.rev.value = wav->type.ida.rev;
        cbyte = ida_compress(&ida, comp, data, cdata, datlen);
        idaClearHandle(&ida);

        if (cbyte < 0) {
            xfer_errno = XFER_EINVAL;
            return XFER_ERROR;
        } else if (cbyte > XFER_MAXDAT) {
            util_log(1, "%s: increase XFER_MAXDAT (%d) to %d",
                fid, XFER_MAXDAT, cbyte
            );
            xfer_errno = XFER_ELIMIT;
            return XFER_ERROR;
        } else {
            *nbyte = cbyte;
            memcpy(data, cdata, (*nbyte = cbyte));
            return XFER_OK;
        }
        break;
#endif

      default:

        xfer_errno = XFER_EFORMAT;
        return XFER_ERROR;
    }

    switch (cnf->format) {

      case XFER_CNFGEN1:

        wrdsiz = cnf->type.gen1.sta[standx].chn[chnndx].wrdsiz;
        order  = cnf->type.gen1.sta[standx].chn[chnndx].order;
        break;

      case XFER_CNFNRTS:

        wrdsiz = cnf->type.nrts.sta[standx].chn[chnndx].wrdsiz;
        order  = cnf->type.nrts.sta[standx].chn[chnndx].order;
        break;

      default:

        xfer_errno = XFER_EFORMAT;
        return XFER_ERROR;
    }

/* Sanity checks */

    if (wrdsiz != *nbyte / nsamp) {
        util_log(1, "%s: configuration and waveform discrepancy", fid);
        util_log(1, "%s: %d != %d / %d", fid, wrdsiz, *nbyte, nsamp);
        xfer_errno = XFER_EINVAL;
        return XFER_ERROR;
    }

    if (wrdsiz == 8) {
        *comp = XFER_CMPNONE;
    } else if (wrdsiz != 2 && wrdsiz != 4) {
        util_log(1, "%s: unexpected wrdsiz `%d'", fid, wrdsiz);
        xfer_errno = XFER_EINVAL;
        return XFER_ERROR;
    }

/* Do the compression */

    switch (*comp) {

      case XFER_CMPIGPP:

        if (wrdsiz == 4) {
            memcpy((char *) buffer.l, data, *nbyte);
            if (order != util_order()) util_lswap(buffer.l, nsamp);
            cbyte = util_lcomp(cdata, buffer.l, nsamp);
        } else {
            memcpy((char *) buffer.s, data, *nbyte);
            if (order != util_order()) util_sswap(buffer.s, nsamp);
            cbyte = util_scomp(cdata, buffer.s, nsamp);
        }

        /* Check compression and only keep it if the size reduced */

        if (cbyte > 0 && cbyte < *nbyte) {
            output = cdata;
        } else {
            *comp = XFER_CMPNONE;
        }
        break;

      case XFER_CMPNONE:
        break;

      default:
        *comp = XFER_CMPNONE;
    }

    if (*comp == XFER_CMPNONE) return XFER_OK;

/* Copy the compressed data to the waveform record's data buffer */

    memcpy(data, output, (*nbyte = cbyte));

    return XFER_OK;
}

/* Decode a request preamble */

static int xfer_DecodePreamble(preamble, src, protocol)
union xfer_preamble *preamble;
char **src;
int protocol;
{
static char *fid = "xfer_DecodePreamble";

    switch (protocol) {

      case 0x01:

        memcpy(&lval, *src, llen);
        *src += llen;
        preamble->ver01.client_id = (INT32) ntohl(lval);

        memcpy(&sval, *src, slen);
        *src += slen;
        preamble->ver01.format = (int) ntohs(sval);

        break;

      default:
        xfer_errno = XFER_EPROTOCOL;
        return XFER_ERROR;
    }

    return XFER_OK;

}

/* Decode a waveform request */

static int xfer_DecodeWavReq(union xfer_wavreq *req, char **src, int protocol)
{
int i, j;
struct xfer_time xtime;
static char *fid = "xfer_DecodeWavReq";

    switch (protocol) {

      case 0x01:

        memcpy(&sval, *src, slen);
        *src += slen;
        req->ver01.format = (int) ntohs(sval);

    /* Make sure this is a supported return format 
     * (See comments in xfer_EncodeWav, below)
     */

        switch (req->ver01.format) {
          case XFER_WAVGEN1:
          case XFER_WAVIDA:
          case XFER_WAVRAW:
          case XFER_WAVSEED:
            break;
          default:
            xfer_errno = XFER_EFORMAT;
            return XFER_ERROR;
        }

    /* Supported format, continue decoding */

        memcpy(&sval, *src, slen);
        *src += slen;
        req->ver01.keepup = (int) ntohs(sval);

        memcpy(&sval, *src, slen);
        *src += slen;
        req->ver01.comp = (int) ntohs(sval);

        memcpy(&sval, *src, slen);
        *src += slen;
        req->ver01.nsta = (int) ntohs(sval);
        if (req->ver01.nsta > XFER_MAXSTA) {
            util_log(1, "illegal request: nsta=%d > XFER_MAXSTA=%d",
                req->ver01.nsta, XFER_MAXSTA
            );
            xfer_errno = XFER_EREJECT;
            return XFER_ERROR;
        }

        for (i = 0; i < req->ver01.nsta; i++) {
            memcpy(req->ver01.sta[i].name, *src, XFER01_SNAMLEN + 1);
            *src += XFER01_SNAMLEN + 1;

            memcpy(&sval, *src, slen);
            *src += slen;
            req->ver01.sta[i].nchn = (int) ntohs(sval);
            if (req->ver01.sta[i].nchn > XFER_MAXCHN) {
                util_log(1, "illegal request: nchn=%d > XFER_MAXCHN=%d",
                    req->ver01.sta[i].nchn, XFER_MAXCHN
                );
                xfer_errno = XFER_EREJECT;
                return XFER_ERROR;
            }

            for (j = 0; j < req->ver01.sta[i].nchn; j++) {
                memcpy(req->ver01.sta[i].chn[j].name,*src,XFER01_CNAMLEN+1);
                *src += XFER01_CNAMLEN + 1;

                memcpy(&lval, *src, llen);
                *src += llen;
                xtime.sec = (INT32) ntohl(lval);
                memcpy(&lval, *src, llen);
                *src += llen;
                xtime.usec = (UINT32) ntohl(lval);
                req->ver01.sta[i].chn[j].beg = xfer_dtime(&xtime);

                memcpy(&lval, *src, llen);
                *src += llen;
                xtime.sec = (INT32) ntohl(lval);
                memcpy(&lval, *src, llen);
                *src += llen;
                xtime.usec = (UINT32) ntohl(lval);
                req->ver01.sta[i].chn[j].end = xfer_dtime(&xtime);
            }
        }

        break;

      default:
        xfer_errno = XFER_EPROTOCOL;
        return XFER_ERROR;
    }

    return XFER_OK;
}

/* Decode an arbitrary request */

static int xfer_DecodeReq(req, src)
struct xfer_req *req;
char *src;
{
int retval;
static char *fid = "xfer_DecodeReq";

/* ALL requests specify protocol as first two bytes */

    memcpy(&sval, src, slen);
    src += slen;
    req->protocol = (int) ntohs(sval);

/* Followed by 4 bytes of request code */

    memcpy(&lval, src, llen);
    src += llen;
    req->type = (INT32) ntohl(lval);

/* Followed by 2 bytes of socket I/O timeout */

    memcpy(&sval, src, slen);
    src += slen;
    req->timeout = (int) ntohs(sval);
    if (req->timeout < XFER_DEFTO) req->timeout = XFER_DEFTO;

/* AND followed by 4 bytes of client send (server recv) buffer size */

    memcpy(&lval, src, llen);
    src += llen;
    req->rcvbuf = (INT32) ntohl(lval);

/* AND followed by 4 bytes of client recv (server send) buffer size */

    memcpy(&lval, src, llen);
    src += llen;
    req->sndbuf = (INT32) ntohl(lval);

/* Set the global socket I/O timeout */

    _xfer_timeout = req->timeout;

/* What follows depends on the protocol */

    switch (req->protocol) {

      case 0x01:

        retval = xfer_DecodePreamble(&req->preamble, &src, req->protocol);
        if (retval != XFER_OK) return retval;

        switch (req->type) {

          case XFER_CNFREQ:
            return XFER_OK;

          case XFER_WAVREQ:
            return xfer_DecodeWavReq(&req->request.wav,&src,req->protocol);

          default:
            xfer_errno = XFER_EREQUEST;
            return XFER_ERROR;
        }
    }
    return XFER_ERROR;
}

/* Encode a channel configuration descriptor */

static INT32 xfer_EncodeChnCnf(start_of_message, info, format)
char *start_of_message;
void *info;
int format;
{
int sign;
INT32 factor;
float test;
char *ptr;
struct xfer_time *xtime;
struct xfer_gen1chncnf *gen1;
struct xfer_nrtschncnf *nrts;
static char *fid = "xfer_EncodeChnCnf";

    ptr = start_of_message;

    switch (format) {

      case XFER_CNFGEN1:

        gen1 = (struct xfer_gen1chncnf *) info;

        memcpy(ptr, gen1->name, XFER01_CNAMLEN + 1);
        ptr += XFER01_CNAMLEN + 1;

        memcpy(ptr, gen1->instype, XFER01_INAMLEN + 1);
        ptr += XFER01_INAMLEN + 1;

        sval = htons((UINT16) gen1->wrdsiz);
        memcpy(ptr, &sval, slen);
        ptr += slen;

        lval = htonl((UINT32) gen1->order);
        memcpy(ptr, &lval, llen);
        ptr += llen;

        lval = htonl((UINT32) (1000000.0 * gen1->sint));
        memcpy(ptr, &lval, llen);
        ptr += llen;

        sign = (gen1->calib < 0) ? -1 : 1;
        test = (float) sign * gen1->calib;
        if (test < 1000.0) {
            factor = sign * 1000000;
        } else if (test < 10000.0) {
            factor = sign * 100000;
        } else if (test < 100000.0) {
            factor = sign * 10000;
        } else if (test < 1000000.0) {
            factor = sign * 1000;
        } else if (test < 10000000.0) {
            factor = sign * 100;
        } else if (test < 100000000.0) {
            factor = sign * 10;
        } else {
            factor = sign;
        }
        lval = htonl((UINT32) factor);
        memcpy(ptr, &lval, llen);
        ptr += llen;

        lval = htonl((UINT32) ((float) factor * gen1->calib));
        memcpy(ptr, &lval, llen);
        ptr += llen;

        lval = htonl((UINT32) (100000.0 * gen1->calper));
        memcpy(ptr, &lval, llen);
        ptr += llen;

        lval = htonl((UINT32) (1000000.0 * gen1->vang));
        memcpy(ptr, &lval, llen);
        ptr += llen;

        lval = htonl((UINT32) (1000000.0 * gen1->hang));
        memcpy(ptr, &lval, llen);
        ptr += llen;

        xtime = (struct xfer_time *) xfer_time(gen1->beg);
        lval = htonl((UINT32) xtime->sec);
        memcpy(ptr, &lval, llen);
        ptr += llen;
        lval = htonl(xtime->usec);
        memcpy(ptr, &lval, llen);
        ptr += llen;

        xtime = (struct xfer_time *) xfer_time(gen1->end);
        lval = htonl((UINT32) xtime->sec);
        memcpy(ptr, &lval, llen);
        ptr += llen;
        lval = htonl(xtime->usec);
        memcpy(ptr, &lval, llen);
        ptr += llen;

        break;

#ifdef NRTS_SUPPORT
      case XFER_CNFNRTS:

        nrts = (struct xfer_nrtschncnf *) info;

        memcpy(ptr, nrts->name, XFER01_CNAMLEN + 1);
        ptr += XFER01_CNAMLEN + 1;
        util_log(3, "%s: name = %s", fid, nrts->name);

        memcpy(ptr, nrts->instype, XFER01_INAMLEN + 1);
        ptr += XFER01_INAMLEN + 1;
        util_log(3, "%s: instype = %s", fid, nrts->instype);

        sval = htons((UINT16) nrts->wrdsiz);
        memcpy(ptr, &sval, slen);
        ptr += slen;
        util_log(3, "%s: wrdsiz = %d", fid, nrts->wrdsiz);

        lval = htonl((UINT32) nrts->order);
        memcpy(ptr, &lval, llen);
        ptr += llen;
        util_log(3, "%s: order = 0x%08x", fid, nrts->order);

        lval = htonl((UINT32) (1000000.0 * nrts->sint));
        memcpy(ptr, &lval, llen);
        ptr += llen;
        util_log(3, "%s: sint = %.3f", fid, nrts->sint);

        sign = (nrts->calib < 0) ? -1 : 1;
        test = (float) sign * nrts->calib;
        if (test < 1000.0) {
            factor = sign * 1000000;
        } else if (test < 10000.0) {
            factor = sign * 100000;
        } else if (test < 100000.0) {
            factor = sign * 10000;
        } else if (test < 1000000.0) {
            factor = sign * 1000;
        } else if (test < 10000000.0) {
            factor = sign * 100;
        } else if (test < 100000000.0) {
            factor = sign * 10;
        } else {
            factor = sign;
        }
        lval = htonl((UINT32) factor);
        memcpy(ptr, &lval, llen);
        ptr += llen;
        util_log(3, "%s: factor = %ld", fid, factor);

        lval = htonl((UINT32) ((float) factor * nrts->calib));
        memcpy(ptr, &lval, llen);
        ptr += llen;
        util_log(3, "%s: calib = %.3f", fid, nrts->calib);

        lval = htonl((UINT32) (100000.0 * nrts->calper));
        memcpy(ptr, &lval, llen);
        ptr += llen;
        util_log(3, "%s: calper = %.3f", fid, nrts->calper);

        lval = htonl((UINT32) (1000000.0 * nrts->vang));
        memcpy(ptr, &lval, llen);
        ptr += llen;
        util_log(3, "%s: vang = %.3f", fid, nrts->vang);

        lval = htonl((UINT32) (1000000.0 * nrts->hang));
        memcpy(ptr, &lval, llen);
        ptr += llen;
        util_log(3, "%s: hang = %.3f", fid, nrts->hang);

        xtime = (struct xfer_time *) xfer_time(nrts->beg);
        lval = htonl((UINT32) xtime->sec);
        memcpy(ptr, &lval, llen);
        ptr += llen;
        lval = htonl(xtime->usec);
        memcpy(ptr, &lval, llen);
        ptr += llen;
        util_log(3, "%s: beg = %s", fid, util_dttostr(nrts->beg,0));

        xtime = (struct xfer_time *) xfer_time(nrts->end);
        lval = htonl((UINT32) xtime->sec);
        memcpy(ptr, &lval, llen);
        ptr += llen;
        lval = htonl(xtime->usec);
        memcpy(ptr, &lval, llen);
        ptr += llen;
        util_log(3, "%s: end = %s", fid, util_dttostr(nrts->end,0));

        sval = htons((UINT16) nrts->type);
        memcpy(ptr, &sval, slen);
        ptr += slen;
        util_log(3, "%s: type = %hd", fid, nrts->type);

        sval = htons((UINT16) nrts->hlen);
        memcpy(ptr, &sval, slen);
        ptr += slen;
        util_log(3, "%s: hlen = %hd", fid, nrts->hlen);

        sval = htons((UINT16) nrts->dlen);
        memcpy(ptr, &sval, slen);
        ptr += slen;
        util_log(3, "%s: dlen = %hd", fid, nrts->dlen);

        lval = htonl((UINT32) nrts->nrec);
        memcpy(ptr, &lval, llen);
        ptr += llen;
        util_log(3, "%s: nrec = %ld", fid, nrts->nrec);

        lval = htonl((UINT32) nrts->nhide);
        memcpy(ptr, &lval, llen);
        ptr += llen;
        util_log(3, "%s: nhide = %ld", fid, nrts->nhide);

        lval = htonl((UINT32) nrts->latency);
        memcpy(ptr, &lval, llen);
        ptr += llen;
        util_log(3, "%s: latency = %ld", fid, nrts->latency);

        break;
#endif /* NRTS_SUPPORT */

      default:
        xfer_errno = XFER_EFORMAT;
        return XFER_ERROR;
    }

    return (INT32) (ptr - start_of_message);
}

/* Encode a station configuration descriptor */

INT32 xfer_EncodeStaCnf(start_of_message, info, format)
char *start_of_message;
void *info;
int format;
{
int i;
char *ptr;
struct xfer_gen1stacnf *gen1;
struct xfer_nrtsstacnf *nrts;
static char *fid = "xfer_EncodeStaCnf";

    ptr = start_of_message;

    switch (format) {

      case XFER_CNFGEN1:

        gen1 = (struct xfer_gen1stacnf *) info;

        memcpy(ptr, gen1->name, XFER01_SNAMLEN + 1);
        ptr += XFER01_SNAMLEN + 1;

        lval = htonl((UINT32) (1000000.0 * gen1->lat));
        memcpy(ptr, &lval, llen);
        ptr += llen;

        lval = htonl((UINT32) (1000000.0 * gen1->lon));
        memcpy(ptr, &lval, llen);
        ptr += llen;

        lval = htonl((UINT32) (1000000.0 * gen1->elev));
        memcpy(ptr, &lval, llen);
        ptr += llen;

        lval = htonl((UINT32) (1000000.0 * gen1->depth));
        memcpy(ptr, &lval, llen);
        ptr += llen;

        sval = htons((UINT16) gen1->nchn);
        memcpy(ptr, &sval, slen);
        ptr += slen;

        for (i = 0; i < gen1->nchn; i++) {
            len = xfer_EncodeChnCnf(ptr, (void*)(gen1->chn+i), format);
            if (len <= 0) {
                util_log(1, "%s: xfer_EncodeChnCnf: %d", fid, len);
                return XFER_ERROR;
            }
            ptr += len;
        }

        break;

#ifdef NRTS_SUPPORT
      case XFER_CNFNRTS:

        nrts = (struct xfer_nrtsstacnf *) info;

        memcpy(ptr, nrts->name, XFER01_SNAMLEN + 1);
        ptr += XFER01_SNAMLEN + 1;
        util_log(3, "%s: name = %s", fid, nrts->name);

        lval = htonl((UINT32) (1000000.0 * nrts->lat));
        memcpy(ptr, &lval, llen);
        ptr += llen;
        util_log(3, "%s: lat = %.3f", fid, nrts->lat);

        lval = htonl((UINT32) (1000000.0 * nrts->lon));
        memcpy(ptr, &lval, llen);
        ptr += llen;
        util_log(3, "%s: lon = %.3f", fid, nrts->lon);

        lval = htonl((UINT32) (1000000.0 * nrts->elev));
        memcpy(ptr, &lval, llen);
        ptr += llen;
        util_log(3, "%s: elev = %.3f = %ld", fid, nrts->elev);

        lval = htonl((UINT32) (1000000.0 * nrts->depth));
        memcpy(ptr, &lval, llen);
        ptr += llen;
        util_log(3, "%s: depth = %.3f", fid, nrts->depth);

        sval = htons((UINT16) nrts->nchn);
        memcpy(ptr, &sval, slen);
        ptr += slen;

        for (i = 0; i < nrts->nchn; i++) {
            len = xfer_EncodeChnCnf(ptr, (void*)(nrts->chn+i), format);
            if (len <= 0) {
                util_log(1, "%s: xfer_EncodeChnCnf: %d", fid, len);
                return XFER_ERROR;
            }
            ptr += len;
        }

        break;
#endif /* NRTS_SUPPORT */

      default:
        xfer_errno = XFER_EFORMAT;
        util_log(1, "%s: unsupported format `%d'", fid, format);
        return -1;
    }

    return (INT32) (ptr - start_of_message);
}

/* Encode a configuration record */

INT32 xfer_EncodeCnf(start_of_message, cnf)
char *start_of_message;
struct xfer_cnf *cnf;
{
int i;
INT32 len;
char *ptr;
static char *fid = "xfer_EncodeCnf";

    ptr = start_of_message;

    sval = htons((UINT16) cnf->format);
    memcpy(ptr, &sval, slen);
    ptr += slen;

    switch (cnf->format) {

      case XFER_CNFGEN1:

        lval = htonl((UINT32) cnf->type.gen1.order);
        memcpy(ptr, &lval, llen);
        ptr += llen;

        sval = htons((UINT16) cnf->type.gen1.nsta);
        memcpy(ptr, &sval, slen);
        ptr += slen;

        for (i = 0; i < cnf->type.gen1.nsta; i++) {
            len = xfer_EncodeStaCnf(
                    ptr, (void *)(cnf->type.gen1.sta + i), cnf->format
            );
            if (len < 0) return XFER_ERROR;
            ptr += len;
        }

        break;

#ifdef NRTS_SUPPORT
      case XFER_CNFNRTS:

        lval = htonl((UINT32) cnf->type.nrts.order);
        memcpy(ptr, &lval, llen);
        ptr += llen;

        sval = htons((UINT16) cnf->type.nrts.nsta);
        memcpy(ptr, &sval, slen);
        ptr += slen;

        for (i = 0; i < cnf->type.nrts.nsta; i++) {
            len = xfer_EncodeStaCnf(
                    ptr, (void *)(cnf->type.nrts.sta + i), cnf->format
            );
            if (len < 0) return XFER_ERROR;
            ptr += len;
        }

        break;
#endif /* NRTS_SUPPORT */

      default:
        xfer_errno = XFER_EFORMAT;
        return XFER_ERROR;
    }

    return (INT32) (ptr - start_of_message);
}

/* Encode a waveform record */

INT32 xfer_EncodeWav(start_of_message, wav)
char *start_of_message;
struct xfer_wav *wav;
{
char *ptr;
struct xfer_time *xtime;
static char *fid = "xfer_EncodeWav";

    ptr = start_of_message;

    sval = htons((UINT16) wav->format);
    memcpy(ptr, &sval, slen);
    ptr += slen;

/* Since we won't come here until after the initial request has been
 * received and acknowledged, we need to check for supported formats
 * outside of this function.  This is done above, in xfer_DecodeWavReq.
 * There is no simple way (that I am aware of) to code things so that
 * the check falls out automatically.  Therefore, when support for new
 * formats are added here, one should be very careful to update the
 * check statement in xfer_DecodeWavReq.
 */

    switch (wav->format) {

      case XFER_WAVGEN1:

        sval = htons((UINT16) wav->type.gen1.standx);
        memcpy(ptr, &sval, slen);
        ptr += slen;

        sval = htons((UINT16) wav->type.gen1.chnndx);
        memcpy(ptr, &sval, slen);
        ptr += slen;

        xtime = (struct xfer_time *) xfer_time(wav->type.gen1.tofs);
        lval = htonl((UINT32) xtime->sec);
        memcpy(ptr, &lval, llen);
        ptr += llen;
        lval = htonl(xtime->usec);
        memcpy(ptr, &lval, llen);
        ptr += llen;

        sval = htons((UINT16) wav->type.gen1.tear);
        memcpy(ptr, &sval, slen);
        ptr += slen;

        sval = htons((UINT16) wav->type.gen1.comp);
        memcpy(ptr, &sval, slen);
        ptr += slen;

        lval = htonl((UINT32) wav->type.gen1.nsamp);
        memcpy(ptr, &lval, llen);
        ptr += llen;

        lval = htonl((UINT32) wav->type.gen1.nbyte);
        memcpy(ptr, &lval, llen);
        ptr += llen;

        if (wav->type.gen1.nbyte > XFER_MAXDAT) {
            util_log(1, "%s: increase XFER_MAXDAT (%d) to %d",
                fid, XFER_MAXDAT, wav->type.gen1.nbyte
            );
            xfer_errno = XFER_ELIMIT;
            return XFER_ERROR;
        }
        memcpy(ptr, wav->type.gen1.data, wav->type.gen1.nbyte);
        ptr += wav->type.gen1.nbyte;

        break;

      case XFER_WAVRAW:

        lval = htonl((UINT32) wav->type.raw.nbyte);
        memcpy(ptr, &lval, llen);
        ptr += llen;

        if (wav->type.raw.nbyte > XFER_MAXDAT) {
            util_log(1, "%s: increase XFER_MAXDAT (%d) to %d",
                fid, XFER_MAXDAT, wav->type.raw.nbyte
            );
            xfer_errno = XFER_ELIMIT;
            return XFER_ERROR;
        }
        memcpy(ptr, wav->type.raw.data, wav->type.raw.nbyte);
        ptr += wav->type.raw.nbyte;

        break;

#ifdef IDA_SUPPORT
      case XFER_WAVIDA:

        sval = htons((UINT16) wav->type.ida.rev);
        memcpy(ptr, &sval, slen);
        ptr += slen;

        sval = htons((UINT16) wav->type.ida.comp);
        memcpy(ptr, &sval, slen);
        ptr += slen;

        lval = htonl((UINT32) wav->type.ida.nbyte);
        memcpy(ptr, &lval, llen);
        ptr += llen;

        if (wav->type.ida.nbyte > XFER_MAXDAT) {
            util_log(1, "%s: increase XFER_MAXDAT (%d) to %d",
                fid, XFER_MAXDAT, wav->type.ida.nbyte
            );
            xfer_errno = XFER_ELIMIT;
            return XFER_ERROR;
        }
        memcpy(ptr, wav->type.ida.data, wav->type.ida.nbyte);
        ptr += wav->type.ida.nbyte;

        break;
#endif /* IDA_SUPPORT */

#ifdef SEED_SUPPORT
      case XFER_WAVSEED:
        lval = htonl((UINT32) wav->type.seed.nbyte);
        memcpy(ptr, &lval, llen);
        ptr += llen;

        memcpy(ptr, wav->type.seed.data, wav->type.seed.nbyte);
        ptr += wav->type.seed.nbyte;

        break;
#endif /* SEED_SUPPORT */

#ifdef PASSCAL_SUPPORT
      case XFER_WAVPASSCAL:
        lval = htonl((UINT32) wav->type.pas.nbyte);
        memcpy(ptr, &lval, llen);
        ptr += llen;

        break;
#endif /* PASSCAL_SUPPORT */


      default:
        xfer_errno = XFER_EFORMAT;
        return XFER_ERROR;
    }

    return (INT32) (ptr - start_of_message);
}

/*============================ external use ==========================*/

/* Send an ack */

int Xfer_Ack(sd, code)
int sd;
int code;
{
char *ptr;
static char *fid = "Xfer_Ack";

/* Send a break to alert the client that something unexpected is coming */

    if (xfer_SendMsg(sd, Xfer_Buffer, 0) != XFER_OK) {
        if (xfer_errno != XFER_ETIMEDOUT && xfer_errno != XFER_EPIPE) {
            util_log(2, "%s: xfer_SendMsg: %s", fid, strerror(errno));
        } else {
            util_log(2, "%s", Xfer_ErrStr());
        }
        Xfer_Exit(sd, xfer_errno);
    }

/* Send over the cause code */

    ptr = Xfer_Buffer + 4;
    lval = htonl((UINT32) code);
    memcpy(ptr, &lval, llen);
    if (xfer_SendMsg(sd, Xfer_Buffer, llen) != XFER_OK) {
        if (xfer_errno != XFER_ETIMEDOUT && xfer_errno != XFER_EPIPE) {
            util_log(2, "%s: xfer_SendMsg: %s", fid, Xfer_ErrStr());
        } else {
            util_log(2, "%s", Xfer_ErrStr());;
        }
        Xfer_Exit(sd, xfer_errno);
    }

    return 1;
}

/* Send a configuration record */

void Xfer_SendCnf(sd, cnf)
int sd;
struct xfer_cnf *cnf;
{
INT32 msglen;
static char *fid = "Xfer_SendCnf";

    if ((msglen = xfer_EncodeCnf(Xfer_Buffer+4, cnf)) < 0) {
        util_log(1, "%s: xfer_EncodeCnf: %s", fid, Xfer_ErrStr());
        Xfer_Ack(sd, xfer_errno);
        Xfer_Exit(sd, xfer_errno);
    }

    if (xfer_SendMsg(sd, Xfer_Buffer, msglen) != XFER_OK) {
        if (xfer_errno != XFER_ETIMEDOUT && xfer_errno != XFER_EPIPE) {
            util_log(2, "%s: xfer_SendMsg: %s", fid, Xfer_ErrStr());
        } else {
            util_log(2, "%s", Xfer_ErrStr());;
        }
        Xfer_Ack(sd, xfer_errno);
        Xfer_Exit(sd, xfer_errno);
    }
}

/* Server initialization */

int Xfer_ServInit(char *home, int timeo)
{
int i, ntoken;
FILE *fp;
#define MAX_TOKEN 10
char *fname, *token[MAX_TOKEN];
static char *fid = "Xfer_ServInit";

/* Set default time out interval */

    _xfer_timeout = timeo;

/* Install SIGPIPE handler */

    /*if (xfer_Signals() != XFER_OK) {
        xfer_errno = XFER_EHANDLER;;
        return XFER_ERROR;
    }*/

/* Read and store deny list */

    listInit(&deny);

    fname = Xfer_Buffer;

    sprintf(fname, "%s/%s", home, XFER_DENY);
    if ((fp = fopen(fname, "r")) == NULL) {
        util_log(1, "%s: %s", fname, syserrmsg(errno));
        util_log(1, "access control disabled");
    } else {
        while (utilGetLine(fp, Xfer_Buffer, MAXPATHLEN, '#', NULL) == 0) {
            if ((ntoken = utilParse(Xfer_Buffer, token, " \t", MAX_TOKEN, 0)) != 1) {
                util_log(1, "%s: bad token count", fname);
                util_log(1, "access control disabled");
                break;
            }
            for (i = 0; i < ntoken; i++) if (!listAppend(&deny, token[i], strlen(token[i])+1)) {
                util_log(1, "%s: %s", fname, syserrmsg(errno));
                util_log(1, "access control disabled");
                break;
            }
        }
        fclose(fp);
    }

    return XFER_OK;
}

/* Determine name of peer */

char *Xfer_Peer(sd)
int sd;
{
socklen_t addrlen;
struct sockaddr_in cli_addr, *cli_addrp;
struct hostent *hp;
static char hostname[MAXHOSTNAMELEN+1];
static char *not_a_socket = "stdio";
static char *fid = "Xfer_Peer";

    addrlen = sizeof(cli_addr);
    cli_addrp = &cli_addr;
    if (getpeername(sd, (struct sockaddr *)cli_addrp, &addrlen) != 0) {
        if (errno == ENOTSOCK) {
            return not_a_socket;
        } else {
            util_log(1, "%s: getpeername: %s", fid, syserrmsg(errno));
            Xfer_Exit(sd, errno);
        }
    }
    hp = gethostbyaddr((char *) &cli_addrp->sin_addr, sizeof(struct in_addr), cli_addrp->sin_family);
    if (hp != NULL) {
        strcpy(hostname, hp->h_name);
    } else {
        strcpy(hostname, inet_ntoa(cli_addrp->sin_addr));
    }

    return hostname;
}

/*  Check client access rights */

int Xfer_Allowed(int sd)
{
char *ident;
LNKLST_NODE *crnt;
static char *fid = "Xfer_Allowed";

    crnt = listFirstNode(&deny);
    while (crnt != NULL) {
        ident = (char *) crnt->payload;
        if (strcasecmp(ident, Xfer_Peer(sd)) == 0) return 0;
        crnt = listNextNode(crnt);
    }

    return 1;

}

/* Receive an arbitrary request */

int Xfer_RecvReq(sd, req)
int sd;
struct xfer_req *req;
{
int retval, ilen;
INT32 dummy;
static char *fid = "Xfer_RecvReq";

    ilen = sizeof(int);

    retval = xfer_RecvMsg(sd, Xfer_Buffer, Xfer_Buflen, &dummy);
    if (retval != XFER_OK) {
        if (xfer_errno != XFER_ETIMEDOUT) {
            util_log(1, "%s: xfer_RecvMsg: %s", fid, Xfer_ErrStr());
        }
        return XFER_ERROR;
    }

    if (xfer_DecodeReq(req, Xfer_Buffer) != XFER_OK) {
        return XFER_ERROR;
    }

/* If TCP buffers were specified, configure the socket accordingly */

    if (req->sndbuf > 0) {
        if ( setsockopt( sd, SOL_SOCKET, SO_SNDBUF, (char *) &req->sndbuf, ilen) != 0) {
            util_log(1, "%s: warning: setsockopt(req->sndbuf = %d): %s", fid, req->sndbuf, syserrmsg(errno));
        } else {
            util_log(2, "socket sndbuf set to %d bytes", req->sndbuf);
        }
    }

    if (req->rcvbuf > 0) {
        if ( setsockopt( sd, SOL_SOCKET, SO_SNDBUF, (char *) &req->rcvbuf, ilen) != 0) {
            util_log(1, "%s: warning: setsockopt(req->rcvbuf = %d): %s", fid, req->rcvbuf, syserrmsg(errno));
        } else {
            util_log(2, "socket rcvbuf set to %d bytes", req->rcvbuf);
        }
    }

    return XFER_OK;
}

/* Send a waveform record */

int Xfer_SendWav(sd, cnf, wav)
int sd;
struct xfer_cnf *cnf;
struct xfer_wav *wav;
{
INT32 msglen;
static char *fid = "Xfer_SendWav";

    if (xfer_Compress(cnf, wav) != XFER_OK) {
        util_log(1, "%s: xfer_Compress: %s", fid, Xfer_ErrStr());
        return XFER_ERROR;
    }

    if ((msglen = xfer_EncodeWav(Xfer_Buffer+4, wav)) < 0) {
        util_log(1, "%s: xfer_EncodeWav: %s", fid, Xfer_ErrStr());
        return XFER_ERROR;
    }

    if (xfer_SendMsg(sd, Xfer_Buffer, msglen) != XFER_OK) {
        if (xfer_errno != XFER_ETIMEDOUT && xfer_errno != XFER_EPIPE) {
            util_log(2, "%s: xfer_SendMsg: %s", fid, Xfer_ErrStr());
        } else {
            util_log(2, "%s", Xfer_ErrStr());;
        }
        return XFER_ERROR;
    }

    return XFER_OK;
}

/* Revision History
 *
 * $Log: server.c,v $
 * Revision 1.17  2015/12/04 22:31:17  dechavez
 * casts, format fixes and benign bug fixes to calm OS X compiles
 *
 * Revision 1.16  2012/02/14 20:22:33  dechavez
 * use portable integer types (eg, long to INT32, unsigned long to UINT32)
 *
 * Revision 1.15  2011/10/18 21:53:05  dechavez
 * fixed bug causing core dump when etc/edes.deny does not exist (duh)
 *
 * Revision 1.14  2011/08/04 22:02:36  dechavez
 * switch from allowed client list to deny list
 *
 * Revision 1.13  2006/11/10 06:30:26  dechavez
 * limited support for 8-byte data
 *
 * Revision 1.12  2006/02/09 20:01:48  dechavez
 * IDA handle support
 *
 * Revision 1.11  2005/05/25 22:41:56  dechavez
 * mods to calm Visual C++ warnings
 *
 * Revision 1.10  2005/05/23 21:02:24  dechavez
 * cleaned up includes (05-23 update AAP)
 *
 * Revision 1.9  2003/12/10 06:05:03  dechavez
 * various casts, includes, and the like to calm solaris cc
 *
 * Revision 1.8  2003/05/05 23:25:40  dechavez
 * change Xfer_Ack to return 1 instead of void
 *
 * Revision 1.7  2002/04/02 02:23:33  nobody
 * log io error messages at level 2
 *
 * Revision 1.6  2002/04/02 00:50:57  nobody
 * check for buffer overflows with incoming requests
 *
 * Revision 1.5  2002/02/21 22:39:16  dec
 * don't install any signal handlers
 *
 * Revision 1.4  2001/09/09 01:12:33  dec
 * pass data buffer length to ida_compress
 *
 * Revision 1.3  2000/06/24 23:43:36  dec
 * check access list fp before closing
 *
 * Revision 1.2  2000/03/16 06:45:45  nobody
 * missing clients file logged at level 2
 *
 * Revision 1.1.1.1  2000/02/08 20:20:43  dec
 * import existing IDA/NRTS sources
 *
 */
