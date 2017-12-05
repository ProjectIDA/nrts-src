#pragma ident "$Id: lookup.c,v 1.13 2013/06/19 22:21:04 dechavez Exp $"
/*======================================================================
 * 
 * Lookup things in the configuration database
 *
 *====================================================================*/
#include "q330.h"

#define DEV_PREFIX "/dev/"

Q330_ADDR *q330LookupAddr(char *ident, Q330_CFG *cfg)
{
Q330_ADDR *addr;
LNKLST_NODE *crnt;
char *tmpstr;

    if (cfg == NULL) {
        errno = EINVAL;
        return NULL;
    }

    if ((crnt = listFirstNode(cfg->addr)) == NULL) {
        errno = ENOENT;
        return NULL;
    }

/* Full device path ident means we are using the console */

    if (strncmp(ident, DEV_PREFIX, strlen(DEV_PREFIX)) == 0) {
        addr = (Q330_ADDR *) crnt->payload;
        strncpy(addr->connect.iostr, ident, MAXPATHLEN+1);
        tmpstr = ident + strlen(DEV_PREFIX);
        strncpy(addr->connect.ident, tmpstr, MAXPATHLEN+1);
        strcpy(ident, addr->connect.ident);
        return addr;
    }

/* otherwise, search the list for the requested name */

    crnt = listNextNode(crnt);
    while (crnt != NULL) {
        addr = (Q330_ADDR *) crnt->payload;
        if (strcmp(ident, addr->connect.ident) == 0) return addr;
        crnt = listNextNode(crnt);
    }

    errno = ENOENT;
    return NULL;
}

Q330_AUTH *q330LookupAuth(UINT32 tag, Q330_CFG *cfg)
{
Q330_AUTH *auth;
LNKLST_NODE *crnt;

    if (cfg == NULL) {
        errno = EINVAL;
        return NULL;
    }

    if ((crnt = listFirstNode(cfg->auth)) == NULL) {
        errno = ENOENT;
        return NULL;
    }

    while (crnt != NULL) {
        auth = (Q330_AUTH *) crnt->payload;
        if (tag == auth->tag) return auth;
        crnt = listNextNode(crnt);
    }

    errno = ENOENT;
    return NULL;
}

Q330_DETECTOR *q330LookupDetector(char *name, Q330_CFG *cfg)
{
Q330_DETECTOR *detector;
LNKLST_NODE *crnt;

    if (name == NULL || cfg == NULL) {
        errno = EINVAL;
        return NULL;
    }

    crnt = listFirstNode(cfg->detector);
    while (crnt != NULL) {
        detector = (Q330_DETECTOR *) crnt->payload;
        if (strcmp(name, detector->name) == 0) return detector;
        crnt = listNextNode(crnt);
    }

    errno = ENOENT;
    return NULL;
}

Q330_CALIB *q330LookupCalib(char *name, Q330_ADDR *addr, Q330_CFG *cfg)
{
Q330_CALIB *calib;
LNKLST_NODE *crnt;

    if (name == NULL || cfg == NULL) {
        errno = EINVAL;
        return NULL;
    }

    crnt = listFirstNode(cfg->calib);
    while (crnt != NULL) {
        calib = (Q330_CALIB *) crnt->payload;
        if (strcmp(name, calib->name) == 0 && strcmp(addr->input.a.name, calib->input.name) == 0) return calib;
        crnt = listNextNode(crnt);
    }

    errno = ENOENT;
    return NULL;
}

Q330_CALIB *q330LookupCalibB(char *name, Q330_ADDR *addr, Q330_CFG *cfg)
{
Q330_CALIB *calib;
LNKLST_NODE *crnt;

    if (name == NULL || cfg == NULL) {
        errno = EINVAL;
        return NULL;
    }

    crnt = listFirstNode(cfg->calib);
    while (crnt != NULL) {
        calib = (Q330_CALIB *) crnt->payload;
        if (strcmp(name, calib->name) == 0 && strcmp(addr->input.b.name, calib->input.name) == 0) return calib;
        crnt = listNextNode(crnt);
    }

    errno = ENOENT;
    return NULL;
}

Q330_SENSOR *q330LookupSensor(char *name, Q330_CFG *cfg)
{
Q330_SENSOR *sensor;
LNKLST_NODE *crnt;

    if (name == NULL || cfg == NULL) {
        errno = EINVAL;
        return NULL;
    }

    crnt = listFirstNode(cfg->sensor);
    while (crnt != NULL) {
        sensor = (Q330_SENSOR *) crnt->payload;
        if (strcmp(name, sensor->name) == 0) return sensor;
        crnt = listNextNode(crnt);
    }

    errno = ENOENT;
    return NULL;
}

Q330_LCQ *q330LookupLcq(char *name, Q330_CFG *cfg)
{
LNKLST_NODE *crnt;
Q330_LCQ *lcq;

    if (name == NULL || cfg == NULL) {
        errno = EINVAL;
        return NULL;
    }

    crnt = listFirstNode(cfg->lcq);
    while (crnt != NULL) {
        lcq = (Q330_LCQ *) crnt->payload;
        if (strcmp(name, lcq->name) == 0) return lcq;
        crnt = listNextNode(crnt);
    }

    return NULL;
}

Q330_ADDR *q330GetAddr(char *ident, Q330_CFG *cfg, Q330_ADDR *dest)
{
Q330_ADDR *addr;

    if ((addr = q330LookupAddr(ident, cfg)) == NULL) return NULL;
    if (dest == NULL) return addr;

    *dest = *addr;
    return dest;
}

Q330_AUTH *q330GetAuth(UINT32 tag, Q330_CFG *cfg, Q330_AUTH *dest)
{
Q330_AUTH *auth;

    if ((auth = q330LookupAuth(tag, cfg)) == NULL) return NULL;
    if (dest == NULL) return auth;

    *dest = *auth;
    return dest;
}

Q330_DETECTOR *q330GetDetector(char *name, Q330_CFG *cfg, Q330_DETECTOR *dest)
{
Q330_DETECTOR *detector;

/* NULL name means return the default (if any) */

    if (name == NULL) return q330GetDetector(Q330_DEFAULT_SITE_NAME, cfg, dest);

/* Search the list for an exact match (or default if no exact match) */

    if ((detector = q330LookupDetector(name, cfg)) == NULL) {
        if (strcmp(name, Q330_DEFAULT_SITE_NAME) == 0) {
            if (dest != NULL) {
                dest->enabled = FALSE;
                return dest;
            } else {
                return NULL;
            }
        } else {
            return q330GetDetector(Q330_DEFAULT_SITE_NAME, cfg, dest);
        }
    }

    if (dest == NULL) return detector;
    *dest = *detector;
    return dest;
}

Q330_CALIB *q330GetCalib(char *name, Q330_ADDR *addr, Q330_CFG *cfg, Q330_CALIB *dest)
{
Q330_CALIB *calib;

    if ((calib = q330LookupCalib(name, addr, cfg)) == NULL) return NULL;
    if (dest != NULL) *dest = *calib;
    return calib;
}

Q330_CALIB *q330GetCalibB(char *name, Q330_ADDR *addr, Q330_CFG *cfg, Q330_CALIB *dest)
{
Q330_CALIB *calib;

    if ((calib = q330LookupCalibB(name, addr, cfg)) == NULL) return NULL;
    if (dest != NULL) *dest = *calib;
    return calib;
}

Q330_SENSOR *q330GetSensor(char *name, Q330_CFG *cfg, Q330_SENSOR *dest)
{
Q330_SENSOR *sensor;

    if ((sensor = q330LookupSensor(name, cfg)) == NULL) return NULL;
    if (dest != NULL) *dest = *sensor;
    return sensor;
}

/*-----------------------------------------------------------------------+
 |                                                                       |
 | Copyright (C) 2010 Regents of the University of California            |
 |                                                                       |
 | This software is provided 'as-is', without any express or implied     |
 | warranty.  In no event will the authors be held liable for any        |
 | damages arising from the use of this software.                        |
 |                                                                       |
 | Permission is granted to anyone to use this software for any purpose, |
 | including commercial applications, and to alter it and redistribute   |
 | it freely, subject to the following restrictions:                     |
 |                                                                       |
 | 1. The origin of this software must not be misrepresented; you must   |
 |    not claim that you wrote the original software. If you use this    |
 |    software in a product, an acknowledgment in the product            |
 |    documentation of the contribution by Project IDA, UCSD would be    |
 |    appreciated but is not required.                                   |
 | 2. Altered source versions must be plainly marked as such, and must   |
 |    not be misrepresented as being the original software.              |
 | 3. This notice may not be removed or altered from any source          |
 |    distribution.                                                      |
 |                                                                       |
 +-----------------------------------------------------------------------*/

/* Revision History
 *
 * $Log: lookup.c,v $
 * Revision 1.13  2013/06/19 22:21:04  dechavez
 * added q330LookupCalibB() and q330GetCalibB()
 *
 * Revision 1.12  2011/04/15 17:53:33  dechavez
 * fixed seg fault when searching for a non-existant tag in q330LookupLcq()
 *
 * Revision 1.11  2011/04/14 19:24:15  dechavez
 * added q330LookupLcq()
 *
 * Revision 1.10  2011/02/07 20:04:08  dechavez
 * used DEV_PREFIX for finding basename of device entry instead of utilBasename()
 * which had all sorts of unpleasant side effects on the Solaris 9 builds
 *
 * Revision 1.9  2011/02/01 20:18:47  dechavez
 * q330LookupAddr() updated to recognize device specs for console connection
 *
 * Revision 1.8  2011/01/25 18:09:00  dechavez
 * connect.ident replaces name
 *
 * Revision 1.7  2010/12/17 19:33:00  dechavez
 * libq330 2.0.0
 *
 * Revision 1.6  2010/11/24 18:38:09  dechavez
 * q330LookupCalib() and q330GetCalib() use sensor name as part of search (via new addr argument)
 *
 * Revision 1.5  2010/10/19 21:11:24  dechavez
 * allow NULL name in q330LookupAddr(), give first address in the list
 *
 * Revision 1.4  2009/07/28 18:12:13  dechavez
 * q330GetCalib returns NULL if no match for requested calibration
 *
 * Revision 1.3  2009/07/27 22:41:04  dechavez
 * removed dangling debug printf
 *
 * Revision 1.2  2009/07/27 22:40:17  dechavez
 * fixed bug in q330GetDetector() when no detector is defined
 *
 * Revision 1.1  2009/07/25 17:29:24  dechavez
 * created
 *
 */
