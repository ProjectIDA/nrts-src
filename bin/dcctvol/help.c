#pragma ident "$Id: help.c,v 1.1 2011/03/17 20:23:20 dechavez Exp $"
/*======================================================================
 *
 *  dcctvol/help.c
 *
 *====================================================================*/
#include "globals.h"
#define fp(a) fprintf(stderr,a)

void help()
{

fp("\n");
fp("usage:   dcctvol infile [options]\n");
fp("\n");
/* ------------------------------------------- */
fp("Options:\n");
fp("  keep=list            => list of channels to process\n");
fp("  reject=list          => list of channels to reject\n");
fp("  use=stream           => use this stream for timing\n");
fp("  skip=n               => skip n records before beginning\n");
fp("  maxrec=n             => use up to n records from beginning\n");
fp("  discard=n1-n2,n3...  => discard records n1 through n2, n3\n");
fp("  force=n1,n2,n3...    => force new time parameter fit after recs n1,n2 and n3\n");
fp("  timcor=sec           => time correction in seconds to be ADDED to all records\n");
fp("  timcoef=filename     => file containing drift, intercept information\n");
fp("  tol=n                => allow a variance of n ticks in calculating time tolerances\n");
fp("  year=nyear           => force year to be nyear (e.g., 1997)\n");
fp("  -a                   => extract configuration from Antelope tables.\n");
fp("  -p                   => generate Postcript plot of time history.\n");
fp("  -l                   => print abbreviated log records.\n");
fp("  -h                   => print header records.\n");
fp("  -f                   => force fitting each segment formed by jump of external time.\n");
fp("\n");
/* ------------------------------------------- */
fp("      When the drift or intercept of one or more segments must be forced to a\n");
fp("      certain value, these values should be written to an ASCII file, one line\n");
fp("      for each segment to be forced.  The format of this file is:\n");
fp("             Logger_base segment_number drift intercept\n");
fp("      A two-line example is:\n");
fp("             DAS   3 -9.820099e-07   9695702.076\n");
fp("             ARS   2 -2.021899e-07   4732736.870\n");
fp("      This example would set the third DAS segment and the second ARS segment.\n");
fp("      All other segments would be fit by dcctvol .  This ASCII file should be\n");
fp("      referenced using timcoef=filename in the command line.\n\n");
/* ------------------------------------------- */
fp("      When you wish to force dcctvol to fit time parameters to distinct segemnts\n");
fp("      choose force=n, where n is the record number you would like a newly fit\n");
fp("      segment to begin.  By convention, force=0 constrains dcctvol to compute\n");
fp("      time parameters for all segments.\n\n");
/* ------------------------------------------- */
fp("example: dcctvol aak023\n");
fp("            process aak023 keeping all channels and not printing H or L recs.\n");
fp("example: dcctvol pfo312 keep=bhz,bhn,bhe\n");
fp("            process pfo312 keeping only bhz,bhn and bhe channels.\n");
fp("example: dcctvol asc100 -alh\n");
fp("            process asc100 using ANTELOPE for configuration information and\n");
fp("            printing H and L recs.\n");
fp("example: dcctvol pfo420 timcoef=pfo_segments\n");
fp("            process pfo420 forcing drift and intercept of selected segments\n");
fp("            to have values set in file pfo_segments.\n");
fp("example: dcctvol mse100 -a use=vhz\n");
fp("            process mse100 using ANTELOPE and using stream vhz for timing\n");
fp("example: dcctvol mse100 -a use=6\n");
fp("            process mse100 using ANTELOPE and using stream 6 (vhz) for timing\n");
fp("example: dcctvol ray004 discard=1,5500-6000\n");
fp("            process ray004 discarding record 1 and records 5500 through 6000.\n");
fp("\n");
/* ------------------------------------------- */
exit(1);
}

/* Revision History
 *
 * $Log: help.c,v $
 * Revision 1.1  2011/03/17 20:23:20  dechavez
 * initial release, based on DCC time_vol sources
 *
 */
