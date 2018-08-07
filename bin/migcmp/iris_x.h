/* @(#)iris.h	1.37 6/3/93 */
/*======================================================================
 *
 *  include/iris.h
 *
 *  Include file of defines and type declarations for routines which
 *  manipulate IGPP IRIS field data.
 *
 *====================================================================*/
#ifndef iris_h_included
#define iris_h_included

/* Constants */

#include <stdio.h>
#include <sys/types.h>

#define IRIS_HEAD_SIZE (64)
#define IRIS_DATA_SIZE (960)
#define IRIS_BUFSIZ    (IRIS_HEAD_SIZE + IRIS_DATA_SIZE)
#define IRIS_MAX_DATSIZ (962)

#define IRIS_MAXCNFG   (225) /* MAX dim of configuration table  */
#define MAX_HREC       (10)  /* MAX number of H record types    */

#define IRIS_UNDEFINED (-1)

/*  Data logger id's       */

enum { IRIS_MK3, IRIS_MK4, IRIS_MK5, IRIS_MK6, IRIS_MK7, IRIS_MK8, IRIS_Q330 }; 

/*  Possible sources of data records  */

#define DAS 0
#define ARS 1

/*  Calibration type codes  */

#define IRIS_RANDOM_STEP (1)
#define IRIS_STEP        (2)
#define IRIS_PULSE       (3)
#define IRIS_SINE        (4)
#define IRIS_TRIANGLE    (5)

#define IRIS_LABEL "Soviet IRIS/IDA archive tape"

#ifndef TRUE
#define TRUE 1
#endif
#ifndef FALSE
#define FALSE 0
#endif

#ifndef SUN_ORDER
#define SUN_ORDER 1
#endif
#ifndef VAX_ORDER
#define VAX_ORDER 2
#endif
#ifndef PDP_ORDER
#define PDP_ORDER 3
#endif

#ifndef SEEK_SET
#define SEEK_SET 0
#endif

#ifndef SEEK_CUR
#define SEEK_CUR 1
#endif

#ifndef SEEK_END
#define SEEK_END 2
#endif

/*  Format codes  */

typedef enum iris_form {
    S_UNCOMP,   /* 0 = uncompressed shorts      */
    L_UNCOMP,   /* 1 = uncompressed longs       */
    IGPP_COMP,  /* 2 = IGPP compression         */
    STEIM_COMP, /* 3 = Steim compression        */
    BAD_FORM    /* 4 = unrecognized format code */
} IRIS_FORM;

/* Mode codes  */

typedef enum iris_mode {
    CONT,       /* 0 = continuous recording   */
    TRIG,       /* 1 = triggered              */
    BAD_MODE    /* 2 = unrecognized mode code */
} IRIS_MODE;

/*  Digital filter codes  */
         
typedef enum iris_filt {
    NONE,       /*  0 = no filtering                            */
    SDECIM,     /*  1 = straight decimation                     */
    RDECIM,     /*  2 = decimated with running mean decimator   */
    BB_5,       /*  3 = broad-band filter, 5 (or 2.5) sps       */
    BB_20,      /*  4 = broad-band filter, 20 sps               */
    LP,         /*  5 = long-period filter, 0.1 sps             */
    MP,         /*  6 = mass-position, 0.1 (or 0.0166666) sps   */
    DETECT,     /*  7 = detection bandpass filter               */
    BB_1,       /*  8 = broad-band filter, 1 sps                */
    SP_40,      /*  9 = short-period filter, 40 sps             */
    BAD_FILT    /* 10 = unrecognized filter code                */
} IRIS_FILT;

/*  Analog input channel codes  */

typedef enum iris_chan {
    BBZ_HI,     /*  0 = Z broad-band sensor, hi-gain       */
    BBN_HI,     /*  1 = N broad-band sensor, hi-gain       */
    BBE_HI,     /*  2 = E broad-band sensor, hi-gain       */
    BBZ_LO,     /*  3 = Z broad-band sensor, lo-gain       */
    BBN_LO,     /*  4 = N broad-band sensor, lo-gain       */
    BBE_LO,     /*  5 = E broad-band sensor, lo-gain       */
    SPZ_HI,     /*  6 = Z short-period sensor, hi-gain     */
    SPN_HI,     /*  7 = N short-period sensor, hi-gain     */
    SPE_HI,     /*  8 = E short-period sensor, hi-gain     */
    SPZ_LO,     /*  9 = Z short-period sensor, lo-gain     */
    SPN_LO,     /* 10 = N short-period sensor, lo-gain     */
    SPE_LO,     /* 11 = E short-period sensor, lo-gain     */
    MPZ,        /* 12 = Z STS mass position                */
    MPN,        /* 13 = N STS mass position                */
    MPE,        /* 14 = E STS mass position                */
    MODE,       /* 15 = IDA mode (LaCoste)                 */
    TIDE,       /* 16 = IDA tide (LaCoste)                 */
    BARO,       /* 17 = microbarograph                     */
    NSLSM,      /* 18 = */
    EWLSM,      /* 19 = */
    NWLSM,      /* 20 = */
    T_DAS,      /* 21 = DAS temperature                    */
    T_ARS,      /* 22 = ARS temperature                    */
    V_DAS,      /* 23 = DAS voltage                        */
    FBAZ,       /* 24 = Z strong motion sensor             */
    FBAN,       /* 25 = N strong motion sensor             */
    FBAE,       /* 26 = E strong motion sensor             */
    OTHER_CHAN, /* 27 = recognized but unsupported channel */
    BAD_CHAN    /* 28 = unrecognized channel code          */
} IRIS_CHAN;

/*  Stream number or ID */

#define BBZ_HI_S  0
#define BBN_HI_S  1
#define BBE_HI_S  2
#define BBZ_LO_S  3
#define BBN_LO_S  4
#define BBE_LO_S  5
#define LPZ_HI_S  6
#define LPN_HI_S  7
#define LPE_HI_S  8
#define MPZ_S     9
#define MPN_S    10
#define MPE_S    11
#define DETZ_S   12
#define SPZ_HI_S 13
#define SPN_HI_S 14
#define SPE_HI_S 15
#define SPZ_LO_S 16
#define SPN_LO_S 17
#define SPE_LO_S 18
#define MODE_S   19
#define TIDE_S   20
#define BARO_S   21
#define NSLSM_S  22
#define EWLSM_S  23
#define NWLSM_S  24
#define LPZ_LO_S 25
#define LPN_LO_S 26
#define LPE_LO_S 27
#define T_DAS_S  28
#define T_ARS_S  29
#define V_DAS_S  30
#define LPFBAZ_S 31
#define LPFBAN_S 32
#define LPFBAE_S 33
#define FBAZ_S   34
#define FBAN_S   35
#define FBAE_S   36
#define BAD_STRM 37

/*  These can be used to dimension arrays  */

#define NFORM (BAD_FORM + 1)
#define NMODE (BAD_MODE + 1)
#define NFILT (BAD_FILT + 1)
#define NCHAN (BAD_CHAN + 1)
#define NSTRM (BAD_STRM + 1)

/*  Time structure  */

typedef struct iris_time {
    long  sec;       /* Seconds since Jan 1 00:00 of current year */
    short msec;      /* Fractional part in milliseconds           */
    char  sign;      /* '+' or '-'                                */
} IRIS_TIME;

struct timepieces { int year, day, hour, min, sec, msec; };

struct leapsec {
    int year;
    int day;
};

struct chan_map {
    int strm;
    int filt;
    int chan;
    int mode;
    char css[4];
    char seed[4];
    float calper;
};

/* 'C' (calibration) record */

typedef struct iris_crec {

    /* header area */
    double        ext;        /* external time              */
    double        sys;        /* system (1-Hz) time         */
    double        tag;        /* time tag                   */
    double        off;        /* external - system time     */
    short         type;       /* calibration type code      */
    short         period;     /* input wave period (sec)    */
    short         amp;        /* amplitude                  */
    short         nsteps;     /* number of steps            */
    float         sint;       /* sample interval in seconds */

    /* data area */
    short data[IRIS_MAX_DATSIZ/2]; /* DAC output or random step seeds  */

} IRIS_CREC;

/* 'D' (data) record */

typedef struct iris_drec {

    /*  header area  */
    IRIS_MODE     mode;       /* internal mode code          */
    IRIS_FORM     form;       /* internal format code        */
    IRIS_CHAN     chan;       /* internal channel channel    */
    IRIS_FILT     filt;       /* internal filter  code       */
    char          stream;     /* internal stream code        */
    int           logger;     /* internal logger type code   */
    char          old_mode;   /* data logger mode code       */
    char          old_form;   /* data logger format code     */
    char          old_chan;   /* data logger channel code    */
    char          old_filt;   /* data logger filter code     */
    char          old_stream; /* data logger stream code     */
    double        ext;        /* external time               */
    double        sys;        /* system (1-Hz) time          */
    double        tag;        /* time tag                    */
    double        off;        /* external - system           */
    char          tqual;      /* external time quality code  */
    long          gain;       /* value of amplifier gain     */
    short         nsamp;      /* no. of samples in data area */
    float         sint;       /* sample interval in seconds  */
    char          wrdsiz;     /* uncompressed word size      */

    /*  data area  */
    union {
      char  b[IRIS_MAX_DATSIZ];
      short i[IRIS_MAX_DATSIZ/2];
      long  l[IRIS_MAX_DATSIZ/4];
      float f[IRIS_MAX_DATSIZ/4];
      double d[IRIS_MAX_DATSIZ/8];
    } data;

} IRIS_DREC;

/* Configration table (pre-Horwitt style) */

typedef struct config {
    short nrows;               /* number of rows         */
    short ncols;               /* number of columns      */
    short table[IRIS_MAXCNFG]; /* the configration table */
} CONFIG;

/* Event detector parameters (pre-Horwitt style) */

typedef struct trig_dat {
    short     sta;          /* short term average time (msc)           */
    short     lta;          /* long term average time (sec)            */
    short     thresh_off;   /* threshold offset                        */
    short     turnon_fact;  /* turn on:  STA/LTA * 100                 */
    short     turnoff_fact; /* turn off: STA/LTA * 100                 */
    short     min_rtime;    /* minimum no. seconds to record per event */
    short     max_rtime;    /* maximum no. seconds to record per event */
    short     min_trgint;   /* minimum no. seconds between events      */
    short     dfg;          /* detector filter gain                    */
} TRIG_DAT;

/* 'H' (header) record  (pre-Horwitt style) */

typedef struct iris_hrec {

    char      id;            /* head record counter                */
    char      sname[7];      /* station name                       */
    char      rev[6];        /* firmware revision code             */
    char      das_id[8];     /* DAS ident                          */
    char      ars_id[8];     /* ARS ident                          */
    short     year;          /* year tape began recording          */
    double    ext;           /* absolute time tag, double          */
    CONFIG    config;        /* configuration table                */
    short     numtap;        /* number of tape drives              */
    short     reclen;        /* tape record length                 */
    short     rpf;           /* records per tape file              */
    short     fpt;           /* maximum number of files per tape   */
    TRIG_DAT  bb_trig;       /* BB event detector parameters       */
    TRIG_DAT  sp_trig;       /* SP event detector parameters       */
    short     mt1_errors[3]; /* tape unit 1 error counts           */
    short     mt2_errors[3]; /* tape unit 2 error counts           */
    short     ars_errors[3]; /* data transmission error counts     */
    short     das_errors[4]; /* data acquisition  error counts     */
    short     ars_reboots;   /* number of ARS reboots              */
    short     das_reboots;   /* number of DAS reboots              */

} IRIS_HREC;

/*  ID record (Horwitt style)  */

typedef struct iris_idrec {
    char       sname[8];   /* station name                     */
    double     ext;        /* absolute time stamp              */
    double     last_start; /* external time of LAST tape start */
    double     last_end;   /* external time of LAST tape end   */
    int        logger;     /* data logger flag                 */
    short      cpu_id;     /* ID no. on data logger CPU        */
    short      year;       /* year                             */
    short      rev;        /* data logger firmware rev         */
} IRIS_IDREC;

/*  E record (Horwitt style)  */
#define NEREC	54		/* # of entries in IRIS_EREC */
typedef struct {
	double event_time;	/* event time */
	float magnitude;	/* approximate magnitude */
} IRIS_EREC;

/*  Login file entry  */

typedef struct iris_entry {
    char sname[4];  /*  Station code                     */
    int  id;        /*  IGPP assigned tape ID number     */
    char type;      /*  A (for archive) or F (for field) */
    char start[13]; /*  Start time (yy:ddd-hh:mm)        */
    char stop[13];  /*  Stop    "         "              */
    char rev[6];    /*  Firmware revision code           */
    int  rev_level; /*  Numerical revision code (yymm)   */
    int  system;    /*  IRIS_MK3, IRIS_MK4, etc.         */
    int  skip;      /*  Number of 1K records to skip     */
    long size;      /*  Size of file in K bytes          */
    char logged[7]; /*  Login date (yy:ddd)              */
} IRIS_ENTRY;

/*  Structure for css calib, calper                               */

struct calib {
    int chan;        /* css chan, integer                         */
    float cal;       /* css calib                                 */
    float per;       /* css calper                                */
};

/*  Nom-int function in math lib    */

extern double    fabs();

/*  Non-int functions in libiris.a  */

extern void      crec_0288();
extern void      crec_0189();
extern void      crec_0789();
extern void      drec_0288();
extern void      drec_0888();
extern void      drec_0189();
extern void      drec_0789();
extern void      hrec_0288();
extern void      hrec_0189();
extern void      hrec_0789();
extern void      idrec_0391();
extern void      iris_crec();
extern void      iris_drec();
extern void      iris_hrec();
extern void      iris_idrec();
extern void      mk3_swap();
extern char      *css_chan();
extern char      *seed_chan();
extern char      *seed_chan_map();
extern char      *iris_tim();
extern char      *dbltim_to_ascii();
extern char      *iris_int();
extern char      *iris_itoa();
extern long      get_list();
extern long      iris_dcmp();
extern long      css_date();
extern long      css_today();
extern long      read_discards();
extern float     css_calib();
extern float     css_calper();
extern double    css_time();
extern double    dbltim_to_css();
extern struct leapsec *read_leapsec();
extern struct timepieces *split_csstim();

/* Macros */

#ifndef leap_year
#define leap_year(i) ((i % 4 == 0 && i % 100 != 0) || i % 400 == 0)
#endif
#ifndef dysize
#define dysize(i) (365 + leap_year(i))
#endif

#endif
