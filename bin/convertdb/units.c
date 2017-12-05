#pragma ident "$Id: units.c,v 1.2 2015/03/06 21:44:15 dechavez Exp $"
/*======================================================================
 *
 *  Generate the units table
 *
 *====================================================================*/
#include "convertdb.h"

#define MY_MOD_ID MOD_UNITS

typedef struct {
    char *unit;
    char code;
    char *desc;
} UNITS_MAP;

static UNITS_MAP map[] = {
    {      "A", 1, "Amperes"},
    { "COUNTS", 2, "Digital Counts"},
    {      "M", 3, "Displacement in Meters"},
    {    "M/S", 4, "Velocity in Meters Per Second"},
    { "M/S**2", 5, "Acceleration in Meters Per Second Per Second"},
    {     "MS", 6, "Milliseconds"},
    {      "V", 7, "Volts"},
    {     "PA", 8, "Pressure in Pascals"},
    {      "1", 9, "Strain"},
    {      "T", 10, "Magnetic field in Teslas"},
    {   "USEC", 11, "Microseconds"},
    {"PERCENT", 12, "%"},
    {   "TILT", 13, "Radian"},
    {     NULL, 0, NULL}
};

char *LookupUnits(char *string)
{
int i, code;

    code = atoi(string);
    for (i = 0; map[i].unit != NULL; i++) if (map[i].code == code) return map[i].unit;
    return NULL;
}

void BuildUnits(OLD *old, DCCDB *new, char *prefix)
{
FILE *fp;
int i, count;
char path[MAXPATHLEN+1];
static char *fid = "BuildUnit";

    count = 0;
    while (map[++count].unit != NULL);

    new->nb34 = count;
    if ((new->b34 = (MSEED_B34 *) malloc (sizeof(MSEED_B34) * new->nb34)) == NULL) {
        LogErr("%s: malloc: %s", fid, strerror(errno));
        GracefulExit(MY_MOD_ID + 2);
    }

    for (i = 0; i < count; i++) {
        new->b34[i].code = map[i].code;
        strncpy(new->b34[i].unit, map[i].unit, MSEED_B34_UNIT_LEN + 1);
        strncpy(new->b34[i].desc, map[i].desc, MSEED_B34_DESC_LEN + 1);
    }

    sprintf(path, "%s.%s", prefix, DCCDB_UNITS_TABLE_NAME);
    if ((fp = fopen(path, "w")) == NULL) {
        fprintf(stderr, "fopen: ");
        perror(path);
        GracefulExit(MY_MOD_ID + 6);
    }

    for (i = 0; i < new->nb34; i++) dccdbPrintUnitsRecord(fp, &new->b34[i]);
    fclose(fp);
    LogMsg(1, "%s created OK\n", path);
}

/* Revision History
 *
 * $Log: units.c,v $
 * Revision 1.2  2015/03/06 21:44:15  dechavez
 * initial production release
 *
 */
