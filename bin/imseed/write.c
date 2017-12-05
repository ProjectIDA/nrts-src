#pragma ident "$Id: write.c,v 1.7 2015/12/05 00:30:27 dechavez Exp $"
/*======================================================================
 *
 *  Call-back function invoked by mseedPackRecord() each time it has a
 *  MiniSEED record ready for the application.
 *
 *  Write MiniSEED output.  If disk option is selected then the data
 *  are written in individual files with names of the form
 *  STA.NN.LC.CHN.YEAR.DAY, and sequence numbers in each file start by
 *  1 and increment by 1.
 *
 *  If standard out is selected then the first record emitted begins
 *  with sequence number 1 and all subsquent records increment the by 1.
 *
 *====================================================================*/
#include "imseed.h"

#define MY_MOD_ID MOD_WRITE

static char *root = NULL;
static BOOL UseStdout = FALSE;
static UINT32 nrec = 0;
LNKLST *head = NULL;

typedef struct {
    FILE *fp;
    char *path;
} PATH_HANDLE;

static PATH_HANDLE *LocateHandle(char *path)
{
PATH_HANDLE *handle;
LNKLST_NODE *crnt;

    crnt = listFirstNode(head);
    while (crnt != NULL) {
        handle = (PATH_HANDLE *) crnt->payload;
        if (path == NULL) {
            if (handle->path == NULL) return handle;
        } else {
            if (strcmp(path, handle->path) == 0) return handle;
        }
        crnt = listNextNode(crnt);
    }

    return NULL;
}

static PATH_HANDLE *CreateHandle(char *path)
{
static int count = 0;
PATH_HANDLE new, *pnew;
static char *fid = "WriteRecord:CreateHandle";

    if (path == NULL) {
        new.path = NULL;
        new.fp   = stdout;
    } else if ((new.path = strdup(path)) == NULL) {
        LogErr("%s: strdup: %s (FATAL ERROR)\n", fid, strerror(errno));
        GracefulExit(MY_MOD_ID + 1);
    } else if ((new.fp = fopen(new.path, "w")) == NULL) {
        LogErr("%s: fopen: %s: %s (FATAL ERROR)\n", fid, new.path, strerror(errno));
        GracefulExit(MY_MOD_ID + 2);
    }

    if (!listAppend(head, (void *) &new, sizeof(PATH_HANDLE))) {
        LogErr("%s: listAppend: %s (FATAL ERROR)\n", fid, strerror(errno));
        GracefulExit(MY_MOD_ID + 3);
    }

    if ((pnew = LocateHandle(path)) == NULL) {
        LogErr("%s: can't locate my own path!: %s (FATAL ERROR)\n", fid, strerror(errno));
        GracefulExit(MY_MOD_ID + 4);
    }

    return pnew;
}

static PATH_HANDLE *GetHandle(char *path)
{
PATH_HANDLE *handle;
static char *fid = "WriteRecord:GetHandle";

    if ((handle = LocateHandle(path)) == NULL) handle = CreateHandle(path);

    if (handle == NULL) {
        LogErr("%s: LocateHandle: %s (FATAL ERROR)\n", fid, strerror(errno));
        GracefulExit(MY_MOD_ID + 5);
    }

    return handle;
}

void CloseFiles(void)
{
int nfile = 0;
PATH_HANDLE *handle;
LNKLST_NODE *crnt;
char cwd[MAXPATHLEN+1];

    if (UseStdout) {
        LogErr("%lu records written to standard output\n", nrec);
        return;
    }

    crnt = listFirstNode(head);
    while (crnt != NULL) {
        handle = (PATH_HANDLE *) crnt->payload;
        if (handle->fp != NULL) {
            ++nfile;
            fclose(handle->fp);
        }
        crnt = listNextNode(crnt);
    }
    getcwd(cwd, MAXPATHLEN+1);
    LogErr("%lu records written to %d files in %s/%s\n", nrec, nfile, cwd, root);
}

void WriteRecord(void *argptr, MSEED_PACKED *packed)
{
PATH_HANDLE *handle;
int i, year, day, unused;
char *path, buffer[MAXPATHLEN+1];
static char *fid = "WriteRecord";

    if (packed == NULL) {
        LogErr("%s: UNEXPECTED ERROR: NULL packed!\n", fid);
        GracefulExit(MY_MOD_ID + 6);
    }

/* Get (or create) the path handle for this record */

    if (UseStdout) {
        path = NULL;
    } else {
        utilDecomposeTimestamp(packed->hdr.tstamp, &year, &day, &unused, &unused, &unused, &unused);
        sprintf(buffer, "%s/%s.%s.%s.%s.%04d.%03d", root, packed->hdr.staid, packed->hdr.netid, packed->hdr.locid, packed->hdr.chnid, year, day);
        path = buffer;
    }
    handle = GetHandle(path);

/* Write out the record */

    if (fwrite(packed->data, packed->len, 1, handle->fp) != 1) {
        LogErr("%s: fwrite: %s: %s: (FATAL ERROR)\n", fid, handle->path == NULL ? "stdout" : handle->path, strerror(errno));
        GracefulExit(MY_MOD_ID + 7);
    }
    ++nrec;

/* Don't forget to free the packet now that we are done with it! */

    free(packed);
}

int InitWriter(char *use_root)
{
int seqno;
static char *fid = "WriteInit";

/* save the output dir */

    if (use_root != NULL && (root = strdup(use_root)) == NULL) {
        fprintf(stderr, "%s: strdup: %s\n", fid, strerror(errno));
        exit(MY_MOD_ID + 10);
    }

/* initialize the linked list we will use to keep track of sequence numbers */

    if ((head = listCreate()) == NULL) {
        fprintf(stderr, "%s: listCreate: %s (FATAL ERROR)\n", fid, strerror(errno));
        exit(MY_MOD_ID + 12);
    }

/* NULL use_root means we will be writing to standard output */

    if (use_root == NULL) {
        UseStdout = TRUE;
        seqno = 0;
    } else {
        /* otherwise, output directory must not already exist */
        if (utilFileExists(use_root) || utilDirectoryExists(use_root)) {
            fprintf(stderr, "%s: %s already exists\n", fid, use_root);
            exit(MY_MOD_ID + 13);
        }
        /* we create it here */
        if (util_mkpath(root, 0775) != 0) {
            fprintf(stderr, "%s: util_mkpath: %s: %s\n", fid, root, strerror(errno));
            exit(MY_MOD_ID + 14);
        }
        seqno = -1;
    }

    return seqno;
}

/* Revision History
 *
 * $Log: write.c,v $
 * Revision 1.7  2015/12/05 00:30:27  dechavez
 * excised some dead debug code
 *
 * Revision 1.6  2014/08/19 18:18:21  dechavez
 * rework to used MSEED_PACKED instead of MSEED_RECORD
 *
 * Revision 1.5  2014/08/11 20:45:06  dechavez
 * Rework to support libmseed 2.0.0
 *
 * Revision 1.4  2014/04/30 22:51:28  dechavez
 * eliminated the incorrect output file counter
 *
 * Revision 1.3  2014/04/14 19:11:03  dechavez
 * user supplied MSRecord *msr instead of static version
 *
 * Revision 1.2  2014/03/04 21:05:40  dechavez
 * removed a bunch of tabs, removed the dataquality update (now handled by the template)
 *
 * Revision 1.1  2014/02/05 21:16:25  dechavez
 * initial release
 *
 */
