#pragma ident "$Id: wdedit.c,v 1.5 2015/12/08 20:43:40 dechavez Exp $"
/*======================================================================
 *
 *  Read a wfdisc from stdin and send to stdout a new wfdisc which 
 *  has some/all fields modifed as per the command line.
 *
 *====================================================================*/
#include "cssio.h"

static void help(char *myname)
{
int i, j;
char *field;

    fprintf(stderr, "usage: %s field=value ... < CSS_wfdisc\n", myname);
    fprintf(stderr, "\n");
    fprintf(stderr, "The following field names are recognized:\n");
    for (i = 0, j = 0; (field = cssioFieldString(i)) != NULL; i++) {
        fprintf(stderr, "%-8s ", field);
        if (++j % 8 == 0) fprintf(stderr, "\n");
    }
    fprintf(stderr, "\n\n");
    fprintf(stderr, "The 'path' option will set both 'dir' and 'dfile'.\n");
    fprintf(stderr, "In addition the following are also recognized: addtime=sec adddir=string\n");
    fprintf(stderr, "\n");
    fprintf(stderr, "where 'addtime' will cause the specified seconds to be added to 'time' and 'endtime'\n");
    fprintf(stderr, "and 'adddir' will prepend the specified string to 'dir'\n");

    exit(0);
}

static void SetDirDfile(WFDISC *wd, char *path)
{
static BOOL first = TRUE, absolute_path;
static int i, ntoken;
#define MAXCOMPONENTS 256
static char *token[MAXCOMPONENTS];
static char tmpstr[MAXPATHLEN];
static char dir[CSSIO_DIR_LEN+1];
static char dfile[CSSIO_DFILE_LEN+1];

    if (first) {
        absolute_path = (path[0] == '/') ? TRUE : FALSE;
        if ((ntoken = utilParse(path, token, "/", MAXCOMPONENTS, 0)) < 0) {
            perror("utilParse");
            exit(1);
        }

        if (ntoken == 1) {
            if (absolute_path) {
                sprintf(dir, "/");
            } else {
                sprintf(dir, ".");
            }
        } else {
            if (absolute_path) {
                sprintf(tmpstr, "/");
            } else {
                tmpstr[0] = 0;
            }
            for (i = 0; i < ntoken-2; i++) sprintf(tmpstr+strlen(tmpstr), "%s/", token[i]);
            sprintf(tmpstr+strlen(tmpstr), "%s", token[ntoken-2]); /* no trailing slash */
            strncpy(dir, tmpstr, CSSIO_DIR_LEN+1);
        }
        strncpy(dfile, token[ntoken-1], CSSIO_DFILE_LEN+1);
        first = FALSE;
    }

    sprintf(wd->dir, "%s", dir);
    sprintf(wd->dfile, "%s", dfile);
}

int main(int argc, char **argv)
{
int i;
double tfact;
char tmpstring[MAXPATHLEN+1];
struct cssio_wfdisc wd;

    if (argc < 2) help(argv[0]);

    while (rwfdrec(stdin, &wd) == 0) {
        for (i = 1; i < argc; i++) {

            if (strncmp(argv[i], "sta=", strlen("sta=")) == 0) {
                strcpy(wd.sta, argv[i]+strlen("sta="));

            } else if (strncmp(argv[i], "chan=", strlen("chan=")) == 0) {
                strcpy(wd.chan, argv[i]+strlen("chan="));

            } else if (strncmp(argv[i], "time=", strlen("time=")) == 0) {
                wd.time = atof(argv[i]+strlen("time="));

            } else if (strncmp(argv[i], "wfid=", strlen("wfid=")) == 0) {
                wd.wfid = atol(argv[i]+strlen("wfid="));

            } else if (strncmp(argv[i], "chanid=", strlen("chanid=")) == 0) {
                wd.chanid = atol(argv[i]+strlen("chanid="));

            } else if (strncmp(argv[i], "jdate=", strlen("jdate=")) == 0) {
                wd.jdate = atol(argv[i]+strlen("jdate="));

            } else if (strncmp(argv[i], "endtime=", strlen("endtime=")) == 0) {
                wd.endtime = atof(argv[i]+strlen("endtime="));

            } else if (strncmp(argv[i], "nsamp=", strlen("nsamp=")) == 0) {
                wd.nsamp = atol(argv[i]+strlen("nsamp="));

            } else if (strncmp(argv[i], "smprate=", strlen("smprate=")) == 0) {
                wd.smprate = atof(argv[i]+strlen("smprate="));

            } else if (strncmp(argv[i], "samprate=", strlen("samprate=")) == 0) {
                wd.smprate = atof(argv[i]+strlen("samprate="));

            } else if (strncmp(argv[i], "calib=", strlen("calib=")) == 0) {
                wd.calib = atof(argv[i]+strlen("calib="));

            } else if (strncmp(argv[i], "calper=", strlen("calper=")) == 0) {
                wd.calper = atof(argv[i]+strlen("calper="));

            } else if (strncmp(argv[i], "instype=", strlen("instype=")) == 0) {
                strcpy(wd.instype, argv[i]+strlen("instype="));

            } else if (strncmp(argv[i], "segtype=", strlen("segtype=")) == 0) {
                wd.segtype = (argv[i]+strlen("segtype="))[0];

            } else if (strncmp(argv[i], "datatype=",strlen("datatype=")) == 0) {
                strcpy(wd.datatype, argv[i]+strlen("datatype="));

            } else if (strncmp(argv[i], "clip=", strlen("clip=")) == 0) {
                wd.clip = (argv[i]+strlen("clip="))[0];

            } else if (strncmp(argv[i], "dir=", strlen("dir=")) == 0) {
                strcpy(wd.dir, argv[i]+strlen("dir="));

            } else if (strncmp(argv[i], "dfile=", strlen("dfile=")) == 0) {
                strcpy(wd.dfile, argv[i]+strlen("dfile="));

            } else if (strncmp(argv[i], "foff=", strlen("foff=")) == 0) {
                wd.foff = atol(argv[i]+strlen("foff="));

            } else if (strncmp(argv[i], "commid=", strlen("commid=")) == 0) {
                wd.commid = atol(argv[i]+strlen("commid="));

            } else if (strncmp(argv[i], "remark=", strlen("remark=")) == 0) {
                wd.commid = atol(argv[i]+strlen("remark="));

            } else if (strncmp(argv[i], "lddate=", strlen("lddate=")) == 0) {
                strcpy(wd.lddate, argv[i]+strlen("lddate="));

            } else if (strncmp(argv[i], "path=", strlen("path=")) == 0) {
                SetDirDfile(&wd, argv[i]+strlen("path="));

            } else if (strncmp(argv[i], "addtime=", strlen("addtime=")) == 0) {
                tfact = atof(argv[i]+strlen("addtime="));
                wd.time += tfact;
                wd.endtime += tfact;

            } else if (strncmp(argv[i], "adddir=", strlen("adddir=")) == 0) {
                sprintf(tmpstring, "%s%s", argv[i] + strlen("adddir="), wd.dir);
                strcpy(wd.dir, tmpstring);

            } else {
                fprintf(stderr,"wdedit: unrecognized argument ");
                fprintf(stderr,"'%s' ignored\n",argv[i]);
            }
        }
        wwfdisc(stdout, &wd);
    }
    exit(0);
}

/* Revision History
 *
 * $Log: wdedit.c,v $
 * Revision 1.5  2015/12/08 20:43:40  dechavez
 * various minor tweaks to get clean Mac OS X builds
 *
 * Revision 1.4  2014/04/15 21:00:36  dechavez
 * made compatible with DCC version of the program with the same name
 *
 * Revision 1.3  2011/03/17 20:47:38  dechavez
 * use the new CSSIO_, cssio_, prefixed names from libcssio 2.2.0
 *
 * Revision 1.2  2003/12/10 06:15:38  dechavez
 * various superficial changes in order to calm solaris cc
 *
 * Revision 1.1.1.1  2000/02/08 20:19:57  dec
 * import existing IDA/NRTS sources
 *
 */
