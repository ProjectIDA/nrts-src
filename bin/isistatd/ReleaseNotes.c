#pragma ident "$Id: ReleaseNotes.c,v 1.7 2015/12/18 18:52:14 dechavez Exp $"

char *VersionIdentString = "Release 1.0.6";

/* Release notes

1.0.6   2015-12-18  dechavez
        changed syslogd facility from LOG_USER to LOG_LOCAL0

1.0.5   2015-12-18  dauerbach
        correct bug in channel live_pcnt calculation.
        Was not properly checking tslw value against LIVE_LATENCY_THRESHOLD

1.0.4   2015-11-13  dauerbach
        Filter out 'e' channels for station live_pcnt calculation

1.0.3   2015-10-20  dauerbach
        refactored code to wrap curl setup and tear down around entire server 
        instead of for each server. This at least doubled throughout to browser

1.0.2   2015-10-19  dauerbach
        changed channel details array to :chns from :chns_attributes
        to avoid rename on the rails side

1.0.1   2015-10-14  dauerbach
        Changed default API URL to use HTTPS and added 
        curl_easy_setopt call to set SSL protocol to TLS v1.2

0.0.0  09/02/2015
       initial template
*/
