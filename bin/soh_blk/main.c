#pragma ident "$Id: main.c,v 1.1 2014/04/16 15:34:12 dechavez Exp $"
/*======================================================================
 *
 *  soh_blk
 *
 *  Main for creating seed header blockettes for SOH channels.
 *
 *  Reads lists of station/channel/loccode combinations from stdin,
 *  creates subdirs sta/chan.loccode and writes a B052 file in that
 *  dir.
 *
 *====================================================================*/
#include "soh_blk.h"

#define INCBUF { sprintf(&blk[charcnt], "%s", line); charcnt += strlen(line); }

static int B052(FILE *fp, char *chan, char *loccode, char *starttime, char *endtime, REAL32 r_lat, REAL32 r_long, REAL32 r_elev, REAL32 r_depth)
{
int  charcnt = 0;
char line[128];
char year[8], day[8];
char blk[MAXBLKLEN];

	sprintf(line, "%s", loccode); INCBUF;
	sprintf(line, "%s", chan); INCBUF;
	sprintf(line, "   0"); INCBUF;
	sprintf(line, "%3d", Q330 ); INCBUF;
	sprintf(line, "S/N #%s~", "-"); INCBUF; /* leave hyphenated for now */
	if (strcmp(chan, "LCE") == 0) {
        sprintf(line, "%3d", LCEUNIT); INCBUF;
    } else {
        sprintf(line, "%3d", LCQUNIT); INCBUF;
    }
    sprintf(line, "%3d", COUNTS); INCBUF;   
    sprintf(line, "%10.6f", r_lat); INCBUF; /* latitude stub */
    sprintf(line, "%11.6f", r_long); INCBUF; /* longitude stub */
    sprintf(line, "%7.1f", r_elev * 1000);  INCBUF; /* elevation stub */
    sprintf(line, "%5.1f", r_depth * 1000); INCBUF; /* depth stub */
    sprintf(line, "%5.1f", 0); INCBUF;
    sprintf(line, "%5.1f", 0); INCBUF;
    sprintf(line, "%4d", STEIMCOMP2); INCBUF;
    sprintf(line, "%2d", DATARECLEN); INCBUF;
    sprintf(line, "%10.4E", 1.0); INCBUF;
    sprintf(line, "%10.4E", 0.01); INCBUF;
    sprintf(line, "0000"); INCBUF;
    sprintf(line, "%s", "H~" ); INCBUF; /* channel flag for SOH */
    sprintf(line, "%s,00:00:00.000~", starttime); INCBUF;
    if (strcmp(endtime, "-" ) == 0) {
        sprintf(line, "~"); INCBUF;
    } else {
        sprintf(line, "%s,00:00:00.000~", endtime); INCBUF;
    }
    sprintf(line, "N"); INCBUF;
    fprintf(fp, "052");
    fprintf(fp, "%4d", strlen(blk) + 7);
    fprintf(fp, "%s", blk);
    fprintf(fp, "\n");

    return(0);
}

int main(int argc, char **argv)
{
char    dname[128];
char    fname[128];
char    stat[8], chan[8], loccode[4], starttime[16], endtime[16];
char    line[128];
REAL32  r_lat, r_long, r_elev, r_depth;
FILE    *fp;

#define DIR_PERMS 0775

    while ( (fgets(line, 127, stdin)) != NULL ) {

        if (line[0] == '#') continue; /* ingore comment lines */
        sscanf(line, "%s %s %s %s %s %f %f %f %f", &stat, &chan, &loccode, &starttime, &endtime, &r_lat, &r_long, &r_elev, &r_depth);
        util_ucase(chan);
        util_ucase(loccode);

/* create channel.loc dir: */

        sprintf(dname, "%s.II/%s.%s", stat, chan, loccode);
        if (util_mkpath(dname, DIR_PERMS) != 0) {
            fprintf(stderr, "util_mkpath: ");
            perror(dname);
            exit(1);
        }

/* open a B052 file for each */

        sprintf(fname, "%s.II/%s.%s/B052", stat, chan, loccode );
        if ((fp = fopen(fname, "w")) == NULL) {
            fprintf(stderr, "fopen: ");
            perror(fname);
            exit(1);
        }

/* write the B052 blockette information to this file: */

        B052(fp, chan, loccode, starttime, endtime, r_lat, r_long, r_elev, r_depth);

/* write the accompanying B054, 057, and 058 lines: */

        if (strcmp(chan, "LCE") == 0) {
            fprintf(fp, "054  24D01%03d%03d   0   0\n", LCEUNIT, COUNTS);
        } else {
            fprintf(fp, "054  24D01%03d%03d   0   0\n", LCQUNIT, COUNTS);
        }

        fprintf(fp, "057  51011.0000E+00    1    0+0.0000E+00+0.0000E+00\n");

        if (strcmp(chan, "LCE" ) == 0) {
            fprintf(fp, "058  3501+1.00000E+06+0.00000E+00 0\n");
            fprintf(fp, "058  3500+1.00000E+06+0.00000E+00 0\n");
        } else {
            fprintf(fp, "058  3501+1.00000E+00+0.00000E+00 0\n");
            fprintf(fp, "058  3500+1.00000E+00+0.00000E+00 0\n");
        }
        fclose(fp);
    }

    exit(0);
}

/* Revision History
 *
 * $Log: main.c,v $
 * Revision 1.1  2014/04/16 15:34:12  dechavez
 * Created using code from the DCC
 *
 */
