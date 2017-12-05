#pragma ident "$Id: wdecho.c,v 1.4 2014/04/15 20:58:45 dechavez Exp $"
/*======================================================================
 *
 *  Print the indicated wfdisc elements from the selected record.
 *
 *====================================================================*/
#include "cssio.h"


#define JULIAN_FORMAT 0
#define HUMAN_FORMAT  1
#define RAW_FORMAT    2

static void help(char *myname)
{
int i, j;
char *field; 

    fprintf(stderr, "usage: %s [ {-r | -j | -h} -o ] wfdisc_field_name ... < CSS_wfdisc\n", myname);
    fprintf(stderr, "\n");
    fprintf(stderr, "Options: -r => print raw (epcoch) time (eg. 526953600.000000)\n");
    fprintf(stderr, "         -j => print julian dates      (eg. 1986:256-00:00:00.000)\n");
    fprintf(stderr, "         -h => print human dates       (eg. Fri Sep 13 00:00:00 1986)\n");
    fprintf(stderr, "         -o => print out fields in the order specified\n");
    fprintf(stderr, "\n");
    fprintf(stderr, "         -r is assumed if no format option is specified\n");
    fprintf(stderr, "\n");
    fprintf(stderr, "The following field names are recognized:\n");

    for (i = 0, j = 0; (field = cssioFieldString(i)) != NULL; i++) {
        fprintf(stderr, "%-8s ", field); 
        if (++j % 8 == 0) fprintf(stderr, "\n");
    }
    fprintf(stderr, "\n");

    exit(0);
}

static void PrintTimeStamp(double tstamp, int format)
{
static char *NULL_TIME = "(css null)";

    if (format != RAW_FORMAT && tstamp == cssio_wfdisc_null.time) {
        printf("%s ", NULL_TIME);
        return;
    }

    switch (format) {
      case HUMAN_FORMAT:  printf("'%s' ", utilDttostr(tstamp, 10, NULL)); break;
      case JULIAN_FORMAT: printf("%s ",   utilDttostr(tstamp,  0, NULL)); break;
      default:
        printf("%.6lf ", tstamp);
    }
}

int main(int argc, char **argv)
{
LNKLST *list;
LNKLST_NODE *crnt;
int i, index, format = RAW_FORMAT;
WFDISC wd;
BOOL match[CSSIO_NUMFIELD], UserOrder = FALSE;

    if ((list = listCreate()) == NULL) {
        perror("listCreate");
        exit(1);
    }

    if (argc < 2) help(argv[0]);

    for (i = 0; i < CSSIO_NUMFIELD; i++) match[i] = FALSE;

    for (i = 1; i < argc; i++) {
        if (strcasecmp(argv[i], "-help") == 0) {
            help(argv[0]);
        } else if (strcasecmp(argv[i], "-h") == 0) {
            format = HUMAN_FORMAT;
        } else if (strcasecmp(argv[i], "-j") == 0) {
            format = JULIAN_FORMAT;
        } else if (strcasecmp(argv[i], "-r") == 0) {
            format = RAW_FORMAT;
        } else if (strcasecmp(argv[i], "-o") == 0) {
            UserOrder = TRUE;
        } else if ((index = cssioFieldCode(argv[i])) < 0) {
            fprintf(stderr, "unrecognized argument '%s'\n", argv[i]);
            help(argv[0]);
        } else {
            match[index] = TRUE;
            if (!listAppend(list, &index, sizeof(int))) {
                perror("listAppend");
                exit(1);
            }
        }
    }

/* If printing out in wfdisc order */

    if (!UserOrder) {
        while (rwfdrec(stdin, &wd) == 0) {
            if (match[CSSIO_FIELD_STA     ]) printf("%s ",   wd.sta     );
            if (match[CSSIO_FIELD_CHAN    ]) printf("%s ",   wd.chan    );
            if (match[CSSIO_FIELD_TIME    ]) PrintTimeStamp(wd.time, format);
            if (match[CSSIO_FIELD_WFID    ]) printf("%d ",   wd.wfid    );
            if (match[CSSIO_FIELD_CHANID  ]) printf("%d ",   wd.chanid  );
            if (match[CSSIO_FIELD_JDATE   ]) printf("%d ",   wd.jdate   );
            if (match[CSSIO_FIELD_ENDTIME ]) PrintTimeStamp(wd.endtime, format);
            if (match[CSSIO_FIELD_NSAMP   ]) printf("%d ",   wd.nsamp   );
            if (match[CSSIO_FIELD_SMPRATE ]) printf("%f ",   wd.smprate );
            if (match[CSSIO_FIELD_CALIB   ]) printf("%f ",   wd.calib   );
            if (match[CSSIO_FIELD_CALPER  ]) printf("%f ",   wd.calper  );
            if (match[CSSIO_FIELD_INSTYPE ]) printf("%s ",   wd.instype );
            if (match[CSSIO_FIELD_SEGTYPE ]) printf("%c ",   wd.segtype );
            if (match[CSSIO_FIELD_DATATYPE]) printf("%s ",   wd.datatype);
            if (match[CSSIO_FIELD_CLIP    ]) printf("%c ",   wd.clip    );
            if (match[CSSIO_FIELD_DIR     ]) printf("%s ",   wd.dir     );
            if (match[CSSIO_FIELD_DFILE   ]) printf("%s ",   wd.dfile   );
            if (match[CSSIO_FIELD_FOFF    ]) printf("%d ",   wd.foff    );
            if (match[CSSIO_FIELD_COMMID  ]) printf("%d ",   wd.commid  );
            if (match[CSSIO_FIELD_LDDATE  ]) printf("%s ",   wd.lddate  );
            if (match[CSSIO_FIELD_PATH    ]) printf("%s/%s", wd.dir, wd.dfile);
            printf("\n");
        }
    } else {
        while (rwfdrec(stdin, &wd) == 0) {
            crnt = listFirstNode(list);
            while (crnt != NULL) {
                switch (*((int *) crnt->payload)) {
                  case CSSIO_FIELD_STA:      printf("%s ",   wd.sta     );       break;
                  case CSSIO_FIELD_CHAN:     printf("%s ",   wd.chan    );       break;
                  case CSSIO_FIELD_TIME:     PrintTimeStamp(wd.time, format);    break;
                  case CSSIO_FIELD_WFID:     printf("%d ",   wd.wfid    );       break;
                  case CSSIO_FIELD_CHANID:   printf("%d ",   wd.chanid  );       break;
                  case CSSIO_FIELD_JDATE:    printf("%d ",   wd.jdate   );       break;
                  case CSSIO_FIELD_ENDTIME:  PrintTimeStamp(wd.endtime, format); break;
                  case CSSIO_FIELD_NSAMP:    printf("%d ",   wd.nsamp   );       break;
                  case CSSIO_FIELD_SMPRATE:  printf("%f ",   wd.smprate );       break;
                  case CSSIO_FIELD_CALIB:    printf("%f ",   wd.calib   );       break;
                  case CSSIO_FIELD_CALPER:   printf("%f ",   wd.calper  );       break;
                  case CSSIO_FIELD_INSTYPE:  printf("%s ",   wd.instype );       break;
                  case CSSIO_FIELD_SEGTYPE:  printf("%c ",   wd.segtype );       break;
                  case CSSIO_FIELD_DATATYPE: printf("%s ",   wd.datatype);       break;
                  case CSSIO_FIELD_CLIP:     printf("%c ",   wd.clip    );       break;
                  case CSSIO_FIELD_DIR:      printf("%s ",   wd.dir     );       break;
                  case CSSIO_FIELD_DFILE:    printf("%s ",   wd.dfile   );       break;
                  case CSSIO_FIELD_FOFF:     printf("%d ",   wd.foff    );       break;
                  case CSSIO_FIELD_COMMID:   printf("%d ",   wd.commid  );       break;
                  case CSSIO_FIELD_LDDATE:   printf("%s ",   wd.lddate  );       break;
                  case CSSIO_FIELD_PATH:     printf("%s/%s", wd.dir, wd.dfile);  break;
                }
                crnt = listNextNode(crnt);
            }
            printf("\n");
        }

/* printing out in user specified order */

    }

    exit(0);
}

/* Revision History
 *
 * $Log: wdecho.c,v $
 * Revision 1.4  2014/04/15 20:58:45  dechavez
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
