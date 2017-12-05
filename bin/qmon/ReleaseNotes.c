#pragma ident "$Id: ReleaseNotes.c,v 1.48 2017/09/20 19:27:57 dauerbach Exp $"

char *VersionIdentString = "Release 2.7.0";

/* Release Notes

2.7.0   09/20/2017
        Introduced 'preamp' command to enable/disable preamp for sensor 'a' or 'b'

2.6.0   09/06/2017
        introduced 'D' command to toggle between GPS string and decimal degree views for lat/lon

2.5.8   01/25/2017
        link with libq330 3.7.6 to address re-introducing the double free bug in 3.7.5! 

2.5.7   01/24/2017
        link with libq330 3.7.5 to address bugs introduced in libq330 3.7.4

2.5.6   12/22/2016
        link with libq330 3.7.4 to address double free bug with bad config files

2.5.5   09/23/2015
        link with libqdp 3.14.6

2.5.4   09/07/2015
        link with libqdp 3.14.5

2.5.3   08/26/2016
        Use qdpShutdown() instead of qdpDeregister() when breaking connection... fixes
        "busy server" problems with repeated runs of qmon 2.5.2

2.5.2   08/19/2016
        link with libqdp 3.13.3

2.5.1   08/04/2016
        link with libqdp 3.13.1 and libq330 3.7.1

2.5.0   07/20/2016
        libqdp 3.13.0 compatibility changes (basically adding regerr to q330Register() calls)

2.4.6   02/03/2016
        Link with libqdp 3.11.0 to handle connections to digitizers which lack EP supoort

2.4.5   01/28/2016
        Include EP1 status (if available)

2.4.0   12/23/2015
        Restored serial console support that somehow got lost somewhere along
        the way, added useful help message when q330.cfg tagno lacks a corresponding
        q330/auth entry, don't fail if the system disk loops can't be accessed
        (ie, by a user other than nrts)

2.3.0   12/07/2015
        Mac OS X support

2.2.8   09/08/2014
        Print error message and fail when configuration is incomplete (ie, no internal serial number found)

2.2.7   08/11/2014
        Added -h option for help

2.2.6   04/18/2014
        added "save boot" command

2.2.5   01/23/2014
        Added support for STS-5A sensor control lines

2.2.0   01/24/2013
        Added 'P' command to toggle displaying percent resent vs count resent

2.1.0   07/27/2011
        Added "isi" and "nrts" commands to monitor disk loops.
        Added undocumented "forever" command to disable auto-timeout.
        Auto-clear pop-ups after 10 seconds.
        Don't set save/reboot flags when changing input bitmap and instead print
        a reminder that settings will auto-clear in 30 minutes.

2.0.1   02/14/2011
        Fixed toggled locked/unlocked message errors for Sensor A control

2.0.0   02/01/2011
        serial console support added

1.4.5   01/25/2011
        Updated to use QDP_CONNECT and link with libqio 0.1.0

1.4.4   01/11/2011
        Bumped version number to reflect linkage with libqdp2.4.0/libslp0.0.0

1.4.3   01/07/2011
        Added "set" command to set operational limits
        Changed help message to just list qmon display commands and added
        "q330" or "help q330" to list Q330 commands
        Improved behavior when qmon run with no entries in q330.cfg file

1.4.2   01/03/2011
        Added 'input' command

1.4.1   12/27/2010
        Added "other" and "none" to list of sensors in sc help

1.4.0   12/27/2010
        Switched to left/right arrorws for shifting active digitizer(s), as CR
        was causing inadvertent shifts.  Changed sc command to print list of
        recognized complex input names, not just sensors.

1.3.3   12/23/2010
        fixed bug introduced in 1.3.2 causing digitizer commands to fail

1.3.2   12/20/2010
        use q330PrintErrcode() for more informative q330ReadCfg() failures

1.3.1   12/17/2010
        support libq330 2.0.0 Q330 configuration database and structure formats

1.3.0   12/13/2010
        Q330 command support: sc gps gsv pulse calib save boot flush resync

1.2.3   12/08/2010
        Added help pop up (and stubs for later adding Q330 command support)

1.2.2   12/06/2010
        Leave old info on screen when state has transitioned to unconnected

1.2.1   12/02/2010
        Fixed up clearing bottom window when toggling from single to dual mode,
        restored printing connection state messages for unconnected digitizers
        (which got lost in 1.1.4).
   
1.2.0   11/24/2010
        Use new config file with explicit sensor assignments to reverse video
        inconsisent sensor control maps.
        Reverse video calibration stations when anything other than "off".
        Display "Baro: n/a" when no barometer data are available.
        Display qmon version ident

1.1.5   11/16/2010
        Fixed "flashing" effect visible on sites with slow tail circuits

1.1.4   10/25/2010
        Retain tag line data on shutdown, move shutdown message to command window

1.1.3   10/24/2010
        Clear bottom window when toggling from dual to single unit display

1.1.2   10/24/2010
        Fixed lat instead of lon display bug

1.1.1   10/22/2010
        Added TAB (add/remove digitizer from display), ENTER (advance display),
        and + (add specific digitizer), and ESC (quit) commands.

1.1.0   10/20/2010
        added barometer and clock qual reports,  allow user to
        specify start up digitizer(s) on command line, enable toggling of
        debug ouput via explcit log file specification on command line.
        First production release.

1.0.1   10/19/2010
        minor bug fixes and asthetic improvements

1.0.0   10/19/2010
        initial release

 */
