#pragma ident "$Id: string.c,v 1.1 2016/06/06 21:51:36 dechavez Exp $"
/*======================================================================
 *
 * String conversions
 *
 *====================================================================*/
#include "tristar.h"

static TS_TEXT_MAP SlaveMap[] = {
    {"charge", TS_SLAVE_CHRG}, 
    {  "load", TS_SLAVE_LOAD},
    {NULL, 0}
};

static TS_TEXT_MAP DeviceMap[] = {
    {     "TS-45", TS_DEVICE_TS45}, 
    {     "TS-60", TS_DEVICE_TS60},
    {"TS-MPPT-45", TS_DEVICE_MP45}, 
    {"TS-MPPT-60", TS_DEVICE_MP60}, 
    {   "TS-CHRG", TS_DEVICE_TSCH}, 
    {   "TS-LOAD", TS_DEVICE_TSLD}, 
    {   "TS-MPPT", TS_DEVICE_MPPT}, 
    {NULL, 0}
};

static TS_TEXT_MAP TSCH_StateMap[] = {
    {      "start", TS_TSCH_STATE_START      },
    {"night check", TS_TSCH_STATE_NIGHT_CHECK},
    { "disconnect", TS_TSCH_STATE_DISCONNECT },
    {      "night", TS_TSCH_STATE_NIGHT      },
    {      "fault", TS_TSCH_STATE_FAULT      },
    {       "bulk", TS_TSCH_STATE_BULK       },
    {        "pwm", TS_TSCH_STATE_PWM        },
    {      "float", TS_TSCH_STATE_FLOAT      },
    {   "equalize", TS_TSCH_STATE_EQUALIZE   },
    {NULL, 0}
};

static TS_TEXT_MAP TSLD_StateMap[] = {
    {                 "start", TS_TSLD_STATE_START       },
    {                "normal", TS_TSLD_STATE_NORMAL      },
    {              "LVD warn", TS_TSLD_STATE_LVD_WARN    },
    {"low voltage disconnect", TS_TSLD_STATE_LVD         },
    {                 "fault", TS_TSLD_STATE_FAULT       },
    {            "disconnect", TS_TSLD_STATE_DISCONNECT  },
    {                   "off", TS_TSLD_STATE_NORMAL_OFF  },
    {          "override LVD", TS_TSLD_STATE_OVERRIDE_LVD},
    {NULL, 0}
};

static TS_TEXT_MAP MPPT_StateMap[] = {
    {      "start", TS_MPPT_STATE_START      },
    {"night check", TS_MPPT_STATE_NIGHT_CHECK},
    { "disconnect", TS_MPPT_STATE_DISCONNECT },
    {      "night", TS_MPPT_STATE_NIGHT      },
    {      "fault", TS_MPPT_STATE_FAULT      },
    {       "mppt", TS_MPPT_STATE_MPPT       },
    { "absorption", TS_MPPT_STATE_ABSORPTION },
    {      "float", TS_MPPT_STATE_FLOAT      },
    {   "equalize", TS_MPPT_STATE_EQUALIZE   },
    {      "slave", TS_MPPT_STATE_SLAVE      },
    {NULL, 0}
};

static TS_TEXT_MAP LDCH_AlarmMap[] = {
    {                "RTS open", TS_LDCH_ALARM_RTS_OPEN        },
    {             "RTS shorted", TS_LDCH_ALARM_RTS_SHORTED     },
    {        "RTS disconnected", TS_LDCH_ALARM_RTS_DISCONNECTED},
    {        "THS disconnected", TS_LDCH_ALARM_THS_DISCONNECTED},
    {             "THS shorted", TS_LDCH_ALARM_THS_SHORTED     },
    {             "Tristar hot", TS_LDCH_ALARM_TRISTAR_HOT     },
    {           "Current limit", TS_LDCH_ALARM_CURRENT_LIMIT   },
    {          "Current offset", TS_LDCH_ALARM_CURRENT_OFFSET  },
    {           "Battery sense", TS_LDCH_ALARM_BATTERY_SENSE   },
    {"Battery sense disconnect", TS_LDCH_ALARM_BATT_SENSE_DISC },
    {            "Uncalibrated", TS_LDCH_ALARM_UNCALIBRATED    },
    {             "RTS_MISWIRE", TS_LDCH_ALARM_RTS_MISWIRE     },
    { "High voltage disconnect", TS_LDCH_ALARM_HVD             },
    {                  "High d", TS_LDCH_ALARM_HIGH_D          },
    {                 "miswire", TS_LDCH_ALARM_MISWIRE         },
    {                "FET open", TS_LDCH_ALARM_FET_OPEN        },
    {                     "P12", TS_LDCH_ALARM_P12             },
    {         "Load disconnect", TS_LDCH_ALARM_LOAD_DISC       },
    {                "Alarm 19", TS_LDCH_ALARM_ALARM_19        },
    {                "Alarm 20", TS_LDCH_ALARM_ALARM_20        },
    {                "Alarm 21", TS_LDCH_ALARM_ALARM_21        },
    {                "Alarm 22", TS_LDCH_ALARM_ALARM_22        },
    {                "Alarm 23", TS_LDCH_ALARM_ALARM_23        },
    {                "Alarm 24", TS_LDCH_ALARM_ALARM_24        },
    {NULL, 0}
};

static TS_TEXT_MAP MPPT_AlarmMap[] = {
    {                        "RTS open", TS_MPPT_ALARM_RTS_OPEN                        },
    {                     "RTS shorted", TS_MPPT_ALARM_RTS_SHORTED                     },
    {                "RTS disconnected", TS_MPPT_ALARM_RTS_DISCONNECTED                },
    {            "Heatsink sensor open", TS_MPPT_ALARM_HEATSINK_SENSOR_OPEN            },
    {         "Heatsink sensor shorted", TS_MPPT_ALARM_HEATSINK_SENSOR_SHORTED         },
    {         "High temp current limit", TS_MPPT_ALARM_HIGH_TEMP_CURRENT_LIMIT         },
    {                   "Current limit", TS_MPPT_ALARM_CURRENT_LIMIT                   },
    {                  "Current offset", TS_MPPT_ALARM_CURRENT_OFFSET                  },
    {      "Battery sense out of range", TS_MPPT_ALARM_BATTERY_SENSE_OUT_OF_RANGE      },
    {      "Battery sense disconnected", TS_MPPT_ALARM_BATTER_SENSE_DISCONNECTED       },
    {                    "Uncalibrated", TS_MPPT_ALARM_UNCALIBRATED                    },
    {                     "RTS miswire", TS_MPPT_ALARM_RTS_MISWIRE                     },
    {         "High voltage disconnect", TS_MPPT_ALARM_HVD                             },
    {            "Undefined (Alarm 13)", TS_MPPT_ALARM_ALARM13                         },
    {                  "System miswire", TS_MPPT_ALARM_SYSTEM_MISWIRE                  },
    {                     "MOSFET open", TS_MPPT_ALARM_MOSFET_OPEN                     },
    {                     "Voltage off", TS_MPPT_ALARM_P12_VOLTAGE_OFF                 },
    {"High input voltage current limit", TS_MPPT_ALARM_HIGH_INPUT_VOLTAGE_CURRENT_LIMIT},
    {                   "ADC input max", TS_MPPT_ALARM_ADC_INPUT_MAX                   },
    {                "Controller reset", TS_MPPT_ALARM_CONTROLLER_RESET                },
    {                        "Alarm 21", TS_MPPT_ALARM_ALARM21                         },
    {                        "Alarm 22", TS_MPPT_ALARM_ALARM22                         },
    {                        "Alarm 23", TS_MPPT_ALARM_ALARM23                         },
    {                        "Alarm 24", TS_MPPT_ALARM_ALARM24                         },
    {NULL, 0}
};

static TS_TEXT_MAP LDCH_FaultMap[] = {
    {         "External short", TS_LDCH_FAULT_EXTERNAL_SHORT  },
    {            "Overcurrent", TS_LDCH_FAULT_OVERCURRENT     },
    {              "FET short", TS_LDCH_FAULT_FET_SHORT       },
    {         "Software error", TS_LDCH_FAULT_SOFTWARE        },
    {"High voltage disconnect", TS_LDCH_FAULT_HVD             },
    {            "TriStar hot", TS_LDCH_FAULT_TRISTAR_HOT     },
    {     "DIP switch changed", TS_LDCH_FAULT_DIP_SW_CHANGED  },
    {        "Settings edited", TS_LDCH_FAULT_SETTING_EDIT    },
    {                 "Reset?", TS_LDCH_FAULT_RESET           },
    {                "Miswire", TS_LDCH_FAULT_MISWIRE         },
    {            "RTS shorted", TS_LDCH_FAULT_RTS_SHORTED     },
    {       "RTS disconnected", TS_LDCH_FAULT_RTS_DISCONNECTED},
    {               "Fault 12", TS_LDCH_FAULT_FAULT_12        },
    {               "Fault 13", TS_LDCH_FAULT_FAULT_13        },
    {               "Fault 14", TS_LDCH_FAULT_FAULT_14        },
    {               "Fault 15", TS_LDCH_FAULT_FAULT_15        },
    {NULL, 0}
};

static TS_TEXT_MAP MPPT_FaultMap[] = {
    {                    "Overcurrent", TS_MPPT_FAULT_OVERCURRENT          },
    {                   "FETS shorted", TS_MPPT_FAULT_FETS_SHORTED         },
    {                 "Software error", TS_MPPT_FAULT_SOFTWARE             },
    {"Battery high voltage disconnect", TS_MPPT_FAULT_BATTERY_HVD          },
    {  "Array high voltage disconnect", TS_MPPT_FAULT_ARRAY_HVD            },
    {             "DIP switch changed", TS_MPPT_FAULT_DIP_SW_CHANGED       },
    {                "Settings edited", TS_MPPT_FAULT_SETTING_EDIT         },
    {                    "RTS shorted", TS_MPPT_FAULT_RTS_SHORTED          },
    {               "RTS disconnected", TS_MPPT_FAULT_RTS_DISCONNECTED     },
    {             "EEPROM retry limit", TS_MPPT_FAULT_EEPROM_RETRY_LIMIT   },
    {                       "Reserved", TS_MPPT_FAULT_RESERVED             },
    {          "Slave control timeout", TS_MPPT_FAULT_SLAVE_CONTROL_TIMEOUT},
    {                       "Fault 13", TS_MPPT_FAULT_FAULT13              },
    {                       "Fault 14", TS_MPPT_FAULT_FAULT14              },
    {                       "Fault 15", TS_MPPT_FAULT_FAULT15              },
    {                       "Fault 16", TS_MPPT_FAULT_FAULT16              },
    {NULL, 0}
};

static TS_TEXT_MAP ConnectErrorMap[] = {
    {     "can't identify device type", TS_CONNECT_UNKOWN_DEVICE       },
    {  "can't read MPPT scale factors", TS_CONNECT_HANDSHAKE_FAIL      },
    {        "modbus_new_tcp() failed", TS_CONNECT_MODBUS_NEW_ERROR    },
    {        "modbus_connect() failed", TS_CONNECT_FAIL                },
    {"MPPT device found at LOAD slave", TS_CONNECT_MISWIRE             },
    {                  "illegal slave", TS_CONNECT_ILLEGAL_SLAVE       },
    {               "illegal argument", TS_CONNECT_EINVAL              },
    {                       "no error", TS_CONNECT_OK                  },
    {NULL, 0}
};

static TS_TEXT_MAP CommandErrorMap[] = {
    {      "unrecognized device", TS_COMMAND_UNKNOWN_DEVICE},
    {          "illegal command", TS_COMMAND_UNSUPPORTED   },
    {"modbus_write_bit() failed", TS_COMMAND_FAIL          },
    {         "illegal argument", TS_COMMAND_EINVAL        },
    {                 "no error", TS_COMMAND_OK            },
    {NULL, 0}
};

static char *LocateString(int code, TS_TEXT_MAP *map, char *DefaultMessage)
{
int i;

    for (i = 0; map[i].text != NULL; i++) if (map[i].code == code) return map[i].text;
    return DefaultMessage;
}

static int LocateCode(char *string, TS_TEXT_MAP *map, int DefaultCode)
{
int i;

    for (i = 0; map[i].text != NULL; i++) if (strcasecmp(map[i].text, string) == 0) return map[i].code;
    return DefaultCode;
}

char *tristarSlaveString(int code)
{
static char *DefaultMessage = "<unknown>";

    return LocateString(code, SlaveMap, DefaultMessage);
}

char *tristarDeviceString(int code)
{
static char *DefaultMessage = "<unknown>";

    return LocateString(code, DeviceMap, DefaultMessage);
}

char *tristarControlStateStringTSCH(int code)
{
static char *DefaultMessage = "<unknown>";

    return LocateString(code, TSCH_StateMap, DefaultMessage);
}

char *tristarControlStateStringTSLD(int code)
{
static char *DefaultMessage = "<unknown>";

    return LocateString(code, TSLD_StateMap, DefaultMessage);
}

char *tristarControlStateStringMPPT(int code)
{
static char *DefaultMessage = "<unknown>";

    return LocateString(code, MPPT_StateMap, DefaultMessage);
}

static char *AlarmStringLDCH(int code)
{
static char *DefaultMessage = "<unknown>";

    return LocateString(code, LDCH_AlarmMap, DefaultMessage);
}

char *tristarAlarmStringTSCH(int code)
{
    return AlarmStringLDCH(code);
}

char *tristarAlarmStringTSLD(int code)
{
    return AlarmStringLDCH(code);
}

char *tristarAlarmStringMPPT(int code)
{
static char *DefaultMessage = "<unknown>";

    return LocateString(code, MPPT_AlarmMap, DefaultMessage);
}

static char *FaultStringLDCH(int code)
{
static char *DefaultMessage = "<unknown>";

    return LocateString(code, LDCH_FaultMap, DefaultMessage);
}

char *tristarFaultStringTSCH(int code)
{
    return FaultStringLDCH(code);
}

char *tristarFaultStringTSLD(int code)
{
    return FaultStringLDCH(code);
}

char *tristarFaultStringMPPT(int code)
{
static char *DefaultMessage = "<unknown>";

    return LocateString(code, MPPT_FaultMap, DefaultMessage);
}

char *tristarConnectErrorString(int code)
{
static char *DefaultMessage = "<unknown>";

    return LocateString(code, ConnectErrorMap, DefaultMessage);
}

char *tristarCommandErrorString(int code)
{
static char *DefaultMessage = "<unknown>";

    return LocateString(code, CommandErrorMap, DefaultMessage);
}

char *tristarDipSwitchString(UINT8 bitfield, char *dest)
{
static char *DefaultMessage = "n/a";
static char MTunsafe[TS_DIPSWITCH_STRING_LEN+1];

    if (dest == NULL) dest = MTunsafe;

    dest[0] = 0;
    sprintf(dest+strlen(dest), "1=%s", bitfield & 0x01 ? "ON" : "off"); strcat(dest, " ");
    sprintf(dest+strlen(dest), "2=%s", bitfield & 0x02 ? "ON" : "off"); strcat(dest, " ");
    sprintf(dest+strlen(dest), "3=%s", bitfield & 0x04 ? "ON" : "off"); strcat(dest, " ");
    sprintf(dest+strlen(dest), "4=%s", bitfield & 0x08 ? "ON" : "off"); strcat(dest, " ");
    sprintf(dest+strlen(dest), "5=%s", bitfield & 0x10 ? "ON" : "off"); strcat(dest, " ");
    sprintf(dest+strlen(dest), "6=%s", bitfield & 0x20 ? "ON" : "off"); strcat(dest, " ");
    sprintf(dest+strlen(dest), "7=%s", bitfield & 0x40 ? "ON" : "off"); strcat(dest, " ");
    sprintf(dest+strlen(dest), "8=%s", bitfield & 0x80 ? "ON" : "off"); strcat(dest, " ");

    return dest;
}

char *tristarVersionNumberString(UINT16 version, char *dest)
{
UINT8 major, minor;
static char MTunsafe[TS_VERSION_NUMBER_STRING_LEN+1];

    if (dest == NULL) dest = MTunsafe;

    major = version >> 8;
    minor = version & 0x00ff;

    sprintf(dest, "%x.%02x", major, minor);

    return dest;
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
 * $Log: string.c,v $
 * Revision 1.1  2016/06/06 21:51:36  dechavez
 * initial release
 *
 */
