#pragma ident "$Id: match.c,v 1.2 2015/03/06 21:44:14 dechavez Exp $"
/*======================================================================
 *
 *  Match things
 *
 *====================================================================*/
#include "convertdb.h"

BOOL StageSeedlocPartialMatch(OLD_STAGE *stage, OLD_SEEDLOC *seedloc)
{
    if (strcmp(stage->sta, seedloc->sta) == 0) {
        if (strcmp(stage->chan, seedloc->chan) == 0) {
            if ((long) stage->time >= (long) seedloc->time || (long) stage->endtime <= (long) seedloc->endtime) {
                return TRUE;
            }
        }
    }

    return FALSE;
}

BOOL StageSeedlocExactMatch(OLD_STAGE *stage, OLD_SEEDLOC *seedloc)
{
    if (strcmp(stage->sta, seedloc->sta) == 0) {
        if (strcmp(stage->chan, seedloc->chan) == 0) {
            if ((long) stage->time >= (long) seedloc->time) {
                if ((long) stage->endtime <= (long) seedloc->endtime) {
                    return TRUE;
                }
            }
        }
    }

    return FALSE;
}

BOOL SitechanSeedlocPartialMatch(OLD_SITECHAN *sitechan, OLD_SEEDLOC *seedloc)
{

    if (strcmp(sitechan->sta, seedloc->sta) == 0) {
        if (strcmp(sitechan->chan, seedloc->chan) == 0) {
            if ((long) sitechan->begt >= (long) seedloc->time || (long) sitechan->endt <= (long) seedloc->endtime) {
                return TRUE;
            }
        }
    }

    return FALSE;
}

BOOL SitechanSeedlocExactMatch(OLD_SITECHAN *sitechan, OLD_SEEDLOC *seedloc)
{
    if (strcmp(sitechan->sta, seedloc->sta) == 0) {
        if (strcmp(sitechan->chan, seedloc->chan) == 0) {
            if ((long) sitechan->begt >= (long) seedloc->time) {
                if ((long) sitechan->endt <= (long) seedloc->endtime) {
                    return TRUE;
                }
            }
        }
    }

    return FALSE;
}

BOOL SitechanCalibrationExactMatch(OLD_SITECHAN *sitechan, OLD_CALIBRATION *calibration)
{
    if (strcmp(sitechan->sta, calibration->sta) == 0) {
        if (strcmp(sitechan->chan, calibration->chan) == 0) {
            if ((long) sitechan->begt >= (long) calibration->time) {
                if ((long) sitechan->endt <= (long) calibration->endtime) {
                    return TRUE;
                }
            }
        }
    }

    return FALSE;
}

BOOL SitechanCalibrationPartialMatch(OLD_SITECHAN *sitechan, OLD_CALIBRATION *calibration)
{
    if (strcmp(sitechan->sta, calibration->sta) == 0) {
        if (strcmp(sitechan->chan, calibration->chan) == 0) {
            if ((long) sitechan->begt >= (long) calibration->time || (long) sitechan->endt <= (long) calibration->endtime) {
                   return TRUE;
            }
        }
    }

    return FALSE;
}

/* Revision History
 *
 * $Log: match.c,v $
 * Revision 1.2  2015/03/06 21:44:14  dechavez
 * initial production release
 *
 */
