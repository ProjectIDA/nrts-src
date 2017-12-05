#pragma ident "$Id: ReleaseNotes.c,v 1.11 2017/01/26 19:03:38 dechavez Exp $"

char *VersionIdentString = "Release 1.2.3";

/* Release notes

1.2.3  01/26/2016
       link with libq330 3.7.6 to address re-introducing the double free bug in 3.7.5! 

1.2.2  01/24/2016
       link with libq330 3.7.5 to address bugs introduced in libq330 3.7.4

1.2.1  12/22/2016
       link with libq330 3.7.4 to address double free bug with bad config files

1.2.0  08/04/2016
       libqdp 3.13.1 and libq330 3.7.1 compatibility changes (basically adding regerr to q330Register() calls)

1.1.8  01/11/2011
       Bumped version number to reflect linkage with libqdp2.4.0/libslp0.0.0

1.1.7  12/20/2010
       use q330PrintErrcode() for more informative q330ReadCfg() failures

1.1.6  12/17/2010
       support libq330 2.0.0 Q330 configuration database and structure formats

1.1.5  11/24/2010
       Link with libq330 1.6.0 to recognize new Q330 config file format

1.1.0  03/22/2010
       abort startup if q330 is found busy during handshake

1.0.0  10/02/2009
       Initial production release

*/
