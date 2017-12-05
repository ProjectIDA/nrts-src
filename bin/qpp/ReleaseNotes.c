#pragma ident "$Id: ReleaseNotes.c,v 1.38 2017/09/06 22:20:14 dechavez Exp $"

char *VersionIdentString = "Release 3.5.2";

/* Release notes

3.5.2  09/06/2017
       link with libqdp 3.15.0 to get decimal degree support for coordinates

3.5.1  01/30/2017
       link with libqdp 3.14.1 to support reading meta-data files with both old and new-style tags

3.5.0  01/19/2017
       added -nosechk option to suppress pre-emptive tossing of packets with 1-sec overlap
       (use that to generate ida10 packets with proper leap second behavior)

3.4.2  01/22/2016
       added "epd" print support (but won't get invoked until state machine is
       updated to include requesting QDP_TYPE_C2_EPD as part of the handshake

3.4.1  01/19/2016
       Link with 3.9.0 to introduce environmental processor support

3.4.0  12/08/2015
       Clean Mac OS X build

3.3.2  01/27/2014
       remove 'strict' option from HLP decoder (so channels will unpack even if tokens are missing)

3.3.1  01/13/2013
       Don't give up so easily when encountering I/O errors

3.3.0  04/07/2011
       Added print supprt for QDPLUS_PAROSCI packets

3.2.4  01/25/2011
       Link with libqio 0.1.0

3.2.3  01/11/2011
       Bumped version number to reflect linkage with libqdp2.4.0/libslp0.0.0

3.2.2  11/02/2010
       Link with libqdp 2.2.3 to fix seg fault when encountering C1_STAT packets with UMSG

3.2.1  03/31/2010
       Don't unpack streams which lack tokens (no option to override), flush partially
       completed IDA10 packets on program end

3.2.0  10/29/2009
       Added "ccd" (configuration change data) print option, include event bits in event report

3.1.1  07/13/2009
       Link with libqdp 1.7.1 to avoid choking on garbage GPS data in C1_STATUS

3.1.0  07/10/2009
       Automatically, and silently, accept enn and ene data in place of en1 and en2
       when running detector.

3.0.0  07/09/2009
       Added event detection support for tee option

2.7.1  07/02/2009
       Print packet counts, even when aborting

2.7.0  07/02/2009
       Transparent support of gzip compressed input

2.6.1  07/02/2009
       Report early termination due to errors in packet stream

2.6.0  06/24/2009
       Added split option
       Quit if corrupt data (serial number = 0) are encountered

2.5.1  06/19/2009
       Link with libqdp 1.5.3 to fix little-endian QDP decompression bug

2.5.0  02/26/2009
       Added tee option

2.4.0  02/23/2009
       Replaced -v with more elaborate print flags
       
2.3.0  10/07/2008
       Added ida10 option

2.2.1  03/05/2007
       Relink with libqdp 1.2.0 for CNP316 (aux packet) support

2.2.0  12/20/2007
       Decode C1_STAT packets

2.1.4  06/28/2007
       Fixed decompression error (libqdp 1.0.2), improved help message

2.1.3  05/11/2007
       Relink with libqdp with compiled root time tag instead of macro

2.1.2  01/31/2007
       Relink with libqdp 0.9.14 to fix decompression problem

2.1.1  12/21/2006
       Changes to accomodate reworked structure fields

2.1.0  12/13/2006
       Added meta= option for specifying metadata directory.  Load and use
       metadata to preinit the logical channel queues, if available.

2.0.0  12/06/2006
       Use QDPLUS logical channel queues, much print stuff moved off to library

1.0.2  11/13/2006
       Added hex dumps of compressed DT_DATA blockettes

1.0.1  10/13/2006
       Preliminary DT_DATA support

1.0.0  07/06/2006
       Initial release

*/
