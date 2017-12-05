#pragma ident "$Id: abbrev.c,v 1.2 2015/03/06 21:44:14 dechavez Exp $"
/*======================================================================
 *
 *  Generate the abbrev table
 *
 *====================================================================*/
#include "convertdb.h"

#define MY_MOD_ID MOD_ABBREV

typedef struct {
    char *item;
    char *desc;
} ABBREV_MAP;

static ABBREV_MAP map[] = {
    {    "II", "(GSN) Global Seismograph Network (IRIS/IDA)"     },
    { "ASKHP", "Askania horizontal pendulum"                     },
    {"MAG-03", "Bartington MAG-03MSESL100 magnetometer"          },
    {"STRAIN", "Cambridge Invar wire strainmeter"                },
    {"SG-056", "GWR Dual Sphere Superconducting Gravimeter"      },
    {  "GS13", "Geotech GS-13 Seismometer"                       },
    {"K54000", "Geotech KS-54000 Borehole Seismometer"           },
    { "3ESPC", "Guralp CMG-3ESP Compact Seismometer"             },
    { "CMG3T", "Guralp CMG-3T Seismometer"                       },
    {  "CMG1", "Guralp CMG1 Seismometer"                         },
    {  "TILT", "Horsfall long-baseline fluid tilt"               },
    {"FBAEST", "Kinemetrics Episensor ES-T"                      },
    { "FBA23", "Kinemetrics FBA-23 Low-Gain Sensor"              },
    {"LRGRAV", "La Coste-Romberg Gravimeter"                     },
    {"TRI_PH", "Nanometrics Trillium 120 Posthole BB Seismometer"},
    { "TR240", "Nanometrics Trillium 240 Seismometer"            },
    {  "PARO", "Paroscientific microbarograph"                   },
    {"FLXMAG", "Rasmusen FGE Fluxgate magnetometer"              },
    {  "BM-G", "Relative Microbarograph, BM-G"                   },
    { "SETRA", "Setra Barometric Pressure Transducer model 270"  },
    {"STS1E3", "Streckeisen STS-1 Seismometer with Metrozet E300"},
    {"STS1HB", "Streckeisen STS-1H/VBB Seismometer"              },
    {"STS1VB", "Streckeisen STS-1V/VBB Seismometer"              },
    {  "STS2", "Streckeisen STS-2 Seismometer"                   },
    {"STS2_5", "Streckeisen STS-2.5 Seismometer"                 },
    {"STS-5A", "Streckeisen STS-5A Seismometer"                  },
    {"M2166V", "Metrozet M2166-VBV Seismometer"                  },
    {"M2166H", "Metrozet M2166-VBH Seismometer"                  },
    {     NULL, NULL                                             }
};

BOOL VerifyAbbrevItem(char *target)
{
int i;

    for (i = 0; map[i].item != NULL; i++) {
        if (strcmp(map[i].item, target) == 0) return TRUE;
    }

    return FALSE;
}

void BuildAbbrev(OLD *old, DCCDB *new, char *prefix)
{
FILE *fp;
int i, count;
char path[MAXPATHLEN+1];
static char *fid = "BuildAbbrev";

    count = 0;
    while (map[++count].item != NULL);

    new->nb33 = count;
    if ((new->b33 = (MSEED_B33 *) malloc (sizeof(MSEED_B33) * new->nb33)) == NULL) {
        LogErr("%s: malloc: %s", fid, strerror(errno));
        GracefulExit(MY_MOD_ID + 2);
    }

    for (i = 0; i < count; i++) {
        new->b33[i].code = i;
        strncpy(new->b33[i].item, map[i].item, MSEED_B33_ITEM_LEN + 1);
        strncpy(new->b33[i].desc, map[i].desc, MSEED_B33_DESC_LEN + 1);
    }

    sprintf(path, "%s.%s", prefix, DCCDB_ABBREV_TABLE_NAME);
    if ((fp = fopen(path, "w")) == NULL) {
        fprintf(stderr, "fopen: ");
        perror(path);
        GracefulExit(MY_MOD_ID + 6);
    }

    for (i = 0; i < new->nb33; i++) dccdbPrintAbbrevRecord(fp, &new->b33[i]);
    fclose(fp);
    LogMsg(1, "%s created OK\n", path);
}

/* Revision History
 *
 * $Log: abbrev.c,v $
 * Revision 1.2  2015/03/06 21:44:14  dechavez
 * initial production release
 *
 */
