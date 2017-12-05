#pragma ident "$Id: convertdb.h,v 1.2 2015/03/06 21:44:14 dechavez Exp $"
#ifndef convertdb_h_included
#define convertdb_h_included

#include "dccdb.h"
#include "platform.h"
#include "logio.h"
#include "list.h"
#include "util.h"

extern char *VersionIdentString;

#define MOD_MAIN    1000
#define MOD_SANITY  2000
#define MOD_LOOKUP  3000
#define MOD_ABBREV  4000
#define MOD_SEEDLOC 5000
#define MOD_SITE    6000
#define MOD_UNITS   7000
#define MOD_STAGE   8000
#define MOD_CHAN    9000

#define DEFAULT_OLD_PATH   "/ida/home/iris.epi/dbms/antelope/CSS/IDA"
#define DEFAULT_NEW_SCHEMA "/ida/dcc/db/schemas/ida1.0"
#define DEFAULT_NEW_PREFIX "IDA"
#define DEFAULT_LOGTHRESHOLD 1

/* the old "calibration" relation */

#define OLD_CALIBRATION_TABLE_NAME  "calibration"

#define OLD_CALIBRATION_STA      "sta"
#define OLD_CALIBRATION_CHAN     "chan"
#define OLD_CALIBRATION_TIME     "time"
#define OLD_CALIBRATION_ENDTIME  "endtime"
#define OLD_CALIBRATION_INSNAME  "insname"
#define OLD_CALIBRATION_SAMPRATE "samprate"
#define OLD_CALIBRATION_SEGTYPE  "segtype"
#define OLD_CALIBRATION_CALIB    "calib"
#define OLD_CALIBRATION_CALPER   "calper"
#define OLD_CALIBRATION_FC       "fc"
#define OLD_CALIBRATION_UNITS    "units"
#define OLD_CALIBRATION_LDDATE   "lddate"

#define OLD_CALIBRATION_STA_LEN     6
#define OLD_CALIBRATION_CHAN_LEN    8
#define OLD_CALIBRATION_INSNAME_LEN 50
#define OLD_CALIBRATION_SEGTYPE_LEN 1
#define OLD_CALIBRATION_UNITS_LEN   12

typedef struct {
    char sta[OLD_CALIBRATION_STA_LEN+1];
    char chan[OLD_CALIBRATION_CHAN_LEN+1];
    char insname[OLD_CALIBRATION_INSNAME_LEN+1];
    char segtype[OLD_CALIBRATION_SEGTYPE_LEN+1];
    char units[OLD_CALIBRATION_UNITS_LEN+1];
    double samprate;
    double calib;
    double calper;
    double fc;
    double time;
    double endtime;
    long long lddate;
} OLD_CALIBRATION;

/* the old "instrument" relation */

#define OLD_INSTRUMENT_TABLE_NAME  "instrument"

#define OLD_INSTRUMENT_INID        "inid"
#define OLD_INSTRUMENT_INSNAME     "insname"
#define OLD_INSTRUMENT_INSTYPE     "instype"
#define OLD_INSTRUMENT_BAND        "band"
#define OLD_INSTRUMENT_DIGITAL     "digital"
#define OLD_INSTRUMENT_SAMPRATE    "samprate"
#define OLD_INSTRUMENT_NCALIB      "ncalib"
#define OLD_INSTRUMENT_NCALPER     "ncalper"
#define OLD_INSTRUMENT_DIR         "dir"
#define OLD_INSTRUMENT_DFILE       "dfile"
#define OLD_INSTRUMENT_RSPTYPE     "rsptype"
#define OLD_INSTRUMENT_LDDATE      "lddate"

#define OLD_INSTRUMENT_INSNAME_LEN    50
#define OLD_INSTRUMENT_INSTYPE_LEN     6
#define OLD_INSTRUMENT_BAND_LEN        1
#define OLD_INSTRUMENT_DIGITAL_LEN     1
#define OLD_INSTRUMENT_DIR_LEN        64
#define OLD_INSTRUMENT_DIRFILE_LEN    32
#define OLD_INSTRUMENT_RSPTYPE_LEN     6

typedef struct {
    long inid;
    double samprate;
    double ncalib;
    double ncalper;
    long long lddate;
    char insname[OLD_INSTRUMENT_INSNAME_LEN+1];
    char instype[OLD_INSTRUMENT_INSTYPE_LEN+1];
    char band[OLD_INSTRUMENT_BAND_LEN+1];
    char digital[OLD_INSTRUMENT_DIGITAL_LEN+1];
    char dir[OLD_INSTRUMENT_DIR_LEN+1];
    char dfile[OLD_INSTRUMENT_DIRFILE_LEN+1];
    char rsptype[OLD_INSTRUMENT_DIRFILE_LEN+1];
} OLD_INSTRUMENT;

/* the old "schanloc" relation */

#define OLD_SCHANLOC_TABLE_NAME "schanloc"

#define OLD_SCHANLOC_STA    "sta"
#define OLD_SCHANLOC_FCHAN  "fchan"
#define OLD_SCHANLOC_LOC    "loc"
#define OLD_SCHANLOC_CHAN   "chan"
#define OLD_SCHANLOC_LDDATE "lddate"

#define OLD_SCHANLOC_STA_LEN   6
#define OLD_SCHANLOC_CHAN_LEN  8
#define OLD_SCHANLOC_FCHAN_LEN OLD_SCHANLOC_CHAN_LEN
#define OLD_SCHANLOC_LOC_LEN   8

typedef struct {
    char sta[OLD_SCHANLOC_STA_LEN+1];
    char chan[OLD_SCHANLOC_CHAN_LEN+1];
    char fchan[OLD_SCHANLOC_FCHAN_LEN+1];
    char loc[OLD_SCHANLOC_LOC_LEN+1];
} OLD_SCHANLOC;

/* the old "seedloc" relation */

#define OLD_SEEDLOC_TABLE_NAME   "seedloc"

#define OLD_SEEDLOC_STA        "sta"
#define OLD_SEEDLOC_CHAN       "chan"
#define OLD_SEEDLOC_TIME       "time"
#define OLD_SEEDLOC_ENDTIME    "endtime"
#define OLD_SEEDLOC_NEWCHAN    "newchan"
#define OLD_SEEDLOC_LOCCODE    "loccode"
#define OLD_SEEDLOC_LDDATE     "lddate"

#define OLD_SEEDLOC_STA_LEN     6
#define OLD_SEEDLOC_CHAN_LEN    OLD_SCHANLOC_CHAN_LEN
#define OLD_SEEDLOC_NEWCHAN_LEN OLD_SEEDLOC_CHAN_LEN
#define OLD_SEEDLOC_LOCCODE_LEN 2

typedef struct {
    double time;
    double endtime;
    double lddate;
    char sta[OLD_SEEDLOC_STA_LEN+1];
    char chan[OLD_SEEDLOC_CHAN_LEN+1];
    char newchan[OLD_SEEDLOC_NEWCHAN_LEN+1];
    char loccode[OLD_SEEDLOC_LOCCODE_LEN+1];
} OLD_SEEDLOC;

/* old "sensor" relation */

#define OLD_SENSOR_TABLE_NAME   "sensor"

#define OLD_SENSOR_STA        "sta"
#define OLD_SENSOR_CHAN       "chan"
#define OLD_SENSOR_TIME       "time"
#define OLD_SENSOR_ENDTIME    "endtime"
#define OLD_SENSOR_INID       "inid"
#define OLD_SENSOR_CHANID     "chanid"
#define OLD_SENSOR_JDATE      "jdate"
#define OLD_SENSOR_CALRATIO   "calratio"
#define OLD_SENSOR_CALPER     "calper"
#define OLD_SENSOR_TSHIFT     "tshift"
#define OLD_SENSOR_INSTANT    "instant"
#define OLD_SENSOR_LDDATE     "lddate"

#define OLD_SENSOR_STA_LEN     OLD_SEEDLOC_STA_LEN
#define OLD_SENSOR_CHAN_LEN    OLD_SEEDLOC_CHAN_LEN
#define OLD_SENSOR_INSTANT_LEN 1

typedef struct {
    double time;
    double endtime;
    long inid;
    long chanid;
    long jdate;
    double calratio;
    double calper;
    double tshift;
    double lddate;
    char sta[OLD_SENSOR_STA_LEN+1];
    char chan[OLD_SENSOR_CHAN_LEN+1];
    char instant[OLD_SENSOR_INSTANT_LEN+1];
} OLD_SENSOR;

/* the old "site" relation */
 
#define OLD_SITE_TABLE_NAME "site"

#define OLD_SITE_STA            "sta"
#define OLD_SITE_ONDATE         "ondate"
#define OLD_SITE_OFFDATE        "offdate"
#define OLD_SITE_LAT            "lat"
#define OLD_SITE_LON            "lon"
#define OLD_SITE_ELEV           "elev"
#define OLD_SITE_STANAME        "staname"
#define OLD_SITE_STATYPE        "statype"
#define OLD_SITE_REFSTA         "refsta"
#define OLD_SITE_DNORTH         "dnorth"
#define OLD_SITE_DEAST          "deast"
#define OLD_SITE_LDDATE         "lddate"

#define OLD_SITE_STA_LEN        OLD_SEEDLOC_STA_LEN
#define OLD_SITE_STANAME_LEN    50
#define OLD_SITE_STATYPE_LEN     4
#define OLD_SITE_REFSTA_LEN      6

typedef struct {
    long   ondate;
    long   offdate;
    double begt;
    double endt;
    double lat;
    double lon;
    double elev;
    double dnorth;
    double deast;
    double lddate;
    char sta[OLD_SITE_STA_LEN+1];
    char staname[OLD_SITE_STANAME_LEN+1];
    char statype[OLD_SITE_STATYPE_LEN+1];
    char refsta[OLD_SITE_REFSTA_LEN+1];
} OLD_SITE;

/* the old "sitechan" relation */

#define OLD_SITECHAN_TABLE_NAME "sitechan"

#define OLD_SITECHAN_STA        "sta"
#define OLD_SITECHAN_CHAN       "chan"
#define OLD_SITECHAN_ONDATE     "ondate"
#define OLD_SITECHAN_CHANID     "chanid"
#define OLD_SITECHAN_OFFDATE    "offdate"
#define OLD_SITECHAN_CTYPE      "ctype"
#define OLD_SITECHAN_EDEPTH     "edepth"
#define OLD_SITECHAN_HANG       "hang"
#define OLD_SITECHAN_VANG       "vang"
#define OLD_SITECHAN_DESCRIP    "descrip"
#define OLD_SITECHAN_LDDATE     "lddate"

#define OLD_SITECHAN_STA_LEN     OLD_SEEDLOC_STA_LEN
#define OLD_SITECHAN_CHAN_LEN    OLD_SEEDLOC_CHAN_LEN
#define OLD_SITECHAN_CTYPE_LEN   4
#define OLD_SITECHAN_DESCRIP_LEN 50

typedef struct {
    long   ondate;
    long   offdate;
    double begt;
    double endt;
    long  chanid;
    double edepth;
    double hang;
    double vang;
    double lddate;
    char sta[OLD_SITECHAN_STA_LEN+1];
    char chan[OLD_SITECHAN_CHAN_LEN+1];
    char ctype[OLD_SITECHAN_CTYPE_LEN+1];
    char descrip[OLD_SITECHAN_DESCRIP_LEN+1];
} OLD_SITECHAN;

/* the old "stage" relation */

#define OLD_STAGE_TABLE_NAME    "stage"

#define OLD_STAGE_STA         "sta"
#define OLD_STAGE_CHAN        "chan"
#define OLD_STAGE_TIME        "time"
#define OLD_STAGE_ENDTIME     "endtime"
#define OLD_STAGE_STAGEID     "stageid"
#define OLD_STAGE_SSIDENT     "ssident"
#define OLD_STAGE_GNOM        "gnom"
#define OLD_STAGE_IUNITS      "iunits"
#define OLD_STAGE_OUNITS      "ounits"
#define OLD_STAGE_GCALIB      "gcalib"
#define OLD_STAGE_GTYPE       "gtype"
#define OLD_STAGE_IZERO       "izero"
#define OLD_STAGE_DECIFAC     "decifac"
#define OLD_STAGE_SAMPRATE    "samprate"
#define OLD_STAGE_LEADFAC     "leadfac"
#define OLD_STAGE_DIR         "dir"
#define OLD_STAGE_DFILE       "dfile"
#define OLD_STAGE_LDDATE      "lddate"

#define OLD_STAGE_STA_LEN     OLD_SEEDLOC_STA_LEN
#define OLD_STAGE_CHAN_LEN    OLD_SEEDLOC_CHAN_LEN
#define OLD_STAGE_SSIDENT_LEN 16
#define OLD_STAGE_IUNITS_LEN  16
#define OLD_STAGE_OUNITS_LEN  OLD_STAGE_IUNITS_LEN
#define OLD_STAGE_GTYPE_LEN   20
#define OLD_STAGE_DIR_LEN     OLD_INSTRUMENT_DIR_LEN
#define OLD_STAGE_DFILE_LEN   OLD_INSTRUMENT_DIRFILE_LEN

typedef struct {
    double time;
    double endtime;
    long stageid;
    double gnom;  
    double gcalib;
    long izero;
    long decifac;
    double samprate;
    double leadfac;
    double lddate;
    char sta[OLD_STAGE_STA_LEN+1];
    char chan[OLD_STAGE_CHAN_LEN+1];
    char ssident[OLD_STAGE_SSIDENT_LEN+1];
    char iunits[OLD_STAGE_IUNITS_LEN+1];
    char ounits[OLD_STAGE_OUNITS_LEN+1];
    char gtype[OLD_STAGE_GTYPE_LEN+1];
    char dir[OLD_STAGE_DIR_LEN+1];
    char dfile[OLD_STAGE_DFILE_LEN+1];
} OLD_STAGE;

/* All the old tables in one convenient place */

typedef struct {
    Dbptr all;            /* the entire OLD database */
    Dbptr calibration;    /* the old calibration table */
    Dbptr instrument;     /* the old instrument table */
    Dbptr schanloc;       /* the old schanloc table */
    Dbptr seedloc;        /* the old seedloc table */
    Dbptr sensor;         /* the old sensor table */
    Dbptr site;           /* the old site table */
    Dbptr sitechan;       /* the old sitechan table */
    Dbptr stage;          /* the old stage table */
} OLD_TABLES;

typedef struct {
    LOGIO *lp;                    /* for library error messages */
    OLD_TABLES table;             /* handles to the input database tables */
    OLD_CALIBRATION *calibration; /* array of calibration table entries */
    int ncalibration;             /* number of elements in the above array */
    OLD_INSTRUMENT *instrument;   /* array of instrument table entries */
    int ninstrument;              /* number of elements in the above array */
    OLD_SCHANLOC *schanloc;       /* array of schanloc table entries */
    int nschanloc;                /* number of elements in the above array */
    OLD_SEEDLOC *seedloc;         /* array of seedloc table entries */
    int nseedloc;                 /* number of elements in the above array */
    OLD_SENSOR *sensor;           /* array of sensor table entries */
    int nsensor;                  /* number of elements in the above array */
    OLD_SITE *site;               /* array of site table entries */
    int nsite;                    /* number of elements in the above array */
    OLD_SITECHAN *sitechan;       /* array of sitechan table entries */
    int nsitechan;                /* number of elements in the above array */
    OLD_STAGE *stage;             /* array of stage table entries */
    int nstage;                   /* number of elements in the above array */
} OLD;

/* Function prototypes */

/* abbrev.c */
BOOL VerifyAbbrevItem(char *target);
void BuildAbbrev(OLD *old, DCCDB *new, char *prefix);

/* exit.c */
void GracefulExit(int status);

/* log.c */
void LogRunParameters(int level, int argc, char **argv, char *old_path, char *new_schema, char *new_prefix);
void LogErr(char *format, ...);
void LogMsg(int level, char *format, ...);
void LogMsgLevel(int newlevel);
LOGIO *InitLogging(char *myname, char *path, int level);

/* lookup.c */
LNKLST *GenerateStaChnLocTuplesForNewStage(OLD *old, OLD_STAGE *stage);
LNKLST *GenerateStaChnLocTuplesForNewChan(OLD *old, OLD_SITECHAN *sitechan);
char *LookupInstypeFromInid(OLD *old, int inid);
BOOL LookupCalperInstype(OLD *old, DCCDB *new, OLD_SITECHAN *sitechan, double *ncalper, char *instype);

/* match.c */
BOOL StageSeedlocPartialMatch(OLD_STAGE *stage, OLD_SEEDLOC *seedloc);
BOOL StageSeedlocExactMatch(OLD_STAGE *stage, OLD_SEEDLOC *seedloc);
BOOL SitechaneSeedlocPartialMatch(OLD_SITECHAN *sitechan, OLD_SEEDLOC *seedloc);
BOOL SitechaneSeedlocExactMatch(OLD_SITECHAN *sitechan, OLD_SEEDLOC *seedloc);
BOOL SitechanCalibrationExactMatch(OLD_SITECHAN *sitechan, OLD_CALIBRATION *calibration);
BOOL SitechanCalibrationPartialMatch(OLD_SITECHAN *sitechan, OLD_CALIBRATION *calibration);

/* new.c */
void CloseNewDatabase(void);
DCCDB *CreateNewDatabase(LOGIO *lp);

/* old.c */
void CloseOldDatabase(void);
OLD *OpenOldDatabase(char *dbpath, LOGIO *lp, BOOL CheckSanity);

/* print.c */
void PrintOldStage(FILE *fp, OLD_STAGE *stage);
void PrintOldSeedloc(FILE *fp, OLD_SEEDLOC *seedloc);
void PrintOldSitechan(FILE *fp, OLD_SITECHAN *sitechan);

/* sanity.c */
BOOL SanityCheck(OLD *old);

/* seedloc.c */
void BuildSeedloc(OLD *old, DCCDB *new, char *prefix);

/* site.c */
void BuildSite(OLD *old, DCCDB *new, char *prefix);

/* stage.c */
void BuildStage(OLD *old, DCCDB *new, char *prefix);

/* strings.c */
char *sitechanstr(OLD_SITECHAN *sitechan);
char *seedlocstr(OLD_SEEDLOC *seedloc);
char *stagestr(OLD_STAGE *stage);
char *schanlocstr(OLD_SCHANLOC *schanloc);
char *instrumentstr(OLD_INSTRUMENT *instrument);

/* units.c */
char *LookupUnits(char *string);
void BuildUnits(OLD *old, DCCDB *new, char *prefix);

#endif /* convertdb_h_included */

/* Revision History
 *
 * $Log: convertdb.h,v $
 * Revision 1.2  2015/03/06 21:44:14  dechavez
 * initial production release
 *
 */
