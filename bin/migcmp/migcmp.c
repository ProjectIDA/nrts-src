/* @(#)migcmp.c    1.10  07/09/01  */
/*======================================================================
 *
 *  utilities/migcmp.c
 *
 *  Copy all the data in the wfdisc at stdin to the base directory,
 *  using Epoch path name convention.  New wfdisc file is sent to
 *  stdout.
 *
 *  Data is compressed if -c option is specified.  If data is already
 *  compressed, it is only copied.
 *
 *  Data is decompressed if -d option is specified.  If data is not
 *  compressed, it is only copied.
 *
 *====================================================================*/
#include <stdio.h>
#include <sys/param.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <time.h>
#include "csstime.h"
#include "wfdisc30io.h"
#include "util.h"
#include "misc.h"

#define MAX_DIRLEN 64

static char SccsId[] = "1.10  (07/09/01)";

extern int wfdisc_cmp();
extern int remark_cmp();
extern char *get_newdir();

char ipath[MAXPATHLEN], opath[MAXPATHLEN];

struct group {
    int nrec;
    struct wfdisc *wfdisc;
};

main(argc, argv)
int argc;
char *argv[];
{
    FILE   *ip, *op, *fpwfin, *fpwfout, *fpremin, *fpremout;
    char   *base, *new_dir, *sname, *fname, *inpref, *outpref, tempstring[120];
    char   *temprem, oldpath[80], outfile[28], infile[28], *indata, *bufout;
    int    i, j, nrecs, nrem, outnrem, nfiles;
    int    verbose, nunames, mode, compress, append;
    int    decomp, wordsize;
    long   lastcommid, num_lines, ioff, *longout, longcheck, longcsum, ltime;
    long   n28m1 = 134217727, nbytes, nsamp;
    short  *shortout;
    void   wfdisc_copy();
    struct wfdisc *in_rec;
    struct group  *grp;
    struct remark *remark, *outrem, *oldrem;
    struct tm *tm;
    struct stat statbuf;

/*  Set up defaults  */

    base     = NULL;
    verbose  = FALSE;
    nunames  = FALSE;
    compress = FALSE;
    decomp   = FALSE;
    append   = FALSE;
    mode     = -1;

/*  Scan command line for overrides  */

    for (i = 1; i < argc; i++) {
        if (strncmp(argv[i], "base=", strlen("base=")) == 0) {
            base = argv[i] + strlen("base=");
        } else if (strcmp(argv[i], "+v") == 0) {
            verbose = TRUE;
        } else if (strcmp(argv[i], "-v") == 0) {
            verbose = FALSE;
        } else if (strcmp(argv[i], "+rename") == 0) {
            nunames = TRUE;
        } else if (strcmp(argv[i], "-rename") == 0) {
            nunames = FALSE;
        } else if (strcmp(argv[i], "+c") == 0) {
        compress = TRUE;
        } else if (strcmp(argv[i], "-c") == 0) {
        compress = FALSE;
        } else if (strcmp(argv[i], "+d") == 0) {
        decomp = TRUE;
        } else if (strcmp(argv[i], "-d") == 0) {
        decomp = FALSE;
        } else if (strcmp(argv[i], "+a") == 0) {
        append = TRUE;
        } else if (strncmp(argv[i], "mode=", strlen("mode=")) == 0) {
            mode = strtol(argv[i]+strlen("mode="), NULL, 8);
    } else if (strncmp(argv[i], "inwfd=", strlen("inwfd=")) == 0) {
        inpref = (char *) malloc(strlen(argv[i]) - strlen("inwfd=") +
                     strlen(".wfdisc") + 2);
        strcpy(inpref, argv[i] + strlen("inwfd="));
    } else if (strncmp(argv[i], "outwfd=", strlen("outwfd=")) == 0) {
        outpref = (char *) malloc(strlen(argv[i]) - strlen("outwfd=") +
                     strlen(".wfdisc") + 2);
        strcpy(outpref, argv[i] + strlen("outwfd="));
        } else {
            fprintf(stderr,"migcmp: unrecognized argument '%s'\n",argv[i]);
            help();
        }
    }    /* end of for at line 75 */
    if (base == NULL) {
        fprintf(stderr,"migcmp: must specify base directory name\n");
        help();
    }
    if (inpref == NULL) {
        fprintf(stderr,"migcmp: must specify input prefix\n");
        help();
    }
    if (outpref == NULL) {
        fprintf(stderr,"migcmp: must specify output prefix\n");
        help();
    }

    if (compress && decomp) {
      fprintf(stderr, "migcmp: can only use one option +c or +d, not both\n");
      help();
    }

    get_prefix(inpref);
    get_prefix(outpref);

/*  Read in entire wfdisc, and sort by path name and foff */

    sprintf(tempstring, "%s.wfdisc", inpref);
    if ((fpwfin = fopen(tempstring, "r")) == NULL) {
        fprintf(stderr, "migcmp: error openning input wfdisc\n");
    exit(2);
    }

    if ((nrecs = rd_wfdisc(fpwfin, &in_rec)) <= 0) {
        fprintf(stderr,"migcmp: bad or empty wfdisc file\n");
        exit(2);
    }
    fclose(fpwfin);

    if (nrecs > 1) qsort(in_rec, nrecs, sizeof(struct wfdisc), wfdisc_cmp);

/*  Read in remark file, and sort by commid and lineno */

    sprintf(tempstring, "%s.remark", inpref);
    if ((fpremin = fopen(tempstring, "r")) == NULL) {
        fprintf(stderr, "migcmp: error openning input remark file\n");
    exit(2);
    }

    if ((nrem = rd_remark(fpremin, &remark)) < 0) {
        fprintf(stderr,"migcmp: bad remark file\n");
        exit(2);
    }
    fclose(fpremin);

    if (nrem > 1) qsort(remark, nrem, sizeof(struct remark), remark_cmp);

/*  Read output remark file to add new remarks to  */

    sprintf(tempstring, "%s.remark", outpref);
    if ((stat(tempstring, &statbuf) != 0) && (errno == 2)) {
      outnrem = 0;
      outrem = (struct remark *) malloc(sizeof(struct remark));
    } else {
      if ((fpremout = fopen(tempstring, "r")) == NULL) {
    fprintf(stderr, "migcmp: error openning output remark file\n");
    exit(2);
      }

      if ((outnrem = rd_remark(fpremout, &outrem)) < 0) {
    fprintf(stderr, "migcmp: error reading output remark file\n");
    exit(2);
      }
      fclose(fpremout);
    }

/*  Copy each file, printing its updated wfdisc record(s)  */

    sprintf(tempstring, "%s.wfdisc", outpref);
    if ((stat(tempstring, &statbuf) != 0) && (errno == 2)) {
      if ((fpwfout = fopen(tempstring, "w")) == NULL) {
    fprintf(stderr, "migcmp: error openning output wfdisc\n");
    exit(2);
      }
    } else if ((fpwfout = fopen(tempstring, "a+")) == NULL) {
        fprintf(stderr, "migcmp: error openning output wfdisc\n");
    exit(2);
    }

    if (!decomp) {    /* if decomp == FALSE == -d */

      /*  If not renameing, gather wfdisc records with common path name  */

      if (!nunames)
    nfiles = gather(in_rec, nrecs, &grp);
      else {
    if ((grp = (struct group *) malloc(sizeof(struct group) * nrecs))
         == NULL) {
      fprintf(stderr, "migcmp: error mallocing grp\n");
      exit(2);
    }
    for (i = 0; i < nrecs; i++) {
      grp[i].wfdisc = (struct wfdisc *) malloc(sizeof(struct wfdisc));
      grp[i].nrec = 1;
      wfdisc_copy(&in_rec[i], &grp[i].wfdisc[0]);
    }
    nfiles = nrecs;
      }

      for (i = 0; i < nfiles; i++) {    /* big loop for each file */

        /*  Open input file  */

        sprintf(ipath, "%s/%s", grp[i].wfdisc[0].dir, grp[i].wfdisc[0].dfile);
        if ((ip = fopen(ipath, "rb")) == NULL) {
      perror(ipath);
      exit(3);
        }

        /*  Determine input path ond open file  */

        new_dir = get_newdir(base, &grp[i].wfdisc[0]);
        if (strlen(new_dir) > MAX_DIRLEN) {
      fprintf(stderr,"migcmp: ERROR! '%s' exceeds ", new_dir);
      fprintf(stderr,"maximum directory name length!\n");
      exit(4);
        }
        if (mkpath(new_dir, 0775) != 0) {
      fprintf(stderr,"merge: ");
      perror(new_dir);
      exit(5);
        }

        /*  Determine output path and create file  */

        if (nunames) get_newfile(&grp[i]);
    if (compress)
      sprintf(opath, "%s/%s.C", new_dir, grp[i].wfdisc[0].dfile);
    else
      sprintf(opath, "%s/%s", new_dir, grp[i].wfdisc[0].dfile);

        /*  Open the file for input  */

        if (append == TRUE) {

          if ((op = fopen(opath, "a+")) == NULL) {
        perror(opath);
        exit(6);
          }

          if (stat(opath, &statbuf) != 0) {
        perror(opath);
        exit(6);
          }

          ioff = statbuf.st_size;

        } else {

          if ((op = fopen(opath, "wb")) == NULL) {
        perror(opath);
        exit(6);
          }

          ioff = 0;
        }    /* end if at line 250 */

        /*  Copy the file into its new home  */

        if (verbose) {
      fprintf(stderr,"%s -> %s", ipath, opath);
      fprintf(stderr,"\n");
        }

    copy(ip, op, &grp[i], compress, ioff);

    fclose(ip);
    fclose(op);

        /*  Change mode if desired  */

        if (mode > 0 && chmod(opath, mode) != 0) {
      fprintf(stderr,"migcmp: can't chmod %o: ", mode);
      perror(opath);
        }

        /*  Write the updated wfdisc record(s) for this file  */
        /*  Enforce CSS rules on alphanumeric case  */

        for (j = 0; j < grp[i].nrec; j++) {

      ucase(grp[i].wfdisc[j].sta);
      ucase(grp[i].wfdisc[j].instype);
      lcase(grp[i].wfdisc[j].segtype);
      lcase(grp[i].wfdisc[j].datatype);
      lcase(grp[i].wfdisc[j].clip);
      strcpy(grp[i].wfdisc[j].dir, new_dir);
      if (compress) {
        sprintf(grp[i].wfdisc[j].dfile, "%s.C",grp[i].wfdisc[j].dfile);
        if (strstr(grp[i].wfdisc[j].datatype, "2") != NULL)
          strcpy(grp[i].wfdisc[j].datatype, "c2");
        else
          strcpy(grp[i].wfdisc[j].datatype, "c4");
      }

      if (grp[i].wfdisc[j].commid != -1) {
            temprem = NULL;
        num_lines = get_remark(remark, nrem, grp[i].wfdisc[j].commid,
                   &temprem);

        if (num_lines > 0) {
          grp[i].wfdisc[j].commid = add_remark(&outrem, &outnrem, temprem,
                           num_lines);
          free(temprem);
        } else
          grp[i].wfdisc[j].commid = -1;

      }

      print_wd(&(grp[i].wfdisc[j]), fpwfout);
        }    /* end of for each wf record at line 296 */
      }        /* end of for each file at line 216 */
    } else {    /* end of -d */

/*  Decompress data */
/*  1. if data are uncompressed long data, copy them */
/*  2. if data are uncompressed short data, copy them */
/*  3. if data are compressed data, decompressed them */

      ioff = 0;
      oldpath[0] = NULL;
      op = NULL;

      for (i = 0; i < nrecs; i++) {    /* nrecs == wfdisc recs */

    sprintf(infile, "%s", in_rec[i].dfile);
    sprintf(ipath, "%s/%s", in_rec[i].dir, infile);

    if ((ip = fopen(ipath, "rb")) == NULL) {
      perror(ipath);
      exit(6);
    }
    if (fseek(ip, in_rec[i].foff, 0) == -1) {
      perror(ipath);
      exit(7);
    }

    if (strcmp(in_rec[i].datatype, "s4") == 0) {
      /*  Data is uncompressed long data  */

      nsamp = in_rec[i].nsamp;

      longout = (long *) malloc(in_rec[i].nsamp * sizeof(long));

      nbytes = fread(longout, sizeof(long), in_rec[i].nsamp, ip);
      fclose(ip);

      if (nbytes != in_rec[i].nsamp) {
        perror(ipath);
        exit(8);
      }

      new_dir = get_newdir(base, &in_rec[i]);
      if (strlen(new_dir) > MAX_DIRLEN) {
        fprintf(stderr,"migcmp: ERROR! '%s' exceeds ", new_dir);
        fprintf(stderr,"maximum directory name length!\n");
        exit(4);
      }
      if (mkpath(new_dir, 0775) != 0) {
        fprintf(stderr,"merge: ");
        perror(new_dir);
        exit(5);
      }

      sprintf(in_rec[i].dir, "%s", new_dir);
      sprintf(opath, "%s/%s", new_dir, infile);

      if (strcmp(ipath, oldpath) != 0) {
        if (op != NULL) fclose(op);
        if ((op = fopen(opath, "wb")) == NULL) {
          perror(opath);
          exit(9);
        }
        ioff = 0;
      }

      nbytes = fwrite(longout, sizeof(long), nsamp, op);

      if (nbytes != nsamp) {
        perror(outfile);
        exit(10);
      }

      free(longout);

    } else if (strcmp(in_rec[i].datatype, "s2") == 0) {
      /*  Data is uncompressed short data  */

      nsamp = in_rec[i].nsamp;

      shortout = (short *) malloc(in_rec[i].nsamp * sizeof(short));

      nbytes = fread(shortout, sizeof(short), in_rec[i].nsamp, ip);
      fclose(ip);

      if (nbytes != in_rec[i].nsamp) {
        perror(ipath);
        exit(8);
      }

      new_dir = get_newdir(base, &in_rec[i]);
      if (strlen(new_dir) > MAX_DIRLEN) {
        fprintf(stderr,"migcmp: ERROR! '%s' exceeds ", new_dir);
        fprintf(stderr,"maximum directory name length!\n");
        exit(4);
      }
      if (mkpath(new_dir, 0775) != 0) {
        fprintf(stderr,"merge: ");
        perror(new_dir);
        exit(5);
      }

      sprintf(in_rec[i].dir, "%s", new_dir);
      sprintf(opath, "%s/%s", new_dir, infile);

      if (strcmp(ipath, oldpath) != 0) {
        if (op != NULL) fclose(op);
        if ((op = fopen(opath, "wb")) == NULL) {
          perror(opath);
          exit(9);
        }
        ioff = 0;
      }

      nbytes = fwrite(shortout, sizeof(short), nsamp, op);

      if (nbytes != nsamp) {
        perror(outfile);
        exit(10);
      }

      free(shortout);

    } else {
      /*  Data is compressed  */

      if (strcmp(in_rec[i].datatype, "c4") == 0)
        wordsize = sizeof(long);
      else
        wordsize = sizeof(short);

      indata = (char *) malloc(sizeof(char) * in_rec[i].nsamp * 2);

      nbytes = fread (indata, sizeof(char), in_rec[i].nsamp, ip);
      fclose(ip);

      if (nbytes != in_rec[i].nsamp) {
        perror(ipath);
        exit(8);
      }

      if (nunames) {
        ltime = (long) in_rec[i].time;
        tm = gmtime(&ltime);
        sprintf(outfile, "%s-%s-%2.2d%2.2d.w", in_rec[i].sta,
            in_rec[i].chan, tm->tm_hour, tm->tm_min);
        lcase(outfile);
      } else {
        strncpy(outfile, infile, strlen(infile) - 2);
        outfile[strlen(infile) - 2] = NULL;
      }

      new_dir = get_newdir(base, &in_rec[i]);
      if (strlen(new_dir) > MAX_DIRLEN) {
        fprintf(stderr,"decomp: ERROR! '%s' exceeds ", new_dir);
        fprintf(stderr,"maximum directory name length!\n");
        exit(4);
      }
      if (mkpath(new_dir, 0775) != 0) {
        fprintf(stderr,"merge: ");
        perror(new_dir);
        exit(5);
      }

      sprintf(in_rec[i].dfile, "%s", outfile);
      sprintf(in_rec[i].dir, "%s", new_dir);
      sprintf(opath, "%s/%s", new_dir, outfile);

      if (strcmp(ipath, oldpath) != 0) {
        if (op != NULL) fclose(op);
        if ((op = fopen(opath, "wb")) == NULL) {
          perror(opath);
          exit(9);
        }
        ioff = 0;
      }

      bufout = (char *) malloc(sizeof(long) * (in_rec[i].nsamp + 10));
      longout = (long *) bufout;
      shortout = (short *) bufout;
      nsamp = longdcpress(in_rec[i].nsamp, indata, longout);

      nsamp--;
      longcsum = longout[nsamp];

      rmfdif(nsamp, longout, longout);
      rmfdif(nsamp, longout, longout);

      longcheck = 0;
      for (j = 0; j < nsamp; j++)
        longcheck = longcheck + longout[j];

      longcheck = longcheck & n28m1;

      if (longcsum != longcheck) {
        fprintf(stderr, "migcmp: checksum mismatch on file %s\n", ipath);
        fprintf(stderr, "        before compression:  %d\n", longcsum);
        fprintf(stderr, "        after decompression: %d\n", longcheck);
        fflush(stderr);
        exit(10);
      }

      if (wordsize == sizeof(short)) {
        for (j = 0; j < nsamp; j++)
          shortout[j] = (short)longout[j];
        nbytes = fwrite(shortout, sizeof(short), nsamp, op);
        if (nbytes != nsamp) {
          perror(outfile);
          exit(11);
        }
        sprintf(in_rec[i].datatype, "s2");
      } else {
        nbytes = fwrite(longout, wordsize, nsamp, op);
        if (nbytes != nsamp) {
          perror(outfile);
          exit(11);
        }
        sprintf(in_rec[i].datatype, "s4");
      }

      free(indata);
      free(longout);
      free(shortout);
      free(bufout);

      in_rec[i].foff = ioff;
      in_rec[i].nsamp = nsamp;

      ioff = ioff + (wordsize * nsamp);

    }    /* end of if at line 354 for 3 different situations */

    sprintf(oldpath, "%s", ipath);

    /*  Write the new wfdisc entry  */
    /*  Enforce CSS rules on alphanumeric case */

    ucase(in_rec[i].sta);
    ucase(in_rec[i].instype);
    lcase(in_rec[i].segtype);
    lcase(in_rec[i].datatype);
    lcase(in_rec[i].clip);
    if (in_rec[i].commid != -1) {
          temprem = NULL;
      num_lines = get_remark(remark, nrem, in_rec[i].commid,
                 &temprem);
      if (num_lines > 0) {
        in_rec[i].commid = add_remark(&outrem, &outnrem, temprem,
                      num_lines);
        free(temprem);
      } else
        in_rec[i].commid = -1;

    }

    print_wd(&in_rec[i], fpwfout);
    fflush(fpwfout);
      }        /* end for each at line 340 */
    }        /* end of if at line 196 */

    fclose(fpwfout);

    /*  Write output remark file  */

    sprintf(tempstring, "%s.remark", outpref);
    if ((fpremout = fopen(tempstring, "w")) == NULL) {
        fprintf(stderr, "migcmp: error openning output remark file\n");
    exit(2);
    }

    for (i = 0; i < outnrem; i++)
      print_remark(&outrem[i], fpremout);

    fclose(fpremout);

    exit(0);
}    /* end of main */

int wfdisc_cmp(a, b)
struct wfdisc *a, *b;
{
    int cnd;

    if ((cnd = strcmp(a->dir, b->dir)))       /* Sort on dir */
      return(cnd);
    if ((cnd = strcmp(a->dfile, b->dfile)))   /* Sort on file */
      return(cnd);
    cnd = a->jdate - b->jdate;                /* sort by jdate */
    if (cnd != 0)
      return(cnd);
    return(-(b->foff - a->foff));             /* Sort on foff */
}

int remark_cmp(a, b)
struct remark *a, *b;
{
    if (a->commid == b->commid)
      return(a->lineno - b->lineno);
    else
      return(a->commid - b->commid);
}

int gather(wfdisc, nrecs, grp)
struct wfdisc wfdisc[];
int nrecs;
struct group **grp;
{
    int count, i, j, k, l, doit;
    char name1[MAXPATHLEN], name2[MAXPATHLEN];
    struct group tmp, *out;
    register struct date_time date_time,*dt = &date_time;

/*  Scan thru list first time to get number of unique names  */

    count = 0; i = 0; j = 1;
    dt->epoch = wfdisc[i].time;
    etoh(dt);
    sprintf(name1, "%d/%s/%s", dt->date, wfdisc[i].dir, wfdisc[i].dfile);
    do {
        if (j < nrecs) {
        dt->epoch = wfdisc[j].time;
        etoh(dt);
            sprintf(name2, "%d/%s/%s", dt->date, wfdisc[j].dir, wfdisc[j].dfile);
            doit = (strcmp(name1, name2) != 0);
        } else {
            doit = TRUE;
        }
        if (doit) {
            ++count;
            strcpy(name1, name2);
            i = j++;
        } else {
            ++j;
        }
    } while (j <= nrecs);

/*  Allocate memory to hold all the groups  */

    if ((out = (struct group *) malloc(count*sizeof(struct group))) == NULL) {
        perror("migcmp (malloc)");
        exit(8);
    }

/*  Now repeat test, assigning groups  */
/*  Verify that datatype is the same for each group  */

    count = 0; i = 0; j = 1;
    dt->epoch = wfdisc[i].time;
    etoh(dt);
    sprintf(name1, "%d/%s/%s", dt->date, wfdisc[i].dir, wfdisc[i].dfile);
    do {
        if (j < nrecs) {
        dt->epoch = wfdisc[j].time;
        etoh(dt);
            sprintf(name2, "%d/%s/%s", dt->date, wfdisc[j].dir, wfdisc[j].dfile);
            doit = (strcmp(name1, name2) != 0);
        } else {
            doit = TRUE;
        }
        if (doit) {
            tmp.nrec = j - i;
            if ((tmp.wfdisc = (struct wfdisc *)
                malloc(tmp.nrec*sizeof(struct wfdisc))) == NULL) {
                perror("migcmp (malloc)");
                exit(9);
            }
            for (k=0, l=i; k < tmp.nrec; k++, l++) {
                tmp.wfdisc[k] = wfdisc[l];
                if (
                    (strcmp(tmp.wfdisc[k].datatype, "i2") != 0) &&
                    (strcmp(tmp.wfdisc[k].datatype, "i4") != 0) &&
                    (strcmp(tmp.wfdisc[k].datatype, "s2") != 0) &&
                    (strcmp(tmp.wfdisc[k].datatype, "s4") != 0) &&
                    (strcmp(tmp.wfdisc[k].datatype, "c2") != 0) &&
                    (strcmp(tmp.wfdisc[k].datatype, "c4") != 0) &&
                    (strcmp(tmp.wfdisc[k].datatype, "f4") != 0) &&
                    (strcmp(tmp.wfdisc[k].datatype, "t4") != 0) &&
                    (strcmp(tmp.wfdisc[k].datatype, "t8") != 0)
                ) {
                    fprintf(stderr,"migcmp: unsupported datatype ");
                    fprintf(stderr,"'%s' ", tmp.wfdisc[k].datatype);
                    fprintf(stderr,"for file '%s/",  tmp.wfdisc[k].dir);
                    fprintf(stderr,"%s'\n", tmp.wfdisc[k].dfile);
                    exit(10);
                }
                if (k && strcmp(tmp.wfdisc[k].datatype, tmp.wfdisc[k-1].datatype) != 0)
        {
                    fprintf(stderr,"migcmp: conflicting datatype's ");
                    fprintf(stderr,"('%s' and ", tmp.wfdisc[k-1].datatype);
                    fprintf(stderr,"'%s') ", tmp.wfdisc[k].datatype);
                    fprintf(stderr,"for file '%s/",  tmp.wfdisc[k].dir);
                    fprintf(stderr,"%s'\n", tmp.wfdisc[k].dfile);
                    exit(11);
                }
            }
            out[count++] = tmp;
            strcpy(name1, name2);
            i = j++;
        } else {
            ++j;
        }
    } while (j <= nrecs);

/*  Return total number of groups  */

    *grp = out;
    return count;

}

char *get_newdir(base, wfdisc)
char *base;
struct wfdisc *wfdisc;
{
    static char new_name[MAXPATHLEN];
    int yr, da;
    register struct date_time date_time,*dt = &date_time;

    dt->epoch = wfdisc->time;
    etoh(dt);
    yr = dt->year;
    da = dt->doy;

    sprintf(new_name, "%s/%4.4d/%3.3d", base, yr, da);
    return new_name;
}

int get_newfile(grp)
struct group *grp;
{
    int i, hh, mm;
    long ltime;
    static char new_name[MAXPATHLEN];
    struct tm *tm;

    for (i = 0; i < grp->nrec; i++) {
        ltime = (long) grp->wfdisc[i].time;
        tm = gmtime(&ltime);
    sprintf(new_name, "%s-%s-%2.2d%2.2d.w",
        grp->wfdisc[i].sta, grp->wfdisc[i].chan, tm->tm_hour, tm->tm_min);
        lcase(strcpy(grp->wfdisc[i].dfile, new_name));
    }

}

int copy(ip, op, grp, compress, offset)
FILE *ip, *op;
struct group *grp;
int compress, offset;
{
    char *dataout;
    long i, nsamp, ioff, oldoff, *longin, longcheck, longcsum, n28m1 = 134217727;
    short *shortin, shortcheck, n13m1 = 8191;
    float *floatin;
    double *doublein;
    int compnum, count;

    ioff = offset;

    for (count = 0; count < grp->nrec; count++) {

    oldoff = ioff;

    if (!compress || (grp->wfdisc[count].datatype[0] == 'c')) {
          if (strstr(grp->wfdisc[count].datatype, "s4") != NULL) {
        /* Long data, no compression */
            longin = (long *) malloc((grp->wfdisc[count].nsamp + 10)
                     * sizeof(long));
        fseek(ip, grp->wfdisc[count].foff, 0);
        nsamp = fread(longin, sizeof(long), grp->wfdisc[count].nsamp,
                  ip);
        if (fwrite(longin, sizeof(long), nsamp, op) != nsamp) {
            perror(opath);
            exit(12);
        }
        free(longin);
        ioff = ioff + nsamp * sizeof(long);
        } else if (strstr(grp->wfdisc[count].datatype, "s2") != NULL) {
        /* Short data, no compression */
            shortin = (short *) malloc((grp->wfdisc[count].nsamp + 10)
                     * sizeof(short));
        fseek(ip, grp->wfdisc[count].foff, 0);
        nsamp = fread(shortin, sizeof(short), grp->wfdisc[count].nsamp,
                  ip);
        if (fwrite(shortin, sizeof(short), nsamp, op) != nsamp) {
            perror(opath);
            exit(12);
        }
        free(shortin);
        ioff = ioff + nsamp * sizeof(short);
        } else if (strstr(grp->wfdisc[count].datatype, "t4") != NULL) {
        /* float data, no compression */
            floatin = (float *) malloc((grp->wfdisc[count].nsamp + 10)
                     * sizeof(float));
        fseek(ip, grp->wfdisc[count].foff, 0);
        nsamp = fread(floatin, sizeof(float), grp->wfdisc[count].nsamp,
                  ip);
        if (fwrite(floatin, sizeof(float), nsamp, op) != nsamp) {
            perror(opath);
            exit(12);
        }
        free(floatin);
        ioff = ioff + nsamp * sizeof(float);
        } else if (strstr(grp->wfdisc[count].datatype, "t8") != NULL) {
        /* float data, no compression */
            doublein = (double *) malloc((grp->wfdisc[count].nsamp + 10)
                     * sizeof(double));
        fseek(ip, grp->wfdisc[count].foff, 0);
        nsamp = fread(doublein, sizeof(double), grp->wfdisc[count].nsamp,
                  ip);
        if (fwrite(doublein, sizeof(double), nsamp, op) != nsamp) {
            perror(opath);
            exit(12);
        }
        free(doublein);
        ioff = ioff + nsamp * sizeof(double);
        } else {
            /* Data is already compressed */
            dataout = malloc((grp->wfdisc[count].nsamp + 10) *
                 sizeof(char));
        fseek(ip, grp->wfdisc[count].foff, 0);
        nsamp = fread(dataout, sizeof(char), grp->wfdisc[count].nsamp,
                  ip);
        if (fwrite(dataout, sizeof(char), nsamp, op) != nsamp) {
            perror(opath);
            exit(12);
        }
        free(dataout);
        ioff = ioff + nsamp * sizeof(char);
        }
        } else {
        shortin = (short *) malloc((grp->wfdisc[count].nsamp + 10)
                       * sizeof(short));

        longin = (long *) malloc((grp->wfdisc[count].nsamp + 10)
                     * sizeof(long));

        fseek(ip, grp->wfdisc[count].foff, 0);

        if (strstr(grp->wfdisc[count].datatype, "s4") != NULL)
              nsamp = fread(longin, sizeof(long), grp->wfdisc[count].nsamp,
                  ip);
        else {
            nsamp = fread(shortin, sizeof(short), grp->wfdisc[count].nsamp,
                  ip);
            for (i = 0; i < nsamp; i++)
          longin[i] = (long)shortin[i];
            }

        dataout = (char *) malloc((grp->wfdisc[count].nsamp + 10)
                     * sizeof(long));

        /*  Calculate checksum  */

        longcheck = 0;
        for (i = 0; i < nsamp; i++)
          longcheck = longcheck + longin[i];
        longcheck = longcheck & n28m1;

        /*  Do First differences  */

        fstdif(nsamp, longin, longin);
        fstdif(nsamp, longin, longin);

        /*  Insert checksum at end of data and compress data  */

        longin[nsamp] = longcheck;
        nsamp++;

        compnum = longcmprss(nsamp, dataout, longin);

        nsamp = compnum;

        /*  Write out data  */

        if (fwrite(dataout, sizeof(char), compnum, op) != compnum) {
        perror(opath);
        exit(12);
        }
        ioff = ioff + compnum;

        free(longin);
        free(dataout);
        free(shortin);
        }

    if (compress)
      grp->wfdisc[count].nsamp = compnum;
    grp->wfdisc[count].foff = oldoff;

    }

}

/*======================================================================
    wfdisc_copy                                            AJR 8/20/92
  Copy all elements of the inputed source wfdisc entry to the
  desination entry.
======================================================================*/
void wfdisc_copy(source, dest)
struct wfdisc source, *dest;

{
    sprintf(dest->sta, "%s", source.sta);
    sprintf(dest->chan, "%s", source.chan);

    dest->time = source.time;
    dest->wfid = source.wfid;
    dest->chanid = source.chanid;
    dest->jdate = source.jdate;
    dest->endtime = source.endtime;
    dest->nsamp = source.nsamp;
    dest->samprate = source.samprate;
    dest->calib = source.calib;
    dest->calper = source.calper;

    sprintf(dest->instype, "%s", source.instype);
    sprintf(dest->segtype, "%s", source.segtype);
    sprintf(dest->datatype, "%s", source.datatype);
    sprintf(dest->clip, "%s", source.clip);
    sprintf(dest->dir, "%s", source.dir);
    sprintf(dest->dfile, "%s", source.dfile);

    dest->foff = source.foff;
    dest->commid = source.commid;

    sprintf(dest->lddate, "%s", source.lddate);
}

int help()
{
    fprintf(stderr,"migcmp version %s\n", SccsId);
    fprintf(stderr,"usage: migcmp base=dir inwfd= outwfd= ");
    fprintf(stderr,"[+/-v +/-c +/-d +a +/-rename mode=0xxx]\n");
    fprintf(stderr,"     base is the base directory to write data to\n");
    fprintf(stderr,"     inwfd and outwfd are prefixes for input/output wfdiscs and remarks\n");
    fprintf(stderr,"     +v turns on verbose mode\n");
    fprintf(stderr,"     +c turns on data compression\n");
    fprintf(stderr,"     +d turns on data decompression\n");
    fprintf(stderr,"     +a turns on append mode during compression\n");
    fprintf(stderr,"         NOTE: only one options (+c or +d) may be used at any one time\n");
    fprintf(stderr,"     +rename turns on file renaming (new names in format sta-chan-hhmm.w)\n");
    fprintf(stderr,"     mode is the mode to set the data files to after writing\n");
    exit(1);
}

