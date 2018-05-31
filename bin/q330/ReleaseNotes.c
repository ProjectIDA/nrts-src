char *VersionIdentString = "Release 2.11.4 beta";

/* Release notes

2.11.4  05/25/2018
        link with libqdpxml 1.4.2 to fix bug in parsing optional token LCQ parameters

2.11.3  01/08/2018
        link with libqdp 3.17.1 to fix bug loading XML files with control detector entries

2.11.2  10/11/2017
        Print "library standard" message when reporting on EP firmware incompatibility

2.11.1  9/28/2017
        added version checks for EP commands
        link with libqdp x.x.x to suppress envproc update when QEP not supported by firmware

2.11.0  09/13/2017
        added preamp command

2.10.8  09/06/2017
        link with libqdp 3.15.0 to better handle GPS strings

2.10.7  01/27/2017
        link with libq330 3.7.6 to address re-introducing the double free bug in 3.7.5!

2.10.6  01/24/2017
        link with libq330 3.7.5 to address bugs introduced in libq330 3.7.4

2.10.5  12/22/2016
        link with libq330 3.7.4 to address double free bug with bad config files

2.10.4  09/23/2016
        link with libqdp 3.13.6

2.10.3  09/07/2016
        link with libqdp 3.13.5

2.10.2  08/19/2016
        link with libqdp 3.13.3

2.10.1  08/04/2016
        link with libqdp 3.13.1 and libq330 3.7.1

2.10.0  07/10/2016
       replaced maxtry=integer option with watchdog=secs
       link with libqdp 3.13.0 to address handshake failure modes not previously encountered
       and dealt with in a half-assed manner in release 1.2.1

2.9.0   06/23/2016
        Added support for debug options 3 and 4 (raw packet logging).
        Link with libqdp-3.12.0/1 to fix incorrect auth code handling, added combo command

2.8.3   02/11/2016
        Changed maxsrq to maxtry, link with lib330 3.6.10 and libqdp 3.11.3 to add
        graceful failures when auth code is bad

2.8.2   02/05/2016
        Added maxsrq option, link with libq330 3.6.9 and libqdp 3.11.2 for maxsrq support

2.8.1   02/03/2016
        Restored help command which got accidently removed from previous build

2.8.0   02/03/2016
        Link with libqdp 3.11.0 to handle connections to digitizers which lack EP supoort

2.7.1   01/22/2016
        Include tokens in "ep cfg" output, added "ep delay" command

2.7.0   01/21/2016
        Environmental processor support

2.6.1   01/19/2016
        Introduced environmental processor status support

2.6.0   12/05/2015
        OS X compatibility - link with a passel of libraries newly updated to keep DARWIN compiles quiet

2.5.8   09/24/2014
        Added additional sanity tests for possible "q330 set" input errors and either
        fail or fix as appropriate or possible.

2.5.7   09/08/2014
        Improved messages when "q330 set" runs into problems

2.5.6   08/11/2014
        Minor changes to function names

2.5.5   01/23/2014
        Allow "reboot" == "boot" and "save reboot" == "save boot".
        Added support for STS-5A sensor control lines

2.5.4   03/01/2013
        Fixed bug that caused custom calibs to think there was an E300 present

2.5.3   02/25/2013
        Fixed bug that caused core dump with "q330 help sc"

2.5.2   08/31/2012
        Suppress printing of warning when new configuration is not automatically saved
        (since the -save automatic save option often results in "memory operation in progress" errors).

2.5.1   08/06/2012
        Added "baler44" support to XML files (via libqdpxml 1.3.0)

2.5.0   06/24/2012
        Use libqdp 3.4.0 QDP_TYPE_FULL_CONFIG new dport field where data port and tokens are bound
        together with new dport, and where main digitizer board output is forced to agree with
        token assignments.

2.4.0   01/17/2012
        Print clock quality percentage (global status) and gave prefixes to both "Input Voltage"
        reports to make them unique strings for grepping convenience (all via libqdp 3.3.0)

2.3.1   12/20/2011
        Fixed bug with setting station name during config

2.3.0   10/13/2011
        Introduced raceful exits in the event of errors, to avoid hanging the config port.
        Configurations no longer automatically saved and booted.  Must be explictly done
        via -save and -boot config options, or as separate commands.

2.2.1   04/15/2011
        Link with libq330 3.2.2 to fix seg fault on non-existant tokenset (tokens command)

2.2.0   04/12/2011
        Added tokenset option to tokens command, to permit configuring tokens on
        a specific data port using token sets defined in the lcq database.
        Added "lcq" command to list the lcq database.

2.1.2   02/14/2011
        fixed toggled locked/unlocked message errors for Sensor A control

        Link with libqio 1.1.0 to fix some problems with serial I/O on Suns
2.1.1   02/07/2011

2.1.0   02/04/2011
        Added config options to override sensor A and B control maps

2.0.0   02/01/2011
        Serial console support.  Config command extended to support overriding of
        ethernet IP address if using a console connection.

1.10.10 01/26/2011
        Fixed 'set' command

1.10.9 01/25/2011
       Updated to use QDP_CONNECT and link with libqio 0.1.0
       'set' command does not work!

1.10.8 01/11/2011
       Bumped version number to reflect linkage with libqdp2.4.0/libslp0.0.0

1.10.7 01/07/2011
       Don't allow q330 set to insert unrecognized sensors into q330.cfg
       Changed command to list dbfiles from "q330 cfg dbfile" to just "q330 dbfile"

1.10.6 01/07/2011
       complain when using set if tag number lacks matching serial number

1.10.5 01/03/2011
       moved some input bitmap related stuff to libqdp as qdpInputMuxCode(), qdpInputMuxString2()

1.10.4 12/22/2010
       fixed overflow bug introduced in 1.10.3 via use of QDP_MAX_PORT together
       with the general symbolic port names in qdp.h.

1.10.3 12/22/2010
       link with libqdp 2.3.3 to fix bug with reading config for XML dump,
       use qdpDataPortNumber() to assign data port parameter

1.10.2 12/20/2010
       Improved(?) header of q330.cfg output via "q330 cfg".  Removed unused
       resistors from Q330_INPUT.

1.10.1 12/20/2010
       Use q330PrintErrcode() for more informative q330ReadCfg() failures.
       Extend the "q330 set" command to support updates to just q330.cfg or to
       both the q330.cfg and auth files

1.10.0 12/17/2010
       support libq330 2.0.0 Q330 configuration database and structure formats
       Set sensor control lines to match Q330 config database when programming EEPROM

1.9.6 12/13/2010
      Moved some code and constants to libq330 (1.7.0).

1.9.5 11/24/2010
      Sensor control map consistency checks and "sc auto" command
      Slightly more useful error messages when config file fails to load

1.9.0 09/28/2010
      Automatically enable E300 calibration relays when applicable

1.8.5 08/18/2010
      Added status to "help gps".

1.8.4  06/08/2010
       fixed swapped locked/unlock control line mapping for Guralp sensors

1.8.3  04/12/2010
       added "off" and "abort" aliases for "calib stop"

1.8.2  04/07/2010
       added input command

1.8.1  03/31/2010
       link with libqdp 2.0.0 to remove problem of potential client side port conflicts
       Moved UpdateBitmap to qdp library as qdpUpdateSensorControlBitmap(), ensure that
       QDP_TYPE_C1_STAT structures are properly initialized

1.8.0  03/22/2010
       Abort startup if q330 configuration port is already in use

1.7.1  03/18/2010
       Check for active calibration before attempting start, added calib status command.

1.7.0  03/11/2010
       Support for calibration monitor spec in q330 addr.
       Fixed fall through bug when unable to load configuration.

1.6.5  02/22/2010
       Fixed internal/external mixup with gps cnf [internal | external] command

1.6.4  11/14/2009
       q330 token updates in config command fixed (libqdp 1.9.4).  Still ignoring web page due
       to inconsistent performance.  Don't override token station name if it is TST.

1.6.3  11/05/2009 (checkpoint)
       q330cc.h: added reboot field to config command parameters
       q330 config command mostly working, with improved help message, and options to override
       site info and to automatically save and reboot

1.6.2  10/21/2009
       Removed k1 support, prematurely added

1.6.1  10/20/2009
       Added serial sensor status support, link with libqdpxml 1.0.0 and libmyxml 1.1.5
       Added trillium and k1 sensor control line support

1.6.0  10/02/2009
       Production release with xml command.

1.5.5  09/28/2009
       Checkpoint build with "xml" command.  Still need to add CRCs and some other fields

1.5.4  09/14/2009
       Added "tokens" command

1.5.3  09/04/2009
       link with libqdp 1.7.2 to fix bug in reporting GPS and global status timestamps

1.5.2  07/27/2009
       Calibration shortcut support.  Now require either "now" or an absolute
       start time to be specified.

1.5.1  07/24/2009
       Added "cfg" and "set" commands

1.5.0  07/23/2009
       Improved help
       Added "list" command
       Fixed byte order problem setting IP addresses in ifconfig

1.4.5  07/10/2009
       Added "id" command

1.4.4  07/09/2009
       Fixed calibration command to set calibrate enable lines and to
       set start time such that settling time is accomodated

1.4.3  06/23/2009
       Fixed initialization bug in checkout option

1.4.2  03/12/2009
       Fixed bug in reporting calibration amplitude

1.4.1  02/23/2009
       Permit setting GPS parameters via internal/external option to
       gps cnf, and setting of operational limits (either default or
       specific values)

1.4.0  02/04/2009
       Added "checkout", "spp", "dcp", and "man" commands, and "gps"
       subcommands "id" and "cnf".  Added "reboot" option to "save"
       command.
       Use new libqdp qdp_C1_RQx commands to retrieve info from Q330..

1.3.1  01/26/2009
       Fixed bug setting pulse duration, changed default pulse width to
       one second, fixed bug specifying sensor b recentering, changed
       flag for recenter line from mass to mp. Added glob command.

1.3.0  01/23/2009
       Added amass, pulse, and sc commands

1.2.0  01/05/2009
       Extended ifconfig command to allow setting IP addresses
       Added save command for saving current configuration to eeprom

1.1.2  10/03/2008
       Allow specifying an arbitrary number of status selectors

1.1.1  10/03/2008
       Added gsv command, removed satellites from gps selection

1.1.0  10/02/2008
       Added ifconfig, status, and fixvar commands

1.0.2  03/08/2008
       Changed default calibration duration to hardware max (16383 sec)
       and silenty ensure that custom durations do not exceed this

1.0.1  01/05/2008
       Fixed bug reporting calibration frequency

1.0.0  12/20/2007
       Initial release
*/

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
