#pragma ident "$Id: qdp.h,v 1.98 2017/10/11 20:22:50 dechavez Exp $"
/*======================================================================
 *
 *  Quanterra QDP communication
 *
 *====================================================================*/
#ifndef qdp_h_included
#define qdp_h_included

#include "platform.h"
#include "zlib.h"
#include "qio.h"
#include "util.h"
#include "msgq.h"
#include "logio.h"
#include "qdp/limits.h"
#include "qdp/codes.h"
#include "qdp/fsa.h"
#include "qdp/tokens.h"
#include "qdp/status.h"
#include "qdp/errno.h"
#include "list.h"

#ifdef __cplusplus
extern "C" {
#endif

extern char *libqdpBuildIdent;

/* Constants */

#define QDP_VERSION 2

#define QDP_OK         0
#define QDP_TIMEOUT    1
#define QDP_IOERROR    2
#define QDP_CRCERR     3
#define QDP_OVERFLOW   4
#define QDP_INVALID    5
#define QDP_INCOMPLETE 6
#define QDP_ERROR      7 /* check errno */

#define QDP_CTRL  0
#define QDP_DATA  1

#define QDP_EPOCH_TO_1970_EPOCH 946684800
#define QDP_EPOCH_TO_1999_EPOCH  31536000

/* Broadcast address and port */

#define QDP_BROADCAST_IP   0xFFFFFFFF
#define QDP_BROADCAST_PORT 0xFFFF

/* UDP port numbers relative to base port */

#define QDP_PORT_CONFIG  0 /* configuration */
#define QDP_PORT_SPECIAL 1 /* special functions */
#define QDP_PORT_1_CTRL  2 /* control on data port 1 */
#define QDP_PORT_1_DATA  3 /* data on data port 1 */
#define QDP_PORT_2_CTRL  4 /* control on data port 2 */
#define QDP_PORT_2_DATA  5 /* data on data port 2 */
#define QDP_PORT_3_CTRL  6 /* control on data port 3 */
#define QDP_PORT_3_DATA  7 /* data on data port 3 */
#define QDP_PORT_4_CTRL  8 /* control on data port 4 */
#define QDP_PORT_4_DATA  9 /* data on data port 4 */

/* Port numbers as specified by the app
 * NOTE: The values assigned for the defined ports MUST start at 0
 *       and increment by one, as they are used as array indices
 */

#define QDP_UNDEFINED_PORT -1
#define QDP_LOGICAL_PORT_1  0 /* Logical Port 1 - NEVER CHANGE THIS VALUE! */
#define QDP_LOGICAL_PORT_2  1 /* Logical Port 2 - NEVER CHANGE THIS VALUE!*/
#define QDP_LOGICAL_PORT_3  2 /* Logical Port 3 - NEVER CHANGE THIS VALUE! */
#define QDP_LOGICAL_PORT_4  3 /* Logical Port 4 - NEVER CHANGE THIS VALUE! */
#define QDP_CFG_PORT        4 /* Configuration */
#define QDP_SFN_PORT        5 /* Special Functions */
#define QDP_NUM_PORT        6
#define QDP_NUM_DPORT       4 /* 0-3 are the 4 data ports */

/* Bits in the status request bitmap */

#define QDP_STATUS_GLOBAL          0x000000000001
#define QDP_STATUS_GPS             0x000000000002
#define QDP_STATUS_POWER_SUPPLY    0x000000000004
#define QDP_STATUS_BOOM_ETC        0x000000000008
#define QDP_STATUS_THREAD          0x000000000010
#define QDP_STATUS_PLL             0x000000000020
#define QDP_STATUS_SATELLITES      0x000000000040
#define QDP_STATUS_ARP             0x000000000080
#define QDP_STATUS_DATA_PORT_1     0x000000000100
#define QDP_STATUS_DATA_PORT_2     0x000000000200
#define QDP_STATUS_DATA_PORT_3     0x000000000400
#define QDP_STATUS_DATA_PORT_4     0x000000000800
#define QDP_STATUS_SERIAL_1        0x000000001000
#define QDP_STATUS_SERIAL_2        0x000000002000
#define QDP_STATUS_SERIAL_3        0x000000004000
#define QDP_STATUS_ETHERNET        0x000000008000
#define QDP_STATUS_BALER           0x000000010000
#define QDP_STATUS_DYNIP           0x000000020000
#define QDP_STATUS_AUXBOARD        0x000000040000
#define QDP_STATUS_SERIAL_SENSOR   0x000000080000

#define QDP_STATUS_DEFAULT_BITMAP  0xEF

/* Physical Ports */

#define QDP_PP_SERIAL_1 0
#define QDP_PP_SERIAL_2 1
#define QDP_PP_SERIAL_3 2
#define QDP_PP_ETHERNET 3
#define QDP_PP_NUMPHY   4 /* number of defined physical interfaces (SERIAL3 doesn't seem to exist however) */

/* Logging verbosity codes */

#define QDP_TERSE  0 /* only warnings and errors */
#define QDP_INFO   1 /* normal */
#define QDP_DEBUG  2 /* very noisy */
#define QDP_DEBUG2 3 /* QDP_DEBUG plus QIO_DEBUG_TERSE messages */
#define QDP_DEBUG3 4 /* QDP_DEBUG plus QIO_DEBUG_VERBOSE messages */

/* Clock Quality Flag Bits */

#define QDP_CQ_HAS_BEEN_LOCKED 1 /* Has been locked, else internal time */
#define QDP_CQ_2D 2 /* 2D Lock */
#define QDP_CQ_3D 4 /* 3D Lock */
#define QDP_CQ_1D 8 /* No fix, but time should be good */
#define QDP_CQ_FILT 0x10 /* Filtering in progress */
#define QDP_CQ_SPEC 0x20 /* Speculative 1-D startup */

#define QDP_CQ_IS_LOCKED (QDP_CQ_2D | QDP_CQ_3D | QDP_CQ_1D)

/* PLL State */

#define QDP_PLL_OFF 0x00 /* Not on */
#define QDP_PLL_HOLD 0x40
#define QDP_PLL_TRACK 0x80
#define QDP_PLL_MASK 0xC0
#define QDP_PLL_LOCK QDP_PLL_MASK

/* Calibration State */

#define QDP_CAL_UNCHANGED 1
#define QDP_CAL_ENABLED   2
#define QDP_CAL_RUNNING   3
#define QDP_CAL_IDLE      4

/* QDP common header */

typedef struct {
    UINT32 crc;   /* checksum over all the packet */
    UINT8 cmd;    /* command */
    UINT8 ver;    /* version */
    UINT16 dlen;  /* payload length, not including header */
    UINT16 seqno; /* sender's sequence number */
    UINT16 ack;   /* sender's acknowledge */
} QDP_CMNHDR;

/* Quanterra Data Packet */

typedef struct {
    QDP_CMNHDR hdr;         /* header */
    UINT8 *payload;         /* points to payload */
    UINT8 raw[QDP_MAX_MTU]; /* the entire raw packet, including header */
    int len;                /* number of bytes actually used in raw */
} QDP_PKT;

#define QDP_MAX_DOTDECIMAL_LEN 16
#define QDP_MAX_PEER_LEN (QDP_MAX_DOTDECIMAL_LEN + 16)
#define QDP_IDENT_LEN 16

/* Command and response packets */

/* server IP, Port, Registration */

typedef struct {
    UINT32 ip;
    UINT16 port;
    UINT16 registration;
    char dotdecimal[QDP_MAX_DOTDECIMAL_LEN+1];
} QDP_DP_IPR;

/* server challenge */

typedef struct {
    UINT64 challenge;
    QDP_DP_IPR dp;
} QDP_TYPE_C1_SRVCH;

/* server challenge response */

typedef struct {
    UINT64 auth;
    UINT64 serialno;
    UINT64 challenge;
    QDP_DP_IPR dp;
    UINT64 random;
    UINT32 md5[4];
} QDP_TYPE_C1_SRVRSP;

/* memory request */

typedef struct {
    UINT32 offset;
    UINT16 nbyte;
    UINT16 type;
    UINT32 passwd[4];
} QDP_TYPE_C1_RQMEM;

/* memory response */

typedef struct {
    UINT32 offset;
    UINT16 nbyte;
    UINT16 type;
    UINT16 seg;
    UINT16 nseg;
    UINT8  contents[QDP_MAX_C1_MEM_PAYLOAD];
} QDP_TYPE_C1_MEM;

/* C1_FIX */

typedef struct {
    int major;
    int minor;
    UINT16 raw;
} QDP_SOFTVER;

typedef struct {
    UINT8  raw;   /* raw 8-bit frequency code */
    UINT64 nsint; /* equivalent sample interval, nanoseconds */
    REAL64 dsint; /* equivalent sample interval, seconds */
} QDP_FREQ;

typedef struct {
    UINT32 last_reboot;
    UINT32 reboots;
    UINT32 backup_map;
    UINT32 default_map;
#define QDP_CAL_TYPE_QCAL330 33
    UINT16 cal_type;
    QDP_SOFTVER cal_ver;
#define QDP_AUX_TYPE_AUXAD 32
    UINT16 aux_type;
    QDP_SOFTVER aux_ver;
#define QDP_CLOCK_NONE 0
#define QDP_CLOCK_M12  1
    UINT16 clk_type;
#define QDP_C1_FIX_FLAG_ETHERNET 0x01
#define QDP_C1_FIX_FLAG_DHCP     0x02
    UINT16 flags;
    QDP_SOFTVER sys_ver;
    QDP_SOFTVER sp_ver;
    QDP_SOFTVER pld_ver;
    UINT16 mem_block;
    UINT32 proper_tag;
    UINT64 sys_num;
    UINT64 amb_num;
    UINT64 sensor_num[QDP_NSENSOR];
    UINT32 qapchp1_num;
    UINT32 int_sz;
    UINT32 int_used;
    UINT32 ext_sz;
    UINT32 flash_sz;
    UINT32 ext_used;
    UINT32 qapchp2_num;
    UINT32 log_sz[QDP_NLP];
    QDP_FREQ freq[QDP_NFREQ];
    INT32  ch13_delay[QDP_NFREQ];
    INT32  ch46_delay[QDP_NFREQ];
} QDP_TYPE_C1_FIX;

/* C1_GLOB */

typedef struct {
    UINT16 clock_to;
    UINT16 initial_vco;
    UINT16 gps_backup;
    UINT16 samp_rates;
    UINT16 gain_map;
    UINT16 filter_map;
    UINT16 input_map;
    UINT16 web_port;
    UINT16 server_to;
    UINT16 drift_tol;
    UINT16 jump_filt;
    UINT16 jump_thresh;
    INT16  cal_offset;
    UINT16 sensor_map;
    UINT16 sampling_phase;
    UINT16 gps_cold;
    UINT32 user_tag;
    UINT16 scaling[QDP_NCHAN][QDP_NFREQ];
    UINT16 offset[QDP_NCHAN];
    UINT16 gain[QDP_NCHAN];
    UINT32 msg_map;
    UINT32 crc;
} QDP_TYPE_C1_GLOB;

/* C1_LOG */

typedef struct {
    UINT16 port;
#define QDP_LOG_FLAG_MASK_FILL_MODE    0x0001 /* 00000000 00000001 */
#define QDP_LOG_FLAG_MASK_FLUSH_BUFFER 0x0002 /* 00000000 00000010 */
#define QDP_LOG_FLAG_MASK_FREEZE_OUPUT 0x0004 /* 00000000 00000100 */
#define QDP_LOG_FLAG_MASK_FREEZE_INPUT 0x0008 /* 00000000 00001000 */
#define QDP_LOG_FLAG_MASK_KEEP_OLDEST  0x0010 /* 00000000 00010000 */
#define QDP_LOG_FLAG_MASK_PIGGYBACK    0x0100 /* 00000001 00000000 */
#define QDP_LOG_FLAG_MASK_LED_ON       0x0200 /* 00000010 00000000 */
#define QDP_LOG_FLAG_MASK_HOT_SWAP     0x0400 /* 00000100 00000000 */
#define QDP_LOG_FLAG_MASK_FLUSH_WINDOW 0x0800 /* 00001000 00000000 */
#define QDP_LOG_FLAG_MASK_BASE_96      0x4000 /* 01000000 00000000 */
#define QDP_LOG_FLAG_MASK_SAVE_EEPROM  0x8000 /* 10000000 00000000 */
    UINT16 flags;
    UINT16 perc;
    UINT16 mtu;
    UINT16 group_cnt;
    UINT16 rsnd_max;
    UINT16 grp_to;
    UINT16 rsnd_min;
    UINT16 window;
    UINT16 dataseq;
    UINT16 freqs[QDP_NCHAN];
    UINT16 ack_cnt;
    UINT16 ack_to;
    UINT32 olddata;
    UINT16 eth_throttle;
    UINT16 full_alert;
    UINT16 auto_filter;
    UINT16 man_filter;
    UINT32 crc;
} QDP_TYPE_C1_LOG;

/* C1_COMBO (aka C1_FLGS), ignoring the C1_SC portion */

typedef struct {
    QDP_TYPE_C1_FIX fix;
    QDP_TYPE_C1_GLOB glob;
    QDP_TYPE_C1_LOG log;
} QDP_TYPE_C1_COMBO;

/* C1_QCAL */

#define QDP_QCAL_SINE  0
#define QDP_QCAL_RED   1
#define QDP_QCAL_WHITE 2
#define QDP_QCAL_STEP  3
#define QDP_QCAL_RB    4

typedef struct {
    UINT32 starttime; /* start time, 0 for immediately */
    UINT16 waveform;  /* cal signal type, sign, auto flag */
    UINT16 amplitude; /* number of bits shifted, 0=-6db, 1=-12db, etc */
    UINT16 duration;  /* duration in seconds */
    UINT16 settle;    /* settling time in seconds */
    UINT16 trailer;   /* time after cal ends before control lines or relays are restored */
    UINT16 chans;     /* calibration channels (bitmap) */
    UINT16 sensor;    /* sensor control bitmap */
    UINT16 monitor;   /* monitor channel bitmap */
    UINT16 divisor;   /* frequency divider 1=1Hz, 2=.5Hz, 20=.05Hz, etc */
} QDP_TYPE_C1_QCAL;

/* C1_MYSN */

typedef struct {
    UINT64 serialno;       /* serial number */
    UINT32 proper_tag;     /* KMI property tag */
    UINT32 user_tag;       /* user tag */
} QDP_TYPE_C1_MYSN;

/* C1_PHY */

typedef struct {
    UINT64 serialno;       /* serial number */
    UINT16 baseport;       /* starting UDP port number */
    struct {
        UINT32 ip;         /* IP address */
        UINT16 baud;       /* baud rate code */
        UINT16 flags;      /* flags bitmask */
        UINT16 throttle;   /* 1024000 / bytes per second (or zero if disabled) */
    } serial[3];
    struct {
        UINT32 ip;         /* IP address */
        UINT8  mac[6];     /* hardware address */
        UINT16 flags;      /* flags bitmask */
    } ethernet;
    UINT16 reserved[2];    /* presently unused */
    UINT32 crc;
} QDP_TYPE_C1_PHY;

/* C1_DCP */

typedef struct {
    INT32 grounded[QDP_NCHAN];  /* counts with grounded input */
    INT32 reference[QDP_NCHAN]; /* counts with reference input */
} QDP_TYPE_C1_DCP;

/* C1_GID */

typedef char QDP_TYPE_C1_GID[QDP_GIDLEN+1];

/* C1_SPP */

typedef struct {
    UINT16 max_main_current;
    UINT16 min_off_time;
    UINT16 min_ps_voltage;
    UINT16 max_antenna_current;
     INT16 min_temp;
     INT16 max_temp;
    UINT16 temp_hysteresis;
    UINT16 volt_hysteresis;
    UINT16 default_vco;
    UINT32 crc;
} QDP_TYPE_C1_SPP;

#define QDP_DEFAULT_MAX_MAIN_CURRENT    350
#define QDP_DEFAULT_MIN_OFF_TIME         60
#define QDP_DEFAULT_MIN_PS_VOLTAGE       60
#define QDP_DEFAULT_MAX_ANTENNA_CURRENT  34
#define QDP_DEFAULT_MIN_TEMP            -40
#define QDP_DEFAULT_MAX_TEMP             65
#define QDP_DEFAULT_TEMP_HYSTERESIS       5
#define QDP_DEFAULT_VOLT_HYSTERESIS      13
#define QDP_DEFAULT_DEFAULT_VCO        2048

#define QDP_DEFAULT_C1_SPP {         \
    QDP_DEFAULT_MAX_MAIN_CURRENT,    \
    QDP_DEFAULT_MIN_OFF_TIME,        \
    QDP_DEFAULT_MIN_PS_VOLTAGE,      \
    QDP_DEFAULT_MAX_ANTENNA_CURRENT, \
    QDP_DEFAULT_MIN_TEMP,            \
    QDP_DEFAULT_MAX_TEMP,            \
    QDP_DEFAULT_TEMP_HYSTERESIS,     \
    QDP_DEFAULT_VOLT_HYSTERESIS,     \
    QDP_DEFAULT_DEFAULT_VCO          \
}

/* C1_MAN */

typedef struct {
    UINT32 password[4];
    UINT16 qapchp1_type;
    QDP_SOFTVER qapchp1_ver;
    UINT16 qapchp2_type;
    QDP_SOFTVER qapchp2_ver;
    UINT32 qapchp1_num;
    UINT32 qapchp2_num;
    INT32 reference[QDP_NCHAN];
    UINT32 born_on;
    UINT32 packet_memory;
    UINT16 clk_type;
    UINT16 model;
    INT16 default_calib_offset;
    UINT16 flags;
    UINT32 proper_tag;
    UINT32 max_power_on;
} QDP_TYPE_C1_MAN;

/* C2_AMASS */

typedef struct {
    UINT16 tolerance[3];
    UINT16 maxtry;
    UINT16 duration;
    struct {
        UINT16 squelch;
        UINT16 normal;
    } interval;
    UINT16 bitmap;
} QDP_TYPE_C2_AMASS_SENSOR;

typedef struct {
    QDP_TYPE_C2_AMASS_SENSOR sensor[QDP_NSENSOR];
    UINT32 crc;
} QDP_TYPE_C2_AMASS;

typedef struct {
    UINT16 mode;          /* timing mode */
    UINT16 flags;         /* power cycling mode */
    UINT16 off_time;      /* GPS off time */
    UINT16 resync;        /* GPS resync hour */
    UINT16 max_on;        /* GPS maximum on time */
    UINT16 lock_usec;     /* PLL lock usec */
    UINT16 interval;      /* PLL update interval (sec) */
    UINT16 initial_pll;   /* PLL flags */
    REAL32 pfrac;         /* pfrac */
    REAL32 vco_slope;     /* VCO slope */
    REAL32 vco_intercept; /* VCO intersept */
    REAL32 max_ikm_rms;   /* Max. initial KM rms */
    REAL32 ikm_weight;    /* initial KM weight */
    REAL32 km_weight;     /* KM weight */
    REAL32 best_weight;   /* best VCO weight */
    REAL32 km_delta;      /* KM delta */
    struct {UINT32 gps; UINT32 pll;} crc;
} QDP_TYPE_C2_GPS;

/* C2_PHY */

#define QDP_PHY2_MODEM_INIT_LEN   39
#define QDP_PHY2_PHONE_NUMBER_LEN 39
#define QDP_PHY2_OUT_NAME_LEN     19
#define QDP_PHY2_OUT_PASS_LEN     19
#define QDP_PHY2_IN_NAME_LEN       9
#define QDP_PHY2_IN_PASS_LEN       9
#define QDP_PHY2_MAX_NAME_LEN QDP_PHY2_OUT_NAME_LEN
#define QDP_PHY2_MAX_PASS_LEN QDP_PHY2_OUT_PASS_LEN

typedef struct {
    UINT16 phynum;   /* physical interface number */
    UINT16 lognum;   /* data port number */
    char modem_init[QDP_PHY2_MODEM_INIT_LEN+1];     /* modem initialization string */
    char phone_number[QDP_PHY2_PHONE_NUMBER_LEN+1]; /* phone number */
    struct {
        char name[QDP_PHY2_MAX_NAME_LEN+1];
        char pass[QDP_PHY2_MAX_PASS_LEN+1];
    } dial_in, dial_out;
    UINT32 trigger;   /* memory trigger level */
    UINT16 flags;     /* options bitmap */
    UINT16 retry_int; /* retry interval (dial in/out) or power off timer (baler) */
    UINT16 interval;  /* dial-out or logging interval, in minutes */
    UINT16 webbps;    /* web server bps limit, divided by 10 */
    UINT32 poc_ip;    /* point of contact or baler IP address */
    UINT32 log2_ip;   /* dial-in IP suggestion or baler secondary IP address */
    UINT16 poc_port;  /* point of contact port number */
    UINT16 retries;   /* baler retries */
    UINT16 reg_to;    /* baler registration time out */
    UINT16 baud;      /* serial baud rate */
    UINT16 routed_to; /* routed packet timeout */
    UINT16 ss;        /* serial sensor bitmask */
    UINT32 crc;
} QDP_TYPE_C2_PHY;

/* C3_ANNC */

#define QDP_MAX_ANNC 6

typedef struct {
    UINT32 dp_ip;     /* DP IP address */
    UINT32 router_ip; /* router IP address */
    UINT16 timeout;   /* timeout in minutes */
    UINT16 resume;    /* resume time in minutes */
    UINT16 flags;     /* flag bitmap */
    UINT16 dp_port;   /* DP UDP port */
} QDP_TYPE_C3_ANNC_ENTRY;

typedef struct {
    UINT16 c3_rqannc;  /* sanity check */
    UINT16 c3_annc;    /* sanity check */
    UINT16 dps_active; /* number of active entries */
    UINT16 flags;      /* unlock flags bitmap */
    QDP_TYPE_C3_ANNC_ENTRY entry[QDP_MAX_ANNC];
} QDP_TYPE_C3_ANNC;

/* C3_BCFG */

#define QDP_BCFG_MAX_FREEFORM_LEN 231

typedef struct {
    UINT16 c3_rqbcfg; /* sanity check */
    UINT16 c3_bcfg;   /* sanity check */
    UINT16 phyport;   /* physical interface number */
    UINT16 balertype; /* Baler type */
    UINT16 version;   /* version number describing Baler specific data */
    char freeform[QDP_BCFG_MAX_FREEFORM_LEN+1]; /* Baler configuration string */
} QDP_TYPE_C3_BCFG;

/* Environmental processor constants (for C2_EPD and C2_EPCFG) */

#define QDP_MIN_EP_VERSION_MAJOR 1
#define QDP_MIN_EP_VERSION_MINOR 137

#define C2_EP_SUBCHAN_ANALOG1_40HZ          0x00
#define C2_EP_SUBCHAN_ANALOG1_20HZ          0x01
#define C2_EP_SUBCHAN_ANALOG1_10HZ          0x02
#define C2_EP_SUBCHAN_ANALOG1_1HZ           0x03
#define C2_EP_SUBCHAN_ANALOG2_40HZ          0x04
#define C2_EP_SUBCHAN_ANALOG2_20HZ          0x05
#define C2_EP_SUBCHAN_ANALOG2_10HZ          0x06
#define C2_EP_SUBCHAN_ANALOG2_1HZ           0x07
#define C2_EP_SUBCHAN_ANALOG3_40HZ          0x08
#define C2_EP_SUBCHAN_ANALOG3_20HZ          0x09
#define C2_EP_SUBCHAN_ANALOG3_10HZ          0x0a
#define C2_EP_SUBCHAN_ANALOG3_1HZ           0x0b
#define C2_EP_SUBCHAN_INTERNAL_PRESSURE     0x10
#define C2_EP_SUBCHAN_INTERNAL_TEMP         0x11
#define C2_EP_SUBCHAN_INTERNAL_HUMIDITY     0x12
#define C2_EP_SUBCHAN_INPUT_VOLTAGE         0x13
#define C2_EP_SUBCHAN_PLL_OFFSET            0x14
#define C2_EP_SUBCHAN_VCO_CONTROL           0x15
#define C2_EP_SUBCHAN_WXT520_WIND_DIR       0x20
#define C2_EP_SUBCHAN_WXT520_WIND_SPEED     0x21
#define C2_EP_SUBCHAN_WXT520_PRESSURE       0x22
#define C2_EP_SUBCHAN_WXT520_TEMP           0x23
#define C2_EP_SUBCHAN_WXT520_HUMIDITY       0x24
#define C2_EP_SUBCHAN_WXT520_RAIN_INTENSITY 0x25
#define C2_EP_SUBCHAN_WXT520_HAIL_INTENSITY 0x26
#define C2_EP_SUBCHAN_WXT520_HEATER_TEMP    0x27
#define C2_EP_SUBCHAN_WXT520_HEATER_VOLTAGE 0x28
#define C2_EP_SUBCHAN_WXT520_SUPPLY_VOLTAGE 0x29
#define C2_EP_SUBCHAN_WXT520_REF_VOLTAGE    0x2a

#define QDP_EP_SUBCHAN_MASK 0x7F /* to get above subchans from the EP channel number */
#define QDP_EP_SRC0         0xFC
#define QDP_MAX_EP_CHANNEL  128

typedef UINT8 QDP_EPCHN; /* EP channel number (0-127 = EP1, 128-255 = EP2) */

/* C2_EPD */

typedef struct {
    UINT32 raw;        /* raw CP_EPD value */
    QDP_EPCHN channel; /* channel number */
    INT32 tics;        /* filter delay in tics */
    INT32 usec;        /* filter delay in usec */
#define QDP_EPD_USPEC_PER_TIC 20
} QDP_EP_DELAY;

typedef struct {
    UINT32 res1;    /* reserved (serial 1 EP) */
    UINT32 res2;    /* reserved (serial 2 EP) */
    UINT16 chancnt; /* channel count */
    UINT16 spare;   /* spare */
    QDP_EP_DELAY entry[QDP_MAX_EP_CHANNEL];
} QDP_TYPE_C2_EPD;

/* C2_EPCFG */

typedef struct {
    UINT16 raw;        /* raw C2_EPCFG value */
    int source;        /* 1 for EP1, 2 for EP2 */
    QDP_EPCHN channel; /* channel number */
    UINT8 dpmask;      /* data port bitmask */
} QDP_EP_CHANMASK;

typedef struct {
    UINT32 res1;    /* reserved (serial 1 EP) */
    UINT32 res2;    /* reserved (serial 2 EP) */
#define QDP_C2_EPCFG_FLAG_WXT520_CONTINUOUS 0x0001
    UINT32 flags1;  /* flags (serial 1 EP) */
    UINT32 flags2;  /* flags (serial 2 EP) */
    UINT16 chancnt; /* channel count */
    UINT16 spare;   /* spare */
    QDP_EP_CHANMASK entry[QDP_MAX_EP_CHANNEL];
} QDP_TYPE_C2_EPCFG;

/* Aux ADC configuration */

typedef struct {
    UINT8 nchan;
    UINT8 gain;
#define QDP_NUM_AUXCHAN 8
    UINT8 channel[QDP_NUM_AUXCHAN];
} QDP_TYPE_AUXADC;

/* Serial sensor (Paroscientific) configuration */

#define QDP_SS_TYPE_ABSOLUTE     0
#define QDP_SS_TYPE_GUAGE        1
#define QDP_SS_TYPE_DIFFERENTIAL 2

#define QDP_SS_UNITS_USER  0
#define QDP_SS_UNITS_PSI   1
#define QDP_SS_UNITS_MBAR  2
#define QDP_SS_UNITS_BAR   3
#define QDP_SS_UNITS_KPA   4
#define QDP_SS_UNITS_MPA   5
#define QDP_SS_UNITS_INHG  6
#define QDP_SS_UNITS_MMHG  7
#define QDP_SS_UNITS_MH2O  8

#define QDP_SS_MODEL_LEN   19
#define QDP_SS_VERSION_LEN 15
typedef struct {
    int port;   /* QDP_SS_PORT_x */
    UINT16 type;/* QDP_SS_TYPE_x */
    UINT16 si;  /* sample interval (seconds/sample) */
    UINT16 un;  /* QDP_SS_UNITS_x */
    UINT16 it;  /* integration time (ms) */
    UINT16 fd;  /* fractional digits */
    UINT32 sn;  /* serial number */
    REAL32 fp;  /* full scale pressure value */
    REAL32 pa;  /* pressure adder */
    REAL32 pm;  /* pressure multiplier */
    REAL32 tc;  /* time-base correction factor */
    REAL32 c1;  /* C1 pressure coefficient */
    REAL32 c2;  /* C2 pressure coefficient */
    REAL32 c3;  /* C3 pressure coefficient */
    REAL32 d1;  /* D1 pressure coefficient */
    REAL32 d2;  /* D2 pressure coefficient */
    REAL32 t1;  /* T1 pressure coefficient */
    REAL32 t2;  /* T2 pressure coefficient */
    REAL32 t3;  /* T3 pressure coefficient */
    REAL32 t4;  /* T4 pressure coefficient */
    REAL32 t5;  /* T5 pressure coefficient */
    REAL32 u0;  /* U0 pressure coefficient */
    REAL32 y1;  /* Y1 pressure coefficient */
    REAL32 y2;  /* Y2 pressure coefficient */
    REAL32 y3;  /* Y3 pressure coefficient */
    char model[QDP_SS_MODEL_LEN + 1];
    char version[QDP_SS_VERSION_LEN + 1];
} QDP_TYPE_SSCFG;

/* DT_DACK */

typedef struct {
    UINT16 throttle;
    UINT32 ack[4];
} QDP_TYPE_DT_DACK;

/* User supplied function for dealing with received packets */

typedef void (*QDP_USER_FUNC)(void *arg, QDP_PKT *pkt);

/* Structure for receiving C1_MEMs */

typedef struct {
    UINT16 type;
    UINT8 data[QDP_TOKEN_BUFLEN];
    INT16 pending[QDP_MAX_MEM_SEG];
    INT16 nbyte;
    INT16 remain;
    BOOL full;
} QDP_MEMBLK;

/* Raw meta-data needed for processing DT_DATA packets */

typedef struct {
    struct {
        QDP_MEMBLK token;             /* DP token memory */
        UINT8 combo[QDP_MAX_PAYLOAD]; /* C1_COMBO payload */
        UINT8 epd[QDP_MAX_PAYLOAD]; /* C2_EPD payload */
    } raw;
    QDP_TYPE_C1_COMBO combo; /* decoded raw.combo */
    QDP_DP_TOKEN token;      /* decoded raw.token */
    QDP_TYPE_C2_EPD epd;     /* decoded raw.epd */
    UINT8 state;  /* completeness state */
} QDP_META;

#define QDP_META_TAG_EOF   0xeeee
#define QDP_META_TAG_TOKEN 0xaaaa
#define QDP_META_TAG_COMBO 0xbbbb
#define QDP_META_TAG_EPD   0xcccc
#define QDP_META_STATE_HAVE_MN232 0x01
#define QDP_META_STATE_HAVE_COMBO 0x02
#define QDP_META_STATE_HAVE_TOKEN 0x04
#define QDP_META_STATE_HAVE_EPD   0x08

#define QDP_META_OLDTAG_EOF   0
#define QDP_META_OLDTAG_TOKEN 1
#define QDP_META_OLDTAG_COMBO 2

/* User supplied function for dealing with meta-data */

typedef void (*QDP_META_FUNC)(void *arg, QDP_META *meta);

/* Parameters for initializing a QDP connection */

typedef struct {
    char ident[MAXPATHLEN+1]; /* name to assign to this connection */
    int type;                 /* QIO_UDP, QIO_TTY, or QIO_DIG */
    UINT32 tag;               /* property tag if QDP_UDPIO, else 0 */
    char iostr[MAXPATHLEN+1]; /* iostr for QIO_TTY or QIO_DIG connections */
} QDP_CONNECT;

typedef struct {
    int base;  /* base */
    int value; /* assigned value */
} QDP_CLIENT;

typedef struct {
    QDP_CONNECT connect; /* connection data */
    struct {
        int base;        /* server side base IP port */
        int link;        /* link port: logical data port (1-4), cnf (0), or special (-1) */
        QDP_CLIENT ctrl; /* client side control port */
        QDP_CLIENT data; /* client side data port */
    } port;
    struct {
        UINT32 ctrl;     /* msec read timeout on control port */
        UINT32 data;     /* msec read timeout on data port */
    } timeout;
    struct {
        UINT32 open;     /* msec DT_OPEN on idle data link interval */
        UINT32 retry;    /* msec retry registration interval */
        UINT32 watchdog; /* msec registration watchdog limit (if non-zero) */
    } interval;
    struct {
        UINT32 interval; /* msec status request interval */
        BOOL forward;    /* if TRUE, copy replies to application */
        UINT32 bitmap;   /* C1_RQSTAT bitmap parameter */
    } hbeat;
    UINT64 authcode;     /* authentication code */
    UINT64 serialno;     /* digitizer serial number (0 implies console connection) */
    int nbuf;            /* number of packets to buffer */
    int debug;           /* library log message verbosity */
    MUTEX mutex;         /* for protection */
    struct {
        void *arg;          /* points to user supplied parameters for func */
        QDP_USER_FUNC func; /* user supplied packet saving function */
    } user;
    struct {
        void *arg;          /* points to user supplied parameters for func */
        QDP_META_FUNC func; /* user supplied meta-data saving function */
    } meta;
#define QDP_PAR_OPTION_NONE        0x00000000
#define QDP_PAR_OPTION_QIO_TERSE   0x00000001 /* log byte counts pluse src and destination IP addresses */
#define QDP_PAR_OPTION_QIO_VERBOSE 0x00000002 /* log above info plus hex dump of payload conents */
    UINT32 options;         /* option flags */
} QDP_PAR;

/* Default parameters for both C&C and data links */

#define QDP_DEFAULT_BASEPORT           5330
#define QDP_DEFAULT_DEBUG         QDP_TERSE
#define QDP_DEFAULT_CTRL_TIMEOUT       1000 /*   1 second  */
#define QDP_DEFAULT_DATA_TIMEOUT       1000 /*   1 second  */
#define QDP_DEFAULT_OPEN_INTERVAL    100000 /* 100 second  */
#define QDP_DEFAULT_RETRY_INTERVAL    10000 /*  10 seconds */
#define QDP_DEFAULT_WATCHDOG_INTERVAL 10000 /*  10 seconds */
#define QDP_DEFAULT_HBEAT_FORWARD      TRUE
#define QDP_DEFAULT_HBEAT_BITMAP       QDP_STATUS_DEFAULT_BITMAP
#define QDP_DEFAULT_USER_ARG           NULL
#define QDP_DEFAULT_USER_FUNC          NULL
#define QDP_DEFAULT_META_ARG           NULL
#define QDP_DEFAULT_META_FUNC          NULL
#define QDP_DEFAULT_OPTIONS            QDP_PAR_OPTION_NONE

#define QDP_DEFAULT_CTRL_BASEPORT      6330
#define QDP_DEFAULT_DATA_BASEPORT      7330

/* Default parameters for C&C links */

#define QDP_DEFAULT_CMD_NBUF        10
#define QDP_DEFAULT_CMD_HBEAT_INTERVAL 0 /* no automatic heartbeat */

/* Default parameters for data links */

#define QDP_DEFAULT_DATA_NBUF      (2 * QDP_MAX_WINDOW_NBUF)
#define QDP_DEFAULT_DATA_HBEAT_INTERVAL 30000 /* 30 seconds */

typedef struct {
    MSGQ_BUF pkt;   /* circular buffer of packets */
    MSGQ_BUF event; /* event queue */
    MSGQ_BUF cmd;   /* outbound command queue */
} QDP_BUF;

typedef struct {
    int code;       /* event identifier */
    MSGQ_MSG *msg;  /* if not NULL, a packet from the pkt queue */
    int next_state; /* transition state */
} QDP_EVENT;

typedef struct {
    MSGQ_MSG *msg; /* pending command, if not NULL */
    SEMAPHORE sem; /* to signal command processing complete */
    BOOL ok;       /* FALSE if C1_CERR was received, else TRUE */
    int err;       /* C1_CERR code (if any) */
#define QDP_MAX_CMD_ATTEMPTS 2
    int attempts;  /* number of times we've attempted this command */
} QDP_CMD;

/* Digitizer coordinates */

typedef struct {
    char ident[QDP_IDENT_LEN + 1]; /* serial number */
    UINT32 addr; /* ip address in host byte order*/
    struct {
        int ctrl; /* ip port for control */
        int data; /* ip port for data */
    } port;
} QDP_PEER;

/* Statistics */

typedef struct {
    UINT32 overrun; /* number of packets that failed to make it to application layer */
    UINT32 crc;     /* number of packets dropped for CRC errors */
    UINT32 data;    /* number of data packets received */
    UINT32 dupdata; /* number of duplicate data packets received */
    UINT32 fill;    /* number of fill packets received */
    UINT32 drop;    /* number of spurious packets dropped */
    UINT32 retry;   /* number of command retries */
} QDP_COUNTER;

typedef struct {
    MUTEX mutex;    /* for protection */
    UINT64 tstamp;  /* time when last registered */
    UINT32 nreg;    /* number of times we've registered with the digitizer */
    UINT64 uptime;  /* used when processing a snapshot of this structure */
    struct {
        QDP_COUNTER session;  /* counts for this registration instance */
        QDP_COUNTER total;    /* counts for the life of this application */
    } counter;
} QDP_STATS;

/* Reorder buffer */

typedef struct {
    UINT32 ack[4];
    UINT16 ack_seqno;
    UINT16 last_packet;
    MSGQ_MSG *msg[QDP_MAX_WINDOW_NBUF];
    UTIL_TIMER timer;
    UINT16 count;
    UINT64 ack_to;
} QDP_DTBUF;

/* Stuff needed to manage the Finite State Automaton */

typedef struct {
    MUTEX mutex; /* for protection */
    int nthread; /* number of active FSA threads */
    struct {
        int initial;
        int current;
        int previous;
    } state;
    struct {
        SEMAPHORE Automaton;
        SEMAPHORE WatchdogThread;
    } semaphore;
    QDP_TYPE_C1_SRVCH srvch; /* Server challenge received from Q330 */
    QDP_PKT pkt;             /* holds outgoing registration commands */
    BOOL tld;                /* TRUE when automaton is shutting down */
} QDP_FSA;

/* Handle for reliable QDP I/O */

#define QDP_FLAGS_NONE         0x00000000
#define QDP_FLAGS_EP_SUPPORTED 0x00000001

typedef struct {
    QIO ctrl;                /* control port */
    QIO data;                /* data port */
    QDP_PAR par;             /* connection parameters */
    QDP_PEER peer;           /* digitizer coordinates */
    QDP_BUF Q;               /* packet and event buffer message queues */
    UINT16 seqno;            /* outbound sequence number */
    LOGIO *lp;               /* for logging */
    UINT64 start;            /* system start time stamp */
    QDP_CMD cmd;             /* holds pending user command */
    QDP_STATS stats;         /* statistics */
    QDP_DTBUF recv;          /* reorder buffer */
    BOOL dataLink;           /* TRUE for data connections */
    UTIL_TIMER open;         /* DT_OPEN interval timer */
    QDP_FSA fsa;             /* automaton stuff */
    QDP_TYPE_C1_FIX c1_fix;  /* C1_FIX from handshake stored here */
    QDP_TYPE_C1_MEM c1_mem;  /* C1_MEM packets (for data port connections) decoded here */
    QDP_META meta;           /* meta-data */
    UINT32 reboots;          /* number of reboots pulled from most recent handshake C1_FIX */
    int errcode;             /* library error codes */
    int suberr;              /* C1_CERR code or automaton state at time of errcode error */
    QDP_PKT *dbgpkt;         /* if not NULL, use this for I/O so apps can debug stuff */
    UINT32 flags;            /* Session options */
} QDP;

/* macros for simple Q330 commands */

#define qdpReboot(qp, wait)                 qdpCtrl(qp, QDP_CTRL_REBOOT, wait)
#define qdpGPSOn(qp, wait)                  qdpCtrl(qp, QDP_CTRL_GPS_ON, wait)
#define qdpGPSOff(qp, wait)                 qdpCtrl(qp, QDP_CTRL_GPS_OFF, wait)
#define qdpGPSColdStart(qp, wait)           qdpCtrl(qp, QDP_CTRL_GPS_COLDSTART, wait)
#define qdpResync(qp, wait)                 qdpCtrl(qp, QDP_CTRL_GPS_RESYNC, wait)
#define qdpSaveCurrentProgramming(qp, wait) qdpCtrl(qp, QDP_CTRL_SAVE, wait)

/* and some other macros, for software convenience */

#define qdpLenBT_38(byte)  ((int) 4)
#define qdpLenBT_816(byte) ((int) 4)
#define qdpLenBT_316(byte) ((int) 8)
#define qdpLenBT_32(byte)  ((int) 8)
#define qdpLenBT_232(byte) ((int) 12)
#define qdpLenBT_MULT(byte) qdpLenBT_COMP(byte)

#define QDP_UNDEFINED_SERIALNO 0x00000000
#define QDP_INVALID_SERIALNO   0xFFFFFFFF
#define qdpIsValidSerialno(serial) (((serial) == QDP_INVALID_SERIALNO) ? FALSE : TRUE)

/* Checkout packets */

typedef struct {
    char ident[QDP_IDENTLEN+1];
    QDP_TYPE_C1_FIX fix;
    QDP_TYPE_C1_GLOB glob;
    QDP_TYPE_C1_DCP dcp;
    UINT32 sc[QDP_NSC];
    QDP_TYPE_C1_GID gid[QDP_NGID];
    QDP_TYPE_C2_GPS gps;
    QDP_TYPE_C2_AMASS amass;
    QDP_TYPE_C1_STAT status;
    QDP_TYPE_C1_PHY phy;
    QDP_TYPE_C1_SPP spp;
    QDP_TYPE_C1_MAN man;
} QDP_TYPE_CHECKOUT;

/* For the monitor app */

typedef struct {
    QDP_TYPE_C1_STAT stat;
    QDP_TYPE_C1_FIX  fix;
    UINT32           sc[QDP_NSC];
    QDP_TYPE_C2_GPS  gps;
    QDP_TYPE_C1_SPP  spp;
    QDP_TYPE_C1_GLOB glob;
} QDP_TYPE_MONITOR;

/* Complete logical (data) port */

typedef struct {
    QDP_TYPE_C1_LOG log;
    QDP_DP_TOKEN token;
#define QDP_DPORT_ERROR_NONE   0x0000
#define QDP_DPORT_ERROR_NOSINT 0x0001 /* some tokens are for sample rates that don't exist */
    UINT32 error;
#define QDP_DPORT_DEFINED_LOG    0x0001
#define QDP_DPORT_DEFINED_TOKEN  0x0002
    int set; /* QDP_DPORT_DEFINED_x bitmask */
    BOOL valid; /* when both QDP_DPORT_DEFINED_LOG and QDP_DPORT_DEFINED_TOKEN are set */
} QDP_TYPE_DPORT;

/* Complete Q330 configuration */

#define QDP_XML_WRITER_MAXSTRLEN 63
typedef struct {
    char proto_ver[QDP_XML_WRITER_MAXSTRLEN+1];
    char name[QDP_XML_WRITER_MAXSTRLEN+1];
    char soft_ver[QDP_XML_WRITER_MAXSTRLEN+1];
    char created[QDP_XML_WRITER_MAXSTRLEN+1];
    char updated[QDP_XML_WRITER_MAXSTRLEN+1];
} QDP_TYPE_XML_WRITER;

#define QDP_CONFIG_DEFINED_WRITER     0x00000001
#define QDP_CONFIG_DEFINED_GLOBAL     0x00000002
#define QDP_CONFIG_DEFINED_INTERFACES 0x00000004
#define QDP_CONFIG_DEFINED_SENSCTRL   0x00000008
#define QDP_CONFIG_DEFINED_SLAVE      0x00000010
#define QDP_CONFIG_DEFINED_ADVSER1    0x00000020
#define QDP_CONFIG_DEFINED_ADVSER2    0x00000040
#define QDP_CONFIG_DEFINED_ADVSER3    0x00000080
#define QDP_CONFIG_DEFINED_ADVETHER   0x00000100
#define QDP_CONFIG_DEFINED_GPS        0x00000200
#define QDP_CONFIG_DEFINED_PLL        0x00000400
#define QDP_CONFIG_DEFINED_AUTOMASS   0x00000800
#define QDP_CONFIG_DEFINED_ANNOUNCE   0x00001000
#define QDP_CONFIG_DEFINED_WEBPAGE    0x00002000
#define QDP_CONFIG_DEFINED_BCFG       0x00004000
#define QDP_CONFIG_DEFINED_EPCFG      0x00008000

typedef struct {
    QDP_TYPE_C1_FIX fix;
    QDP_TYPE_C1_GLOB glob;
    QDP_TYPE_C1_PHY phy;
    QDP_TYPE_C1_SPP spp;
    QDP_TYPE_C2_PHY phy2[QDP_PP_NUMPHY];
    QDP_TYPE_C2_AMASS amass;
    QDP_TYPE_C2_GPS gps;
    UINT32 sc[QDP_NSC];
    QDP_TYPE_C3_ANNC annc;
    QDP_TYPE_C3_BCFG bcfg;
    QDP_TYPE_C2_EPCFG epcfg;
    LNKLST webpage;
    QDP_TYPE_DPORT dport[QDP_NLP];
#define QDP_FULL_CONFIG_SIG_LEN 15
    char signature[QDP_FULL_CONFIG_SIG_LEN+1]; /* typing monkeys validity flag */
    QDP_TYPE_XML_WRITER writer;
    char *basetag;
    UINT32 set; /* bit mask of QDP_CONFIG_DEFINED_x values */
} QDP_TYPE_FULL_CONFIG;

/* Logical channel queues are off in its own file */

#include "qdp/lcq.h"

/* function prototypes */

/* clock.c */
UINT8 qdpClockQuality(QDP_TOKEN_CLOCK *dp_clock, QDP_MN232_DATA *mn232);

/* cmds.c */
BOOL qdpPostCmd(QDP *qp, QDP_PKT *pkt, BOOL wait);
BOOL qdpDeregister(QDP *qp, BOOL wait);
BOOL qdpRqflgs(QDP *qp, BOOL wait);
BOOL qdpCtrl(QDP *qp, UINT16 flags, BOOL wait);
BOOL qdpNoParCmd(QDP *qp, int command, BOOL wait);
BOOL qdpRqstat(QDP *qp, UINT32 bitmap, BOOL wait);
BOOL qdpMemCmd(QDP *qp, QDP_PKT *pkt, UINT32 delay, int ntry, FILE *fp);
BOOL qdpFlushDataPort(QDP *qdp, int which, BOOL wait);

/* config.c */
BOOL qdpInitConfig(QDP_TYPE_FULL_CONFIG *config);
BOOL qdpRequestDportConfig(QDP *qdp, QDP_TYPE_DPORT *dport, UINT16 port);
BOOL qdpRequestFullConfig(QDP *qdp, QDP_TYPE_FULL_CONFIG *config);
BOOL qdpConfigureDataPort(QDP *qdp, QDP_TYPE_DPORT *dport, UINT16 port, QDP_TYPE_C1_FIX *fix, UINT32 delay, int ntry);
BOOL qdpSetConfig(QDP *qdp, QDP_TYPE_FULL_CONFIG *config, BOOL verbose);

/* connect.c */
QDP *qdpConnect(QDP_PAR *par, LOGIO *lp, int *errcode, int *suberr);

/* crc.c */
UINT32 qdpCRC(UINT8 *buf, int len);
BOOL qdpVerifyCRC(QDP_PKT *pkt);

/* data.c */
int qdpLenBT_COMP(UINT8 *start);
int qdpLenBT_SPEC(UINT8 *start);
void qdpLoadMN232(QDP_BLOCKETTE_232 *blk, QDP_MN232_DATA *dest);
BOOL qdpDecode_DT_DATA(UINT8 *start, UINT16 dlen, QDP_TYPE_DT_DATA *dest);
BOOL qdpHaveCalNotifyBlockette(UINT8 *start, UINT16 dlen, UINT8 *abort, UINT16 *bitmap);

/* debug.c */
void qdpDebug2(QDP *qp, char *format, ...);
void qdpDebug(QDP *qp, char *format, ...);
void qdpInfo(QDP *qp, char *format, ...);
void qdpWarn(QDP *qp, char *format, ...);
void qdpError(QDP *qp, char *format, ...);
void qdpLcqDebug(QDP_LCQ *lcq, char *format, ...);
void qdpLcqInfo(QDP_LCQ *lcq, char *format, ...);
void qdpLcqWarn(QDP_LCQ *lcq, char *format, ...);
void qdpLcqError(QDP_LCQ *lcq, char *format, ...);

/* decode.c */
void qdpDecode_C1_SRVCH(UINT8 *start, QDP_TYPE_C1_SRVCH *dest);
void qdpDecode_C1_SRVRSP(UINT8 *start, QDP_TYPE_C1_SRVRSP *dest);
void qdpDecode_C1_RQMEM(UINT8 *start, QDP_TYPE_C1_RQMEM *dest);
void qdpDecode_C1_MEM(UINT8 *start, QDP_TYPE_C1_MEM *dest);
void qdpDecode_C1_FIX(UINT8 *start, QDP_TYPE_C1_FIX *dest);
void qdpDecode_C1_GID(UINT8 *start, QDP_TYPE_C1_GID *dest);
void qdpDecode_C1_SPP(UINT8 *start, QDP_TYPE_C1_SPP *dest);
void qdpDecode_C1_MYSN(UINT8 *start, QDP_TYPE_C1_MYSN *dest);
void qdpDecode_C1_MAN(UINT8 *start, QDP_TYPE_C1_MAN *dest);
void qdpDecode_C1_GLOB(UINT8 *start, QDP_TYPE_C1_GLOB *dest);
void qdpDecode_C1_LOG(UINT8 *start, QDP_TYPE_C1_LOG *dest);
void qdpDecode_C1_COMBO(UINT8 *start, QDP_TYPE_C1_COMBO *dest);
void qdpDecode_C1_PHY(UINT8 *start, QDP_TYPE_C1_PHY *dest);
void qdpDecode_C1_SC(UINT8 *start, UINT32 *dest);
void qdpDecode_C1_DCP(UINT8 *start, QDP_TYPE_C1_DCP *dest);
void qdpDecode_C2_AMASS(UINT8 *start, QDP_TYPE_C2_AMASS *dest);
void qdpDecode_C2_GPS(UINT8 *start, QDP_TYPE_C2_GPS *dest);
void qdpDecode_C2_PHY(UINT8 *start, QDP_TYPE_C2_PHY *dest);
BOOL qdpDecode_C3_ANNC(UINT8 *start, QDP_TYPE_C3_ANNC *dest);
void qdpDecodeRawEpDelay(QDP_EP_DELAY *delay);
BOOL qdpDecode_C2_EPD(UINT8 *start, QDP_TYPE_C2_EPD *dest);
void qdpDecodeRawEpChanmask(QDP_EP_CHANMASK *chanmask);
BOOL qdpDecode_C2_EPCFG(UINT8 *start, QDP_TYPE_C2_EPCFG *dest);
BOOL qdpDecode_SSCFG(UINT8 *start, QDP_TYPE_SSCFG *dest);
int qdpDecodeCMNHDR(UINT8 *start, QDP_CMNHDR *hdr);

/* destroy.c */
void qdpDestroyHandle(QDP *qp);

/* encode.c */
void qdpEncode_C1_SRVRSP(QDP_PKT *pkt, QDP_TYPE_C1_SRVRSP *src);
void qdpEncode_C1_RQSRV(QDP_PKT *pkt, UINT64 serialno);
void qdpEncode_C1_DSRV(QDP_PKT *pkt, UINT64 serialno);
void qdpEncode_C1_RQFLGS(QDP_PKT *pkt, UINT16 dataport);
void qdpEncode_C1_RQLOG(QDP_PKT *pkt, INT16 port);
void qdpEncode_C1_RQSTAT(QDP_PKT *pkt, UINT32 bitmap);
void qdpEncode_C1_CTRL(QDP_PKT *pkt, UINT16 flags);
void qdpEncode_C1_RQMEM(QDP_PKT *pkt, QDP_TYPE_C1_RQMEM *src);
void qdpEncode_C2_RQPHY(QDP_PKT *pkt, INT16 port);
void qdpEncode_C1_QCAL(QDP_PKT *pkt, QDP_TYPE_C1_QCAL *src);
void qdpEncode_NoParCmd(QDP_PKT *pkt, int command);
void qdpEncode_C1_LOG(QDP_PKT *pkt, QDP_TYPE_C1_LOG *log, UINT16 port);
void qdpEncode_C1_PHY(QDP_PKT *pkt, QDP_TYPE_C1_PHY *phy);
void qdpEncode_C1_PULSE(QDP_PKT *pkt, UINT16 bitmap, UINT16 duration);
void qdpEncode_C1_SC(QDP_PKT *pkt, UINT32 *sc);
void qdpEncode_C1_GLOB(QDP_PKT *pkt, QDP_TYPE_C1_GLOB *glob);
void qdpEncode_C1_SPP(QDP_PKT *pkt, QDP_TYPE_C1_SPP *spp);
void qdpEncode_C1_POLLSN(QDP_PKT *pkt, UINT16 mask, UINT16 match);
void qdpEncode_C2_AMASS(QDP_PKT *pkt, QDP_TYPE_C2_AMASS *amass);
void qdpEncode_C2_GPS(QDP_PKT *pkt, QDP_TYPE_C2_GPS *gps);
void qdpEncode_C2_RQPHY(QDP_PKT *pkt, INT16 port);
void qdpEncode_C2_PHY(QDP_PKT *pkt, QDP_TYPE_C2_PHY *phy2);
void qdpEncode_C3_ANNC(QDP_PKT *pkt, QDP_TYPE_C3_ANNC *annc);
void qdpEncode_C3_RQANNC(QDP_PKT *pkt);
void qdpEncode_C2_EPCFG(QDP_PKT *pkt, QDP_TYPE_C2_EPCFG *epcfg);

/* fsa.c */
int qdpFsaState(QDP *qp);
BOOL qdpStartFSA(QDP *qp);
BOOL qdpShutdown(QDP *qp);

/* hlp.c */
void qdpFlushHLP(QDP_LCQ *lcq, QDP_LC *lc);
void qdpFlushLCQ(QDP_LCQ *lcq);
BOOL qdpBuildHLP(QDP_LCQ *lcq);
BOOL qdpInitHLPRules(QDP_HLP_RULES *dest, UINT32 maxbyte, UINT8 format, QDP_HLP_FUNC func, void *args, UINT32 flags);
void qdpDestroyHLP(QDP_HLP *hlp);
BOOL qdpInitHLP(QDP_HLP_RULES *rules, QDP_LC *lc, QDP_META *meta);

/* ida10.c */
BOOL qdpHlpToIDA10(UINT8 *start, QDP_HLP *hlp, UINT32 seqno);

/* init.c */
void qdpInitMemBlock(QDP *qp, UINT16 type);
BOOL qdpInit(QDP *qp, QDP_PAR *par, LOGIO *lp);

/* io.c */
void qdpHostToNet(QDP_PKT *pkt);
void qdpNetToHost(QDP_PKT *pkt);
MSGQ_MSG *qdpRecvPkt(QDP *qp, QIO *up, int *status);
void qdpSendPkt(QDP *qp, int which, QDP_PKT *pkt);
void qdpLogQdp(QDP *qdp);

/* md5.c */
void qdpMD5(QDP_TYPE_C1_SRVRSP *c1_srvrsp, UINT64 auth);

/* mem.c */
void qdpInitMemBlk(QDP_MEMBLK *blk, UINT16 type);
BOOL qdpSaveMem(QDP_MEMBLK *blk, QDP_TYPE_C1_MEM *mem);
void qdpGetMemBlk(QDP *qp, QDP_MEMBLK *mem);
BOOL qdpPostMemBlk(QDP *qp, QDP_MEMBLK *mem, UINT32 delay, int ntry, FILE *fp);

/* meta.c */
int qdpWriteMeta(FILE *fp, QDP_META *meta);
int qdpReadMeta(FILE *fp, QDP_META *meta);
int qdpReadMetaFile(char *path, QDP_META *meta);
INT32 qdpLookupFilterDelay(QDP_META *meta, UINT8 *src);
UINT64 qdpRootTimeTag(QDP_MN232_DATA *mn232);
UINT64 qdpDelayCorrectedTime(QDP_MN232_DATA *mn232, INT32 delay);

/* misc.c */
void qdpConvertSampleRate(INT16 rate, REAL64 *frate, REAL64 *dsint, UINT64 *nsint);
int qdpMainDigitizerChanNum(UINT8 channel);
void qdpUpdateDataPortChannelFreqs(QDP_TYPE_DPORT *dport, QDP_TYPE_C1_FIX *fix);
int qdpCalState(QDP_PKT *pkt, UINT8 *abort);

/* msgq.c */
MSGQ_MSG *qdpGetEmptyPktMsg(char *fid, QDP *qp, int wait);
void qdpReturnPktMsgToHeap(char *fid, QDP *qp, MSGQ_MSG *msg);
void qdpForwardFullPktMsg(char *fid, QDP *qp, MSGQ_MSG *msg);
MSGQ_MSG *qdpGetFullPktMsg(char *fid, QDP *qp, int wait);
BOOL qdpNextPkt(QDP *qp, QDP_PKT *dest);

/* par.c */
BOOL qdpValidPort(int port);
int qdpDataPortNumber(int port);
void qdpInitPar(QDP_PAR *par, int port);
BOOL qdpSetConnect(QDP_PAR *par, QDP_CONNECT *connect);
BOOL qdpSetBaseport(QDP_PAR *par, int value);
BOOL qdpSetCtrlBaseport(QDP_PAR *par, int value);
BOOL qdpSetDataBaseport(QDP_PAR *par, int value);
BOOL qdpSetMyCtrlPort(QDP_PAR *par, int value);
BOOL qdpSetMyDataPort(QDP_PAR *par, int value);
BOOL qdpSetAuthcode(QDP_PAR *par, UINT64 value);
BOOL qdpSetSerialno(QDP_PAR *par, UINT64 value);
BOOL qdpSetCtrlTimeout(QDP_PAR *par, UINT32 value);
BOOL qdpSetDataTimeout(QDP_PAR *par, UINT32 value);
BOOL qdpSetRetryInterval(QDP_PAR *par, UINT32 value);
BOOL qdpSetOpenInterval(QDP_PAR *par, UINT32 value);
BOOL qdpSetWatchdogInterval(QDP_PAR *par, UINT32 value);
BOOL qdpSetHeartbeat(QDP_PAR *par, UINT32 interval, UINT32 bitmap, BOOL forward);
BOOL qdpSetDebug(QDP_PAR *par, int value);
BOOL qdpSetUser(QDP_PAR *par, void *arg, QDP_USER_FUNC func);
BOOL qdpSetMeta(QDP_PAR *par, void *arg, QDP_META_FUNC func);
BOOL qdpSetOptionFlags(QDP_PAR *par, UINT32 options);
BOOL qdpAddOption(QDP_PAR *par, UINT32 options);
BOOL qdpSetMaxtry(QDP_PAR *par, UINT32 maxsrq);
void qdpLogPar(QDP *qp, QDP_PAR *par);
void qdpPrintPar(FILE *fp, QDP_PAR *par);

/* pkts.c */
int qdpErrorCode(QDP_PKT *pkt);
void qdpInitPkt(QDP_PKT *pkt, UINT8 cmd, UINT16 seqno, UINT16 ack);
void qdpCopyPkt(QDP_PKT *dest, QDP_PKT *src);
int qdpReadPkt(FILE *fp, QDP_PKT *pkt);
int qdpReadPktgz(gzFile *fp, QDP_PKT *pkt);
int qdpWritePkt(FILE *fp, QDP_PKT *pkt);
BOOL qdp_C1_RQGLOB(QDP *qdp, QDP_TYPE_C1_GLOB *glob);
BOOL qdp_C1_RQSC(QDP *qdp, UINT32 *sc);
BOOL qdp_C1_RQPHY(QDP *qdp, QDP_TYPE_C1_PHY *phy);
BOOL qdp_C1_RQFIX(QDP *qdp, QDP_TYPE_C1_FIX *fix);
BOOL qdp_C1_RQLOG(QDP *qdp, QDP_TYPE_C1_LOG *log, INT16 port);
BOOL qdp_C1_RQGID(QDP *qdp, QDP_TYPE_C1_GID *gid);
BOOL qdp_C1_RQDCP(QDP *qdp, QDP_TYPE_C1_DCP *dcp);
BOOL qdp_C1_RQSPP(QDP *qdp, QDP_TYPE_C1_SPP *spp);
BOOL qdp_C1_RQMAN(QDP *qdp, QDP_TYPE_C1_MAN *man);
BOOL qdp_C2_RQAMASS(QDP *qdp, QDP_TYPE_C2_AMASS *amass);
BOOL qdp_C2_RQPHY(QDP *qdp, QDP_TYPE_C2_PHY *phy, INT16 port);
BOOL qdp_C2_RQGPS(QDP *qdp, QDP_TYPE_C2_GPS *gps);
BOOL qdp_C2_RQEPD(QDP *qdp, QDP_TYPE_C2_EPD *epd);
BOOL qdp_C2_RQEPCFG(QDP *qdp, QDP_TYPE_C2_EPCFG *epcfg);
BOOL qdp_C1_RQCOMBO(QDP *qdp, QDP_TYPE_C1_COMBO *combo, INT16 port);
BOOL qdp_C1_RQSTAT(QDP *qp, UINT32 bitmap, QDP_TYPE_C1_STAT *stat);
BOOL qdp_C1_RQMEM(QDP *qdp, UINT16 type, QDP_MEMBLK *blk);
BOOL qdp_C3_RQANNC(QDP *qdp, QDP_TYPE_C3_ANNC *annc);
BOOL qdpRequestTokens(QDP *qdp, QDP_DP_TOKEN *token, UINT16 port);
BOOL qdpRequestCheckout(QDP *qdp, QDP_TYPE_CHECKOUT *co, char *ident);
BOOL qdpRequestMonitor(QDP *qdp, QDP_TYPE_MONITOR *mon);
BOOL qdpRequestWebpage(QDP *qdp, LNKLST *webpage);

/* print.c */
int qdpBaud(int code);
int qdpBaudCode(int value);
void qdpPrintBT_SPEC(FILE *fp, QDP_BLOCKETTE_SPEC *blk);
void qdpPrintBT_38(FILE *fp, QDP_BLOCKETTE_38 *blk);
void qdpPrintBT_816(FILE *fp, QDP_BLOCKETTE_816 *blk);
void qdpPrintBT_316(FILE *fp, QDP_BLOCKETTE_316 *blk);
void qdpPrintBT_32(FILE *fp, QDP_BLOCKETTE_32 *blk);
void qdpPrintBT_232(FILE *fp, QDP_BLOCKETTE_232 *blk);
void qdpPrintBT_COMP(FILE *fp, QDP_BLOCKETTE_COMP *blk);
void qdpPrintBT_MULT(FILE *fp, QDP_BLOCKETTE_MULT *blk);
void qdpPrintBlockette(FILE *fp, QDP_DT_BLOCKETTE *blk, UINT32 seqno);
void qdpPrintBlockettes(FILE *fp, QDP_TYPE_DT_DATA *dt_data);
void qdpPrintTokenVersion(FILE *fp, QDP_TOKEN_VERSION *src);
void qdpPrintTokenSite(FILE *fp, QDP_TOKEN_SITE *src);
void qdpPrintTokenServer(FILE *fp, QDP_TOKEN_SERVER *src);
void qdpPrintTokenServices(FILE *fp, QDP_TOKEN_SERVICES *src);
void qdpPrintTokenDss(FILE *fp, QDP_TOKEN_DSS *src);
void qdpPrintTokenClock(FILE *fp, QDP_TOKEN_CLOCK *src);
void qdpPrintTokenLogid(FILE *fp, QDP_TOKEN_LOGID *src);
void qdpPrintTokenCnfid(FILE *fp, QDP_TOKEN_CNFID *src);
void qdpPrintTokenLcq(FILE *fp, QDP_TOKEN_LCQ *src);
void qdpPrintTokenLcqList(FILE *fp, LNKLST *src);
void qdpPrintTokenIir(FILE *fp, QDP_TOKEN_IIR *iir);
void qdpPrintTokenIirList(FILE *fp, LNKLST *src);
void qdpPrintTokenFir(FILE *fp, QDP_TOKEN_FIR *fir);
void qdpPrintTokenFirList(FILE *fp, LNKLST *src);
void qdpPrintTokenCds(FILE *fp, QDP_TOKEN_CDS *cds);
void qdpPrintTokenCdsList(FILE *fp, LNKLST *src);
void qdpPrintTokenDet(FILE *fp, QDP_TOKEN_DET *det);
void qdpPrintTokenDetList(FILE *fp, LNKLST *src);
void qdpPrintTokenCenList(FILE *fp, LNKLST *src);
void qdpPrintTokens(FILE *fp, QDP_DP_TOKEN *src);
void qdpPrintTokenReport(FILE *fp, QDP_DP_TOKEN *src);
void qdpPrint_C1_FIX(FILE *fp, QDP_TYPE_C1_FIX *fix);
void qdpPrint_C1_GID(FILE *fp, QDP_TYPE_C1_GID *gid);
void qdpPrint_C1_GLOB(FILE *fp, QDP_TYPE_C1_GLOB *glob);
void qdpPrintInputBitmap(FILE *fp, UINT16 bitmap);
void qdpPrint_C1_LOG(FILE *fp, QDP_TYPE_C1_LOG *log);
void qdpPrint_C1_COMBO(FILE *fp, QDP_TYPE_C1_COMBO *combo);
void qdpPrint_C1_PHY(FILE *fp, QDP_TYPE_C1_PHY *phy);
void qdpPrint_C1_SC(FILE *fp, UINT32 *sc);
void qdpPrint_C1_DCP(FILE *fp, QDP_TYPE_C1_DCP *dcp);
void qdpPrint_C1_DCPMAN(FILE *fp, QDP_TYPE_C1_DCP *dcp, QDP_TYPE_C1_MAN *man);
void qdpPrint_C1_SPP(FILE *fp, QDP_TYPE_C1_SPP *spp);
void qdpPrint_C1_MAN(FILE *fp, QDP_TYPE_C1_MAN *man);
void qdpPrint_C2_AMASS_SENSOR(FILE *fp, QDP_TYPE_C2_AMASS_SENSOR *sensor);
void qdpPrint_C2_AMASS(FILE *fp, QDP_TYPE_C2_AMASS *amass);
void qdpPrint_C2_GPS(FILE *fp, QDP_TYPE_C2_GPS *gps);
void qdpPrintCheckoutPackets(FILE *fp, QDP_TYPE_CHECKOUT *co);
void qdpPrintMonitorPackets(FILE *fp, QDP_TYPE_MONITOR *mon);
void qdpPrint_CMNHDR(FILE *fp, QDP_CMNHDR *hdr);
void qdpPrintPkt(FILE *fp, QDP_PKT *pkt, UINT16 print);
void qdpPrintLCQ(FILE *fp, QDP_LCQ *lcq, UINT16 print);
void qdpPrint_SSCFG(FILE *fp, QDP_TYPE_SSCFG *sscfg);
void qdpPrint_C2_EPD(FILE *fp, QDP_TYPE_C2_EPD *epd);
void qdpPrint_C2_EPCFG(FILE *fp, QDP_TYPE_C2_EPCFG *epcfg, QDP_DP_TOKEN *token);
void qdpPrintUnsupportedQEP(FILE *fp, QDP *qdp);

/* process.c */
BOOL qdpProcessPacket(QDP_LCQ *lcq, QDP_PKT *pkt);
BOOL qdpInitLCQ(QDP_LCQ *lcq, QDP_LCQ_PAR *par);
void qdpDestroyLCQ(QDP_LCQ *lcq);

/* pt.c */
int qdpPackTokenMemblk(QDP_MEMBLK *dest, QDP_DP_TOKEN *token, UINT16 type);

/* stats.c */
void qdpInitStats(QDP *qp);
void qdpSetStatsTstamp(QDP *qp);
void qdpResetSessionStats(QDP *qp);
void qdpIncrStatsNreg(QDP *qp);
void qdpIncrStatsOverrun(QDP *qp);
void qdpIncrStatsCrc(QDP *qp);
void qdpIncrStatsDrop(QDP *qp);
void qdpIncrStatsDupData(QDP *qp);
void qdpIncrStatsData(QDP *qp);
void qdpIncrStatsFill(QDP *qp);
void qdpIncrStatsRetry(QDP *qp);
void qdpGetStats(QDP *qp, QDP_STATS *dest);
UINT64 qdpSessionUptime(QDP *qp);
void qdpLogStats(QDP *qp);
void qdpPrintStats(FILE *fp, QDP *qp);

/* status.c */
BOOL qdpClear_C1_STAT(QDP_TYPE_C1_STAT *dest);
BOOL qdpInit_C1_STAT(QDP_TYPE_C1_STAT *dest);
void qdpPrint_C1_STAT(FILE *fp, QDP_TYPE_C1_STAT *stat);
BOOL qdpDecode_C1_STAT(UINT8 *start, QDP_TYPE_C1_STAT *dest);
BOOL qdpCalibrationInProgress(QDP_TYPE_C1_STAT *stat);
UINT32 qdpGpsDateTimeToEpoch(char *date, char *time);
REAL64 qdpGpsStringToDecDeg(char *string);

/* sc.c */
BOOL qdpUpdateSensorControlBitmap(UINT16 *bitmap, UINT32 *sc, int line);

/* steim.c */
int qdpDecompressCOMP(QDP_BLOCKETTE_COMP *blk, INT32 *out, UINT32 maxsamp, UINT32 *nsamp);
int qdpDecompressENVDAT(QDP_SP_ENVDAT *blk, INT32 *out, UINT32 maxsamp, UINT32 *nsamp);

/* string.c */
char *qdpTerseStateString(UINT8 code);
char *qdpStateString(UINT8 code);
char *qdpTerseEventString(UINT8 code);
char *qdpEventString(UINT8 code);
char *qdpActionString(UINT8 code);
char *qdpCmdString(UINT8 code);
char *qdpTercmdString(UINT8 code);
int qdpCmdCode(char *string);
int qdpTercmdCode(char *string);
char *qdpErrString(UINT16 code);
char *qdpCtrlString(UINT8 code);
char *qdpPortString(int code);
char *qdpDataPortString(int code);
char *qdpDebugString(UINT8 code);
char *qdpMemTypeString(UINT8 code);
char *qdpBlocketteIdentString(UINT8 code);
char *qdpPLLStateString(UINT16 code);
char *qdpPhysicalPortString(UINT16 code);
char *qdpSensorControlString(UINT32 code);
char *qdpSensorControlString2(UINT32 code);
char *qdpAuxtypeString(UINT16 code);
char *qdpClocktypeString(UINT16 code);
char *qdpDetectorString(UINT16 code);
char *qdpCalibratorString(UINT16 code);
char *qdpWebPageMacro(UINT8 code);
char *qdpLogicalOperatorString(UINT8 code);
int qdpLogicalOperatorCode(char *string);
char *qdpSSPortString(UINT16 code);
char *qdpSSTypeString(UINT16 code);
char *qdpSSUnitsString(UINT16 code);
char *qdpTokenTypeString(UINT8 code);
char *qdpErrcodeString(int code);
char *qdpPreampMuxString(int code);
char *qdpInputMuxString(int code);
char *qdpInputMuxString2(int code);
int qdpInputMuxCode(char *string);
char *qdpSdiPhaseString(int code);
char *qdpSdiDriverString(int code);
char *qdpEPSubchanString(int code);
char *qdpInputBitmapString(UINT16 bitmap, char *buf);
char *qdpPreampBitmapString(UINT16 bitmap, char *buf);

/* tokens.c */
void qdpSetTokenIdent(QDP_TOKEN_IDENT *ident, UINT8 code, char *name);
void qdpInitTokenDet(QDP_TOKEN_DET *det);
void qdpInitLcq(QDP_TOKEN_LCQ *lcq);
void qdpInitTokens(QDP_DP_TOKEN *token);
BOOL qdpUnpackTokens(QDP_MEMBLK *blk, QDP_DP_TOKEN *token);
void qdpInitTokens(QDP_DP_TOKEN *token);
void qdpClearTokens(QDP_DP_TOKEN *token);
void qdpDestroyTokens(QDP_DP_TOKEN *token);
void qdpSetLcqIdentName(QDP_TOKEN_LCQ *lcq);
UINT8 qdpLcqCode(QDP_DP_TOKEN *token, char *name);
char *qdpLcqName(QDP_DP_TOKEN *token, int number);
UINT8 qdpFirFilterCode(QDP_DP_TOKEN *token, char *name);
char *qdpFirFilterName(QDP_DP_TOKEN *token, int number);
UINT8 qdpIirFilterCode(QDP_DP_TOKEN *token, char *name);
char *qdpIirFilterName(QDP_DP_TOKEN *token, int number);
UINT8 qdpControlDetectorCode(QDP_DP_TOKEN *token, char *name);
char *qdpControlDetectorName(QDP_DP_TOKEN *token, int number);
UINT8 qdpEventDetectorCode(QDP_DP_TOKEN *token, char *name);
char *qdpEventDetectorName(QDP_DP_TOKEN *token, int number);
char *qdpCommEventName(QDP_DP_TOKEN *token, int number);
UINT8 qdpControlDetectorFilterCode(QDP_DP_TOKEN *token, char *string);
char *qdpControlDetectorFilterName(QDP_DP_TOKEN *token, int number, char *buf);
BOOL qdpFinalizeToken(QDP_DP_TOKEN *token);

/* webpage.c */
BOOL qdpRequestWebpage(QDP *qdp, LNKLST *list);
int qdpPackWebpageMemblk(QDP_MEMBLK *dest, QDP_TYPE_FULL_CONFIG *config);

/* version.c */
char *qdpVersionString(void);
VERSION *qdpVersion(void);

#ifdef __cplusplus
}
#endif

#endif /* qdp_h_included */

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
 * $Log: qdp.h,v $
 * Revision 1.98  2017/10/11 20:22:50  dechavez
 * added qdpPrintUnsupportedQEP() prototype
 *
 * Revision 1.97  2017/09/14 00:21:30  dauerbach
 * updated prototypes
 *
 * Revision 1.96  2017/09/06 21:52:13  dechavez
 * added qdpGpsStringToDecDeg() prototype
 *
 * Revision 1.95  2017/02/03 20:44:38  dechavez
 * added qdpTerseStateString() prototype
 *
 * Revision 1.94  2017/01/30 23:39:22  dechavez
 * Defined QDP_META_OLDTAG_EOF/TOKEN/COMBO to restore the old constants used in
 * libqdp 3.10.1 and before.  This to permit qpp to read meta-data files tagged
 * with both the old and new constants.
 *
 * Revision 1.93  2016/08/19 22:43:08  dechavez
 * changed QDP_DEFAULT_CTRL_TIMEOUT to 1 second, eliminated QDP_DEFAULT_MAXTRY
 *
 * Revision 1.92  2016/08/19 16:02:12  dechavez
 * changed QDP_DEFAULT_CTRL_TIMEOUT to 2 seconds, added attempts field to QDP_CMD
 *
 * Revision 1.91  2016/08/15 19:31:32  dechavez
 * eliminated QDP_PAR_OPTION_QUICK_EXITS option
 *
 * Revision 1.90  2016/08/04 21:15:23  dechavez
 * added tld to QDP_FSA, removed QDP_PAR_OPTION_QUITBUSY, added option QDP_PAR_OPTION_QUICK_EXITS,
 * got rid if unused EventThread and CtrlThread semaphores, added reboots counter to QDP
 *
 * Revision 1.89  2016/07/20 16:33:37  dechavez
 * (Checkpoint commit following major redesign of state machine.  Things are working
 * under normal conditions but common failure modes have yet to be fully tested)
 * Replaced maxtry with watchdog interval in QDP_PAR, added QDP_DEFAULT_WATCHDOG_INTERVAL,
 * Removed QDP_DEFAULT_TRASH_x constants, introduced QDP_FSA structure to hold stuff
 * used by the automaton (not sure if I'm going to keep that, however), removed ntry from
 * the QDP handle, renamed regerr to suberr and include a pointer to that to qdpConnect()
 * the idea being that errcode explains the general reason and suberr has additional informatin
 * the definition of which is determined by the value of errcode (eg, C1_CERR in the case of
 * Q330 rejections and current machine state in case of other failures...).  Not sure I'm going
 * to keep that either (probably should have a QDP_FSA_ERROR structure instead).
 *
 * Revision 1.88  2016/06/23 19:51:00  dechavez
 * added qdp_C1_RQCOMBO() prototype
 *
 * Revision 1.87  2016/06/15 21:27:31  dechavez
 * introduced QDP_DEBUG2 and QDP_DEBUG3 states and associated options QDP_PAR_OPTION_QIO_TERSE and QDP_PAR_OPTION_QIO_VERBOSE
 *
 * Revision 1.86  2016/02/12 18:00:08  dechavez
 * add c1_fix to QDP handle
 *
 * Revision 1.85  2016/02/11 18:50:45  dechavez
 * changed QDP nsrq to ntry and QDP_PAR maxresp to maxtry as now we are
 * tracking challege responses as well as server request attempts
 *
 * Revision 1.84  2016/02/05 16:44:21  dechavez
 * added maxsrq to QDP_PAR, defined QDP_DEFAULT_MAXSRQ (0), added nsrq to QDP
 *
 * Revision 1.83  2016/02/03 19:36:51  dechavez
 * removed QDP_META_STATE_COMPLETE
 *
 * Revision 1.82  2016/02/03 17:19:11  dechavez
 * Introduced QDP_SOFTVER structure for managing software versions,
 * added constants to determine level of environmental processor support
 * removed un-used "combo" field from QDP handle and added a new 'flags'
 * field with session options (which is currently just QDP_FLAGS_EP_SUPPORTED)
 *
 * Revision 1.81  2016/01/28 21:49:09  dechavez
 * Changed all QDP_STAT_SDI_x contants to QDP_STAT_EP_SDI_x equivalents.
 * Changed all QDP_STAT_ADC_x contants to QDP_STAT_EP_ADC_x equivalents.
 * Defined (essentially empty) QDP_STAT_MET3 and added a met3 field to QDP_STAT_SS
 *   so that it more correctly reflects the design of the serial sensor status blockette.
 * Changed the design of QDP_STAT_GSV, QDP_STAT_ARP, and QDP_STAT_THR to use fixed
 *   size arrays instead of pointers (no more malloc and free).
 *
 * Revision 1.80  2016/01/28 00:39:58  dechavez
 * added epd to QDP_META 'raw' field
 * changed QDP_META_STATE_COMPLETE to include QDP_META_STATE_HAVE_EPD
 * changed QDP_META_TAG_x definitions to values more easily searched for (debug aid)
 *
 * Revision 1.79  2016/01/23 00:04:33  dechavez
 * added QDP_TYPE_C2_EPD support, reworked QDP_TYPE_C2_EPCFG
 *
 * Revision 1.78  2016/01/21 17:56:51  dechavez
 * updated prototypes
 *
 * Revision 1.77  2016/01/21 17:13:46  dechavez
 * reworked QDP_EP_CHANNEL structure, updated prototypes
 *
 * Revision 1.76  2016/01/19 22:50:56  dechavez
 * initial environmental processor support
 *
 * Revision 1.75  2015/12/04 23:19:12  dechavez
 * added some missing prototypes
 *
 * Revision 1.74  2015/08/24 19:07:01  dechavez
 * added QDP_CAL_x constants, qdpCalState() protype
 *
 * Revision 1.73  2014/08/11 18:02:34  dechavez
 * added qdpPrintTokenReport() prototype which should have been here long ago
 *
 * Revision 1.72  2012/08/07 21:25:42  dechavez
 * updated qdpDestroyHLP() prototype
 *
 * Revision 1.71  2012/08/02 17:56:45  dechavez
 * changed "new" to "det" in qdpInitTokenDet() prototype to avoid name clash
 *
 * Revision 1.70  2012/06/24 17:47:48  dechavez
 * replaced QDP_TYPE_FULL_CONFIG log and token fields with dport, added misc.c prototypes
 *
 * Revision 1.69  2012/01/17 18:43:25  dechavez
 * added qdpDataPortString() prototype
 *
 * Revision 1.68  2011/02/03 17:48:54  dechavez
 * removed unused qdpClose() prototype
 *
 * Revision 1.67  2011/01/31 21:09:48  dechavez
 * defined QDP_BROADCAST_IP and QDP_BROADCAST_PORT
 *
 * Revision 1.66  2011/01/25 17:53:47  dechavez
 * eliminated QDPIO in favor of QIO (from new libqio), introduced QDP_CONNECT
 *
 * Revision 1.65  2011/01/14 00:21:23  dechavez
 * changed name of QDP_PAR "host" field to more generic "input", added macros
 * for checking serial number and inferring console
 *
 * Revision 1.64  2011/01/13 20:23:10  dechavez
 * added QDP_TYPE_C1_MYSN
 *
 * Revision 1.63  2011/01/12 23:10:55  dechavez
 * added qdpEncode_C1_POLLSN prototype
 *
 * Revision 1.62  2011/01/11 17:22:29  dechavez
 * introduced QDPIO
 *
 * Revision 1.61  2011/01/07 22:51:52  dechavez
 * added QDP_DEFAULT_C1_SPP
 *
 * Revision 1.60  2011/01/03 21:23:15  dechavez
 * added QDP_TYPE_C1_GLOB glob to QDP_TYPE_MONITOR
 *
 * Revision 1.59  2010/12/23 21:43:15  dechavez
 * removed QDP_NUM_LOGICAL_PORTS (in favor of already present QDP_NLP) and
 * eliminated QDP_MIN_PORT and QDP_MAX_PORT
 *
 * Revision 1.58  2010/12/22 22:54:07  dechavez
 * cleaned up port numbering, defined QDP_NUM_LOGICAL_PORTS
 *
 * Revision 1.57  2010/12/17 19:03:49  dechavez
 * renumbered the port number defines so they could be used as array indicies
 *
 * Revision 1.56  2010/12/13 22:47:51  dechavez
 * removed scMisMatch from QDP_TYPE_MONITOR
 *
 * Revision 1.55  2010/12/06 17:24:06  dechavez
 * added some prototypes
 *
 * Revision 1.54  2010/11/24 18:28:55  dechavez
 * added scMisMatch to QDP_TYPE_MONITOR
 *
 * Revision 1.53  2010/11/15 23:07:49  dechavez
 * defined QDP_LOG_FLAG_x constants for logical data port flags
 *
 * Revision 1.52  2010/10/20 18:29:49  dechavez
 * updated prototypes
 *
 * Revision 1.51  2010/10/19 21:14:08  dechavez
 * added QDP_TYPE_MONITOR, qdpRequestMonitor(), qdpSensorControlString2() prototypes
 *
 * Revision 1.50  2010/04/07 19:46:51  dechavez
 * updated prototypes
 *
 * Revision 1.49  2010/04/02 18:13:47  dechavez
 * restored qdpSetMyCtrlPort(), qdpSetMyDataPort() prototypes
 *
 * Revision 1.48  2010/03/31 20:41:40  dechavez
 * updated prototypes, removed trash link parameters
 *
 * Revision 1.47  2010/03/22 21:37:09  dechavez
 * added options flag to QDP_PAR (default QDP_PAR_OPTION_QUITBUSY), defined
 * various qdpLenBT_x macros, updated prototypes
 *
 * Revision 1.46  2009/11/13 00:02:15  dechavez
 * updated prototypes
 *
 * Revision 1.45  2009/11/05 18:26:53  dechavez
 * added new prototypes
 *
 * Revision 1.44  2009/10/29 17:04:56  dechavez
 * added dbgpkt pointer to QDP handle, updated prototypes
 *
 * Revision 1.43  2009/10/20 21:52:06  dechavez
 * added serial sensor support, crc's to various stuctures
 *
 * Revision 1.42  2009/10/10 00:33:16  dechavez
 * moved xml stuff off to libqdpxml, added basetag, writer, and set bitmask
 * to QDP_TYPE_FULL_CONFIG
 *
 * Revision 1.41  2009/10/02 18:27:36  dechavez
 * added xml.c prototypes, plus few others
 *
 * Revision 1.40  2009/09/28 17:26:26  dechavez
 * completed C2_PHY definition, updated QDP_TYPE_FULL_CONFIG, prototypes
 *
 * Revision 1.39  2009/09/22 19:48:06  dechavez
 * checkpoint while working on QDP_TYPE_FULL_CONFIG and relatives
 *
 * Revision 1.38  2009/09/15 23:08:52  dechavez
 * added qdp_C1_RQMEM
 *
 * Revision 1.37  2009/07/09 18:14:36  dechavez
 * added more new prototypes
 *
 * Revision 1.36  2009/07/02 21:43:49  dechavez
 * changed QDP_DEFAULT_DATA_HBEAT_INTERVAL to 30 seconds
 *
 * Revision 1.35  2009/07/02 21:26:23  dechavez
 * added prototypes
 *
 * Revision 1.34  2009/07/02 16:56:05  dechavez
 * updated prototypes
 *
 * Revision 1.33  2009/03/17 17:17:48  dechavez
 * added extern char *libqdpBuildIdent;
 *
 * Revision 1.32  2009/02/23 21:48:18  dechavez
 * added new prototypes
 *
 * Revision 1.31  2009/02/04 05:42:59  dechavez
 * added QDP_TYPE_C1_DCP, QDP_TYPE_C1_SPP, QDP_TYPE_C1_MAN, QDP_TYPE_C2_GPS,
 * QDP_TYPE_CHECKOUT and associated function prototypes, added errcode field
 * to QDP handle
 *
 * Revision 1.30  2009/01/23 23:54:14  dechavez
 * added QDP_TYPE_C2_AMASS typedef, updated prototypes
 *
 * Revision 1.29  2009/01/06 20:44:25  dechavez
 * updated prototypes
 *
 * Revision 1.28  2008/10/09 20:38:09  dechavez
 * Added constants for the C1_STAT bitmap, added QDP_TYPE_C1_PHY typedef,
 * updated prototypes
 *
 * Revision 1.27  2008/03/11 20:49:46  dechavez
 * removed QDP_QCAL_MAX_DIVISOR (found now in q330.h)
 *
 * Revision 1.26  2007/12/20 22:46:04  dechavez
 * moved various limits to new qdp/limits.h, added support for C1_STAT, defined physical ports
 *
 * Revision 1.25  2007/10/31 17:20:06  dechavez
 * added C1_QCAL support
 *
 * Revision 1.24  2007/05/17 22:27:41  dechavez
 * initial production release
 *
 */
