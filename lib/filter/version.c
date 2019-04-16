/*======================================================================
 *
 * library version management
 *
 *====================================================================*/
#include "filter.h"

static VERSION version = {1, 1, 5};

/* filter library release notes

1.1.5  2019-04-15  dauerbach
       response.c:  Add switch clause for FILTER_TYPE_LAPLACE so
                    it doesn't fall through resulting in a0==1.0

1.1.4  11/04/2015
       print.c: removed extraneous (and ignored) printf argument in PrintCoeff()
       string.c: define SetPandZString() as void (oversight)

1.1.3  09/30/2015

1.1.3  09/30/2015
       filter.h: define FILTER_UNITS_PERCENT
       string.c: added support for FILTER_UNITS_PERCENT

1.1.2  09/03/2015
       response.c: fixed bug in IIRtrans()

1.1.1  09/01/2015
       response.c: fixed bug in RunMean()
       string.c: print filter type code in hex

1.1.0  08/24/2015
       filter.h: updated prototypes
       print.c:  change printf format from %lf to %12.5E (compatible with SEED)
       response.c: removed factor from filterResponse(), added filterA0()
       string.c: added filterUnitsString()

1.0.0  07/15/2015
       filter.h: created
       print.c: created
       read.c: created
       response.c: created
       string.c created
 */

char *filterVersionString()
{
static char string[] = "filter library version 100.100.100 and slop";

    snprintf(string, strlen(string), "filter library version %d.%d.%d",
        version.major, version.minor, version.patch
    );

    return string;
}

VERSION *filterVersion()
{
    return &version;
}
