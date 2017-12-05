#pragma ident "$Id: old.c,v 1.2 2015/03/06 21:44:14 dechavez Exp $"
/*======================================================================
 *
 *  Read the OLD database
 *
 *====================================================================*/
#include "convertdb.h"

static OLD *olddb = NULL;

static OLD *FreeOldDB(OLD *old)
{
    if (old == NULL) return NULL;

    if (old->instrument  != NULL) free(old->instrument);
    if (old->seedloc     != NULL) free(old->seedloc);
    if (old->sensor      != NULL) free(old->sensor);
    if (old->site        != NULL) free(old->site);
    if (old->sitechan    != NULL) free(old->sitechan);
    if (old->stage       != NULL) free(old->stage);

    free(old);
    return NULL;
}

void CloseOldDatabase(void)
{
    if (olddb != NULL) {
        dbclose(olddb->table.instrument);
        dbclose(olddb->table.seedloc);
        dbclose(olddb->table.sensor);
        dbclose(olddb->table.site);
        dbclose(olddb->table.sitechan);
        dbclose(olddb->table.stage);
    }

    FreeOldDB(olddb);
}

static BOOL ReadCalibration(OLD *old)
{
int i;
static char *fid = "OpenOldDatabase:ReadCalibration";

    if (dbquery(old->table.calibration, dbRECORD_COUNT, &old->ncalibration) < 0) {
        logioMsg(old->lp, LOG_ERR, "%s: dbquery failed", fid);
        return FALSE;
    }

    if ((old->calibration = (OLD_CALIBRATION *) malloc(sizeof(OLD_CALIBRATION) * old->ncalibration)) == NULL) {
        logioMsg(old->lp, LOG_ERR, "%s: malloc failed: %s", fid, strerror(errno));
        return FALSE;
    }

    for (i = 0; i < old->ncalibration; i++) {
        old->table.calibration.record = i;
        if (dbgetv(old->table.calibration, 0, OLD_CALIBRATION_STA, old->calibration[i].sta, 0) < 0) {
            logioMsg(old->lp, LOG_ERR, "%s: table '%s' dbgetv '%s' failed", fid, OLD_CALIBRATION_TABLE_NAME, OLD_CALIBRATION_STA);
            free(old->calibration);
            return FALSE;
        }
        if (dbgetv(old->table.calibration, 0, OLD_CALIBRATION_CHAN, old->calibration[i].chan, 0) < 0) {
            logioMsg(old->lp, LOG_ERR, "%s: table '%s' dbgetv '%s' failed", fid, OLD_CALIBRATION_TABLE_NAME, OLD_CALIBRATION_CHAN);
            free(old->calibration);
            return FALSE;
        }
        if (dbgetv(old->table.calibration, 0, OLD_CALIBRATION_TIME, &old->calibration[i].time, 0) < 0) {
            logioMsg(old->lp, LOG_ERR, "%s: table '%s' dbgetv '%s' failed", fid, OLD_CALIBRATION_TABLE_NAME, OLD_CALIBRATION_TIME);
            free(old->calibration);
            return FALSE;
        }
        if (dbgetv(old->table.calibration, 0, OLD_CALIBRATION_ENDTIME, &old->calibration[i].endtime, 0) < 0) {
            logioMsg(old->lp, LOG_ERR, "%s: table '%s' dbgetv '%s' failed", fid, OLD_CALIBRATION_TABLE_NAME, OLD_CALIBRATION_ENDTIME);
            free(old->calibration);
            return FALSE;
        }
        if (dbgetv(old->table.calibration, 0, OLD_CALIBRATION_INSNAME, old->calibration[i].insname, 0) < 0) {
            logioMsg(old->lp, LOG_ERR, "%s: table '%s' dbgetv '%s' failed", fid, OLD_CALIBRATION_TABLE_NAME, OLD_CALIBRATION_INSNAME);
            free(old->calibration);
            return FALSE;
        }
        if (dbgetv(old->table.calibration, 0, OLD_CALIBRATION_SAMPRATE, &old->calibration[i].samprate, 0) < 0) {
            logioMsg(old->lp, LOG_ERR, "%s: table '%s' dbgetv '%s' failed", fid, OLD_CALIBRATION_TABLE_NAME, OLD_CALIBRATION_SAMPRATE);
            free(old->calibration);
            return FALSE;
        }
        if (dbgetv(old->table.calibration, 0, OLD_CALIBRATION_SEGTYPE, old->calibration[i].segtype, 0) < 0) {
            logioMsg(old->lp, LOG_ERR, "%s: table '%s' dbgetv '%s' failed", fid, OLD_CALIBRATION_TABLE_NAME, OLD_CALIBRATION_SEGTYPE);
            free(old->calibration);
            return FALSE;
        }
        if (dbgetv(old->table.calibration, 0, OLD_CALIBRATION_CALIB, &old->calibration[i].calib, 0) < 0) {
            logioMsg(old->lp, LOG_ERR, "%s: table '%s' dbgetv '%s' failed", fid, OLD_CALIBRATION_TABLE_NAME, OLD_CALIBRATION_CALIB);
            free(old->calibration);
            return FALSE;
        }
        if (dbgetv(old->table.calibration, 0, OLD_CALIBRATION_CALPER, &old->calibration[i].calper, 0) < 0) {
            logioMsg(old->lp, LOG_ERR, "%s: table '%s' dbgetv '%s' failed", fid, OLD_CALIBRATION_TABLE_NAME, OLD_CALIBRATION_CALPER);
            free(old->calibration);
            return FALSE;
        }
        if (dbgetv(old->table.calibration, 0, OLD_CALIBRATION_FC, &old->calibration[i].fc, 0) < 0) {
            logioMsg(old->lp, LOG_ERR, "%s: table '%s' dbgetv '%s' failed", fid, OLD_CALIBRATION_TABLE_NAME, OLD_CALIBRATION_FC);
            free(old->calibration);
            return FALSE;
        }
        if (dbgetv(old->table.calibration, 0, OLD_CALIBRATION_UNITS, old->calibration[i].units, 0) < 0) {
            logioMsg(old->lp, LOG_ERR, "%s: table '%s' dbgetv '%s' failed", fid, OLD_CALIBRATION_TABLE_NAME, OLD_CALIBRATION_UNITS);
            free(old->calibration);
            return FALSE;
        }
        if (dbgetv(old->table.calibration, 0, OLD_CALIBRATION_LDDATE, &old->calibration[i].lddate, 0) < 0) {
            logioMsg(old->lp, LOG_ERR, "%s: table '%s' dbgetv '%s' failed", fid, OLD_CALIBRATION_TABLE_NAME, OLD_CALIBRATION_LDDATE);
            free(old->calibration);
            return FALSE;
        }
    }

    return TRUE;
}

static BOOL ReadInstrument(OLD *old)
{
int i;
static char *fid = "OpenOldDatabase:ReadInstrument";

    if (dbquery(old->table.instrument, dbRECORD_COUNT, &old->ninstrument) < 0) {
        logioMsg(old->lp, LOG_ERR, "%s: dbquery failed", fid);
        return FALSE;
    }

    if ((old->instrument = (OLD_INSTRUMENT *) malloc(sizeof(OLD_INSTRUMENT) * old->ninstrument)) == NULL) {
        logioMsg(old->lp, LOG_ERR, "%s: malloc failed: %s", fid, strerror(errno));
        return FALSE;
    }

    for (i = 0; i < old->ninstrument; i++) {
        old->table.instrument.record = i;
        if (dbgetv(old->table.instrument, 0, OLD_INSTRUMENT_INID, &old->instrument[i].inid, 0) < 0) {
            logioMsg(old->lp, LOG_ERR, "%s: table '%s' dbgetv '%s' failed", fid, OLD_INSTRUMENT_TABLE_NAME, OLD_INSTRUMENT_INID);
            free(old->instrument);
            return FALSE;
        }
        if (dbgetv(old->table.instrument, 0, OLD_INSTRUMENT_SAMPRATE, &old->instrument[i].samprate, 0) < 0) {
            logioMsg(old->lp, LOG_ERR, "%s: table '%s' dbgetv '%s' failed", fid, OLD_INSTRUMENT_TABLE_NAME, OLD_INSTRUMENT_SAMPRATE);
            free(old->instrument);
            return FALSE;
        }
        if (dbgetv(old->table.instrument, 0, OLD_INSTRUMENT_NCALIB, &old->instrument[i].ncalib, 0) < 0) {
            logioMsg(old->lp, LOG_ERR, "%s: table '%s' dbgetv '%s' failed", fid, OLD_INSTRUMENT_TABLE_NAME, OLD_INSTRUMENT_NCALIB);
            free(old->instrument);
            return FALSE;
        }
        if (dbgetv(old->table.instrument, 0, OLD_INSTRUMENT_NCALPER, &old->instrument[i].ncalper, 0) < 0) {
            logioMsg(old->lp, LOG_ERR, "%s: table '%s' dbgetv '%s' failed", fid, OLD_INSTRUMENT_TABLE_NAME, OLD_INSTRUMENT_NCALPER);
            free(old->instrument);
            return FALSE;
        }
        if (dbgetv(old->table.instrument, 0, OLD_INSTRUMENT_LDDATE, &old->instrument[i].lddate, 0) < 0) {
            logioMsg(old->lp, LOG_ERR, "%s: table '%s' dbgetv '%s' failed", fid, OLD_INSTRUMENT_TABLE_NAME, OLD_INSTRUMENT_LDDATE);
            free(old->instrument);
            return FALSE;
        }
        if (dbgetv(old->table.instrument, 0, OLD_INSTRUMENT_INSNAME, old->instrument[i].insname, 0) < 0) {
            logioMsg(old->lp, LOG_ERR, "%s: table '%s' dbgetv '%s' failed", fid, OLD_INSTRUMENT_TABLE_NAME, OLD_INSTRUMENT_INSNAME);
            free(old->instrument);
            return FALSE;
        }
        if (dbgetv(old->table.instrument, 0, OLD_INSTRUMENT_INSTYPE, old->instrument[i].instype, 0) < 0) {
            logioMsg(old->lp, LOG_ERR, "%s: table '%s' dbgetv '%s' failed", fid, OLD_INSTRUMENT_TABLE_NAME, OLD_INSTRUMENT_INSTYPE);
            free(old->instrument);
            return FALSE;
        }
        if (dbgetv(old->table.instrument, 0, OLD_INSTRUMENT_DIR, old->instrument[i].dir, 0) < 0) {
            logioMsg(old->lp, LOG_ERR, "%s: table '%s' dbgetv '%s' failed", fid, OLD_INSTRUMENT_TABLE_NAME, OLD_INSTRUMENT_DIR);
            free(old->instrument);
            return FALSE;
        }
        if (dbgetv(old->table.instrument, 0, OLD_INSTRUMENT_DFILE, old->instrument[i].dfile, 0) < 0) {
            logioMsg(old->lp, LOG_ERR, "%s: table '%s' dbgetv '%s' failed", fid, OLD_INSTRUMENT_TABLE_NAME, OLD_INSTRUMENT_DFILE);
            free(old->instrument);
            return FALSE;
        }
        if (dbgetv(old->table.instrument, 0, OLD_INSTRUMENT_RSPTYPE, old->instrument[i].rsptype, 0) < 0) {
            logioMsg(old->lp, LOG_ERR, "%s: table '%s' dbgetv '%s' failed", fid, OLD_INSTRUMENT_TABLE_NAME, OLD_INSTRUMENT_RSPTYPE);
            free(old->instrument);
            return FALSE;
        }
    }

    return TRUE;
}

static BOOL ReadSchanloc(OLD *old)
{
int i;
static char *fid = "OpenOldDatabase:ReadSchanloc";

    if (dbquery(old->table.schanloc, dbRECORD_COUNT, &old->nschanloc) < 0) {
        logioMsg(old->lp, LOG_ERR, "%s: dbquery failed", fid);
        return FALSE;
    }

    if ((old->schanloc = (OLD_SCHANLOC *) malloc(sizeof(OLD_SCHANLOC) * old->nschanloc)) == NULL) {
        logioMsg(old->lp, LOG_ERR, "%s: malloc failed: %s", fid, strerror(errno));
        return FALSE;
    }

    for (i = 0; i < old->nschanloc; i++) {
        old->table.schanloc.record = i;
        if (dbgetv(old->table.schanloc, 0, OLD_SCHANLOC_STA, old->schanloc[i].sta, 0) < 0) {
            logioMsg(old->lp, LOG_ERR, "%s: table '%s' dbgetv '%s' failed", fid, OLD_SCHANLOC_TABLE_NAME, OLD_SCHANLOC_STA);
            free(old->schanloc);
            return FALSE;
        }
        if (dbgetv(old->table.schanloc, 0, OLD_SCHANLOC_CHAN, old->schanloc[i].chan, 0) < 0) {
            logioMsg(old->lp, LOG_ERR, "%s: table '%s' dbgetv '%s' failed", fid, OLD_SCHANLOC_TABLE_NAME, OLD_SCHANLOC_CHAN);
            free(old->schanloc);
            return FALSE;
        }
        if (dbgetv(old->table.schanloc, 0, OLD_SCHANLOC_FCHAN, old->schanloc[i].fchan, 0) < 0) {
            logioMsg(old->lp, LOG_ERR, "%s: table '%s' dbgetv '%s' failed", fid, OLD_SCHANLOC_TABLE_NAME, OLD_SCHANLOC_FCHAN);
            free(old->schanloc);
            return FALSE;
        }
        if (dbgetv(old->table.schanloc, 0, OLD_SCHANLOC_LOC, old->schanloc[i].loc, 0) < 0) {
            logioMsg(old->lp, LOG_ERR, "%s: table '%s' dbgetv '%s' failed", fid, OLD_SCHANLOC_TABLE_NAME, OLD_SCHANLOC_LOC);
            free(old->schanloc);
            return FALSE;
        }
    }

    return TRUE;
}

static BOOL ReadSeedloc(OLD *old)
{
int i;
static char *fid = "OpenOldDatabase:ReadSeedloc";

    if (dbquery(old->table.seedloc, dbRECORD_COUNT, &old->nseedloc) < 0) {
        logioMsg(old->lp, LOG_ERR, "%s: dbquery failed", fid);
        return FALSE;
    }

    if ((old->seedloc = (OLD_SEEDLOC *) malloc(sizeof(OLD_SEEDLOC) * old->nseedloc)) == NULL) {
        logioMsg(old->lp, LOG_ERR, "%s: malloc failed: %s", fid, strerror(errno));
        return FALSE;
    }

    for (i = 0; i < old->nseedloc; i++) {
        old->table.seedloc.record = i;
        if (dbgetv(old->table.seedloc, 0, OLD_SEEDLOC_TIME, &old->seedloc[i].time, 0) < 0) {
            logioMsg(old->lp, LOG_ERR, "%s: table '%s' dbgetv '%s' failed", fid, OLD_SEEDLOC_TABLE_NAME, OLD_SEEDLOC_TIME);
            free(old->seedloc);
            return FALSE;
        }
        if (dbgetv(old->table.seedloc, 0, OLD_SEEDLOC_ENDTIME, &old->seedloc[i].endtime, 0) < 0) {
            logioMsg(old->lp, LOG_ERR, "%s: table '%s' dbgetv '%s' failed", fid, OLD_SEEDLOC_TABLE_NAME, OLD_SEEDLOC_ENDTIME);
            free(old->seedloc);
            return FALSE;
        }
        if (dbgetv(old->table.seedloc, 0, OLD_SEEDLOC_LDDATE, &old->seedloc[i].lddate, 0) < 0) {
            logioMsg(old->lp, LOG_ERR, "%s: table '%s' dbgetv '%s' failed", fid, OLD_SEEDLOC_TABLE_NAME, OLD_SEEDLOC_LDDATE);
            free(old->seedloc);
            return FALSE;
        }
        if (dbgetv(old->table.seedloc, 0, OLD_SEEDLOC_STA, old->seedloc[i].sta, 0) < 0) {
            logioMsg(old->lp, LOG_ERR, "%s: table '%s' dbgetv '%s' failed", fid, OLD_SEEDLOC_TABLE_NAME, OLD_SEEDLOC_STA);
            free(old->seedloc);
            return FALSE;
        }
        if (dbgetv(old->table.seedloc, 0, OLD_SEEDLOC_CHAN, old->seedloc[i].chan, 0) < 0) {
            logioMsg(old->lp, LOG_ERR, "%s: table '%s' dbgetv '%s' failed", fid, OLD_SEEDLOC_TABLE_NAME, OLD_SEEDLOC_CHAN);
            free(old->seedloc);
            return FALSE;
        }
        if (dbgetv(old->table.seedloc, 0, OLD_SEEDLOC_NEWCHAN, old->seedloc[i].newchan, 0) < 0) {
            logioMsg(old->lp, LOG_ERR, "%s: table '%s' dbgetv '%s' failed", fid, OLD_SEEDLOC_TABLE_NAME, OLD_SEEDLOC_NEWCHAN);
            free(old->seedloc);
            return FALSE;
        }
        if (dbgetv(old->table.seedloc, 0, OLD_SEEDLOC_LOCCODE, old->seedloc[i].loccode, 0) < 0) {
            logioMsg(old->lp, LOG_ERR, "%s: table '%s' dbgetv '%s' failed", fid, OLD_SEEDLOC_TABLE_NAME, OLD_SEEDLOC_LOCCODE);
            free(old->seedloc);
            return FALSE;
        }
    }

    return TRUE;
}

static BOOL ReadSensor(OLD *old)
{
int i;
static char *fid = "OpenOldDatabase:ReadSensor";

    if (dbquery(old->table.sensor, dbRECORD_COUNT, &old->nsensor) < 0) {
        logioMsg(old->lp, LOG_ERR, "%s: dbquery failed", fid);
        return FALSE;
    }

    if ((old->sensor = (OLD_SENSOR *) malloc(sizeof(OLD_SENSOR) * old->nsensor)) == NULL) {
        logioMsg(old->lp, LOG_ERR, "%s: malloc failed: %s", fid, strerror(errno));
        return FALSE;
    }

    for (i = 0; i < old->nsensor; i++) {
        old->table.sensor.record = i;
        if (dbgetv(old->table.sensor, 0, OLD_SENSOR_TIME, &old->sensor[i].time, 0) < 0) {
            logioMsg(old->lp, LOG_ERR, "%s: table '%s' dbgetv '%s' failed", fid, OLD_SENSOR_TABLE_NAME, OLD_SENSOR_TIME);
            free(old->sensor);
            return FALSE;
        }
        if (dbgetv(old->table.sensor, 0, OLD_SENSOR_ENDTIME, &old->sensor[i].endtime, 0) < 0) {
            logioMsg(old->lp, LOG_ERR, "%s: table '%s' dbgetv '%s' failed", fid, OLD_SENSOR_TABLE_NAME, OLD_SENSOR_ENDTIME);
            free(old->sensor);
            return FALSE;
        }
        if (dbgetv(old->table.sensor, 0, OLD_SENSOR_INID, &old->sensor[i].inid, 0) < 0) {
            logioMsg(old->lp, LOG_ERR, "%s: table '%s' dbgetv '%s' failed", fid, OLD_SENSOR_TABLE_NAME, OLD_SENSOR_INID);
            free(old->sensor);
            return FALSE;
        }
        if (dbgetv(old->table.sensor, 0, OLD_SENSOR_CHANID, &old->sensor[i].chanid, 0) < 0) {
            logioMsg(old->lp, LOG_ERR, "%s: table '%s' dbgetv '%s' failed", fid, OLD_SENSOR_TABLE_NAME, OLD_SENSOR_CHANID);
            free(old->sensor);
            return FALSE;
        }
        if (dbgetv(old->table.sensor, 0, OLD_SENSOR_JDATE, &old->sensor[i].jdate, 0) < 0) {
            logioMsg(old->lp, LOG_ERR, "%s: table '%s' dbgetv '%s' failed", fid, OLD_SENSOR_TABLE_NAME, OLD_SENSOR_JDATE);
            free(old->sensor);
            return FALSE;
        }
        if (dbgetv(old->table.sensor, 0, OLD_SENSOR_CALRATIO, &old->sensor[i].calratio, 0) < 0) {
            logioMsg(old->lp, LOG_ERR, "%s: table '%s' dbgetv '%s' failed", fid, OLD_SENSOR_TABLE_NAME, OLD_SENSOR_CALRATIO);
            free(old->sensor);
            return FALSE;
        }
        if (dbgetv(old->table.sensor, 0, OLD_SENSOR_CALPER, &old->sensor[i].calper, 0) < 0) {
            logioMsg(old->lp, LOG_ERR, "%s: table '%s' dbgetv '%s' failed", fid, OLD_SENSOR_TABLE_NAME, OLD_SENSOR_CALPER);
            free(old->sensor);
            return FALSE;
        }
        if (dbgetv(old->table.sensor, 0, OLD_SENSOR_TSHIFT, &old->sensor[i].tshift, 0) < 0) {
            logioMsg(old->lp, LOG_ERR, "%s: table '%s' dbgetv '%s' failed", fid, OLD_SENSOR_TABLE_NAME, OLD_SENSOR_TSHIFT);
            free(old->sensor);
            return FALSE;
        }
        if (dbgetv(old->table.sensor, 0, OLD_SENSOR_LDDATE, &old->sensor[i].lddate, 0) < 0) {
            logioMsg(old->lp, LOG_ERR, "%s: table '%s' dbgetv '%s' failed", fid, OLD_SENSOR_TABLE_NAME, OLD_SENSOR_LDDATE);
            free(old->sensor);
            return FALSE;
        }
        if (dbgetv(old->table.sensor, 0, OLD_SENSOR_STA, old->sensor[i].sta, 0) < 0) {
            logioMsg(old->lp, LOG_ERR, "%s: table '%s' dbgetv '%s' failed", fid, OLD_SENSOR_TABLE_NAME, OLD_SENSOR_STA);
            free(old->sensor);
            return FALSE;
        }
        if (dbgetv(old->table.sensor, 0, OLD_SENSOR_CHAN, old->sensor[i].chan, 0) < 0) {
            logioMsg(old->lp, LOG_ERR, "%s: table '%s' dbgetv '%s' failed", fid, OLD_SENSOR_TABLE_NAME, OLD_SENSOR_CHAN);
            free(old->sensor);
            return FALSE;
        }
        if (dbgetv(old->table.sensor, 0, OLD_SENSOR_INSTANT, old->sensor[i].instant, 0) < 0) {
            logioMsg(old->lp, LOG_ERR, "%s: table '%s' dbgetv '%s' failed", fid, OLD_SENSOR_TABLE_NAME, OLD_SENSOR_INSTANT);
            free(old->sensor);
            return FALSE;
        }
    }

    return TRUE;
}

static BOOL ReadSite(OLD *old)
{
int i;
static char *fid = "OpenOldDatabase:ReadSite";

    if (dbquery(old->table.site, dbRECORD_COUNT, &old->nsite) < 0) {
        logioMsg(old->lp, LOG_ERR, "%s: dbquery failed", fid);
        return FALSE;
    }

    if ((old->site = (OLD_SITE *) malloc(sizeof(OLD_SITE) * old->nsite)) == NULL) {
        logioMsg(old->lp, LOG_ERR, "%s: malloc failed: %s", fid, strerror(errno));
        return FALSE;
    }

    for (i = 0; i < old->nsite; i++) {
        old->table.site.record = i;
        if (dbgetv(old->table.site, 0, OLD_SITE_ONDATE, &old->site[i].ondate, 0) < 0) {
            logioMsg(old->lp, LOG_ERR, "%s: table '%s' dbgetv '%s' failed", fid, OLD_SITE_TABLE_NAME, OLD_SITE_ONDATE);
            free(old->site);
            return FALSE;
        }
        if (dbgetv(old->table.site, 0, OLD_SITE_OFFDATE, &old->site[i].offdate, 0) < 0) {
            logioMsg(old->lp, LOG_ERR, "%s: table '%s' dbgetv '%s' failed", fid, OLD_SITE_TABLE_NAME, OLD_SITE_OFFDATE);
            free(old->site);
            return FALSE;
        }
        if (dbgetv(old->table.site, 0, OLD_SITE_LDDATE, &old->site[i].lddate, 0) < 0) {
            logioMsg(old->lp, LOG_ERR, "%s: table '%s' dbgetv '%s' failed", fid, OLD_SITE_TABLE_NAME, OLD_SITE_LDDATE);
            free(old->site);
            return FALSE;
        }
        if (dbgetv(old->table.site, 0, OLD_SITE_LAT, &old->site[i].lat, 0) < 0) {
            logioMsg(old->lp, LOG_ERR, "%s: table '%s' dbgetv '%s' failed", fid, OLD_SITE_TABLE_NAME, OLD_SITE_LAT);
            free(old->site);
            return FALSE;
        }
        if (dbgetv(old->table.site, 0, OLD_SITE_LON, &old->site[i].lon, 0) < 0) {
            logioMsg(old->lp, LOG_ERR, "%s: table '%s' dbgetv '%s' failed", fid, OLD_SITE_TABLE_NAME, OLD_SITE_LON);
            free(old->site);
            return FALSE;
        }
        if (dbgetv(old->table.site, 0, OLD_SITE_ELEV, &old->site[i].elev, 0) < 0) {
            logioMsg(old->lp, LOG_ERR, "%s: table '%s' dbgetv '%s' failed", fid, OLD_SITE_TABLE_NAME, OLD_SITE_ELEV);
            free(old->site);
            return FALSE;
        }
        if (dbgetv(old->table.site, 0, OLD_SITE_DNORTH, &old->site[i].dnorth, 0) < 0) {
            logioMsg(old->lp, LOG_ERR, "%s: table '%s' dbgetv '%s' failed", fid, OLD_SITE_TABLE_NAME, OLD_SITE_DNORTH);
            free(old->site);
            return FALSE;
        }
        if (dbgetv(old->table.site, 0, OLD_SITE_DEAST, &old->site[i].deast, 0) < 0) {
            logioMsg(old->lp, LOG_ERR, "%s: table '%s' dbgetv '%s' failed", fid, OLD_SITE_TABLE_NAME, OLD_SITE_DEAST);
            free(old->site);
            return FALSE;
        }
        if (dbgetv(old->table.site, 0, OLD_SITE_LDDATE, &old->site[i].lddate, 0) < 0) {
            logioMsg(old->lp, LOG_ERR, "%s: table '%s' dbgetv '%s' failed", fid, OLD_SITE_TABLE_NAME, OLD_SITE_LDDATE);
            free(old->site);
            return FALSE;
        }
        if (dbgetv(old->table.site, 0, OLD_SITE_STA, old->site[i].sta, 0) < 0) {
            logioMsg(old->lp, LOG_ERR, "%s: table '%s' dbgetv '%s' failed", fid, OLD_SITE_TABLE_NAME, OLD_SITE_STA);
            free(old->site);
            return FALSE;
        }
        if (dbgetv(old->table.site, 0, OLD_SITE_STANAME, old->site[i].staname, 0) < 0) {
            logioMsg(old->lp, LOG_ERR, "%s: table '%s' dbgetv '%s' failed", fid, OLD_SITE_TABLE_NAME, OLD_SITE_STANAME);
            free(old->site);
            return FALSE;
        }
        if (dbgetv(old->table.site, 0, OLD_SITE_STATYPE, old->site[i].statype, 0) < 0) {
            logioMsg(old->lp, LOG_ERR, "%s: table '%s' dbgetv '%s' failed", fid, OLD_SITE_TABLE_NAME, OLD_SITE_STATYPE);
            free(old->site);
            return FALSE;
        }
        if (dbgetv(old->table.site, 0, OLD_SITE_REFSTA, old->site[i].refsta, 0) < 0) {
            logioMsg(old->lp, LOG_ERR, "%s: table '%s' dbgetv '%s' failed", fid, OLD_SITE_TABLE_NAME, OLD_SITE_REFSTA);
            free(old->site);
            return FALSE;
        }
        old->site[i].begt = dccdbOndateToBegt(old->site[i].ondate);
        old->site[i].endt = dccdbOffdateToEndt(old->site[i].offdate);
    }

    return TRUE;
}

static BOOL ReadSitechan(OLD *old)
{
int i;
static char *fid = "OpenOldDatabase:ReadSitechan";
char t1[MAXPATHLEN+1];
char t2[MAXPATHLEN+1];

    if (dbquery(old->table.sitechan, dbRECORD_COUNT, &old->nsitechan) < 0) {
        logioMsg(old->lp, LOG_ERR, "%s: dbquery failed", fid);
        return FALSE;
    }

    if ((old->sitechan = (OLD_SITECHAN *) malloc(sizeof(OLD_SITECHAN) * old->nsitechan)) == NULL) {
        logioMsg(old->lp, LOG_ERR, "%s: malloc failed: %s", fid, strerror(errno));
        return FALSE;
    }

    for (i = 0; i < old->nsitechan; i++) {
        old->table.sitechan.record = i;
        if (dbgetv(old->table.sitechan, 0, OLD_SITECHAN_ONDATE, &old->sitechan[i].ondate, 0) < 0) {
            logioMsg(old->lp, LOG_ERR, "%s: table '%s' dbgetv '%s' failed", fid, OLD_SITECHAN_TABLE_NAME, OLD_SITECHAN_ONDATE);
            free(old->sitechan);
            return FALSE;
        }
        if (dbgetv(old->table.sitechan, 0, OLD_SITECHAN_OFFDATE, &old->sitechan[i].offdate, 0) < 0) {
            logioMsg(old->lp, LOG_ERR, "%s: table '%s' dbgetv '%s' failed", fid, OLD_SITECHAN_TABLE_NAME, OLD_SITECHAN_OFFDATE);
            free(old->sitechan);
            return FALSE;
        }
        if (dbgetv(old->table.sitechan, 0, OLD_SITECHAN_LDDATE, &old->sitechan[i].lddate, 0) < 0) {
            logioMsg(old->lp, LOG_ERR, "%s: table '%s' dbgetv '%s' failed", fid, OLD_SITECHAN_TABLE_NAME, OLD_SITECHAN_LDDATE);
            free(old->sitechan);
            return FALSE;
        }
        if (dbgetv(old->table.sitechan, 0, OLD_SITECHAN_CHANID, &old->sitechan[i].chanid, 0) < 0) {
            logioMsg(old->lp, LOG_ERR, "%s: table '%s' dbgetv '%s' failed", fid, OLD_SITECHAN_TABLE_NAME, OLD_SITECHAN_CHANID);
            free(old->sitechan);
            return FALSE;
        }
        if (dbgetv(old->table.sitechan, 0, OLD_SITECHAN_EDEPTH, &old->sitechan[i].edepth, 0) < 0) {
            logioMsg(old->lp, LOG_ERR, "%s: table '%s' dbgetv '%s' failed", fid, OLD_SITECHAN_TABLE_NAME, OLD_SITECHAN_EDEPTH);
            free(old->sitechan);
            return FALSE;
        }
        if (dbgetv(old->table.sitechan, 0, OLD_SITECHAN_HANG, &old->sitechan[i].hang, 0) < 0) {
            logioMsg(old->lp, LOG_ERR, "%s: table '%s' dbgetv '%s' failed", fid, OLD_SITECHAN_TABLE_NAME, OLD_SITECHAN_HANG);
            free(old->sitechan);
            return FALSE;
        }
        if (dbgetv(old->table.sitechan, 0, OLD_SITECHAN_VANG, &old->sitechan[i].vang, 0) < 0) {
            logioMsg(old->lp, LOG_ERR, "%s: table '%s' dbgetv '%s' failed", fid, OLD_SITECHAN_TABLE_NAME, OLD_SITECHAN_VANG);
            free(old->sitechan);
            return FALSE;
        }
        if (dbgetv(old->table.sitechan, 0, OLD_SITECHAN_STA, old->sitechan[i].sta, 0) < 0) {
            logioMsg(old->lp, LOG_ERR, "%s: table '%s' dbgetv '%s' failed", fid, OLD_SITECHAN_TABLE_NAME, OLD_SITECHAN_STA);
            free(old->sitechan);
            return FALSE;
        }
        if (dbgetv(old->table.sitechan, 0, OLD_SITECHAN_CHAN, old->sitechan[i].chan, 0) < 0) {
            logioMsg(old->lp, LOG_ERR, "%s: table '%s' dbgetv '%s' failed", fid, OLD_SITECHAN_TABLE_NAME, OLD_SITECHAN_CHAN);
            free(old->sitechan);
            return FALSE;
        }
        if (dbgetv(old->table.sitechan, 0, OLD_SITECHAN_CTYPE, old->sitechan[i].ctype, 0) < 0) {
            logioMsg(old->lp, LOG_ERR, "%s: table '%s' dbgetv '%s' failed", fid, OLD_SITECHAN_TABLE_NAME, OLD_SITECHAN_CTYPE);
            free(old->sitechan);
            return FALSE;
        }
        if (dbgetv(old->table.sitechan, 0, OLD_SITECHAN_DESCRIP, old->sitechan[i].descrip, 0) < 0) {
            logioMsg(old->lp, LOG_ERR, "%s: table '%s' dbgetv '%s' failed", fid, OLD_SITECHAN_TABLE_NAME, OLD_SITECHAN_DESCRIP);
            free(old->sitechan);
            return FALSE;
        }
        old->sitechan[i].begt = dccdbOndateToBegt(old->sitechan[i].ondate);
        old->sitechan[i].endt = dccdbOffdateToEndt(old->sitechan[i].offdate);
    }

    return TRUE;
}

static BOOL ReadStage(OLD *old)
{
int i;
static char *fid = "OpenOldDatabase:ReadStage";

    if (dbquery(old->table.stage, dbRECORD_COUNT, &old->nstage) < 0) {
        logioMsg(old->lp, LOG_ERR, "%s: dbquery failed", fid);
        return FALSE;
    }

    if ((old->stage = (OLD_STAGE *) malloc(sizeof(OLD_STAGE) * old->nstage)) == NULL) {
        logioMsg(old->lp, LOG_ERR, "%s: malloc failed: %s", fid, strerror(errno));
        return FALSE;
    }

    for (i = 0; i < old->nstage; i++) {
        old->table.stage.record = i;
        if (dbgetv(old->table.stage, 0, OLD_STAGE_TIME, &old->stage[i].time, 0) < 0) {
            logioMsg(old->lp, LOG_ERR, "%s: table '%s' dbgetv '%s' failed", fid, OLD_STAGE_TABLE_NAME, OLD_STAGE_TIME);
            free(old->stage);
            return FALSE;
        }
        if (dbgetv(old->table.stage, 0, OLD_STAGE_ENDTIME, &old->stage[i].endtime, 0) < 0) {
            logioMsg(old->lp, LOG_ERR, "%s: table '%s' dbgetv '%s' failed", fid, OLD_STAGE_TABLE_NAME, OLD_STAGE_ENDTIME);
            free(old->stage);
            return FALSE;
        }
        if (dbgetv(old->table.stage, 0, OLD_STAGE_LDDATE, &old->stage[i].lddate, 0) < 0) {
            logioMsg(old->lp, LOG_ERR, "%s: table '%s' dbgetv '%s' failed", fid, OLD_STAGE_TABLE_NAME, OLD_STAGE_LDDATE);
            free(old->stage);
            return FALSE;
        }
        if (dbgetv(old->table.stage, 0, OLD_STAGE_STAGEID, &old->stage[i].stageid, 0) < 0) {
            logioMsg(old->lp, LOG_ERR, "%s: table '%s' dbgetv '%s' failed", fid, OLD_STAGE_TABLE_NAME, OLD_STAGE_STAGEID);
            free(old->stage);
            return FALSE;
        }
        if (dbgetv(old->table.stage, 0, OLD_STAGE_GNOM, &old->stage[i].gnom, 0) < 0) {
            logioMsg(old->lp, LOG_ERR, "%s: table '%s' dbgetv '%s' failed", fid, OLD_STAGE_TABLE_NAME, OLD_STAGE_GNOM);
            free(old->stage);
            return FALSE;
        }
        if (dbgetv(old->table.stage, 0, OLD_STAGE_GCALIB, &old->stage[i].gcalib, 0) < 0) {
            logioMsg(old->lp, LOG_ERR, "%s: table '%s' dbgetv '%s' failed", fid, OLD_STAGE_TABLE_NAME, OLD_STAGE_GCALIB);
            free(old->stage);
            return FALSE;
        }
        if (dbgetv(old->table.stage, 0, OLD_STAGE_IZERO, &old->stage[i].izero, 0) < 0) {
            logioMsg(old->lp, LOG_ERR, "%s: table '%s' dbgetv '%s' failed", fid, OLD_STAGE_TABLE_NAME, OLD_STAGE_IZERO);
            free(old->stage);
            return FALSE;
        }
        if (dbgetv(old->table.stage, 0, OLD_STAGE_DECIFAC, &old->stage[i].decifac, 0) < 0) {
            logioMsg(old->lp, LOG_ERR, "%s: table '%s' dbgetv '%s' failed", fid, OLD_STAGE_TABLE_NAME, OLD_STAGE_DECIFAC);
            free(old->stage);
            return FALSE;
        }
        if (dbgetv(old->table.stage, 0, OLD_STAGE_SAMPRATE, &old->stage[i].samprate, 0) < 0) {
            logioMsg(old->lp, LOG_ERR, "%s: table '%s' dbgetv '%s' failed", fid, OLD_STAGE_TABLE_NAME, OLD_STAGE_SAMPRATE);
            free(old->stage);
            return FALSE;
        }
        if (dbgetv(old->table.stage, 0, OLD_STAGE_LEADFAC, &old->stage[i].leadfac, 0) < 0) {
            logioMsg(old->lp, LOG_ERR, "%s: table '%s' dbgetv '%s' failed", fid, OLD_STAGE_TABLE_NAME, OLD_STAGE_LEADFAC);
            free(old->stage);
            return FALSE;
        }
        if (dbgetv(old->table.stage, 0, OLD_STAGE_STA, old->stage[i].sta, 0) < 0) {
            logioMsg(old->lp, LOG_ERR, "%s: table '%s' dbgetv '%s' failed", fid, OLD_STAGE_TABLE_NAME, OLD_STAGE_STA);
            free(old->stage);
            return FALSE;
        }
        if (dbgetv(old->table.stage, 0, OLD_STAGE_CHAN, old->stage[i].chan, 0) < 0) {
            logioMsg(old->lp, LOG_ERR, "%s: table '%s' dbgetv '%s' failed", fid, OLD_STAGE_TABLE_NAME, OLD_STAGE_CHAN);
            free(old->stage);
            return FALSE;
        }
        if (dbgetv(old->table.stage, 0, OLD_STAGE_SSIDENT, old->stage[i].ssident, 0) < 0) {
            logioMsg(old->lp, LOG_ERR, "%s: table '%s' dbgetv '%s' failed", fid, OLD_STAGE_TABLE_NAME, OLD_STAGE_SSIDENT);
            free(old->stage);
            return FALSE;
        }
        if (dbgetv(old->table.stage, 0, OLD_STAGE_IUNITS, old->stage[i].iunits, 0) < 0) {
            logioMsg(old->lp, LOG_ERR, "%s: table '%s' dbgetv '%s' failed", fid, OLD_STAGE_TABLE_NAME, OLD_STAGE_IUNITS);
            free(old->stage);
            return FALSE;
        }
        if (dbgetv(old->table.stage, 0, OLD_STAGE_OUNITS, old->stage[i].ounits, 0) < 0) {
            logioMsg(old->lp, LOG_ERR, "%s: table '%s' dbgetv '%s' failed", fid, OLD_STAGE_TABLE_NAME, OLD_STAGE_OUNITS);
            free(old->stage);
            return FALSE;
        }
        if (dbgetv(old->table.stage, 0, OLD_STAGE_GTYPE, old->stage[i].gtype, 0) < 0) {
            logioMsg(old->lp, LOG_ERR, "%s: table '%s' dbgetv '%s' failed", fid, OLD_STAGE_TABLE_NAME, OLD_STAGE_GTYPE);
            free(old->stage);
            return FALSE;
        }
        if (dbgetv(old->table.stage, 0, OLD_STAGE_DIR, old->stage[i].dir, 0) < 0) {
            logioMsg(old->lp, LOG_ERR, "%s: table '%s' dbgetv '%s' failed", fid, OLD_STAGE_TABLE_NAME, OLD_STAGE_DIR);
            free(old->stage);
            return FALSE;
        }
        if (dbgetv(old->table.stage, 0, OLD_STAGE_DFILE, old->stage[i].dfile, 0) < 0) {
            logioMsg(old->lp, LOG_ERR, "%s: table '%s' dbgetv '%s' failed", fid, OLD_STAGE_TABLE_NAME, OLD_STAGE_DFILE);
            free(old->stage);
            return FALSE;
        }
    }

    return TRUE;
}

static void InitDB(OLD *old, LOGIO *lp)
{
    old->lp = lp;

    old->instrument = NULL;
    old->ninstrument = 0;

    old->schanloc = NULL;
    old->nschanloc = 0;

    old->seedloc = NULL;
    old->nseedloc = 0;

    old->sensor = NULL;
    old->nsensor = 0;

    old->site = NULL;
    old->nsite = 0;

    old->sitechan = NULL;
    old->nsitechan = 0;

    old->stage = NULL;
    old->nstage = 0;
}

OLD *OpenOldDatabase(char *dbpath, LOGIO *lp, BOOL CheckSanity)
{
OLD *old;
static char *fid = "OpenOldDatabase";

    if ((old = (OLD *) malloc(sizeof(OLD))) == NULL) {
        logioMsg(lp, LOG_ERR, "%s: malloc: %s", fid, strerror(errno));
        return NULL;
    }
    InitDB(old, lp);

    if (dbopen(dbpath, DCCDB_READONLY, &old->table.all) != 0) {
        logioMsg(old->lp, LOG_ERR, "%s: dbopen: ERROR: %s\n", fid, dbpath);
        return FreeOldDB(old);
    }

    old->table.instrument  = dblookup(old->table.all, 0, OLD_INSTRUMENT_TABLE_NAME, 0, 0);
    old->table.seedloc     = dblookup(old->table.all, 0, OLD_SEEDLOC_TABLE_NAME,    0, 0);
    old->table.sensor      = dblookup(old->table.all, 0, OLD_SENSOR_TABLE_NAME,     0, 0);
    old->table.site        = dblookup(old->table.all, 0, OLD_SITE_TABLE_NAME,       0, 0);
    old->table.sitechan    = dblookup(old->table.all, 0, OLD_SITECHAN_TABLE_NAME,   0, 0);
    old->table.stage       = dblookup(old->table.all, 0, OLD_STAGE_TABLE_NAME,   0, 0);

    if (!ReadInstrument(old) ) return FreeOldDB(old);
    if (!ReadSeedloc(old)    ) return FreeOldDB(old);
    if (!ReadSensor(old)     ) return FreeOldDB(old);
    if (!ReadSite(old)       ) return FreeOldDB(old);
    if (!ReadSitechan(old)   ) return FreeOldDB(old);
    if (!ReadStage(old)      ) return FreeOldDB(old);
    LogMsg(1, "Input Database '%s' read OK\n", dbpath);

    if (CheckSanity && !SanityCheck(old)) return FreeOldDB(old);

    olddb = old;
    return old;
}

/* Revision History
 *
 * $Log: old.c,v $
 * Revision 1.2  2015/03/06 21:44:14  dechavez
 * initial production release
 *
 */
