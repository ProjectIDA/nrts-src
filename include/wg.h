#pragma ident "$Id: wg.h,v 1.1 2012/07/03 16:19:30 dechavez Exp $"
/*======================================================================
 *
 *  Wave Glider support library
 *
 *====================================================================*/
#ifndef wg_h_included
#define wg_h_included

#include "iacp.h"
#include "util.h"
#include "liss.h" /* for the name length macros */

#ifdef __cplusplus
extern "C" {
#endif

/* IACP payload type codes (2000 - 2999) */

#define WG_IACP_MIN     IACP_TYPE_WG_MIN
#define WG_IACP_WGID    WG_IACP_MIN + 0  /* wave glider identification packet */
#define WG_IACP_SEQNO   WG_IACP_MIN + 1  /* sequence number of last frame receceived from peer */
#define WG_IACP_OBSPKT  WG_IACP_MIN + 2  /* an opaque OBS packet */
#define WG_IACP_MAX     IACP_TYPE_WG_MAX

/* String lengths */

#define WG_STALEN   LISS_SNAMLEN
#define WG_CHNLEN   LISS_CNAMLEN
#define WG_LOCLEN   LISS_LNAMLEN
#define WG_CHNLOCLEN (WG_CHNLEN+WG_LOCLEN)
#define WG_STREAM_NAME_LEN (WG_STALEN + 1 + WG_CHNLEN + 1 + WG_LOCLEN)

#define WG_STA_CHN_LOC_DELIMITERS ".,:;/\\"

#define WG_BLANK_LOC "  "

/* WG sequence numbers (maps to IACP packet type 2001) */

typedef struct {
    UINT16 valid; /* non-zero if next field is valid */
    UINT16 value; /* sequence number */
} WG_OBSPKT_SEQNO;

/* psuedo opaque OBS packet (maps to IACP packet type 2002) */

typedef struct {
    UINT16 seqno;
    UINT16 len;
    UINT8  *payload;
} WG_WGOBS1;

/* OBS packet codes */

#define WG_TYPE_OBS_NOP              0x0000 /* NOP */
#define WG_TYPE_OBS_GET_STATUS       0x0002 /* GET_OBS_STATUS */
#define WG_TYPE_OBS_SET_POWER        0x0003 /* SET_OBS_POWER */
#define WG_TYPE_OBS_SET_SENSOR_POWER 0x0004 /* SET_TRILLIUM_POWER */
#define WG_TYPE_OBS_SET_LS_TRANSMIT  0x0005 /* SET_LS_TRANSMIT */
#define WG_TYPE_OBS_GET_STORED_DATA  0x0006 /* GET_STORED_DATA */
#define WG_TYPE_OBS_LS_DATA          0x0101 /* LS_DATA */
#define WG_TYPE_OBS_REC_DATA         0x0201 /* REC_DATA */
#define WG_TYPE_OBS_STATUS           0x0102 /* OBS_STATUS */

/* OBS packets */

typedef struct {
    UINT16 src;   /* src ID */
    UINT16 type;  /* WG_TYPE_OBS_x */
    UINT16 seqno; /* sequence number */
    UINT16 len;   /* length of data field */
    UINT8 *data;  /* points to data */
} WG_OBS_PAYLOAD;

#define WG_OBS_FIXED_NSAMP 126

typedef struct {
    UINT64 meta;
    UINT32 data[WG_OBS_FIXED_NSAMP];
} WG_OBS_DATA;

/* Defaults */

#define WG_DEFAULT_PORT  57475
#define WG_DEFAULT_HOST  "localhost"

/* Function prototypes */

/* pack.c */
int wgPackSeqno(UINT8 *start, WG_OBSPKT_SEQNO *seqno);
int wgUnpackSeqno(UINT8 *start, WG_OBSPKT_SEQNO *seqno);
int wgUnpackWGID(UINT8 *start, char *dest, int len);
int wgPackWGOBS1(UINT8 *start, WG_WGOBS1 *src);
int wgUnpackWGOBS1(UINT8 *start, WG_WGOBS1 *dest);

/* version.c */
char *wgVersionString(VOID);
VERSION *wgVersion(VOID);

#ifdef __cplusplus
}
#endif

#endif

/* Revision History
 *
 * $Log: wg.h,v $
 * Revision 1.1  2012/07/03 16:19:30  dechavez
 * initial release
 *
 */
