#pragma ident "$Id: ReleaseNotes.c,v 1.7 2015/07/17 18:59:34 dechavez Exp $"

char *VersionIdentString = "Release 1.2.3";

/* Release notes

1.2.3  07/17/2015
       Presort ADDOSS frames into time order in order to reduce artificial time
       tears introduced when missed packets come in out of order. Also rework to
       support inclusion of options in ISI_DL handle and user supplied write function.
       Checking this in after two years!

1.2.2  07/19/2013
       Apply filter delay correction in IDA10 output (libaddoss 1.1.0, libida10 2.18.0)
       Improved logging

1.2.1  05/14/2013
       Fixed 24-bit sample decoder and ADDOSS channel (libaddoss 1.0.0)

1.2.0  05/11/2013
       Initial ADDOSS/IDA10 integration

1.1.1  03/15/2013
       include checks, and log messages, for all possible SBD IEs
   
1.1.0  03/13/2013
       Link with libsbd 1.2.0 to include support for all defined IEs, rework to
       accomodate new SBD_MESSAGE layout.

1.0.0  03/11/2013
       Initial release

*/
