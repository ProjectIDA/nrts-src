#pragma ident "$Id: q330.h,v 1.33 2017/01/26 19:47:27 dechavez Exp $"
/*======================================================================
 *
 *  Q330 utilities (NOT QDP communication stuff)
 *
 *====================================================================*/
#ifndef q330_h_included
#define q330_h_included

#include "platform.h"
#include "isi.h" /* for string lengths */
#include "detect.h"
#include "util.h"
#include "logio.h"
#include "qdp.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Constants */

#define Q330_ROOT_ENV_STRING  "Q330_ROOT"
#ifdef DARWIN
#define Q330_DEFAULT_ROOT     "/Users/nrts/etc"
#else
#define Q330_DEFAULT_ROOT     "/usr/nrts/etc"
#endif
#define Q330_DEFAULT_WATCHDOG 10000 /* 10 seconds */

#define Q330_NAME_SUBDIR      "q330"
#define Q330_NAME_ADDR        "q330.cfg"
#define Q330_NAME_AUTH        "auth"
#define Q330_NAME_SENSOR      "sensor"
#define Q330_NAME_DETECTOR    "detector"
#define Q330_NAME_CALIB       "calib"
#define Q330_NAME_AUTOCAL     "autocal"
#define Q330_NAME_LCQ         "lcq"

#define Q330_CAL_START_NOW                  0
#define Q330_CAL_SHORTCUT_CHANS_BITMAP   0x07
#define Q330_CAL_SHORTCUT_MONITOR_BITMAP 0x08
#define Q330_MAX_CAL_WAVEFORM               4
#define Q330_MAX_CAL_DURATION           16383
#define Q330_MAX_CAL_AMPLTUDE               0
#define Q330_MAX_CAL_DIVISOR              255

#define Q330_SENSOR_A_BITMAP 0x07 /* 0000 0111 */
#define Q330_SENSOR_B_BITMAP 0x38 /* 0011 1000 */

#define Q330_SENSOR_A_CALMON 0x08 /* 0000 1000 */
#define Q330_SENSOR_B_CALMON 0x01 /* 0000 0001 */

/* Various return codes */

#define Q330_ERROR        -1
#define Q330_NO_ERROR      0
#define Q330_CAL_OFF       1
#define Q330_CAL_ENABLE    2
#define Q330_CAL_ACTIVE    3
#define Q330_CAL_OTHER     4
#define Q330_PORT_BUSY     5
#define Q330_BAD_CFG       6
#define Q330_DUP_ADDR      7
#define Q330_BAD_ADDR      8
#define Q330_IOE_ADDR      9
#define Q330_DUP_AUTH     10
#define Q330_BAD_AUTH     11
#define Q330_IOE_AUTH     12
#define Q330_BAD_INPUT    13
#define Q330_DUP_SENSOR   14
#define Q330_BAD_SENSOR   15
#define Q330_IOE_SENSOR   16
#define Q330_DUP_CALIB    17
#define Q330_BAD_CALIB    18
#define Q330_IOE_CALIB    19
#define Q330_DUP_DETECTOR 20
#define Q330_BAD_DETECTOR 21
#define Q330_IOE_DETECTOR 22
#define Q330_SC_MATCH     23
#define Q330_SC_MISMATCH  24
#define Q330_NO_AUTH      25
#define Q330_DUP_LCQ      26
#define Q330_BAD_LCQ      27
#define Q330_IOE_LCQ      28
#define Q330_NO_LCQMEM    29

/* default config values */

#define Q330_DEFAULT_C1_SPP_MAX_MAIN_CURRENT   350
#define Q330_DEFAULT_C1_SPP_MIN_OFF_TIME        60
#define Q330_DEFAULT_C1_SPP_MIN_PS_VOLTAGE      60
#define Q330_DEFAULT_C1_SPP_MAX_ANTENNA_CURRENT 34
#define Q330_DEFAULT_C1_SPP_MIN_TEMP           -40
#define Q330_DEFAULT_C1_SPP_MAX_TEMP            65
#define Q330_DEFAULT_C1_SPP_TEMP_HYSTERESIS      5
#define Q330_DEFAULT_C1_SPP_VOLD_HYSTERESIS     13
#define Q330_DEFAULT_C1_SPP_DEFAULT_VCO       2048

#define Q330_DEFAULT_TYPE_C1_SPP {           \
    Q330_DEFAULT_C1_SPP_MAX_MAIN_CURRENT,    \
    Q330_DEFAULT_C1_SPP_MIN_OFF_TIME,        \
    Q330_DEFAULT_C1_SPP_MIN_PS_VOLTAGE,      \
    Q330_DEFAULT_C1_SPP_MAX_ANTENNA_CURRENT, \
    Q330_DEFAULT_C1_SPP_MIN_TEMP,            \
    Q330_DEFAULT_C1_SPP_MAX_TEMP,            \
    Q330_DEFAULT_C1_SPP_TEMP_HYSTERESIS,     \
    Q330_DEFAULT_C1_SPP_VOLD_HYSTERESIS,     \
    Q330_DEFAULT_C1_SPP_DEFAULT_VCO          \
}

#define Q330_DEFAULT_C2_GPS_MODE_INTERNAL   8
#define Q330_DEFAULT_C2_GPS_MODE_EXTERNAL   1
#define Q330_DEFAULT_C2_GPS_FLAGS           0
#define Q330_DEFAULT_C2_GPS_OFF_TIME      180
#define Q330_DEFAULT_C2_GPS_RESYNC          0
#define Q330_DEFAULT_C2_GPS_MAX_ON        120
#define Q330_DEFAULT_C2_GPS_LOCK_USEC       5
#define Q330_DEFAULT_C2_GPS_INTERVAL       10
#define Q330_DEFAULT_C2_GPS_INITIAL_PLL     3
#define Q330_DEFAULT_C2_GPS_PFRAC         150.000
#define Q330_DEFAULT_C2_GPS_VCO_SLOPE      -0.175
#define Q330_DEFAULT_C2_GPS_VCO_INTERCEPT   7.110
#define Q330_DEFAULT_C2_GPS_MAX_IKM_RMS     0.250
#define Q330_DEFAULT_C2_GPS_IKM_WEIGHT      0.250
#define Q330_DEFAULT_C2_GPS_KM_WEIGHT       0.120
#define Q330_DEFAULT_C2_GPS_BEST_WEIGHT     0.100
#define Q330_DEFAULT_C2_GPS_KM_DELTA      600.000

#define Q330_DEFAULT_TYPE_C2_GPS_INTERNAL { \
    Q330_DEFAULT_C2_GPS_MODE_INTERNAL,      \
    Q330_DEFAULT_C2_GPS_FLAGS,              \
    Q330_DEFAULT_C2_GPS_OFF_TIME,           \
    Q330_DEFAULT_C2_GPS_RESYNC,             \
    Q330_DEFAULT_C2_GPS_MAX_ON,             \
    Q330_DEFAULT_C2_GPS_LOCK_USEC,          \
    Q330_DEFAULT_C2_GPS_INTERVAL,           \
    Q330_DEFAULT_C2_GPS_INITIAL_PLL,        \
    Q330_DEFAULT_C2_GPS_PFRAC,              \
    Q330_DEFAULT_C2_GPS_VCO_SLOPE,          \
    Q330_DEFAULT_C2_GPS_VCO_INTERCEPT,      \
    Q330_DEFAULT_C2_GPS_MAX_IKM_RMS,        \
    Q330_DEFAULT_C2_GPS_IKM_WEIGHT,         \
    Q330_DEFAULT_C2_GPS_KM_WEIGHT,          \
    Q330_DEFAULT_C2_GPS_BEST_WEIGHT,        \
    Q330_DEFAULT_C2_GPS_KM_DELTA            \
}

#define Q330_DEFAULT_TYPE_C2_GPS_EXTERNAL { \
    Q330_DEFAULT_C2_GPS_MODE_EXTERNAL,      \
    Q330_DEFAULT_C2_GPS_FLAGS,              \
    Q330_DEFAULT_C2_GPS_OFF_TIME,           \
    Q330_DEFAULT_C2_GPS_RESYNC,             \
    Q330_DEFAULT_C2_GPS_MAX_ON,             \
    Q330_DEFAULT_C2_GPS_LOCK_USEC,          \
    Q330_DEFAULT_C2_GPS_INTERVAL,           \
    Q330_DEFAULT_C2_GPS_INITIAL_PLL,        \
    Q330_DEFAULT_C2_GPS_PFRAC,              \
    Q330_DEFAULT_C2_GPS_VCO_SLOPE,          \
    Q330_DEFAULT_C2_GPS_VCO_INTERCEPT,      \
    Q330_DEFAULT_C2_GPS_MAX_IKM_RMS,        \
    Q330_DEFAULT_C2_GPS_IKM_WEIGHT,         \
    Q330_DEFAULT_C2_GPS_KM_WEIGHT,          \
    Q330_DEFAULT_C2_GPS_BEST_WEIGHT,        \
    Q330_DEFAULT_C2_GPS_KM_DELTA            \
}

/* Structures */

typedef struct {
    char addr[MAXPATHLEN+1];
    char auth[MAXPATHLEN+1];
    char calib[MAXPATHLEN+1];
    char detector[MAXPATHLEN+1];
    char sensor[MAXPATHLEN+1];
    char lcq[MAXPATHLEN+1];
} Q330_CFG_PATH;

typedef struct {
    UINT32 tag;              /* property tag */
    UINT64 serialno;         /* internal serial number */
    int code[QDP_NUM_PORT];  /* authorization codes */
} Q330_AUTH;

typedef struct {
#define Q330_SENSOR_NAME_LEN 31
    char name[Q330_SENSOR_NAME_LEN+1]; /* sensor name */
    UINT32 scA[QDP_NSC];               /* sensor control map if sensor A */
    UINT32 scB[QDP_NSC];               /* sensor control map if sensor B */
#define Q330_SENSOR_DESC_LEN 255
    char desc[Q330_SENSOR_DESC_LEN+1]; /* descriptive name */
    int type;                          /* unique number for easy compares */
} Q330_SENSOR;

typedef struct {
    BOOL present; /* TRUE if E330 is present */
#define Q330_E300_NAME_LEN 256
    char name[Q330_E300_NAME_LEN+1]; /* /etc/hosts name for E300 console Digi */
} Q330_E300;

typedef struct {
#define Q330_INPUT_NAME_LEN 255
    char name[Q330_INPUT_NAME_LEN+1];  /* Complex input name from configuration file */
    Q330_SENSOR sensor;                /* input sensor */
    Q330_E300 e300;                    /* Metrozet E300 description */
    struct {
        BOOL present;                     /* TRUE if token present */
        char name[Q330_INPUT_NAME_LEN+1]; /* final token of complex name */
    } extra;
} Q330_INPUT;

typedef struct {
    QDP_CONNECT connect;      /* name and physical connection attributes */
    UINT64 serialno;          /* serial number */
    UINT64 authcode;          /* authorization code */
    struct {
        Q330_INPUT a;         /* Sensor A input */
        Q330_INPUT b;         /* Sensor B input */
    } input;
    int instance;             /* used to generate predictable UDP return ports */
    int watchdog;             /* registration watchdog timeout, msec */
} Q330_ADDR;

#define Q330_DEFAULT_SITE_NAME "default"

typedef struct {
    BOOL enabled;                /* TRUE if we have something here */
    char name[ISI_STALEN+1];     /* site name for which this applies */
    char channels[MAXPATHLEN+1]; /* comma delimited list of names of all triggered channels */
    int votes;                   /* mininum number of "on" channels required to declare an event */
    UINT32 pre;                  /* pre-event memory duration, in seconds */
    UINT32 pst;                  /* post-event memory duration, in seconds */
    DETECTOR engine;             /* event detector parameters */
} Q330_DETECTOR;

typedef struct {
    char name[ISI_STALEN+1];   /* name to assign to this particular calibration */
    Q330_INPUT input;          /* input (needs to match addr) */
    QDP_TYPE_C1_QCAL qcal;     /* calibration parameters */
} Q330_CALIB; 

typedef struct {
    char chn[QDP_CNAME_LEN+1];      /* channel code (seed name) */
    char loc[QDP_LNAME_LEN+1];      /* location code */
    UINT8 src[QDP_LCQSRC_LEN];      /* src bytes */
    REAL64 sint;                    /* sample interval, seconds */
    INT16 rate;                     /* sample rate (pos => Hz, neg => 1/-rate (Hz)) */
} Q330_LCQ_ENTRY;

typedef struct {
#define Q330_LCQ_NAME_LEN 31
    char name[Q330_LCQ_NAME_LEN+1]; /* name of channel set this entry belongs to */
    LNKLST *list;                   /* linked list of Q330_LCQ_ENTRY entries */
} Q330_LCQ;

typedef struct {
    char root[MAXPATHLEN+1];
    Q330_CFG_PATH path;  /* path names to files that were used to build this config */
    LNKLST *addr;        /* linked list of Q330_ADDR digitizer addresses */
    LNKLST *auth;        /* linked list of Q330_AUTH authorization codes */
    LNKLST *detector;    /* linked list of Q330_DETECTOR event detectors */
    LNKLST *calib;       /* linked list of Q330_CALIB calibration sets */
    LNKLST *sensor;      /* linked list of Q330_SENSOR sensor control maps */
    LNKLST *lcq;         /* linked list of Q330_LCQ logical channel sets */
} Q330_CFG;

/* function prototypes */

/* cfg.c */
char *q330RootPath(char *path);
Q330_CFG *q330DestroyCfg(Q330_CFG *cfg);
char *q330PathName(char *root, char *name, char *buf);
BOOL q330AddAddr(Q330_CFG *cfg, Q330_ADDR *new);
BOOL q330AddAuth(Q330_CFG *cfg, Q330_AUTH *new);
Q330_CFG *q330ReadCfg(char *root, int *errcode);

/* lcq.c */
BOOL q330SetTokenLcq(QDP_DP_TOKEN *token, Q330_LCQ *input, char *sta, char *net);

/* lookup.c */
Q330_ADDR *q330LookupAddr(char *name, Q330_CFG *cfg);
Q330_AUTH *q330LookupAuth(UINT32 tag, Q330_CFG *cfg);
Q330_DETECTOR *q330LookupDetector(char *name, Q330_CFG *cfg);
Q330_CALIB *q330LookupCalib(char *name, Q330_ADDR *addr, Q330_CFG *cfg);
Q330_CALIB *q330LookupCalibB(char *name, Q330_ADDR *addr, Q330_CFG *cfg);
Q330_SENSOR *q330LookupSensor(char *name, Q330_CFG *cfg);
Q330_LCQ *q330LookupLcq(char *name, Q330_CFG *cfg);
Q330_ADDR *q330GetAddr(char *name, Q330_CFG *cfg, Q330_ADDR *dest);
Q330_AUTH *q330GetAuth(UINT32 tag, Q330_CFG *cfg, Q330_AUTH *auth);
Q330_DETECTOR *q330GetDetector(char *name, Q330_CFG *cfg, Q330_DETECTOR *dest);
Q330_CALIB *q330GetCalib(char *name, Q330_ADDR *addr, Q330_CFG *cfg, Q330_CALIB *dest);
Q330_CALIB *q330GetCalibB(char *name, Q330_ADDR *addr, Q330_CFG *cfg, Q330_CALIB *dest);
Q330_SENSOR *q330GetSensor(char *name, Q330_CFG *cfg, Q330_SENSOR *dest);

/* register.c */
QDP *q330Register(Q330_ADDR *addr, int port, int debug, LOGIO *lp, int *errcode, int *suberr);

/* sc.c */
BOOL q330ParseSensorControlLines(char which, UINT32 *sc, char *input, BOOL ActiveHigh, int *errcode);
BOOL q330InitializeSensorControl(UINT32 *sc, Q330_SENSOR *a, Q330_SENSOR *b);
UINT16 q330SensorControlBitmap(UINT32 *sc);
char *q330SensorCtrlLineString(UINT32 *sc, char *buf);
char *q330SensorHiLoString(Q330_SENSOR *sensor);

/* string.c */
char *q330CalibParamString(QDP_TYPE_C1_QCAL *calib, char *buf);

/* util.c */
int q330CalibrationStatus(Q330_ADDR *addr, int *errcode);
BOOL q330ParseComplexInputName(Q330_INPUT *input, char *name, int *error);
void q330PrintErrcode(FILE *fp, char *prefix, char *root, int errcode);
UINT16 q330ChannelBitmap(char *string);

/* wcfg.c */
void q330PrintAddr(FILE *fp, Q330_ADDR *addr);
void q330WriteAddr(FILE *fp, Q330_CFG *cfg);
void q330PrintAuth(FILE *fp, Q330_AUTH *auth);
void q330WriteAuth(FILE *fp, Q330_CFG *cfg);
void q330WriteSensor(FILE *fp, Q330_CFG *cfg);
void q330WriteDetector(FILE *fp, Q330_CFG *cfg);
void q330WriteCalib(FILE *fp, Q330_CFG *cfg);
void q330WriteLcq(FILE *fp, Q330_CFG *cfg);
BOOL q330SaveAddr(char *root, Q330_CFG *cfg);
BOOL q330SaveAuth(char *root, Q330_CFG *cfg);
BOOL q330SaveSensor(char *root, Q330_CFG *cfg);
BOOL q330SaveDetector(char *root, Q330_CFG *cfg);
BOOL q330SaveCalib(char *root, Q330_CFG *cfg);
BOOL q330SaveCfg(char *root, Q330_CFG *cfg);

/* version.c */
char *q330VersionString(void);
VERSION *q330Version(void);

#ifdef __cplusplus
}
#endif

#endif /* q330_h_included */

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
 * $Log: q330.h,v $
 * Revision 1.33  2017/01/26 19:47:27  dechavez
 * defined Q330_NO_LCQMEM
 *
 * Revision 1.32  2016/08/15 19:43:57  dechavez
 * changed Q330_DEFAULT_WATCHDOG to 10 seconds
 *
 * Revision 1.31  2016/07/20 21:54:26  dechavez
 * replaced Q330_ADDR:maxtry with watchdog and added suberr argument to q330Register()
 * prototype to reflect corresponding changes in QDP state machine (libqdp 3.13.0 and later)
 *
 * Revision 1.30  2016/02/11 19:07:02  dechavez
 * changed maxsrq to maxtry to follow libqdp 3.11.3 names
 *
 * Revision 1.29  2016/02/05 16:52:07  dechavez
 * added maxsrq to Q330_ADDR
 *
 * Revision 1.28  2014/08/11 17:41:22  dechavez
 * updated prototypes
 *
 * Revision 1.27  2013/06/19 22:18:47  dechavez
 * added default calibrate and monitor bitmaps, plus q330GetCalibB() and q330LookupCalibB() prototypes
 *
 * Revision 1.26  2011/04/16 04:50:49  dechavez
 * updated prototypes
 *
 * Revision 1.25  2011/04/14 19:23:11  dechavez
 * updated prototypes
 *
 * Revision 1.24  2011/04/12 20:52:49  dechavez
 * added Q330_DUP_LCQ
 *
 * Revision 1.23  2011/04/12 20:48:40  dechavez
 * added Q330_LCQ to Q330_CFG
 *
 * Revision 1.22  2011/01/25 17:59:24  dechavez
 * replaced name and tag fields in Q330_ADDR with QDP_CONNECT
 *
 * Revision 1.21  2010/12/27 22:36:24  dechavez
 * added "extra" field to Q330_INPUT
 *
 * Revision 1.20  2010/12/21 21:31:36  dechavez
 * removed unused resistors field from Q330_INPUT
 *
 * Revision 1.19  2010/12/21 19:52:48  dechavez
 * added more error codes and prototypes
 *
 * Revision 1.18  2010/12/17 19:32:41  dechavez
 * libq330 2.0.0
 *
 * Revision 1.17  2010/12/13 22:54:11  dechavez
 * brought over default config values from bin/q330/default.h
 *
 * Revision 1.16  2010/12/02 20:27:37  dechavez
 * added STS2.5 support
 *
 * Revision 1.15  2010/11/24 19:10:38  dechavez
 * added Q330_SENSOR to Q330_ADDR (and related stuff)
 *
 * Revision 1.14  2010/09/30 17:35:18  dechavez
 * added e300 parameter to Q330_CALIB
 *
 * Revision 1.13  2010/04/12 21:15:12  dechavez
 * defined Q330_CAL_x, Q330_ERROR, Q330_PORT_BUSY
 *
 * Revision 1.12  2010/04/02 18:14:09  dechavez
 * restored instance parameter to Q330_ADDR
 *
 * Revision 1.11  2010/03/31 20:16:13  dechavez
 * removed instance parameter
 *
 * Revision 1.10  2010/03/22 21:45:44  dechavez
 * replaced cal.src, cal.active with simple calport in Q330_ADDR
 *
 * Revision 1.9  2010/03/12 00:23:46  dechavez
 * added calibration monitor stuff to Q330_ADDR
 *
 * Revision 1.8  2009/07/28 18:08:52  dechavez
 * fixed duration problem in default calibration, updated prototypes
 *
 * Revision 1.7  2009/07/25 17:35:12  dechavez
 * use linked lists instead of arrays in Q330_CFG, redefined Q330_CALIB to
 * support arbitrary number of calibration sets, updated prototyes
 *
 * Revision 1.6  2008/03/11 20:50:28  dechavez
 * defined Q330_MAX_CAL_DURATION, Q330_MAX_CAL_AMPLTUDE, and Q330_MAX_CAL_DIVISOR
 * changed default duration to Q330_MAX_CAL_DURATION
 *
 * Revision 1.5  2007/12/14 21:34:29  dechavez
 * added "calib" support
 *
 * Revision 1.4  2007/10/31 17:20:22  dechavez
 * updated prototypes
 *
 * Revision 1.3  2007/09/25 20:55:30  dechavez
 * added enabled field to detector
 *
 * Revision 1.2  2007/09/22 02:38:17  dechavez
 * added detector
 *
 * Revision 1.1  2007/09/06 18:24:39  dechavez
 * created
 *
 */
