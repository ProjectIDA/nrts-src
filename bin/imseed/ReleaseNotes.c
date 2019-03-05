#pragma ident "$Id: ReleaseNotes.c,v 1.18 2018/01/18 23:34:20 dechavez Exp $"

char *VersionIdentString = "Release 1.8.1";

/* Release notes

1.8.1  03/02/2019 dauerbach
       Link with libmseed 2.9.1 with mseed.h fix. Records now flushed when clock qual changes

1.8.0  01/18/2018
       Link with libida10 2.25.0 for updated IDA10.12 definition

1.7.0  11/02/2017
       Pass thru support for IDA10.12 (encapsulated Miniseed, payload emitted directly without unpacking)

1.6.1  02/01/2016
       Give up immediately on any read errors

1.6.0  12/05/2015
       OS X compatibility - link with a passel of libraries newly updated to keep DARWIN compiles quiet

1.5.6  05/21/2015
       Added tests for bad reads and sane recovery from same

1.5.5  10/29/2014
       Link with libmseed 2.2.0 to fix problem with uncompressible data

1.5.4  09/02/2014
       Fixed bug setting netID

1.5.3  08/26/2014
       link with imseed 2.1.2 to flush MiniSEED records if time tear exeeds 1/2 sample interval

1.5.2  08/26/2014
       link with libmseed 2.1.1 to address
         a) sample rate bug when using drift option
         b) incorrect packing of non-Steim data
         c) problems detecting time tears

1.5.1  08/xx/2014
       Reworked to use libmseed 2.1.0

1.5.0  08/05/2014
       Reworked to use libmseed 2.0.0

1.3.4  05/06/2014
       Fixed bug that was causing incorrect sample rates to be generated

1.3.3  04/30/2014
       Fixed segfault that would happen when converting INT16 data to INT32

1.3.2  04/25/2014
       Re-link with limbseed after modifications there-in to ensure that blockette 1000
       appears first in the output records

1.3.1  04/18/2014
       Included rechan option description on help message, suppressed the (unused) option of
       specifying clock thresholds (locked=percent suspect=percent)

1.3.0  04/14/2014
       added "rechan" option for renaming channels
       Added "drift" option to set blockette 100 actual sample rate (MK8 and Femtometer packets only)
       Fixed spurious bit 5 in data quality flag bug for Q330 input

1.2.0  never committed

1.1.1  03/06/2014
       Rework to accomodate move of template managment over to library

1.1.0  03/04/2014
       FSDH flags and Blocketee 1001 timing quality support added

1.0.0  02/05/2014
       initial release
*/
