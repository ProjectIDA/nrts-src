#pragma ident "$Id: ReleaseNotes.c,v 1.9 2017/04/14 19:38:19 dechavez Exp $"

char *VersionIdentString = "Release 1.1.2";

/* Release notes

1.1.2 03/02/2017
      Fresh build with Antelope 2.6 libraries

1.1.1 12/02/2015
      Link with libfilter 1.2.6 so that things won't fall apart uneccesarily if there
      are stage iunit entries that don't have corresponding matches in the hardcoded
      lookup table that library uses.  Anything missing from that lookup table isn't
      needed downstream.

1.1.0 9/30/2015
      initial production release
*/
