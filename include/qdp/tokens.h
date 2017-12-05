#pragma ident "$Id: tokens.h,v 1.10 2012/06/24 17:50:35 dechavez Exp $"
/*======================================================================
 *
 * Q330 DP Tokens
 *
 *====================================================================*/
#ifndef qdp_tokens_h_included
#define qdp_tokens_h_included

#include "platform.h"

#ifdef __cplusplus
extern "C" {
#endif

#define QDP_NNAME_LEN 2 /* network name */
#define QDP_SNAME_LEN 5 /* station name */
#define QDP_CNAME_LEN 3 /* channel name */
#define QDP_LNAME_LEN 2 /* location code */
#define QDP_PASCALSTRING_LEN 0xff
#define QDP_LCQ_MAX_NAMELEN (QDP_LNAME_LEN+1+QDP_CNAME_LEN+1) // eg. "00-BHZ"
#define QDP_LCQ_MAX_DETECTOR_STRING_LEN (QDP_LCQ_MAX_NAMELEN + 1+ QDP_PASCALSTRING_LEN + 1)

#define QDP_UNDEFINED_ID 0xff

typedef struct {
    UINT8 code;
    char name[QDP_PASCALSTRING_LEN + 1];
} QDP_TOKEN_IDENT;

/* type 0 - padding */

#define QDP_TOKEN_TYPE_IGNORE 0

/* type 1 - Token Version Number */

#define QDP_TOKEN_TYPE_VER 1
#define QDP_TOKEN_VERSION_ZERO  0 /* that's all there is */

typedef struct {
    UINT8 version;
    BOOL valid;
} QDP_TOKEN_VERSION;

/* type 2 - Network and Station ID */

#define QDP_TOKEN_TYPE_SITE 2

typedef struct {
    char nname[QDP_NNAME_LEN+1];
    char sname[QDP_SNAME_LEN+1];
    BOOL valid;
} QDP_TOKEN_SITE;

/* type 3, 5, 9 - server ports */

#define QDP_TOKEN_TYPE_NETSERVER 3
#define QDP_TOKEN_TYPE_WEBSERVER 5
#define QDP_TOKEN_TYPE_DATSERVER 9

typedef struct {
    INT16 port;
    BOOL valid;
} QDP_TOKEN_SERVER;

typedef struct {
    QDP_TOKEN_SERVER net;
    QDP_TOKEN_SERVER web;
    QDP_TOKEN_SERVER data;
} QDP_TOKEN_SERVICES;

/* type 4 - Data Subscription Server Parameters */

#define QDP_TOKEN_TYPE_DSS 4

#define QDP_DSS_PASSWD_LEN 7

typedef struct {
    struct {
        char hi[QDP_DSS_PASSWD_LEN+1];
        char mi[QDP_DSS_PASSWD_LEN+1];
        char lo[QDP_DSS_PASSWD_LEN+1];
    } passwd;
    UINT32 timeout;
    UINT32 maxbps;
    UINT8 verbosity;
    UINT8 maxcpu;
    UINT16 port;
    UINT16 maxmem;
    BOOL valid;
} QDP_TOKEN_DSS;

/* type 6 - Clock Processing Parameters */

#define QDP_TOKEN_TYPE_CLOCK 6

typedef struct {
    INT32 offset;
    UINT16 maxlim;
    struct {
        UINT8 locked;
        UINT8 track;
        UINT8 hold;
        UINT8 off;
        UINT8 spare;
    } pll;
    UINT8 maxhbl;
    UINT8 minhbl;
    UINT8 nbl;
    UINT16 clkqflt;
    BOOL valid;
} QDP_TOKEN_CLOCK;

#define QDP_DEFAULT_TOKEN_CLOCK_OFFSET       0
#define QDP_DEFAULT_TOKEN_CLOCK_MAXLIM     100
#define QDP_DEFAULT_TOKEN_CLOCK_PLL_LOCKED 100
#define QDP_DEFAULT_TOKEN_CLOCK_PLL_TRACK   90
#define QDP_DEFAULT_TOKEN_CLOCK_PLL_HOLD    80
#define QDP_DEFAULT_TOKEN_CLOCK_PLL_OFF     80
#define QDP_DEFAULT_TOKEN_CLOCK_MAXHBL      60
#define QDP_DEFAULT_TOKEN_CLOCK_MINHBL      10
#define QDP_DEFAULT_TOKEN_CLOCK_NBL          0
#define QDP_DEFAULT_TOKEN_CLOCK_CLKQFLT     20

#define QDP_DEFAULT_TOKEN_CLOCK_PLL {   \
    QDP_DEFAULT_TOKEN_CLOCK_PLL_LOCKED, \
    QDP_DEFAULT_TOKEN_CLOCK_PLL_TRACK,  \
    QDP_DEFAULT_TOKEN_CLOCK_PLL_HOLD,   \
    QDP_DEFAULT_TOKEN_CLOCK_PLL_OFF     \
}

#define QDP_DEFAULT_TOKEN_CLOCK {    \
    QDP_DEFAULT_TOKEN_CLOCK_OFFSET,  \
    QDP_DEFAULT_TOKEN_CLOCK_MAXLIM,  \
    QDP_DEFAULT_TOKEN_CLOCK_PLL,     \
    QDP_DEFAULT_TOKEN_CLOCK_MAXHBL,  \
    QDP_DEFAULT_TOKEN_CLOCK_MINHBL,  \
    QDP_DEFAULT_TOKEN_CLOCK_NBL,     \
    QDP_DEFAULT_TOKEN_CLOCK_CLKQFLT, \
    TRUE                             \
}

/* type 7 - Log and Timing Identification */

#define QDP_TOKEN_TYPE_LOGID 7

typedef struct {
    struct {
        char loc[QDP_LNAME_LEN+1];
        char chn[QDP_CNAME_LEN+1];
    } mesg, time;
    BOOL valid;
} QDP_TOKEN_LOGID;

/* type 8 - Configuration Identification */

#define QDP_TOKEN_TYPE_CNFID 8

typedef struct {
    char loc[QDP_LNAME_LEN+1];
    char chn[QDP_CNAME_LEN+1];
    UINT8 flags;
    UINT16 interval;
    BOOL valid;
} QDP_TOKEN_CNFID;

/* type 128 - Logical Channel Queue */

#define QDP_TOKEN_TYPE_LCQ 128

typedef struct {
    UINT32 len; /* number of samples between reports */
    QDP_TOKEN_IDENT filt; /* optional IIR filter */
} QDP_LCQ_AVEPAR;

typedef struct {
    QDP_TOKEN_IDENT src; /* src LCQ */
    QDP_TOKEN_IDENT fir; /* decimation FIR filter */
} QDP_LCQ_DECIM;

typedef struct {
    QDP_TOKEN_IDENT base; /* event detector to use as base */
    UINT8 use;     /* detector number for this invocation */
    UINT8 options; /* option bitmap (see QDP_LCQ_DETECT_x descriptions below) */
    BOOL  set;     /* TRUE when contents have been set */
} QDP_LCQ_DETECT;

#define QDP_LCQ_DETECT_RUN            0x01 /* bit 0 */
#define QDP_LCQ_DETECT_LOG_ENABLED    0x02 /* bit 1 */
#define QDP_LCQ_DETECT_BIT_2_RESERVED 0x04 /* bit 2 */
#define QDP_LCQ_DETECT_LOG_MESSAGE    0x08 /* bit 2 */
#define QDP_LCQ_DETECT_BIT_4_RESERVED 0x10 /* bit 4 */
#define QDP_LCQ_DETECT_BIT_5_RESERVED 0x20 /* bit 5 */
#define QDP_LCQ_DETECT_BIT_6_RESERVED 0x40 /* bit 6 */
#define QDP_LCQ_DETECT_BIT_7_RESERVED 0x80 /* bit 7 */

typedef struct {
    int len;                   /* length of this token */
    char loc[QDP_LNAME_LEN+1]; /* location code */
    char chn[QDP_CNAME_LEN+1]; /* channel code (seed name) */
    QDP_TOKEN_IDENT ident;     /* LCQ identifier */
#define QDP_LCQSRC_LEN 2
    UINT8 src[QDP_LCQSRC_LEN]; /* DT_DATA channel, filter code for 24-bit data, parameter no. for other */
    UINT32 options;            /* option bitmap (see bit descriptions below) */
    INT16 rate;                /* sample rate (pos => Hz, neg => 1/-rate (Hz)) */
    REAL64 frate;              /* derived sample rate in seconds */
    REAL64 dsint;              /* derived sample interval in seconds */
    UINT64 nsint;              /* derived sample interval in nano-seconds */
    /* the following are defined only if their corresponding option bit is set */
    UINT16 pebuf;              /* number of pre-event buffers */
    REAL32 gapthresh;          /* gap threshold */
    UINT16 caldly;             /* calibration delay in seconds */
#define QDP_LCQ_DEFAULT_COMFR 255
    UINT8 comfr;               /* maximum frame count */
    REAL32 firfix;             /* FIR multiplier */
    QDP_TOKEN_IDENT cntrl;     /* control detector */
    QDP_LCQ_AVEPAR ave;        /* averaging parameters */
    QDP_LCQ_DECIM decim;       /* decimation entry */
#define QDP_LCQ_NUM_DETECT 8
    QDP_LCQ_DETECT detect[QDP_LCQ_NUM_DETECT]; /* detector to run */
} QDP_TOKEN_LCQ;

/* LCQ option bits */

#define QDP_LCQ_TRIGGERED_MODE         0x00000001 /* bit  0 */
#define QDP_LCQ_WRITE_DETECT_PKTS      0x00000002 /* bit  1 */
#define QDP_LCQ_WRITE_CALIB_PKTS       0x00000004 /* bit  2 */
#define QDP_LCQ_HAVE_PRE_EVENT_BUFFERS 0x00000008 /* bit  3 */
#define QDP_LCQ_HAVE_GAP_THRESHOLD     0x00000010 /* bit  4 */
#define QDP_LCQ_HAVE_CALIB_DELAY       0x00000020 /* bit  5 */
#define QDP_LCQ_HAVE_FRAME_COUNT       0x00000040 /* bit  6 */
#define QDP_LCQ_HAVE_FIR_MULTIPLIER    0x00000080 /* bit  7 */
#define QDP_LCQ_HAVE_AVEPAR            0x00000100 /* bit  8 */
#define QDP_LCQ_HAVE_CNTRL_DETECTOR    0x00000200 /* bit  9 */
#define QDP_LCQ_HAVE_DECIM_ENTRY       0x00000400 /* bit 10 */
#define QDP_LCQ_DO_NOT_OUTPUT          0x00000800 /* bit 11 */
#define QDP_LCQ_HAVE_DETECTOR_1        0x00001000 /* bit 12 */
#define QDP_LCQ_HAVE_DETECTOR_2        0x00002000 /* bit 13 */
#define QDP_LCQ_HAVE_DETECTOR_3        0x00004000 /* bit 14 */
#define QDP_LCQ_HAVE_DETECTOR_4        0x00008000 /* bit 15 */
#define QDP_LCQ_HAVE_DETECTOR_5        0x00010000 /* bit 16 */
#define QDP_LCQ_HAVE_DETECTOR_6        0x00020000 /* bit 17 */
#define QDP_LCQ_HAVE_DETECTOR_7        0x00040000 /* bit 18 */
#define QDP_LCQ_HAVE_DETECTOR_8        0x00080000 /* bit 19 */
#define QDP_LCQ_BIT_20_UNUSED          0x00100000 /* bit 20 */
#define QDP_LCQ_BIT_21_UNUSED          0x00200000 /* bit 21 */
#define QDP_LCQ_BIT_22_UNUSED          0x00400000 /* bit 22 */
#define QDP_LCQ_BIT_23_UNUSED          0x00800000 /* bit 23 */
#define QDP_LCQ_BIT_24_UNUSED          0x01000000 /* bit 24 */
#define QDP_LCQ_BIT_25_UNUSED          0x02000000 /* bit 25 */
#define QDP_LCQ_BIT_26_UNUSED          0x04000000 /* bit 26 */
#define QDP_LCQ_NETSERV_EVENT_ONLY     0x08000000 /* bit 27 */
#define QDP_LCQ_DISABLE                0x10000000 /* bit 28 */
#define QDP_LCQ_SEND_TO_DATA_SERVER    0x20000000 /* bit 29 */
#define QDP_LCQ_SEND_TO_NET_SERVER     0x40000000 /* bit 30 */
#define QDP_LCQ_FORCE_CNP_BLOCKETTTES  0x80000000 /* bit 31 */

/* type 129 - IIR filter specification */

#define QDP_TOKEN_TYPE_IIR 129

typedef struct {
    REAL32 ratio;
    UINT8 npole;
#define QDP_IIR_HIPASS 1
#define QDP_IIR_LOPASS 0
    int type;
} QDP_IIR_DATA;

typedef struct {
    int len;                   /* length of this token */
    UINT8 id;
    char name[QDP_PASCALSTRING_LEN+1]; /* this filter's name */
    UINT8 nsection;
    REAL32 gain;
    REAL32 refreq;
    QDP_IIR_DATA data[0xff];
} QDP_TOKEN_IIR;

/* types 130 - FIR Filter specification */

#define QDP_TOKEN_TYPE_FIR 130

typedef struct {
    int len;                   /* length of this token */
    UINT8 id;
    char name[QDP_PASCALSTRING_LEN+1]; /* this filter's name */
} QDP_TOKEN_FIR;

/* type 131 - Control Detector Specification */

#define QDP_TOKEN_TYPE_CDS 131

#define QDP_CDS_NIB_COMM 0
#define QDP_CDS_NIB_DET  1
#define QDP_CDS_NIB_CAL  2
#define QDP_CDS_NIB_OP   3

typedef struct {
    int len;                           /* length of this token */
    UINT8 id;                          /* control detector number */
    UINT8 options;                     /* control detector options */
    char name[QDP_PASCALSTRING_LEN+1]; /* control detector name */
#define QDP_MAX_CDS_ENTRY 0xff
    QDP_TOKEN_IDENT equation[QDP_MAX_CDS_ENTRY]; /* detector equation */
    int nentry;                        /* number valid elements in the equation */
} QDP_TOKEN_CDS;

/* types 132, 133 - event detectors */

#define QDP_TOKEN_TYPE_MHD 132
#define QDP_TOKEN_TYPE_TDS 133

typedef struct {
    UINT8 type;                        /* either 132 or 133 */
    int len;                           /* length of this token */
    /* These are common to both types of detectors */
    UINT8 id;                          /* detector number */
    char name[QDP_PASCALSTRING_LEN+1]; /* this detector's name */
    QDP_TOKEN_IDENT detf;              /* IIR filter */
    UINT8 iw;                          /* Iw parameter */
    UINT8 nht;                         /* Nht parameter */
    UINT32 fhi;                        /* Filhi parameter */
    UINT32 flo;                        /* Fillo parameter */
    UINT16 wa;                         /* Wa parameter */
    /* The following are provided only for Murdock Hutt type 132 (QDP_TOKEN_TYPE_MHD) */
    UINT16 tc;                         /* Tc parameter */
    UINT8 x1;                          /* X1 parameter */
    UINT8 x2;                          /* X2 parameter */
    UINT8 x3;                          /* X3 parameter */
    UINT8 xx;                          /* Xx parameter */
    UINT8 av;                          /* Av parameter */
} QDP_TOKEN_DET;

/* type 134 - non-compliant DP (no parameters) */

#define QDP_TOKEN_TYPE_NONCOMP 134

/* type 192 - comm event names */

#define QDP_TOKEN_TYPE_CEN 192

typedef struct {
    UINT8 id;                          /* comm event bit number */
    char name[QDP_PASCALSTRING_LEN+1]; /* comm event name */
} QDP_TOKEN_CEN;

/* type 193 - email alert configuration */

#define QDP_TOKEN_TYPE_EMAIL 193

/* type 194 - opaque configuration */

#define QDP_TOKEN_TYPE_OPAQUE 194

/* Everything */

#define QDP_COMM_EVENT_MAXLEN 128

typedef struct {
    QDP_TOKEN_VERSION ver;
    QDP_TOKEN_SITE site;
    QDP_TOKEN_DSS dss;
    QDP_TOKEN_CLOCK clock;
    QDP_TOKEN_LOGID logid;
    QDP_TOKEN_CNFID cnfid;
    QDP_TOKEN_SERVICES srvr;
    LNKLST lcq; /* list of QDP_TOKEN_LCQ */
    LNKLST iir; /* list of QDP_TOKEN_IIR */
    LNKLST fir; /* list of QDP_TOKEN_FIR */
    LNKLST cds; /* list of QDP_TOKEN_CDS */
    LNKLST det; /* list of QDP_TOKEN_DET */
    LNKLST cen; /* list of QDP_TOKEN_CEN */
    int count;
    int noncomp;
    char setname[QDP_MAX_TOKENSET_NAME+1];
#define QDP_DP_TOKEN_SIG_LEN 15
    char signature[QDP_DP_TOKEN_SIG_LEN+1]; /* typing monkeys validity flag */
    UINT32 crc;
} QDP_DP_TOKEN;

#ifdef __cplusplus
}
#endif

#endif /* qdp_tokens_h_included */

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
 * $Log: tokens.h,v $
 * Revision 1.10  2012/06/24 17:50:35  dechavez
 * renamed QDP_TOKEN_LCQ sint to dsint, added nsint
 *
 * Revision 1.9  2010/10/20 18:29:33  dechavez
 * defined QDP_DEFAULT_TOKEN_CLOCK
 *
 * Revision 1.8  2009/11/05 18:23:59  dechavez
 * added names for token types 193 and 194 (but no code)
 *
 * Revision 1.7  2009/10/29 16:59:46  dechavez
 * defined QDP_TOKEN_VERSION_ZERO, fixed missing closed comment typo
 *
 * Revision 1.6  2009/10/20 22:22:05  dechavez
 * various structure changes to facilitate bi-directional XML I/O (mostly through the use of QDP_TOKEN_IDENT)
 *
 * Revision 1.5  2009/10/02 18:18:57  dechavez
 * added QDP_LCQ_DEFAULT_COMFR, renamed QDP_LCQ_BIT_27_UNUSED to QDP_LCQ_NETSERV_EVENT_ONLY,
 * swapped the definitions of the QDP_IIR_{HO}{LO}PASS macros so they agree with XML codes,
 * added QDP_TOKEN_TYPE_NONCOMP, and noncomp and setname to QDP_DP_TOKEN
 *
 * Revision 1.4  2009/09/28 17:23:04  dechavez
 * gave QDP_TOKEN_TYPE_x names to the various token identifiers
 * combined mdh and tds fields into a generic detector det field, added cen
 *
 * Revision 1.3  2007/05/17 22:27:44  dechavez
 * initial production release
 *
 */
