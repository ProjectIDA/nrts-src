#pragma ident "$Id: ReleaseNotes.c,v 1.77 2017/01/26 19:03:24 dechavez Exp $"

char *VersionIdentString = "Release 4.5.7";

/* Release notes

4.5.7  05/07/2018
      link with libdbio 3.5.1 to ignore missing deprecated files in ~nrts/etc

4.5.6  01/26/2016
       link with libq330 3.7.6 to address re-introducing the double free bug in 3.7.5!

4.5.5  01/24/2016
       link with libq330 3.7.5 to address bugs introduced in libq330 3.7.4

4.5.4  12/22/2016
       link with libq330 3.7.4 to address double free bug with bad config files

4.5.3  11/10/2016
       link with libmseed 2.6.2 to use Seedlink idents that do not use qc code

4.5.2  02/03/2016
       Link with libqdp 3.11.1 to handle packets from diverse digitizers, some with
       EP support and some without

4.5.1  01/27/2016
       Link with libqdp 3.10.1 to address bugs related to C2_EPD meta-data

4.5.0  01/26/2016
       Link with libqdp 3.10.0 for first full support of environmental processor data (all streams, with filter delays)

4.4.3  01/22/2016
       Link with libqdp 3.9.2 for compressed environmental processor stream support (still need to add delays)

4.4.2  01/19/2016
       Link with libqdp 3.9.0 for initial environmental processor support (1-Hz streams only)

4.4.1  12/19/2015
       changed syslog facility from LOG_USER to LOG_LOCAL0

4.4.0  12/07/2015
       Clean Mac OS X build

4.3.0  08/28/2014
       Rework to support moving of ISI_DL_OPTIONS into ISI_DL handle, but the
       main change is use of WritePacketToDisk() for all disk loop output and
       hopefully address the occasional packet clobber with data coming in via
       the "net" option.

4.2.1  08/26/2014
       Link with libmseed 2.1.2 to address SeedLink time tear detection error

4.2.0  08/19/2014
       Remove QDP_LCQ_IS_DT_USER support (we never used this).
       Added slinkdebug option
       Link with libmseed 2.1.0 to address multiple SeedLink issues

4.1.1  07/19/2013
       Link with libisidl 1.22.2 to address multi-client bug in isidlPacketServer()

4.1.0  03/08/2012
       Link with libisidl 1.22.1 to flush partial MiniSEED packets on exit

4.0.2  02/07/2013
       Link with libisidl 1.21.2 to address memory leak (this is getting old)

4.0.1  02/07/2013
       Link with libisidl 1.21.1 to address memory leak

4.0.0  02/06/2013
       Link with libqdp 3.6.0 to correct 1-sec QDP overlaps on the fly.
       Link with libisidl 1.21.0 to use new buffering of SeedLink data

3.2.0  02/05/2013
       MAJOR BUG FIX - corrects timing problem with SeedLink data

3.1.3  01/22/2013
       Link with libisidl 1.20.3 to fix unitialized msr bug
       Bumped version number one extra to match isidl, that way I can do the
       global update without having to think too much

3.1.1  01/22/2013
       Link with libisidl 1.20.1 to fix memory leak with SeedLink interface

3.1.0  01/16/2013
       Enabled SeedLink support by default, added -noseedlink option to suppress

3.0.0  01/14/2013
       SeedLink support introduced

2.6.2  11/09/2011
       Link with libliss 1.3.6 to pad short SEED/FSDH names with spaces

2.6.1  11/09/2011
       Link with libisi 2.14.1 to split long station names into sta+net for SEED

2.6.0  11/07/2011
       BUD support

2.5.5  11/02/2011
       Populate ISI_RAW_HEADER mseed field

2.5.0  10/31/2011
       Support for ISI_RAW_HEADER mseed field

2.4.0  10/12/2011
       Added net=port option for accepting IDA10 packets from a remote source

2.3.0  04/07/2011
       Convert DT_USER packets into IDA10 barometer packets

2.2.0  01/25/2011
       IDA10_MAXDATALEN increased to 16320

2.1.4  01/25/2011
       Link with libqio 0.1.0

2.1.3  01/11/2011
       Bumped version number to reflect linkage with libqdp2.4.0/libslp0.0.0

2.1.2  12/20/2010
       use q330PrintErrcode() for more informative q330ReadCfg() failures

2.1.1  12/17/2010
       Link with libq330 2.0.0 to support new Q330 configuration database

2.1.0  11/24/2010
       Link with libq330 1.6.0 to recognize new Q330 config file format

2.0.0  11/10/2010
       Link with libisidl 1.15.1 to use "smart" ISI_DL_SYS (w/ hdrlen)

1.8.3  11/02/2010
       Link with libqdp 2.2.3 to fix seg fault when encountering C1_STAT packets with UMSG

1.8.2  05/10/2010
       Fixed bug where a manual trigger would terminate if an actual event came
       along and triggered/detriggered (ie, skip the event detector if a manual
       trigger is in progress).

1.8.1  03/21/2010
       Removed all special handling of calibration monitor data

1.8.0  03/11/2010
       Decoupled unpacking of calibration monitor channel from event trigger

1.7.6  10/02/2009
       Link with libqdplus 1.3.1 that fixes(?) bug which could cause init failure
      if the meta directory contains extraneous files

1.7.5  07/27/2009
       using q330GetX instead of q330LookupX (libq3301.1.0)

1.7.4  07/25/2009
       Added -lax option

1.7.3  07/13/2009
       Link with libqdp 1.7.1 to avoid choking on garbage GPS data in C1_STATUS

1.7.2  07/09/2009
       Changed trigger for calibration events to use the new lcq cal_in_progress
       field, eliminating the reliance on c1_stat packets in the data stream.
       Cleaned up the logic flow of the trigger state switch.

1.7.1  07/08/2009
       Fixed multiple Q330 automatic calibration detrigger bug, verify CRC

1.7.0  07/02/2009
       Automatic calibration "triggers"

1.6.2  07/01/2009
       Graceful handling of misconfigured detector channel list

1.6.1  06/19/2009
       Link with libqdp 1.5.3 to fix little-endian QDP decompression bug

1.6.0  05/14/2009
       Include sequence number in main() event warning messages
       Added support for /usr/nrts/etc/debug toggling

1.5.0  01/26/2009
       Link with libisidl 1.11.2 to remove isidlSnapshot() memory leak, added
       liblogio 2.4.1 SIGHUP handler for watchdog logging.

1.4.2  01/15/2009
       added logioUpdateWatchdog() tracers

1.4.1  12/16/2008
       Fixed empty qhlp state file bug

1.4.0  12/14/2008
       Handle corrupt state files gracefully.
       Link with libisidl x.x.x in include sync'd mmaps and disk loop writes

1.3.6  10/10/2008
       Link with libida 4.9.2 to eliminate the "no chndx" messages

1.3.5  03/05/2008
       CNP316 (aux packet) support

1.3.4  01/07/2008
       *** little-endian support ***

1.3.3  12/20/2007
       Link with libq330 1.0.5 to accept calib entries in the cfg file
       (not yet using this information)

1.3.2  12/14/2007
       Link wth libqdp 1.0.5 to correct signed/unsigned datum bug

1.3.1  09/25/2007
       Continually update pre-event memory (even during detections), link with
       libdetect 1.0.2 to permit immediate triggers following post event dump

1.3.0  09/25/2007
       Rework trigger framework into a more general "process" path which currently
       just runs the event detector.  The event detector has been generalized into
       an opaque structure managed by libdetect.  All detector parameters are
       specfied via the q330.cfg file.  Linked with libdetect 1.0.0 which includes
       a simple STA/LTA trigger.

1.2.0  09/14/2007
       Fixed problem saving/recovering state on shutdown/startup
       Set ISI dl options to reject overlapping packets from ISI disk loop
       Fixed problem with NRTS disk loop getting plugged on Q330 restarts
       Enabled station system checks to detect short packets and keep them out of NRTS
       Added framework for event detector and implemented manual trigger
       Added -dbgpkt, -dbgttag, -dbgbwd, -dbgdl, -dbglock and -notrig command line options

1.1.0  09/07/2007
       Set user id to "nrts" or that which is specified via user= on the command line
       Specify Q330 digitizer parameters via name (look up in cfg= config file)

1.0.2  06.26.2007
       Fixed missing state file bug.
       Set "station system" flag for NRTS disk loop so that strict packet checks
       would detect short packets (and reject them from NRTS disk loop).

1.0.1  06.26.2007
       Fixed fixed decompression error (libqdp 1.0.2)

1.0.0  06/14/2007
       Initial production release

*/
