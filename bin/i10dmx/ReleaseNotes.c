#pragma ident "$Id: ReleaseNotes.c,v 1.43 2018/01/18 23:33:20 dechavez Exp $"

char *VersionIdentString = "Release 3.2.0";

/*

i10dmx Release Notes

3.2.0  01/18/2018
       Link with libida10 2.25.0 for updated IDA10.12 definition

3.1.0  10/19/2017
       linked with libida10 2.24.0 for IDA format 10.12 support

3.0.2  01/26/2017
       link with libq330 3.7.6 to address re-introducing the double free bug in 3.7.5!

3.0.1  12/22/2016
       link with libq330 3.7.4 to address double free bug with bad config files

3.0.0  12/05/2015
       OS X compatibility - link with a passel of libraries newly updated to keep DARWIN compiles quiet

2.9.3  09/18/2014
       Fixed bug causing extra wfdisc records to be created

2.9.2  08/29/2014
       Split records if time tears exceed 0.5 samples

2.9.1  08/26/2014
       Split records if time stamp increments aren't perfect

2.9.0  12/12/2013
       Added maxdur option for forcing shorter wfdisc records

2.8.3  12/02/2013
       Link with libcssio 2.2.3 to compute endtimes using double precision arithmitic

2.8.2  11/01/2013
       Link with libcssio 2.2.2 to print out san wfid and chnid for 64-bit linux

2.8.1  07/10/2013
       IDA10.11 support, plus don't start new wfdisc record if time stamp increment
       errors are less than one sample

2.8.0  05/11/2013
       IDA10.10 support, plus don't start new wfdisc record if time stamp increment
       errors are less than one sample

2.7.0  10/19/2011
       link with libda10 2.13.1 to include full IDA10.8 support

2.6.1  03/17/2011
       support change in return value of ida10ReadGz (from error code to nbytes, libida10 2.12.0)

2.6.0  02/24/2011
       Added capacity to automatically detect and read gzip compressed data

2.5.0  01/25/2011
       IDA10_MAXDATALEN increased to 16320

2.4.3  01/11/2011
       Bumped version number to reflect linkage with libqdp2.4.0/libslp0.0.0

2.4.2  12/20/2010
       use q330PrintErrcode() for more informative q330ReadCfg() failures

2.4.1  12/17/2010
       support libq330 2.0.0 Q330 configuration database and structure formats

2.4.0  11/24/2010
       support new Q330 config file with explicit sensor assignments (for detect option)

2.3.2  07/27/2009
       Link with lib3301.1.0 and using q330GetX instead of q330LookupX

2.3.1  08/20/2008
       Force new wfdisc record each time clock first sees a valid time string (this
       addresses the problem of bogus 1999 stamped data following femtometer reboots).

2.3.0  04/02/2008
       IDA10.5 support

2.2.6  09/26/2007
       Fixed shared detector channel mixing bug
       Fixed spurious time tears bug when multi-station file has duplicate channel names
       on different stations.

2.2.5  09/25/2007
       Added detect option for testing Q330 detector code

2.2.2  01/11/2007
       Relink with updated isidb libraries

2.2.1  01/04/2007
       Use new incerr header struct to decide to neglect a time tear or not

2.2.0  12/21/2006
       Initial 10.4 support.  Warn about missing database.
       Don't generate new wfdisc records if errors are less than one sample

2.1.2  11/13/2006
       Final 10.3 support, use ida10 library print functions for diagnostic options

2.1.1  08/17/2006
       further 10.3 support

2.1.0  08/14/2006
       IDA 10.3 (OFIS) support

2.0.0  02/08/2006
       libisidb database support

1.5.2  10/11/2005
       Generate descriptor file

1.5.1  08/18/2005
       IDA 10.2 support

1.5.0  05/13/2005 (cvs rtag i10dmx_1_5_0 i10dmx)
       Rework to use BufferedStream I/O

1.4.2  10/22/2004 (cvs rtag i10dmx_1_4_2 i10dmx)
       Don't abort on incomprehensible records

1.4.1  11/21/2003 (cvs rtag i10dmx_1_4_1 i10dmx)
       Removed Sigfunc casts

1.4.0  10/16/2003 (cvs rtag i10dmx_1_4_0 i10dmx)
       Added (untested) support for types other than INT32

1.3.4  05/13/2003 (cvs rtag i10dmx_1_3_4 i10dmx)
       Replace spaces in station and channel names with blanks.
       Fixed help message to remove reference to abandoned buflen argument (since 1.3.0)

1.3.3  05/05/2003 (cvs rtag i10dmx_1_3_3 i10dmx)
       Increased IDA10_MAXSTREAMS to 128

1.3.2  05/15/2002 (cvs rtag i10dmx_1_3_2 i10dmx)
       Changed numeric decomposition of ttag status to interpreted
       yes/no strings for easier reading, compute and print offset
       change regardless of validity of time stamp

1.3.1  05/15/2002 (cvs rtag i10dmx_1_3_1 i10dmx)
       Print record number, stream name, offset difference and comments
       when printing time tags

1.3.0  03/15/2002 (cvs rtag i10dmx_1_3_0 i10dmx)
       Use ida10ReadRecord() to support all IDA10.x formats, and
       hence transparent variable buffer lengths

1.2.2  01/25/2002 (cvs rtag i10dmx_1_2_2 i10dmx)
       Tolerate stream sample interval changes.
       Recognize, but ignore, CF records.

1.2.1  12/20/2001 (cvs rtag i10dmx_1_2_1 i10dmx)
       Fixed error printing size of data gaps in samples.

1.2.0  09/08/2001 (cvs rtag i10dmx_1_2_0 i10dmx)
       Support arbitrary (fixed) length records via buflen= argument

1.1.1  11/06/2000 (cvs rtag i10dmx_1_1_1 i10dmx)
       Added support for ISP (I~I) log records.

1.1.0  02/17/2000 (cvs rtag i10dmx_1_1_0 i10dmx)
       Initial release.

*/
