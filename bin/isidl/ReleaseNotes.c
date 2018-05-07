#pragma ident "$Id: ReleaseNotes.c,v 1.157 2017/12/21 00:04:21 dechavez Exp $"

char *VersionIdentString = "Release 4.12.2";

/* Release Notes

4.12.2  05/07/2018
        link with libdbio 3.5.1 to ignore missing deprecated files in ~nrts/etc

4.12.1  02/02/2018
        Fresh build as 4.12.0 seemed to have some library version skew resulting in a core
        dump when isidlCloseDiskLoop() was invoking isidlFeedSeedLink() but which doesn't happen
        when I build it now, bumping version number so I'll know its this one

4.12.0  12/20/2017
        link with libisidl 1.26.0 for IDA10.12 NRTS and Seedlink support (untested)

4.11.6  02/03/2017
        link with libqdp 3.14.3 to get useful error messages about invalid action codes

4.11.5  02/01/2017
        link with libqdp 3.14.2 to fix problem with QEP time stamps

4.11.4  01/26/2017
        link with libq330 3.7.6 to address re-introducing the double free bug in 3.7.5!

4.11.3  12/22/2016
        link with libq330 3.7.4 to address double free bug with bad config files

4.11.2  11/10/2016
		link with libmseed 2.6.2 to use Seedlink idents that do not use qc code

4.11.1  9/23/2016
        link with libqdp 3.13.6 to fix bug associated with C1_LOG timeout during handshake

4.11.0  8/26/2016
        isidl.h: replaced tstamp with a structure that tracks time since most recent
                 DT_DATA, C1_STAT and "other" packets were received
        introduced ToggleQ330DebugState() and SIGHUP handler to invoke same
        track time since DT_DATA, C1_STAT and "other" packets were received, and
        restart machine when there is a DT_DATA timeout

4.10.1b added LOG_DEBUG commands to main q330 loop to see what that 3pm hang is all about
4.10.1d broke timestamp into dt_data, c1_stat, and others, reset after dt_data timeout exceeded
        added SIGHUP handler to invoke ToggleQ330DebugState()

4.10.1 08/19/2016
       link with libqdp 3.13.3

4.10.0 08/04/2016
       link with libqdp 3.13.1 (redesigned automaton)
       reworked main Q330 loop to accomodate libqdp 3.13.1 changes

4.9.0  02/12/2016
       Striped out all the code related to old barometer, ars, and pre-IDA10 packet support.  Glup.

4.8.2  02/03/2016
       Link with libqdp 3.11.0 to handle connections to digitizers which lack EP supoort

4.8.1  01/27/2016
       Link with libqdp 3.10.1 to address bugs related to C2_EPD meta-data

4.8.0  01/26/2016
       link with libqdp 3.10.0 for environmental processor packet support

4.7.1  12/19/2015
       changed syslog facility from LOG_USER to LOG_LOCAL0

4.7.0  12/09/2015
       Clean Mac OS X build

4.6.1  10/29/2014
       Link with libmseed 2.2.0 to address core dumps coming from uncompressible solar channels
       The uncompressible solar channels are a side effect of the version skew in the firmware
       beteween the TS-45 and TS-60 charge controllers.

4.6.0  08/28/2014
       Rework to support moving of ISI_DL_OPTIONS into ISI_DL handle, and use
       of WritePacketToDisk() for all disk loop output. Starting to remove
       barometer, ars, and pre-IDA10 packet support.  Freezing code now
       before the next phase where all that code is actually excised.

4.5.2  08/26/2014
       Link with libmseed 2.1.2 to address bugs in detecting time tears

4.5.1  08/19/2014
       Link with libmseed 2.1.0 to address multiple SeedLink issues
       Added -slinkdebug option

4.5.0  08/11/2014
       Link with libmseed 2.0.0 to address Seedlink issues that arose in 4.4.0,
       and with libisidl 1.24.0 to take care of core dumps when bad packets are
       encountered when exporting to SeedLink.

4.4.0  04/29/2014
       Link with libisidl 1.23.0 and libnrts 4.16.5 for SeedLink status bits and clock quality support

4.3.0  01/30/2014
       Link with libnrts-4.16.0 to address byte order confusion with 16-bit data on little endian hosts
       Link with libqdp-3.6.0+ to include reworked Q330 state machines for serial input and hopefully not mess up UDP side

4.2.1  07/19/2013
       Link with libida10 2.18.0 to include IDA10.11 support, link with
       libisidl 1.22.2 to address multi-client bug in isidlPacketServer()

4.2.0  05/14/2013
       Link with libida10 2.17.1 to include IDA10.10 support

4.1.0  03/08/2013
       Link with libisidl 1.22.1 to flush partial MiniSEED packets on exit

4.0.2  02/07/2013
       Link with libisidl 1.21.2 to address memory leak

4.0.1  02/07/2013
       Link with libisidl 1.21.1 to address memory leak

4.0.0  02/07/2013
       Link with libisidl 1.21.0 to use new buffering of SeedLink data
       (bumped major version to 4 just so it would match qhlp)

3.2.0  02/05/2013
       MAJOR BUG FIX - corrects timing problem with SeedLink data

3.1.3  01/23/2013
       Link with libisidl 1.20.3 to fix unitialized msr bug

3.1.2  01/22/2013
       Link with libisidl 1.20.1 to fix memory leak with SeedLink interface

3.1.1  01/18/2013
       MK7 byte order problem with SeedLink tee corrected (via libnrts 4.15.0)

3.1.0  01/16/2013
       Enabled SeedLink support by default, added -noseedlink option to suppress

3.0.0  01/14/2013
       SeedLink support introduced

2.8.0  12/13/2012
       added option to change incoming station and channel names on IDA10 ISI feeds

2.7.6  08/01/2012
       -nosync effect gets propagated to NRTS writes as well as ISI
       (via libnrts 4.13.1)

2.7.5  07/30/2012
       added -nosync option for asynchronous ISI disk loop writes

2.7.4  12/26/2011
       added maxdur option to force exit after specified time interval

2.7.3  12/15/2011
       Fixed bug with setting offset to tstamp in packet header in ProcessLocalData()

2.7.2  11/09/2011
       Link with libliss 1.3.6 to pad short SEED/FSDH names with spaces

2.7.1  11/09/2011
       Link with libisi 2.14.1 to split long station names into sta+net for SEED

2.7.0  11/07/2011
       BUD support

2.6.5  11/02/2011
       Populate ISI_RAW_HEADER mseed field (not tested)

2.6.0  10/31/2011
       Support for ISI_RAW_HEADER mseed field

2.5.2  10/18/2011
       changed -91011bug to daybug=value (now that the 9/10/11 bug is looking variable)

2.5.1  10/14/2011
       actually added support for 9/10/11 bug (2.5.0 only set flags but
       neglected to do anything with those flags... duh).

2.5.0  10/13/2011
       added support for 9/10/11 GPS clock bug (ARS systems)
       use libisidl isidlPacketServer() for servicing remote packets

2.4.0  08/04/2011
       Added net=port option to accept packets pushed from remote sources

2.3.2  04/25/2011
       Added comp=none option

2.3.1  04/22/2011
       Fixed bug setting end sequence number, and with terminating connection
       on EOF on non-continuous feeds

2.3.0  04/07/2011
       Modified QDP barometer support to conform with libqdplus 1.4.0.  Works
       OK except for some occasional "stutters" in time stamp and small spikes.
       Further work still needed before deploying.

2.2.1  02/03/2011
       link with libqdp 3.1.0 to fix auto-restart of Q330 connections

2.2.0  01/25/2011
       IDA10_MAXDATALEN increased to 16320, trec command line override option

2.1.7  01/25/2011
       Updated to use QDP_CONNECT and link with libqio 0.1.0

2.1.6  01/11/2011
       Bumped version number to reflect linkage with libqdp2.4.0/libslp0.0.0

2.1.5  12/23/2010
       link with libqdp 2.3.4 to fix the horrible bugs in 2.3.3

2.1.4  12/22/2010
       link with libqdp 2.3.3, use qdpDataPortNumber() to assign data port parameter

2.1.3  12/20/2010
       use q330PrintErrcode() for more informative q330ReadCfg() failures

2.1.2  12/17/2010
       support libq330 2.0.0 Q330 configuration database and structure formats

2.1.1  12/06/2010
       Added watchdog restarts of Q330 I/O in the event of data timeouts
       (using hardcoded retry interval and timeout threshold paramters)

2.1.0  11/24/2010
       Support new Q330 config file with explicit sensor assignments

2.0.0  11/10/2010
       Link with libisidl 1.15.1 to use "smart" ISI_DL_SYS (w/ hdrlen)

1.20.7  10/29/2010
        Fixed bug in isi backend that allowed in packets with sequence numbers
        smaller than what were already in the disk loop

1.20.6  09/30/2010
        link with libq330 1.5.0 to recognize q330.cfg files that include the
        e300 calibration parameter

1.20.5  07/23/2010
        link with libisi 2.7.1 to correct problem with uncompressed ISI input

1.20.4  06/23/2010
        asynchronous MSYNC (MS_ASYNC)
        (ultra-paranoid syncing introduced in 1.19.4 causes performance
        problems in hub systems)

1.20.3  04/02/2010
        Skip the deregistration requirement and instead use predicatable
        return ports (requires libisi 2.1.0).  That way start/stop is much
        more likely to not result in "port busy" errors as a result of too
        quick kills at reboot.

1.20.2  04/01/2010
        Clean up to avoid race conditions and ensure Q330 degregistration on exit

1.20.1  03/22/2010
        Link with libqdp 2.0.0 to remove potential client side port clashes

1.20.0  03/22/2010
        Eliminated instance number from q330 specification, abort startup if any
        q330s are found busy during handsake

1.19.10 02/02/2010
        link with libida 4.3.3 to fix new year bug for ARS based systems

1.19.9  11/19/2009
        Recognize (and ignore) LISS heartbeats support

1.19.8  10/02/2009
        Link with libqdp 1.8.0, fixes connect to busy Q330 retry bug

1.19.7  07/27/2009
        using q330GetX instead of q330LookupX (libq3301.1.0)

1.19.6  06/19/2009
        Link with libqdp 1.5.3 to fix little-endian QDP decompression bug

1.19.5  05/14/2009
        Changed missed barometer sample from -2 to 0xFFFFFFFF, changed paroOpen() to
        conform with libparo 1.2.1

1.19.4  12/15/2008
        Link with libisidl 1.11.1 in include sync'd mmaps and disk loop writes

1.19.3  12/09/2008
        Link with libqdp 1.3.1 to eliminate "out of window" messages

1.19.2  10/10/2008
        Link with libida 4.9.2 to eliminate the "no chndx" messages

1.19.1  03/06/2008
        link with libida 4.3.1 to correct bug in ARS leap year bug patch

1.19.0  03/05/2008
        Added support for Q330 aux packets (via libqdp 1.2.0)
        Added optional "intercept" processing (-intercept) and
        ARS leap year bug check (-leap)

1.18.3  02/08/2008
        Link with libnrts 4.8.6 to correct IDA10.5 sample data byte order
        problem on little endian systems.  Add SIGPIPE handler for Slate builds.

1.18.2  02/03/2008
        Link with libliss 1.2.2 to correct bug where non-data packets were
        not making it into the disk loop

1.18.1  02/02/2008
        Fixed incorrect name of exit handler for ARM_SLATE builds

1.18.0  01/25/2008
        Add support for scl (stream control list).  Link with libnrts 4.8.4
        to include smooth LISS time of next sample updates to eliminate
        spurious 1 sample blocking errors due to clock drift (when building
        IDA10 from MiniSEED)

1.17.4  01/14/2008
        Suppress signal handling thread for those platforms that have problems
        with sigwait receiving signals sent to the controlling process (ie, Slate)

1.17.3  01/08/2008
        Link with libliss 1.2.1 to address bus errors on Solaris builds when
        trying to convert ISI supplied LISS packets to NRTS

1.17.2  01/08/2008
        Link with libnrts 4.8.1 to fix spurious failures with liss conversion

1.17.1  01/07/2008
        MiniSEED to IDA10 conversion into library so that ISI feeds from
        LISS sites can also drive NRTS disk loops

1.17.0  12/21/2007
        Added NRTS disk loop support for MiniSEED packets from LISS feeds
        via repackaging as IDA10.5 packets (original data still available
        via ISI disk loop).

1.16.3  12/20/2007
        Fixed bug where 16-bit streams in RT593 equipped ARS systems were
        getting flagged as Ida9 instead of IDA9.

1.16.2  10/31/2007
        Lookup Q330_ADDR instead of serialno and authcode, link with various
        updated libraries that included strlcpy instead of blanket memcpy's
        (fixes some non-Solaris core dumps)

1.16.1  10/05/2007
        Link with libdbio 3.2.6, libdbio 1.3.2 and libisidl 1.10.1 for
        support for NRTS_HOME environment variable

1.16.0  09/14/2007
        Link with libnrts 4.7.0 and libisidl 1.10.0 for changes in overlap
        definition and response

1.15.0  09/07/2007
        Use Q330 config file via cfg option to specify digitizer addresses

1.14.3  06/23/2007
        Added -lax option to ignore suspicious bit

1.14.2  06/01/2007
        link with libnrts 4.6.2 to fix fixed spurious (zero sample) gaps

1.14.1  06/01/2007
        link with libnrts 4.6.1 to fix NRTS bwd nseg at startup error

1.14.0  05/03/2007
        Added LISS support

1.13.1  04/18/2007
        Added -gsras option to set NRTS DL flags for default wfdisc
        managment in Obninsk

1.13.0  03/28/2007
        RT593 option verified.

1.12.2x 03/26/2007
        Added RT593 option.  Remains to be tested, so do NOT deploy
        1.12.2x at sites equipped with RT593 digitizer boards.

1.12.2  02/19/2007
        Set log tag for single station isi sites to site instead of server.
        Fixed "reconnect at beginning" bug for isi feeds with specific beg seqno.

1.12.1  02/08/2007
        CompleteIDA10Header calls ida10InsertSeqno32().
        NRTS_MAXCHN increased to 128. LOG_ERR and LOG_WARN to LOG_INFO.
        libnrts4.4.0 for improved wfdisc support and IDA10 packet gaps fixed
        Local packet support via LOCALPKT handle contaning dl, mutex and pkt

1.12.0  12/12/2006
        Use QDP library callback for transparent handling of metadata

1.11.1  12/06/2006
        Preliminary 10.4 support (underlying library not fully tested).
        Use the streamlined QDPLUS_PKT

1.11.0  11/13/2006
        Updated 10.3 support, NRTS_MAXCHN increased to 64, added various
        -dbgxxx command line options

1.10.0  09/29/2006
        Default to 64Kbyte TCP I/O buffers

1.9.2c  08/18/2006
        Preliminary 10.3 support (underlying library not fully tested)
        Ignore IACP signatures

1.9.2   07/07/2006
        Better IDA5/6 support, link with libisidl 1.6.6 to copy lock tee
        files under Windows.

1.9.1   07/07/2006
        Link with libnrts 4.1.2 for fix allow streams absent in local
        NRTS disk loop to still be considered for inclusion in downstream
        systems.

1.9.0   06/30/2006
        Replaced message queues for handling locally acquired data with static
        buffers, eliminating the randcom core dumps on Q330 reconnect problem.

1.8.7   06/23/2006
        Added client side port parameter to Q330 argument list

1.8.6   06/20/2006
        Fixed Win32 build, link with libisi2.4.1

1.8.5   06/19/2006
        Proper handling of IDA 5 packets, link with libnrts4.1.0 for smart
        nrtsReads and more efficient bwd construction at initialization.

1.8.4   06/14/2006
        Fixed bogus failure in Q330 init code

1.8.3   06/12/2006
        Link with libisidl 1.6.3 for more rational synchronized mmap I/O

1.8.2   06/07/2006
        This time really fixed the uppercase ARS station names in IDA9 packets.

1.8.1   06/07/2006
        Fixed some QDP barometer bugs.  Unresolved crashes in Q330/barometer test.

1.8.0   06/02/2006
        Initial Q330/QDP support
        Link with libisi 1.6.2 for synchronized memory mapped updates

1.7.3   04/20/2006
        Fix bug allowing multiple instances to "lock" a single disk loop

1.7.2   04/17/2006
        Relink with libnrts 4.0.2 to eliminate bogus locks after system crash

1.7.1   04/07/2006
        Fixed initialization problem with systems that have no barometers

1.7.0   04/07/2006
        Link with libisidl 1.6.0 (remove gzip tee file support)

1.6.0   04/03/2006
        Added Paroscientific barometer support.  Redesigned ARS input to allow
        the inclusion of one or more barometer streams into an ARS system, or
        simply act as a standalone barometer station.  Change the design for
        station systems to use a message queue for generating new packets, and
        added starter hooks for eventual inclusion of an arbitrary number of Q330s.

1.5.0   03/13/2006
        Link with libisidl 1.5.0 (O_SYNC on write) !!!
        Replaced ini=file command line option with db=spec for global init
        Bracket ISI_DL access with mutexes to prevent race condition in shutdown
        from closing active disk loops (messy, probably needs to be addressed in
        the handle).

1.4.0   02/08/2006
        link with libida 4.0.0, libisidb 1.0.0 and neighbors (MySQL support)

1.3.0   12/20/2005
        Link with updated ida and nrts libraries that attempt to handle year
        transitions better, spurious year increments in particular

1.2.9   12/14/2005
        Fixed bug that clobbered ISI sequence numbers in ARS feeds, introduced
        in 1.2.8

1.2.8   12/09/2005
        Fixed duplicate packet problem from ARS feeds, fixed off by 1000 bug in
        serial I/O timeout

1.2.7   10/18/2005
        Link with libnrts 3.1.7, trying to keep bad time stamps out of NRTS disk loop

1.2.6   10/17/2005
        Link with liblogio 2.1.1, fixing LOG_ERR and LOG_WARN filtering problem

1.2.5   10/11/2005
        link with repaired isiClose()
        allow comma delimiter in tty spec (eg, "/dev/term/d5001,19200")

1.2.4   10/09/2005
        Fixed infinite loop bug in data request phase of reconnect to ISI server

1.2.3   10/06/2005
        Fixed bug clobbering common header of non-DATA records when reading tty data.

1.2.2   09/30/2005
        Fixed msec vs sec confusion in specifying timeout (user specifies seconds)
        Toggle IACP packet dumps on/off with SIGHUP

1.2.1   09/15/2005
        Fixed core dump on server disconnect bug

1.2.0   09/13/2005
        Reworked to use explicit disk I/O for ISI disk loop operations instead of
        mapped memory assignments.
        Changed default log to syslogd for daemon runs, stdout for foreground.
        Fixed bug causing core dump from ReadFromISI() when server disconnected.
        Include build date/time in version string output
        Added dbgpath option.
        Abandon ISI reconnects in case of server fault and other critical errors,
        otherwise sleep for a progressively increasing longer interval between attempts.

1.1.1   09/06/2004
        fixed bug in verifying IDA9 time offsets between packets

1.1.0   09/05/2005
        Convert tty input to ISI_TYPE_IDA9
        Changed default log spec to syslogd:user

1.0.1   07/27/2005
        Fixed problem with foff's in wfdiscs, better logging verbosity

1.0.0   07/25/2005
        Initial release
 */
