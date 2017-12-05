#pragma ident "$Id: soh_blk.h,v 1.1 2014/04/16 15:34:12 dechavez Exp $"
#ifndef soh_blk_h_defined
#define soh_blk_h_defined
#include "platform.h"
#include "util.h"

/*  Blockette 52 field #17: data record length field; Manual page 58  */

#define DATARECLEN  12

/*  Blockette 52 field #19: clock tolerance; Manual page 58  */

#define CLOCK_TOLER  5.e-5

#define SUN_LW_ORDER "3210"  /* Seed manual page 55 */
#define SUN_W_ORDER  "10"


/*  Blockette 30 lookup keys  */

#define SHORTINTS  3
#define STEIMCOMP  4
#define LONGINTS   6
#define ASCIICHARS 7
#define STEIMCOMP2 8

/*  Blockette 33 lookup keys followed by their description and instype listed in IDA.instrument */
/*  Note: new instrument needs to be added in /home/response/Seed/H.A as well */
#define GS_13      16   /* Geotech GS-13 Seismometer: GS13 */
#define STS1HVBB   23   /* Streckeisen STS-1H/VBB Seismometer: STS1HB */
#define STS1VVBB   25   /* Streckeisen STS-1V/VBB Seismometer: STS1VB */
#define IRISIDA    26
#define CONSOLELOG 27
#define GURALP3    28   /* Guralp CMG-3T Seismometer: CMG3T */
#define LACOSTEx   29   /* 'x' because LACOSTE already defined in irisdb.h */
#define KS_54000   31   /* Geotech KS-54000 Borehole Seismometer: K54000 */
#define STS_2      30   /* Streckeisen STS-2 Seismometer: STS2 */
#define FBAx       12   /* Kinemetrics FBA-23 Low-Gain Sensor: FBA23 */
#define GURALP1    32   /* Guralp CMG1 Seismometer: CMG1 or CMG1t */
#define INVAR      33   /* Cambridge Invar wire strainmeter at BFO: STRAIN */
#define BMG        34   /* BM-G relative microbarograph at BFO: BM-G */
#define ASK        35   /* Askania horizontal pendulum at BFO: ASKHP */
#define PARO       36   /* Paroscientific microbarograph: PARO */
#define FBAEST     37   /* Kinemetrics Episensor ES-T: FBAEST */
#define TR240      38   /* Nanometrics Trillium 240 Seismometer: TR240 */
#define MAG_03     39   /* Bartington MAG-03MSESL100 magnetometer: MAG-03 */
#define STS1E3     40   /* Streckeisen STS-1 Seismometer with E300: STS1E3 */
#define STS2_5     41   /* Streckeisen STS-2.5 Seismometer: STS2_5 */
#define SG_056     42   /* GWR Superconducting Gravimeter */
#define Q330       43   /* For GPS clocks, LCE and LCQ channels - SOH (state of health) */

/*  Blockette 34 lookup keys  */
/*  Note: These keys' definitions are defined in /home/response/Seed/H.A */
/*  Note: IMPORTANT - please use these numbers as "iunits" in IDA.stage */  

#define AMPERES       1
#define COUNTS        2
#define METERS        3
#define METERSPERSEC  4
#define METERSPERSEC2 5
#define MILLISECS     6
#define VOLTS         7
#define PASCALS       8
#define STRAIN        9
#define TESLAS       10
#define LCEUNIT      11
#define LCQUNIT      12

#define MAXBLKLEN    10000

struct names {
    char *outdir;                 /* output directory                 */
    char stadir[MAXPATHLEN];      /* station output directory         */
    char stalogdir[MAXPATHLEN];   /* station log output directory     */
    char chndir[MAXPATHLEN];      /* channel output directory         */
};

#endif

/* Revision History
 *
 * $Log: soh_blk.h,v $
 * Revision 1.1  2014/04/16 15:34:12  dechavez
 * Created using code from the DCC
 *
 */
