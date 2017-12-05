#pragma ident "$Id: ReleaseNotes.c,v 1.3 2011/11/09 20:07:02 dechavez Exp $"

char *VersionIdentString = "Release 1.1.1";

/* Release notes

1.1.1  11/09/2011
       Link with libida10 2.14.2 to split too long station names into sta+net

1.1.0  10/28/2011
       Rewrote to accomodate libida 4.4.1 and libida10 2.14.1 which populate
       a LISS_MSEED_HDR structure instead of writing packed header directly.
       Header update now done using libliss 1.3.0 lissPackMseedHdr()

1.0.0  10/24/2011
       initial release
*/
