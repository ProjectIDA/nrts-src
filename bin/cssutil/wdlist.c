#pragma ident "$Id: wdlist.c,v 1.5 2015/12/08 20:43:40 dechavez Exp $"
/*======================================================================
 *
 *  List the contents of CSS wfdisc file.
 *
 *====================================================================*/
#include "util.h"
#include "cssio.h"

static void wdlist(fp)
FILE *fp;
{
struct cssio_wfdisc wfdisc;
int count = 0;
char tmpbuf[64];

    while (rwfdrec(fp, &wfdisc) == 0) {
        count++;
        printf("Record %3d: ", count);

        printf("jdate    = ");
        if (wfdisc.jdate == cssio_wfdisc_null.jdate) 
            puts("(css null)");
        else
            printf("%d\n",wfdisc.jdate);

        printf("            time     = ");
        if (wfdisc.time == cssio_wfdisc_null.time)
            puts("(css null)");
        else {
            printf("%lf (%s)\n",wfdisc.time, utilDttostr(wfdisc.time, 0, tmpbuf));
        }

        printf("            endtime  = ");
        if (wfdisc.endtime == cssio_wfdisc_null.endtime)
            puts("(css null)");
        else {
            printf("%lf (%s)\n",wfdisc.endtime, utilDttostr(wfdisc.endtime, 0, tmpbuf));
        }

        printf("            sta      = ");
        if (strcmp(wfdisc.sta, cssio_wfdisc_null.sta) == 0)
            puts("(css null)");
        else
            printf("%s\n",wfdisc.sta);

        printf("            chan     = ");
        if (strcmp(wfdisc.chan, cssio_wfdisc_null.chan) == 0)
            puts("(css null)");
        else
            printf("%s\n",wfdisc.chan);

        printf("            nsamp    = ");
        if (wfdisc.nsamp == cssio_wfdisc_null.nsamp)
            puts("(css null)");
        else
            printf("%d\n",wfdisc.nsamp);

        printf("            smprate  = ");
        if (wfdisc.smprate == cssio_wfdisc_null.smprate)
            puts("(css null)");
        else
            printf("%f\n",wfdisc.smprate);

        printf("            calib    = ");
        if (wfdisc.calib == cssio_wfdisc_null.calib)
            puts("(css null)");
        else
            printf("%f\n",wfdisc.calib);

        printf("            calper   = ");
        if (wfdisc.calper == cssio_wfdisc_null.calper)
            puts("(css null)");
        else
            printf("%f\n",wfdisc.calper);

        printf("            instype  = ");
        if (strcmp(wfdisc.instype, cssio_wfdisc_null.instype) == 0)
            puts("(css null)");
        else
            printf("%s\n",wfdisc.instype);

        printf("            segtype  = ");
        if (wfdisc.segtype == cssio_wfdisc_null.segtype)
            puts("(css null)");
        else
            printf("%c\n",wfdisc.segtype);

        printf("            datatype = ");
        if (strcmp(wfdisc.datatype, cssio_wfdisc_null.datatype) == 0)
            puts("(css null)");
        else
            printf("%s\n",wfdisc.datatype);

        printf("            clip     = ");
        if (wfdisc.clip == cssio_wfdisc_null.clip)
            puts("(css null)");
        else
            printf("%c\n",wfdisc.clip);

        printf("            chanid   = ");
        if (wfdisc.chanid == cssio_wfdisc_null.chanid)
            puts("(css null)");
        else
            printf("%d\n",wfdisc.chanid);

        printf("            wfid     = ");
        if (wfdisc.wfid == cssio_wfdisc_null.wfid)
            puts("(css null)");
        else
            printf("%d\n",wfdisc.wfid);

        printf("            dir      = ");
        if (strcmp(wfdisc.dir, cssio_wfdisc_null.dir) == 0)
            puts("(css null)");
        else
            printf("%s\n",wfdisc.dir);

        printf("            dfile    = ");
        if (strcmp(wfdisc.dfile, cssio_wfdisc_null.dfile) == 0)
            puts("(css null)");
        else
            printf("%s\n",wfdisc.dfile);

        printf("            foff     = %d\n", wfdisc.foff);

        printf("            commid   = ");
        if (wfdisc.commid == cssio_wfdisc_null.commid)
            puts("(css null)");
        else
            printf("%d\n",wfdisc.commid);

        printf("            lddate   = ");
        if (strcmp(wfdisc.lddate, cssio_wfdisc_null.lddate) == 0)
            puts("(css null)");
        else
            printf("%s\n",wfdisc.lddate);

        printf("\n");
    }
}

int main(int argc, char **argv)
{
FILE *fp;
char *fname;
int i;

    if (argc > 1) {
        for (i = 1; i < argc; i++) {
            fname = argv[i];
            if ((fp = fopen(fname, "r")) == NULL) {
                perror(fname);
            } else {
                printf("File: %s\n", fname);
                wdlist(fp);
                fclose(fp);
            }
        }
    } else {
        wdlist(stdin);
    }
}

/* Revision History
 *
 * $Log: wdlist.c,v $
 * Revision 1.5  2015/12/08 20:43:40  dechavez
 * various minor tweaks to get clean Mac OS X builds
 *
 * Revision 1.4  2011/03/17 20:47:38  dechavez
 * use the new CSSIO_, cssio_, prefixed names from libcssio 2.2.0
 *
 * Revision 1.3  2004/07/22 22:26:09  dechavez
 * removed unused code
 *
 * Revision 1.2  2003/12/10 06:15:38  dechavez
 * various superficial changes in order to calm solaris cc
 *
 * Revision 1.1.1.1  2000/02/08 20:19:57  dec
 * import existing IDA/NRTS sources
 *
 */
