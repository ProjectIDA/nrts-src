#pragma ident "$Id: status.c,v 1.20 2017/09/06 21:58:19 dechavez Exp $"
/*======================================================================
 * 
 * Deal with C1_STAT packets
 *
 *====================================================================*/
#include "qdp.h"
#include "util.h"

/* Print contents of a decoded C1_STAT packet */

static void BinUINT32(FILE *fp, UINT16 value)
{
    utilPrintBinUINT8(fp, (UINT8) (value >> 24));
    fprintf(fp, " ");
    utilPrintBinUINT8(fp, (UINT8) value >> 16);
    fprintf(fp, " ");
    utilPrintBinUINT8(fp, (UINT8) (value >> 8));
    fprintf(fp, " ");
    utilPrintBinUINT8(fp, (UINT8) value);
    fprintf(fp, "\n");
}

static void BinUINT16(FILE *fp, UINT16 value)
{
    utilPrintBinUINT8(fp, (UINT8) (value >> 8));
    fprintf(fp, " ");
    utilPrintBinUINT8(fp, (UINT8) value);
    fprintf(fp, "\n");
}

static void BinUINT8(FILE *fp, UINT8 value)
{
    utilPrintBinUINT8(fp, value);
    fprintf(fp, "\n");
}

static void PrintGBL(FILE *fp, QDP_STAT_GBL *gbl)
{
char tmpbuf[80];

    fprintf(fp, "Global Status\n");
    fprintf(fp, "-------------\n");
    fprintf(fp, "               Timestamp = %s\n", utilDttostr(gbl->tstamp + QDP_EPOCH_TO_1970_EPOCH, 0, tmpbuf));
    fprintf(fp, "     Acquisition Control = 0x%04x\n", gbl->aqctrl);
    fprintf(fp, "           Clock Quality = %d%%\n", gbl->clock_perc);
    fprintf(fp, "      Minutes Since Loss = %hd\n", gbl->clock_loss);
    fprintf(fp, "          Seconds Offset = %u.%06u\n", gbl->sec_offset, gbl->usec_offset);
    fprintf(fp, "             Total Hours = %4.2f\n", (float) (gbl->total_time / SEC_PER_HOUR));
    fprintf(fp, "          Power On Hours = %4.2f\n", (float) (gbl->power_time / SEC_PER_HOUR));
    fprintf(fp, "    Time of Last Re-Sync = %s\n", utilLttostr(gbl->last_resync + QDP_EPOCH_TO_1970_EPOCH, 0, tmpbuf));
    fprintf(fp, "Total Number of Re-Syncs = %d\n", (int) gbl->resyncs);
    fprintf(fp, "              GPS Status = "); BinUINT16(fp, gbl->gps_stat);
    fprintf(fp, "       Calibrator Status = "); BinUINT16(fp, gbl->cal_stat);
    fprintf(fp, "  Sensor Control Bitmmap = "); BinUINT16(fp, gbl->sensor_map);
    fprintf(fp, "           Status Inputs = "); BinUINT16(fp, gbl->stat_inp);
    fprintf(fp, "    Miscellaneous Inputs = "); BinUINT16(fp, gbl->misc_inp);
    fprintf(fp, "    Data sequence number = %hu\n", gbl->data_seq);
    fprintf(fp, " Current sequence number = %u\n", gbl->cur_sequence);
    fprintf(fp, "             Current VCO = %d", gbl->cur_vco & 0x7ffff);
    if (gbl->cur_vco & 0x8000) fprintf(fp, " (under PLL control)"); fprintf(fp, "\n");

    fprintf(fp, "\n");
}

static void PrintGPS(FILE *fp, QDP_STAT_GPS *gps)
{
char tmpbuf[80];

    fprintf(fp, "GPS Status\n");
    fprintf(fp, "----------\n");
    fprintf(fp, "        Timestamp = %s\n", utilDttostr(gps->tstamp + QDP_EPOCH_TO_1970_EPOCH, 0, tmpbuf));
    if (gps->gpson) {
        fprintf(fp, "          ON ");
    } else {
        fprintf(fp, "         OFF ");
    }
    fprintf(fp, "Time = %d min\n", (int) gps->gpstime);
    fprintf(fp, "       Satellites = %d used, %d in view\n", (int) gps->sat_used, (int) gps->sat_view);
    fprintf(fp, "             Time = %s\n", gps->time);
    fprintf(fp, "             Date = %s\n", gps->date);
    fprintf(fp, "              Fix = %s\n", gps->fix);
    fprintf(fp, "        Elevation = %s\n", gps->height);
    fprintf(fp, "         Latitude = %s\n", gps->lat.str);
    fprintf(fp, "        Longitude = %s\n", gps->lon.str);
    fprintf(fp, "      Coordinates = %.4lf, %.4lf\n", gps->lat.deg, gps->lon.deg);
    fprintf(fp, "Time of Last 1PPS = %s\n", utilLttostr(gps->last_good + QDP_EPOCH_TO_1970_EPOCH, 0, tmpbuf));
    fprintf(fp, "  Checksum Errors = %d\n", (int) gps->check_err);

    fprintf(fp, "\n");
}

static void PrintPWR(FILE *fp, QDP_STAT_PWR *pwr)
{
    fprintf(fp, "Power Supply Status\n");
    fprintf(fp, "-------------------\n");
    fprintf(fp, "      Charging Phase = "); BinUINT16(fp, pwr->phase);
    fprintf(fp, " Battery Temperature = %hu deg C\n", pwr->battemp);
    fprintf(fp, "    Battery Capacity = %hu%%\n", pwr->capacity);
    fprintf(fp, "  Depth of Discharge = %hu%%\n", pwr->depth);
    fprintf(fp, "     Battery Voltage = %.3f\n", (float) pwr->batvolt / 1000.0);
    fprintf(fp, "Supply Input Voltage = %.3f\n", (float) pwr->inpvolt / 1000.0);
    fprintf(fp, "     Battery Current = %hu ma\n", pwr->batcur);
    fprintf(fp, " Absorption Setpoint = %hu\n", pwr->absorption);
    fprintf(fp, "      Float Setpoint = %hu\n", pwr->floatsp);
    fprintf(fp, "       Alerts Bitmap = "); BinUINT8(fp, pwr->alerts);
    fprintf(fp, "        Loads Bitmap = "); BinUINT8(fp, pwr->loads_off);

    fprintf(fp, "\n");
}

static void PrintBOOM(FILE *fp, QDP_STAT_BOOM *boom)
{
int i;

    fprintf(fp, "Miscellaneous Analog Status\n");
    fprintf(fp, "---------------------------\n");
    fprintf(fp, "           Boom Positions ="); for (i = 0; i < QDP_NCHAN; i++) fprintf(fp, " %4hd", boom->pos[i]);
    fprintf(fp, "\n");
    fprintf(fp, "   Analog Positive Supply = %.2fV\n", (float) boom->amb_pos / 100.0);
    if (boom->amb_neg) fprintf(fp, "   Analog Negative Supply = %.1fV\n", (float) boom->amb_neg / 100.0);
    fprintf(fp, "       Q330 Input Voltage = %.2fV\n", (float) boom->supply * 150.0 / 1000.0);
    fprintf(fp, "       System Temperature = %huC\n", boom->sys_temp);
    fprintf(fp, "             Main Current = %huma\n", boom->main_cur);
    fprintf(fp, "      GPS Antenna Current = %huma\n", boom->ant_cur);
    fprintf(fp, "Seismometer 1 Temperature = ");
    if (boom->seis1_temp != 666) fprintf(fp, "%hu deg C\n", boom->seis1_temp); else fprintf(fp, "n/a\n");
    fprintf(fp, "Seismometer 2 Temperature = ");
    if (boom->seis2_temp != 666) fprintf(fp, "%hu deg C\n", boom->seis2_temp); else fprintf(fp, "n/a\n");
    fprintf(fp, " Slave Processor Timeouts = %u\n", boom->cal_timeouts);

    fprintf(fp, "\n");
}

static void PrintTHR(FILE *fp, QDP_STAT_THR *stat)
{
UINT16 i;

    fprintf(fp, "Thread Status\n");
    fprintf(fp, "-------------\n");

    fprintf(fp, "\n");
}

static void PrintPLL(FILE *fp, QDP_STAT_PLL *pll)
{
    fprintf(fp, "PLL Status\n");
    fprintf(fp, "----------\n");
    fprintf(fp, "     Initial VCO = %.2f\n", pll->vco.initial);
    fprintf(fp, "         RMS VCO = %.2f\n", pll->vco.rms);
    fprintf(fp, "        Best VCO = %.2f\n", pll->vco.best);
    fprintf(fp, "      Time error = %.3f\n", pll->time_error);
    fprintf(fp, "Ticks Track Lock = %u\n", pll->ticks_track_lock);
    fprintf(fp, "              Km = %u\n", pll->ticks_track_lock);
    fprintf(fp, "           State = %s (0x%04x)\n", qdpPLLStateString(pll->state), pll->state);

    fprintf(fp, "\n");
}

static void PrintGSV(FILE *fp, QDP_STAT_GSV *gsv)
{
UINT16 i;

    fprintf(fp, "GPS Satellites\n");
    fprintf(fp, "--------------\n");
    fprintf(fp, "    Number Elevation Azimuth SNR\n");
    for (i = 0; i < gsv->nentry; i++) fprintf(fp, "%2d: %6hu %9hu %7hu %3hu\n",
        i+1,
        gsv->satl[i].num,
        gsv->satl[i].elevation,
        gsv->satl[i].azimuth,
        gsv->satl[i].snr
    );

    fprintf(fp, "\n");
}

static char *DotDecimal(UINT32 ip, char *buf)
{
    buf[0] = 0;
    sprintf(buf + strlen(buf), "%03d", (ip & 0xFF000000) >> 24);
    sprintf(buf + strlen(buf), ".");
    sprintf(buf + strlen(buf), "%03d", (ip & 0x00FF0000) >> 16);
    sprintf(buf + strlen(buf), ".");
    sprintf(buf + strlen(buf), "%03d", (ip & 0x0000FF00) >>  8);
    sprintf(buf + strlen(buf), ".");
    sprintf(buf + strlen(buf), "%03d", (ip & 0x000000FF) >>  0);

    return buf;
}

static char *MacString(UINT8 *mac, char *buf)
{
int i;

    buf[0] = 0;
    for (i = 0; i < 6; i++) sprintf(buf + strlen(buf), "%02x:", mac[i]);
    buf[strlen(buf)-1] = 0; /* truncate trailing colon */

    return buf;
}

static void PrintARP(FILE *fp, QDP_STAT_ARP *arp)
{
UINT16 i;
char buf1[32], buf2[32];

    fprintf(fp, "ARP Table\n");
    fprintf(fp, "---------\n");
    fprintf(fp, "      IP Address       MAC Address    Timeout\n");
    for (i = 0; i < arp->nentry; i++) fprintf(fp, "%2d: %s %s %7hu\n",
        i+1,
        DotDecimal(arp->addr[i].ip, buf1),
        MacString(arp->addr[i].mac, buf2),
        arp->addr[i].timeout
    );

    fprintf(fp, "\n");
}

static void PrintDPORT(FILE *fp,  QDP_STAT_DPORT *dport)
{
char tmpbuf[80];

    fprintf(fp, "Data Port %s Status\n", qdpDataPortString(dport->log_num));
    fprintf(fp, "------------------\n");
    fprintf(fp, "  Data Packets Sent = %u\n", dport->sent);
    fprintf(fp, "    Packets Re-Sent = %u\n", dport->resends);
    fprintf(fp, "    Percent Re-Sent = %.2f%%\n", dport->percent);
    fprintf(fp, "  Fill Packets Sent = %u\n", dport->fill);
    fprintf(fp, "    Sequence Errors = %u\n", dport->seq);
    fprintf(fp, "            Backlog = %u bytes\n", dport->pack_used);
    fprintf(fp, "            Timeout = %u msec\n", dport->retran * 100);
    fprintf(fp, "          Interface = %s\n", qdpPhysicalPortString(dport->phy_num));
    fprintf(fp, "Time Since Last Ack = %u sec\n", dport->last_ack);
    fprintf(fp, "              Flags = "); BinUINT16(fp, dport->flags);

    fprintf(fp, "\n");
}

static void PrintSPORT(FILE *fp, QDP_STAT_SPORT *sport, int index)
{
    fprintf(fp, "Serial Port %d Status\n", index + 1);
    fprintf(fp, "--------------------\n");
    fprintf(fp, "Receive Checksum Errors = %u\n", sport->check);
    fprintf(fp, "             I/O Errors = %u\n", sport->ioerrs);
    fprintf(fp, "              Interface = %s\n", qdpPhysicalPortString(sport->phy_num));
    fprintf(fp, "Dest. Unreachable Count = %u\n", sport->unreach);
    fprintf(fp, "      Quench ICMP Count = %u\n", sport->quench);
    fprintf(fp, "Echo Request ICMP Count = %u\n", sport->echo);
    fprintf(fp, "         Redirect Count = %u\n", sport->redirect);
    fprintf(fp, "         Overrun Errors = %u\n", sport->over);
    fprintf(fp, "         Framing Errors = %u\n", sport->frame);

    fprintf(fp, "\n");
}

static void PrintETH(FILE *fp, QDP_STAT_ETH *eth)
{
    fprintf(fp, "Ethernet Status\n");
    fprintf(fp, "---------------\n");
    fprintf(fp, "Receive Checksum Errors = %u\n", eth->check);
    fprintf(fp, "             I/O Errors = %u\n", eth->ioerrs);
    fprintf(fp, "              Interface = %s\n", qdpPhysicalPortString(eth->phy_num));
    fprintf(fp, "Dest. Unreachable Count = %u\n", eth->unreach);
    fprintf(fp, "      Quench ICMP Count = %u\n", eth->quench);
    fprintf(fp, "Echo Request ICMP Count = %u\n", eth->echo);
    fprintf(fp, "         Redirect Count = %u\n", eth->redirect);
    fprintf(fp, "      Total Runt Frames = %u\n", eth->runt);
    fprintf(fp, "             CRC errors = %u\n", eth->crc_err);
    fprintf(fp, "       Broadcast Frames = %u\n", eth->bcast);
    fprintf(fp, "         Unicast Frames = %u\n", eth->ucast);
    fprintf(fp, "            Good Frames = %u\n", eth->good);
    fprintf(fp, "          Jabber Errors = %u\n", eth->jabber);
    fprintf(fp, "          Out of Window = %u\n", eth->outwin);
    fprintf(fp, "            Tranmsit OK = %u\n", eth->txok);
    fprintf(fp, "         Packets Missed = %u\n", eth->miss);
    fprintf(fp, "             Collisions = %u\n", eth->collide);
    fprintf(fp, "            Link Status = "); BinUINT16(fp, eth->linkstat);

    fprintf(fp, "\n");
}

static void PrintBALER(FILE *fp, QDP_STAT_BALER *baler)
{
char tmpbuf[80];

    fprintf(fp, "          Last On = %s\n", utilLttostr(baler->last_on + QDP_EPOCH_TO_1970_EPOCH, 0, tmpbuf));
    fprintf(fp, "    No. of Cycles = %u\n", baler->powerups);
    fprintf(fp, "           Status = "); BinUINT16(fp, baler->baler_status);
    fprintf(fp, "          Up Time = %hu minutes\n", baler->baler_time);
}

static void PrintAllBALERs(FILE *fp, QDP_TYPE_C1_STAT *stat)
{
    fprintf(fp, "BALER status\n");
    fprintf(fp, "------------\n");
    fprintf(fp, "status = "); BinUINT16(fp, stat->baler.special.baler_status);

    if (stat->baler.special.baler_status & 0x0001) {
        fprintf(fp, "\n");
        fprintf(fp, "    Serial 1 Baler\n");
        fprintf(fp, "    --------------\n");
        PrintBALER(fp, &stat->baler.serial[0]);
    }

    if (stat->baler.special.baler_status & 0x0002) {
        fprintf(fp, "\n");
        fprintf(fp, "    Serial 2 Baler\n");
        fprintf(fp, "    --------------\n");
        PrintBALER(fp, &stat->baler.serial[1]);
    }

    if (stat->baler.special.baler_status & 0x0004) {
        fprintf(fp, "\n");
        fprintf(fp, "    Ethernet Baler\n");
        fprintf(fp, "    --------------\n");
        PrintBALER(fp, &stat->baler.ethernet);
    }

    fprintf(fp, "\n");

}

static void PrintDYN(FILE *fp, QDP_STAT_DYN *dyn)
{
char buf[32];

    fprintf(fp, "Dynamic IP addresses\n");
    fprintf(fp, "--------------------\n");
    fprintf(fp, "Serial 1 IP address = %s\n", DotDecimal(dyn->serial[0], buf));
    fprintf(fp, "Serial 2 IP address = %s\n", DotDecimal(dyn->serial[1], buf));
    fprintf(fp, "Ethernet IP address = %s\n", DotDecimal(dyn->ethernet, buf));

    fprintf(fp, "\n");
}

static void PrintAUX(FILE *fp, QDP_STAT_AUX *aux)
{
    ; // UNSUPPORTED
}

static void PrintSSParo(FILE *fp, QDP_STAT_PARO *paro)
{
    fprintf(fp, "Paroscientific Barometer Status\n");
    fprintf(fp, "-------------------------------\n");
    fprintf(fp, "             port = %s\n", qdpSSPortString(paro->port));
    fprintf(fp, "  sample interval = %hd seconds\n", paro->sint);
    if (paro->units.valid) {
          fprintf(fp, "            units = %d (%s)\n", paro->units.value, qdpSSUnitsString(paro->units.value));
    } else {
          fprintf(fp, "            units = UNREPORTED\n");
    }
    if (paro->inttime.valid) {
        fprintf(fp, " integration time = %hd ms\n", paro->inttime.value);
    } else {
        fprintf(fp, " integration time = UNREPORTED\n");
    }
    if (paro->frac.valid) {
        fprintf(fp, "fractional digits = %hd\n", paro->frac.value);
    } else {
        fprintf(fp, "fractional digits = UNREPORTED\n");
    }
    if (paro->pressure.valid   ) {
        fprintf(fp, "         pressure = %d\n", paro->pressure.value);
    } else {
        fprintf(fp, "         pressure = UNREPORTED\n");
    }
    if (paro->temperature.valid) {
        fprintf(fp, "      temperature = %d\n", paro->temperature.value);
    } else {
        fprintf(fp, "      temperature = UNREPORTED\n");
    }

    fprintf(fp, "\n");
}

static void PrintSSMet3(FILE *fp, QDP_STAT_MET3 *met3)
{
    fprintf(fp, "Paroscientfic Met3 Status\n");
    fprintf(fp, "-------------------------\n");
    fprintf(fp, "UNIMPLEMENTED (sorry)\n");
}

static void PrintSSUnknown(FILE *fp, UINT16 type)
{
    fprintf(fp, "<<<< UNSUPPORTED SERIAL SENSOR TYPE %hd FOUND >>>>\n", type);
}

static void PrintSS(FILE *fp, QDP_STAT_SS *ss)
{
int i, report, undefined;

    for (i = 0, undefined = 0, report = 0; i < QDP_NSERIAL; i++) {
        switch (ss->type[i]) {
          case QDP_STAT_SS_TYPE_PARO:
            PrintSSParo(fp, &ss->paro[i]);
            ++report;
            break;
          case QDP_STAT_SS_TYPE_MET3:
            PrintSSMet3(fp, &ss->met3[i]);
            ++report;
            break;
          default:
            PrintSSUnknown(fp, ss->type[i]);
            ++undefined;
        }
    }

    if (!report) fprintf(fp, "No supported serial sensors found\n");
}

static void PrintEP(FILE *fp, QDP_STAT_EP *ep, int index)
{
int i;
char buf[1024];

    if (ep->version == 0) return;

    fprintf(fp, "Environmental Processor Status (serial %d)\n", i+1);
    fprintf(fp, "-----------------------------------------\n");
    fprintf(fp, "    MEMS temperature = %.1f C\n", (REAL32) ep->temperature / 10.0);
    fprintf(fp, "            humidity = %d%%\n", ep->humidity);
    fprintf(fp, "       MEMS pressure = %.3f mbar\n", (REAL32) ep->pressure / 1000.0);
    for (i = 0; i < ep->num_analog; i++) fprintf(fp, "      analog input %d = %d counts\n", i+1, ep->analog[i]);
    fprintf(fp, "       input voltage = %.1f VDC\n", (REAL32) ep->input_voltage / 10.0);
    fprintf(fp, "     time since boot = %d sec\n", ep->sec_since_boot);
    fprintf(fp, "  time since re-sync = %d sec\n", ep->sec_since_resync);
    fprintf(fp, "       re-sync count = %d\n", ep->num_resync);
    fprintf(fp, "           PLL state = %s\n", qdpPLLStateString(ep->pll_state));
    fprintf(fp, "         initial VCO = %.2f\n", ep->initial_vco);
    fprintf(fp, "          time error = %.6f sec\n", ep->time_error);
    fprintf(fp, "            best VCO = %.2f\n", ep->best_vco);
    fprintf(fp, "  ticks since update = %d\n", ep->ticks);
    fprintf(fp, "         VCO control = %d\n", ep->km);
    fprintf(fp, "comm errors (tx, rx) = %d, %d\n", ep->num_q330_comm_errs, ep->num_ep_comm_errs);
    fprintf(fp, "               flags = 0x%04x\n", ep->flags);
    for (i = 0; i < ep->num_active_sdi; i++) {
        fprintf(fp, "        SDI device %d = %d, %s, %s, %s, %s\n",
            i+1, ep->sdi[i].address, ep->sdi[i].model, ep->sdi[i].version, ep->sdi[i].serialno, qdpSdiPhaseString(ep->sdi[i].phase)
        );
    }
    fprintf(fp, "         Base serial = %016llX\n", ep->serialno);
    fprintf(fp, "          ADC serial = %016llX\n", ep->adc.serialno);
}

void qdpPrint_C1_STAT(FILE *fp, QDP_TYPE_C1_STAT *stat)
{
int i;
char buf[32];

    if (stat->alert.tokens) fprintf(fp, "*** DP tokens changed\n");
    if (stat->alert.port) fprintf(fp, "*** Logical port programming changed\n");
    if (stat->alert.usrmsg.ip != 0) {
        fprintf(fp, "*** User message from %s: '%s'\n", DotDecimal(stat->alert.usrmsg.ip, buf), stat->alert.usrmsg.txt);
    }
    if (stat->bitmap & QDP_SRB_GBL)   PrintGBL(fp, &stat->gbl);
    if (stat->bitmap & QDP_SRB_GPS)   PrintGPS(fp, &stat->gps);
    if (stat->bitmap & QDP_SRB_GSV)   PrintGSV(fp, &stat->gsv);
    if (stat->bitmap & QDP_SRB_PWR)   PrintPWR(fp, &stat->pwr);
    if (stat->bitmap & QDP_SRB_BOOM)  PrintBOOM(fp, &stat->boom);
    if (stat->bitmap & QDP_SRB_THR)   PrintTHR(fp, &stat->thr);
    if (stat->bitmap & QDP_SRB_PLL)   PrintPLL(fp, &stat->pll);
    if (stat->bitmap & QDP_SRB_ARP)   PrintARP(fp, &stat->arp);
    if (stat->bitmap & QDP_SRB_DP1)   PrintDPORT(fp, &stat->dport[0]);
    if (stat->bitmap & QDP_SRB_DP2)   PrintDPORT(fp, &stat->dport[1]);
    if (stat->bitmap & QDP_SRB_DP3)   PrintDPORT(fp, &stat->dport[2]);
    if (stat->bitmap & QDP_SRB_DP4)   PrintDPORT(fp, &stat->dport[3]);
    if (stat->bitmap & QDP_SRB_SP1)   PrintSPORT(fp, &stat->sport[0], 0);
    if (stat->bitmap & QDP_SRB_SP2)   PrintSPORT(fp, &stat->sport[1], 1);
    if (stat->bitmap & QDP_SRB_SP3)   PrintSPORT(fp, &stat->sport[2], 2);
    if (stat->bitmap & QDP_SRB_ETH)   PrintETH(fp, &stat->eth);
    if (stat->bitmap & QDP_SRB_BALER) PrintAllBALERs(fp, stat);
    if (stat->bitmap & QDP_SRB_DYN)   PrintDYN(fp, &stat->dyn);
    if (stat->bitmap & QDP_SRB_AUX)   PrintAUX(fp, &stat->aux);
    if (stat->bitmap & QDP_SRB_SS)    PrintSS(fp, &stat->ss);
    if (stat->bitmap & QDP_SRB_EP)    for (i = 0; i < QDP_NEP; i++) PrintEP(fp, &stat->ep[i], i);
}

/* Initialize a QDP_TYPE_C1_STAT structure */

BOOL qdpClear_C1_STAT(QDP_TYPE_C1_STAT *dest)
{
    if (dest == NULL) {
        errno = EINVAL;
        return FALSE;
    }

    memset(dest, 0, sizeof(QDP_TYPE_C1_STAT));
    return TRUE;
}

BOOL qdpInit_C1_STAT(QDP_TYPE_C1_STAT *dest)
{
    return qdpClear_C1_STAT(dest);
}

static int DecodeUMSG(UINT8 *start, QDP_TYPE_C1_STAT *dest)
{
int nchar;
UINT8 *ptr;
#define UMSG_FIXED_LENGTH 80

    ptr = start;

    ptr += utilUnpackUINT32(ptr, &dest->alert.usrmsg.ip);
    nchar = *ptr++;
    ptr += utilUnpackBytes(ptr, (UINT8 *) dest->alert.usrmsg.txt, UMSG_FIXED_LENGTH-1);
    dest->alert.usrmsg.txt[nchar] = 0;

    return (int) (ptr - start);
}

static int DecodeGBL(UINT8 *start, QDP_STAT_GBL *dest)
{
UINT8 *ptr;
QDP_MN232_DATA mn232;
QDP_TOKEN_CLOCK dp_clock = QDP_DEFAULT_TOKEN_CLOCK;

    ptr = start;

    ptr += utilUnpackUINT16(ptr, &dest->aqctrl);
    ptr += utilUnpackUINT16(ptr, &dest->clock_qual);
    ptr += utilUnpackUINT16(ptr, &dest->clock_loss);
    ptr += utilUnpackUINT16(ptr, &dest->current_voltage);

    ptr += utilUnpackUINT32(ptr, &dest->sec_offset);
    ptr += utilUnpackUINT32(ptr, &dest->usec_offset);
    ptr += utilUnpackUINT32(ptr, &dest->total_time);
    ptr += utilUnpackUINT32(ptr, &dest->power_time);
    ptr += utilUnpackUINT32(ptr, &dest->last_resync);
    ptr += utilUnpackUINT32(ptr, &dest->resyncs);

    ptr += utilUnpackUINT16(ptr, &dest->gps_stat);
    ptr += utilUnpackUINT16(ptr, &dest->cal_stat);
    ptr += utilUnpackUINT16(ptr, &dest->sensor_map);
    ptr += utilUnpackUINT16(ptr, &dest->cur_vco);
    ptr += utilUnpackUINT16(ptr, &dest->data_seq);
    ptr += utilUnpackUINT16(ptr, &dest->pll_flag);
    ptr += utilUnpackUINT16(ptr, &dest->stat_inp);
    ptr += utilUnpackUINT16(ptr, &dest->misc_inp);

    ptr += utilUnpackUINT32(ptr, &dest->cur_sequence);

    dest->tstamp = dest->cur_sequence + dest->sec_offset;

    mn232.qual = dest->clock_qual;
    mn232.loss = dest->clock_loss;
    dest->clock_perc = qdpClockQuality(&dp_clock, &mn232);

    return (int) (ptr - start);
}

static int DecodeGPS(UINT8 *start, QDP_STAT_GPS *dest)
{
int nchar;
UINT8 *ptr;

    ptr = start;

    ptr += utilUnpackUINT16(ptr, &dest->gpstime);
    ptr += utilUnpackUINT16(ptr, &dest->gpson);
    ptr += utilUnpackUINT16(ptr, &dest->sat_used);
    ptr += utilUnpackUINT16(ptr, &dest->sat_view);

    nchar = *ptr++; ptr += utilUnpackBytes(ptr, (UINT8 *) dest->time, 9); dest->time[nchar] = 0;
    nchar = *ptr++; ptr += utilUnpackBytes(ptr, (UINT8 *) dest->date, 11); dest->date[nchar] = 0;
    nchar = *ptr++; ptr += utilUnpackBytes(ptr, (UINT8 *) dest->fix, 5); dest->fix[nchar] = 0;
    nchar = *ptr++; ptr += utilUnpackBytes(ptr, (UINT8 *) dest->height, 11); dest->height[nchar] = 0;
    nchar = *ptr++; ptr += utilUnpackBytes(ptr, (UINT8 *) dest->lat.str, 13); dest->lat.str[nchar] = 0;
    nchar = *ptr++; ptr += utilUnpackBytes(ptr, (UINT8 *) dest->lon.str, 13); dest->lon.str[nchar] = 0;

    dest->lat.deg = qdpGpsStringToDecDeg(dest->lat.str);
    dest->lon.deg = qdpGpsStringToDecDeg(dest->lon.str);

    ptr += utilUnpackUINT32(ptr, &dest->last_good);
    ptr += utilUnpackUINT32(ptr, &dest->check_err);

    dest->tstamp = qdpGpsDateTimeToEpoch(dest->date, dest->time);

    return (int) (ptr - start);
}

static int DecodePWR(UINT8 *start, QDP_STAT_PWR *dest)
{
UINT8 *ptr;

    ptr = start;

    ptr += utilUnpackUINT16(ptr, &dest->phase);
    ptr += utilUnpackUINT16(ptr, &dest->battemp);
    ptr += utilUnpackUINT16(ptr, &dest->capacity);
    ptr += utilUnpackUINT16(ptr, &dest->depth);
    ptr += utilUnpackUINT16(ptr, &dest->batvolt);
    ptr += utilUnpackUINT16(ptr, &dest->inpvolt);
    ptr += utilUnpackUINT16(ptr, &dest->batcur);
    ptr += utilUnpackUINT16(ptr, &dest->absorption);
    ptr += utilUnpackUINT16(ptr, &dest->floatsp);

    dest->alerts = *ptr++;
    dest->loads_off = *ptr++;

    return (int) (ptr - start);
}

static int DecodeBOOM(UINT8 *start, QDP_STAT_BOOM *dest)
{
int i;
UINT8 *ptr;

    ptr = start;

    for (i = 0; i < QDP_NCHAN; i++) ptr += utilUnpackINT16(ptr, &dest->pos[i]);

    ptr += utilUnpackUINT16(ptr, &dest->amb_pos);
    ptr += utilUnpackUINT16(ptr, &dest->amb_neg);
    ptr += utilUnpackUINT16(ptr, &dest->supply);

    ptr += utilUnpackINT16(ptr, &dest->sys_temp);

    ptr += utilUnpackUINT16(ptr, &dest->main_cur);
    ptr += utilUnpackUINT16(ptr, &dest->ant_cur);

    ptr += utilUnpackINT16(ptr, &dest->seis1_temp);
    ptr += utilUnpackINT16(ptr, &dest->seis2_temp);

    ptr += utilUnpackUINT32(ptr, &dest->cal_timeouts);

    return (int) (ptr - start);
}

static int DecodeTHR(UINT8 *start, QDP_STAT_THR *dest)
{
UINT8 *ptr;
UINT16 tmp16, i, len;
UINT32 tmp32;

    ptr = start;

    ptr += utilUnpackUINT16(ptr, &dest->nentry);
    ptr += utilUnpackUINT16(ptr, &len);
    ptr += utilUnpackUINT32(ptr, &tmp32);
    ptr += utilUnpackUINT16(ptr, &tmp16);
    dest->stime = (UINT64) ((tmp32 << 16) | tmp16);
    ptr += 2; /* skip over spare */

    if (dest->nentry > QDP_MAX_STAT_TASK_ENTRY) dest->nentry = QDP_MAX_STAT_TASK_ENTRY;
    for (i = 0; i < dest->nentry; i++) {
        ptr += utilUnpackUINT32(ptr, &tmp32);
        ptr += utilUnpackUINT16(ptr, &tmp16);
        dest->task[i].runtime = (UINT64) ((tmp32 << 16) | tmp16);
        dest->task[i].priority = *ptr++;
        dest->task[i].counter = *ptr++;
        ptr += utilUnpackUINT32(ptr, &tmp32);
        ptr += utilUnpackUINT16(ptr, &tmp16);
        dest->task[i].since = (UINT64) ((tmp32 << 16) | tmp16);
        ptr += utilUnpackUINT16(ptr, &dest->task[i].flags);
    }

    return (int) len;
}

static int DecodePLL(UINT8 *start, QDP_STAT_PLL *dest)
{
UINT8 *ptr;

    ptr = start;

    ptr += utilUnpackREAL32(ptr, &dest->vco.initial);
    ptr += utilUnpackREAL32(ptr, &dest->time_error);
    ptr += utilUnpackREAL32(ptr, &dest->vco.rms);
    ptr += utilUnpackREAL32(ptr, &dest->vco.best);
    ptr += 4; /* skip over "spare" */
    ptr += utilUnpackUINT32(ptr, &dest->ticks_track_lock);

    ptr += utilUnpackINT16(ptr, &dest->km);
    ptr += utilUnpackUINT16(ptr, &dest->state);

    return (int) (ptr - start);
}

static int DecodeGSV(UINT8 *start, QDP_STAT_GSV *dest)
{
UINT16 i, len;
UINT8 *ptr;

    ptr = start;

    ptr += utilUnpackUINT16(ptr, &dest->nentry);
    ptr += utilUnpackUINT16(ptr, &len);
    if (dest->nentry > QDP_MAX_STAT_SATL_ENTRY) dest->nentry = QDP_MAX_STAT_SATL_ENTRY;
    for (i = 0; i < dest->nentry; i++) {
        ptr += utilUnpackUINT16(ptr, &dest->satl[i].num);
        ptr += utilUnpackINT16(ptr, &dest->satl[i].elevation);
        ptr += utilUnpackINT16(ptr, &dest->satl[i].azimuth);
        ptr += utilUnpackUINT16(ptr, &dest->satl[i].snr);
    }

    return (int) (ptr - start);
}

static int DecodeARP(UINT8 *start, QDP_STAT_ARP *dest)
{
UINT16 i, j, len;
UINT8 *ptr;

    ptr = start;

    ptr += utilUnpackUINT16(ptr, &dest->nentry);
    ptr += utilUnpackUINT16(ptr, &len);
    if (dest->nentry > QDP_MAX_STAT_ADDR_ENTRY) dest->nentry = QDP_MAX_STAT_ADDR_ENTRY;
    for (i = 0; i < dest->nentry; i++) {
        ptr += utilUnpackUINT32(ptr, &dest->addr[i].ip);
        for (j = 0; j < 6; j++) dest->addr[i].mac[j] = *ptr++;
        ptr += utilUnpackUINT16(ptr, &dest->addr[i].timeout);
    }

    return (int) len;
}

static int DecodeDPORT(UINT8 *start, QDP_STAT_DPORT *dest)
{
UINT8 *ptr;

    ptr = start;
    ptr += utilUnpackUINT32(ptr, &dest->sent);
    ptr += utilUnpackUINT32(ptr, &dest->resends);
    ptr += utilUnpackUINT32(ptr, &dest->fill);
    ptr += utilUnpackUINT32(ptr, &dest->seq);
    ptr += utilUnpackUINT32(ptr, &dest->pack_used);
    ptr += utilUnpackUINT32(ptr, &dest->last_ack);

    ptr += utilUnpackUINT16(ptr, &dest->phy_num);
    ptr += utilUnpackUINT16(ptr, &dest->log_num);
    ptr += utilUnpackUINT16(ptr, &dest->retran);
    ptr += utilUnpackUINT16(ptr, &dest->flags);

    if (dest->sent) {
        dest->percent = ((float) dest->resends / (float) dest->sent) * 100.0;
    } else {
        dest->percent = 0.0;
    }

    return (int) (ptr - start);
}

static int DecodeSPORT(UINT8 *start, QDP_STAT_SPORT *dest)
{
UINT8 *ptr;

    ptr = start;

    ptr += utilUnpackUINT32(ptr, &dest->check);
    ptr += utilUnpackUINT32(ptr, &dest->ioerrs);

    ptr += utilUnpackUINT16(ptr, &dest->phy_num);
    ptr += 2; /* skip over spare */

    ptr += utilUnpackUINT32(ptr, &dest->unreach);
    ptr += utilUnpackUINT32(ptr, &dest->quench);
    ptr += utilUnpackUINT32(ptr, &dest->echo);
    ptr += utilUnpackUINT32(ptr, &dest->redirect);
    ptr += utilUnpackUINT32(ptr, &dest->over);
    ptr += utilUnpackUINT32(ptr, &dest->frame);

    return (int) (ptr - start);
}

static int DecodeETH(UINT8 *start, QDP_STAT_ETH *dest)
{
UINT8 *ptr;

    ptr = start;

    ptr += utilUnpackUINT32(ptr, &dest->check);
    ptr += utilUnpackUINT32(ptr, &dest->ioerrs);

    ptr += utilUnpackUINT16(ptr, &dest->phy_num);
    ptr += 2; /* skip over spare */

    ptr += utilUnpackUINT32(ptr, &dest->unreach);
    ptr += utilUnpackUINT32(ptr, &dest->quench);
    ptr += utilUnpackUINT32(ptr, &dest->echo);
    ptr += utilUnpackUINT32(ptr, &dest->redirect);
    ptr += utilUnpackUINT32(ptr, &dest->runt);
    ptr += utilUnpackUINT32(ptr, &dest->crc_err);
    ptr += utilUnpackUINT32(ptr, &dest->bcast);
    ptr += utilUnpackUINT32(ptr, &dest->ucast);
    ptr += utilUnpackUINT32(ptr, &dest->good);
    ptr += utilUnpackUINT32(ptr, &dest->jabber);
    ptr += utilUnpackUINT32(ptr, &dest->outwin);
    ptr += utilUnpackUINT32(ptr, &dest->txok);
    ptr += utilUnpackUINT32(ptr, &dest->miss);
    ptr += utilUnpackUINT32(ptr, &dest->collide);

    ptr += utilUnpackUINT16(ptr, &dest->linkstat);
    ptr += 2; /* skip over spare */
    ptr += 4; /* skip over spare */

    return (int) (ptr - start);
}

static int DecodeBALER(UINT8 *start, QDP_STAT_BALER *dest)
{
UINT8 *ptr;

    ptr = start;

    ptr += utilUnpackUINT32(ptr, &dest->last_on);
    ptr += utilUnpackUINT32(ptr, &dest->powerups);

    ptr += utilUnpackUINT16(ptr, &dest->baler_status);
    ptr += utilUnpackUINT16(ptr, &dest->baler_time);

    return (int) (ptr - start);
}

static int DecodeDYN(UINT8 *start, QDP_STAT_DYN *dest)
{
UINT8 *ptr;

    ptr = start;

    ptr += utilUnpackUINT32(ptr, &dest->serial[0]);
    ptr += utilUnpackUINT32(ptr, &dest->serial[1]);
    ptr += 4; /* skip over spare */
    ptr += utilUnpackUINT32(ptr, &dest->ethernet);

    return (int) (ptr - start);
}

static int DecodeAUX(UINT8 *start, QDP_STAT_AUX *dest)
{
UINT8 *ptr;
UINT16 blksiz;

    ptr = start;

/* Skip for now */
    utilUnpackUINT16(ptr, &blksiz);
    return blksiz;
}

static int DecodeSS_paro(UINT8 *start, QDP_STAT_PARO *dest, UINT8 *end)
{
UINT8 *ptr;
UINT16 valid;

    dest->units.valid = dest->inttime.valid = dest->frac.valid = FALSE;
    dest->pressure.valid = dest->temperature.valid = FALSE;

    ptr = start;

    ptr += utilUnpackUINT16(ptr, &dest->port);
    ptr += utilUnpackUINT16(ptr, &dest->sint);          if (end <= ptr)  return (int) (ptr - start);
    dest->units.valid = TRUE;
    ptr += utilUnpackUINT16(ptr, &dest->units.value);   if (end <= ptr)  return (int) (ptr - start);
    dest->inttime.valid = TRUE;
    ptr += utilUnpackUINT16(ptr, &dest->inttime.value); if (end <= ptr)  return (int) (ptr - start);
    dest->frac.valid = TRUE;
    ptr += utilUnpackUINT16(ptr, &dest->frac.value); if (end <= ptr)  return (int) (ptr - start);
    ptr += utilUnpackUINT16(ptr, &valid);
    dest->pressure.valid    = (valid & 0x0001) ? TRUE : FALSE;
    dest->temperature.valid = (valid & 0x0002) ? TRUE : FALSE;
    ptr += utilUnpackINT32(ptr, &dest->pressure.value);
    if (dest->temperature.valid) ptr += utilUnpackINT32(ptr, &dest->temperature.value);

    return (int) (end - start);
}

static int DecodeSS(UINT8 *start, QDP_STAT_SS *dest)
{
int i;
UINT8 *ptr, *blkbeg;
UINT16 blksiz, nblk, len;

    for (i = 0; i < QDP_NSERIAL; i++) dest->type[i] = QDP_STAT_SS_TYPE_UNDEFINED;

    ptr = start;

    ptr += utilUnpackUINT16(ptr, &blksiz);
    ptr += utilUnpackUINT16(ptr, &nblk);
    if (nblk > QDP_NSERIAL) nblk = QDP_NSERIAL;
    for (i = 0; i < nblk; i++) {
        blkbeg = ptr;
        ptr += utilUnpackUINT16(ptr, &len);
        ptr += utilUnpackUINT16(ptr, &dest->type[i]);
        switch (dest->type[i]) {
          case QDP_STAT_SS_TYPE_PARO: ptr += DecodeSS_paro(ptr, &dest->paro[i], blkbeg + len); break;
          default: ptr = blkbeg + len;
        }
    }

    return (int) blksiz;
}

static int DecodeEP_sdi(UINT8 *start, QDP_STAT_EP_SDI *dest)
{
int i;
UINT8 *ptr;

    ptr = start;

    dest->address = *ptr++;
    dest->phase = *ptr++;
    dest->driver = *ptr++;
    ++ptr; /* skip over spare */
    for (i = 0; i < QDP_STAT_EP_SDI_MODEL_STRING_LEN; i++) dest->model[i] = *ptr++;
    for (i = 0; i < QDP_STAT_EP_SDI_SERIALNO_STRING_LEN; i++) dest->serialno[i] = *ptr++;
    ++ptr; /* skip over spare */
    for (i = 0; i < QDP_STAT_EP_SDI_VERSION_STRING_LEN; i++) dest->version[i] = *ptr++;
    ++ptr; /* skip over spare */

    /* make sure all the strings are null terminated */

    dest->model[QDP_STAT_EP_SDI_MODEL_STRING_LEN] = 0;
    dest->serialno[QDP_STAT_EP_SDI_SERIALNO_STRING_LEN] = 0;
    dest->version[QDP_STAT_EP_SDI_VERSION_STRING_LEN] = 0;

    return (int) (ptr - start);
}

static int DecodeEP_adc(UINT8 *start, QDP_STAT_EP_ADC *dest)
{
int i;
UINT8 *ptr;

    ptr = start;

    ptr += utilUnpackUINT64(ptr, &dest->serialno);
    dest->model = *ptr++;
    dest->revision = *ptr++;
    ptr += 20; /* skip over spare */

    return (int) (ptr - start);
}

static int DecodeEP(UINT8 *start, QDP_STAT_EP *dest)
{
int i;
UINT8 *ptr;

    ptr = start;

    ptr += utilUnpackREAL32(ptr, &dest->initial_vco);
    ptr += utilUnpackREAL32(ptr, &dest->time_error);
    ptr += utilUnpackREAL32(ptr, &dest->best_vco);
    ptr += utilUnpackUINT32(ptr, &dest->ticks);
    ptr += utilUnpackUINT32(ptr, &dest->km);
    ptr += utilUnpackUINT16(ptr, &dest->pll_state);
    ptr += 2; /* skip over 16-bit spare */
    ptr += utilUnpackUINT64(ptr, &dest->serialno);
    ptr += utilUnpackUINT32(ptr, &dest->processor_id);
    ptr += utilUnpackUINT32(ptr, &dest->sec_since_boot);
    ptr += utilUnpackUINT32(ptr, &dest->sec_since_resync);
    ptr += utilUnpackUINT32(ptr, &dest->num_resync);
    ptr += utilUnpackUINT32(ptr, &dest->num_q330_comm_errs);
    ptr += utilUnpackUINT32(ptr, &dest->num_ep_comm_errs);
    ptr += 2; /* skip over 16-bit spare */
    ptr += utilUnpackUINT16(ptr, &dest->num_active_sdi);
    ptr += utilUnpackUINT16(ptr, &dest->version);
    ptr += utilUnpackUINT16(ptr, &dest->flags);
    ptr += utilUnpackUINT16(ptr, &dest->num_analog);
    dest->model = *ptr++;
    dest->revision = *ptr++;
    ptr += utilUnpackUINT32(ptr, &dest->gains);
    ptr += utilUnpackINT16(ptr, &dest->input_voltage);
    ptr += utilUnpackINT16(ptr, &dest->humidity);
    ptr += utilUnpackINT32(ptr, &dest->pressure);
    ptr += utilUnpackINT32(ptr, &dest->temperature);
    for (i = 0; i < QDP_STAT_MAX_ADC; i++) ptr += utilUnpackINT32(ptr, &dest->analog[i]);
    ptr += 4; /* skip over 32-bit spare */
    for (i = 0; i < QDP_STAT_MAX_SDI; i++) ptr += DecodeEP_sdi(ptr, &dest->sdi[i]);
    ptr += DecodeEP_adc(ptr, &dest->adc);

    return (int) (ptr - start);
}

/* Decode a QDP C1_STAT payload into a QDP_TYPE_C1_STAT structure */

BOOL qdpDecode_C1_STAT(UINT8 *start, QDP_TYPE_C1_STAT *dest)
{
UINT8 *ptr;

    if (start == NULL || dest == NULL) {
        errno = EINVAL;
        return FALSE;
    }

    if (!qdpClear_C1_STAT(dest)) return FALSE;

    ptr = start;
    ptr += utilUnpackUINT32(ptr, &dest->bitmap);
    if (dest->bitmap & QDP_SRB_TOKEN) dest->alert.tokens = TRUE;
    if (dest->bitmap & QDP_SRB_LCHG)  dest->alert.port = TRUE;
    if (dest->bitmap & QDP_SRB_UMSG)  ptr += DecodeUMSG(ptr, dest);
    if (dest->bitmap & QDP_SRB_GBL)   ptr += DecodeGBL(ptr,  &dest->gbl);
    if (dest->bitmap & QDP_SRB_GPS)   ptr += DecodeGPS(ptr,  &dest->gps);
    if (dest->bitmap & QDP_SRB_PWR)   ptr += DecodePWR(ptr,  &dest->pwr);
    if (dest->bitmap & QDP_SRB_BOOM)  ptr += DecodeBOOM(ptr, &dest->boom);
    if (dest->bitmap & QDP_SRB_THR)   ptr += DecodeTHR(ptr,  &dest->thr);
    if (dest->bitmap & QDP_SRB_PLL)   ptr += DecodePLL(ptr,  &dest->pll);
    if (dest->bitmap & QDP_SRB_GSV)   ptr += DecodeGSV(ptr,  &dest->gsv);
    if (dest->bitmap & QDP_SRB_ARP)   ptr += DecodeARP(ptr,  &dest->arp);
    if (dest->bitmap & QDP_SRB_DP1)   ptr += DecodeDPORT(ptr, &dest->dport[0]);
    if (dest->bitmap & QDP_SRB_DP2)   ptr += DecodeDPORT(ptr, &dest->dport[1]);
    if (dest->bitmap & QDP_SRB_DP3)   ptr += DecodeDPORT(ptr, &dest->dport[2]);
    if (dest->bitmap & QDP_SRB_DP4)   ptr += DecodeDPORT(ptr, &dest->dport[3]);
    if (dest->bitmap & QDP_SRB_SP1)   ptr += DecodeSPORT(ptr, &dest->sport[0]);
    if (dest->bitmap & QDP_SRB_SP2)   ptr += DecodeSPORT(ptr, &dest->sport[1]);
    if (dest->bitmap & QDP_SRB_SP3)   ptr += DecodeSPORT(ptr, &dest->sport[2]);
    if (dest->bitmap & QDP_SRB_ETH)   ptr += DecodeETH(ptr, &dest->eth);
    if (dest->bitmap & QDP_SRB_BALER) {
        ptr += DecodeBALER(ptr, &dest->baler.serial[0]);
        ptr += DecodeBALER(ptr, &dest->baler.serial[1]);
        ptr += DecodeBALER(ptr, &dest->baler.special);
        ptr += DecodeBALER(ptr, &dest->baler.ethernet);
    }
    if (dest->bitmap & QDP_SRB_DYN)   ptr += DecodeDYN(ptr, &dest->dyn);
    if (dest->bitmap & QDP_SRB_AUX)   ptr += DecodeAUX(ptr, &dest->aux);
    if (dest->bitmap & QDP_SRB_SS)    ptr += DecodeSS(ptr, &dest->ss);
    if (dest->bitmap & QDP_SRB_EP) {
       ptr += DecodeEP(ptr, &dest->ep[0]);
       ptr += DecodeEP(ptr, &dest->ep[1]);
    }

    return TRUE;
}

BOOL qdpCalibrationInProgress(QDP_TYPE_C1_STAT *stat)
{
    if (stat->gbl.cal_stat & QDP_CAL_ENABLE) return TRUE;
    if (stat->gbl.cal_stat & QDP_CAL_ON    ) return TRUE;

    return FALSE;
}

UINT32 qdpGpsDateTimeToEpoch(char *date, char *time)
{
UINT32 result;
int da, mo, year, hr, mn, sc, day;

    if (strlen(date) == 0 || strlen(time) == 0) return 0;
    sscanf(date, "%02d/%02d/%04d", &da, &mo, &year);
    if (da < 1 || da > 31 || mo < 1 || mo > 12 || year < 2000 || year > 3000) return 0;
    sscanf(time, "%02d:%02d:%02d", &hr, &mn, &sc);
    if (hr < 0 || hr > 23 || mn < 0 || mn > 59 || sc < 0 || sc > 60) return 0;
    day = utilYmdtojd(year, mo, da);
    result = (UINT32) utilYdhmsmtod(year, day, hr, mn, sc, 0) - QDP_EPOCH_TO_1970_EPOCH;

    return result;
}

REAL64 qdpGpsStringToDecDeg(char *string)
{
char direction, *copy;
INT32 deg;
REAL64 factor, value, min, failure = -1.0;

    if (string == NULL) {
        errno = EINVAL;
        return failure;
    }

/* string MUST end in N/S/E/W if valid */

    switch (direction = string[strlen(string)-1]) {
      case 'n':
      case 'N':
      case 'e':
      case 'E':
        factor = 1.0;
        break;
      case 's':
      case 'S':
      case 'w':
      case 'W':
        factor = -1.0;
        break;
      default:
        return failure;
    }

/* clobber the trailing direction character to avoid E confusion between Exponent and East */

    if ((copy = strdup(string)) == NULL) return failure;
    copy[strlen(copy)-1] = 0;

/* convert to double and dispose of the copy */

    errno = 0;
    value = strtod(copy, NULL);
    if (errno != 0) {
        free(copy);
        return failure;
    }
    free(copy);

/* value from string is deg*100 + decimal minutes... convert to decimal degrees */

    deg = (INT32) (value / 100.0);
    min = value - ((REAL64) deg * 100.0);
    value = factor * (deg + (min / 60.0));

    return value;
}

/*-----------------------------------------------------------------------+
 |                                                                       |
 | Copyright (C) 2007 Regents of the University of California            |
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
 * $Log: status.c,v $
 * Revision 1.20  2017/09/06 21:58:19  dechavez
 * introduced qdpGpsStringToDecDeg(), include decimal degree coordinats in PrintGPS()
 * output, compute decimal degrees from GPS degmin strings, save in QDP_GPS_COORD
 *
 * Revision 1.19  2016/01/29 00:21:19  dechavez
 * changed bars to mbar in PrintEP()
 *
 * Revision 1.18  2016/01/28 22:08:11  dechavez
 * Changed all QDP_STAT_SDI_x contants to QDP_STAT_EP_SDI_x equivalents.
 * Changed all QDP_STAT_ADC_x contants to QDP_STAT_EP_ADC_x equivalents.
 * Reworked DecodeARP(), DecodeGSV(), and DecodeTHR() to use static arrays instead of malloc,
 * and massively simplified qdpClear_C1_STAT() and qdpInit_C1_STAT() now that pointers are gone.
 * Added stubs for QDP_STAT_MET3 support.
 *
 * Revision 1.17  2016/01/19 22:54:38  dechavez
 * environmental processor support
 *
 * Revision 1.16  2015/12/04 23:15:12  dechavez
 * casts, format fixes and the like, all to calm OS X compiles
 *
 * Revision 1.15  2013/02/05 23:39:10  dechavez
 * added QDP_STAT_DPORT percent support to DecodeDPORT() and PrintDPORT()
 *
 * Revision 1.14  2012/06/24 18:08:37  dechavez
 * PrintDPORT() uses qdpDataPortString() for data port identifier
 *
 * Revision 1.13  2012/01/17 18:46:53  dechavez
 * added prefixes to make all "Input Voltage" strings unique, compute and print clock quality percentage instead of bitmap
 *
 * Revision 1.12  2010/11/02 22:23:27  dechavez
 * fixed unpack error in DecodeUMSG() that was causing qdpDecode_C1_STAT() to seg fault when present
 *
 * Revision 1.11  2009/10/20 23:31:52  dechavez
 * fixed bug initializing serial sensor status
 *
 * Revision 1.10  2009/10/20 23:03:37  dechavez
 * implement serial sensor status support
 *
 * Revision 1.9  2009/09/04 18:22:45  dechavez
 * fixed bug in qdpGpsDateTimeToEpoch() sanity checks
 *
 * Revision 1.8  2009/07/13 22:38:42  dechavez
 * sanity checks on qdpGpsDateTimeToEpoch() input
 *
 * Revision 1.7  2009/07/09 18:24:59  dechavez
 * added support for new tstamp fields in global and gps status
 *
 * Revision 1.6  2009/07/02 21:25:54  dechavez
 * added qdpCalibrationInProgress()
 *
 * Revision 1.5  2009/02/03 23:05:10  dechavez
 * fixed up QDP_BOOM report
 *
 * Revision 1.4  2008/10/02 22:45:44  dechavez
 * removed unused dlen parameter from qdpDecode_C1_STAT, moved GSV report to under GPS in qdpPrint_C1_STAT
 *
 * Revision 1.3  2007/12/21 18:57:02  dechavez
 * fixed off by one in serial port status header
 *
 * Revision 1.2  2007/12/20 23:06:59  dechavez
 * fixed numerous typos
 *
 * Revision 1.1  2007/12/20 22:47:59  dechavez
 * created
 *
 */
