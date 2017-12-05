#pragma ident "$Id: help.c,v 1.25 2014/08/28 22:00:44 dechavez Exp $"
/*======================================================================
 *
 *  Help message
 *
 *====================================================================*/
#include "isidl.h"

void help(char *myname)
{
    fprintf(stderr, "\n");
    fprintf(stderr, "usage: %s { PrimaryInput } [options] site{+site...}\n", myname);
    fprintf(stderr, "\n");
    fprintf(stderr, "Required Primary Input Specifier:\n");
    fprintf(stderr, "      isi=[site@]host:port => ISI/IACP from a remote ISI disk loop\n");
    fprintf(stderr, "liss=server:port:blksiz:to => MiniSEED packets via LISS\n");
    fprintf(stderr, "                  cfg=path => alternate path for config file (use BEFORE q330 args)\n");
    fprintf(stderr, "    q330=name:port[:debug] => Quanterra Q330 input (may be repeated)\n");
    fprintf(stderr, "\n");
    fprintf(stderr, "Options:\n");
    fprintf(stderr, "        db=spec => set database to `spec'\n");
    fprintf(stderr, "       trec=int => override records/tee file parameter\n");
    fprintf(stderr, "        to=secs => set I/O timeout interval\n");
    fprintf(stderr, "       log=name => set log file name\n");
    fprintf(stderr, "    maxdur=secs => force exit after 'secs' seconds\n");
    fprintf(stderr, "seedlink=cfgstr => set SeedLink configuration (where cfgstr is srv:port:len:depth:net)\n");
    fprintf(stderr, "    -noseedlink => disable SeedLink support\n");
    fprintf(stderr, " slinkdebug=int => set SeedLink debug level\n");
    fprintf(stderr, "            -bd => run in the background\n");
    fprintf(stderr, "\n");
    fprintf(stderr, "These additional options are available when requesting ");
    fprintf(stderr, "data from an ISI server:\n");
    fprintf(stderr, "     beg=string => begin seqno (or 'oldest' or 'newest')\n");
    fprintf(stderr, "     end=string => end seqno (or 'newest' or 'never')\n");
    fprintf(stderr, "     scl=string => stream control list specifier ('nrts' for NRTS, else path name)\n");
    fprintf(stderr, "     -intercept => enable intercept processor\n");
    fprintf(stderr, "\n");
    exit(1);
}

/* Revision History
 *
 * $Log: help.c,v $
 * Revision 1.25  2014/08/28 22:00:44  dechavez
 * removed messages about deprecated options, prettied up the formatting
 *
 * Revision 1.24  2014/08/19 18:13:32  dechavez
 * added slinkdebug option
 *
 * Revision 1.23  2013/01/16 23:03:40  dechavez
 * Enabled SeedLink support by default, added -noseedlink option to suppress
 * BUD option removed
 *
 * Revision 1.22  2013/01/14 22:39:16  dechavez
 * SeedLink support introduced
 *
 * Revision 1.21  2012/12/13 21:53:56  dechavez
 * added support for changing station and channel names
 *
 * Revision 1.20  2011/12/26 20:56:18  dechavez
 * added maxdur option
 *
 * Revision 1.19  2011/11/07 17:47:52  dechavez
 * added bud= option
 *
 * Revision 1.18  2011/10/19 16:21:07  dechavez
 * changed static 91011bug to variable daybug to accomodate new behavior in these crazy clocks
 *
 * Revision 1.17  2011/10/13 18:06:58  dechavez
 * added -91011bug option to help message
 *
 * Revision 1.16  2011/08/04 20:52:19  dechavez
 * added net option to help message
 *
 * Revision 1.15  2011/01/25 21:31:04  dechavez
 * added trec override option
 *
 * Revision 1.14  2010/03/22 21:51:21  dechavez
 * eliminated instance number from q330 specification
 *
 * Revision 1.13  2008/03/05 23:24:41  dechavez
 * added -intercept and -leap options
 *
 * Revision 1.12  2008/01/25 22:01:27  dechavez
 * added scl option
 *
 * Revision 1.11  2007/09/07 20:07:01  dechavez
 * use Q330 cfg file to specify digitizers by name
 *
 * Revision 1.10  2007/05/03 20:27:57  dechavez
 * added LISS support
 *
 * Revision 1.9  2007/03/28 17:37:50  dechavez
 * added RT593 option
 *
 * Revision 1.8  2006/06/23 18:31:19  dechavez
 * Added client side port parameter to Q330 argument list
 *
 * Revision 1.7  2006/06/19 19:16:39  dechavez
 * conditional Q330 support
 *
 * Revision 1.6  2006/06/02 21:05:47  dechavez
 * added Q330 help
 *
 * Revision 1.5  2006/03/30 22:05:32  dechavez
 * barometer support
 *
 * Revision 1.4  2006/03/13 23:09:19  dechavez
 * Replaced ini=file command line option with db=spec for global init
 *
 * Revision 1.3  2005/07/26 00:49:05  dechavez
 * initial release
 *
 */
