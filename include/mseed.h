#pragma ident "$Id: mseed.h,v 1.19 2018/01/18 23:36:20 dechavez Exp $"
/*======================================================================
 *
 * Defines, templates, and prototypes for MiniSEED library
 *
 *====================================================================*/
#ifndef mseed_h_included
#define mseed_h_included

#include "platform.h"
#include "logio.h"
#include "list.h"
#include "isi.h"
#include "filter.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Macros */

#define MSEED_STEIM_FRAME_LEN 64
#define mseedMaxSteim2SampleCount(reclen) ((((((reclen)/MSEED_STEIM_FRAME_LEN)*15)-1)*7)-7)
#define mseedConvertToHptime(tstamp) ((hptime_t) (tstamp + (SAN_EPOCH_TO_1970_EPOCH * NANOSEC_PER_SEC)) / 1000)

/* Misc. constants */

#define MSEED_NNAMLEN 2  /* network code */
#define MSEED_SNAMLEN 5  /* station name */
#define MSEED_LNAMLEN 2  /* location code */
#define MSEED_CNAMLEN 3  /* channel name */
#define MSEED_QNAMLEN 1  /* QC flag */
#define MSEED_INAMLEN   (MSEED_NNAMLEN+1+MSEED_SNAMLEN+1+MSEED_LNAMLEN+1+MSEED_CNAMLEN+1+MSEED_QNAMLEN+6+1) /* NN_SSSSS_LL_CCC_Q/MSEED */
#define MSEED_TIMELEN 22 /* TIME string length */
#define MSEED_SIGLEN    (MSEED_INAMLEN+1+2+1+16+1) /* NN_SSSSS_LL_CCC_Q/MSEED_FF_SSSSSSSSSSSSSSSS */
#define MSEED_CHNLOCLEN (MSEED_CNAMLEN + MSEED_LNAMLEN)

#define MSEED_MIN_RECLEN_EXP         8 /* 256 */
#define MSEED_MAX_RECLEN_EXP        16 /* 65536 */
#define MSEED_DEFAULT_RECLEN_EXP     9 /* 512 */
#define MSEED_MIN_RECLEN           256 /* 2**MSEED_MIN_RECLEN */
#define MSEED_MAX_RECLEN         65536 /* 2**MSEED_MAX_RECLEN_EXP */
#define MSEED_DEFAULT_RECLEN       512 /* 2**MSEED_DEFAULT_RECLEN_EXP */
#define MSEED_MAX_BUFLEN           mseedMaxSteim2SampleCount(MSEED_MAX_RECLEN)
#define MSEED_NEVER                0xFFFFFFFFFFFFFFFFLL

#define MSEED_FSDH_LEN 48
#define MSEED_B100_LEN 12
#define MSEED_B1000_LEN 8
#define MSEED_B1001_LEN 8
#define MSEED_MAX_FSDH_SEQNO 999999
#define MSEED_FSDH_SEQNO_MODULO (MSEED_MAX_FSDH_SEQNO + 1)

#define MSEED_UNDEFINED_DRIFT 0.0

#define MSEED_DEFAULT_NETID "II"
#define MSEED_BLANK_LOCID "  "
#define MSEED_DEFAULT_LOCID MSEED_BLANK_LOCID
#define MSEED_DEFAULT_B50_UPDATE_FLAG 'N'

/* Only selected encoding formats are supported */

#define MSEED_FORMAT_UNCOMPRESSED -2 /* means pack data in network byte order */
#define MSEED_FORMAT_COMPRESSED   -1 /* means Steim2 compress data, if possible (ie, integer input) */
#define MSEED_FORMAT_UNDEF         0 /* means let the library decide on the output format */
/* THESE NUMBERS ARE DEFINED BY SEED - DO NOT CHANGE! */
#define MSEED_FORMAT_INT_16        1 /* 16 bit integers */
#define MSEED_FORMAT_INT_32        3 /* 32 bit integers */
#define MSEED_FORMAT_IEEE_F        4 /* IEEE floading point */
#define MSEED_FORMAT_IEEE_D        5 /* IEEE double precision floating point */
#define MSEED_FORMAT_STEIM1       10 /* Steim 1 compression */
#define MSEED_FORMAT_STEIM2       11 /* Steim 2 compression */

#define MSEED_BIG_ENDIAN     1 /* Network byte order */
#define MSEED_LITTLE_ENDIAN  0 /* not */

/* FSDH quality control flags */

#define MSEED_QC_UNDEFINED 'D'
#define MSEED_QC_RAW       'R'
#define MSEED_QC_VERIFIED  'Q'
#define MSEED_QC_MODIFIED  'M'
#define MSEED_DEFAULT_QC MSEED_QC_UNDEFINED

/* FSDH activity flag bits */

#define MSEED_ACT_CAL_PRESENT       0x01
#define MSEED_ACT_TIMECOR_APPLIED   0x02
#define MSEED_ACT_EVENT_BEG         0x04
#define MSEED_ACT_EVENT_END         0x08
#define MSEED_ACT_POS_LEAP_SEC      0x10
#define MSEED_ACT_NEG_LEAP_SEC      0x20
#define MSEED_ACT_EVENT_IN_PROGRESS 0x40
#define MSEED_DEFAULT_ACT_FLAG      0x00

/* FSDH I/O and clock flag bits */

#define MSEED_IOC_PARITY_ERROR      0x01
#define MSEED_IOC_LONG_READ         0x02
#define MSEED_IOC_SHORT_READ        0x04
#define MSEED_IOC_START_OF_SERIES   0x08
#define MSEED_IOC_END_OF_SERIES     0x10
#define MSEED_IOC_CLOCK_LOCKED      0x20
#define MSEED_DEFAULT_IOC_FLAG      0x00

/* FSDH data quality flag bits */

#define MSEED_DAT_SATURATED         0x01
#define MSEED_DAT_CLIPPED           0x02
#define MSEED_DAT_SPIKES            0x04
#define MSEED_DAT_GLITCHES          0x08
#define MSEED_DAT_MISSING_DATA      0x10
#define MSEED_DAT_SYNC_ERR          0x20
#define MSEED_DAT_FILTER_CHARGE     0x40
#define MSEED_DAT_TIME_SUSPECT      0x80
#define MSEED_DEFAULT_DAT_FLAG      0x00

/* Blockette 50 word order values */

#if (NATIVE_BYTE_ORDER == BIG_ENDIAN_BYTE_ORDER)
#define MSEED_32_BIT_WORD_ORDER "3210"
#define MSEED_16_BIT_WORD_ORDER "10"
#else
#define MSEED_32_BIT_WORD_ORDER "0123"
#define MSEED_16_BIT_WORD_ORDER "01"
#endif

/* Stage types for response blockettes 53 and 54 */

#define MSEED_STAGE_TYPE_NULL       0
#define MSEED_STAGE_TYPE_RADSEC    'A' /* analog stage in rad/sec */
#define MSEED_STAGE_TYPE_HERTZ     'B' /* analog stage in Hz */
#define MSEED_STAGE_TYPE_COMPOSITE 'C' /* composite */
#define MSEED_STAGE_TYPE_DIGITAL   'D' /* digital */

/* Structure templates */

typedef struct {
    char qc;   /* MSEED_QC_x quality control flag */
    UINT8 act; /* MSEED_ACT_x bits */
    UINT8 ioc; /* MSEED_IOC_x bits */
    UINT8 dat; /* MSEED_DAT_x bits */
} MSEED_FLAGS;

typedef struct {
    INT32       seqno;                  /* sequence number */
    char        ident[MSEED_INAMLEN+1]; /* trace identifier (net_sta_loc_chn_Q/MSEED) */
    char        staid[MSEED_SNAMLEN+1]; /* station name */
    char        chnid[MSEED_CNAMLEN+1]; /* channel name */
    char        locid[MSEED_LNAMLEN+1]; /* location code */
    char        netid[MSEED_NNAMLEN+1]; /* network code */
    INT32       nsamp;                  /* total number of samples, all records */
    UINT64      nsint;                  /* nominal nanosec sample interval */
    UINT64      asint;                  /* if non-zero, actual nanosec sample interval */
    UINT64      sint;                   /* asint != 0 ? asint : nsint */
    INT64       tstamp;                 /* time of first sample (nanoseconds since 1/1/1999) */
    INT64       endtime;                /* time of last sample */
    INT32       tcorr;                  /* time correction */
    MSEED_FLAGS flags;                  /* activity, I/O, clock and data quality flags */
    INT8        tqual;                  /* in positive, blockette 1001 clock quality as a percentage */
   /* these are set when READING miniSEED but are not used for CREATING */
    INT16       bod;                    /* offset to begining of data */
    INT8        format;                 /* blockette 1000 data encoding format (MSEED_FORMAT_x) */
    int         order;                  /* blockette 1000 data word order */
    INT32       reclen;                 /* blockette 1000 record length */
    char        sig[MSEED_SIGLEN+1];    /* signature for inter-record comparison help */
} MSEED_HDR;

typedef union {
    UINT8  uint8[MSEED_MAX_BUFLEN];
    INT16  int16[MSEED_MAX_BUFLEN/sizeof(INT16)];
    INT32  int32[MSEED_MAX_BUFLEN/sizeof(INT32)];
    REAL32 real32[MSEED_MAX_BUFLEN/sizeof(REAL32)];
    REAL64 real64[MSEED_MAX_BUFLEN/sizeof(REAL64)];
} MSEED_DAT;

typedef struct {
    INT32 seqno;   /* if positive, current sequence number (if using per channel values) */
    MSEED_HDR hdr; /* everything needed to build FSDH with blockette 1000, and optionally 100 and 1001 */
    MSEED_DAT dat; /* UNCOMPRESSED data */
} MSEED_RECORD;

/* for exporting packed MiniSeed */

typedef struct {
    UINT8 data[MSEED_MAX_BUFLEN]; /* big enough to hold the largest supported packet */
    int   len;                    /* how much of packed is actually used */
    MSEED_HDR hdr;                /* a description of what's inside packed */
} MSEED_PACKED;

/* For renaming channels */

typedef struct {
    LNKLST *ida10;  /* linked list of IDA10_NAME_MAPs */
} MSEED_NAME_MAP;

/* User supplied function for disposing of newly created MiniSEED packets */

typedef void (*MSEED_SAVE_FUNC)(void *argptr, MSEED_PACKED *packed);

/* various thresholds for use when setting status bits */

typedef struct {
#define MSEED_DEFAULT_CLOCK_LOCKED_PERCENT  80
#define MSEED_DEFAULT_CLOCK_SUSPECT_PERCENT 10
    struct {
        INT8 locked;
        INT8 suspect;
    } clock;
} MSEED_THRESH;

/* For tracking compression history and per-channel sequence numbers */

typedef struct {
    char ident[MSEED_INAMLEN+1]; /* trace identifier */
    INT32 value;                 /* last sample of previous frame */
} MSEED_DATUM;

typedef struct {
    LNKLST *data;      /* linked list of MSEED_DATUMs storing last sample values for Steim compression */
    LNKLST *seqno;     /* linked list of MSEED_DATUMs storing sequence numbers for per-channel numbering */
} MSEED_HISTORY;

/* Handle for miniSEED creator */

typedef struct {
    LOGIO *lp;             /* for library logging */
    int reclen;            /* size of MiniSeed records to generate */
    INT32 seqno;           /* current sequence number (if using global value) */
    char qc;               /* data quality flag to use in FSDH */
    char *staid;           /* default station name */
    char *netid;           /* default network code */
    INT8 format;           /* MSEED_FORMAT_x output encoding format */
    MSEED_SAVE_FUNC func;  /* user supplied function for processing MiniSEED packets */
    void *argptr;          /* points to user supplied argument to func */
    MSEED_THRESH thresh;   /* various thresholds */
    MSEED_NAME_MAP rename; /* input specific rename maps */
    REAL64 drift;          /* optional clock drift rate */
    LNKLST *records;       /* linked list of MSEED_RECORDs, accumulating data for packing */
    MSEED_HISTORY history; /* for tracking stuff on a per-channel basis */
    int debug;             /* debug verbosity */
#define MSEED_HANDLE_OPTION_NONE            0x00000000
#define MSEED_HANDLE_OPTION_FSDH_FLAG_CHECK 0x00000001
#define MSEED_HANDLE_OPTION_FSDH_QUAL_CHECK 0x00000002
#define MSEED_DEFAULT_HANDLE_OPTIONS       (MSEED_HANDLE_OPTION_FSDH_FLAG_CHECK | MSEED_HANDLE_OPTION_FSDH_QUAL_CHECK)
    INT32 options;         /* options */
} MSEED_HANDLE;

/* blockette 30 */

#define MSEED_B30_INT16_CODE       MSEED_FORMAT_INT_16
#define MSEED_B30_INT16_DESC       "16-bit Integer Format"
#define MSEED_B30_INT16_DDL        \
    "M0~"                          \
    "W4 D0-15 C2~"

#define MSEED_B30_INT32_CODE       MSEED_FORMAT_INT_32
#define MSEED_B30_INT32_DESC       "32-bit Integer Format"
#define MSEED_B30_INT32_DDL        \
    "M0~"                          \
    "W4 D0-31 C2~"

#ifdef MSEED_FLOAT_FAMILY_SUPPORTED
#define MSEED_B30_REAL32_CODE      MSEED_FORMAT_IEEE_F
#define MSEED_B30_REAL32_DESC      "32-bit IEEE Floating Point Format"
#define MSEED_B30_REAL32_DDL       \
    "M0~"                          \
    "W4 D0-31 ??~"

#define MSEED_B30_REAL64_CODE      MSEED_FORMAT_IEEE_D
#define MSEED_B30_REAL64_DESC      "64-bit IEEE Floating Point Format"
#define MSEED_B30_REAL64_DDL       \
    "M0~"                          \
    "W8 D0-63 ??~"
#endif /* MSEED_FLOAT_FAMILY_SUPPORTED */

#define MSEED_B30_STEIM1_CODE      MSEED_FORMAT_STEIM1
#define MSEED_B30_STEIM1_DESC      "Steim1 Integer Compression Format"
#define MSEED_B30_STEIM1_DDL       \
    "F1 P4 W4 D0-31 C2 R1 P8 W4 D0-31 C2~"                        \
    "P0 W4 N15 S2,0,1~"                                           \
    "T0 X N0 W4 D0-31 C2~"                                        \
    "T1 N0 W1 D0-7 C2 N1 W1 D0-7 C2 N2 W1 D0-7 C2 N3 W1 D0-7 C2~" \
    "T2 N0 W2 D0-15 C2 N1 W2 D0-15 C2~"                           \
    "T3 N0 W4 D0-31 C2~"

#define MSEED_B30_STEIM2_CODE      MSEED_FORMAT_STEIM2
#define MSEED_B30_STEIM2_DESC      "Steim2 Integer Compression Format"
#define MSEED_B30_STEIM2_DDL       \
    "F1 P4 W4 D C2 R1 P8 W4 D C2~" \
    "P0 W4 N15 S2,0,1~"            \
    "T0 X W4~"                     \
    "T1 Y4 W1 D C2~"               \
    "T2 W4 I D2~"                  \
    "K0 X D30~"                    \
    "K1 N0 D30 C2~"                \
    "K2 Y2 D15 C2~"                \
    "K3 Y3 D10 C2~"                \
    "T3 W4 I D2~"                  \
    "K0 Y5 D6 C2~"                 \
    "K1 Y6 D5 C2~"                 \
    "K2 X D2 Y7 D4 C2 K3 X D30~"

#define MSEED_B30_FAMILY_INTEGER         0
#define MSEED_B30_FAMILY_GAINRANGED      1
#define MSEED_B30_FAMILY_COMPRESSED     50
#define MSEED_B30_FAMILY_ASCII_TEXT     80
#define MSEED_B30_FAMILY_NON_ASCII_TEXT 81
#ifdef MSEED_FLOAT_FAMILY_SUPPORTED
#define MSEED_B30_FAMILY_IEEEFLOAT     100 /* MADE UP NUMBER!  CHANGE IF AND WHEN YOU GET A REAL FAMILY CODE FOR FLOATS */
#endif /* MSEED_FLOAT_FAMILY_SUPPORTED */

#define MSEED_B30_DESC_LEN 50
#define MSEED_B30_KEYS_LEN 1024   /* arbitrarily chosen to be "big enough" */
#define MSEED_B30_STRING_LEN 1024 /* ditto */

typedef struct {
    int code;                            /* data format , as MSEED_B30_CODE_x */
    int family;                          /* data family type, as MSEED_B30_FAMILY_x */
    struct {
        char text[MSEED_B30_KEYS_LEN+1]; /* tilde delimited list of decoder keys */
//      LNKLST head;                     /* EVENTUALLY (ie never): above string decoded into a linked list */
    } ddl;
    char desc[MSEED_B30_DESC_LEN+1];     /* short descriptive name */
} MSEED_B30;

/* blockette 33 */

#define MSEED_B33_ITEM_LEN 6
#define MSEED_B33_DESC_LEN 50

typedef struct {
    int code;
    char item[MSEED_B33_ITEM_LEN+1];
    char desc[MSEED_B33_DESC_LEN+1];
} MSEED_B33;

/* blockette 34 */

#define MSEED_B34_UNIT_LEN 16
#define MSEED_B34_DESC_LEN 50

typedef struct {
    int code;
    char unit[MSEED_B34_UNIT_LEN+1];
    char desc[MSEED_B34_DESC_LEN+1];
} MSEED_B34;

/* blockette 50 */

#define MSEED_B50_STA_LEN  5
#define MSEED_B50_NET_LEN  2
#define MSEED_B50_DESC_LEN 50

typedef struct {
    char name[MSEED_B50_STA_LEN+1];
    int code;
    INT64 beg;
    INT64 end;
    REAL64 lat;
    REAL64 lon;
    REAL64 elev;
    char desc[MSEED_B50_DESC_LEN+1];
    char net[MSEED_B50_NET_LEN+1];
    char update;
} MSEED_B50;

/* blockette 52 */

#define MSEED_B52_LOC_LEN      2
#define MSEED_B52_CHN_LEN      3
#define MSEED_B52_COMMENT_LEN 31
#define MSEED_B52_FLAG_LEN    27

#define MSEED_B52_FLAG_TRIGGERED    'T'
#define MSEED_B52_FLAG_CONTINUOUS   'C'
#define MSEED_B52_FLAG_HEALTH       'H'
#define MSEED_B52_FLAG_GEOPHYSICAL  'G'
#define MSEED_B52_FLAG_WEATHER      'W'
#define MSEED_B52_FLAG_FLAG         'F'
#define MSEED_B52_FLAG_SYNTHETIC    'S'
#define MSEED_B52_FLAG_CALIBRATION  'I'
#define MSEED_B52_FLAG_EXPERIMENTAL 'E'
#define MSEED_B52_FLAG_MAINTENANCE  'M'
#define MSEED_B52_FLAG_BEAM         'B'

#define MSEED_B52_DEFAULT_SUBCHN          0
#define MSEED_B52_DEFAULT_DFCODE          MSEED_FORMAT_STEIM1
#define MSEED_B52_DEFAULT_DRLEN           MSEED_DEFAULT_RECLEN_EXP
#define MSEED_B52_DEFAULT_NCOMMENTS       0
#define MSEED_B52_DEFAULT_UPDATE          'N'
#define MSEED_B52_CLOCK_TOLERANCE_FACTOR  ((REAL64) 5.e-5)
#define MSEED_B52_CLOCK_TOLERANCE(x)      (x > 0.0 ? (MSEED_B52_CLOCK_TOLERANCE_FACTOR / (x)) : 0.0)

typedef struct {
    char   loc[MSEED_B52_LOC_LEN+1];
    char   chn[MSEED_B52_CHN_LEN+1];
    int    subchn;
    int    instid;
    char   comment[MSEED_B52_COMMENT_LEN+1];
    int    runitid;
    int    cunitid;
    REAL64 lat;
    REAL64 lon;
    REAL64 elev;
    REAL64 depth;
    REAL64 azimuth;
    REAL64 dip;
    int    dfcode;
    int    drlen;
    REAL64 srate;
    REAL64 maxdrift;
    int    ncomments;
    char   flag[MSEED_B52_FLAG_LEN+1];
    INT64  beg;
    INT64  end;
    char   update;
} MSEED_B52;

/* blockette 53 */

typedef struct {
    UTIL_COMPLEX value;
    UTIL_COMPLEX error;
} MSEED_COMPLEX_PZ;

typedef struct {
    int stageid;
    char type;
    int iunits;
    int ounits;
    REAL64 a0;
    REAL64 freq;
    int nzero;
    MSEED_COMPLEX_PZ zero[FILTER_MAX_PZ];
    int npole;
    MSEED_COMPLEX_PZ pole[FILTER_MAX_PZ];
} MSEED_B53;

/* blockette 54 */

typedef struct {
    REAL64 value;
    REAL64 error;
} MSEED_COEFF;

typedef struct {
    int stageid;
    char type;
    int iunits;
    int ounits;
    int nnum;
    MSEED_COEFF num[FILTER_MAX_COEF];
    int nden;
    MSEED_COEFF den[FILTER_MAX_COEF];
} MSEED_B54;

/* blockette 57 */

typedef struct {
    int stageid;
    int factor;
    int offset;
    REAL64 irate;
    REAL64 estdelay;
    REAL64 correction;
} MSEED_B57;

/* blockette 58 */

typedef struct {
    int stageid;
    REAL64 gain;
    REAL64 freq;
} MSEED_B58;

/* Function prototypes */

/* b30.c */
LNKLST *mseedDestroyBlockette30List(LNKLST *list);
LNKLST *mseedDefaultBlockette30List(void);
BOOL mseedWriteBlockette30(FILE *fp, MSEED_B30 *b30);
BOOL mseedWriteBlockette30List(FILE *fp, LNKLST *list);

/* b33.c */
LNKLST *mseedDestroyBlockette33List(LNKLST *list);
BOOL mseedWriteBlockette33(FILE *fp, MSEED_B33 *b33);
BOOL mseedWriteBlockette33List(FILE *fp, LNKLST *list);

/* b34.c */
BOOL mseedWriteBlockette34(FILE *fp, MSEED_B34 *b34);

/* b50.c */
BOOL mseedWriteBlockette50(FILE *fp, MSEED_B50 *b50);

/* b52.c */
BOOL mseedWriteBlockette52(FILE *fp, MSEED_B52 *b52);

/* b53.c */
BOOL mseedWriteBlockette53(FILE *fp, MSEED_B53 *b53);

/* b54.c */
BOOL mseedWriteBlockette54(FILE *fp, MSEED_B54 *b54);

/* b57.c */
BOOL mseedWriteBlockette57(FILE *fp, MSEED_B57 *b57);

/* b58.c */
BOOL mseedWriteBlockette58(FILE *fp, MSEED_B58 *b58);

/* chnloc.c */
char *mseedChnLocToChnloc(char *chnid, char *locid, char *chnloc);

/* copy.c */
int mseedCopyINT16(INT16 *dest, INT16 *src, INT32 nsamp);
int mseedCopyINT32(INT32 *dest, INT32 *src, INT32 nsamp);
int mseedCopyREAL32(REAL32 *dest, REAL32 *src, INT32 nsamp);
int mseedCopyREAL64(REAL64 *dest, REAL64 *src, INT32 nsamp);

/* convert.c */
BOOL mseedConvertIsiRawPacket(MSEED_HANDLE *handle, MSEED_RECORD *dest, ISI_RAW_PACKET *raw);

/* data.c */
BOOL mseedPackData(MSEED_HANDLE *handle, MSEED_PACKED *packed, MSEED_RECORD *record, int *pnsamp, int *pnframe, int *format);

/* header.c */
BOOL mseedPackHeader(MSEED_HANDLE *handle, MSEED_PACKED *packed, MSEED_RECORD *record, int nsamp, int nframe, int format);

/* generic.c */
BOOL mseedConvertGenericTS(MSEED_HANDLE *handle, MSEED_RECORD *dest, ISI_GENERIC_TS *gen);

/* handle.c */
BOOL mseedSetOptions(MSEED_HANDLE *handle, INT32 value);
BOOL mseedSetDebug(MSEED_HANDLE *handle, int value);
BOOL mseedSetClockThreshold(MSEED_HANDLE *handle, int locked, int suspect);
BOOL mseedSetStaid(MSEED_HANDLE *handle, char *staid);
BOOL mseedSetNetid(MSEED_HANDLE *handle, char *netid);
BOOL mseedSetIDA10Chanmap(MSEED_HANDLE *handle, LNKLST *chanmap);
BOOL mseedSetDriftRate(MSEED_HANDLE *handle, REAL64 drift);
INT32 mseedIncrementSeqno(MSEED_HANDLE *handle, char *ident);
MSEED_HANDLE *mseedDestroyHandle(MSEED_HANDLE *handle);
MSEED_HANDLE *mseedCreateHandle(LOGIO *lp, int reclen, char qc, int seqno, MSEED_SAVE_FUNC func, void *argptr);
BOOL mseedFlushRecord(MSEED_HANDLE *handle, MSEED_RECORD *record);
BOOL mseedFlushHandle(MSEED_HANDLE *handle);

/* history.c */
MSEED_DATUM *mseedGetHistory(MSEED_HANDLE *handle, MSEED_RECORD *record, char *ident);

/* ida9.c */
BOOL mseedConvertIDA9(MSEED_HANDLE *handle, MSEED_RECORD *dest, UINT8 *src);

/* ida10.c */
BOOL mseedConvertIDA10(MSEED_HANDLE *handle, MSEED_RECORD *dest, UINT8 *src);

/* ida1012.c */
UINT8 *mseed512ToIDA1012(UINT8 *mseed512, UINT8 *dest, char *sname, char *nname, UINT64 serialno);

/* log.c */
VOID mseedLog(MSEED_HANDLE *handle, int level, CHAR *format, ...);

/* pack.c */
BOOL mseedPackRecord(MSEED_HANDLE *handle, MSEED_RECORD *record, BOOL flush);

/* print.c */
char *mseedDataFormatString(int code);
char *mseedHdrString(MSEED_HDR *hdr, char *buf);
void mseedPrintHeader(FILE *fp, MSEED_HDR *hdr);

/* read.c */
BOOL mseedReadRecord(FILE *fp, MSEED_RECORD *dest);
BOOL mseedReadPackedRecord(FILE *fp, MSEED_PACKED *dest);

/* record.c */
BOOL mseedAddRecordToHandle(MSEED_HANDLE *handle, MSEED_RECORD *new);

/* steim.c */
int mseedPackSteim1(UINT8 *output, INT32 *data, INT32 d0, int ns, int nf, int pad, int *pnframes, int *pnsamples);
int mseedPackSteim2(UINT8 *output, INT32 *data, INT32 d0, int ns, int nf, int pad, int *pnframes, int *pnsamples);

/* time.c */
char *mseedSetTIMEstring(char *dest, INT64 tstamp, BOOL full);

/* unpack.c */
INT16 mseedUnpackINT16(UINT8 *src, BOOL swap);
INT32 mseedUnpackINT32(UINT8 *src, BOOL swap);
INT64 mseedUnpackBtime(UINT8 *src, BOOL swap);
UINT64 mseedUnpackSampleInterval(UINT8 *src, BOOL swap);
void mseedUnpackB100(MSEED_HDR *dest, UINT8 *src, BOOL swap);
void mseedUnpackB1000(MSEED_HDR *dest, UINT8 *src, BOOL unused);
void mseedUnpackB1001(MSEED_HDR *dest, UINT8 *src, BOOL unused);
int mseedHeaderByteOrder(UINT8 *raw);
int mseedUnpackFSDH(MSEED_HDR *dest, UINT8 *src);
BOOL mseedUnpackHeader(MSEED_HDR *dest, UINT8 *src);
BOOL mseedUnpackRecord(MSEED_RECORD *dest, UINT8 *src);

/* util.c */
INT64 mseedTimeTearInSamples(MSEED_HDR *a, MSEED_HDR *b);
char *mseedCSSDatatypeString(MSEED_HDR *hdr);
char *mseedChnloc(MSEED_HDR *hdr, char *buf);
char mseedStageType(FILTER *filter);
BOOL mseedValidQC(char qc);
UINT64 mseedFactMultToNsint(int fact, int mult);
void mseedNsintToFactMult(UINT64 nsint, int *fact, int *mult);
char *mseedSetIdent(MSEED_HDR *hdr);
void mseedSetEndtime(MSEED_HDR *hdr);
void mseedSetActualSint(MSEED_HDR *hdr, REAL64 drift);
int mseedPackedHeaderLength(MSEED_HDR *hdr);
int mseedMaxSamp(MSEED_HANDLE *handle, MSEED_HDR *hdr, BOOL flush);
void mseedInitHeader(MSEED_HDR *hdr);

/* version.c */
char *mseedVersionString(VOID);
VERSION *mseedVersion(VOID);

#ifdef __cplusplus
}
#endif

#endif

/*-----------------------------------------------------------------------+
 |                                                                       |
 | Copyright (C) 2014 Regents of the University of California            |
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
 * $Log: mseed.h,v $
 * Revision 1.19  2018/01/18 23:36:20  dechavez
 * updated mseed512ToIDA1012() prototype to include serial number
 *
 * Revision 1.18  2018/01/13 00:59:57  dechavez
 * updated mseed512ToIDA1012() prototype to include sname and nname arguments
 *
 * Revision 1.17  2017/11/21 21:05:14  dechavez
 * changed mseed512ToIDA1012() to return UINT8 * instead of BOOL
 *
 * Revision 1.16  2017/11/21 18:04:44  dechavez
 * added mseedChnLocToChnloc() and mseed512ToIDA1012() prototypes
 *
 * Revision 1.15  2017/10/20 01:33:22  dauerbach
 * added and edited some convenience #defines:
 *     #define MSEED_CHNLOCLEN (MSEED_CNAMLEN + MSEED_LNAMLEN)
 *     #define MSEED_BLANK_LOCID "  "
 *     #define MSEED_DEFAULT_LOCID MSEED_BLANK_LOCID
 *
 * Revision 1.14  2015/12/08 17:47:56  dechavez
 * updated prototypes
 *
 * Revision 1.13  2015/11/17 18:49:07  dechavez
 * added options field to MSEED_HANDLE
 *
 * Revision 1.12  2015/10/02 15:34:00  dechavez
 * added sig field to MSEED_HDR, mseedTimeTearInSamples() and mseedCSSDatatypeString() prototypes
 *
 * Revision 1.11  2015/09/30 20:15:06  dechavez
 * added mseedChnloc() protoype
 * changed MSEED_B52_CLOCK_TOLERANCE() macro to avoid division by zero
 *
 * Revision 1.10  2015/09/24 22:11:41  dechavez
 * changed MSEED_HDR nsamp from INT16 to INT32 and updated mseedCopyX() protoypes accordingly
 *
 * Revision 1.9  2015/09/15 23:19:42  dechavez
 * changed MSEED_FORMAT_x defines to match SEED specification(!), updated prototypes
 *
 * Revision 1.8  2015/08/24 18:56:06  dechavez
 * reworked MSEED_B30_type_CODEs to use MSEED_FORMAT_types, fixed some DDL typos,
 * ifdef'd out my made up MSEED_B30_FAMILY_IEEEFLOAT
 *
 * Revision 1.7  2015/07/15 17:00:23  dechavez
 * fixed MSEED_B30_STEIM2_CODE (wasn't unique), fixed MSEED_B50_STA_LEN (was too big)
 *
 * Revision 1.6  2015/07/10 17:55:51  dechavez
 * addes support for blockettes 53, 54, 57, 58
 *
 * Revision 1.5  2015/06/30 19:13:32  dechavez
 * MSEED_B52 defined, MSEED_B50 lat/lon/elev changed to REAL64, mseedSetTIMEstring() prototype updated
 *
 * Revision 1.4  2015/03/06 23:09:14  dechavez
 * added MSEED_B52_FLAG_* constants
 *
 * Revision 1.3  2014/10/29 21:30:03  dechavez
 * MSEED_B30, MSEED_B33, MSEED_B34, MSEED_B50 typedefs defined, MSEED_B52 in progress
 *
 * Revision 1.2  2014/08/19 17:57:59  dechavez
 * removed packed and len fields from MSEED_RECORD and introduced MSEED_PACKED
 *
 * Revision 1.1  2014/08/11 18:20:30  dechavez
 * initial release
 *
 */
