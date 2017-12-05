#pragma ident "$Id: ReleaseNotes.c,v 1.40 2017/04/14 19:37:25 dechavez Exp $"

char *VersionIdentString = "Release 2.3.5";

/* Release Notes

2.3.5   04/13/2017
        link with libqdp 3.14.4 to fix "too many servers" error after attempting to connect to a disabled data port

2.3.4   01/26/2017
        link with libq330 3.7.6 to address re-introducing the double free bug in 3.7.5! 

2.3.3   01/24/2017
        link with libq330 3.7.5 to address bugs introduced in libq330 3.7.4

2.3.2   12/22/2016
        link with libq330 3.7.4 to address double free bug with bad config files

2.3.1   09/23/2016
        link with libqdp 3.13.6

2.3.0   09/01/2016
        extensive rework, conforms to libqdp 3.13.4, libq330 3.7.3

2.2.0   08/04/2016
        rework to conform to libqdp 3.13.1 and libq330 3.7.1

2.1.1   06/23/2016
        Reordered exit handling steps to ensure data files get closed, added some logging to
        help debug hangs.  Deregister with no wait option.

2.1.0   04/20/2016
        Include a "B" in the file names for sensor B calibrations

2.0.1   02/05/2016
        Fixed problem with the root= option

2.0.0   12/03/2015
        Generate MiniSEED data by default, IDA10 and QDP optionally.  Clean Max OS X support.

1.8.0   10/21/2014
        "ping" e300 at 1-minute intervals, if present.

1.7.4   06/12/2014
        Replaced -strict with -lax.  Now the default is back to strict again.
        Also, no longer save QDP data unless -qdp option is used.

1.7.3   05/08/2014
        made the "strict" option to hlp decoder an option, with the default enabled

1.7.2   01/27/2014
        removed "strict" option to hlp decoder so channels will unpack even if tokens are missing

1.7.1   06/19/2013
        Fixed bug where sensor B calibrations were getting sensor A parameters
        Allow cal=A and cal=B rather than bitmaps

1.7.0   06/24/2012
        Modified to use libqdp 3.4.0 where data port and tokens are bound 
        together and main channel output frequencies are forced to agree with token set

1.6.3   01/11/2012
        Added wdsecs option for setting watchdog exit delay.  Fixed what
        gets printed in help for default port

1.6.2   12/20/2011
        Removed -V from help message

1.6.1   12/15/2011
        Added -V (verify) option

1.6.0   04/15/2011
        Added cal and mon options to set calibrate and monitor channels

1.5.1   04/15/2011
        Link with libq330 3.2.2 to fix seg fault on non-existant tokenset

1.5.0   04/14/2011
        Added digitizer options to override output data port tokens

1.4.0   02/03/2011
        Link with libqdp 3.1.0 to handle auto connects better

1.3.11  01/25/2011
        Updated to use QDP_CONNECT and link with libqio 0.1.0

1.3.10  01/11/2011
        Bumped version number to reflect linkage with libqdp2.4.0/libslp0.0.0

1.3.9   12/23/2010
        link with libqdp 2.3.4 to fix the horrible bugs in 2.3.3

1.3.8   12/22/2010
        link with libqdp 2.3.3, use qdpDataPortNumber() to assign data port parameter

1.3.7   12/20/2010
        use q330PrintErrcode() for more informative q330ReadCfg() failures

1.3.6   12/17/2010
        Support libq330 2.0.0 Q330 configuration database and structure formats

1.3.5   11/24/2010
        Link with libq330 1.6.0 to recognize new Q330 config file format
        Change default file prefix to CAL-digitizer-sensor-caltag-date

1.3.0   11/15/2010
        Flush data port before sending calibration start command

1.2.1   11/02/2010
        Link with libqdp 2.2.3 to fix seg fault when encountering C1_STAT packets with UMSG

1.2.0   09/30/2010
        Automatically control external calibration lines for E300 based systems

1.1.4   08/25/2010
        Fixed startup problem for selected SunFire builds

1.1.3   04/26/2010
        abort calibration upon receipt of termination signals

1.1.2   04/12/2010
        removed some console verbosity and moved things around in a failed effort
        to cleanly background the whole affair

1.1.1   04/05/2010
        added watchdog thread to avoid hanging exits

1.1.0   04/01/2010
        initial reasonably working release

1.0.0   03/31/2010
        initial release
 */
