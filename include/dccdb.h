#pragma ident "$Id: dccdb.h,v 1.5 2015/09/30 20:00:27 dechavez Exp $"
#ifndef dccdb_h_included
#define dccdb_h_included

#include "db.h"
#define HAVE_STRLCPY /* Antelope includes its own prototype */
#include "util.h"
#include "list.h"
#include "logio.h"
#include "mseed.h"
#include "filter.h"

#define DCCDB_DEFAULT_PATH        "/ida/dcc/db/response/IDA"
#define DCCDB_READONLY            "r"

#define DCCDB_ENDT_FOREVER        ((INT64) 9084851199999000549)
#define DCCDB_NULL_BEGT           (-9999999999.99900)
#define DCCDB_NULL_ENDT           ( 9999999999.99900)
#define DCCDB_NULL_YEARDAY        -1
#define DCCDB_NULL_STRING         "-"
#define DCCDB_EMPTY_LOC           "  "

#define DCCDB_TABLE_NULL     0
#define DCCDB_TABLE_ABBREV   1
#define DCCDB_TABLE_CHAN     2
#define DCCDB_TABLE_SEEDLOC  3
#define DCCDB_TABLE_SITE     4
#define DCCDB_TABLE_STAGE    5
#define DCCDB_TABLE_UNITS    6

/* names of the fields in the "abbrev" table */

#define DCCDB_ABBREV_TABLE_NAME   "abbrev"

#define DCCDB_ABBREV_ITEM       "item"
#define DCCDB_ABBREV_DESC       "desc"

/* names of the fields in the "chan" table */

#define DCCDB_CHAN_TABLE_NAME   "chan"

#define DCCDB_CHAN_STA          "sta"
#define DCCDB_CHAN_CHN          "chn"
#define DCCDB_CHAN_LOC          "loc"
#define DCCDB_CHAN_BEGT         "begt"
#define DCCDB_CHAN_ENDT         "endt"
#define DCCDB_CHAN_EDEPTH       "edepth"
#define DCCDB_CHAN_HANG         "hang"
#define DCCDB_CHAN_VANG         "vang"
#define DCCDB_CHAN_FLAG         "flag"
#define DCCDB_CHAN_INSTYPE      "instype"
#define DCCDB_CHAN_NOMFREQ      "nomfreq"

#define DCCDB_CHAN_FLAG_MODE_CONTINUOUS  'C'
#define DCCDB_CHAN_FLAG_MODE_TRIGGERED   'T'
#define DCCDB_CHAN_FLAG_TYPE_GEOPHYSICAL 'G'
#define DCCDB_CHAN_FLAG_TYPE_WEATHER     'W'
#define DCCDB_CHAN_FLAG_TYPE_SOH         'H'

/* names of the fields in the "seedloc" table */

#define DCCDB_SEEDLOC_TABLE_NAME "seedloc"

#define DCCDB_SEEDLOC_STA       "sta"
#define DCCDB_SEEDLOC_CHN       "chn"
#define DCCDB_SEEDLOC_LOC       "loc"
#define DCCDB_SEEDLOC_SEEDCHN   "seedchn"
#define DCCDB_SEEDLOC_BEGT      "begt"
#define DCCDB_SEEDLOC_ENDT      "endt"
#define DCCDB_SEEDLOC_LDDATE    "lddate"

/* names of the fields in the "site" table */
 
#define DCCDB_SITE_TABLE_NAME   "site"

#define DCCDB_SITE_STA          "sta"
#define DCCDB_SITE_BEGT         "begt"
#define DCCDB_SITE_ENDT         "endt"
#define DCCDB_SITE_LAT          "lat"
#define DCCDB_SITE_LON          "lon"
#define DCCDB_SITE_ELEV         "elev"
#define DCCDB_SITE_DESC         "staname"
#define DCCDB_SITE_LDDATE       "lddate"

/* names of the fields in the "stage" table */

#define DCCDB_STAGE_TABLE_NAME    "stage"

#define DCCDB_STAGE_STA         "sta"
#define DCCDB_STAGE_CHN         "chn"
#define DCCDB_STAGE_LOC         "loc"
#define DCCDB_STAGE_BEGT        "begt"
#define DCCDB_STAGE_ENDT        "endt"
#define DCCDB_STAGE_STAGEID     "stageid"
#define DCCDB_STAGE_SSIDENT     "ssident"
#define DCCDB_STAGE_GNOM        "gnom"
#define DCCDB_STAGE_GCALIB      "gcalib"
#define DCCDB_STAGE_IUNITS      "iunits"
#define DCCDB_STAGE_OUNITS      "ounits"
#define DCCDB_STAGE_UNUSED1     "izero"
#define DCCDB_STAGE_DECIFAC     "decifac"
#define DCCDB_STAGE_SRATE       "srate"
#define DCCDB_STAGE_UNUSED2     "leadfac"
#define DCCDB_STAGE_DIR         "dir"
#define DCCDB_STAGE_DFILE       "dfile"
#define DCCDB_STAGE_LDDATE      "lddate"

/* names of the fields in the "units" table */

#define DCCDB_UNITS_TABLE_NAME  "units"

#define DCCDB_UNITS_UNIT        "unit"
#define DCCDB_UNITS_DESC        "desc"

/* site table */

#define DCCDB_SITE_STA_LEN      6
#define DCCDB_SITE_DESC_LEN    50

typedef struct {
    char sta[DCCDB_SITE_STA_LEN+1];
    char desc[DCCDB_SITE_DESC_LEN+1];
    REAL64 begt;
    REAL64 endt;
    REAL64 lat;
    REAL64 lon;
    REAL64 elev;
    REAL64 lddate;
} DCCDB_SITE;

/* chan table */

#define DCCDB_CHAN_STA_LEN      6
#define DCCDB_CHAN_CHN_LEN      8
#define DCCDB_CHAN_LOC_LEN      2
#define DCCDB_CHAN_FLAG_LEN     2
#define DCCDB_CHAN_INSTYPE_LEN  6

typedef struct {
    char sta[DCCDB_CHAN_STA_LEN+1];
    char chn[DCCDB_CHAN_CHN_LEN+1];
    char loc[DCCDB_CHAN_LOC_LEN+1];
    char flag[DCCDB_CHAN_FLAG_LEN+1];
    char instype[DCCDB_CHAN_INSTYPE_LEN+1];
    REAL64 begt;
    REAL64 endt;
    REAL64 edepth;
    REAL64 hang;
    REAL64 vang;
    REAL64 nomfreq;
    REAL64 lddate;
} DCCDB_CHAN;

/* stage table */

#define DCCDB_STAGE_STA_LEN      6
#define DCCDB_STAGE_CHN_LEN      8
#define DCCDB_STAGE_LOC_LEN      2
#define DCCDB_STAGE_SSIDENT_LEN 16
#define DCCDB_STAGE_UNITS_LEN   16
#define DCCDB_STAGE_DIR_LEN     64
#define DCCDB_STAGE_DFILE_LEN   32

typedef struct {
    char sta[DCCDB_STAGE_STA_LEN+1];
    char chn[DCCDB_STAGE_CHN_LEN+1];
    char loc[DCCDB_STAGE_LOC_LEN+1];
    char ssident[DCCDB_STAGE_SSIDENT_LEN+1];
    char iunits[DCCDB_STAGE_UNITS_LEN+1];
    char ounits[DCCDB_STAGE_UNITS_LEN+1];
    char dir[DCCDB_STAGE_DIR_LEN+1];
    char dfile[DCCDB_STAGE_DFILE_LEN+1];
    REAL64 begt;
    REAL64 endt;
    INT32 stageid;
    REAL64 gnom;
    REAL64 gcalib;
    INT32 unused1;
    INT32 decifac;
    REAL64 srate;
    REAL64 unused2;
    REAL64 lddate;
    FILTER filter;
} DCCDB_STAGE;

/* cascades are collections of stage records */

#define DCCDB_MAX_CASCADE_ENTRIES 16

typedef struct {
    char sta[DCCDB_STAGE_STA_LEN+1];
    char chn[DCCDB_STAGE_CHN_LEN+1];
    char loc[DCCDB_STAGE_LOC_LEN+1];
    REAL64 begt;
    REAL64 endt;
    REAL64 srate;
    REAL64 freq;
    REAL64 a0;
    int errcode;
    int nentry;
    DCCDB_STAGE entry[DCCDB_MAX_CASCADE_ENTRIES];
    DCCDB_CHAN *chan;
} DCCDB_CASCADE;

/* seedloc table */

#define DCCDB_SEEDLOC_STA_LEN 6
#define DCCDB_SEEDLOC_CHN_LEN 8
#define DCCDB_SEEDLOC_LOC_LEN 2
#define DCCDB_SEEDLOC_SEEDCHN_LEN (DCCDB_SEEDLOC_CHN_LEN - DCCDB_SEEDLOC_LOC_LEN)

typedef struct {
    char sta[DCCDB_SEEDLOC_STA_LEN+1];
    char chn[DCCDB_SEEDLOC_CHN_LEN+1];
    char loc[DCCDB_SEEDLOC_LOC_LEN+1];
    char seedchn[DCCDB_SEEDLOC_SEEDCHN_LEN+1];
    REAL64 begt;
    REAL64 endt;
    REAL64 lddate;
} DCCDB_SEEDLOC;

/* all the tables in one handy structure */

typedef struct {
    Dbptr all;              /* the entire IDA database */
    Dbptr abbrev;           /* the generic abbreviation table */
    Dbptr site;             /* the site table */
    Dbptr chan;             /* the chan table */
    Dbptr stage;            /* the stage table */
    Dbptr units;            /* the units table */
    Dbptr seedloc;          /* the seedloc table */
} DCCDB_TABLES;

/* database handle */

typedef struct {
    LOGIO *lp;              /* for library error messages */
    BOOL verbose;           /* log library information messages if TRUE */
    DCCDB_TABLES table;     /* handles to the input database tables */
    MSEED_B30 *b30;         /* blockette 30 data, generated by the library */
    int nb30;               /* number of elements in the above array */
    MSEED_B33 *b33;         /* blockette 33 data, pulled from the "abbrev" table */
    int nb33;               /* number of elements in the above array */
    MSEED_B34 *b34;         /* blockette 34 data, pulled from the "units" table */
    int nb34;               /* number of elements in the above array */
    DCCDB_SITE *site;       /* array of site table entries */
    int nsite;              /* number of elements in the above array */
    DCCDB_CHAN *chan;       /* array of chan table entries */
    int nchan;              /* number of elements in the above array */
    DCCDB_STAGE *stage;     /* array of stage table entries */
    int nstage;             /* number of elements in the above array */
    DCCDB_SEEDLOC *seedloc; /* array of stage table entries */
    int nseedloc;           /* number of elements in the above array */
    DCCDB_CASCADE *cascade; /* array of response cascades */
    int ncascade;           /* number of elements in the above array */
} DCCDB;

/* Function error codes */

#define DCCDB_ERRCODE_NO_ERROR        0
#define DCCDB_ERRCODE_NULL_UNITS      1
#define DCCDB_ERRCODE_RESPONSE_ERROR  2

/* Function prototypes */

/* abbrev.c */
void dccdbPrintAbbrevRecord(FILE *fp, MSEED_B33 *b33);
BOOL dccdbReadAbbrev(DCCDB *db);

/* cascade.c */
void dccdbPrintCascade(FILE *fp, DCCDB_CASCADE *cascade, BOOL full);
void dccdbPrintCascadeList(FILE *fp, LNKLST *list, BOOL full);
BOOL dccdbBuildCascades(DCCDB *db);

/* chan.c */
void dccdbPrintChanRecord(FILE *fp, DCCDB_CHAN *chan);
BOOL dccdbReadChan(DCCDB *db);

/* db.c */
DCCDB *dccdbOpenDatabase(char *dbpath, LOGIO *lp, BOOL verbose);
DCCDB *dccdbCloseDatabase(DCCDB *db);

/* filter.c *.
BOOL dccdbReadFilterFile(DCCDB_FILTER *dest, char *dir, char *dfile);
void dccdbPrintFilter(FILE *fp, DCCDB_FILTER *filter);
char *dccdbFilterString(DCCDB_FILTER *filter, char *buf);

/* lookup.c */
int dccdbLookupUnitCode(DCCDB *db, char *target);
int dccdbLookupInstCode(DCCDB *db, char *target);
LNKLST *dccdbLookupCascadeFromChan(DCCDB *db, DCCDB_CHAN *target);

/* response.c */
BOOL dccdbSetCascadeA0Freq(DCCDB_CASCADE *cascade, REAL64 freq);

/* seedloc.c */
void dccdbPrintSeedlocRecord(FILE *fp, DCCDB_SEEDLOC *seedloc);
BOOL dccdbReadSeedloc(DCCDB *db);

/* site.c */
void dccdbPrintSiteRecord(FILE *fp, DCCDB_SITE *site);
BOOL dccdbReadSite(DCCDB *db);

/* stage.c */
void dccdbPrintStageRecord(FILE *fp, DCCDB_STAGE *stage);
BOOL dccdbReadStage(DCCDB *db);

/* strings.c */
 char *dccdbTableName(int code);
int dccdbTableCode(char *string);
char *dccdbChanString(DCCDB_CHAN *chan, char *buf);
char *dccdbStageString(DCCDB_STAGE *stage, char *buf);

/* units.c */
void dccdbPrintUnitsRecord(FILE *fp, MSEED_B34 *b34);
BOOL dccdbReadUnits(DCCDB *db);

/* util.c */
REAL64 dccdbOndateToBegt(INT32 yearday);
REAL64 dccdbOffdateToEndt(INT32 yearday);

/* version.c */
char *dccdbVersionString(void);
VERSION *dccdbVersion(void);

#endif /* dccdb_h_included */

/* Revision History
 *
 * $Log: dccdb.h,v $
 * Revision 1.5  2015/09/30 20:00:27  dechavez
 * removed the isduplicate fields from DCCDB_CHAN, DCCDB_STAGE, DCCDB_CASCADE
 * added definitions for DCCDB_EMPTY_LOC and DCCDB_CHAN_FLAG_TYPE_SOH
 *
 * Revision 1.4  2015/09/04 00:32:06  dechavez
 * added chan and isduplicate fields to DCCDB_CASCADE, updated prototypes
 *
 * Revision 1.3  2015/08/24 18:21:32  dechavez
 * added DCCDB_ERRCODE_* constants, turned stage izero and leadfac to unused1 and unused2,
 * added a0 and errcode to DCCDB_CASCADE, added dccdbSetCascadeA0Freq() prototype
 *
 * Revision 1.2  2015/07/10 18:24:47  dechavez
 * changes needed for release 1.1.0
 *
 * Revision 1.1  2015/03/06 23:15:58  dechavez
 * initial release
 *
 */
