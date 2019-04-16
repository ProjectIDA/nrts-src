/*======================================================================
 *
 * library version management
 *
 *====================================================================*/
#include "dccdb.h"

static VERSION version = {1, 2, 7};

/* dccdb library release notes

1.2.7   2019-04-15 dauerbach
        response.c: Check for FILTER_TYPE_LAPLACE poles and zeros response to skip call to
                    filterResponse() same as for FILTER_TYPE_ANALOG poles and zeros response
                    This fixes problem with stage 0 A0 calculation for LAPLACE responses

1.2.6   12/01/2015
        response.c: don't fail if units lookup returns NULL

1.2.5   09/30/2015
        dccdb.h:   removed the isduplicate fields from DCCDB_CHAN, DCCDB_STAGE, DCCDB_CASCADE
                   added definitions for DCCDB_EMPTY_LOC and DCCDB_CHAN_FLAG_TYPE_SOH
        abbrev.c:  removed uneeded free() in the event of dbgetv error (FreeDB() in db.c takes care of that)
        cascade.c: removed isduplicate field
        chan.c:    removed uneeded free() in the event of dbgetv error (FreeDB() in db.c takes care of that)
                   set loc to "  " (double blanks) if stored as a null value ("-") in the database
        lookup.c:  AddEmptyCascade() if no stage channels match chan epoch in dccdbLookupCascadeFromChan()
        stage.c:   removed uneeded free() in the event of dbgetv error (FreeDB() in db.c takes care of that)
                   set ssident to an empty string if stored as a null value ("-") in the database
        strings.c: removed isduplicate field support from dccdbStageString()
        units.c:   use positive, non-zero codes
                   removed uneeded free() in the event of dbgetv error (FreeDB() in db.c takes care of that)

1.2.4   09/11/2015
        abbrev.c: use positive, non-zero codes
        lookup.c: return actual code, not loop index, in dccdbLookupAbbrevCode() and dccdbLookupUnitCode()

1.2.3   09/03/2015
        dccdb.h: added chan and isduplicate fields to DCCDB_CASCADE, updated prototypes
        cascade.c: added "full" arg to dccdbPrintCascade(), initialize new chan and isduplicate fields in dccdbBuildCascades()
        db.c: removed end time sanity checks from previous commit
        lookup.c: assign cascade->chan in dccdbLookupCascadeFromChan() with "parent"

1.2.2   08/27/2015
        db.c: added sanity checks for suspcious channel and stage end times, then ifdef'd out after patching database
        lookup.c: fixed logic in "join" of chan and cascade in dccdbLookupCascadeFromChan()

1.2.1   08/26/2015
        response.c: fixed bug in setting cascade->a0 (multiply by mag(resp), NOT the inverse!)

1.2.0   08/24/2015
        dccdb.h: added DCCDB_ERRCODE_* constants, turned stage izero and leadfac to
           unused1 and unused2, added a0 and errcode to DCCDB_CASCADE,
           added dccdbSetCascadeA0Freq() prototype
        cascade.c: added a0 and removed izero and leadfac from dccdbPrintCascade(), added a0 to dccdbBuildCascades()
        chan.c: fixed cut and paste typo when reporting the nmber of chan records read/sorted
        lookup.c: made rule for time window matching between stage and chan more lax, set a0 field
        response.c: initial release
        stage.c: replaced izero and leadfac with unused1 and unused2

1.1.0   07/10/2015
        abbrev.c: initial release
        cascade.c: initial release
        chan.c: initial release
        chan.c: initial release
        db.c: moved all the read functions to their own files, added dccdbCloseDatabase(), added cascade support
        lookup.c: added dccdbLookupCascadeFromChan()
        print.c: added dccdbPrintStageList()
        seedloc.c: initial release
        site.c: initial release
        stage.c: initial release
        strings.c: added dccdbTableName(), dccdbTableCode()
        units.c: initial release
        util.c: replaced long with INT32

1.0.0   03/06/2015
        dccdb.h: initial release
        db.c: initial release
        lookup.c: initial release
        print.c: initial release
        strings.c: initial release
        util.c: initial release
        version.c: 1.0.0
 */

char *dccdbVersionString()
{
static char string[] = "dccdb library version 100.100.100 and slop";

    snprintf(string, strlen(string), "dccdb library version %d.%d.%d",
        version.major, version.minor, version.patch
    );

    return string;
}

VERSION *dccdbVersion()
{
    return &version;
}
