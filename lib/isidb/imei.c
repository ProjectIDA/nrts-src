#pragma ident "$Id: imei.c,v 1.1 2013/05/11 22:55:02 dechavez Exp $"
/*======================================================================
 *
 * IMEI vs ISI site name lookups
 *
 *====================================================================*/
#include "isi/db.h"

BOOL isidbLookupSiteByIMEI(DBIO *db, char *imei, char *site, char *net, UINT64 *zerotime)
{
LOGIO *lp;
static char *fid = "isidbLookupSiteByIMEI";

    if (db == NULL || imei == NULL || site == NULL || net == NULL || zerotime == NULL) {
        lp = (db == NULL) ? NULL : db->lp;
        logioMsg(lp, LOG_DEBUG, "%s: NULL input(s)", fid);
        errno = EINVAL;
        return FALSE;
    }

    if (db->engine != DBIO_MYSQL) {
        return idaffLookupSiteByIMEI(db->ff, imei, site, net, zerotime);
    } else {
        return FALSE; /* didn't bother writing MySQL version */
    }
}

char *isidbLookupIMEIbySite(DBIO *db, char *site)
{
LOGIO *lp;
static char *fid = "isidbLookupIMEIbySite";

    if (db == NULL || site == NULL) {
        lp = (db == NULL) ? NULL : db->lp;
        logioMsg(lp, LOG_DEBUG, "%s: NULL input(s)", fid);
        errno = EINVAL;
        return NULL;
    }

    if (db->engine != DBIO_MYSQL) {
        return idaffLookupIMEIbySite(db->ff, site);
    } else {
        return NULL; /* didn't bother writing MySQL version */
    }
}

/* Revision History
 *
 * $Log: imei.c,v $
 * Revision 1.1  2013/05/11 22:55:02  dechavez
 * initial release
 *
 */
