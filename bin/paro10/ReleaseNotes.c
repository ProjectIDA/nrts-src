#pragma ident "$Id: ReleaseNotes.c,v 1.9 2015/12/22 21:41:49 dechavez Exp $"

char *VersionIdentString = "Release 1.2.5";

/* Release Notes

1.2.5  12/22/2015
       Link with libutil 3.20.2 to correct weird FreeBSD 10.1 serial I/O bug

1.2.4  12/19/2015
       changed syslog facility from LOG_USER to LOG_LOCAL0

1.2.3  12/07/2015
       Clean Mac OS X builds

1.2.2  08/28/2014
       Log seriall number in response to "pkill -USR2 paro10"

1.2.1  08/28/2014
       Print a message the first time a packet is delivered to the server

1.2.0  12/14/2012
       Reworked time stamp again, this time to force 1-sec granularity and
       hopefully eliminate the new segment at each packet boundary problem.

1.1.0  10/31/2012
       Reworked time stamp logic to accomodate host clock going backwards

1.0.0  10/25/2012
       initial release

 */
