#pragma ident "$Id: ReleaseNotes.c,v 1.26 2014/08/28 21:47:06 dechavez Exp $"

char *VersionIdentString = "Release 2.3.3";

/* Release notes

2.3.3  05/07/2018
       link with libdbio 3.5.1 to ignore missing deprecated files

2.3.2  08/28/2014
       link with libisidl 1.25.0 (ISI_DL_OTIONS moved into ISI_DL handle), and support for same

2.3.1  05/11/2013
       added support for type "addoss"

2.3.0  03/04/2013
       added support for type "sbd"

2.2.0  10/31/2011
       link with libisi 2.12.1, which includes MSEED header (hdrlen now is 156)

2.1.0  01/25/2011
       IDA10_MAXDATALEN increased to 16320, added type=femto option

2.0.0  11/04/2010
       Link with libisidl 1.15.0 to use "smart" ISI_DL_SYS (w/ hdrlen)

1.8.2  01/26/2010
       added support for type "reftek"

1.8.1  11/12/2009
       Fixed "isidlCloseDiskLoop: LockDiskLoop: utilReadLockWait: Invalid argument" error

1.8.0  01/26/2009
       Don't create QDP state file, fixed annoying "No such file" message
       when creating new QDP loop w/o pre-existing metadir

1.7.3  02/20/2008
       Fixed bug where type was required even if explicit sizes were given

1.7.2  10/05/2007
        Link with libdbio 3.2.6, libdbio 1.3.2 and libisidl 1.10.1 for
        support for NRTS_HOME environment variable

1.7.1  09/07/2007
       Create QDP state file

1.7.0  05/03/2007
       Added LISS defaults (type=liss)

1.6.2  04/18/2007
       Note state file, if present

1.6.1  01/11/2007
       Use isidlX function names

1.6.0  12/12/2006
       Added generation of metadata subdirectory for QDP disk loops

1.5.0  06/02/2006
       Added support for QDPLUS packets, and now require type= arg
       on the command line to specify what type of disk loop to build

1.4.1  03/15/2006
       Replaced ini=file command line option with db=spec for global init

1.4.0  02/08/2006
       link with libida 4.0.0, isidb 1.0.0 and neighbors

1.3.1  09/14/2005
       don't print any warning messages related to bad or missing NRTS disk loop

1.3.0  07/25/2005
       added ISI_GLOB support, open after to create to check NRTS status

1.2.0  06/29/2005
       added time stamps to disk loop

1.1.0  06/24/2005
       build new version of disk loop, with "hide" parameter

1.0.1  06/02/2005
       added perm argument to isiOpenDiskLoop call

1.0.0  05/24/2005 (cvs rtag isimkdl_1_0_0 isimkdl)
       Initial release

*/
