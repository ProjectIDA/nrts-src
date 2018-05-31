#pragma ident "$Id: print.c,v 1.9 2016/01/23 00:09:34 dechavez Exp $"
/*======================================================================
 *
 *  Print configuration in XML
 *
 *  The fsdetect field, marked GUESS, was found in a third party XML file.
 *  I have been unable to find any vendor documentation as to what it
 *  means and where it is found but am GUESSing that it is a newly defined
 *  bit in the aux/status/web flags bitmap (confusingly named samp_rates
 *  to match the Quanterra structures).
 *
 *====================================================================*/
#include "qdp/xml.h"

void qdpXmlPrintWriter(MYXML_HANDLE *xp, int indent, int subdent, QDP_TYPE_XML_WRITER *writer)
{
    XmlIndent(xp, indent); XmlPrint(xp, "<writer>\n");
        XmlPrintString(xp, indent+subdent, "proto_ver", writer->proto_ver);
        XmlPrintString(xp, indent+subdent, "name", writer->name);
        XmlPrintString(xp, indent+subdent, "soft_ver", writer->soft_ver);
        XmlPrintString(xp, indent+subdent, "created", writer->created);
        XmlPrintString(xp, indent+subdent, "updated", writer->updated);
    XmlIndent(xp, indent); XmlPrint(xp, "</writer>\n");
}

static void PrintGlobScaling(MYXML_HANDLE *xp, int indent, int subdent, char *tag, UINT16 *array)
{
int i, dent;

    dent = indent + subdent;

    XmlIndent(xp, indent); XmlPrint(xp, "<%s>\n", tag);
        if (array[0] != 0) XmlPrintDouble(xp, dent, "%.1lf", "Hz1", (double) array[0] / 1024.0);
        if (array[1] != 0) XmlPrintDouble(xp, dent, "%.1lf", "Hz10", (double) array[1] / 1024.0);
        if (array[2] != 0) XmlPrintDouble(xp, dent, "%.1lf", "Hz20", (double) array[2] / 1024.0);
        if (array[3] != 0) XmlPrintDouble(xp, dent, "%.1lf", "Hz40", (double) array[3] / 1024.0);
        if (array[4] != 0) XmlPrintDouble(xp, dent, "%.1lf", "Hz50", (double) array[4] / 1024.0);
        if (array[5] != 0) XmlPrintDouble(xp, dent, "%.1lf", "Hz100", (double) array[4] / 1024.0);
        if (array[6] != 0) XmlPrintDouble(xp, dent, "%.1lf", "Hz200", (double) array[6] / 1024.0);
    XmlIndent(xp, indent); XmlPrint(xp, "</%s>\n", tag);

}

void qdpXmlPrint_C1_GLOB(MYXML_HANDLE *xp, int indent, int subdent, QDP_TYPE_C1_GLOB *glob, char *basetag)
{
int dent;

    dent = indent + subdent;

    XmlIndent(xp, indent); XmlStartCRC(xp); XmlPrint(xp, "<global>\n");
    if (basetag != NULL) XmlPrintString(xp, dent, "basetag", basetag);
    XmlPrintInt(xp, dent, "clock_to",      glob->clock_to);
    XmlPrintInt(xp, dent, "initial_vco",   glob->initial_vco);
    XmlPrintInt(xp, dent, "gps_backup",    glob->gps_backup);
                                                                              // 11
                                                                              // 1098 76543210
    XmlPrintInt(xp, dent, "aux_rate",      (glob->samp_rates & 0x0003) >> 0); // 0000 00000011
    XmlPrintInt(xp, dent, "stat_rate",     (glob->samp_rates & 0x0300) >> 8); // 0011 00000000
    XmlPrintInt(xp, dent, "open_balepwr",  (glob->samp_rates & 0x0004) >> 2); // 0000 00000100
    XmlPrintInt(xp, dent, "open_balelink", (glob->samp_rates & 0x0008) >> 3); // 0000 00001000
    XmlPrintInt(xp, dent, "show_sn",       (glob->samp_rates & 0x0010) >> 4); // 0000 00010000
    XmlPrintInt(xp, dent, "fsdetect",      (glob->samp_rates & 0x0020) >> 5); // 0000 00100000 /* GUESS */

                                                                              // 11
                                                                              // 1098 76543210
    XmlPrintInt(xp, dent, "gainmap1",      (glob->gain_map & 0x0003) >>  0);  // 0000 00000011
    XmlPrintInt(xp, dent, "gainmap2",      (glob->gain_map & 0x000C) >>  2);  // 0000 00001100
    XmlPrintInt(xp, dent, "gainmap3",      (glob->gain_map & 0x0030) >>  4);  // 0000 00110000
    XmlPrintInt(xp, dent, "gainmap4",      (glob->gain_map & 0x00C0) >>  6);  // 0000 11000000
    XmlPrintInt(xp, dent, "gainmap5",      (glob->gain_map & 0x0300) >>  8);  // 0011 00000000
    XmlPrintInt(xp, dent, "gainmap6",      (glob->gain_map & 0x0C00) >> 10);  // 1100 00000000

                                                                              // 11
                                                                              // 1098 76543210
    XmlPrintInt(xp, dent, "filtmap1",      (glob->filter_map & 0x0003) >> 0); // 0000 00000011
    XmlPrintInt(xp, dent, "filtmap2",      (glob->filter_map & 0x000C) >> 2); // 0000 00001100

                                                                              // 11
                                                                              // 1098 76543210
    XmlPrintInt(xp, dent, "input1",        (glob->input_map & 0x0001) >>  0); // 0000 00000001
    XmlPrintInt(xp, dent, "input2",        (glob->input_map & 0x0002) >>  1); // 0000 00000010
    XmlPrintInt(xp, dent, "input3",        (glob->input_map & 0x0004) >>  2); // 0000 00000100
    XmlPrintInt(xp, dent, "input4",        (glob->input_map & 0x0008) >>  3); // 0000 00001000
    XmlPrintInt(xp, dent, "input5",        (glob->input_map & 0x0010) >>  4); // 0000 00010000
    XmlPrintInt(xp, dent, "input6",        (glob->input_map & 0x0020) >>  5); // 0000 00100000
    XmlPrintInt(xp, dent, "mux1",          (glob->input_map & 0x0300) >>  8); // 0011 00000000
    XmlPrintInt(xp, dent, "mux2",          (glob->input_map & 0x0C00) >> 10); // 1100 00000000

    XmlPrintInt(xp, dent, "dp_to",         glob->server_to);
    XmlPrintInt(xp, dent, "drift_tol",     glob->drift_tol);
    XmlPrintInt(xp, dent, "jump_filt",     glob->jump_filt);
    XmlPrintInt(xp, dent, "jump_thresh",   glob->jump_thresh);
    XmlPrintInt(xp, dent, "cal_offset",    glob->cal_offset);
    XmlPrintInt(xp, dent, "sensor_map",    glob->sensor_map);
    XmlPrintInt(xp, dent, "samp_phase",    (int) ((double) glob->sampling_phase * 1.5626)); /* covert from tics to usec */
    XmlPrintInt(xp, dent, "gps_cold",      glob->gps_cold);

    PrintGlobScaling(xp, dent, subdent, "scaling1", &glob->scaling[0][0]);
    PrintGlobScaling(xp, dent, subdent, "scaling2", &glob->scaling[1][0]);
    PrintGlobScaling(xp, dent, subdent, "scaling3", &glob->scaling[2][0]);
    PrintGlobScaling(xp, dent, subdent, "scaling4", &glob->scaling[3][0]);
    PrintGlobScaling(xp, dent, subdent, "scaling5", &glob->scaling[4][0]);
    PrintGlobScaling(xp, dent, subdent, "scaling6", &glob->scaling[5][0]);

    XmlPrintInt(xp, dent, "offsets1", glob->offset[0] * 100);
    XmlPrintInt(xp, dent, "offsets2", glob->offset[1] * 100);
    XmlPrintInt(xp, dent, "offsets3", glob->offset[2] * 100);
    XmlPrintInt(xp, dent, "offsets4", glob->offset[3] * 100);
    XmlPrintInt(xp, dent, "offsets5", glob->offset[4] * 100);
    XmlPrintInt(xp, dent, "offsets6", glob->offset[5] * 100);

    XmlPrintDouble(xp, dent, "%.1lf", "gains1", (double) glob->gain[0] / 1024.0);
    XmlPrintDouble(xp, dent, "%.1lf", "gains2", (double) glob->gain[1] / 1024.0);
    XmlPrintDouble(xp, dent, "%.1lf", "gains3", (double) glob->gain[2] / 1024.0);
    XmlPrintDouble(xp, dent, "%.1lf", "gains4", (double) glob->gain[3] / 1024.0);
    XmlPrintDouble(xp, dent, "%.1lf", "gains5", (double) glob->gain[4] / 1024.0);
    XmlPrintDouble(xp, dent, "%.1lf", "gains6", (double) glob->gain[5] / 1024.0);
    XmlPrintHex32(xp, dent, "crc", XmlCRC(xp, TRUE));

    XmlPrintInt(xp, dent, "web_port",      glob->web_port);
    XmlPrintInt(xp, dent, "user_tag",      glob->user_tag);
    XmlIndent(xp, indent); XmlPrint(xp, "</global>\n");
}

void qdpXmlPrint_C1_PHY(MYXML_HANDLE *xp, int indent, int subdent, QDP_TYPE_C1_PHY *phy)
{
int dent;
int i;

    dent = indent + subdent;

    XmlIndent(xp, indent); XmlStartCRC(xp); XmlPrint(xp, "<interfaces>\n");
        XmlPrintInt(xp, dent, "ser1_baud", qdpBaud(phy->serial[0].baud));
                                                                                     // 1111 11
                                                                                     // 5432 1098 7654 3210
        XmlPrintInt(xp, dent, "ser1_enable", (phy->serial[0].flags & 0x0001) >>  0); // 0000 0000 0000 0001
        XmlPrintInt(xp, dent, "ser1_hard",   (phy->serial[0].flags & 0x0004) >>  2); // 0000 0000 0000 0100
        XmlPrintInt(xp, dent, "ser1_break",  (phy->serial[0].flags & 0x0008) >>  3); // 0000 0000 0000 1000
        XmlPrintInt(xp, dent, "ser1_rping",  (phy->serial[0].flags & 0x0010) >>  4); // 0000 0000 0001 0000
        XmlPrintInt(xp, dent, "ser1_rtcp",   (phy->serial[0].flags & 0x0020) >>  5); // 0000 0000 0010 0000
        XmlPrintInt(xp, dent, "ser1_route",  (phy->serial[0].flags & 0x0040) >>  6); // 0000 0000 0100 0000
        XmlPrintInt(xp, dent, "ser1_rqping", (phy->serial[0].flags & 0x0080) >>  7); // 0000 0000 1000 0000
        XmlPrintInt(xp, dent, "ser1_bootp",  (phy->serial[0].flags & 0x0300) >>  8); // 0000 0011 0000 0000
        XmlPrintInt(xp, dent, "ser1_unlock", (phy->serial[0].flags & 0x0400) >> 10); // 0000 0100 0000 0000

        XmlPrintInt(xp, dent, "ser2_baud", qdpBaud(phy->serial[1].baud));
                                                                                     // 1111 11
                                                                                     // 5432 1098 7654 3210
        XmlPrintInt(xp, dent, "ser2_enable", (phy->serial[1].flags & 0x0001) >>  0); // 0000 0000 0000 0001
        XmlPrintInt(xp, dent, "ser2_hard",   (phy->serial[1].flags & 0x0004) >>  2); // 0000 0000 0000 0100
        XmlPrintInt(xp, dent, "ser2_break",  (phy->serial[1].flags & 0x0008) >>  3); // 0000 0000 0000 1000
        XmlPrintInt(xp, dent, "ser2_rping",  (phy->serial[1].flags & 0x0010) >>  4); // 0000 0000 0001 0000
        XmlPrintInt(xp, dent, "ser2_rtcp",   (phy->serial[1].flags & 0x0020) >>  5); // 0000 0000 0010 0000
        XmlPrintInt(xp, dent, "ser2_route",  (phy->serial[1].flags & 0x0040) >>  6); // 0000 0000 0100 0000
        XmlPrintInt(xp, dent, "ser2_rqping", (phy->serial[1].flags & 0x0080) >>  7); // 0000 0000 1000 0000
        XmlPrintInt(xp, dent, "ser2_bootp",  (phy->serial[1].flags & 0x0300) >>  8); // 0000 0011 0000 0000
        XmlPrintInt(xp, dent, "ser2_unlock", (phy->serial[1].flags & 0x0400) >> 10); // 0000 0100 0000 0000

        XmlPrintInt(xp, dent, "ser3_enable", (phy->serial[2].flags & 0x0001) >>  0); // 0000 0000 0000 0001
        XmlPrintInt(xp, dent, "ser3_route",  (phy->serial[2].flags & 0x0040) >>  6); // 0000 0000 0100 0000

                                                                                   // 1111 11
                                                                                   // 5432 1098 7654 3210
        XmlPrintInt(xp, dent, "eth_enable", (phy->ethernet.flags & 0x0001) >>  0); // 0000 0000 0000 0001
        XmlPrintInt(xp, dent, "eth_always", (phy->ethernet.flags & 0x0002) >>  1); // 0000 0000 0000 0010
        XmlPrintInt(xp, dent, "eth_rping",  (phy->ethernet.flags & 0x0010) >>  4); // 0000 0000 0001 0000
        XmlPrintInt(xp, dent, "eth_rtcp",   (phy->ethernet.flags & 0x0020) >>  5); // 0000 0000 0010 0000
        XmlPrintInt(xp, dent, "eth_route",  (phy->ethernet.flags & 0x0040) >>  6); // 0000 0000 0100 0000
        XmlPrintInt(xp, dent, "eth_rqping", (phy->ethernet.flags & 0x0080) >>  7); // 0000 0000 1000 0000
        XmlPrintInt(xp, dent, "eth_bootp",  (phy->ethernet.flags & 0x0300) >>  8); // 0000 0011 0000 0000
        XmlPrintInt(xp, dent, "eth_unlock", (phy->ethernet.flags & 0x0400) >> 10); // 0000 0100 0000 0000
        XmlPrintHex32(xp, dent, "crc", XmlCRC(xp, TRUE));

        if ( phy->serial[0].throttle) XmlPrintInt(xp, dent, "ser1_throt", 1024000 / phy->serial[0].throttle);
        if ( phy->serial[1].throttle) XmlPrintInt(xp, dent, "ser2_throt", 1024000 / phy->serial[1].throttle);
        if ( phy->serial[2].throttle) XmlPrintInt(xp, dent, "ser3_throt", 1024000 / phy->serial[2].throttle);

        XmlPrintDotDecimal(xp, dent, "ser1_ip", phy->serial[0].ip);
        XmlPrintDotDecimal(xp, dent, "ser2_ip", phy->serial[1].ip);
        XmlPrintDotDecimal(xp, dent, "eth_ip", phy->ethernet.ip);

        XmlPrintInt(xp, dent, "base_port", phy->baseport);
    XmlIndent(xp, indent); XmlPrint(xp, "</interfaces>\n");
}

static void PrintFreqMap(MYXML_HANDLE *xp, int indent, int subdent, char *tag, UINT16 value)
{
int dent;

    dent = indent + subdent;

    XmlIndent(xp, indent); XmlPrint(xp, "<%s>\n", tag);
        XmlPrintInt(xp, dent, "Hz1",   (value & 0x0001) >> 0);
        XmlPrintInt(xp, dent, "Hz10",  (value & 0x0002) >> 1);
        XmlPrintInt(xp, dent, "Hz20",  (value & 0x0004) >> 2);
        XmlPrintInt(xp, dent, "Hz40",  (value & 0x0008) >> 3);
        XmlPrintInt(xp, dent, "Hz50",  (value & 0x0010) >> 4);
        XmlPrintInt(xp, dent, "Hz100", (value & 0x0020) >> 5);
        XmlPrintInt(xp, dent, "Hz200", (value & 0x0040) >> 6);
    XmlIndent(xp, indent); XmlPrint(xp, "</%s>\n", tag);
}

void qdpXmlPrint_C1_LOG(MYXML_HANDLE *xp, int indent, int subdent, int index, QDP_TYPE_C1_LOG *log)
{
int dent;

    dent = indent + subdent;

    XmlIndent(xp, indent); XmlStartCRC(xp); XmlPrint(xp, "<data%d>\n", index+1);
        XmlPrintInt(xp, dent, "lport", index);
                                                                        // 1111 11
                                                                        // 5432 1098 7654 3210
        XmlPrintInt(xp, dent, "flooding",   log->flags & 0x0001 >>  0); // 0000 0000 0000 0001
        XmlPrintInt(xp, dent, "freeze_out", log->flags & 0x0002 >>  1); // 0000 0000 0000 0010
        XmlPrintInt(xp, dent, "freeze_in",  log->flags & 0x0004 >>  2); // 0000 0000 0000 0100
        XmlPrintInt(xp, dent, "keepold",    log->flags & 0x0008 >>  3); // 0000 0000 0000 1000
        XmlPrintInt(xp, dent, "piggyback",  log->flags & 0x0100 >>  8); // 0000 0001 0000 0000
        XmlPrintInt(xp, dent, "flushwarn",  log->flags & 0x0200 >>  9); // 0000 0010 0000 0000
        XmlPrintInt(xp, dent, "hotswap",    log->flags & 0x0400 >> 10); // 0000 0100 0000 0000
        XmlPrintInt(xp, dent, "base96",     log->flags & 0x2000 >> 13); // 0010 0000 0000 0000
        XmlPrintInt(xp, dent, "perc",       (int) (double) log->perc / 2.56);
        XmlPrintInt(xp, dent, "mtu", log->mtu);
        XmlPrintInt(xp, dent, "grp_cnt", log->group_cnt);
        XmlPrintDouble(xp, dent, "%.1lf", "rsnd_max", (double) log->rsnd_max / 10.0);
        XmlPrintDouble(xp, dent, "%.1lf", "grp_to", (double) log->grp_to / 10.0);
        XmlPrintDouble(xp, dent, "%.1lf", "rsnd_min", (double) log->rsnd_min / 10.0);
        XmlPrintInt(xp, dent, "window", log->window);
        PrintFreqMap(xp, dent, subdent, "freq_map1", log->freqs[0]);
        PrintFreqMap(xp, dent, subdent, "freq_map2", log->freqs[1]);
        PrintFreqMap(xp, dent, subdent, "freq_map3", log->freqs[2]);
        PrintFreqMap(xp, dent, subdent, "freq_map4", log->freqs[3]);
        PrintFreqMap(xp, dent, subdent, "freq_map5", log->freqs[4]);
        PrintFreqMap(xp, dent, subdent, "freq_map6", log->freqs[5]);
        XmlPrintInt(xp, dent, "ack_cnt", log->ack_cnt);
        XmlPrintDouble(xp, dent, "%.1lf", "ack_to", (double) log->ack_to / 10);
        if (log->eth_throttle) XmlPrintInt(xp, dent, "eth_throt", 1024000 / log->eth_throttle);
        XmlPrintInt(xp, dent, "full_alert", (int) (double) log->full_alert / 2.56);
        XmlPrintInt(xp, dent, "auto_filter", log->auto_filter);
        XmlPrintInt(xp, dent, "man_filter", log->man_filter);
        XmlPrintHex32(xp, dent, "crc", XmlCRC(xp, TRUE));
    XmlIndent(xp, indent); XmlPrint(xp, "</data%d>\n", index+1);
}

static void PrintSCMap(MYXML_HANDLE *xp, int indent, int subdent, char *tag, UINT32 value)
{
int dent;

    dent = indent + subdent;

    XmlIndent(xp, indent); XmlPrint(xp, "<%s>\n", tag);
        XmlPrintInt(xp, dent, "function", value & 0xff);
        XmlPrintInt(xp, dent, "high", (value & 0x0100) >> 8);
    XmlIndent(xp, indent); XmlPrint(xp, "</%s>\n", tag);
}

void qdpXmlPrintSc(MYXML_HANDLE *xp, int indent, int subdent, UINT32 *sc)
{
int dent;

    dent = indent + subdent;

    XmlIndent(xp, indent); XmlStartCRC(xp); XmlPrint(xp, "<sensctrl>\n");
        PrintSCMap(xp, dent, subdent, "mapping1", sc[0]);
        PrintSCMap(xp, dent, subdent, "mapping2", sc[1]);
        PrintSCMap(xp, dent, subdent, "mapping3", sc[2]);
        PrintSCMap(xp, dent, subdent, "mapping4", sc[3]);
        PrintSCMap(xp, dent, subdent, "mapping5", sc[4]);
        PrintSCMap(xp, dent, subdent, "mapping6", sc[5]);
        PrintSCMap(xp, dent, subdent, "mapping7", sc[6]);
        PrintSCMap(xp, dent, subdent, "mapping8", sc[7]);
        XmlPrintHex32(xp, dent, "crc", XmlCRC(xp, TRUE));
    XmlIndent(xp, indent); XmlPrint(xp, "</sensctrl>\n");
}

void qdpXmlPrint_C1_SPP(MYXML_HANDLE *xp, int indent, int subdent, QDP_TYPE_C1_SPP *spp)
{
int dent;

    dent = indent + subdent;

    XmlIndent(xp, indent); XmlStartCRC(xp); XmlPrint(xp, "<slave>\n");
        XmlPrintInt(xp, dent, "max_main", spp->max_main_current);
        XmlPrintInt(xp, dent, "min_off", spp->min_off_time);
        XmlPrintDouble(xp, dent, "%.1lf", "min_ps", (double) spp->min_ps_voltage * 0.15 );
        XmlPrintInt(xp, dent, "max_ant", spp->max_antenna_current);
        XmlPrintInt(xp, dent, "min_temp", spp->min_temp);
        XmlPrintInt(xp, dent, "max_temp", spp->max_temp);
        XmlPrintInt(xp, dent, "temp_hyst", spp->temp_hysteresis);
        XmlPrintDouble(xp, dent, "%.2lf", "volt_hyst", (double) spp->volt_hysteresis * 0.15);
        XmlPrintInt(xp, dent, "def_vco", spp->default_vco);
        XmlPrintHex32(xp, dent, "crc", XmlCRC(xp, TRUE));
    XmlIndent(xp, indent); XmlPrint(xp, "</slave>\n");
}

void qdpXmlPrint_C2_PHY(MYXML_HANDLE *xp, int indent, int subdent, char *tag, QDP_TYPE_C2_PHY *phy)
{
int dent;
double percent;

    dent = indent + subdent;

    XmlIndent(xp, indent); XmlStartCRC(xp); XmlPrint(xp, "<%s>\n", tag);
        XmlPrintInt(xp, dent, "phynum", phy->phynum);
        XmlPrintInt(xp, dent, "lognum", phy->lognum);
        XmlPrintString(xp, dent, "modem_init", phy->modem_init);
        XmlPrintString(xp, dent, "user_330", phy->dial_in.name);
        XmlPrintString(xp, dent, "pass_330", phy->dial_in.pass);
        percent = (double) (phy->trigger & 0xff) * 100.0 / 256.0;
        XmlPrintInt(xp, dent, "trigger", (int) floor(percent + 0.5)); // no round() in Solaris 9
                                                                           // 1111 11
                                                                           // 5432 1098 7654 3210
        if (phy->phynum != QDP_PP_ETHERNET) {
            XmlPrintInt(xp, dent, "ppp",         (phy->flags & 0x0001) >>  0); // 0000 0000 0000 0001
            XmlPrintInt(xp, dent, "dial",        (phy->flags & 0x0002) >>  1); // 0000 0000 0000 0010
            XmlPrintInt(xp, dent, "dial_ctrl",   (phy->flags & 0x000C) >>  2); // 0000 0000 0000 1100
        }
        XmlPrintInt(xp, dent, "baler_ctrl",  (phy->flags & 0x0030) >>  4); // 0000 0000 0011 0000
        XmlPrintInt(xp, dent, "at_end",      (phy->flags & 0x0040) >>  6); // 0000 0000 0100 0000
        XmlPrintInt(xp, dent, "at_trigger",  (phy->flags & 0x0080) >>  7); // 0000 0000 1000 0000
        XmlPrintInt(xp, dent, "at_interval", (phy->flags & 0x0100) >>  8); // 0000 0001 0000 0000
        XmlPrintInt(xp, dent, "baler_cmds",  (phy->flags & 0x0200) >>  9); // 0000 0010 0000 0000
        if (phy->phynum != QDP_PP_ETHERNET) {
            XmlPrintInt(xp, dent, "DTR",         (phy->flags & 0x0400) >> 10); // 0000 0100 0000 0000
        }
        XmlPrintInt(xp, dent, "NAPT",        (phy->flags & 0x0800) >> 11); // 0000 1000 0000 0000
        XmlPrintInt(xp, dent, "transparent", (phy->flags & 0x1000) >> 12); // 0001 0000 0000 0000
        XmlPrintInt(xp, dent, "baler44",     (phy->flags & 0x2000) >> 13); // 0010 0000 0000 0000
        XmlPrintInt(xp, dent, "simplex",     (phy->flags & 0x8000) >> 15); // 1000 0000 0000 0000

        XmlPrintInt(xp, dent, "retry_int", phy->retry_int);
        XmlPrintInt(xp, dent, "interval", phy->interval);
        XmlPrintInt(xp, dent, "webbps", phy->webbps * 10);
        XmlPrintInt(xp, dent, "retries", phy->retries);
        XmlPrintInt(xp, dent, "reg_to", phy->reg_to);
        XmlPrintInt(xp, dent, "routed_to", phy->routed_to);
                                                                            // 1111 11
        if (phy->phynum != QDP_PP_ETHERNET) {                               // 5432 1098 7654 3210
            XmlPrintInt(xp, dent, "ss_type",     (phy->ss & 0xF000) >> 12); // 1111 0000 0000 0000
            XmlPrintInt(xp, dent, "ss_rate",     (phy->ss & 0x0007) >>  0); // 0000 0000 0000 0111
            XmlPrintInt(xp, dent, "ss_fracdig",  (phy->ss & 0x0070) >>  4); // 0000 0000 0111 0000
            XmlPrintInt(xp, dent, "ss_temp",     (phy->ss & 0x0080) >>  7); // 0000 0000 1000 0000
            XmlPrintInt(xp, dent, "ss_units",    (phy->ss & 0x0F00) >>  8); // 0000 1111 0000 0000
        } else {
            XmlPrintInt(xp, dent, "serial_baud", qdpBaud(phy->baud));
        }

        XmlPrintHex32(xp, dent, "crc", XmlCRC(xp, TRUE));

        XmlPrintString(xp, dent, "phone", phy->phone_number);
        XmlPrintString(xp, dent, "user_isp", phy->dial_out.name);
        XmlPrintString(xp, dent, "pass_isp", phy->dial_out.pass);
        XmlPrintDotDecimal(xp, dent, "poc_ip", phy->poc_ip);
        XmlPrintDotDecimal(xp, dent, "log2_ip", phy->log2_ip);
        XmlPrintInt(xp, dent, "poc_port", phy->poc_port);

    XmlIndent(xp, indent); XmlPrint(xp, "</%s>\n", tag);
}

static void PrintAmassSensor(MYXML_HANDLE *xp, int indent, int subdent, char *tag, QDP_TYPE_C2_AMASS_SENSOR *sensor)
{
int dent;

    dent = indent + subdent;

    XmlIndent(xp, indent); XmlPrint(xp, "<%s>\n", tag);
        XmlPrintInt(xp, dent, "tol_A", sensor->tolerance[0]);
        XmlPrintInt(xp, dent, "tol_B", sensor->tolerance[1]);
        XmlPrintInt(xp, dent, "tol_C", sensor->tolerance[2]);
        XmlPrintInt(xp, dent, "max_try", sensor->maxtry);
        XmlPrintInt(xp, dent, "norm_int", sensor->interval.normal);
        XmlPrintInt(xp, dent, "squelch_int", sensor->interval.squelch);
        XmlPrintInt(xp, dent, "sensor_map", sensor->bitmap);
        XmlPrintDouble(xp, dent, "%.2lf", "duration", (double) sensor->duration * 0.010);
    XmlIndent(xp, indent); XmlPrint(xp, "</%s>\n", tag);
}

void qdpXmlPrint_C2_AMASS(MYXML_HANDLE *xp, int indent, int subdent, QDP_TYPE_C2_AMASS *amass)
{
int dent;

    dent = indent + subdent;

    XmlIndent(xp, indent); XmlStartCRC(xp); XmlPrint(xp, "<automass>\n");
        PrintAmassSensor(xp, dent, subdent, "group1", &amass->sensor[0]);
        PrintAmassSensor(xp, dent, subdent, "group2", &amass->sensor[1]);
        XmlPrintHex32(xp, dent, "crc", XmlCRC(xp, TRUE));
    XmlIndent(xp, indent); XmlPrint(xp, "</automass>\n");
}

void qdpXmlPrint_C2_GPS(MYXML_HANDLE *xp, int indent, int subdent, QDP_TYPE_C2_GPS *gps)
{
int dent;

    dent = indent + subdent;

    XmlIndent(xp, indent); XmlStartCRC(xp); XmlPrint(xp, "<gps>\n");
        XmlPrintInt(xp, dent, "mode",        (gps->mode & 0x0007) >>  0);
        XmlPrintInt(xp, dent, "export",      (gps->mode & 0x0008) >>  3);
        XmlPrintInt(xp, dent, "RS422",       (gps->mode & 0x0010) >>  4);
        XmlPrintInt(xp, dent, "Serial_DGPS", (gps->mode & 0x0020) >>  5);
        XmlPrintInt(xp, dent, "Net_DGPS",    (gps->mode & 0x0040) >>  6);
        XmlPrintInt(xp, dent, "power_mode", gps->flags);
        XmlPrintInt(xp, dent, "off_time", gps->off_time);
        XmlPrintInt(xp, dent, "resync", gps->resync);
        XmlPrintInt(xp, dent, "max_on", gps->max_on);
        XmlPrintHex32(xp, dent, "crc", XmlCRC(xp, TRUE));
    XmlIndent(xp, indent); XmlPrint(xp, "</gps>\n");

    XmlIndent(xp, indent); XmlStartCRC(xp); XmlPrint(xp, "<pll>\n");
        XmlPrintInt(xp, dent, "lock_usec", gps->lock_usec);
        XmlPrintInt(xp, dent, "interval", gps->interval);
        XmlPrintInt(xp, dent, "enab_pll", (gps->initial_pll & 0x0001) >>  0);
        XmlPrintInt(xp, dent, "lock_2D",    (gps->initial_pll & 0x0002) >>  1);
        XmlPrintInt(xp, dent, "vco_temp",   (gps->initial_pll & 0x0004) >>  2);
        XmlPrintDouble(xp, dent, "%.1lf", "pfrac", gps->pfrac);
        XmlPrintDouble(xp, dent, "%.4lf", "vco_slope", gps->vco_slope);
        XmlPrintDouble(xp, dent, "%.2lf", "vco_icpt", gps->vco_intercept);
        XmlPrintDouble(xp, dent, "%.2lf", "max_ikm_rms", gps->max_ikm_rms);
        XmlPrintDouble(xp, dent, "%.2lf", "ikm_weight", gps->ikm_weight);
        XmlPrintDouble(xp, dent, "%.2lf", "km_weight", gps->km_weight);
        XmlPrintDouble(xp, dent, "%.1lf", "best_weight", gps->best_weight);
        XmlPrintDouble(xp, dent, "%.1lf", "km_delta", gps->km_delta);
        XmlPrintHex32(xp, dent, "crc", XmlCRC(xp, TRUE));
    XmlIndent(xp, indent); XmlPrint(xp, "</pll>\n");
}

static void PrintAnncEntry(MYXML_HANDLE *xp, int indent, int subdent, char *tag, QDP_TYPE_C3_ANNC_ENTRY *entry)
{
int dent;

    dent = indent + subdent;

    XmlIndent(xp, indent); XmlPrint(xp, "<%s>\n", tag);
        XmlPrintDotDecimal(xp, dent, "dp_ip", entry->dp_ip);
        XmlPrintDotDecimal(xp, dent, "router_ip", entry->router_ip);
        XmlPrintInt(xp, dent, "timeout", entry->timeout);
        XmlPrintInt(xp, dent, "resume", entry->resume);
        XmlPrintInt(xp, dent, "dp_port", entry->dp_port);
        XmlPrintInt(xp, dent, "log1",     (entry->flags & 0x0001) >>  0);
        XmlPrintInt(xp, dent, "log2",   (entry->flags & 0x0002) >>  1);
        XmlPrintInt(xp, dent, "log3",   (entry->flags & 0x0004) >>  2);
        XmlPrintInt(xp, dent, "log4",   (entry->flags & 0x0008) >>  3);
        XmlPrintInt(xp, dent, "ser1",   (entry->flags & 0x0010) >>  4);
        XmlPrintInt(xp, dent, "ser2",   (entry->flags & 0x0020) >>  5);
        XmlPrintInt(xp, dent, "eth",    (entry->flags & 0x0080) >>  7);
        XmlPrintInt(xp, dent, "ignore", (entry->flags & 0x0100) >>  8);
        XmlPrintInt(xp, dent, "random", (entry->flags & 0x0200) >>  9);
    XmlIndent(xp, indent); XmlPrint(xp, "</%s>\n", tag);
}

void qdpXmlPrint_C3_ANNC(MYXML_HANDLE *xp, int indent, int subdent, QDP_TYPE_C3_ANNC *annc)
{
int dent;

    dent = indent + subdent;

    XmlIndent(xp, indent); XmlPrint(xp, "<announce>\n");
        XmlPrintInt(xp, dent, "dps_active", annc->dps_active);
        XmlPrintInt(xp, dent, "eth_unlock", (annc->flags & 0x0800) >> 11); // 0000 1000 0000 0000
        XmlPrintInt(xp, dent, "ser1_unlock", (annc->flags & 0x0100) >>  8); // 0000 0001 0000 0000
        XmlPrintInt(xp, dent, "ser2_unlock", (annc->flags & 0x0200) >>  9); // 0000 0010 0000 0000
        PrintAnncEntry(xp, dent, subdent, "dp1", &annc->entry[0]);
        PrintAnncEntry(xp, dent, subdent, "dp2", &annc->entry[1]);
        PrintAnncEntry(xp, dent, subdent, "dp3", &annc->entry[2]);
        PrintAnncEntry(xp, dent, subdent, "dp4", &annc->entry[3]);
        PrintAnncEntry(xp, dent, subdent, "dp5", &annc->entry[4]);
        PrintAnncEntry(xp, dent, subdent, "dp6", &annc->entry[5]);
    XmlIndent(xp, indent); XmlPrint(xp, "</announce>\n");
}

void qdpXmlPrint_C3_BCFG(MYXML_HANDLE *xp, int indent, int subdent, QDP_TYPE_C3_BCFG *bcfg)
{
int dent;

    dent = indent + subdent;

    XmlIndent(xp, indent); XmlStartCRC(xp); XmlPrint(xp, "<balecfg>\n");
        XmlPrintInt(xp, dent, "phyport", bcfg->phyport);
        XmlPrintInt(xp, dent, "balertype", bcfg->balertype);
        XmlPrintInt(xp, dent, "version", bcfg->version);
        XmlPrintString(xp, dent, "freeform", bcfg->freeform);
        XmlPrintHex32(xp, dent, "crc", XmlCRC(xp, TRUE));
    XmlIndent(xp, indent); XmlPrint(xp, "</balecfg>\n");
}

void qdpXmlPrint_C2_EPCFG(MYXML_HANDLE *xp, int indent, int subdent, QDP_TYPE_C2_EPCFG *epcfg)
{
int dent, i;
char tag[16]; /* large enough to hold chanmaskxxx */

    dent = indent + subdent;

    XmlIndent(xp, indent); XmlStartCRC(xp); XmlPrint(xp, "<envproc>\n");
        XmlPrintInt(xp, dent, "res1", epcfg->res1);
        XmlPrintInt(xp, dent, "res2", epcfg->res2);
        XmlPrintInt(xp, dent, "wxt1heat", epcfg->flags1 & QDP_C2_EPCFG_FLAG_WXT520_CONTINUOUS);
        XmlPrintInt(xp, dent, "wxt2heat", epcfg->flags2 & QDP_C2_EPCFG_FLAG_WXT520_CONTINUOUS);
        XmlPrintInt(xp, dent, "chancnt", epcfg->chancnt);
        XmlPrintInt(xp, dent, "spare", epcfg->spare);
        for (i = 0; i < epcfg->chancnt; i++) {
            sprintf(tag, "chanmask%d", i);
            XmlPrintInt(xp, dent, tag, epcfg->entry[i].raw);
        }
        XmlPrintHex32(xp, dent, "crc", XmlCRC(xp, TRUE));
    XmlIndent(xp, indent); XmlPrint(xp, "</envproc>\n");
}

void qdpXmlPrintTokenVersion(MYXML_HANDLE *xp, int indent, int subdent, QDP_DP_TOKEN *token)
{
int dent;

    if (!token->ver.valid) return;

    dent = indent;

    XmlPrintInt(xp, dent, "tokever", token->ver.version);
    XmlPrintString(xp, dent, "tokenset", token->setname);
}

void qdpXmlPrintTokenLogid(MYXML_HANDLE *xp, int indent, int subdent, QDP_DP_TOKEN *token)
{
int dent;

    if (!token->logid.valid) return;

    dent = indent;

    XmlPrintString(xp, dent, "msgloc", token->logid.mesg.loc);
    XmlPrintString(xp, dent, "msgname", token->logid.mesg.chn);
    XmlPrintString(xp, dent, "timloc", token->logid.time.loc);
    XmlPrintString(xp, dent, "timname", token->logid.time.chn);
}

void qdpXmlPrintTokenCnfid(MYXML_HANDLE *xp, int indent, int subdent, QDP_DP_TOKEN *token)
{
int dent;

    if (!token->cnfid.valid) return;

    dent = indent;

    XmlPrintString(xp, dent, "cfgloc", token->cnfid.loc);
    XmlPrintString(xp, dent, "cfgname", token->cnfid.chn);
    XmlPrintInt(xp, dent, "cfginterval", token->cnfid.interval);
    XmlPrintInt(xp, dent, "cfgflgbeg", (token->cnfid.flags & 0x0001) >> 0);
    XmlPrintInt(xp, dent, "cfgflgend", (token->cnfid.flags & 0x0002) >> 1);
    XmlPrintInt(xp, dent, "cfgflgint", (token->cnfid.flags & 0x0004) >> 2);
}

void qdpXmlPrintTokenDss(MYXML_HANDLE *xp, int indent, int subdent, QDP_DP_TOKEN *token)
{
int dent;

    if (!token->dss.valid) return;

    dent = indent + subdent;

    XmlIndent(xp, indent); XmlPrint(xp, "<dss>\n");
        XmlPrintString(xp, dent, "highest", token->dss.passwd.hi);
        XmlPrintString(xp, dent, "middle", token->dss.passwd.mi);
        XmlPrintString(xp, dent, "lowest", token->dss.passwd.lo);
        XmlPrintInt(xp, dent, "timeout", token->dss.timeout);
        XmlPrintInt(xp, dent, "maxbps", token->dss.maxbps);
        XmlPrintInt(xp, dent, "verbosity", token->dss.verbosity);
        XmlPrintInt(xp, dent, "maxcpu", token->dss.maxcpu);
        XmlPrintInt(xp, dent, "portnum", token->dss.port);
        XmlPrintInt(xp, dent, "maxmem", token->dss.maxmem);
    XmlIndent(xp, indent); XmlPrint(xp, "</dss>\n");
}

void qdpXmlPrintTokenClock(MYXML_HANDLE *xp, int indent, int subdent, QDP_DP_TOKEN *token)
{
int dent;

    if (!token->clock.valid) return;

    dent = indent + subdent;

    XmlIndent(xp, indent); XmlPrint(xp, "<clock>\n");
        XmlPrintInt(xp, dent, "zoneoffset", token->clock.offset);
        XmlPrintInt(xp, dent, "lossminutes", token->clock.maxlim);
        XmlPrintInt(xp, dent, "locked", token->clock.pll.locked);
        XmlPrintInt(xp, dent, "tracking", token->clock.pll.track);
        XmlPrintInt(xp, dent, "hold", token->clock.pll.hold);
        XmlPrintInt(xp, dent, "off", token->clock.pll.off);
        XmlPrintInt(xp, dent, "highlock", token->clock.maxhbl);
        XmlPrintInt(xp, dent, "lowlock", token->clock.minhbl);
        XmlPrintInt(xp, dent, "neverlock", token->clock.nbl);
        XmlPrintInt(xp, dent, "clkfilter", token->clock.clkqflt);
    XmlIndent(xp, indent); XmlPrint(xp, "</clock>\n");
}

void qdpXmlPrintTokenFirList(MYXML_HANDLE *xp, int indent, int subdent, QDP_DP_TOKEN *token)
{
int i;
int dent;
QDP_TOKEN_FIR *fir;

    dent = indent;

    for (i = 0; i < token->fir.count; i++) {
        fir = (QDP_TOKEN_FIR *) token->fir.array[i];
        XmlPrintString(xp, dent, "firname", fir->name);
    }
}

static void PrintTokenIir(MYXML_HANDLE *xp, int indent, int subdent, QDP_TOKEN_IIR *iir)
{
int i;
int dent;
char tag[8];

    dent = indent + subdent;

    XmlIndent(xp, indent); XmlPrint(xp, "<iir>\n");
        XmlPrintString(xp, dent, "name", iir->name);
        XmlPrintInt(xp, dent, "sections", iir->nsection);
        XmlPrintDouble(xp, dent, "%1lf", "gain", iir->gain);
        XmlPrintDouble(xp, dent, "%lf", "reffreq", iir->refreq);
        for (i = 0; i < iir->nsection; i++) {
            sprintf(tag, "sect%d", i);
            XmlIndent(xp, dent); XmlPrint(xp, "<%s>\n", tag);
                XmlPrintDouble(xp, dent+subdent, "%lf", "cutratio", iir->data[i].ratio);
                XmlPrintInt(xp, dent+subdent, "poles", (iir->data[i].npole & 0x7f));
                XmlPrintInt(xp, dent+subdent, "high",  iir->data[i].type);
            XmlIndent(xp, dent); XmlPrint(xp, "</%s>\n", tag);
        }
    XmlIndent(xp, indent); XmlPrint(xp, "</iir>\n");
}

void qdpXmlPrintTokenIirList(MYXML_HANDLE *xp, int indent, int subdent, QDP_DP_TOKEN *token)
{
int i;
QDP_TOKEN_IIR *iir;

    for (i = 0; i < token->iir.count; i++) {
        iir = (QDP_TOKEN_IIR *) token->iir.array[i];
        PrintTokenIir(xp, indent, subdent, iir);
    }
}

static void PrintTokenDet(MYXML_HANDLE *xp, int indent, int subdent, QDP_DP_TOKEN *token, QDP_TOKEN_DET *det)
{
int i;
int dent;

    dent = indent + subdent;

    if (det->type == QDP_TOKEN_TYPE_MHD) {
        XmlIndent(xp, indent); XmlPrint(xp, "<murdock>\n");
            XmlPrintString(xp, dent, "filter", det->detf.name);
            XmlPrintInt(xp, dent, "filhi", det->fhi);
            XmlPrintInt(xp, dent, "fillo", det->flo);
            XmlPrintInt(xp, dent, "iwin", det->iw);
            XmlPrintInt(xp, dent, "n_hits", det->nht);
            XmlPrintInt(xp, dent, "xth1", det->x1 * 2);
            XmlPrintInt(xp, dent, "xth2", det->x2 * 2);
            XmlPrintInt(xp, dent, "xth3", det->x3 * 2);
            XmlPrintInt(xp, dent, "xthx", det->xx);
            XmlPrintInt(xp, dent, "def_tc", det->tc);
            XmlPrintInt(xp, dent, "wait_blk", det->wa);
            XmlPrintInt(xp, dent, "val_avg", det->av);
            XmlPrintString(xp, dent, "name", det->name);
        XmlIndent(xp, indent); XmlPrint(xp, "</murdock>\n");
    } else {
        XmlIndent(xp, indent); XmlPrint(xp, "<threshold>\n");
            XmlPrintString(xp, dent, "filter", det->detf.name);
            XmlPrintInt(xp, dent, "hysteresis", det->iw);
            XmlPrintInt(xp, dent, "window", det->nht);
            XmlPrintInt(xp, dent, "high", det->fhi);
            XmlPrintInt(xp, dent, "low", det->flo);
            XmlPrintInt(xp, dent, "tail", det->wa);
            XmlPrintString(xp, dent, "name", det->name);
        XmlIndent(xp, indent); XmlPrint(xp, "</threshold>\n");
    }
}

void qdpXmlPrintTokenDetList(MYXML_HANDLE *xp, int indent, int subdent, QDP_DP_TOKEN *token)
{
int i;
QDP_TOKEN_DET *det;

    for (i = 0; i < token->det.count; i++) {
        det = (QDP_TOKEN_DET *) token->det.array[i];
        PrintTokenDet(xp, indent, subdent, token, det);
    }
}

static void PrintTokenCen(MYXML_HANDLE *xp, int indent, int subdent, QDP_DP_TOKEN *token, QDP_TOKEN_CEN *cen)
{
int dent;
char tag[128];

    dent = indent;

    sprintf(tag, "commname_%d", cen->id+1);
    XmlPrintString(xp, dent, tag, cen->name);;
}

void qdpXmlPrintTokenCenList(MYXML_HANDLE *xp, int indent, int subdent, QDP_DP_TOKEN *token)
{
int i;
QDP_TOKEN_CEN *cen;

    for (i = 0; i < token->cen.count; i++) {
        cen = (QDP_TOKEN_CEN *) token->cen.array[i];
        PrintTokenCen(xp, indent, subdent, token, cen);
    }
}

static void PrintDetectors(MYXML_HANDLE *xp, int indent, int subdent, QDP_DP_TOKEN *token, QDP_TOKEN_LCQ *lcq, int max)
{
int i;
int dent;

    dent = indent + subdent;
    for (i = 0; i <= max; i++) {
        XmlIndent(xp, indent); XmlPrint(xp, "<detect>\n");
            XmlPrintString(xp, dent, "name", lcq->detect[i].base.name);
            XmlPrintInt(xp, dent, "run", (lcq->detect[i].options & 0x01) >> 0);
            XmlPrintInt(xp, dent, "log", (lcq->detect[i].options & 0x02) >> 1);
            XmlPrintInt(xp, dent, "msg", (lcq->detect[i].options & 0x08) >> 3);
        XmlIndent(xp, indent); XmlPrint(xp, "</detect>\n");
    }
}

static void PrintTokenLcq(MYXML_HANDLE *xp, int indent, int subdent, QDP_DP_TOKEN *token, QDP_TOKEN_LCQ *lcq)
{
int i;
int dent;
char buf[QDP_LCQ_MAX_NAMELEN];

    dent = indent + subdent;

    XmlIndent(xp, indent); XmlPrint(xp, "<lcq>\n");
        XmlPrintString(xp, dent, "loc", lcq->loc);
        XmlPrintString(xp, dent, "seed", lcq->chn);
        XmlPrintBint8(xp, dent, "chan", lcq->src[0]);
        XmlPrintInt(xp, dent, "subchan", lcq->src[1]);
        XmlPrintInt(xp, dent, "rate", lcq->rate);

        XmlPrintInt(xp, dent, "evonly", lcq->options & QDP_LCQ_TRIGGERED_MODE ? 1 : 0);
        XmlPrintInt(xp, dent, "detpack", lcq->options & QDP_LCQ_WRITE_DETECT_PKTS ? 1 : 0);
        XmlPrintInt(xp, dent, "calpack", lcq->options & QDP_LCQ_WRITE_CALIB_PKTS ? 1 : 0);
        XmlPrintInt(xp, dent, "nooutput", lcq->options & QDP_LCQ_DO_NOT_OUTPUT ? 1 : 0);
        XmlPrintInt(xp, dent, "disable", lcq->options & QDP_LCQ_DISABLE ? 1 : 0);
        XmlPrintInt(xp, dent, "dataserv", lcq->options & QDP_LCQ_SEND_TO_DATA_SERVER ? 1 : 0);
        XmlPrintInt(xp, dent, "netserv", lcq->options & QDP_LCQ_SEND_TO_NET_SERVER ? 1 : 0);
        XmlPrintInt(xp, dent, "netevt", lcq->options & QDP_LCQ_NETSERV_EVENT_ONLY ? 1 : 0);
        XmlPrintInt(xp, dent, "cnpforce", lcq->options & QDP_LCQ_FORCE_CNP_BLOCKETTTES ? 1 : 0);

        if (lcq->options & QDP_LCQ_HAVE_PRE_EVENT_BUFFERS) XmlPrintInt(xp, dent, "precount", lcq->pebuf);
        if (lcq->options & QDP_LCQ_HAVE_GAP_THRESHOLD    ) XmlPrintDouble(xp, dent, "%.1lf", "gap", lcq->gapthresh);
        if (lcq->options & QDP_LCQ_HAVE_CALIB_DELAY      ) XmlPrintInt(xp, dent, "caldly", lcq->caldly);
        if (lcq->options & QDP_LCQ_HAVE_FRAME_COUNT      ) XmlPrintInt(xp, dent, "maxframe", lcq->comfr);
        if (lcq->options & QDP_LCQ_HAVE_FIR_MULTIPLIER   ) XmlPrintDouble(xp, dent, "%lf", "firfix", lcq->firfix);

        if (lcq->options & QDP_LCQ_HAVE_AVEPAR) {
            XmlPrintInt(xp, dent, "avgsamps", lcq->ave.len);
            XmlPrintString(xp, dent, "avgfilt", lcq->ave.filt.name);
        }

        if (lcq->options & QDP_LCQ_HAVE_CNTRL_DETECTOR)    XmlPrintString(xp, dent, "ctrldet", lcq->cntrl.name);

        if (lcq->options & QDP_LCQ_HAVE_DETECTOR_8) {
            PrintDetectors(xp, dent, subdent, token, lcq, 7);
        } else if (lcq->options & QDP_LCQ_HAVE_DETECTOR_7) {
            PrintDetectors(xp, dent, subdent, token, lcq, 6);
        } else if (lcq->options & QDP_LCQ_HAVE_DETECTOR_6) {
            PrintDetectors(xp, dent, subdent, token, lcq, 5);
        } else if (lcq->options & QDP_LCQ_HAVE_DETECTOR_5) {
            PrintDetectors(xp, dent, subdent, token, lcq, 4);
        } else if (lcq->options & QDP_LCQ_HAVE_DETECTOR_4) {
            PrintDetectors(xp, dent, subdent, token, lcq, 3);
        } else if (lcq->options & QDP_LCQ_HAVE_DETECTOR_3) {
            PrintDetectors(xp, dent, subdent, token, lcq, 2);
        } else if (lcq->options & QDP_LCQ_HAVE_DETECTOR_2) {
            PrintDetectors(xp, dent, subdent, token, lcq, 1);
        } else if (lcq->options & QDP_LCQ_HAVE_DETECTOR_1) {
            PrintDetectors(xp, dent, subdent, token, lcq, 0);
        }

        if (lcq->options & QDP_LCQ_HAVE_DECIM_ENTRY) {
            XmlPrintString(xp, dent, "decsource", lcq->decim.src.name);
            XmlPrintString(xp, dent, "decfilt", lcq->decim.fir.name);
        }

    XmlIndent(xp, indent); XmlPrint(xp, "</lcq>\n");
}

void qdpXmlPrintTokenLcqList(MYXML_HANDLE *xp, int indent, int subdent, QDP_DP_TOKEN *token)
{
int i;
QDP_TOKEN_LCQ *lcq;

    for (i = 0; i < token->lcq.count; i++) {
        lcq = (QDP_TOKEN_LCQ *) token->lcq.array[i];
        PrintTokenLcq(xp, indent, subdent, token, lcq);
    }
}

static void PrintTokenCds(MYXML_HANDLE *xp, int indent, int subdent, QDP_DP_TOKEN *token, QDP_TOKEN_CDS *cds)
{
int i, dent;

    dent = indent + subdent;

    XmlIndent(xp, indent); XmlPrint(xp, "<controldet>\n");
        XmlPrintInt(xp, dent, "logmsg", (cds->options & 0x01));
        XmlPrintString(xp, dent, "name", cds->name);
        for (i = 0; i < cds->nentry; i++) {
            switch ((cds->equation[i].code & 0xC0) >> 6) {
              case QDP_CDS_NIB_COMM:
                XmlPrintString(xp, dent, "comm", cds->equation[i].name);
                break;
              case QDP_CDS_NIB_DET:
                XmlPrintString(xp, dent, "det", cds->equation[i].name);
                break;
              case QDP_CDS_NIB_CAL:
                XmlPrintString(xp, dent, "cal", cds->equation[i].name);
                break;
              case QDP_CDS_NIB_OP:
                XmlPrintString(xp, dent, "op", cds->equation[i].name);
                break;
            }
        }
    XmlIndent(xp, indent); XmlPrint(xp, "</controldet>\n");
}

void qdpXmlPrintTokenCdsList(MYXML_HANDLE *xp, int indent, int subdent, QDP_DP_TOKEN *token)
{
int i;
QDP_TOKEN_CDS *cds;

    for (i = 0; i < token->cds.count; i++) {
        cds = (QDP_TOKEN_CDS *) token->cds.array[i];
        PrintTokenCds(xp, indent, subdent, token, cds);
    }
}

void qdpXmlPrintTokenSite(MYXML_HANDLE *xp, int indent, int subdent, QDP_DP_TOKEN *token)
{
int dent;

    if (!token->site.valid) return;

    dent = indent;

    XmlPrintString(xp, dent, "network", token->site.nname);
    XmlPrintString(xp, dent, "station", token->site.sname);
}

void qdpXmlPrintTokenServices(MYXML_HANDLE *xp, int indent, int subdent, QDP_DP_TOKEN *token)
{
int dent;

    dent = indent;

    if (token->srvr.net.valid) XmlPrintUINT16(xp, dent, "netport", token->srvr.net.port);
    if (token->srvr.web.valid) XmlPrintUINT16(xp, dent, "webport", token->srvr.web.port);
    if (token->srvr.data.valid) XmlPrintUINT16(xp, dent, "dataport", token->srvr.data.port);
}

void qdpXmlPrintTokens(MYXML_HANDLE *xp, int indent, int subdent, int index, QDP_DP_TOKEN *token)
{
int dent;

    dent = indent + subdent;

    XmlIndent(xp, indent); XmlStartCRC(xp); XmlPrint(xp, "<tokens%d>\n", index+1);
        qdpXmlPrintTokenVersion  (xp, dent, subdent, token); // token->ver);
        qdpXmlPrintTokenLogid    (xp, dent, subdent, token); // token->logid);
        qdpXmlPrintTokenCnfid    (xp, dent, subdent, token); // token->cnfid);
        XmlPrintInt(xp, dent, "noncomp",   token->noncomp);
        qdpXmlPrintTokenDss      (xp, dent, subdent, token); // token->dss);
        qdpXmlPrintTokenClock    (xp, dent, subdent, token); // token->clock);
        qdpXmlPrintTokenCenList  (xp, dent, subdent, token); // token->cen);
        qdpXmlPrintTokenFirList  (xp, dent, subdent, token); // token->fir);
        qdpXmlPrintTokenIirList  (xp, dent, subdent, token); // token->iir);
        qdpXmlPrintTokenDetList  (xp, dent, subdent, token); // token->det);
        qdpXmlPrintTokenLcqList  (xp, dent, subdent, token); // token->lcq);
        //qdpXmlPrintTokenDplcqList(xp, dent, subdent, token); // token->lcq);
        qdpXmlPrintTokenCdsList  (xp, dent, subdent, token); // token->cds);
        XmlPrintHex32(xp, dent, "crc", XmlCRC(xp, TRUE));
        qdpXmlPrintTokenSite     (xp, dent, subdent, token); // token->site);
        qdpXmlPrintTokenServices (xp, dent, subdent, token); // token->srvr);
    XmlIndent(xp, indent); XmlPrint(xp, "</tokens%d>\n", index+1);
}

void qdpXmlPrintWebpage(MYXML_HANDLE *xp, int indent, int subdent, char *tag, LNKLST *list)
{
int i;
char *line;
BOOL ready = FALSE;
int dent;
UINT32 crc;

    dent = indent + subdent;

      XmlIndent(xp, indent); XmlStartCRC(xp); XmlPrint(xp, "<%s>\n", tag);
        XmlIndent(xp, dent); XmlPrint(xp, "<![CDATA[\n");
        for (i = 0; i < list->count; i++) {
            line = (char *) list->array[i];
            if (!ready && line[0] == '<') ready = TRUE;
            if (ready) { XmlIndent(xp, dent); XmlPrint(xp, "%s\n", line); }
        }
        XmlPrintHex32(xp, dent, "crc", XmlCRC(xp, TRUE));
        XmlIndent(xp, dent); XmlPrint(xp, "]]>\n");
    XmlIndent(xp, indent); XmlPrint(xp, "</%s>\n", tag);
}

void qdpXmlPrintFullConfig(MYXML_HANDLE *xp, QDP_TYPE_FULL_CONFIG *config, char *basetag)
{
int i;
time_t now;
struct tm result;
int indent = 0;
int subdent = 2;
static char *fid = "IDA libqdp:qdpXmlPrintFullConfig()";

    now = time(NULL);
    if (!(config->set & QDP_CONFIG_DEFINED_WRITER)) {
        strlcpy(config->writer.proto_ver, "1", QDP_XML_WRITER_MAXSTRLEN+1);
        strlcpy(config->writer.name, fid, QDP_XML_WRITER_MAXSTRLEN+1);
        strlcpy(config->writer.soft_ver, "$Id: print.c,v 1.9 2016/01/23 00:09:34 dechavez Exp $", QDP_XML_WRITER_MAXSTRLEN+1);
        ASCTIME_R(localtime_r(&now, &result), config->writer.created, QDP_XML_WRITER_MAXSTRLEN);
        ASCTIME_R(localtime_r(&now, &result), config->writer.updated, QDP_XML_WRITER_MAXSTRLEN);
        utilTrimNewline(config->writer.created);
        utilTrimNewline(config->writer.updated);
        config->set |= QDP_CONFIG_DEFINED_WRITER;
    }

    XmlPrint(xp, "<?xml version=\"1.0\" encoding=\"iso-8859-1\" ?>\n");
    XmlPrint(xp, "\n");
    XmlPrint(xp, "<Q330_Configuration>\n");
        if (config->set & QDP_CONFIG_DEFINED_WRITER) qdpXmlPrintWriter(xp, indent, subdent, &config->writer);
        if (config->set & QDP_CONFIG_DEFINED_GLOBAL) qdpXmlPrint_C1_GLOB(xp, indent, subdent, &config->glob, basetag != NULL ? basetag : config->basetag);
        if (config->set & QDP_CONFIG_DEFINED_INTERFACES) qdpXmlPrint_C1_PHY(xp, indent, subdent, &config->phy);
        for (i = 0; i < QDP_NLP; i++) if (config->dport[i].set & QDP_DPORT_DEFINED_LOG) qdpXmlPrint_C1_LOG(xp, indent, subdent, i, &config->dport[i].log);
        if (config->set & QDP_CONFIG_DEFINED_SENSCTRL  ) qdpXmlPrintSc(xp, indent, subdent, config->sc);
        if (config->set & QDP_CONFIG_DEFINED_SLAVE     ) qdpXmlPrint_C1_SPP(xp, indent, subdent, &config->spp);
        if (config->set & QDP_CONFIG_DEFINED_ADVSER1   ) qdpXmlPrint_C2_PHY(xp, indent, subdent, "advser1",  &config->phy2[QDP_PP_SERIAL_1]);
        if (config->set & QDP_CONFIG_DEFINED_ADVSER2   ) qdpXmlPrint_C2_PHY(xp, indent, subdent, "advser2",  &config->phy2[QDP_PP_SERIAL_2]);
        if (config->set & QDP_CONFIG_DEFINED_ADVETHER  ) qdpXmlPrint_C2_PHY(xp, indent, subdent, "advether", &config->phy2[QDP_PP_ETHERNET]);
        if (config->set & QDP_CONFIG_DEFINED_GPS       ) qdpXmlPrint_C2_GPS(xp, indent, subdent, &config->gps);
        if (config->set & QDP_CONFIG_DEFINED_AUTOMASS  ) qdpXmlPrint_C2_AMASS(xp, indent, subdent, &config->amass);
        if (config->set & QDP_CONFIG_DEFINED_ANNOUNCE  ) qdpXmlPrint_C3_ANNC(xp, indent, subdent, &config->annc);
        if (config->set & QDP_CONFIG_DEFINED_BCFG      ) qdpXmlPrint_C3_BCFG(xp, indent, subdent, &config->bcfg);
        if (config->set & QDP_CONFIG_DEFINED_EPCFG     ) qdpXmlPrint_C2_EPCFG(xp, indent, subdent, &config->epcfg);
        for (i = 0; i < QDP_NLP; i++) if (config->dport[i].set & QDP_DPORT_DEFINED_TOKEN) qdpXmlPrintTokens(xp, indent, subdent, i, &config->dport[i].token);
        if (config->set & QDP_CONFIG_DEFINED_WEBPAGE   ) qdpXmlPrintWebpage(xp, indent, subdent, "web_page", &config->webpage);
    XmlPrint(xp, "</Q330_Configuration>\n");
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
 * $Log: print.c,v $
 * Revision 1.9  2016/01/23 00:09:34  dechavez
 * changes related to reworking of QDP_TYPE_C2_EPCFG structure layout
 *
 * Revision 1.8  2016/01/21 17:49:11  dechavez
 * added <balecfg> and <envproc> support
 *
 * Revision 1.7  2016/01/19 23:08:34  dechavez
 * ser3_throt support
 *
 * Revision 1.6  2016/01/19 23:03:54  dechavez
 * added fsdetect (with caveats)
 *
 * Revision 1.5  2012/08/07 21:24:57  dechavez
 * added baler44 support
 *
 * Revision 1.4  2012/06/24 17:57:29  dechavez
 * config->dport[i].token instead of config->token[i]
 *
 * Revision 1.3  2009/10/29 17:22:51  dechavez
 * ignore records that don't have the valid field set
 *
 * Revision 1.2  2009/10/20 22:08:35  dechavez
 * first production release
 *
 */
