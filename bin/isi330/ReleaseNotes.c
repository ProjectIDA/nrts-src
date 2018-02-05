char *VersionIdentString = "Release 1.1.0";

/* Release Notes */
/*

1.1.0   2018-02-05 dauerbach
        packet.c: fix call to mssed512ToIda10; Log sta/net/sn upon 1st packet
        init.c: remove sitename from commandline and replace with sta where needed elsewhere in code
        log.c: replace references to site with sta
        q330.c: replace references to site with sta

1.0.0   2018-01-19 dauerbach
        Update call to mseed512toIDA1012 to include serial number
        Pass netname and sta to mseed512toIDA1012 to override data when not null
        link with enhanced IDA 10.12 format definition (ida10 version 2.25.0)

0.9.8   2018-01-17 dauerbach
        Added 'dropvh' option
        ms data wrapped in ida10 and pushed to isi disk loop in lib330 miniseed callback

0.9.1   2018-01-11 dauerbach
        change logging init to use lowercase site/station id

0.9.0   2018-01-11 dauerbach
        Only allow single q330 host
        Change DP to use 1-based indexing
        Change DATA and CTRL ports to use q330->addr instance offset
            so each q330 has unique ports across multiple instances


*/
