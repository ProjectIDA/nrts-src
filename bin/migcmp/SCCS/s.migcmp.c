h23457
s 00003/00002/00937
d D 1.10 01/07/09 08:53:29 pdavis 10 9
c checking back in
e
s 00050/00024/00889
d D 1.9 95/03/17 12:53:45 dap 9 8
c added new option to append to outputed files
e
s 00009/00006/00904
d D 1.8 94/02/07 13:47:30 dap 8 7
c corrected error with uninitiated pointer
e
s 00003/00003/00907
d D 1.7 93/02/11 13:48:22 reynolds 7 6
c Added some missing free() calls
e
s 00008/00002/00902
d D 1.6 93/01/15 14:52:42 reynolds 6 5
c Fixed sort to include jdate
e
s 00028/00016/00876
d D 1.5 92/08/24 15:07:07 reynolds 5 4
c Fixed up several minor problems
e
s 00431/00074/00461
d D 1.4 92/08/20 13:29:57 reynolds 4 3
c Fixed add_remark line
e
s 00031/00007/00504
d D 1.3 92/08/11 14:03:29 reynolds 3 2
c Append to wfdisc and remark files instead of overwriting
e
s 00119/00047/00392
d D 1.2 92/08/05 18:14:49 reynolds 2 1
c Upgraded to 3.0
e
s 00439/00000/00000
d D 1.1 92/07/09 14:35:46 reynolds 1 0
c date and time created 92/07/09 14:35:46 by reynolds
e
u
U
f e 0
t
T
I 1
/* %W%  %G%  */
/*======================================================================
 *
 *  utilities/migcmp.c
 *
 *  Copy all the data in the wfdisc at stdin to the base directory,
 *  using Epoch path name convention.  New wfdisc file is sent to
 *  stdout.
 *
D 4
 *  VAX data types (i2, i4, f4) are converted to IEEE (s2, s4, t4), and
 *  names are changed to the form sta-ch-hhmm.w, so this program can be
 *  used to migrate the GDSN data as well.
E 4
I 4
 *  Data is compressed if -c option is specified.  If data is already
 *  compressed, it is only copied.
E 4
 *
D 4
 *  Data is compressed if -c option is specified.
E 4
I 4
 *  Data is decompressed if -d option is specified.  If data is not
 *  compressed, it is only copied.
E 4
 *
 *====================================================================*/
#include <stdio.h>
#include <sys/param.h>
I 4
#include <sys/types.h>
#include <sys/stat.h>
D 5
#include <sys/errno.h>
E 5
I 5
#include <errno.h>
E 5
E 4
#include <time.h>
#include "csstime.h"
D 2
#include "wfdiscio.h"
E 2
I 2
#include "wfdisc30io.h"
E 2
#include "util.h"
#include "misc.h"

D 4
#define MAX_DIRLEN 30
E 4
I 4
#define MAX_DIRLEN 64
E 4

I 4
static char SccsId[] = "%I%  (%G%)";

E 4
extern int wfdisc_cmp();
I 2
extern int remark_cmp();
E 2
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
D 2
    FILE *ip, *op;
    char *base, *new_dir, *sname, *fname;
    int  i, j, nrecs, nfiles, verbose, nunames, mode, compress;
E 2
I 2
D 9
    FILE *ip, *op, *fpwfin, *fpwfout, *fpremin, *fpremout;
    char *base, *new_dir, *sname, *fname, *inpref, *outpref, tempstring[120];
D 3
    int  i, j, nrecs, nrem, nfiles, verbose, nunames, mode, compress;
E 3
I 3
D 4
    char *temprem;
E 4
I 4
    char *temprem, oldpath[80], outfile[28], infile[28], *indata, *bufout;
E 4
    int  i, j, nrecs, nrem, outnrem, nfiles, verbose, nunames, mode, compress;
D 4
    long lastcommid, num_lines;
E 4
I 4
    int  decomp, wordsize;
    long lastcommid, num_lines, ioff, *longout, longcheck, longcsum, ltime;
    long n28m1 = 134217727, nbytes, nsamp;
    short *shortout;
E 9
I 9
    FILE   *ip, *op, *fpwfin, *fpwfout, *fpremin, *fpremout;
    char   *base, *new_dir, *sname, *fname, *inpref, *outpref, tempstring[120];
    char   *temprem, oldpath[80], outfile[28], infile[28], *indata, *bufout;
    int    i, j, nrecs, nrem, outnrem, nfiles;
    int    verbose, nunames, mode, compress, append;
    int    decomp, wordsize;
    long   lastcommid, num_lines, ioff, *longout, longcheck, longcsum, ltime;
    long   n28m1 = 134217727, nbytes, nsamp;
    short  *shortout;
E 9
    void   wfdisc_copy();
E 4
E 3
E 2
    struct wfdisc *in_rec;
    struct group  *grp;
I 2
D 3
    struct remark *remark;
E 3
I 3
    struct remark *remark, *outrem, *oldrem;
I 4
    struct tm *tm;
D 9
    struct stat statbuf[1];
E 9
I 9
    struct stat statbuf;
E 9
E 4
E 3
E 2

/*  Set up defaults  */

D 9
    base    = NULL;
    verbose = FALSE;
    nunames = FALSE;
E 9
I 9
    base     = NULL;
    verbose  = FALSE;
    nunames  = FALSE;
E 9
    compress = FALSE;
I 4
D 9
    decomp  = FALSE;
E 4
    mode    = -1;
E 9
I 9
    decomp   = FALSE;
    append   = FALSE;
    mode     = -1;
E 9

/*  Scan command line for overrides  */

    for (i = 1; i < argc; i++) {
        if (strncmp(argv[i], "base=", strlen("base=")) == 0) {
            base = argv[i] + strlen("base=");
        } else if (strcmp(argv[i], "+v") == 0) {
            verbose = TRUE;
I 5
        } else if (strcmp(argv[i], "-v") == 0) {
            verbose = FALSE;
E 5
D 4
        } else if (strcmp(argv[i], "-v") == 0) {
            verbose = FALSE;
E 4
        } else if (strcmp(argv[i], "+rename") == 0) {
            nunames = TRUE;
I 5
        } else if (strcmp(argv[i], "-rename") == 0) {
            nunames = FALSE;
E 5
D 4
        } else if (strcmp(argv[i], "-rename") == 0) {
            nunames = FALSE;
        } else if (strcmp(argv[i], "-c") == 0) {
E 4
I 4
        } else if (strcmp(argv[i], "+c") == 0) {
E 4
	    compress = TRUE;
I 5
        } else if (strcmp(argv[i], "-c") == 0) {
	    compress = FALSE;
E 5
I 4
        } else if (strcmp(argv[i], "+d") == 0) {
	    decomp = TRUE;
I 5
        } else if (strcmp(argv[i], "-d") == 0) {
	    decomp = FALSE;
I 9
        } else if (strcmp(argv[i], "+a") == 0) {
	    append = TRUE;
E 9
E 5
E 4
        } else if (strncmp(argv[i], "mode=", strlen("mode=")) == 0) {
            mode = strtol(argv[i]+strlen("mode="), NULL, 8);
I 2
	} else if (strncmp(argv[i], "inwfd=", strlen("inwfd=")) == 0) {
D 4
	    inpref = argv[i] + strlen("inwfd=");
E 4
I 4
	    inpref = (char *) malloc(strlen(argv[i]) - strlen("inwfd=") +
				     strlen(".wfdisc") + 2);
	    strcpy(inpref, argv[i] + strlen("inwfd="));
E 4
	} else if (strncmp(argv[i], "outwfd=", strlen("outwfd=")) == 0) {
D 4
	    outpref = argv[i] + strlen("outwfd=");
E 4
I 4
	    outpref = (char *) malloc(strlen(argv[i]) - strlen("outwfd=") +
				     strlen(".wfdisc") + 2);
	    strcpy(outpref, argv[i] + strlen("outwfd="));
E 4
E 2
        } else {
D 2
            fprintf(stderr,"migrate: unrecognized argument '%s'\n",argv[i]);
E 2
I 2
            fprintf(stderr,"migcmp: unrecognized argument '%s'\n",argv[i]);
E 2
            help();
        }
    }
    if (base == NULL) {
D 2
        fprintf(stderr,"migrate: must specify base directory name\n");
E 2
I 2
        fprintf(stderr,"migcmp: must specify base directory name\n");
E 2
        help();
    }
I 2
    if (inpref == NULL) {
        fprintf(stderr,"migcmp: must specify input prefix\n");
        help();
    }
    if (outpref == NULL) {
        fprintf(stderr,"migcmp: must specify output prefix\n");
        help();
    }
E 2

I 4
    if (compress && decomp) {
      fprintf(stderr, "migcmp: can only use one option +c or +d, not both\n");
      help();
    }

E 4
I 2
    get_prefix(inpref);
    get_prefix(outpref);

E 2
/*  Read in entire wfdisc, and sort by path name and foff */

D 2
    if ((nrecs = rd_wfdisc(stdin, &in_rec)) <= 0) {
        fprintf(stderr,"migrate: bad or empty wfdisc file\n");
E 2
I 2
    sprintf(tempstring, "%s.wfdisc", inpref);
    if ((fpwfin = fopen(tempstring, "r")) == NULL) {
        fprintf(stderr, "migcmp: error openning input wfdisc\n");
	exit(2);
    }

    if ((nrecs = rd_wfdisc(fpwfin, &in_rec)) <= 0) {
        fprintf(stderr,"migcmp: bad or empty wfdisc file\n");
E 2
        exit(2);
    }
I 2
    fclose(fpwfin);

E 2
    if (nrecs > 1) qsort(in_rec, nrecs, sizeof(struct wfdisc), wfdisc_cmp);

I 2
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
D 4
    fclose(fpwfout);
E 4
I 4
    fclose(fpremin);
E 4

    if (nrem > 1) qsort(remark, nrem, sizeof(struct remark), remark_cmp);

E 2
D 4
/*  Gather wfdisc records with common path name  */

    nfiles = gather(in_rec, nrecs, &grp);

E 4
I 3
/*  Read output remark file to add new remarks to  */

    sprintf(tempstring, "%s.remark", outpref);
D 4
    if ((fpremout = fopen(tempstring, "r")) == NULL) {
      fprintf(stderr, "migcmp: error openning output wfdisc\n");
E 4
I 4
D 9
    if ((stat(tempstring, statbuf) != 0) && (errno == 2)) {
E 9
I 9
    if ((stat(tempstring, &statbuf) != 0) && (errno == 2)) {
E 9
      outnrem = 0;
      outrem = (struct remark *) malloc(sizeof(struct remark));
D 5
    } else if ((fpremout = fopen(tempstring, "r")) == NULL) {
      fprintf(stderr, "migcmp: error openning output remark file\n");
E 4
      exit(2);
E 5
I 5
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
E 5
    }

D 5
    if ((outnrem = rd_remark(fpremout, &outrem)) < 0) {
      fprintf(stderr, "migcmp: error reading output remark file\n");
      exit(2);
    }
    fclose(fpremout);

E 5
E 3
/*  Copy each file, printing its updated wfdisc record(s)  */

I 2
    sprintf(tempstring, "%s.wfdisc", outpref);
D 3
    if ((fpwfout = fopen(tempstring, "w")) == NULL) {
E 3
I 3
D 4
    if ((fpwfout = fopen(tempstring, "a+")) == NULL) {
E 4
I 4
D 9
    if ((stat(tempstring, statbuf) != 0) && (errno == 2)) {
E 9
I 9
    if ((stat(tempstring, &statbuf) != 0) && (errno == 2)) {
E 9
      if ((fpwfout = fopen(tempstring, "w")) == NULL) {
	fprintf(stderr, "migcmp: error openning output wfdisc\n");
	exit(2);
      }
    } else if ((fpwfout = fopen(tempstring, "a+")) == NULL) {
E 4
E 3
        fprintf(stderr, "migcmp: error openning output wfdisc\n");
	exit(2);
    }

E 2
D 4
    for (i = 0; i < nfiles; i++) {
E 4
I 4
    if (!decomp) {
E 4

I 4
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
      
      for (i = 0; i < nfiles; i++) {
	
E 4
        /*  Open input file  */
D 4

E 4
I 4
	
E 4
D 2
        sprintf(ipath, "%s/%s", grp[i].wfdisc[0].dir, grp[i].wfdisc[0].file);
E 2
I 2
        sprintf(ipath, "%s/%s", grp[i].wfdisc[0].dir, grp[i].wfdisc[0].dfile);
E 2
        if ((ip = fopen(ipath, "rb")) == NULL) {
D 4
            perror(ipath);
            exit(3);
E 4
I 4
	  perror(ipath);
	  exit(3);
E 4
        }
D 4

E 4
I 4
	
E 4
        /*  Determine input path ond open file  */
D 4

E 4
I 4
	
E 4
        new_dir = get_newdir(base, &grp[i].wfdisc[0]);
        if (strlen(new_dir) > MAX_DIRLEN) {
D 2
            fprintf(stderr,"migrate: ERROR! '%s' exceeds ", new_dir);
E 2
I 2
D 4
            fprintf(stderr,"migcmp: ERROR! '%s' exceeds ", new_dir);
E 2
            fprintf(stderr,"maximum directory name length!\n");
            exit(4);
E 4
I 4
	  fprintf(stderr,"migcmp: ERROR! '%s' exceeds ", new_dir);
	  fprintf(stderr,"maximum directory name length!\n");
	  exit(4);
E 4
        }
        if (mkpath(new_dir, 0775) != 0) {
D 4
            fprintf(stderr,"merge: ");
            perror(new_dir);
            exit(5);
E 4
I 4
	  fprintf(stderr,"merge: ");
	  perror(new_dir);
	  exit(5);
E 4
        }
D 4

E 4
I 4
	
E 4
        /*  Determine output path and create file  */

        if (nunames) get_newfile(&grp[i]);
	if (compress) 
D 2
  	    sprintf(opath, "%s/%s.C", new_dir, grp[i].wfdisc[0].file);
E 2
I 2
D 4
  	    sprintf(opath, "%s/%s.C", new_dir, grp[i].wfdisc[0].dfile);
E 4
I 4
	  sprintf(opath, "%s/%s.C", new_dir, grp[i].wfdisc[0].dfile);
E 4
E 2
	else
D 2
  	    sprintf(opath, "%s/%s", new_dir, grp[i].wfdisc[0].file);
E 2
I 2
D 4
  	    sprintf(opath, "%s/%s", new_dir, grp[i].wfdisc[0].dfile);
E 4
I 4
	  sprintf(opath, "%s/%s", new_dir, grp[i].wfdisc[0].dfile);
E 4
E 2
        
D 9
        if ((op = fopen(opath, "wb")) == NULL) {
D 4
            perror(opath);
            exit(6);
E 4
I 4
	  perror(opath);
	  exit(6);
E 9
I 9
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
E 9
E 4
        }
D 4

E 4
I 4
	
E 4
        /*  Copy the file into its new home  */
D 4

E 4
I 4
	
E 4
        if (verbose) {
D 4
            fprintf(stderr,"%s -> %s", ipath, opath);
            fprintf(stderr,"\n");
E 4
I 4
	  fprintf(stderr,"%s -> %s", ipath, opath);
	  fprintf(stderr,"\n");
E 4
        }
        
D 9
	copy(ip, op, &grp[i], compress); 
E 9
I 9
	copy(ip, op, &grp[i], compress, ioff); 
E 9
D 4

E 4
I 4
	
E 4
	fclose(ip);
	fclose(op);
D 4

E 4
I 4
	
E 4
        /*  Change mode if desired  */
D 4

E 4
I 4
	
E 4
        if (mode > 0 && chmod(opath, mode) != 0) {
D 2
            fprintf(stderr,"migrate: can't chmod %o: ", mode);
E 2
I 2
D 4
            fprintf(stderr,"migcmp: can't chmod %o: ", mode);
E 2
            perror(opath);
E 4
I 4
	  fprintf(stderr,"migcmp: can't chmod %o: ", mode);
	  perror(opath);
E 4
        }
D 4

E 4
I 4
	
E 4
        /*  Write the updated wfdisc record(s) for this file  */
        /*  Enforce CSS rules on alphanumeric case  */
D 4

E 4
I 4
	
E 4
        for (j = 0; j < grp[i].nrec; j++) {
I 8

E 8
D 4
            ucase(grp[i].wfdisc[j].sta);
D 2
            ucase(grp[i].wfdisc[j].instyp);
            lcase(grp[i].wfdisc[j].segtyp);
            lcase(grp[i].wfdisc[j].dattyp);
E 2
I 2
            ucase(grp[i].wfdisc[j].instype);
            lcase(grp[i].wfdisc[j].segtype);
            lcase(grp[i].wfdisc[j].datatype);
E 2
            lcase(grp[i].wfdisc[j].clip);
            strcpy(grp[i].wfdisc[j].dir, new_dir);
	    if (compress) {
D 2
		sprintf(grp[i].wfdisc[j].file, "%s.C", grp[i].wfdisc[j].file);
		if (strstr(grp[i].wfdisc[j].dattyp, "2") != NULL)
		    strcpy(grp[i].wfdisc[j].dattyp, "c2");
E 2
I 2
		sprintf(grp[i].wfdisc[j].dfile, "%s.C",grp[i].wfdisc[j].dfile);
		if (strstr(grp[i].wfdisc[j].datatype, "2") != NULL)
		    strcpy(grp[i].wfdisc[j].datatype, "c2");
E 2
		else 
D 2
		    strcpy(grp[i].wfdisc[j].dattyp, "c4");
E 2
I 2
		    strcpy(grp[i].wfdisc[j].datatype, "c4");
E 4
I 4
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
I 8
            temprem = NULL;
E 8
	    num_lines = get_remark(remark, nrem, grp[i].wfdisc[j].commid,
				   &temprem);

D 8
	    if (num_lines > 0) 
E 8
I 8
	    if (num_lines > 0) {
E 8
	      grp[i].wfdisc[j].commid = add_remark(&outrem, &outnrem, temprem,
						   num_lines);
D 8
	    else
E 8
I 8
	      free(temprem);
	    } else
E 8
	      grp[i].wfdisc[j].commid = -1;

D 8
	    free(temprem);
E 8
	  }
	  
	  print_wd(&(grp[i].wfdisc[j]), fpwfout);
        }
      }
    } else {

/*  Decompress data */
      
      ioff = 0;
      oldpath[0] = NULL;
      op = NULL;

      for (i = 0; i < nrecs; i++) {

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
E 4
E 2
	    }
D 4
            
I 3
	    if (grp[i].wfdisc[j].commid != -1) {
	      num_lines = get_remark(remark, nrem, grp[i].wfdisc[j].commid,
				     &temprem);
	      if (num_lines > -1) 
		grp[i].wfdisc[j].commid = add_remark(outrem, &outnrem, temprem,
						     num_lines);
E 4
I 4
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
E 4
	    }
I 4
	    ioff = 0;
	  }
	  
	  nbytes = fwrite(shortout, sizeof(short), nsamp, op);
	  
	  if (nbytes != nsamp) {
	    perror(outfile);
	    exit(10);
	  }
	  
	  free(shortout);
E 4

E 3
D 2
	    print_wd(&(grp[i].wfdisc[j]), stdout);
E 2
I 2
D 4
	    print_wd(&(grp[i].wfdisc[j]), fpwfout);
E 2
        }
E 4
I 4
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
D 5
	  
E 5
I 5

E 5
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
D 5
	    sprintf(in_rec[j].datatype, "s2");
E 5
I 5
	    sprintf(in_rec[i].datatype, "s2");
E 5
	  } else {
	    nbytes = fwrite(longout, wordsize, nsamp, op);
	    if (nbytes != nsamp) {
	      perror(outfile);
	      exit(11);
	    }
D 5
	    sprintf(in_rec[j].datatype, "s4");
E 5
I 5
	    sprintf(in_rec[i].datatype, "s4");
E 5
	  }
	  
	  free(indata);
	  free(longout);
	  free(shortout);
	  free(bufout);
	  
	  in_rec[i].foff = ioff;
	  in_rec[i].nsamp = nsamp;
	  
	  ioff = ioff + (wordsize * nsamp);
	  
	}

	sprintf(oldpath, "%s", ipath);
	
	/*  Write the new wfdisc entry  */
	/*  Enforce CSS rules on alphanumeric case */
	
	ucase(in_rec[i].sta);
	ucase(in_rec[i].instype);
	lcase(in_rec[i].segtype);
	lcase(in_rec[i].datatype);
	lcase(in_rec[i].clip);
	if (in_rec[i].commid != -1) {
I 8
          temprem = NULL;
E 8
	  num_lines = get_remark(remark, nrem, in_rec[i].commid,
				 &temprem);
D 5
	  num_lines++;
	  
	  if (num_lines > -1) 
E 5
I 5
D 8
	  if (num_lines > 0) 
E 8
I 8
	  if (num_lines > 0) {
E 8
E 5
	    in_rec[i].commid = add_remark(&outrem, &outnrem, temprem,
					  num_lines);
I 5
D 8
	  else 
E 8
I 8
	    free(temprem);
	  } else 
E 8
	    in_rec[i].commid = -1;

D 8
	  free(temprem);
E 8
E 5
	}
	
	print_wd(&in_rec[i], fpwfout);
	fflush(fpwfout);
      }
E 4
    }
I 2

    fclose(fpwfout);

D 3
    /*  Write remark file  */
E 3
I 3
    /*  Write output remark file  */
E 3

    sprintf(tempstring, "%s.remark", outpref);
    if ((fpremout = fopen(tempstring, "w")) == NULL) {
D 3
        fprintf(stderr, "migcmp: error openning input remark file\n");
E 3
I 3
        fprintf(stderr, "migcmp: error openning output remark file\n");
E 3
	exit(2);
    }

D 3
    for (i = 0; i < nrem; i++) 
      print_remark(&remark[i], fpremout);
E 3
I 3
    for (i = 0; i < outnrem; i++) 
      print_remark(&outrem[i], fpremout);
E 3

    fclose(fpremout);

E 2
    exit(0);
}

int wfdisc_cmp(a, b)
struct wfdisc *a, *b;
{
    int cnd;

    if ((cnd = strcmp(a->dir, b->dir)))       /* Sort on dir */
      return(cnd);
D 2
    if ((cnd = strcmp(a->file, b->file)))     /* Sort on file */
E 2
I 2
D 6
    if ((cnd = strcmp(a->dfile, b->dfile)))     /* Sort on file */
E 6
I 6
    if ((cnd = strcmp(a->dfile, b->dfile)))   /* Sort on file */
E 6
E 2
      return(cnd);
I 6
    cnd = a->jdate - b->jdate;                /* sort by jdate */
    if (cnd != 0)
      return(cnd);
E 6
    return(-(b->foff - a->foff));             /* Sort on foff */
}

I 2
int remark_cmp(a, b)
struct remark *a, *b;
{
    if (a->commid == b->commid)
      return(a->lineno - b->lineno);
    else
      return(a->commid - b->commid);
}

E 2
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
D 2
    sprintf(name1, "%d/%s/%s", dt->date, wfdisc[i].dir, wfdisc[i].file);
E 2
I 2
    sprintf(name1, "%d/%s/%s", dt->date, wfdisc[i].dir, wfdisc[i].dfile);
E 2
    do {
        if (j < nrecs) {
	    dt->epoch = wfdisc[j].time;
	    etoh(dt);
D 2
            sprintf(name2, "%d/%s/%s", dt->date, wfdisc[j].dir, wfdisc[j].file);
E 2
I 2
            sprintf(name2, "%d/%s/%s", dt->date, wfdisc[j].dir, wfdisc[j].dfile);
E 2
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
D 2
        perror("migrate (malloc)");
E 2
I 2
        perror("migcmp (malloc)");
E 2
        exit(8);
    }

/*  Now repeat test, assigning groups  */
D 2
/*  Verify that dattyp is the same for each group  */
E 2
I 2
/*  Verify that datatype is the same for each group  */
E 2

    count = 0; i = 0; j = 1;
    dt->epoch = wfdisc[i].time;
    etoh(dt);
D 2
    sprintf(name1, "%d/%s/%s", dt->date, wfdisc[i].dir, wfdisc[i].file);
E 2
I 2
    sprintf(name1, "%d/%s/%s", dt->date, wfdisc[i].dir, wfdisc[i].dfile);
E 2
    do {
        if (j < nrecs) {
	    dt->epoch = wfdisc[j].time;
	    etoh(dt);
D 2
            sprintf(name2, "%d/%s/%s", dt->date, wfdisc[j].dir, wfdisc[j].file);
E 2
I 2
            sprintf(name2, "%d/%s/%s", dt->date, wfdisc[j].dir, wfdisc[j].dfile);
E 2
            doit = (strcmp(name1, name2) != 0);
        } else {
            doit = TRUE;
        }
        if (doit) {
            tmp.nrec = j - i;
            if ((tmp.wfdisc = (struct wfdisc *) 
                malloc(tmp.nrec*sizeof(struct wfdisc))) == NULL) {
D 2
                perror("migrate (malloc)");
E 2
I 2
                perror("migcmp (malloc)");
E 2
                exit(9);
            }
            for (k=0, l=i; k < tmp.nrec; k++, l++) {
                tmp.wfdisc[k] = wfdisc[l];
                if (
D 2
                    (strcmp(tmp.wfdisc[k].dattyp, "i2") != 0) &&
                    (strcmp(tmp.wfdisc[k].dattyp, "i4") != 0) &&
                    (strcmp(tmp.wfdisc[k].dattyp, "s2") != 0) &&
                    (strcmp(tmp.wfdisc[k].dattyp, "s4") != 0) &&
                    (strcmp(tmp.wfdisc[k].dattyp, "f4") != 0) &&
                    (strcmp(tmp.wfdisc[k].dattyp, "t4") != 0)
E 2
I 2
                    (strcmp(tmp.wfdisc[k].datatype, "i2") != 0) &&
                    (strcmp(tmp.wfdisc[k].datatype, "i4") != 0) &&
                    (strcmp(tmp.wfdisc[k].datatype, "s2") != 0) &&
                    (strcmp(tmp.wfdisc[k].datatype, "s4") != 0) &&
I 4
                    (strcmp(tmp.wfdisc[k].datatype, "c2") != 0) &&
                    (strcmp(tmp.wfdisc[k].datatype, "c4") != 0) &&
E 4
                    (strcmp(tmp.wfdisc[k].datatype, "f4") != 0) &&
                    (strcmp(tmp.wfdisc[k].datatype, "t4") != 0)
E 2
                ) {
D 2
                    fprintf(stderr,"migrate: unsupported dattyp ");
                    fprintf(stderr,"'%s' ", tmp.wfdisc[k].dattyp);
E 2
I 2
                    fprintf(stderr,"migcmp: unsupported datatype ");
                    fprintf(stderr,"'%s' ", tmp.wfdisc[k].datatype);
E 2
                    fprintf(stderr,"for file '%s/",  tmp.wfdisc[k].dir);
D 2
                    fprintf(stderr,"%s'\n", tmp.wfdisc[k].file);
E 2
I 2
                    fprintf(stderr,"%s'\n", tmp.wfdisc[k].dfile);
E 2
                    exit(10);
                }
D 10
                if (k > 0 && strcmp(tmp.wfdisc[k], tmp.wfdisc[k-1]) != 0) {
D 2
                    fprintf(stderr,"migrate: conficting dattyp's ");
                    fprintf(stderr,"('%s' and ", tmp.wfdisc[k-1].dattyp);
                    fprintf(stderr,"'%s') ", tmp.wfdisc[k].dattyp);
E 2
I 2
                    fprintf(stderr,"migcmp: conficting datatype's ");
E 10
I 10
                if (k && strcmp(tmp.wfdisc[k].datatype, tmp.wfdisc[k-1].datatype) != 0)
		{
                    fprintf(stderr,"migcmp: conflicting datatype's ");
E 10
                    fprintf(stderr,"('%s' and ", tmp.wfdisc[k-1].datatype);
                    fprintf(stderr,"'%s') ", tmp.wfdisc[k].datatype);
E 2
                    fprintf(stderr,"for file '%s/",  tmp.wfdisc[k].dir);
D 2
                    fprintf(stderr,"%s'\n", tmp.wfdisc[k].file);
E 2
I 2
                    fprintf(stderr,"%s'\n", tmp.wfdisc[k].dfile);
E 2
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
D 2
        lcase(strcpy(grp->wfdisc[i].file, new_name));
E 2
I 2
        lcase(strcpy(grp->wfdisc[i].dfile, new_name));
E 2
    }
    
}

D 9
int copy(ip, op, grp, compress)
E 9
I 9
int copy(ip, op, grp, compress, offset)
E 9
FILE *ip, *op;
struct group *grp;
D 9
int compress;
E 9
I 9
int compress, offset;
E 9
{
    char *dataout;
    long i, nsamp, ioff, oldoff, *longin, longcheck, longcsum, n28m1 = 134217727;
    short *shortin, shortcheck, n13m1 = 8191;
    int compnum, count;

D 9
    ioff = 0;
E 9
I 9
    ioff = offset;
E 9

    for (count = 0; count < grp->nrec; count++) {

	oldoff = ioff;

D 4
	if (!compress) {
E 4
I 4
	if (!compress || (grp->wfdisc[count].datatype[0] == 'c')) {
E 4
D 2
  	    if (strstr(grp->wfdisc[count].dattyp, "s4") != NULL) {  
E 2
I 2
  	    if (strstr(grp->wfdisc[count].datatype, "s4") != NULL) {  
E 2
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
I 7
		free(longin);
E 7
		ioff = ioff + nsamp * sizeof(long);
D 4
	    } else {
E 4
I 4
	    } else if (strstr(grp->wfdisc[count].datatype, "s2") != NULL) {  
E 4
		/* Short data, no compression */
	        shortin = (short *) malloc((grp->wfdisc[count].nsamp + 10) 
					 * sizeof(short));
		fseek(ip, grp->wfdisc[count].foff, 0);
		nsamp = fread(shortin, sizeof(short), grp->wfdisc[count].nsamp,
			      ip);
I 6
D 7
fprintf(stderr, "%d\n", grp->wfdisc[count].jdate);
fprintf(stderr, "%s/%s\n", grp->wfdisc[count].dir, grp->wfdisc[count].dfile);
fprintf(stderr, "nsamp = %ld    wfnsamp = %ld\n", nsamp,grp->wfdisc[count].nsamp); 
E 7
E 6
		if (fwrite(shortin, sizeof(short), nsamp, op) != nsamp) {
		    perror(opath);
		    exit(12);
		}
I 7
		free(shortin);
E 7
		ioff = ioff + nsamp * sizeof(short);
I 4
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
I 7
		free(dataout);
E 7
		ioff = ioff + nsamp * sizeof(char);
E 4
	    }
        } else {		
	    shortin = (short *) malloc((grp->wfdisc[count].nsamp + 10)
				       * sizeof(short));

	    longin = (long *) malloc((grp->wfdisc[count].nsamp + 10)
				     * sizeof(long));
	    
	    fseek(ip, grp->wfdisc[count].foff, 0);
	    
D 2
	    if (strstr(grp->wfdisc[count].dattyp, "s4") != NULL)
E 2
I 2
	    if (strstr(grp->wfdisc[count].datatype, "s4") != NULL)
E 2
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
D 6
	    
E 6
I 6

E 6
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

I 4
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

E 4
int help()
{
I 4
    fprintf(stderr,"migcmp version %s\n", SccsId);
E 4
D 2
    fprintf(stderr,"usage: migcmp base=dir ");
    fprintf(stderr,"[+/-v -c +/-rename mode=0xxx] < wfdisc > wfdisc\n");
E 2
I 2
    fprintf(stderr,"usage: migcmp base=dir inwfd= outwfd= ");
D 4
    fprintf(stderr,"[+/-v -c +/-rename mode=0xxx]\n");
E 4
I 4
D 9
    fprintf(stderr,"[+/-v +/-c +/-d +/-rename mode=0xxx]\n");
E 9
I 9
    fprintf(stderr,"[+/-v +/-c +/-d +a +/-rename mode=0xxx]\n");
E 9
    fprintf(stderr,"     base is the base directory to write data to\n");
    fprintf(stderr,"     inwfd and outwfd are prefixes for input/output wfdiscs and remarks\n");
    fprintf(stderr,"     +v turns on verbose mode\n");
    fprintf(stderr,"     +c turns on data compression\n");
    fprintf(stderr,"     +d turns on data decompression\n");
I 9
    fprintf(stderr,"     +a turns on append mode during compression\n");
E 9
    fprintf(stderr,"         NOTE: only one options (+c or +d) may be used at any one time\n");
    fprintf(stderr,"     +rename turns on file renaming (new names in format sta-chan-hhmm.w)\n");
    fprintf(stderr,"     mode is the mode to set the data files to after writing\n");
E 4
E 2
    exit(1);
}

E 1
