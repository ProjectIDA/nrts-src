#pragma ident "$Id: q330cc.h,v 1.40 2017/09/13 23:42:37 dauerbach Exp $"
#ifndef q330cc_h_included
#define q330cc_h_included
#include "platform.h"
#include "stdtypes.h"
#include "qdp.h"
#include "qdp/xml.h"
#include "qdp/tokens.h"
#include "q330.h"

extern char *VersionIdentString;
static char *Copyright = "Copyright (C) 2007 - Regents of the University of California.";

typedef struct {
    char *name;
    int code;
    LNKLST *arglst;
    UINT32 p32;
    char *pstr;
    BOOL update;
    struct {
        int line;
        UINT16 duration;
    } pulse;
    struct {
        UINT32 map[QDP_NSC];
        struct {
            Q330_SENSOR *a;
            Q330_SENSOR *b;
        } sensor;
    } sc;
    UINT16 input_map;
    UINT16 gain_map;
    QDP_TYPE_C1_SPP spp;
    QDP_TYPE_C2_GPS gps;
    Q330_CALIB calib;
    struct {
        char *sname;
        char *nname;
        char *xname;
        BOOL warn;
        BOOL save;
        BOOL boot;
        BOOL verbose;
        UINT32 ip;
        struct {
            char *a;
            char *b;
        } sensor;
    } config;
    struct {
        int port;
        char *sta;
        char *net;
        Q330_LCQ *lcq;
    } token;
} Q330_CMD;

typedef struct {
    Q330_CFG *cfg;
    Q330_ADDR addr;
    int port;
    Q330_CMD cmd;
    int debug;
    LOGIO *lp;
    QDP *qdp;
} Q330;

#define Q330_CMD_UNDEFINED   0
#define Q330_CMD_SAVE        1
#define Q330_CMD_REBOOT      2
#define Q330_CMD_RESYNC      3
#define Q330_CMD_GPS         4
#define Q330_CMD_GPS_ON      5
#define Q330_CMD_GPS_OFF     6
#define Q330_CMD_GPS_COLD    7
#define Q330_CMD_GPS_CNF     8
#define Q330_CMD_GPS_ID      9
#define Q330_CMD_CAL_START  10
#define Q330_CMD_CAL_STOP   11
#define Q330_CMD_CAL_STATUS 12
#define Q330_CMD_IFCONFIG   13
#define Q330_CMD_STATUS     14
#define Q330_CMD_FIX        15
#define Q330_CMD_GLOB       16
#define Q330_CMD_SC         17
#define Q330_CMD_PULSE      18
#define Q330_CMD_AMASS      19
#define Q330_CMD_DCP        20
#define Q330_CMD_CO         21
#define Q330_CMD_SPP        22
#define Q330_CMD_MAN        23
#define Q330_CMD_SAVEBOOT   24
#define Q330_CMD_ID         25
#define Q330_CMD_CALIB      26
#define Q330_CMD_TOKENS     27
#define Q330_CMD_XML        28
#define Q330_CMD_CONFIG     29
#define Q330_CMD_INPUT      30
#define Q330_CMD_FLUSH      31
#define Q330_CMD_EP         32
#define Q330_CMD_EP_CNF     33
#define Q330_CMD_EP_DELAY   34
#define Q330_CMD_COMBO      35
#define Q330_CMD_PREAMP     36
#define Q330_CMD_TRIGINFO   37

typedef struct {
    int code; /* one of Q33_CMD_x, above */
    char *command; /* command the user should type */
    char *description; /* command description */
} COMMAND_MAP;

extern COMMAND_MAP *CommandMap;

/* Function prototypes */

/* main.c */
int main (int argc, char **argv);

/* amass.c */
void AmassHelp(char *name, int argc, char **argv);
BOOL VerifyAmass(Q330 *q330);
void amass(Q330 *q330);

/* calib.c */
void CalibHelp(Q330_CFG *cfg, char *name, int argc, char **argv);
BOOL VerifyCalib(Q330 *q330);
void calStart(Q330 *q330);
void calStop(Q330 *q330);
void calStatus(Q330 *q330);

/* checkout.c */
void CheckoutHelp(char *name, int argc, char **argv);
BOOL VerifyCheckout(Q330 *q330);
void checkout(Q330 *q330);

/* cmd.c */
int CommandCode(char *command);
BOOL BadArgLstCount(Q330 *q330);
BOOL UnexpectedArg(Q330 *q330, char *arg);
BOOL VerifyQ330Cmd(Q330 *q330);
void InitCommandMap(void);

/* combo.c */
void ComboHelp(char *name, int argc, char **argv);
BOOL VerifyCombo(Q330 *q330);
void combo(Q330 *q330);

/* config.c */
void ConfigHelp(char *name, int argc, char **argv);
BOOL VerifyConfig(Q330 *q330);
void config(Q330 *q330);

/* dcp.c */
void DcpHelp(char *name, int argc, char **argv);
BOOL VerifyDcp(Q330 *q330);
void dcp(Q330 *q330);

/* ep.c */
void EpHelp(char *name, int argc, char **argv);
BOOL VerifyEp(Q330 *q330);
void EpCnf(Q330 *q330);
void EpDelay(Q330 *q330);

/* exit.c */
void GracefulExit(Q330 *q330, int status);

/* fixvar.c */
void FixHelp(char *name, int argc, char **argv);
BOOL VerifyFix(Q330 *q330);
void fix(Q330 *q330);

/* flush.c */
void FlushHelp(char *name, int argc, char **argv);
BOOL VerifyFlush(Q330 *q330);
void flush(Q330 *q330);

/* glob.c */
void GlobHelp(char *name, int argc, char **argv);
BOOL VerifyGlob(Q330 *q330);
BOOL glob(Q330 *q330);

/* gps.c */
void GpsHelp(char *name, int argc, char **argv);
BOOL VerifyGps(Q330 *q330);
void gpsON(Q330 *q330);
void gpsOFF(Q330 *q330);
void gpsColdStart(Q330 *q330);
void gpsCnf(Q330 *q330);
void gpsId(Q330 *q330);

/* help.c */
void help(Q330_CFG *cfg, int argc, char **argv);

/* id.c */
void IdHelp(char *name, int argc, char **argv);
BOOL VerifyId(Q330 *q330);
void id(Q330 *q330);

/* ifconfig.c */
void IfconfigHelp(char *name, int argc, char **argv);
BOOL VerifyIfconfig(Q330 *q330);
void ifconfig(Q330 *q330);

/* init.c */
void PrintCommandLineHelp(char *myname);
void init(int argc, char **argv, Q330 *q330);

/* input.c */
void InputHelp(char *name, int argc, char **argv);
BOOL VerifyInput(Q330 *q330);
void input(Q330 *q330);

/* list.c */
void ListDigitizers(Q330 *q330);

/* man.c */
void ManHelp(char *name, int argc, char **argv);
BOOL VerifyMan(Q330 *q330);
void man(Q330 *q330);

/* preamp.c */
void PreampHelp(char *name, int argc, char **argv);
BOOL VerifyPreamp(Q330 *q330);
void preamp(Q330 *q330);

/* pulse.c */
void PulseHelp(char *name, int argc, char **argv);
BOOL UpdateBitmap(UINT16 *bitmap, UINT32 *sc, int line);
BOOL VerifyPulse(Q330 *q330);
void pulse(Q330 *q330);

/* reboot.c */
void RebootHelp(char *name, int argc, char **argv);
BOOL VerifyReboot(Q330 *q330);
void boot(Q330 *q330);

/* resync.c */
void ResyncHelp(char *name, int argc, char **argv);
BOOL VerifyResync(Q330 *q330);
void resync(Q330 *q330);

/* save.c */
void SaveHelp(char *name, int argc, char **argv);
BOOL VerifySave(Q330 *q330);
void save(Q330 *q330);
void saveboot(Q330 *q330);

/* sc.c */
void SCHelp(char *name, int argc, char **argv);
BOOL rqsc(Q330 *q330, UINT32 *sc);
BOOL VerifySC(Q330 *q330);
BOOL ConfigSensorControl(Q330 *q330, Q330_SENSOR *a, Q330_SENSOR *b);
void sc(Q330 *q330);

/* set.c */
void SetDigitizer(Q330 *q330, int argc, char **argv);

/* spp.c */
void SppHelp(char *name, int argc, char **argv);
BOOL VerifySpp(Q330 *q330);
void spp(Q330 *q330);

/* status.c */
void StatusHelp(char *name, int argc, char **argv);
BOOL VerifyStatus(Q330 *q330);
void status(Q330 *q330);

/* token.c */
void TokensHelp(char *name, int argc, char **argv);
BOOL VerifyTokens(Q330 *q330);
void tokens(Q330 *q330);

/* triginfo.c */
void TriginfoHelp(char *name, int argc, char **argv);
BOOL VerifyTriginfo(Q330 *q330);
void triginfo(Q330 *q330);

/* xml.c */
void XmlHelp(char *name, int argc, char **argv);
BOOL VerifyXml(Q330 *q330);
BOOL xml(Q330 *q330);

#endif /* q330cc_h_included */

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
 * $Log: q330cc.h,v $
 * Revision 1.40  2017/09/13 23:42:37  dauerbach
 * Implement q330 preamp command to turn 'on' or 'off' preamp for sensor 'a' or 'b'
 *
 * Revision 1.39  2016/07/20 22:32:30  dechavez
 * removed Q330_DEFAULT_MAXTRY
 *
 * Revision 1.38  2016/06/23 20:31:24  dechavez
 * added combo support
 *
 * Revision 1.37  2016/02/11 19:11:15  dechavez
 * changed Q330_DEFAULT_MAXSRQ to Q330_DEFAULT_MAXTRY (libqdp 3.11.3 convention)
 *
 * Revision 1.36  2016/02/05 16:56:43  dechavez
 * define Q330_DEFAULT_MAXSRQ (5)
 *
 * Revision 1.35  2016/02/04 00:31:46  dechavez
 * updated prototypes
 *
 * Revision 1.34  2016/02/03 20:16:41  dechavez
 * added EpDelay() prototype
 *
 * Revision 1.33  2016/01/23 00:23:40  dechavez
 * added Q330_CMD_EP_DELAY support
 *
 * Revision 1.32  2016/01/21 18:56:43  dechavez
 * added ep.c prototypes
 *
 * Revision 1.31  2014/08/11 19:19:53  dechavez
 * changed prototypes to reflect new function names, added qdp/xml.h and qdp/tokens.h
 * to the list of included files
 *
 * Revision 1.30  2014/01/24 00:05:26  dechavez
 * removed BOOL NeedRegistration from Q330 structure
 *
 * Revision 1.29  2012/08/31 18:20:20  dechavez
 * added warn field to config structure
 *
 * Revision 1.28  2011/10/13 18:22:59  dechavez
 * replaced reboot config parameter with boot, and save instead
 *
 * Revision 1.27  2011/04/14 21:02:14  dechavez
 * added Q330.token field (to support sta and net)
 *
 * Revision 1.26  2011/04/12 21:01:10  dechavez
 * added lcq field to Q330_CMD
 *
 * Revision 1.25  2011/02/04 20:03:46  dechavez
 * added sensor A and B complex names to config data
 *
 * Revision 1.24  2011/02/01 20:24:33  dechavez
 * added ip address to Q330_CMD config field
 *
 * Revision 1.23  2010/12/17 19:45:00  dechavez
 * support libq330 2.0.0 Q330 configuration database and structure formats
 *
 * Revision 1.22  2010/12/13 23:00:38  dechavez
 * removed defaults.h
 *
 * Revision 1.21  2010/11/15 23:06:57  dechavez
 * added flush support
 *
 * Revision 1.20  2010/04/07 19:50:37  dechavez
 * added "input" command
 *
 * Revision 1.19  2010/03/19 00:35:25  dechavez
 * added calib status
 *
 * Revision 1.18  2009/11/05 18:45:33  dechavez
 * added reboot field to config command parameters
 *
 * Revision 1.17  2009/10/29 19:36:46  dechavez
 * added config support
 *
 * Revision 1.16  2009/09/28 22:15:35  dechavez
 * added Q330_CMD_TERC
 *
 * Revision 1.15  2009/09/22 19:46:48  dechavez
 * xml command
 *
 * Revision 1.14  2009/09/15 23:15:34  dechavez
 * added support for "tokens" command
 *
 * Revision 1.13  2009/07/28 18:15:05  dechavez
 * updated prototypes
 *
 * Revision 1.12  2009/07/25 17:38:08  dechavez
 * updated prototypes, added calib field to Q330_CMD
 *
 * Revision 1.11  2009/07/23 20:31:04  dechavez
 * improved help support
 *
 * Revision 1.10  2009/07/10 20:51:43  dechavez
 * added prototype
 *
 * Revision 1.9  2009/07/10 18:34:34  dechavez
 * added id command
 *
 * Revision 1.8  2009/02/23 22:12:38  dechavez
 * added spp and gps fields to handle
 *
 * Revision 1.7  2009/02/04 17:49:56  dechavez
 * support for spp, dcp, man, gps cnf, gps id, save (re)boot
 *
 * Revision 1.6  2009/01/24 00:13:42  dechavez
 * support for amass, pulse, and sc commands
 *
 * Revision 1.5  2009/01/05 22:49:06  dechavez
 * added save command
 *
 * Revision 1.4  2008/10/02 23:02:46  dechavez
 * updated prototypes, added new command codes
 *
 * Revision 1.3  2007/12/14 22:09:58  dechavez
 * 0.0.2 checkpoint
 *
 * Revision 1.2  2007/10/31 17:55:37  dechavez
 * checkpoint, gps, reboot, resync added
 *
 * Revision 1.1  2007/10/16 22:01:07  dechavez
 * checkpoint
 *
 */
