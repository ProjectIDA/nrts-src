#pragma ident "$Id: main.c,v 1.2 2015/11/18 20:25:51 dechavez Exp $"
/*======================================================================
 *
 *  idaresponse.c
 *
 *  Data are convolved with instrument response if +/-c option is specified.
 *  Data are deconvolved if +/-d option is specified.
 *
 *====================================================================*/
#include "idaresponse.h"

BOOL verbose = FALSE;

static void help(char *myname)
{
    fprintf(stderr, "%s %s\n", myname, VersionIdentString);
    fprintf(stderr, "usage: %s inwfd=string outwfd=string [+/-c +/-d] [acc|disp] [ida] [s1] [file=path] [res_limit=real64] [verbose] \n", myname);
    fprintf(stderr, "\n");
    fprintf(stderr, "where:\n");
    fprintf(stderr, "     inwfd -> input wfdisc prefix\n");
    fprintf(stderr, "    outwfd -> output wfdisc prefix\n");
    fprintf(stderr, "        +c -> apply instrument response\n");
    fprintf(stderr, "        +d -> remove instrument response\n");
    fprintf(stderr, "       ida -> use program 'staresp' to calculate response (default is 'evalresp')\n");
    fprintf(stderr, "      file -> specify response file (default is %s)\n", DEFAULT_RESP_PATH);
    fprintf(stderr, " res_limit -> places a resolution limit on dyn range\n");
    fprintf(stderr, "       acc -> use instrument acceleration response\n");
    fprintf(stderr, "      disp -> use instrument displacement response\n");
    fprintf(stderr, "        s1 -> output just stage 1 (requires evalresp)\n");
    fprintf(stderr, "   verbose -> print out the command line of the response program invoked\n");
    fprintf(stderr, "\n");
    fprintf(stderr, "If neither acc nor disp is stated, instrument velocity response is assumed.\n");
    fprintf(stderr, "For reasons not understood, choosing acc for an *G*\n");
    fprintf(stderr, "     channel does not work for the non-ida response.\n");
    exit(1);
}

static int wfdisc_cmp(const void *pa, const void *pb)
{
WFDISC *a, *b;
int cnd;

    a = (WFDISC *) pa;
    b = (WFDISC *) pb;

    if ((cnd = strcmp(a->dir,   b->dir)))   return(cnd); /* Sort on dir */
    if ((cnd = strcmp(a->dfile, b->dfile))) return(cnd); /* Sort on file */
    if ((cnd = a->jdate - b->jdate))        return(cnd); /* sort by jdate */
    return(-(b->foff - a->foff));                        /* Sort on foff */
}

int main(int argc, char **argv)
{
FILE   *fpwfin, *fpwfout;
char   *inpref, *outpref, tempstring[120];
char   *respath;
INT32  i, nrecs ;
BOOL   convolve, deconvolve, acc, displ, ida, s1only;
REAL64 res_limit = -1.0;
WFDISC *in_rec, out_rec;
static char *default_respath = DEFAULT_RESP_PATH;

/*  Set up defaults  */

    convolve = deconvolve = acc = displ = ida = s1only = FALSE;
    respath = inpref = outpref = NULL;

/*  Scan command line for overrides  */

    if (argc == 1) help(argv[0]);

    for (i = 1; i < argc; i++) {
        if (strcmp(argv[i], "+c") == 0) {
            convolve = TRUE;
        } else if (strcmp(argv[i], "-c") == 0) {
            convolve = FALSE;
        } else if (strcmp(argv[i], "+d") == 0) {
            deconvolve = TRUE;
        } else if (strcmp(argv[i], "-d") == 0) {
            deconvolve = FALSE;
        } else if (strcmp(argv[i], "ida") == 0) {
            ida = TRUE;
        } else if (strcmp(argv[i], "acc") == 0) {
            acc = TRUE;
        } else if (strcmp(argv[i], "disp") == 0) {
            displ = TRUE;
        } else if (strcmp(argv[i], "s1") == 0) {
            s1only = TRUE;
        } else if (strcmp(argv[i], "verbose") == 0) {
            verbose = TRUE;
        } else if (strncmp(argv[i], "res_limit=", strlen("res_limit=")) == 0) {
            res_limit = atof( argv[i] + strlen("res_limit=") );
        } else if (strncmp(argv[i], "file=", strlen("file=")) == 0) {
            respath = (char *) malloc(strlen(argv[i]) - strlen("file=") + 2);
            strcpy(respath, argv[i] + strlen("file="));
        } else if (strncmp(argv[i], "inwfd=", strlen("inwfd=")) == 0) {
            inpref = (char *) malloc(strlen(argv[i]) - strlen("inwfd=") + strlen(".wfdisc") + 2);
            strcpy(inpref, argv[i] + strlen("inwfd="));
        } else if (strncmp(argv[i], "outwfd=", strlen("outwfd=")) == 0) {
            outpref = (char *) malloc(strlen(argv[i]) - strlen("outwfd=") + strlen(".wfdisc") + 2);
            strcpy(outpref, argv[i] + strlen("outwfd="));
        } else {
            fprintf(stderr,"unrecognized argument '%s'\n", argv[i]);
            help(argv[0]);
        }
    }

    if (inpref == NULL || outpref == NULL) {
        fprintf(stderr,"%s: input and output prefixes must be specified\n", argv[0]);
        help(argv[0]);
    }

    if (strcmp(inpref, outpref) == 0) {
        fprintf(stderr,"%s: input and output prefixes must differ\n", argv[0]);
        help(argv[0]);
    }

    if (ida && s1only) {
        fprintf(stderr, "%s: use of s1 option precludes ida option\n", argv[0]);
        help(argv[0]);
    }

    if (respath == NULL) respath = default_respath;

    if (convolve && deconvolve) {
        fprintf(stderr, "%s: can only use one option +c or +d, not both\n", argv[0]);
        help(argv[0]);
    }

/*  Read in entire wfdisc, and sort by path name and foff */

    sprintf(tempstring, "%s.wfdisc", inpref);
    if ((fpwfin = fopen(tempstring, "r")) == NULL) {
        fprintf(stderr, "idaresponse: error opening input wfdisc\n");
        exit(2);
    }

    if ((nrecs = rwfdisc(fpwfin, &in_rec)) <= 0) {
        fprintf(stderr,"idaresponse: bad or empty wfdisc file\n");
        exit(2);
    }
    fclose(fpwfin);

    sprintf(tempstring, "%s.wfdisc", outpref);
    if ((fpwfout = fopen(tempstring, "w")) == NULL) {
        fprintf(stderr, "idaresponse: error opening output wfdisc\n");
        exit(2);
    }

    if (nrecs > 1) qsort(in_rec, nrecs, sizeof(WFDISC), wfdisc_cmp);

/* Process entire file, one record at a time */

    for (i = 0; i < nrecs; i++) {
        out_rec = in_rec[i];
        if (resp(&out_rec, convolve, deconvolve, acc, displ, ida, s1only, respath, res_limit, outpref) == 0) fprintf(fpwfout, "%s\n", cssioWfdiscString(&out_rec, NULL));
    }

    fclose(fpwfout);

    exit(0);
}

/* Revision History
 *
 * $Log: main.c,v $
 * Revision 1.2  2015/11/18 20:25:51  dechavez
 * added support for "s1" and "verbose" options
 *
 * Revision 1.1  2014/04/15 21:12:48  dechavez
 * initial release using sources modified from DCC version
 *
 */
