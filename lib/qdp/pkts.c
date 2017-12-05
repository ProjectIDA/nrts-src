#pragma ident "$Id: pkts.c,v 1.27 2016/06/23 19:53:27 dechavez Exp $"
/*======================================================================
 * 
 * Simple packet utilities
 *
 *====================================================================*/
#include "qdp.h"

int qdpErrorCode(QDP_PKT *pkt)
{
UINT16 code;

    if (pkt == NULL || pkt->hdr.cmd != QDP_C1_CERR) return QDP_CERR_NOERR;
    utilUnpackUINT16(pkt->payload, &code);

    return (int) code;
}

void qdpInitPkt(QDP_PKT *pkt, UINT8 cmd, UINT16 seqno, UINT16 ack)
{
    memset(pkt->raw, 0, QDP_MAX_MTU);
    pkt->payload = &pkt->raw[QDP_CMNHDR_LEN];
    pkt->hdr.cmd = cmd;
    pkt->hdr.dlen = 0;
    pkt->hdr.seqno = seqno;
    pkt->hdr.ack = ack;
    pkt->hdr.ver = QDP_VERSION;
    pkt->len = QDP_CMNHDR_LEN;
}

void qdpCopyPkt(QDP_PKT *dest, QDP_PKT *src)
{
static char *fid = "qdpCopyPkt";

    memcpy(dest, src, sizeof(QDP_PKT));
    dest->payload = &dest->raw[QDP_CMNHDR_LEN];
}

int qdpReadPkt(FILE *fp, QDP_PKT *pkt)
{
    if (fp == NULL || pkt == NULL) {
        errno = EINVAL;
        return -1;
    }

    if (fread(pkt->raw, 1, QDP_CMNHDR_LEN, fp) != QDP_CMNHDR_LEN) {
        if (feof(fp)) {
            return 0;
        } else {
            return -2;
        }
    }

    qdpNetToHost(pkt);

    if (pkt->hdr.dlen > QDP_MAX_PAYLOAD) {
        errno = EFAULT;
        return -3;
    }

    if (fread(pkt->raw + QDP_CMNHDR_LEN, 1, pkt->hdr.dlen, fp) != pkt->hdr.dlen) return -4;

    errno = 0;
    return QDP_CMNHDR_LEN + pkt->hdr.dlen;
}

int qdpReadPktgz(gzFile *fp, QDP_PKT *pkt)
{
int errnum;

    if (fp == NULL || pkt == NULL) {
        errno = EINVAL;
        return -1;
    }

    if (gzread(fp, pkt->raw, QDP_CMNHDR_LEN) != QDP_CMNHDR_LEN) {
        gzerror(fp, &errnum);
        if (errnum == Z_STREAM_END) {
            return 0;
        } else {
            return -2;
        }
    }

    qdpNetToHost(pkt);

    if (pkt->hdr.dlen > QDP_MAX_PAYLOAD) {
        errno = EFAULT;
        return -3;
    }

    if (gzread(fp, pkt->raw + QDP_CMNHDR_LEN, pkt->hdr.dlen) != pkt->hdr.dlen) return -4;

    errno = 0;
    return QDP_CMNHDR_LEN + pkt->hdr.dlen;
}

int qdpWritePkt(FILE *fp, QDP_PKT *pkt)
{
    if (fp == NULL || pkt == NULL) {
        errno = EINVAL;
        return -1;
    }

    if (fwrite(pkt->raw, 1, QDP_CMNHDR_LEN, fp) != QDP_CMNHDR_LEN) return -2;
    if (fwrite(pkt->raw + QDP_CMNHDR_LEN, 1, pkt->hdr.dlen, fp) != pkt->hdr.dlen) return -3;

    return QDP_CMNHDR_LEN + pkt->hdr.dlen;
}

BOOL qdp_C1_RQGLOB(QDP *qdp, QDP_TYPE_C1_GLOB *glob)
{
QDP_PKT *pkt, local;

    if (qdp == NULL || glob == NULL) {
        errno = EINVAL;
        return FALSE;
    }

    pkt = (qdp->dbgpkt == NULL) ? &local : qdp->dbgpkt;

    if (!qdpNoParCmd(qdp, QDP_C1_RQGLOB, TRUE)) return FALSE;
    if (!qdpNextPkt(qdp, pkt)) return FALSE;
    if (pkt->hdr.cmd != QDP_C1_GLOB) {
        qdp->errcode = QDP_ERR_NOSUCH;
        return FALSE;
    }

    qdpDecode_C1_GLOB(pkt->payload, glob);

    return TRUE;
}

BOOL qdp_C1_RQSC(QDP *qdp, UINT32 *sc)
{
QDP_PKT *pkt, local;

    if (qdp == NULL || sc == NULL) {
        errno = EINVAL;
        return FALSE;
    }

    pkt = (qdp->dbgpkt == NULL) ? &local : qdp->dbgpkt;

    if (!qdpNoParCmd(qdp, QDP_C1_RQSC, TRUE)) return FALSE;
    if (!qdpNextPkt(qdp, pkt)) return FALSE;
    if (pkt->hdr.cmd != QDP_C1_SC) {
        qdp->errcode = QDP_ERR_NOSUCH;
        return FALSE;
    }

    qdpDecode_C1_SC(pkt->payload, sc);

    return TRUE;
}

BOOL qdp_C1_RQPHY(QDP *qdp, QDP_TYPE_C1_PHY *phy)
{
QDP_PKT *pkt, local;

    if (qdp == NULL || phy == NULL) {
        errno = EINVAL;
        return FALSE;
    }

    pkt = (qdp->dbgpkt == NULL) ? &local : qdp->dbgpkt;

    if (!qdpNoParCmd(qdp, QDP_C1_RQPHY, TRUE)) return FALSE;
    if (!qdpNextPkt(qdp, pkt)) return FALSE;
    if (pkt->hdr.cmd != QDP_C1_PHY) {
        qdp->errcode = QDP_ERR_NOSUCH;
        return FALSE;
    }

    qdpDecode_C1_PHY(pkt->payload, phy);

    return TRUE;
}

BOOL qdp_C1_RQFIX(QDP *qdp, QDP_TYPE_C1_FIX *fix)
{
QDP_PKT pkt;

    if (!qdpNoParCmd(qdp, QDP_C1_RQFIX, TRUE)) return FALSE;
    if (!qdpNextPkt(qdp, &pkt)) return FALSE;
    if (pkt.hdr.cmd != QDP_C1_FIX) {
        qdp->errcode = QDP_ERR_NOSUCH;
        return FALSE;
    }

    qdpDecode_C1_FIX(pkt.payload, fix);

    return TRUE;
}

BOOL qdp_C1_RQLOG(QDP *qdp, QDP_TYPE_C1_LOG *log, INT16 port)
{
QDP_PKT *pkt, local;

    if (qdp == NULL || log == NULL) {
        errno = EINVAL;
        return FALSE;
    }

    pkt = (qdp->dbgpkt == NULL) ? &local : qdp->dbgpkt;

    qdpEncode_C1_RQLOG(pkt, port);
    if (!qdpPostCmd(qdp, pkt, TRUE)) return FALSE;
    if (!qdpNextPkt(qdp, pkt)) return FALSE;
    if (pkt->hdr.cmd != QDP_C1_LOG) {
        qdp->errcode = QDP_ERR_NOSUCH;
        return FALSE;
    }

    qdpDecode_C1_LOG(pkt->payload, log);

    return TRUE;
}

BOOL qdp_C1_RQGID(QDP *qdp, QDP_TYPE_C1_GID *gid)
{
QDP_PKT pkt;

    if (!qdpNoParCmd(qdp, QDP_C1_RQGID, TRUE)) return FALSE;
    if (!qdpNextPkt(qdp, &pkt)) return FALSE;
    if (pkt.hdr.cmd != QDP_C1_GID) {
        qdp->errcode = QDP_ERR_NOSUCH;
        return FALSE;
    }

    qdpDecode_C1_GID(pkt.payload, gid);

    return TRUE;
}

BOOL qdp_C1_RQDCP(QDP *qdp, QDP_TYPE_C1_DCP *dcp)
{
QDP_PKT pkt;

    if (!qdpNoParCmd(qdp, QDP_C1_RQDCP, TRUE)) return FALSE;
    if (!qdpNextPkt(qdp, &pkt)) return FALSE;

    if (pkt.hdr.cmd != QDP_C1_DCP) {
        qdp->errcode = QDP_ERR_NOSUCH;
        return FALSE;
    }

    qdpDecode_C1_DCP(pkt.payload, dcp);

    return TRUE;
}

BOOL qdp_C1_RQSPP(QDP *qdp, QDP_TYPE_C1_SPP *spp)
{
QDP_PKT *pkt, local;

    if (qdp == NULL || spp == NULL) {
        errno = EINVAL;
        return FALSE;
    }

    pkt = (qdp->dbgpkt == NULL) ? &local : qdp->dbgpkt;


    if (!qdpNoParCmd(qdp, QDP_C1_RQSPP, TRUE)) return FALSE;
    if (!qdpNextPkt(qdp, pkt)) return FALSE;

    if (pkt->hdr.cmd != QDP_C1_SPP) {
        qdp->errcode = QDP_ERR_NOSUCH;
        return FALSE;
    }

    qdpDecode_C1_SPP(pkt->payload, spp);

    return TRUE;
}

BOOL qdp_C1_RQMAN(QDP *qdp, QDP_TYPE_C1_MAN *man)
{
QDP_PKT pkt;

    if (!qdpNoParCmd(qdp, QDP_C1_RQMAN, TRUE)) return FALSE;
    if (!qdpNextPkt(qdp, &pkt)) return FALSE;

    if (pkt.hdr.cmd != QDP_C1_MAN) {
        qdp->errcode = QDP_ERR_NOSUCH;
        return FALSE;
    }

    qdpDecode_C1_MAN(pkt.payload, man);

    return TRUE;
}

BOOL qdp_C2_RQAMASS(QDP *qdp, QDP_TYPE_C2_AMASS *amass)
{
QDP_PKT *pkt, local;

    if (qdp == NULL || amass == NULL) {
        errno = EINVAL;
        return FALSE;
    }

    pkt = (qdp->dbgpkt == NULL) ? &local : qdp->dbgpkt;

    if (!qdpNoParCmd(qdp, QDP_C2_RQAMASS, TRUE)) return FALSE;
    if (!qdpNextPkt(qdp, pkt)) return FALSE;
    if (pkt->hdr.cmd != QDP_C2_AMASS) {
        qdp->errcode = QDP_ERR_NOSUCH;
        return FALSE;
    }

    qdpDecode_C2_AMASS(pkt->payload, amass);

    return TRUE;
}

BOOL qdp_C2_RQGPS(QDP *qdp, QDP_TYPE_C2_GPS *gps)
{
QDP_PKT *pkt, local;

    if (qdp == NULL || gps == NULL) {
        errno = EINVAL;
        return FALSE;
    }

    pkt = (qdp->dbgpkt == NULL) ? &local : qdp->dbgpkt;

    if (!qdpNoParCmd(qdp, QDP_C2_RQGPS, TRUE)) return FALSE;
    if (!qdpNextPkt(qdp, pkt)) return FALSE;
    if (pkt->hdr.cmd != QDP_C2_GPS) {
        qdp->errcode = QDP_ERR_NOSUCH;
        return FALSE;
    }

    qdpDecode_C2_GPS(pkt->payload, gps);

    return TRUE;
}

BOOL qdp_C2_RQPHY(QDP *qdp, QDP_TYPE_C2_PHY *phy, INT16 port)
{
QDP_PKT *pkt, local;

    if (qdp == NULL || phy == NULL) {
        errno = EINVAL;
        return FALSE;
    }

    pkt = (qdp->dbgpkt == NULL) ? &local : qdp->dbgpkt;

    qdpEncode_C2_RQPHY(pkt, port);
    if (!qdpPostCmd(qdp, pkt, TRUE)) return FALSE;
    if (!qdpNextPkt(qdp, pkt)) return FALSE;
    if (pkt->hdr.cmd != QDP_C2_PHY) {
        qdp->errcode = QDP_ERR_NOSUCH;
        return FALSE;
    }

    qdpDecode_C2_PHY(pkt->payload, phy);

    return TRUE;
}

BOOL qdp_C1_RQSTAT(QDP *qdp, UINT32 bitmap, QDP_TYPE_C1_STAT *stat)
{
QDP_PKT pkt;

    if (!qdpRqstat(qdp, bitmap, TRUE)) return FALSE;
    if (!qdpNextPkt(qdp, &pkt)) return FALSE;
    if (pkt.hdr.cmd != QDP_C1_STAT) {
        qdp->errcode = QDP_ERR_NOSUCH;
        return FALSE;
    }

    qdpDecode_C1_STAT(pkt.payload, stat);

    return TRUE;
}

BOOL qdp_C1_RQMEM(QDP *qdp, UINT16 type, QDP_MEMBLK *blk)
{
int i;
QDP_PKT pkt;
QDP_TYPE_C1_MEM c1_mem;
QDP_TYPE_C1_RQMEM c1_rqmem;

    qdpInitMemBlk(blk, type);
    c1_rqmem.nbyte = 0;
    c1_rqmem.type = type;
    for (i = 0; i < 4; i++) c1_rqmem.passwd[i] = 0;

    while (!blk->full) {
        if (blk->nbyte == 0) {
             c1_rqmem.offset = 0;
        } else {
             c1_rqmem.offset = c1_mem.seg * QDP_MAX_C1_MEM_PAYLOAD;
        }
        qdpEncode_C1_RQMEM(&pkt, &c1_rqmem);
        if (!qdpMemCmd(qdp, &pkt, 1000, 10, stdout)) {
            printf("qdp_C1_RQMEM:qdpMemCmd: %s\n", qdpErrString(qdp->cmd.err));
            return FALSE;
        }
        if (!qdpNextPkt(qdp, &pkt)) {
            printf("qdp_C1_RQMEM:qdpNextPkt: %s\n", qdpErrString(qdp->cmd.err));
            return FALSE;
        }
        if (pkt.hdr.cmd != QDP_C1_MEM) {
            qdp->errcode = QDP_ERR_NOSUCH;
            printf("qdp_C1_RQMEM:pkt.hdr.cmd=%d: %s\n", pkt.hdr.cmd, qdpErrString(qdp->cmd.err));
            return FALSE;
        }
        qdpDecode_C1_MEM(pkt.payload, &c1_mem);
        if (!qdpSaveMem(blk, &c1_mem)) return FALSE;
    }
    
    return TRUE;
}

BOOL qdpRequestTokens(QDP *qdp, QDP_DP_TOKEN *token, UINT16 port)
{
QDP_MEMBLK blk;

    if (!qdp_C1_RQMEM(qdp, port, &blk)) return FALSE;
    if (!qdpUnpackTokens(&blk, token)) return FALSE;

    return TRUE;
}

BOOL qdp_C3_RQANNC(QDP *qdp, QDP_TYPE_C3_ANNC *annc)
{
QDP_PKT *pkt, local;

    if (qdp == NULL || annc == NULL) {
        errno = EINVAL;
        return FALSE;
    }

    pkt = (qdp->dbgpkt == NULL) ? &local : qdp->dbgpkt;

    qdpEncode_C3_RQANNC(pkt);
    if (!qdpPostCmd(qdp, pkt, TRUE)) return FALSE;
    if (!qdpNextPkt(qdp, pkt)) return FALSE;
    if (pkt->hdr.cmd != QDP_C2_TERR) {
        qdp->errcode = QDP_ERR_NOSUCH;
        return FALSE;
    }

    if (!qdpDecode_C3_ANNC(pkt->payload, annc)) {
        qdp->errcode = QDP_ERR_NOSUCH;
        return FALSE;
    }

    return TRUE;
}

BOOL qdp_C2_RQEPD(QDP *qdp, QDP_TYPE_C2_EPD *epd)
{
BOOL supported;
QDP_PKT *pkt, local;

    if (qdp == NULL || epd == NULL) {
        errno = EINVAL;
        return FALSE;
    }

    supported = (qdp->flags & QDP_FLAGS_EP_SUPPORTED);

    if (!supported) {
        memset(epd, 0, sizeof(QDP_TYPE_C2_EPD));
        return TRUE;
    }

    pkt = (qdp->dbgpkt == NULL) ? &local : qdp->dbgpkt;

    if (!qdpNoParCmd(qdp, QDP_C2_RQEPD, TRUE)) return FALSE;
    if (!qdpNextPkt(qdp, pkt)) return FALSE;
    if (pkt->hdr.cmd != QDP_C2_EPD) {
        qdp->errcode = QDP_ERR_NOSUCH;
        return FALSE;
    }

    if (!qdpDecode_C2_EPD(pkt->payload, epd)) {
        qdp->errcode = QDP_ERR_NOSUCH;
        return FALSE;
    }

    return TRUE;
}

BOOL qdp_C2_RQEPCFG(QDP *qdp, QDP_TYPE_C2_EPCFG *epcfg)
{
BOOL supported;
QDP_PKT *pkt, local;

    if (qdp == NULL || epcfg == NULL) {
        errno = EINVAL;
        return FALSE;
    }

    supported = (qdp->flags & QDP_FLAGS_EP_SUPPORTED);

    if (!supported) {
        memset(epcfg, 0, sizeof(QDP_TYPE_C2_EPCFG));
        return TRUE;
    }

    pkt = (qdp->dbgpkt == NULL) ? &local : qdp->dbgpkt;

    if (!qdpNoParCmd(qdp, QDP_C2_RQEPCFG, TRUE)) return FALSE;
    if (!qdpNextPkt(qdp, pkt)) return FALSE;
    if (pkt->hdr.cmd != QDP_C2_EPCFG) {
        qdp->errcode = QDP_ERR_NOSUCH;
        return FALSE;
    }

    if (!qdpDecode_C2_EPCFG(pkt->payload, epcfg)) {
        qdp->errcode = QDP_ERR_NOSUCH;
        return FALSE;
    }

    return TRUE;
}

BOOL qdp_C1_RQCOMBO(QDP *qdp, QDP_TYPE_C1_COMBO *combo, INT16 port)
{
QDP_PKT *pkt, local;

    if (qdp == NULL || combo == NULL) {
        errno = EINVAL;
        return FALSE;
    }

    pkt = (qdp->dbgpkt == NULL) ? &local : qdp->dbgpkt;

    qdpEncode_C1_RQFLGS(pkt, port);
    if (!qdpPostCmd(qdp, pkt, TRUE)) return FALSE;
    if (!qdpNextPkt(qdp, pkt)) return FALSE;
    if (pkt->hdr.cmd != QDP_C1_FLGS) {
        qdp->errcode = QDP_ERR_NOSUCH;
        return FALSE;
    }

    qdpDecode_C1_COMBO(pkt->payload, combo);

    return TRUE;
}

BOOL qdpRequestCheckout(QDP *qdp, QDP_TYPE_CHECKOUT *co, char *ident)
{
static UINT32 bitmap = QDP_SRB_BOOM;

    strlcpy(co->ident, ident, QDP_IDENTLEN);
    if (!qdp_C1_RQFIX(qdp, &co->fix)) return FALSE;
    if (!qdp_C1_RQGLOB(qdp, &co->glob)) return FALSE;
    if (!qdp_C1_RQDCP(qdp, &co->dcp)) return FALSE;
    if (!qdp_C1_RQSC(qdp,  co->sc)) return FALSE;
    if (!qdp_C1_RQGID(qdp, co->gid)) return FALSE;
    if (!qdp_C2_RQGPS(qdp, &co->gps)) return FALSE;
    if (!qdp_C2_RQAMASS(qdp, &co->amass)) return FALSE;
    if (!qdp_C1_RQSTAT(qdp, bitmap, &co->status)) return FALSE;
    if (!qdp_C1_RQPHY(qdp, &co->phy)) return FALSE;
    if (!qdp_C1_RQSPP(qdp, &co->spp)) return FALSE;
    if (!qdp_C1_RQMAN(qdp, &co->man)) return FALSE;
    return TRUE;
}

static void MergeStatus(QDP_TYPE_C1_STAT *dest, QDP_TYPE_C1_STAT *src)
{
    if (src->bitmap & QDP_SRB_GBL )     dest->gbl  = src->gbl;      dest->bitmap |= QDP_SRB_GBL;
    if (src->bitmap & QDP_SRB_BOOM)     dest->boom = src->boom;     dest->bitmap |= QDP_SRB_BOOM;
    if (src->bitmap & QDP_SRB_GPS )     dest->gps  = src->gps;      dest->bitmap |= QDP_SRB_GPS;
    if (src->bitmap & QDP_SRB_EP  )    dest->ep[0] = src->ep[0];    dest->bitmap |= QDP_SRB_EP;
    if (src->bitmap & QDP_SRB_EP  )    dest->ep[1] = src->ep[1];    dest->bitmap |= QDP_SRB_EP;
    if (src->bitmap & QDP_SRB_DP1 ) dest->dport[0] = src->dport[0]; dest->bitmap |= QDP_SRB_DP1;
    if (src->bitmap & QDP_SRB_DP2 ) dest->dport[1] = src->dport[1]; dest->bitmap |= QDP_SRB_DP2;
    if (src->bitmap & QDP_SRB_DP3 ) dest->dport[2] = src->dport[2]; dest->bitmap |= QDP_SRB_DP3;
    if (src->bitmap & QDP_SRB_DP4 ) dest->dport[3] = src->dport[3]; dest->bitmap |= QDP_SRB_DP4;
}

BOOL qdpRequestMonitor(QDP *qdp, QDP_TYPE_MONITOR *mon)
{
QDP_TYPE_C1_STAT tmpstat;
static UINT32 bitmap1 = QDP_SRB_GBL | QDP_SRB_BOOM | QDP_SRB_GPS | QDP_SRB_SS;
static UINT32 bitmap2 = QDP_SRB_DP1 | QDP_SRB_DP2  | QDP_SRB_DP3 | QDP_SRB_DP4;
static UINT32 bitmap3 = QDP_SRB_EP;

    memset(mon, 0, sizeof(QDP_TYPE_MONITOR));

    if (!qdp_C1_RQSTAT(qdp, bitmap1, &tmpstat)) return FALSE;
    MergeStatus(&mon->stat, &tmpstat);

    if (!qdp_C1_RQSTAT(qdp, bitmap2, &tmpstat)) return FALSE;
    MergeStatus(&mon->stat, &tmpstat);

    if (!qdp_C1_RQSTAT(qdp, bitmap3, &tmpstat)) return FALSE;
    MergeStatus(&mon->stat, &tmpstat);

    if (!qdp_C1_RQFIX(qdp, &mon->fix)) return FALSE;
    if (!qdp_C1_RQSC(qdp,  mon->sc)) return FALSE;
    if (!qdp_C2_RQGPS(qdp, &mon->gps)) return FALSE;
    if (!qdp_C1_RQSPP(qdp, &mon->spp)) return FALSE;
    if (!qdp_C1_RQGLOB(qdp, &mon->glob)) return FALSE;

    return TRUE;
}

/*-----------------------------------------------------------------------+
 |                                                                       |
 | Copyright (C) 2006 Regents of the University of California            |
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
 * $Log: pkts.c,v $
 * Revision 1.27  2016/06/23 19:53:27  dechavez
 * introduced qdp_C1_RQCOMBO()
 *
 * Revision 1.26  2016/02/03 17:33:18  dechavez
 * check handle for EP support and fake replies from qdp_C2_RQEPD() and qdp_C2_RQEPCFG() if appropriate
 *
 * Revision 1.25  2016/01/28 22:12:02  dechavez
 * Evidently adding QDP_SRB_EP (bit 20) to the status bitmap doesn't work when
 * in the presence of other items requested (result too big?).  Anyway, worked
 * around this in qdpRequestMonitor() by making a single QDP_SRB_EP status
 * request and merging all the results into a single QDP_TYPE_C1_STAT.
 *
 * Revision 1.24  2016/01/23 00:17:05  dechavez
 * added qdp_C2_RQEPD()
 *
 * Revision 1.23  2016/01/21 17:19:08  dechavez
 * added qdp_C2_RQEPCFG()
 *
 * Revision 1.22  2011/01/03 21:24:25  dechavez
 * added qdp_C1_RQGLOB to qdpRequestMonitor()
 *
 * Revision 1.21  2010/10/20 18:33:58  dechavez
 * added QDP_SRB_SS to status request bitmap in qdpRequestMonitor()
 *
 * Revision 1.20  2010/10/19 21:12:54  dechavez
 * 2.2.1
 *
 * Revision 1.19  2009/11/13 00:25:02  dechavez
 * use qdpMemCmd() instead of qdpPostCmd() in qdp_C1_RQMEM
 *
 * Revision 1.18  2009/11/05 18:36:38  dechavez
 * new type argument for qdpInitMemBlk()
 *
 * Revision 1.17  2009/10/29 17:36:52  dechavez
 * use dbgpkt if available in qdp_C1_RQGLOB(), qdp_C1_RQSC(), qdp_C1_RQPHY(),
 * qdp_C1_RQLOG(), qdp_C1_RQSPP(), qdp_C2_RQAMASS(), qdp_C2_RQPHY, qdp_C3_RQANNC()
 *
 * Revision 1.16  2009/10/02 18:34:12  dechavez
 * moved qdpRequestFullConfig() to config.c
 *
 * Revision 1.15  2009/09/28 17:41:30  dechavez
 * added qdpRequestTokens(), qdp_C3_RQANNC(), updated qdpRequestFullConfig()
 *
 * Revision 1.14  2009/09/22 19:51:40  dechavez
 * added qdp_C1_RQLOG(), qdp_C2_RQPHY(), qdpRequestFullConfig()
 *
 * Revision 1.13  2009/09/15 23:11:47  dechavez
 * added qdp_C1_RQMEM()
 *
 * Revision 1.12  2009/07/02 16:58:30  dechavez
 * added qdpReadPktgz()
 *
 * Revision 1.11  2009/07/02 16:34:52  dechavez
 * explicitly set errno to zero on normal qdpRead() returns
 *
 * Revision 1.10  2009/02/03 23:04:03  dechavez
 * added qdpRequestCheckout(), qdp_C1_RQSTAT(), qdp_C2_RQGPS(), qdp_C1_RQMAN,
 * qdp_C1_RQSPP(), qdp_C1_RQDCP(), qdp_C1_RQGID(), qdp_C1_RQFIX(), qdp_C1_RQPHY(),
 * qdp_C1_RQSC(), qdp_C1_RQGLOB()
 *
 * Revision 1.9  2007/05/18 18:00:57  dechavez
 * initial production release
 *
 */
