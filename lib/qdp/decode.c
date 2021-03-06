#pragma ident "$Id: decode.c,v 1.24 2016/02/03 17:28:09 dechavez Exp $"
/*======================================================================
 *
 * Decode various QDP packets
 *
 *====================================================================*/
#include "qdp.h"

static int Decode_Version(UINT8 *start, QDP_SOFTVER *dest)
{
UINT8 *ptr;

    ptr = start;
    ptr += utilUnpackUINT16(ptr, &dest->raw);
    dest->major = dest->raw / 256;
    dest->minor = dest->raw % 256;

    return (int) (ptr - start);
}

static int Decode_DP_IPR(UINT8 *start, QDP_DP_IPR *dest)
{
UINT8 *ptr;
UINT32 nboIP; /* IP address in network byte order */

    ptr = start;
    ptr += utilUnpackUINT32(ptr, &dest->ip); nboIP = htonl(dest->ip);
    ptr += utilUnpackUINT16(ptr, &dest->port);
    ptr += utilUnpackUINT16(ptr, &dest->registration);
    inet_ntop(AF_INET, &nboIP, dest->dotdecimal, QDP_MAX_DOTDECIMAL_LEN+1);

    return (int) (ptr - start);
}

void qdpDecode_C1_SRVCH(UINT8 *start, QDP_TYPE_C1_SRVCH *dest)
{
UINT8 *ptr;

    ptr = start;

    ptr += utilUnpackUINT64(ptr, &dest->challenge);
    ptr += Decode_DP_IPR(ptr, &dest->dp);
}

void qdpDecode_C1_SRVRSP(UINT8 *start, QDP_TYPE_C1_SRVRSP *dest)
{
int i;
UINT8 *ptr;

    ptr = start;
    ptr += utilUnpackUINT64(ptr, &dest->serialno);
    ptr += utilUnpackUINT64(ptr, &dest->challenge);
    ptr += Decode_DP_IPR(ptr, &dest->dp);
    ptr += utilUnpackUINT64(ptr, &dest->random);
    for (i = 0; i < 4; i++) ptr += utilUnpackUINT32(ptr, &dest->md5[i]);
}

void qdpDecode_C1_RQMEM(UINT8 *start, QDP_TYPE_C1_RQMEM *dest)
{
int i;
UINT8 *ptr;

    ptr = start;
    ptr += utilUnpackUINT32(ptr, &dest->offset);
    ptr += utilUnpackUINT16(ptr, &dest->nbyte);
    ptr += utilUnpackUINT16(ptr, &dest->type);
    for (i = 0; i < 4; i++) ptr += utilUnpackUINT32(ptr, &dest->passwd[i]);
}

void qdpDecode_C1_MEM(UINT8 *start, QDP_TYPE_C1_MEM *dest)
{
int i, len;
UINT8 *ptr;

    ptr = start;
    ptr += utilUnpackUINT32(ptr, &dest->offset);
    ptr += utilUnpackUINT16(ptr, &dest->nbyte);
    ptr += utilUnpackUINT16(ptr, &dest->type);
    switch (dest->type) {
      case QDP_MEM_TYPE_CONFIG_DP1:
      case QDP_MEM_TYPE_CONFIG_DP2:
      case QDP_MEM_TYPE_CONFIG_DP3:
      case QDP_MEM_TYPE_CONFIG_DP4:
      case QDP_MEM_TYPE_WEBPAGE:
        ptr += utilUnpackUINT16(ptr, &dest->seg);  dest->nbyte -= sizeof(UINT16);
        ptr += utilUnpackUINT16(ptr, &dest->nseg); dest->nbyte -= sizeof(UINT16);
        break;
      default:
        dest->seg = dest->nseg = 1;
        break;
    }
    len = dest->nbyte < QDP_MAX_C1_MEM_PAYLOAD ? dest->nbyte : QDP_MAX_C1_MEM_PAYLOAD;
    ptr += utilUnpackBytes(ptr, dest->contents, len);
}

static void DecodeSampleInterval(QDP_FREQ *dest, UINT8 code)
{
static char *fid = "DecodeSampleInterval";

    dest->raw = code;
    if (dest->raw == 0xff) {
        dest->nsint = NANOSEC_PER_SEC * 10;
    } else if (dest->raw == 0x00) {
        dest->nsint = 0;
    } else if (dest->raw & 0x80) {
        switch (dest->raw & 0x0f) {
          case  4: dest->nsint = NANOSEC_PER_SEC /  200; break;
          case  5: dest->nsint = NANOSEC_PER_SEC /  250; break;
          case  6: dest->nsint = NANOSEC_PER_SEC /  300; break;
          case  7: dest->nsint = NANOSEC_PER_SEC /  400; break;
          case  8: dest->nsint = NANOSEC_PER_SEC /  500; break;
          case  9: dest->nsint = NANOSEC_PER_SEC /  800; break;
          case 10: dest->nsint = NANOSEC_PER_SEC / 1000; break;
          default: dest->nsint = 0; break;
        }
    } else {
        dest->nsint = NANOSEC_PER_SEC / dest->raw;
    }

    dest->dsint = (REAL64) dest->nsint / NANOSEC_PER_SEC;
}

void qdpDecode_C1_FIX(UINT8 *start, QDP_TYPE_C1_FIX *dest)
{
int i, freq;
UINT8 *ptr;

    ptr = start;
    ptr += utilUnpackUINT32(ptr, &dest->last_reboot);
    ptr += utilUnpackUINT32(ptr, &dest->reboots);
    ptr += utilUnpackUINT32(ptr, &dest->backup_map);
    ptr += utilUnpackUINT32(ptr, &dest->default_map);

    ptr += utilUnpackUINT16(ptr, &dest->cal_type);
    ptr += Decode_Version(ptr, &dest->cal_ver);
    ptr += utilUnpackUINT16(ptr, &dest->aux_type);
    ptr += Decode_Version(ptr, &dest->aux_ver);
    ptr += utilUnpackUINT16(ptr, &dest->clk_type);
    ptr += utilUnpackUINT16(ptr, &dest->flags);
    ptr += Decode_Version(ptr, &dest->sys_ver);
    ptr += Decode_Version(ptr, &dest->sp_ver);
    ptr += Decode_Version(ptr, &dest->pld_ver);
    ptr += utilUnpackUINT16(ptr, &dest->mem_block);

    ptr += utilUnpackUINT32(ptr, &dest->proper_tag);

    ptr += utilUnpackUINT64(ptr, &dest->sys_num);
    ptr += utilUnpackUINT64(ptr, &dest->amb_num);
    for (i = 0; i < QDP_NSENSOR; i++) ptr += utilUnpackUINT64(ptr, &dest->sensor_num[i]);

    ptr += utilUnpackUINT32(ptr, &dest->qapchp1_num);
    ptr += utilUnpackUINT32(ptr, &dest->int_sz);
    ptr += utilUnpackUINT32(ptr, &dest->int_used);
    ptr += utilUnpackUINT32(ptr, &dest->ext_sz);
    ptr += utilUnpackUINT32(ptr, &dest->flash_sz);
    ptr += utilUnpackUINT32(ptr, &dest->ext_used);
    ptr += utilUnpackUINT32(ptr, &dest->qapchp2_num);
    for (i = 0; i < QDP_NLP; i++) ptr += utilUnpackUINT32(ptr, &dest->log_sz[i]);

    for (freq = QDP_NFREQ-1; freq >= 0; freq--) DecodeSampleInterval(&dest->freq[freq], *ptr++);
    for (freq = QDP_NFREQ-1; freq >= 0; freq--) ptr += utilUnpackINT32(ptr, &dest->ch13_delay[freq]);
    for (freq = QDP_NFREQ-1; freq >= 0; freq--) ptr += utilUnpackINT32(ptr, &dest->ch46_delay[freq]);

}

void qdpDecode_C1_GID(UINT8 *start, QDP_TYPE_C1_GID *dest)
{
int i;
char *gid;
UINT8 *ptr, len;

    ptr = start;
    for (i = 0; i < QDP_NGID; i++) {
        len = *ptr++;
        gid = dest[i];
        ptr += utilUnpackBytes(ptr, (UINT8 *) gid, QDP_GIDLEN); gid[len] = 0;
    }
}

void qdpDecode_C1_SPP(UINT8 *start, QDP_TYPE_C1_SPP *dest)
{
UINT8 *ptr;

    ptr = start;
    ptr += utilUnpackUINT16(ptr, &dest->max_main_current);
    ptr += utilUnpackUINT16(ptr, &dest->min_off_time);
    ptr += utilUnpackUINT16(ptr, &dest->min_ps_voltage);
    ptr += utilUnpackUINT16(ptr, &dest->max_antenna_current);
    ptr += utilUnpackINT16(ptr, &dest->min_temp);
    ptr += utilUnpackINT16(ptr, &dest->max_temp);
    ptr += utilUnpackUINT16(ptr, &dest->temp_hysteresis);
    ptr += utilUnpackUINT16(ptr, &dest->volt_hysteresis);
    ptr += utilUnpackUINT16(ptr, &dest->default_vco);
}

void qdpDecode_C1_MYSN(UINT8 *start, QDP_TYPE_C1_MYSN *dest)
{
UINT8 *ptr;

    ptr = start;
    ptr += utilUnpackUINT64(ptr, &dest->serialno);
    ptr += utilUnpackUINT32(ptr, &dest->user_tag);
    ptr += utilUnpackUINT32(ptr, &dest->proper_tag);
}

void qdpDecode_C1_MAN(UINT8 *start, QDP_TYPE_C1_MAN *dest)
{
int i;
UINT8 *ptr;

    ptr = start;
    for (i = 0; i < 4; i++) ptr += utilUnpackUINT32(ptr, &dest->password[i]);
    ptr += utilUnpackUINT16(ptr, &dest->qapchp1_type);
    ptr += Decode_Version(ptr, &dest->qapchp1_ver);
    ptr += utilUnpackUINT16(ptr, &dest->qapchp2_type);
    ptr += Decode_Version(ptr, &dest->qapchp2_ver);
    ptr += utilUnpackUINT32(ptr, &dest->qapchp1_num);
    ptr += utilUnpackUINT32(ptr, &dest->qapchp2_num);
    for (i = 0; i < QDP_NCHAN; i++) ptr += utilUnpackINT32(ptr, &dest->reference[i]);
    ptr += utilUnpackUINT32(ptr, &dest->born_on);
    ptr += utilUnpackUINT32(ptr, &dest->packet_memory);
    ptr += utilUnpackUINT16(ptr, &dest->clk_type);
    ptr += utilUnpackUINT16(ptr, &dest->model);
    ptr += utilUnpackINT16(ptr, &dest->default_calib_offset);
    ptr += utilUnpackUINT16(ptr, &dest->flags);
    ptr += utilUnpackUINT32(ptr, &dest->proper_tag);
    ptr += utilUnpackUINT32(ptr, &dest->max_power_on);
}

void qdpDecode_C1_GLOB(UINT8 *start, QDP_TYPE_C1_GLOB *dest)
{
int chan, freq;
UINT8 *ptr;

    ptr = start;
    ptr += utilUnpackUINT16(ptr, &dest->clock_to);
    ptr += utilUnpackUINT16(ptr, &dest->initial_vco);
    ptr += utilUnpackUINT16(ptr, &dest->gps_backup);
    ptr += utilUnpackUINT16(ptr, &dest->samp_rates);
    ptr += utilUnpackUINT16(ptr, &dest->gain_map);
    ptr += utilUnpackUINT16(ptr, &dest->filter_map);
    ptr += utilUnpackUINT16(ptr, &dest->input_map);
    ptr += utilUnpackUINT16(ptr, &dest->web_port);
    ptr += utilUnpackUINT16(ptr, &dest->server_to);
    ptr += utilUnpackUINT16(ptr, &dest->drift_tol);
    ptr += utilUnpackUINT16(ptr, &dest->jump_filt);
    ptr += utilUnpackUINT16(ptr, &dest->jump_thresh);
    ptr += utilUnpackINT16(ptr, &dest->cal_offset);
    ptr += utilUnpackUINT16(ptr, &dest->sensor_map);
    ptr += utilUnpackUINT16(ptr, &dest->sampling_phase);
    ptr += utilUnpackUINT16(ptr, &dest->gps_cold);
    ptr += utilUnpackUINT32(ptr, &dest->user_tag);
    for (chan = 0; chan < QDP_NCHAN; chan++) {
        for (freq = 0; freq < QDP_NFREQ; freq++) {
            ptr += utilUnpackUINT16(ptr, &dest->scaling[chan][freq]);
        }
    }
    for (chan = 0; chan < QDP_NCHAN; chan++) ptr += utilUnpackUINT16(ptr, &dest->offset[chan]);
    for (chan = 0; chan < QDP_NCHAN; chan++) ptr += utilUnpackUINT16(ptr, &dest->gain[chan]);
    ptr += utilUnpackUINT32(ptr, &dest->msg_map);
}

void qdpDecode_C1_LOG(UINT8 *start, QDP_TYPE_C1_LOG *dest)
{
int chan;
UINT8 *ptr;

    ptr = start;
    ptr += utilUnpackUINT16(ptr, &dest->port);
    ptr += utilUnpackUINT16(ptr, &dest->flags);
    ptr += utilUnpackUINT16(ptr, &dest->perc);
    ptr += utilUnpackUINT16(ptr, &dest->mtu);
    ptr += utilUnpackUINT16(ptr, &dest->group_cnt);
    ptr += utilUnpackUINT16(ptr, &dest->rsnd_max);
    ptr += utilUnpackUINT16(ptr, &dest->grp_to);
    ptr += utilUnpackUINT16(ptr, &dest->rsnd_min);
    ptr += utilUnpackUINT16(ptr, &dest->window);
    ptr += utilUnpackUINT16(ptr, &dest->dataseq);
    for (chan = 0; chan < QDP_NCHAN; chan++) ptr += utilUnpackUINT16(ptr, &dest->freqs[chan]);
    ptr += utilUnpackUINT16(ptr, &dest->ack_cnt);
    ptr += utilUnpackUINT16(ptr, &dest->ack_to);
    ptr += utilUnpackUINT32(ptr, &dest->olddata);
    ptr += utilUnpackUINT16(ptr, &dest->eth_throttle);
    ptr += utilUnpackUINT16(ptr, &dest->full_alert);
    ptr += utilUnpackUINT16(ptr, &dest->auto_filter);
    ptr += utilUnpackUINT16(ptr, &dest->man_filter);
}

void qdpDecode_C1_COMBO(UINT8 *start, QDP_TYPE_C1_COMBO *dest)
{
struct {
    UINT16 glob;
    UINT16 sc;
    UINT16 log;
} offset;
UINT8 *ptr;


    ptr = start;
    ptr += utilUnpackUINT16(ptr, &offset.glob);
    ptr += utilUnpackUINT16(ptr, &offset.sc);
    ptr += utilUnpackUINT16(ptr, &offset.log);
    ptr += sizeof(UINT16); /* skip over spare */

    qdpDecode_C1_FIX(ptr, &dest->fix);
    qdpDecode_C1_GLOB(start + offset.glob, &dest->glob);
    qdpDecode_C1_LOG(start + offset.log, &dest->log);
}

void qdpDecode_C1_PHY(UINT8 *start, QDP_TYPE_C1_PHY *dest)
{
int i;
UINT8 *ptr;

    ptr = start;
    ptr += utilUnpackUINT64(ptr, &dest->serialno);
    for (i = 0; i < 3; i++) ptr += utilUnpackUINT32(ptr, &dest->serial[i].ip);
    ptr += utilUnpackUINT32(ptr, &dest->ethernet.ip);
    for (i = 0; i < 6; i++) dest->ethernet.mac[i] = *ptr++;
    ptr += utilUnpackUINT16(ptr, &dest->baseport);
    for (i = 0; i < 3; i++) {
        ptr += utilUnpackUINT16(ptr, &dest->serial[i].baud);
        ptr += utilUnpackUINT16(ptr, &dest->serial[i].flags);
    }
    ptr += utilUnpackUINT16(ptr, &dest->reserved[0]);
    ptr += utilUnpackUINT16(ptr, &dest->ethernet.flags);
    for (i = 0; i < 3; i++) ptr += utilUnpackUINT16(ptr, &dest->serial[i].throttle);
    ptr += utilUnpackUINT16(ptr, &dest->reserved[1]);
}

void qdpDecode_C1_SC(UINT8 *start, UINT32 *dest)
{
int i;
UINT8 *ptr;

    ptr = start;
    for (i = 0; i < QDP_NSC; i++) ptr += utilUnpackUINT32(ptr, &dest[i]);
}

void qdpDecode_C1_DCP(UINT8 *start, QDP_TYPE_C1_DCP *dest)
{
int i;
UINT8 *ptr;

    ptr = start;
    for (i = 0; i < QDP_NCHAN; i++) ptr += utilUnpackINT32(ptr, &dest->grounded[i]);
    for (i = 0; i < QDP_NCHAN; i++) ptr += utilUnpackINT32(ptr, &dest->reference[i]);
}

static int Decode_C2_AMASS_SENSOR(UINT8 *start, QDP_TYPE_C2_AMASS_SENSOR *dest)
{
int i;
UINT8 *ptr;

    ptr = start;
    for (i = 0; i < 3; i++) ptr += utilUnpackUINT16(ptr, &dest->tolerance[i]);
    ptr += utilUnpackUINT16(ptr, &dest->maxtry);
    ptr += utilUnpackUINT16(ptr, &dest->interval.normal);
    ptr += utilUnpackUINT16(ptr, &dest->interval.squelch);
    ptr += utilUnpackUINT16(ptr, &dest->bitmap);
    ptr += utilUnpackUINT16(ptr, &dest->duration);

    return (int) (ptr - start);
}

void qdpDecode_C2_AMASS(UINT8 *start, QDP_TYPE_C2_AMASS *dest)
{
int i;
UINT8 *ptr;

    ptr = start;
    for (i = 0; i < QDP_NSENSOR; i++) ptr += Decode_C2_AMASS_SENSOR(ptr, &dest->sensor[i]);
}

void qdpDecode_C2_GPS(UINT8 *start, QDP_TYPE_C2_GPS *dest)
{
UINT8 *ptr;

    ptr = start;
    ptr += utilUnpackUINT16(ptr, &dest->mode);
    ptr += utilUnpackUINT16(ptr, &dest->flags);
    ptr += utilUnpackUINT16(ptr, &dest->off_time);
    ptr += utilUnpackUINT16(ptr, &dest->resync);
    ptr += utilUnpackUINT16(ptr, &dest->max_on);
    ptr += utilUnpackUINT16(ptr, &dest->lock_usec);
    ptr += sizeof(UINT32); /* skip over spare */
    ptr += utilUnpackUINT16(ptr, &dest->interval);
    ptr += utilUnpackUINT16(ptr, &dest->initial_pll);
    ptr += utilUnpackREAL32(ptr, &dest->pfrac);
    ptr += utilUnpackREAL32(ptr, &dest->vco_slope);
    ptr += utilUnpackREAL32(ptr, &dest->vco_intercept);
    ptr += utilUnpackREAL32(ptr, &dest->max_ikm_rms);
    ptr += utilUnpackREAL32(ptr, &dest->ikm_weight);
    ptr += utilUnpackREAL32(ptr, &dest->km_weight);
    ptr += utilUnpackREAL32(ptr, &dest->best_weight);
    ptr += utilUnpackREAL32(ptr, &dest->km_delta);
    ptr += sizeof(UINT32); /* skip over spare */
    ptr += sizeof(UINT32); /* skip over spare */
}

void qdpDecode_C2_PHY(UINT8 *start, QDP_TYPE_C2_PHY *dest)
{
UINT8 *ptr;

    ptr = start;
    ptr += utilUnpackUINT16(ptr, &dest->phynum);
    ptr += utilUnpackUINT16(ptr, &dest->lognum);
    ptr += utilUnpackPascalString(ptr, dest->modem_init, QDP_PHY2_MODEM_INIT_LEN);
    ptr += utilUnpackPascalString(ptr, dest->phone_number, QDP_PHY2_PHONE_NUMBER_LEN);
    ptr += utilUnpackPascalString(ptr, dest->dial_out.name, QDP_PHY2_OUT_NAME_LEN);
    ptr += utilUnpackPascalString(ptr, dest->dial_in.name, QDP_PHY2_IN_NAME_LEN);
    ptr += 2; // skip over spare
    ptr += utilUnpackPascalString(ptr, dest->dial_out.pass, QDP_PHY2_OUT_PASS_LEN);
    ptr += utilUnpackPascalString(ptr, dest->dial_in.pass, QDP_PHY2_IN_PASS_LEN);
    ptr += 2; // skip over spare
    ptr += utilUnpackUINT32(ptr, &dest->trigger);
    ptr += utilUnpackUINT16(ptr, &dest->flags);
    ptr += utilUnpackUINT16(ptr, &dest->retry_int);
    ptr += utilUnpackUINT16(ptr, &dest->interval);
    ptr += utilUnpackUINT16(ptr, &dest->webbps);
    ptr += utilUnpackUINT32(ptr, &dest->poc_ip);
    ptr += utilUnpackUINT32(ptr, &dest->log2_ip);
    ptr += utilUnpackUINT16(ptr, &dest->poc_port);
    ptr += utilUnpackUINT16(ptr, &dest->retries);
    ptr += utilUnpackUINT16(ptr, &dest->reg_to);
    ptr += utilUnpackUINT16(ptr, &dest->baud);
    ptr += utilUnpackUINT16(ptr, &dest->routed_to);
    ptr += utilUnpackUINT16(ptr, &dest->ss);
}

void qdpDecodeRawEpDelay(QDP_EP_DELAY *delay)
{
    if (delay == NULL) return;

     delay->channel = delay->raw >> 24;
     delay->tics    = delay->raw & 0x00FFFFFF;
     delay->usec    = delay->tics * QDP_EPD_USPEC_PER_TIC;
}

BOOL qdpDecode_C2_EPD(UINT8 *start, QDP_TYPE_C2_EPD *dest)
{
int i;
UINT8 *ptr;
UINT32 value;
INT32 tics;
static char *fid = "qdpDecode_C2_EPD";

    ptr = start;
    ptr += utilUnpackUINT32(ptr, &dest->res1);
    ptr += utilUnpackUINT32(ptr, &dest->res2);
    ptr += utilUnpackUINT16(ptr, &dest->chancnt);
    ptr += utilUnpackUINT16(ptr, &dest->spare);
    if (dest->chancnt > QDP_MAX_EP_CHANNEL) dest->chancnt = QDP_MAX_EP_CHANNEL;
    for (i = 0; i < dest->chancnt; i++) {
        ptr += utilUnpackUINT32(ptr, &dest->entry[i].raw);
        qdpDecodeRawEpDelay(&dest->entry[i]);
    }

    return TRUE;
}

void qdpDecodeRawEpChanmask(QDP_EP_CHANMASK *chanmask)
{
    if (chanmask == NULL) return;

    chanmask->source  = (chanmask->raw >> 15) + 1; /* 1=>EP1, 2=>EP2 */
    chanmask->dpmask  = (chanmask->raw & 0x00FF);
    chanmask->channel = (chanmask->raw >> 8);
}

BOOL qdpDecode_C2_EPCFG(UINT8 *start, QDP_TYPE_C2_EPCFG *dest)
{
int i;
UINT8 *ptr;
static char *fid = "qdpDecode_C2_EPCFG";

    ptr = start;
    ptr += utilUnpackUINT32(ptr, &dest->res1);
    ptr += utilUnpackUINT32(ptr, &dest->res2);
    ptr += utilUnpackUINT32(ptr, &dest->flags1);
    ptr += utilUnpackUINT32(ptr, &dest->flags2);
    ptr += utilUnpackUINT16(ptr, &dest->chancnt);
    ptr += utilUnpackUINT16(ptr, &dest->spare);
    if (dest->chancnt > QDP_MAX_EP_CHANNEL) dest->chancnt = QDP_MAX_EP_CHANNEL;
    for (i = 0; i < dest->chancnt; i++) {
        ptr += utilUnpackUINT16(ptr, &dest->entry[i].raw);
        qdpDecodeRawEpChanmask(&dest->entry[i]);
    }

    return TRUE;
}


BOOL qdpDecode_C3_ANNC(UINT8 *start, QDP_TYPE_C3_ANNC *dest)
{
int i;
UINT8 *ptr;

    ptr = start;
    ptr += utilUnpackUINT16(ptr, &dest->c3_rqannc);
    ptr += utilUnpackUINT16(ptr, &dest->c3_annc);
#ifdef QUANTERRA_C3_ANNC_BUG_FIXED /* in fact the c3_rqannc field contains C3_SANNC */
    if (dest->c3_rqannc != QDP_C3_RQANNC || dest->c3_annc != QDP_C3_ANNC) return FALSE;
#endif
    ptr += utilUnpackUINT16(ptr, &dest->dps_active);
    ptr += utilUnpackUINT16(ptr, &dest->flags);
    for (i = 0; i < QDP_MAX_ANNC; i++) {
        ptr += utilUnpackUINT32(ptr, &dest->entry[i].dp_ip);
        ptr += utilUnpackUINT32(ptr, &dest->entry[i].router_ip);
        ptr += utilUnpackUINT16(ptr, &dest->entry[i].timeout);
        ptr += utilUnpackUINT16(ptr, &dest->entry[i].resume);
        ptr += utilUnpackUINT16(ptr, &dest->entry[i].flags);
        ptr += utilUnpackUINT16(ptr, &dest->entry[i].dp_port);
    }

    return TRUE;
}

BOOL qdpDecode_SSCFG(UINT8 *start, QDP_TYPE_SSCFG *dest)
{
UINT8 *ptr;

    ptr = start;
    ptr += utilUnpackUINT16(ptr, &dest->type);
    ptr += utilUnpackUINT16(ptr, &dest->si);
    ptr += utilUnpackUINT16(ptr, &dest->un);
    ptr += utilUnpackUINT16(ptr, &dest->it);
    ptr += utilUnpackUINT16(ptr, &dest->fd);
    ptr += sizeof(UINT16); /* skip over spare */
    ptr += utilUnpackUINT32(ptr, &dest->sn);
    ptr += utilUnpackREAL32(ptr, &dest->fp);
    ptr += utilUnpackREAL32(ptr, &dest->pa);
    ptr += utilUnpackREAL32(ptr, &dest->pm);
    ptr += utilUnpackREAL32(ptr, &dest->tc);
    ptr += utilUnpackREAL32(ptr, &dest->c1);
    ptr += utilUnpackREAL32(ptr, &dest->c2);
    ptr += utilUnpackREAL32(ptr, &dest->c3);
    ptr += utilUnpackREAL32(ptr, &dest->d1);
    ptr += utilUnpackREAL32(ptr, &dest->d2);
    ptr += utilUnpackREAL32(ptr, &dest->t1);
    ptr += utilUnpackREAL32(ptr, &dest->t2);
    ptr += utilUnpackREAL32(ptr, &dest->t3);
    ptr += utilUnpackREAL32(ptr, &dest->t4);
    ptr += utilUnpackREAL32(ptr, &dest->t5);
    ptr += utilUnpackREAL32(ptr, &dest->u0);
    ptr += utilUnpackREAL32(ptr, &dest->y1);
    ptr += utilUnpackREAL32(ptr, &dest->y2);
    ptr += utilUnpackREAL32(ptr, &dest->y3);
    ptr += utilUnpackPascalString(ptr, dest->model, QDP_SS_MODEL_LEN);
    ptr += utilUnpackPascalString(ptr, dest->version, QDP_SS_VERSION_LEN);
    return TRUE;
}

int qdpDecodeCMNHDR(UINT8 *start, QDP_CMNHDR *hdr)
{
UINT8 *ptr;

    ptr = start;
    ptr += utilUnpackUINT32(ptr, &hdr->crc);
    hdr->cmd = *ptr++;
    hdr->ver = *ptr++;
    ptr += utilUnpackUINT16(ptr, &hdr->dlen);
    ptr += utilUnpackUINT16(ptr, &hdr->seqno);
    ptr += utilUnpackUINT16(ptr, &hdr->ack);
    
    return (int) (ptr - start);
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
 * $Log: decode.c,v $
 * Revision 1.24  2016/02/03 17:28:09  dechavez
 * changes related to introduction of QDP_SOFTVER for versions
 *
 * Revision 1.23  2016/01/23 00:14:55  dechavez
 * added QDP_TYPE_C2_EPD, plus changes related to reworking of QDP_TYPE_C2_EPCFG structure layout
 *
 * Revision 1.22  2016/01/21 17:18:00  dechavez
 * added qdpExpandRawEPchannel() and qdpDecode_C2_EPCFG()
 *
 * Revision 1.21  2015/12/04 23:15:12  dechavez
 * casts, format fixes and the like, all to calm OS X compiles
 *
 * Revision 1.20  2015/12/03 18:04:16  dechavez
 * Fixed byte order problem building dot decimal string in Decode_DP_IPR
 *
 * Revision 1.19  2012/06/24 18:15:25  dechavez
 * qdpDecode_C1_LOG() leaves decoded port number 0-based
 *
 * Revision 1.18  2011/01/13 20:30:30  dechavez
 * added qdpDecode_C1_MYSN() (untested)
 *
 * Revision 1.17  2009/10/20 22:43:11  dechavez
 * qdpDecode_SSCFG() (not yet tested)
 *
 * Revision 1.16  2009/10/02 18:35:32  dechavez
 * added web page support to qdpDecode_C1_MEM()
 *
 * Revision 1.15  2009/09/28 17:42:34  dechavez
 * completed qdpDecode_C2_PHY(), added qdpDecode_C3_ANNC()
 *
 * Revision 1.14  2009/09/22 19:49:52  dechavez
 * checkpoint (qdpDecode_C2_PHY)
 *
 * Revision 1.13  2009/02/03 23:00:12  dechavez
 * added qdpDecode_C2_GPS(), qdpDecode_C1_DCP(), qdpDecode_C1_MAN(), qdpDecode_C1_SPP(), qdpDecode_C1_GID()
 *
 * Revision 1.12  2009/01/23 23:57:11  dechavez
 * fixed typo with qpdDecode_C1_FIX() name, added qdpDecode_C1_SC(), qdpDecode_C2_AMASS()
 *
 * Revision 1.11  2008/10/02 22:51:16  dechavez
 * added qdpDecode_C1_PHY
 *
 * Revision 1.10  2007/05/18 18:00:57  dechavez
 * initial production release
 *
 */
