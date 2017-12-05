#pragma ident "$Id: soh.c,v 1.2 2016/07/20 23:14:52 dechavez Exp $"
/*======================================================================
 *
 *  Load a TS_SOH state of health reaport
 *
 *====================================================================*/
#define TS_INCLUDE_STATIC_TS_DESCRIPTOR
#include "tristar.h"

#define TS_MINMAX_BATTERY_VOLTAGE_STRING_LEN 19 /* "-32768.00/-32768.00" */
static char *MinMaxBatteryVoltageString(TS_ITEM *min, TS_ITEM *max, char *dest)
{
static char MT_unsafe[TS_MINMAX_BATTERY_VOLTAGE_STRING_LEN+1];

    if (dest == NULL) dest = MT_unsafe;

    if (min->raw == 0xffffffff) {
        sprintf(dest, "undef");
    } else {
        sprintf(dest, "%.2lf", min->value);
    }

    if (max->raw == 0xffffffff) {
        sprintf(dest+strlen(dest), "/undef");
    } else {
        sprintf(dest+strlen(dest), "/%.2lf", max->value);
    }

    return dest;
}

static void PrintAlarmsLDCH(FILE *fp, UINT32 bitfield)
{
    if (bitfield == 0) {
        fprintf(fp, " none");
        return;
    }

    if (bitfield & TS_LDCH_ALARM_THS_DISCONNECTED) fprintf(fp, " THSDisconn");
    if (bitfield & TS_LDCH_ALARM_THS_SHORTED     ) fprintf(fp, " THSShort");
    if (bitfield & TS_LDCH_ALARM_TRISTAR_HOT     ) fprintf(fp, " TriStarHOT");
    if (bitfield & TS_LDCH_ALARM_CURRENT_LIMIT   ) fprintf(fp, " CrntLimit");
    if (bitfield & TS_LDCH_ALARM_CURRENT_OFFSET  ) fprintf(fp, " CrntOffset");
    if (bitfield & TS_LDCH_ALARM_BATTERY_SENSE   ) fprintf(fp, " BattSense");
    if (bitfield & TS_LDCH_ALARM_BATT_SENSE_DISC ) fprintf(fp, " BattSenseDisconn");
    if (bitfield & TS_LDCH_ALARM_UNCALIBRATED    ) fprintf(fp, " Uncalibrated");
    if (bitfield & TS_LDCH_ALARM_RTS_MISWIRE     ) fprintf(fp, " RTSMiswire");
    if (bitfield & TS_LDCH_ALARM_HVD             ) fprintf(fp, " HVD");
    if (bitfield & TS_LDCH_ALARM_HIGH_D          ) fprintf(fp, " HighD");
    if (bitfield & TS_LDCH_ALARM_MISWIRE         ) fprintf(fp, " Miswire");
    if (bitfield & TS_LDCH_ALARM_FET_OPEN        ) fprintf(fp, " FETOpen");
    if (bitfield & TS_LDCH_ALARM_P12             ) fprintf(fp, " P12");
    if (bitfield & TS_LDCH_ALARM_LOAD_DISC       ) fprintf(fp, " LoadDisconn");
    if (bitfield & TS_LDCH_ALARM_ALARM_19        ) fprintf(fp, " Alarm19");
    if (bitfield & TS_LDCH_ALARM_ALARM_20        ) fprintf(fp, " Alarm20");
    if (bitfield & TS_LDCH_ALARM_ALARM_21        ) fprintf(fp, " Alarm21");
    if (bitfield & TS_LDCH_ALARM_ALARM_22        ) fprintf(fp, " Alarm22");
    if (bitfield & TS_LDCH_ALARM_ALARM_23        ) fprintf(fp, " Alarm23");
    if (bitfield & TS_LDCH_ALARM_ALARM_24        ) fprintf(fp, " Alarm24");
}

static void PrintAlarmsMPPT(FILE *fp, UINT32 bitfield)
{
    if (bitfield == 0) {
        fprintf(fp, " none");
        return;
    }

    if (bitfield & TS_MPPT_ALARM_RTS_OPEN                        ) fprintf(fp, " RTSOpen");
    if (bitfield & TS_MPPT_ALARM_RTS_SHORTED                     ) fprintf(fp, " RTSShort");
    if (bitfield & TS_MPPT_ALARM_RTS_DISCONNECTED                ) fprintf(fp, " RTSDisconn");
    if (bitfield & TS_MPPT_ALARM_HEATSINK_SENSOR_OPEN            ) fprintf(fp, " HeatSinkOpen");
    if (bitfield & TS_MPPT_ALARM_HEATSINK_SENSOR_SHORTED         ) fprintf(fp, " HeatSinkShort");
    if (bitfield & TS_MPPT_ALARM_HIGH_TEMP_CURRENT_LIMIT         ) fprintf(fp, " HiTempCrntLim");
    if (bitfield & TS_MPPT_ALARM_CURRENT_LIMIT                   ) fprintf(fp, " CrntLimit");
    if (bitfield & TS_MPPT_ALARM_CURRENT_OFFSET                  ) fprintf(fp, " CrntOffset");
    if (bitfield & TS_MPPT_ALARM_BATTERY_SENSE_OUT_OF_RANGE      ) fprintf(fp, " BattSenseRange");
    if (bitfield & TS_MPPT_ALARM_BATTER_SENSE_DISCONNECTED       ) fprintf(fp, " BattSenseDisconn");
    if (bitfield & TS_MPPT_ALARM_UNCALIBRATED                    ) fprintf(fp, " Uncalibrated");
    if (bitfield & TS_MPPT_ALARM_RTS_MISWIRE                     ) fprintf(fp, " RTSMiswire");
    if (bitfield & TS_MPPT_ALARM_HVD                             ) fprintf(fp, " HVD");
    if (bitfield & TS_MPPT_ALARM_ALARM13                         ) fprintf(fp, " Alarm13");
    if (bitfield & TS_MPPT_ALARM_SYSTEM_MISWIRE                  ) fprintf(fp, " SysMiswire");
    if (bitfield & TS_MPPT_ALARM_MOSFET_OPEN                     ) fprintf(fp, " MOSFETopen");
    if (bitfield & TS_MPPT_ALARM_P12_VOLTAGE_OFF                 ) fprintf(fp, " P12off");
    if (bitfield & TS_MPPT_ALARM_HIGH_INPUT_VOLTAGE_CURRENT_LIMIT) fprintf(fp, " HiInVCrntLim");
    if (bitfield & TS_MPPT_ALARM_ADC_INPUT_MAX                   ) fprintf(fp, " ADCInMax");
    if (bitfield & TS_MPPT_ALARM_CONTROLLER_RESET                ) fprintf(fp, " CtrlReset");
    if (bitfield & TS_MPPT_ALARM_ALARM21                         ) fprintf(fp, " Alarm21");
    if (bitfield & TS_MPPT_ALARM_ALARM22                         ) fprintf(fp, " Alarm24");
    if (bitfield & TS_MPPT_ALARM_ALARM23                         ) fprintf(fp, " Alarm23");
    if (bitfield & TS_MPPT_ALARM_ALARM24                         ) fprintf(fp, " Alarm21");
}

static void PrintFaultsLDCH(FILE *fp, UINT16 bitfield)
{
    if (bitfield == 0) {
        fprintf(fp, " none");
        return;
    }

    if (bitfield & TS_LDCH_FAULT_EXTERNAL_SHORT   ) fprintf(fp, " ExtShort");
    if (bitfield & TS_LDCH_FAULT_OVERCURRENT      ) fprintf(fp, " OverCrnt");
    if (bitfield & TS_LDCH_FAULT_FET_SHORT        ) fprintf(fp, " FETShort");
    if (bitfield & TS_LDCH_FAULT_SOFTWARE         ) fprintf(fp, " SoftwareBug");
    if (bitfield & TS_LDCH_FAULT_HVD              ) fprintf(fp, " HVD");
    if (bitfield & TS_LDCH_FAULT_TRISTAR_HOT      ) fprintf(fp, " TriStarHOT");
    if (bitfield & TS_LDCH_FAULT_DIP_SW_CHANGED   ) fprintf(fp, " DIPswitch");
    if (bitfield & TS_LDCH_FAULT_SETTING_EDIT     ) fprintf(fp, " SettingsEdit");
    if (bitfield & TS_LDCH_FAULT_RESET            ) fprintf(fp, " Reset");
    if (bitfield & TS_LDCH_FAULT_MISWIRE          ) fprintf(fp, " Miswire");
    if (bitfield & TS_LDCH_FAULT_RTS_SHORTED      ) fprintf(fp, " RTSShort");
    if (bitfield & TS_LDCH_FAULT_RTS_DISCONNECTED ) fprintf(fp, " RTSDisconn");
    if (bitfield & TS_LDCH_FAULT_FAULT_12         ) fprintf(fp, " Fault12");
    if (bitfield & TS_LDCH_FAULT_FAULT_13         ) fprintf(fp, " Fault13");
    if (bitfield & TS_LDCH_FAULT_FAULT_14         ) fprintf(fp, " Fault14");
    if (bitfield & TS_LDCH_FAULT_FAULT_15         ) fprintf(fp, " Fault15");
}

static void PrintFaultsMPPT(FILE *fp, UINT16 bitfield)
{
    if (bitfield == 0) {
        fprintf(fp, " none");
        return;
    }

    if (bitfield & TS_MPPT_FAULT_OVERCURRENT           ) fprintf(fp, " OverCrnt");
    if (bitfield & TS_MPPT_FAULT_FETS_SHORTED          ) fprintf(fp, " FETShort");
    if (bitfield & TS_MPPT_FAULT_SOFTWARE              ) fprintf(fp, " SoftwareBug");
    if (bitfield & TS_MPPT_FAULT_BATTERY_HVD           ) fprintf(fp, " BattHVD");
    if (bitfield & TS_MPPT_FAULT_ARRAY_HVD             ) fprintf(fp, " ArrayHVD");
    if (bitfield & TS_MPPT_FAULT_DIP_SW_CHANGED        ) fprintf(fp, " DIPswitch");
    if (bitfield & TS_MPPT_FAULT_SETTING_EDIT          ) fprintf(fp, " SettingsEdit");
    if (bitfield & TS_MPPT_FAULT_RTS_SHORTED           ) fprintf(fp, " RTSshort");
    if (bitfield & TS_MPPT_FAULT_RTS_DISCONNECTED      ) fprintf(fp, " RTSDisconn");
    if (bitfield & TS_MPPT_FAULT_EEPROM_RETRY_LIMIT    ) fprintf(fp, " EEPROMRetryLim");
    if (bitfield & TS_MPPT_FAULT_RESERVED              ) fprintf(fp, " Reserved");
    if (bitfield & TS_MPPT_FAULT_SLAVE_CONTROL_TIMEOUT ) fprintf(fp, " SlvCtrlTO");
    if (bitfield & TS_MPPT_FAULT_FAULT13               ) fprintf(fp, " Fault13");
    if (bitfield & TS_MPPT_FAULT_FAULT14               ) fprintf(fp, " Fault14");
    if (bitfield & TS_MPPT_FAULT_FAULT15               ) fprintf(fp, " Fault15");
    if (bitfield & TS_MPPT_FAULT_FAULT16               ) fprintf(fp, " Fault16");
}

static void PrintTSCH(FILE *fp, char *serialno, TS_SOH_TSCH *tsch)
{
int device;
char dipswitch[TS_DIPSWITCH_STRING_LEN+1];
char version[TS_VERSION_NUMBER_STRING_LEN+1];
char minmax[TS_MINMAX_BATTERY_VOLTAGE_STRING_LEN+1];

    switch (tsch->K_amp60.raw & 0x00FF) {
      case TS_MODEL_45: device = TS_DEVICE_TS45; break;
      case TS_MODEL_60: device = TS_DEVICE_TS60; break;
      default:          device = TS_DEVICE_TSLD; break;
    }

    fprintf(fp, "         Charge controller = %s\n", tristarDeviceString(device));
    fprintf(fp, "             Serial number = %s\n", serialno);
    fprintf(fp, "          Hardware version = %s\n", tristarVersionNumberString(tsch->K_hw_ver.raw, version));
    fprintf(fp, "                     State = %s\n", tristarControlStateStringTSCH(tsch->control_state.raw));
    fprintf(fp, "  Total hours of operation = %d\n", tsch->hourmeter.raw);
    fprintf(fp, "  DIP switches at power up = %s\n", tristarDipSwitchString((UINT8) tsch->dip_switch.raw, dipswitch));
    fprintf(fp, "      Heatsink temperature = %.2lf\n", tsch->T_hs.value);
    fprintf(fp, "Battery temperature sensor = %.2lf\n", tsch->T_batt.value);
    fprintf(fp, "   Min/Max Battery voltage = %s\n", MinMaxBatteryVoltageString(&tsch->min_batt_v, &tsch->max_batt_v, minmax));
    fprintf(fp, "     Battery sense voltage = %.2lf\n", tsch->adc_vs_f.value);
    fprintf(fp, "           Battery voltage = %.2lf\n", tsch->adc_vb_f.value);
    fprintf(fp, "            Charge voltage = %.2lf\n", tsch->adc_vx_f.value);
    fprintf(fp, "            Charge current = %.2lf\n", tsch->adc_ipv_f.value);
    fprintf(fp, "                    Alarms ="); PrintAlarmsLDCH(fp, tsch->alarm.raw); fprintf(fp, "\n");
    fprintf(fp, "                    Faults ="); PrintFaultsLDCH(fp, tsch->fault.raw); fprintf(fp, "\n");
}

static void PrintTSLD(FILE *fp, char *serialno, TS_SOH_TSLD *tsld)
{
int device;
char dipswitch[TS_DIPSWITCH_STRING_LEN+1];
char version[TS_VERSION_NUMBER_STRING_LEN+1];
char minmax[TS_MINMAX_BATTERY_VOLTAGE_STRING_LEN+1];

    switch (tsld->K_amp60.raw & 0x00FF) {
      case TS_MODEL_45: device = TS_DEVICE_TS45; break;
      case TS_MODEL_60: device = TS_DEVICE_TS60; break;
      default:          device = TS_DEVICE_TSLD; break;
    }

    fprintf(fp, "           Load controller = %s\n", tristarDeviceString(device));
    fprintf(fp, "             Serial number = %s\n", serialno);
    fprintf(fp, "          Hardware version = %s\n", tristarVersionNumberString(tsld->K_hw_ver.raw, version));
    fprintf(fp, "                     State = %s\n", tristarControlStateStringTSLD(tsld->control_state.raw));
    fprintf(fp, "  Total hours of operation = %d\n", tsld->hourmeter.raw);
    fprintf(fp, "  DIP switches at power up = %s\n", tristarDipSwitchString((UINT8) tsld->dip_switch.raw, dipswitch));
    fprintf(fp, "      Heatsink temperature = %.2lf\n", tsld->T_hs.value);
    fprintf(fp, "   Min/Max Battery voltage = %s\n", MinMaxBatteryVoltageString(&tsld->min_batt_v, &tsld->max_batt_v, minmax));
    fprintf(fp, "              Load voltage = %.2lf\n", tsld->adc_vx_f.value);
    fprintf(fp, "              Load current = %.2lf\n", tsld->adc_iload_f.value);
    fprintf(fp, "                    Alarms ="); PrintAlarmsLDCH(fp, tsld->alarm.raw); fprintf(fp, "\n");
    fprintf(fp, "                    Faults ="); PrintFaultsLDCH(fp, tsld->fault.raw); fprintf(fp, "\n");
}

static void PrintMPPT(FILE *fp, char *serialno, TS_SOH_MPPT *mppt)
{
int device;
char dipswitch[TS_DIPSWITCH_STRING_LEN+1];
char version[TS_VERSION_NUMBER_STRING_LEN+1];
char minmax[TS_MINMAX_BATTERY_VOLTAGE_STRING_LEN+1];

    switch (mppt->Emodel.raw) {
      case TS_MPPT_EMODEL_45: device = TS_DEVICE_MP45; break;
      case TS_MPPT_EMODEL_60: device = TS_DEVICE_MP60; break;
      default:                device = TS_DEVICE_MPPT; break;
    }

    fprintf(fp, "         Charge controller = %s\n", tristarDeviceString(device));
    fprintf(fp, "             Serial number = %s\n", serialno);
    fprintf(fp, "          Software version = %x\n", mppt->ver_sw.raw);
    fprintf(fp, "          Hardware version = %s\n", tristarVersionNumberString(mppt->Ehw_version.raw, version));
    fprintf(fp, "                     State = %s\n", tristarControlStateStringMPPT(mppt->charge_state.raw));
    fprintf(fp, "  Total hours of operation = %d\n", mppt->hourmeter.raw);
    fprintf(fp, "  DIP switches at power up = %s\n", tristarDipSwitchString((UINT8) mppt->dip_all.raw, dipswitch));
    fprintf(fp, "      Heatsink temperature = %.2lf\n", mppt->T_hs.value);
    fprintf(fp, "Battery temperature sensor = %.2lf\n", mppt->T_rts.value);
    fprintf(fp, "   Min/Max Battery voltage = %s\n", MinMaxBatteryVoltageString(&mppt->vb_min, &mppt->vb_max, minmax));
    fprintf(fp, "     Battery sense voltage = %.2lf\n", mppt->adc_vbs_f.value);
    fprintf(fp, "           Battery voltage = %.2lf\n", mppt->adc_vbterm_f.value);
    fprintf(fp, "            Charge voltage = %.2lf\n", mppt->adc_va_f.value);
    fprintf(fp, "            Charge current = %.2lf\n", mppt->adc_ib_f_shadow.value);
    fprintf(fp, "                    Alarms ="); PrintAlarmsMPPT(fp, mppt->alarm.raw); fprintf(fp, "\n");
    fprintf(fp, "                    Faults ="); PrintFaultsMPPT(fp, mppt->fault.raw); fprintf(fp, "\n");

}

void tristarPrintSOH(FILE *fp, TS_SOH *soh)
{
int device;
static char *fid = "tristarPrintSOH";

    if (fp == NULL || soh == NULL) return; 

    switch (soh->which) {
      case TS_DEVICE_TSCH: PrintTSCH(fp, soh->serialno, &soh->device.tsch); break;
      case TS_DEVICE_TSLD: PrintTSLD(fp, soh->serialno, &soh->device.tsld); break;
      case TS_DEVICE_MPPT: PrintMPPT(fp, soh->serialno, &soh->device.mppt); break;
      default: fprintf(fp, "%s: WFT? unrecognized soh->which = %d\n", fid, soh->which);
    }
}

static int ReadTSCH(TS *handle, TS_SOH_TSCH *tsch)
{
int result;

    if ((result = tristarReadItem(handle, &TS_DESCRIPTOR.TS_K_amp60,       &tsch->K_amp60      )) != TS_READ_OK) return result;
    if ((result = tristarReadItem(handle, &TS_DESCRIPTOR.TS_K_hw_ver,      &tsch->K_hw_ver     )) != TS_READ_OK) return result;
    if ((result = tristarReadItem(handle, &TS_DESCRIPTOR.TS_control_state, &tsch->control_state)) != TS_READ_OK) return result;
    if ((result = tristarReadItem(handle, &TS_DESCRIPTOR.TS_hourmeter,     &tsch->hourmeter    )) != TS_READ_OK) return result;
    if ((result = tristarReadItem(handle, &TS_DESCRIPTOR.TS_dip_switch,    &tsch->dip_switch   )) != TS_READ_OK) return result;
    if ((result = tristarReadItem(handle, &TS_DESCRIPTOR.TS_T_hs,          &tsch->T_hs         )) != TS_READ_OK) return result;
    if ((result = tristarReadItem(handle, &TS_DESCRIPTOR.TS_T_batt,        &tsch->T_batt       )) != TS_READ_OK) return result;
    if ((result = tristarReadItem(handle, &TS_DESCRIPTOR.TS_min_batt_v,    &tsch->min_batt_v   )) != TS_READ_OK) return result;
    if ((result = tristarReadItem(handle, &TS_DESCRIPTOR.TS_max_batt_v,    &tsch->max_batt_v   )) != TS_READ_OK) return result;
    if ((result = tristarReadItem(handle, &TS_DESCRIPTOR.TS_adc_vb_f,      &tsch->adc_vb_f     )) != TS_READ_OK) return result;
    if ((result = tristarReadItem(handle, &TS_DESCRIPTOR.TS_adc_vs_f,      &tsch->adc_vs_f     )) != TS_READ_OK) return result;
    if ((result = tristarReadItem(handle, &TS_DESCRIPTOR.TS_adc_vx_f,      &tsch->adc_vx_f     )) != TS_READ_OK) return result;
    if ((result = tristarReadItem(handle, &TS_DESCRIPTOR.TS_adc_ipv_f,     &tsch->adc_ipv_f    )) != TS_READ_OK) return result;
    if ((result = tristarReadItem(handle, &TS_DESCRIPTOR.TS_alarm,         &tsch->alarm        )) != TS_READ_OK) return result;
    if ((result = tristarReadItem(handle, &TS_DESCRIPTOR.TS_fault,         &tsch->fault        )) != TS_READ_OK) return result;

    return TS_READ_OK;
}

static int ReadTSLD(TS *handle, TS_SOH_TSLD *tsld)
{
int result;

    if ((result = tristarReadItem(handle, &TS_DESCRIPTOR.TS_K_amp60,       &tsld->K_amp60      )) != TS_READ_OK) return result;
    if ((result = tristarReadItem(handle, &TS_DESCRIPTOR.TS_K_hw_ver,      &tsld->K_hw_ver     )) != TS_READ_OK) return result;
    if ((result = tristarReadItem(handle, &TS_DESCRIPTOR.TS_control_state, &tsld->control_state)) != TS_READ_OK) return result;
    if ((result = tristarReadItem(handle, &TS_DESCRIPTOR.TS_hourmeter,     &tsld->hourmeter    )) != TS_READ_OK) return result;
    if ((result = tristarReadItem(handle, &TS_DESCRIPTOR.TS_dip_switch,    &tsld->dip_switch   )) != TS_READ_OK) return result;
    if ((result = tristarReadItem(handle, &TS_DESCRIPTOR.TS_T_hs,          &tsld->T_hs         )) != TS_READ_OK) return result;
    if ((result = tristarReadItem(handle, &TS_DESCRIPTOR.TS_min_batt_v,    &tsld->min_batt_v   )) != TS_READ_OK) return result;
    if ((result = tristarReadItem(handle, &TS_DESCRIPTOR.TS_max_batt_v,    &tsld->max_batt_v   )) != TS_READ_OK) return result;
    if ((result = tristarReadItem(handle, &TS_DESCRIPTOR.TS_adc_vx_f,      &tsld->adc_vx_f     )) != TS_READ_OK) return result;
    if ((result = tristarReadItem(handle, &TS_DESCRIPTOR.TS_adc_iload_f,   &tsld->adc_iload_f  )) != TS_READ_OK) return result;
    if ((result = tristarReadItem(handle, &TS_DESCRIPTOR.TS_alarm,         &tsld->alarm        )) != TS_READ_OK) return result;
    if ((result = tristarReadItem(handle, &TS_DESCRIPTOR.TS_fault,         &tsld->fault        )) != TS_READ_OK) return result;

    return TS_READ_OK;
}

static int ReadMPPT(TS *handle, TS_SOH_MPPT *mppt)
{
int result;

    if ((result = tristarReadItem(handle, &TS_DESCRIPTOR.MP_ver_sw,          &mppt->ver_sw         )) != TS_READ_OK) return result;
    if ((result = tristarReadItem(handle, &TS_DESCRIPTOR.MP_adc_vbterm_f,    &mppt->adc_vbterm_f   )) != TS_READ_OK) return result;
    if ((result = tristarReadItem(handle, &TS_DESCRIPTOR.MP_adc_vbs_f,       &mppt->adc_vbs_f      )) != TS_READ_OK) return result;
    if ((result = tristarReadItem(handle, &TS_DESCRIPTOR.MP_adc_va_f,        &mppt->adc_va_f       )) != TS_READ_OK) return result;
    if ((result = tristarReadItem(handle, &TS_DESCRIPTOR.MP_adc_ib_f_shadow, &mppt->adc_ib_f_shadow)) != TS_READ_OK) return result;
    if ((result = tristarReadItem(handle, &TS_DESCRIPTOR.MP_T_hs,            &mppt->T_hs           )) != TS_READ_OK) return result;
    if ((result = tristarReadItem(handle, &TS_DESCRIPTOR.MP_T_rts,           &mppt->T_rts          )) != TS_READ_OK) return result;
    if ((result = tristarReadItem(handle, &TS_DESCRIPTOR.MP_vb_min,          &mppt->vb_min         )) != TS_READ_OK) return result;
    if ((result = tristarReadItem(handle, &TS_DESCRIPTOR.MP_vb_max,          &mppt->vb_max         )) != TS_READ_OK) return result;
    if ((result = tristarReadItem(handle, &TS_DESCRIPTOR.MP_hourmeter,       &mppt->hourmeter      )) != TS_READ_OK) return result;
    if ((result = tristarReadItem(handle, &TS_DESCRIPTOR.MP_fault,           &mppt->fault          )) != TS_READ_OK) return result;
    if ((result = tristarReadItem(handle, &TS_DESCRIPTOR.MP_alarm,           &mppt->alarm          )) != TS_READ_OK) return result;
    if ((result = tristarReadItem(handle, &TS_DESCRIPTOR.MP_dip_all,         &mppt->dip_all        )) != TS_READ_OK) return result;
    if ((result = tristarReadItem(handle, &TS_DESCRIPTOR.MP_charge_state,    &mppt->charge_state   )) != TS_READ_OK) return result;
    if ((result = tristarReadItem(handle, &TS_DESCRIPTOR.MP_Emodel,          &mppt->Emodel         )) != TS_READ_OK) return result;
    if ((result = tristarReadItem(handle, &TS_DESCRIPTOR.MP_Ehw_version,     &mppt->Ehw_version    )) != TS_READ_OK) return result;

    return TS_READ_OK;
}

int tristarReadSOH(TS *handle, TS_SOH *soh)
{
    if (handle == NULL || soh == NULL) return TS_READ_EINVAL;

    soh->which = handle->ident.device;
    strcpy(soh->serialno, handle->ident.serialno);

    switch (soh->which) {
      case TS_DEVICE_TSCH: return ReadTSCH(handle, &soh->device.tsch);
      case TS_DEVICE_TSLD: return ReadTSLD(handle, &soh->device.tsld);
      case TS_DEVICE_MPPT: return ReadMPPT(handle, &soh->device.mppt);
    }

    return TS_READ_EINVAL;
}


/*-----------------------------------------------------------------------+
 |                                                                       |
 | Copyright (C) 2016 Regents of the University of California            |
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
 * $Log: soh.c,v $
 * Revision 1.2  2016/07/20 23:14:52  dechavez
 * replaced the 'Device =' strings in the various tristarPrintSOH:PrintXXXX() functions
 * with device specific 'Charge controller =' or 'Load controller =' as appropriate
 *
 * Revision 1.1  2016/06/06 21:51:36  dechavez
 * initial release
 *
 */
