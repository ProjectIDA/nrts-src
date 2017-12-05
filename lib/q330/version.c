#pragma ident "$Id: version.c,v 1.53 2017/01/26 19:00:30 dechavez Exp $"
/*======================================================================
 * 
 * library version management
 *
 *====================================================================*/
#include "q330.h"

static VERSION version = {3, 7, 6};

/* q330 library release notes

3.7.6   01/26/2017
        cfg.c: fixed double free bug REINTRODUCED(!) in 3.7.5 (sigh)
        util.c: fixed newline type in q330PrintErrcode()

3.7.5   01/24/2017
        cfg.c: fixed sloppy bugs introduced in 3.7.4 

3.7.4   12/22/2016
        cfg.c: fixed double free bug when q330ReadCfg() fails

3.7.3   08/24/2016
        register.c: don't log qdpConnect() failures (leave that up to the caller)

3.7.2   08/15/2016
        q330.h: changed Q330_DEFAULT_WATCHDOG to 10 seconds
        cfg.c: AddAddr() sets watchdog to Q330_DEFAULT_WATCHDOG
        register.c: log qdpConnect() failure messages

3.7.1   08/04/2016
        register.c: set QDP_PAR_OPTION_QUICK_EXITS option

3.7.0   07/20/2016 (changes related to libqdp 3.13.0 compatibility)
        q330.h: replaced Q330_ADDR:maxtry with watchdog and added suberr argument to q330Register()
                prototype to reflect corresponding changes in QDP state machine
        cfg.c: replaced Q330_ADDR:maxtry with watchdog
        register.c: Removed the 3.6.12 hack prompted by pre-libqdp 3.13.0 state machines
                    Added suberr argument for compatibility libqdp 3.13.0 version of qdpConnect().
        util.c: qdpConnect() suberr argument added for libqdp 3.13.0 compatibility

3.6.12  06/23/2016
        register.c: added *temporary* hack to q330Register() to avoid automaton infinite loop
                    when attempting to register with a data port that doesn't produce data
                    (detected by checking for no buffer memory)

3.6.11  05/18/2016
        util.c: Fixed incorrect strlcpy length parameter in q330ParseComplexInputName()

3.6.10  02/11/2016
        q330.h: cfg.c:, register.c: changed maxsrq to maxtry to follow libqdp 3.11.3 names

3.6.9   02/05/2016
        q330.h: added maxsrq to Q330_ADDR
        cfg.c: added initialization for new Q330_ADDR maxsrq field in AddAddr()
        register.c: added initialization for new Q330_ADDR maxsrq field in q330Register()

3.6.8   01/04/2016
        wcfg.c: make src2 string two characters long (EP sources, for example)

3.6.7   12/05/2015
        cfg.c:  fixed spurious semicolon in AddSensor() affecting sensor.desc NULL termination
        util.c: casts, format fixes and the like, all to calm OS X compiles
        wcfg.c: casts, format fixes and the like, all to calm OS X compiles

3.6.6   09/08/2014
        cfg.c: added some stderr commentary when AddAddr() and q330ReadCfg() encounter problems
        util.c: added some stderr commentary when q330ParseComplexInputName() encounters problems

3.6.5   08/11/2014
        q330.h: updated prototypes
        cfg.c:  added q330RootPath() (to permit use of Q330_ROOT_ENV_STRING environment variable)
                and reworked q330PathName() and q330ReadCfg() to use same

3.6.0   01/30/2014
        sc.c: added STS-5A dpr (deploy/remove) support
        wcfg.c: added dpr (deploy/remove) to hdr (comments)

3.5.0   06/19/2013
        q330.h: added default calibrate and monitor bitmaps, plus q330GetCalibB() and q330LookupCalibB() prototypes
        lookup.c: added q330LookupCalibB() and q330GetCalibB()
        util.c: added q330ChannelBitmap() support for "A" and "B" strings

3.4.1   08/06/2012
        cfg.c: fixed q330DestroyCfg() return without value bug (aap)

3.4.0   06/24/2012 (requires libqdp 3.4.0 or greater (dsint, nsint in QDP_TOKEN_LCQ))
        lcq.c: q330SetTokenLcq() sets derived frate, dsint, nsint fields with qdpConvertSampleRate()

3.3.0   04/15/2011
        util.c: added q330ChannelBitmap()

3.2.2   04/15/2011
        fixed seg fault when searching for a non-existant tag in q330LookupLcq()

3.2.1   04/14/2011
        lcq.c: moved q330LcqList() to lookup.c:q330LookcupLcq(),
               added sta, net support to q330SetTokenLcq()
        lookup.c: added q330LookupLcq()

3.2.0   04/12/2011
        q330.h: added Q330_LCQ to Q330_CFG
        cfg.c: added Q330_LCQ support (does NOT check for dup entries!)
        lcq.c: created
        util.c: added Q330_DUP_LCQ, Q330_BAD_LCQ, Q330_IOE_LCQ to q330PrintErrcode()
        wcfg.c: added q330SaveLcq(), q330WriteLcq(), PrintLcqList()

3.1.1   02/07/2011
        lookup.c: used DEV_PREFIX for finding basename of device entry
           instead of utilBasename() which had all sorts of unpleasant
           side effects on the Solaris 9 builds

3.1.0   02/01/2011
        moved all these comments over to libqdp which is where they were
        supposed to have been entered.  Nothing happened to libq330 on 2/1/2011

3.0.0   02/01/2011 Q330 serial console
        cfg.c: make first entry in the addr list the console
        lookup.c: q330LookupAddr() updated to recognize device specs for console connection
        register.c: update calling addr with serial number after registration
        wcfg.c: don't include built-in console entry in q330WriteAddr()

2.1.5   01/25/2011
        q330.h: replaced name and tag fields in Q330_ADDR with QDP_CONNECT "connect"
        cfg.c: use new QDP_CONNECT "connect" field of Q330_ADDR
        lookup.c: connect.ident replaces name
        register.c: qdpSetConnect() replaces qdpSetIdent()
        wcfg.c: use ident and tag/iostr from connect field in q330PrintAddr()

2.1.4   01/13/2011
        register.c: qdpSetHost() to qdpSetIdent(), qdpConnectWithPar() to qdpConnect()

2.1.3   12/27/2010
        q330.h: added "extra" field to Q330_INPUT
        cfg.c:  complete the sensor entries in the Q330_CALIB input field
        util.c: added support for populating Q330_INPUT "extra" field in q330ParseComplexInputName()

2.1.2   12/23/2010
        register.c: use qdpValidPort() to test for input validity

2.1.1   12/21/2010
        q330.h: removed unused resistors field from Q330_INPUT
        util.c: removed unused resistors field from Q330_INPUT
        wcfg.c: changed q330.cfg header text to improve(?) description of complex sensor names

2.1.0   12/21/2010
        cfg.c: added q330PathName(), q330AddAuth().  Added Q330_IOE_x errors,
           don't make lack of auth file serial numbers a fatal error in q330ReadCfg().
        util.c: added q330PrintErrcode()
        wcfg.c: added q330PrintAuth(), cleaned up the various q330WriteX routines

2.0.0   12/17/2010
        Changed to q330 subdir for lookup tables split from q330.cfg.  Added auth
        table, allowing q330.cfg to just use property tag instead of internal serial
        number and global auth code.  Added sensor table with sensor control map,
        eliminating hard coded sensors.

1.7.0   12/13/2010
        q330.h: brought over default config values from bin/q330
        string.c: created
        util.c: added q330SensorControlBitmap()
        wcfg.c: added q330SaveCfg()

1.6.1   12/02/2010
        q330.h: added STS2.5 support
        util.c: added STS2.5 support

1.6.0   11/24/2010
        q330.h: added Q330_SENSOR to Q330_ADDR (and related stuff)
        cfg.c: added sensors to addr and calib
        lookup.c: q330LookupCalib() and q330GetCalib() use sensor name as part of search (via new addr argument)
        util.c: added q330SensorNameString(), q330SensorType(), q330ParseComplexSensorName(), q330ConfirmSensorControl()
        wcfg.c: added sensors to addr and calib

1.5.1   10/19/2010
        lookup.c: allow NULL name in q330LookupAddr(), give first address in the list

1.5.0   09/29/2010
        cfg.c: eliminated automatic "default" tag, added support for e300 calibration parameter
        wcfg.c: support for e300 calibration parameter

1.4.2   04/12/2010
        q330.h: defined Q330_CAL_x, Q330_ERROR, Q330_PORT_BUSY
        util.c: created, introducing q330CalibrationStatus()

1.4.1   04/02/2010
        q330.h: restored instance parameter to Q330_ADDR
        cfg.c: restored support for instance parameter
        It turns out that it is convenient to be able to set explicit (and predictable)
        return ports, in order to prevent spurious "server busy" errors when reconnecting
        after ungraceful program exits.  I learned this back when I was first developing
        the QDP library, but in time forgot the significance of this lesson.

1.4.0   03/31/2010
        q330.h: removed calibration port and instance parameters
        cfg.c: removed calibration port and instance parameters
        register.c: don't set client side port numbers (qdpSetMyXPort functions gone)
        wcfg.c: removed calibration port

1.3.1   03/22/2010
        cfg.c: replaced cal.src, cal.active with simple calport in Q330_ADDR
        register.c: added errcode arg required by libqdp 1.11.0
        wcfg.c: added support for printing non-default calibration ports

1.3.0   03/11/2010
        cfg.c: added support for calibration stuff in Q330_ADDR
        wcfg.c: added support for calibration stuff in Q330_ADDR

1.2.1   07/28/2009
        q330.h: corrected default calibration duration
        lookup.c: q330GetCalib returns NULL if no match for requested calibration
        wcfg.c: added q330PrintCalShortcuts()

1.1.1   07/27/2009
        lookup.c: fixed bug in q330GetDetector() when no detector is defined

1.1.0   07/25/2009
        q330.h: redfined Q330_CALIB to support an arbitrary number of sets
        cfg.c: moved lookup routines to lookup.c, reworked arrays into linked lists
        lookup.c: initial release
        wcfg.c: initial release

1.0.6   03/11/2008
        cfg.c: silently ensure calib duration does not exceed hardware max

1.0.5   12/20/2007
        cfg.c: silently ensure calib divisor does not exceed hardware max

1.0.4   12/14/2007
        cfg.c: added calib support

1.0.3   10/31/2007
        cfg.c: q330LookupAddr() changed to pass Q330_ADDR
        register.c: initial release

1.0.2   09/25/2007
        cfg.c: set errno correctly when unable to find a cfg file, fix
           q330LookupDetector() return code when lookup fails

1.0.1   09/25/2007
        cfg.c: added detector support

1.0.0   09/21/2007
        cfg.c: initial release
 */

char *q330VersionString()
{
static char string[] = "q330 library version 100.100.100 and slop";

    snprintf(string, strlen(string), "q330 library version %d.%d.%d",
        version.major, version.minor, version.patch
    );

    return string;
}

VERSION *q330Version()
{
    return &version;
}
