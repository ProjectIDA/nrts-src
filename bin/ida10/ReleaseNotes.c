#pragma ident "$Id: ReleaseNotes.c,v 1.21 2018/01/18 23:32:20 dechavez Exp $"

char *VersionIdentString = "Release 1.15.0";

/* Release notes

1.15.0 01/18/2018
       Link with libida10 2.25.0 for updated IDA10.12 definition

1.14.0 10/19/2017
       Link with libida10 2.24.0 for IDA format 10.12 support
       main.c:   have internal rawDump call utilPrintHexDump()
       Makefile: update Makefile lib dependencies

1.13.0 12/08/2015
       clean DARWIN build

1.12.6 06/17/2015
       link with libida10 2.23.1 for improved clock quality in -v option

1.12.5 07/30/2013
       link with libida10 2.18.1 for IDA10.11 support

1.12.0 05/14/2013
       link with libida10 2.17.1 for IDA10.10 support

1.11.1 01/11/2012
       -t option for femtometer data now includes human readable tofs, host tstamp, and seqno

1.11.0 10/19/2011
       link with libda10 2.13.1 to include full IDA10.8 support

1.10.0 02/25/2011
       Introduced -hdr option for reading header only files

1.9.0  02/22/2011
       Accept both uncompressed and gzip compressed input

1.8.0  01/25/2011
       IDA10_MAXDATALEN increased to 16320

1.7.0  02/10/2010
       Link with libida10 2.9.0 for origseqno support in IDA_EXTRA

1.6.2  03/11/2010
       Link with libida10 2.8.0 that understands 10.4 src bytes, but no change in output

1.6.1  04/03/2008
       Fixed core dump on -t in the presence of IDA10.5 (-t still not
       supported for 10.5 packets, they just get ignored)

1.6.0  04/02/2008
       IDA10.5 support (raw and -v options only)

1.5.2  03/06/2006
       Added -h (help) option.

1.5.1  01/04/2006
       Add multiple digitizer/same stream name support to time tear checker

1.5.0  12/21/2006
       IDA10.4 support

1.4.4  11/13/2006
       Added -t (time tag) and -noseqno options, moved print fuctions
       to ida10 library

1.4.3  08/14/2006
       Relink with libida10 2.5.0 for 10.3 (OFIS) support

1.4.2  10/06/2005
       Detect and correct for faulty nbytes in 10.2 CF records.
       Terse listing includes raw packet format

1.4.1  09/30/2005
       Added -v for full output, default to common header only

1.4.0  08/25/2005
       IDA10.2 support

1.3.0  12/09/2004 (cvs rtag ida10_1_3_0 ida10)
       added ttag=path option for dumping time tag information

1.2.1  04/29/2002 (cvs rtag ida10_1_2_1 ida10)
       use new ida10TStoString to generate TS summary

1.2.0  03/15/2002 (cvs rtag ida10_1_2_0 ida10)
       support IDA10.x (and hence transparent buffer lengths)

1.1.0  09/06/2001 (cvs rtag ida10_1_1_0bis ida10)
       allow user to specify alternate buffer lengths

1.0.0  04/02/2001 (cvs rtag ida10_1_0_0 ida10)
       created
*/
