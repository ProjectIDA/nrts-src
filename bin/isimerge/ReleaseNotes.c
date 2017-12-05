#pragma ident "$Id: ReleaseNotes.c,v 1.7 2015/12/18 18:52:50 dechavez Exp $"

char *VersionIdentString = "Release 1.3.2";

/* Release Notes

1.3.2  12/19/2015
       changed syslog facility from LOG_USER to LOG_LOCAL0

1.3.1  11/09/2011
       Accomodate change of isidlWriteToDiskLoop options field from bitmask to structure

1.3.0  01/25/2011
       IDA10_MAXDATALEN increased to 16320

1.2.0  11/10/2010
       Link with libisidl 1.15.1 to use "smart" ISI_DL_SYS (w/ hdrlen)

1.1.1   09/17/2010
        Default log to syslogd:user, don't require isi= prefix for site@server.
        This program does not work when invoked with the -bd option.  I can't figure out why.
        What happens is the threads all seem to hang.  I've tried reordering things and it
        doesn't help.  It works OK if I use the shell (&) to background it, but not if I fork().
        This is the second time this has happened and I'm real confused, since I've got many
        other multi-threaded daemon apps running which do just fine.  I need to get this
        deployed, so I'm releasing as is, but leaving this note to myself for when (and if)
        I revisit this later.
    
1.1.0   09/09/2010
        Set approrpriate default log for foreground/background

1.0.0   09/09/2010
        initial release

 */
