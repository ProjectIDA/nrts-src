char *VersionIdentString = "Release 1.2.0";

/* Release notes

1.2.0 2019-04-15 dauerbach
      Fresh build with libdccdb v2.5.7 and libfilter v1.1.5
      This addresses issues with FILTER_TYPE_LAPLACE responses

1.1.3 02/21/2018 dauerbach
      Fresh build with Antelope 5.7 libraries

1.1.2 03/02/2017
      Fresh build with Antelope 5.6 libraries

1.1.1 12/02/2015
      Link with libfilter 1.2.6 so that things won't fall apart uneccesarily if there
      are stage iunit entries that don't have corresponding matches in the hardcoded
      lookup table that library uses.  Anything missing from that lookup table isn't
      needed downstream.

1.1.0 9/30/2015
      initial production release
*/
