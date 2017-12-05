#pragma ident "$Id: encode.c,v 1.20 2016/01/23 00:14:19 dechavez Exp $"
/*======================================================================
 *
 * Pack various data structures into QDP packets
 *
 *====================================================================*/
#include "qdp.h"

static int Encode_DP_IPR(UINT8 *start, QDP_DP_IPR *src)
{
UINT8 *ptr;

    ptr = start;
    ptr += utilPackUINT32(ptr, src->ip);
    ptr += utilPackUINT16(ptr, src->port);
    ptr += utilPackUINT16(ptr, src->registration);

    return (int) (ptr - start);
}

void qdpEncode_C1_SRVRSP(QDP_PKT *pkt, QDP_TYPE_C1_SRVRSP *src)
{
int i;
UINT8 *ptr;

    if (pkt == NULL || src == NULL) return;

    qdpInitPkt(pkt, QDP_C1_SRVRSP, 0, 0);

    ptr = pkt->payload;
    ptr += utilPackUINT64(ptr, src->serialno);
    ptr += utilPackUINT64(ptr, src->challenge);
    ptr += Encode_DP_IPR(ptr, &src->dp);
    ptr += utilPackUINT64(ptr, src->random);
    for (i = 0; i < 4; i++) ptr += utilPackUINT32(ptr, src->md5[i]);
    pkt->hdr.dlen = (int) (ptr - pkt->payload);
    pkt->len = QDP_CMNHDR_LEN + pkt->hdr.dlen;
}

void qdpEncode_C1_DSRV(QDP_PKT *pkt, UINT64 serialno)
{
UINT8 *ptr;

    if (pkt == NULL) return;

    qdpInitPkt(pkt, QDP_C1_DSRV, 0, 0);

    ptr = pkt->payload;
    ptr += utilPackUINT64(pkt->payload, serialno);
    pkt->hdr.dlen = (int) (ptr - pkt->payload);
    pkt->len = QDP_CMNHDR_LEN + pkt->hdr.dlen;
}

void qdpEncode_C1_RQSRV(QDP_PKT *pkt, UINT64 serialno)
{
UINT8 *ptr;

    if (pkt == NULL) return;

    qdpInitPkt(pkt, QDP_C1_RQSRV, 0, 0);

    ptr = pkt->payload;
    ptr += utilPackUINT64(pkt->payload, serialno);
    pkt->hdr.dlen = (int) (ptr - pkt->payload);
    pkt->len = QDP_CMNHDR_LEN + pkt->hdr.dlen;
}

void qdpEncode_C1_RQFLGS(QDP_PKT *pkt, UINT16 dataport)
{
UINT8 *ptr;

    if (pkt == NULL) return;

    qdpInitPkt(pkt, QDP_C1_RQFLGS, 0, 0);

    ptr = pkt->payload;
    ptr += utilPackUINT16(pkt->payload, dataport);
    pkt->hdr.dlen = (int) (ptr - pkt->payload);
    pkt->len = QDP_CMNHDR_LEN + pkt->hdr.dlen;
}

void qdpEncode_C1_RQLOG(QDP_PKT *pkt, INT16 port)
{
UINT8 *ptr;

    if (pkt == NULL) return;

    qdpInitPkt(pkt, QDP_C1_RQLOG, 0, 0);

    ptr = pkt->payload;
    ptr += utilPackINT16(pkt->payload, port);
    pkt->hdr.dlen = (int) (ptr - pkt->payload);
    pkt->len = QDP_CMNHDR_LEN + pkt->hdr.dlen;
}

void qdpEncode_C1_RQSTAT(QDP_PKT *pkt, UINT32 bitmap)
{
UINT8 *ptr;

    if (pkt == NULL) return;

    qdpInitPkt(pkt, QDP_C1_RQSTAT, 0, 0);

    ptr = pkt->payload;
    ptr += utilPackUINT32(pkt->payload, bitmap);
    pkt->hdr.dlen = (int) (ptr - pkt->payload);
    pkt->len = QDP_CMNHDR_LEN + pkt->hdr.dlen;
}

void qdpEncode_C1_CTRL(QDP_PKT *pkt, UINT16 flags)
{
UINT8 *ptr;

    if (pkt == NULL) return;

    qdpInitPkt(pkt, QDP_C1_CTRL, 0, 0);

    ptr = pkt->payload;
    ptr += utilPackUINT16(pkt->payload, flags);
    pkt->hdr.dlen = (int) (ptr - pkt->payload);
    pkt->len = QDP_CMNHDR_LEN + pkt->hdr.dlen;
}

void qdpEncode_C1_RQMEM(QDP_PKT *pkt, QDP_TYPE_C1_RQMEM *src)
{
int i;
UINT8 *ptr;

    if (pkt == NULL || src == NULL) return;

    qdpInitPkt(pkt, QDP_C1_RQMEM, 0, 0);

    ptr = pkt->payload;
    ptr += utilPackUINT32(ptr, src->offset);
    ptr += utilPackUINT16(ptr, src->nbyte);
    ptr += utilPackUINT16(ptr, src->type);
    for (i = 0; i < 4; i++) ptr += utilPackUINT32(ptr, src->passwd[i]);
    pkt->hdr.dlen = (int) (ptr - pkt->payload);
    pkt->len = QDP_CMNHDR_LEN + pkt->hdr.dlen;

}

void qdpEncode_C2_RQPHY(QDP_PKT *pkt, INT16 port)
{
UINT8 *ptr;

    if (pkt == NULL) return;

    qdpInitPkt(pkt, QDP_C2_RQPHY, 0, 0);

    ptr = pkt->payload;
    ptr += utilPackINT16(pkt->payload, port);
    pkt->hdr.dlen = (int) (ptr - pkt->payload);
    pkt->len = QDP_CMNHDR_LEN + pkt->hdr.dlen;
}

void qdpEncode_C1_QCAL(QDP_PKT *pkt, QDP_TYPE_C1_QCAL *qcal)
{
UINT8 *ptr;

    if (pkt == NULL || qcal == NULL) return;

    qdpInitPkt(pkt, QDP_C1_QCAL, 0, 0);

    ptr = pkt->payload;
    ptr += utilPackUINT32(ptr, qcal->starttime);
    ptr += utilPackUINT16(ptr, qcal->waveform);
    ptr += utilPackUINT16(ptr, qcal->amplitude);
    ptr += utilPackUINT16(ptr, qcal->duration);
    ptr += utilPackUINT16(ptr, qcal->settle);
    ptr += utilPackUINT16(ptr, qcal->chans);
    ptr += utilPackUINT16(ptr, qcal->trailer);
    ptr += utilPackUINT16(ptr, qcal->sensor);
    ptr += utilPackUINT16(ptr, qcal->monitor);
    ptr += utilPackUINT16(ptr, qcal->divisor);
    ptr += 2; /* skip over spare */
    ptr += 4; /* skip over coupling bytes 0 -  3 */
    ptr += 4; /* skip over coupling bytes 4 -  7 */
    ptr += 4; /* skip over coupling bytes 8 - 11 */
    pkt->hdr.dlen = (int) (ptr - pkt->payload);
    pkt->len = QDP_CMNHDR_LEN + pkt->hdr.dlen;
}

void qdpEncode_NoParCmd(QDP_PKT *pkt, int command)
{
int i;
UINT8 *ptr;

    if (pkt == NULL) return;

    qdpInitPkt(pkt, command, 0, 0);

    ptr = pkt->payload;
    pkt->hdr.dlen = (int) (ptr - pkt->payload);
    pkt->len = QDP_CMNHDR_LEN + pkt->hdr.dlen;

}

void qdpEncode_C1_LOG(QDP_PKT *pkt, QDP_TYPE_C1_LOG *log, UINT16 port)
{
int chan;
UINT8 *ptr;
UINT32 spare = 0;

    if (pkt == NULL || log == NULL) return;

    qdpInitPkt(pkt, QDP_C1_SLOG, 0, 0);

    ptr = pkt->payload;

    ptr += utilPackUINT16(ptr, port);
    ptr += utilPackUINT16(ptr, log->flags);
    ptr += utilPackUINT16(ptr, log->perc);
    ptr += utilPackUINT16(ptr, log->mtu);
    ptr += utilPackUINT16(ptr, log->group_cnt);
    ptr += utilPackUINT16(ptr, log->rsnd_max);
    ptr += utilPackUINT16(ptr, log->grp_to);
    ptr += utilPackUINT16(ptr, log->rsnd_min);
    ptr += utilPackUINT16(ptr, log->window);
    ptr += utilPackUINT16(ptr, log->dataseq);
    for (chan = 0; chan < QDP_NCHAN; chan++) ptr += utilPackUINT16(ptr, log->freqs[chan]);
    ptr += utilPackUINT16(ptr, log->ack_cnt);
    ptr += utilPackUINT16(ptr, log->ack_to);
    ptr += utilPackUINT32(ptr, log->olddata);
    ptr += utilPackUINT16(ptr, log->eth_throttle);
    ptr += utilPackUINT16(ptr, log->full_alert);
    ptr += utilPackUINT16(ptr, log->auto_filter);
    ptr += utilPackUINT16(ptr, log->man_filter);
    ptr += utilPackUINT32(ptr, spare);

    pkt->hdr.dlen = (int) (ptr - pkt->payload);
    pkt->len = QDP_CMNHDR_LEN + pkt->hdr.dlen;

}

void qdpEncode_C1_PHY(QDP_PKT *pkt, QDP_TYPE_C1_PHY *phy)
{
int i;
UINT8 *ptr;

    if (pkt == NULL || phy == NULL) return;

    qdpInitPkt(pkt, QDP_C1_SPHY, 0, 0);

    ptr = pkt->payload;

    ptr += utilPackUINT64(ptr, phy->serialno);
    for (i = 0; i < 3; i++) ptr += utilPackUINT32(ptr, phy->serial[i].ip);
    ptr += utilPackUINT32(ptr, phy->ethernet.ip);
    for (i = 0; i < 6; i++) *ptr++ = phy->ethernet.mac[i];
    ptr += utilPackUINT16(ptr, phy->baseport);
    for (i = 0; i < 3; i++) {
        ptr += utilPackUINT16(ptr, phy->serial[i].baud);
        ptr += utilPackUINT16(ptr, phy->serial[i].flags);
    }
    ptr += 2; /* skip over reserved */
    ptr += utilPackUINT16(ptr, phy->ethernet.flags);
    for (i = 0; i < 3; i++) ptr += utilPackUINT16(ptr, phy->serial[i].throttle);
    ptr += 2; /* skip over reserved */

    pkt->hdr.dlen = (int) (ptr - pkt->payload);
    pkt->len = QDP_CMNHDR_LEN + pkt->hdr.dlen;
}

void qdpEncode_C1_PULSE(QDP_PKT *pkt, UINT16 bitmap, UINT16 duration)
{
int i;
UINT8 *ptr;

    if (pkt == NULL) return;

    qdpInitPkt(pkt, QDP_C1_PULSE, 0, 0);

    ptr = pkt->payload;

    ptr += utilPackUINT16(ptr, bitmap);
    ptr += utilPackUINT16(ptr, duration);

    pkt->hdr.dlen = (int) (ptr - pkt->payload);
    pkt->len = QDP_CMNHDR_LEN + pkt->hdr.dlen;
}

void qdpEncode_C1_SC(QDP_PKT *pkt, UINT32 *sc)
{
int i;
UINT8 *ptr;

    if (pkt == NULL || sc == NULL) return;

    qdpInitPkt(pkt, QDP_C1_SSC, 0, 0);

    ptr = pkt->payload;

    for (i = 0; i < QDP_NSC; i++) ptr += utilPackUINT32(ptr, sc[i]);

    pkt->hdr.dlen = (int) (ptr - pkt->payload);
    pkt->len = QDP_CMNHDR_LEN + pkt->hdr.dlen;
}

void qdpEncode_C1_GLOB(QDP_PKT *pkt, QDP_TYPE_C1_GLOB *glob)
{
int chan, freq;
UINT8 *ptr;

    if (pkt == NULL || glob == NULL) return;

    qdpInitPkt(pkt, QDP_C1_SGLOB, 0, 0);

    ptr = pkt->payload;

    ptr += utilPackUINT16(ptr, glob->clock_to);
    ptr += utilPackUINT16(ptr, glob->initial_vco);
    ptr += utilPackUINT16(ptr, glob->gps_backup);
    ptr += utilPackUINT16(ptr, glob->samp_rates);
    ptr += utilPackUINT16(ptr, glob->gain_map);
    ptr += utilPackUINT16(ptr, glob->filter_map);
    ptr += utilPackUINT16(ptr, glob->input_map);
    ptr += utilPackUINT16(ptr, glob->web_port);
    ptr += utilPackUINT16(ptr, glob->server_to);
    ptr += utilPackUINT16(ptr, glob->drift_tol);
    ptr += utilPackUINT16(ptr, glob->jump_filt);
    ptr += utilPackUINT16(ptr, glob->jump_thresh);
    ptr += utilPackINT16(ptr, glob->cal_offset);
    ptr += utilPackUINT16(ptr, glob->sensor_map);
    ptr += utilPackUINT16(ptr, glob->sampling_phase);
    ptr += utilPackUINT16(ptr, glob->gps_cold);
    ptr += utilPackUINT32(ptr, glob->user_tag);
    for (chan = 0; chan < QDP_NCHAN; chan++) {
        for (freq = 0; freq < QDP_NFREQ; freq++) {
            ptr += utilPackUINT16(ptr, glob->scaling[chan][freq]);
        }
    }
    for (chan = 0; chan < QDP_NCHAN; chan++) ptr += utilPackUINT16(ptr, glob->offset[chan]);
    for (chan = 0; chan < QDP_NCHAN; chan++) ptr += utilPackUINT16(ptr, glob->gain[chan]);
    ptr += utilPackUINT32(ptr, glob->msg_map);

    pkt->hdr.dlen = (int) (ptr - pkt->payload);
    pkt->len = QDP_CMNHDR_LEN + pkt->hdr.dlen;
}

void qdpEncode_C1_SPP(QDP_PKT *pkt, QDP_TYPE_C1_SPP *spp)
{
UINT8 *ptr;

    if (pkt == NULL || spp == NULL) return;

    qdpInitPkt(pkt, QDP_C1_SSPP, 0, 0);

    ptr = pkt->payload;

    ptr += utilPackUINT16(ptr, spp->max_main_current);
    ptr += utilPackUINT16(ptr, spp->min_off_time);
    ptr += utilPackUINT16(ptr, spp->min_ps_voltage);
    ptr += utilPackUINT16(ptr, spp->max_antenna_current);
    ptr += utilPackINT16( ptr, spp->min_temp);
    ptr += utilPackINT16( ptr, spp->max_temp);
    ptr += utilPackUINT16(ptr, spp->temp_hysteresis);
    ptr += utilPackUINT16(ptr, spp->volt_hysteresis);
    ptr += utilPackUINT16(ptr, spp->default_vco);
    ptr += 2; /* skip over spare */

    pkt->hdr.dlen = (int) (ptr - pkt->payload);
    pkt->len = QDP_CMNHDR_LEN + pkt->hdr.dlen;
}

void qdpEncode_C1_POLLSN(QDP_PKT *pkt, UINT16 mask, UINT16 match)
{
UINT8 *ptr;

    if (pkt == NULL) return;

    qdpInitPkt(pkt, QDP_C1_POLLSN, 0, 0);
    ptr = pkt->payload;
    ptr += utilPackUINT16(ptr, mask);
    ptr += utilPackUINT16(ptr, match);

    pkt->hdr.dlen = (int) (ptr - pkt->payload);
    pkt->len = QDP_CMNHDR_LEN + pkt->hdr.dlen;
}

static int Encode_C2_AMASS_SENSOR(UINT8 *start, QDP_TYPE_C2_AMASS_SENSOR *sensor)
{
int i;
UINT8 *ptr;

    ptr = start;

    for (i = 0; i < 3; i++) ptr += utilPackUINT16(ptr, sensor->tolerance[i]);
    ptr += utilPackUINT16(ptr, sensor->maxtry);
    ptr += utilPackUINT16(ptr, sensor->interval.normal);
    ptr += utilPackUINT16(ptr, sensor->interval.squelch);
    ptr += utilPackUINT16(ptr, sensor->bitmap);
    ptr += utilPackUINT16(ptr, sensor->duration);

    return (int) (ptr - start);
}

void qdpEncode_C2_AMASS(QDP_PKT *pkt, QDP_TYPE_C2_AMASS *amass)
{
int i;
UINT8 *ptr;

    if (pkt == NULL || amass == NULL) return;

    qdpInitPkt(pkt, QDP_C2_SAMASS, 0, 0);

    ptr = pkt->payload;
    for (i = 0; i < QDP_NSENSOR; i++) ptr += Encode_C2_AMASS_SENSOR(ptr, &amass->sensor[i]);

    pkt->hdr.dlen = (int) (ptr - pkt->payload);
    pkt->len = QDP_CMNHDR_LEN + pkt->hdr.dlen;
}

void qdpEncode_C2_GPS(QDP_PKT *pkt, QDP_TYPE_C2_GPS *gps)
{
UINT8 *ptr;

    if (pkt == NULL || gps == NULL) return;

    qdpInitPkt(pkt, QDP_C2_SGPS, 0, 0);
    ptr = pkt->payload;

    ptr += utilPackUINT16(ptr, gps->mode);
    ptr += utilPackUINT16(ptr, gps->flags);
    ptr += utilPackUINT16(ptr, gps->off_time);
    ptr += utilPackUINT16(ptr, gps->resync);
    ptr += utilPackUINT16(ptr, gps->max_on);
    ptr += utilPackUINT16(ptr, gps->lock_usec);
    ptr += sizeof(UINT32); /* skip over spare */
    ptr += utilPackUINT16(ptr, gps->interval);
    ptr += utilPackUINT16(ptr, gps->initial_pll);
    ptr += utilPackREAL32(ptr, gps->pfrac);
    ptr += utilPackREAL32(ptr, gps->vco_slope);
    ptr += utilPackREAL32(ptr, gps->vco_intercept);
    ptr += utilPackREAL32(ptr, gps->max_ikm_rms);
    ptr += utilPackREAL32(ptr, gps->ikm_weight);
    ptr += utilPackREAL32(ptr, gps->km_weight);
    ptr += utilPackREAL32(ptr, gps->best_weight);
    ptr += utilPackREAL32(ptr, gps->km_delta);
    ptr += sizeof(UINT32); /* skip over spare */
    ptr += sizeof(UINT32); /* skip over spare */

    pkt->hdr.dlen = (int) (ptr - pkt->payload);
    pkt->len = QDP_CMNHDR_LEN + pkt->hdr.dlen;
}

void qdpEncode_C2_PHY(QDP_PKT *pkt, QDP_TYPE_C2_PHY *phy2)
{
UINT8 *ptr;
UINT16 spare = 0;

    if (pkt == NULL) return;

    qdpInitPkt(pkt, QDP_C2_SPHY, 0, 0);
    ptr = pkt->payload;

    ptr += utilPackUINT16(ptr, phy2->phynum);
    ptr += utilPackUINT16(ptr, phy2->lognum);
    ptr += utilPackPascalString(ptr, phy2->modem_init, QDP_PHY2_MODEM_INIT_LEN);
    ptr += utilPackPascalString(ptr, phy2->phone_number, QDP_PHY2_PHONE_NUMBER_LEN);
    ptr += utilPackPascalString(ptr, phy2->dial_out.name, QDP_PHY2_OUT_NAME_LEN);
    ptr += utilPackPascalString(ptr, phy2->dial_in.name, QDP_PHY2_IN_NAME_LEN);
    ptr += utilPackUINT16(ptr, spare);
    ptr += utilPackPascalString(ptr, phy2->dial_out.pass, QDP_PHY2_OUT_PASS_LEN);
    ptr += utilPackPascalString(ptr, phy2->dial_in.pass, QDP_PHY2_IN_PASS_LEN);
    ptr += utilPackUINT16(ptr, spare);
    ptr += utilPackUINT32(ptr, phy2->trigger);
    ptr += utilPackUINT16(ptr, phy2->flags);
    ptr += utilPackUINT16(ptr, phy2->retry_int);
    ptr += utilPackUINT16(ptr, phy2->interval);
    ptr += utilPackUINT16(ptr, phy2->webbps);
    ptr += utilPackUINT32(ptr, phy2->poc_ip);
    ptr += utilPackUINT32(ptr, phy2->log2_ip);
    ptr += utilPackUINT16(ptr, phy2->poc_port);
    ptr += utilPackUINT16(ptr, phy2->retries);
    ptr += utilPackUINT16(ptr, phy2->reg_to);
    ptr += utilPackUINT16(ptr, phy2->baud);
    ptr += utilPackUINT16(ptr, phy2->routed_to);
    ptr += utilPackUINT16(ptr, phy2->ss);

    pkt->hdr.dlen = (int) (ptr - pkt->payload);
    pkt->len = QDP_CMNHDR_LEN + pkt->hdr.dlen;
}

void qdpEncode_C3_ANNC(QDP_PKT *pkt, QDP_TYPE_C3_ANNC *annc)
{
int i;
UINT8 *ptr;

    if (pkt == NULL) return;

    qdpInitPkt(pkt, QDP_C2_TERC, 0, 0);
    ptr = pkt->payload;

    ptr += utilPackUINT16(ptr, QDP_C3_SANNC);
    ptr += utilPackUINT16(ptr, QDP_C3_ANNC);
    ptr += utilPackUINT16(ptr, annc->dps_active);
    ptr += utilPackUINT16(ptr, annc->flags);
    for (i = 0; i < QDP_MAX_ANNC; i++) {
        ptr += utilPackUINT32(ptr, annc->entry[i].dp_ip);
        ptr += utilPackUINT32(ptr, annc->entry[i].router_ip);
        ptr += utilPackUINT16(ptr, annc->entry[i].timeout);
        ptr += utilPackUINT16(ptr, annc->entry[i].resume);
        ptr += utilPackUINT16(ptr, annc->entry[i].flags);
        ptr += utilPackUINT16(ptr, annc->entry[i].dp_port);
    }

    pkt->hdr.dlen = (int) (ptr - pkt->payload);
    pkt->len = QDP_CMNHDR_LEN + pkt->hdr.dlen;
}

void qdpEncode_C3_RQANNC(QDP_PKT *pkt)
{
UINT8 *ptr;

    if (pkt == NULL) return;

    qdpInitPkt(pkt, QDP_C2_TERC, 0, 0);
    ptr = pkt->payload;

    ptr += utilPackUINT16(ptr, QDP_C3_RQANNC);
    ptr += utilPackUINT16(ptr, 0);

    pkt->hdr.dlen = (int) (ptr - pkt->payload);
    pkt->len = QDP_CMNHDR_LEN + pkt->hdr.dlen;
}

void qdpEncode_C2_EPCFG(QDP_PKT *pkt, QDP_TYPE_C2_EPCFG *epcfg)
{
int i;
UINT8 *ptr;

    if (pkt == NULL) return;

    qdpInitPkt(pkt, QDP_C2_SEPCFG, 0, 0);
    ptr = pkt->payload;

    ptr += utilPackUINT32(ptr, epcfg->res1);
    ptr += utilPackUINT32(ptr, epcfg->res2);
    ptr += utilPackUINT32(ptr, epcfg->flags1);
    ptr += utilPackUINT32(ptr, epcfg->flags2);
    ptr += utilPackUINT16(ptr, epcfg->chancnt);
    ptr += utilPackUINT16(ptr, epcfg->spare);
    for (i = 0; i < epcfg->chancnt; i++) ptr += utilPackUINT16(ptr, epcfg->entry[i].raw);

    pkt->hdr.dlen = (int) (ptr - pkt->payload);
    pkt->len = QDP_CMNHDR_LEN + pkt->hdr.dlen;

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
 * $Log: encode.c,v $
 * Revision 1.20  2016/01/23 00:14:19  dechavez
 * changes related to reworking of QDP_TYPE_C2_EPCFG structure layout
 *
 * Revision 1.19  2016/01/21 17:18:41  dechavez
 * added qdpEncode_C2_EPCFG()
 *
 * Revision 1.18  2012/06/24 18:12:42  dechavez
 * qdpEncode_C1_LOG() packs port number using supplied argument, ignoring C1_LOG field
 *
 * Revision 1.17  2011/01/12 23:12:38  dechavez
 * added qdpEncode_C1_POLLSN()
 *
 * Revision 1.16  2009/10/29 17:41:42  dechavez
 * added qdpEncode_C1_LOG(), qdpEncode_C2_AMASS(), qdpEncode_C2_PHY(), qdpEncode_C3_ANNC()
 *
 * Revision 1.15  2009/09/28 17:42:34  dechavez
 * completed qdpDecode_C2_PHY(), added qdpDecode_C3_ANNC()
 *
 * Revision 1.14  2009/09/22 19:50:43  dechavez
 * added qdpEncode_C1_RQLOG(), qdpEncode_C2_RQPHY()
 *
 * Revision 1.13  2009/02/23 21:56:30  dechavez
 * added qdpEncode_C1_SPP(), qdpEncode_C2_GPS()
 *
 * Revision 1.12  2009/01/24 00:08:09  dechavez
 * added qdpEncode_C1_PULSE(), qdpEncode_C1_SC(), qdpEncode_C1_GLOB()
 *
 * Revision 1.11  2009/01/06 20:46:00  dechavez
 * added qdpEncode_C1_PHY()
 *
 * Revision 1.10  2008/10/02 22:50:42  dechavez
 * added qdpEncode_NoParCmd
 *
 * Revision 1.9  2007/10/31 17:12:16  dechavez
 * added qdpEncode_C1_QCAL()
 *
 * Revision 1.8  2007/05/18 18:00:57  dechavez
 * initial production release
 *
 */
