#pragma ident "$Id: filter.h,v 1.4 2015/12/03 17:41:42 dechavez Exp $"
#ifndef filter_h_included
#define filter_h_included

#include "platform.h"
#include "util.h" /* for UTIL_COMPLEX type */

#ifdef __cplusplus
extern "C" {
#endif

/* filter type codes */

#define FILTER_TYPE_NULL       0x0000 /* 0000 0000 0000 0000 */

#define FILTER_TYPE_PANDZ      0x8000 /* 1000 0000 0000 0000 */
#define FILTER_TYPE_ANALOG     0x8001 /* 1000 0000 0000 0001 */
#define FILTER_TYPE_IIR_PZ     0x8002 /* 1000 0000 0000 0010 */
#define FILTER_TYPE_LAPLACE    0x8004 /* 1000 0000 0000 0100 */

#define FILTER_TYPE_COEFF      0x4000 /* 0100 0000 0000 0000 */
#define FILTER_TYPE_COMB       0x4001 /* 0100 0000 0000 0001 */
#define FILTER_TYPE_FIR_SYM    0x4002 /* 0100 0000 0000 0010 */
#define FILTER_TYPE_FIR_ASYM   0x4004 /* 0100 0000 0000 0100 */

#define FILTER_TYPE_OLD_ANALOG   1
#define FILTER_TYPE_OLD_FIR_SYM  2
#define FILTER_TYPE_OLD_IIR_PZ   3
#define FILTER_TYPE_OLD_COMB     4
#define FILTER_TYPE_OLD_FIR_ASYM 6
#define FILTER_TYPE_OLD_LAPLACE  7

/* units codes */

#define FILTER_UNITS_NULL     0
#define FILTER_UNITS_DIS      1
#define FILTER_UNITS_VEL      2
#define FILTER_UNITS_ACC      3
#define FILTER_UNITS_COUNTS   4
#define FILTER_UNITS_PASCALS  5
#define FILTER_UNITS_STRAIN   6
#define FILTER_UNITS_TESLAS   7
#define FILTER_UNITS_USEC     8
#define FILTER_UNITS_RADIANS  9
#define FILTER_UNITS_VOLTS   10
#define FILTER_UNITS_PERCENT 11

/* poles and zeros representation */

#define FILTER_MAX_PZ 32

typedef struct {
    int nzero;                        /* number of zeros */
    int npole;                        /* number of poles */
    UTIL_COMPLEX zero[FILTER_MAX_PZ]; /* complex zeros */
    UTIL_COMPLEX pole[FILTER_MAX_PZ]; /* complex poles */
} FILTER_PANDZ;

#define FILTER_EMPTY_PANDZ {                                                                            \
    0, 0,                                                                                               \
    {                                                                                                   \
        {0.0, 0.0}, {0.0, 0.0}, {0.0, 0.0}, {0.0, 0.0}, {0.0, 0.0}, {0.0, 0.0}, {0.0, 0.0}, {0.0, 0.0}, \
        {0.0, 0.0}, {0.0, 0.0}, {0.0, 0.0}, {0.0, 0.0}, {0.0, 0.0}, {0.0, 0.0}, {0.0, 0.0}, {0.0, 0.0}, \
        {0.0, 0.0}, {0.0, 0.0}, {0.0, 0.0}, {0.0, 0.0}, {0.0, 0.0}, {0.0, 0.0}, {0.0, 0.0}, {0.0, 0.0}, \
        {0.0, 0.0}, {0.0, 0.0}, {0.0, 0.0}, {0.0, 0.0}, {0.0, 0.0}, {0.0, 0.0}, {0.0, 0.0}, {0.0, 0.0}  \
    },                                                                                                  \
    {                                                                                                   \
        {0.0, 0.0}, {0.0, 0.0}, {0.0, 0.0}, {0.0, 0.0}, {0.0, 0.0}, {0.0, 0.0}, {0.0, 0.0}, {0.0, 0.0}, \
        {0.0, 0.0}, {0.0, 0.0}, {0.0, 0.0}, {0.0, 0.0}, {0.0, 0.0}, {0.0, 0.0}, {0.0, 0.0}, {0.0, 0.0}, \
        {0.0, 0.0}, {0.0, 0.0}, {0.0, 0.0}, {0.0, 0.0}, {0.0, 0.0}, {0.0, 0.0}, {0.0, 0.0}, {0.0, 0.0}, \
        {0.0, 0.0}, {0.0, 0.0}, {0.0, 0.0}, {0.0, 0.0}, {0.0, 0.0}, {0.0, 0.0}, {0.0, 0.0}, {0.0, 0.0}  \
    }                                                                                                   \
}

/* coefficients representation */

#define FILTER_MAX_COEF 256

typedef struct {
    int ncoef;                          /* number of coefficients */
    REAL64 delay;                       /* group delay */
    REAL64 coef[FILTER_MAX_COEF];       /* filter coefficients */
} FILTER_COEFF;

#define FILTER_EMPTY_COEFF {                                               \
    0, 0.0,                                                                             \
    {                                                                                   \
        0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, \
        0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, \
        0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, \
        0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, \
        0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, \
        0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, \
        0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, \
        0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, \
        0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, \
        0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, \
        0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, \
        0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, \
        0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, \
        0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, \
        0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, \
        0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0  \
    }                                                                                   \
}

/* A generic filter */

typedef struct {
    int type;             /* FILTER_TYPE_x */
    union {
        FILTER_PANDZ pz; /* poles and zeros */
        FILTER_COEFF cf; /* coefficients */
    } data;
} FILTER;

/* Function prototypes */

/* print.c */
 void filterPrint(FILE *fp, FILTER *filter);

/* read.c */
BOOL filterRead(FILE *fp, FILTER *dest);

/* response.c */
BOOL filterResponse(FILTER *filter, REAL64 freq, int units, REAL64 srate, UTIL_COMPLEX *out);
BOOL filterA0(FILTER *filter, REAL64 freq, REAL64 srate, REAL64 *result);

/* string.c */
char *filterString(FILTER *filter, char *buf);
char *filterTypeString(int code);
int filterUnitsCode(char *string);
char *filterUnitsString(int code);

/* version.c */
char *filterVersionString(void);
VERSION *filterVersion(void);

#ifdef __cplusplus
}
#endif

#endif /* filter_h_included */

/* Revision History
 *
 * $Log: filter.h,v $
 * Revision 1.4  2015/12/03 17:41:42  dechavez
 * fixed a couple of prototypes
 *
 * Revision 1.3  2015/09/30 20:18:51  dechavez
 * define FILTER_UNITS_PERCENT
 *
 * Revision 1.2  2015/08/24 18:37:13  dechavez
 * updated prototypes
 *
 * Revision 1.1  2015/07/15 17:28:02  dechavez
 * created
 *
 */
