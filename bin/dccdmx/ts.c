#pragma ident "$Id: ts.c,v 1.2 2011/02/25 18:54:58 dechavez Exp $"
/*======================================================================
 *
 * Deal with TS records
 *
 *====================================================================*/
#include "dccdmx.h"

#define MY_MOD_ID DCCDMX_MOD_TS

static char *ident;
static LNKLST *list;
static FILE_HANDLE hdr;

static FILE_HANDLE *SearchList(char *cname)
{
FILE_HANDLE *handle;
LNKLST_NODE *crnt;

    crnt = listFirstNode(list);
    while (crnt != NULL) {
        handle = (FILE_HANDLE *) crnt->payload;
        if (strcmp(handle->cname, cname) == 0) return handle;
        crnt = listNextNode(crnt);
    }

    return NULL;
}

static FILE_HANDLE *GetTsHandle(char *cname)
{
FILE_HANDLE new, *handle;
static char *fid = "GetTsHandle";

    if ((handle = SearchList(cname)) != NULL) return handle;

    strncpy(new.cname, cname, IDA10_CNAMLEN);
    sprintf(new.path, "%s-%s.w", ident, cname);
    if ((new.fp = fopen(new.path, "wb")) == NULL) {
        LogMsg("ERROR: %s: fopen: %s: %s\n", fid, new.path, strerror(errno));
        Exit(MY_MOD_ID + 1);
    }

    if (!listAppend(list, &new, sizeof(FILE_HANDLE))) {
        LogMsg("ERROR: %s: listAppend: %s: %s\n", fid, new.path, strerror(errno));
        Exit(MY_MOD_ID + 2);
    }

    if ((handle = SearchList(cname)) == NULL) {
        LogMsg("*IMPOSSIBLE*ERROR: %s: SearchList fails on new entry!\n", fid);
        Exit(MY_MOD_ID + 3);
    }

    return handle;
}

void ProcessTS(UINT8 *buf, INT32 unused, COUNTER *count)
{
int i;
IDA10_TS ts;
FILE_HANDLE *handle;
static char *fid = "ProcessTS";

    ++count->ts;

    if (!ida10UnpackTS(buf, &ts)) {
        LogMsg("ERROR: unpack error for record %ld (TS): %s\n", count->total, strerror(errno));
        ++count->error;
        return;
    }

    for (i = 0; i < strlen(ts.hdr.cname); i++) if (ts.hdr.cname[i] == ' ') ts.hdr.cname[i] = '_';

    if (fwrite(buf, 1, IDA10_TSHEADLEN, hdr.fp) != IDA10_TSHEADLEN) {
        LogMsg("ERROR: %s: fwrite: %s: %s\n", fid, hdr.path, strerror(errno));
        Exit(MY_MOD_ID + 4);
    }

    handle = GetTsHandle(ts.hdr.cname);
    if (fwrite(ts.data.int8, 1, ts.hdr.nbytes, handle->fp) != ts.hdr.nbytes) {
        LogMsg("ERROR: %s: fwrite: %s: %s\n", fid, handle->path, strerror(errno));
        Exit(MY_MOD_ID + 5);
    }
}

void CloseTS(void)
{
LNKLST_NODE *crnt;
FILE_HANDLE *handle;

    crnt = listFirstNode(list);
    while (crnt != NULL) {
        handle = (FILE_HANDLE *) crnt->payload;
        fclose(handle->fp);
        crnt = listNextNode(crnt);
    }

    if (hdr.fp == NULL) fclose(hdr.fp);
}

void InitTS(char *string)
{
static char *fid = "InitTS";

    ident = string;

    if ((list = listCreate()) == NULL) {
        LogMsg("ERROR: %s: listCreate: %s\n", fid, strerror(errno));
        Exit(MY_MOD_ID + 6);
    }

    sprintf(hdr.path, "%s.hdr", ident);
    if ((hdr.fp = fopen(hdr.path, "wb")) == NULL) {
        LogMsg("ERROR: %s: fopen: %s: %s", fid, hdr.path, strerror(errno));
        Exit(MY_MOD_ID + 7);
    }
}

/* Revision History
 *
 * $Log: ts.c,v $
 * Revision 1.2  2011/02/25 18:54:58  dechavez
 * initial release
 *
 */
