#pragma ident "$Id: ReleaseNotes.c,v 1.5 2015/12/18 18:54:10 dechavez Exp $"

char *VersionIdentString = "Release 1.2.2";

/* Release Notes

1.2.2   12/19/2015
        changed syslog facility from LOG_USER to LOG_LOCAL0

1.2.1   07/25/2012
        Run as user nrts by default (added user=name optional argument)

1.2.0   05/02/2012
        ** initial production release ***
        Track missed readings and permit re-use of previous reading one time.
        This is to eliminate the occasional spike that results when a reading is
        missed.  Use aerrs (replacing aex0[12]) to flag when this happens.

1.1.0   04/26/2012
        Introduced aex0[12] stream to track reading validity.  Now previous
        readings are preserved in case of timeout or other error so that no
        spikes appear in the time series.  The believability of the traces
        can be inferred from looking at the aex stream.  Also, the two slaves
        are treated independently, so an invalid reading from one doesn't
        affect the validity of the reading from the other.

1.0.0   04/25/2012
        initial release

 */
