#pragma ident "$Id: ReleaseNotes.c,v 1.9 2014/09/11 21:54:04 dechavez Exp $"

char *VersionIdentString = "1.4.1";

/* Release notes

1.4.1  09/11/2014
       Fixed "first line bug"

1.4.0  07/07/2014
       Strip out all non-printable characters from input before processing.
       Fixed some printf format string vs data type errors.

1.3.1  04/15/2014
       added some bounds checking

1.3.0  07/09/2013
       Added -p and -q command line options

1.2.1  02/28/2012
       Include version ident in output

1.2.0  02/27/2012
       Fixed bug where final average was just the final reading(!).  Allow for GPGGA
       strings with less than 15 tokens as long as the units field is "M".

1.1.0  12/15/2011
       Don't print out reformatted readings, compute ave/stdev for all valid readings

1.0.1  12/15/2011
       Fix bug computing mean, and prevent divide by zero error

1.0.0  11/29/2011
       first release
*/
