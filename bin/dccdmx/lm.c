#pragma ident "$Id: lm.c,v 1.3 2011/02/25 18:54:58 dechavez Exp $"
/*======================================================================
 *
 * Deal with LM records
 *
 *====================================================================*/
#include "dccdmx.h"

#define MY_MOD_ID DCCDMX_MOD_LM

static FILE_HANDLE handle;

void ProcessLM(UINT8 *buf, INT32 buflen, COUNTER *count)
{
IDA10_LM lm;
static char *fid = "ProcessLM";

    ++count->lm;

    if (!ida10UnpackLM(buf, &lm)) {
        LogMsg("ERROR: unpack error for record %ld (LM): %s\n", count->total, strerror(errno));
        ++count->error;
        return;
    }

    fprintf(handle.fp, "%s", lm.text);
}

void CloseLM(void)
{
    if (handle.fp == NULL) fclose(handle.fp);
}

void InitLM(char *ident)
{
static char *fid = "InitLM";

    sprintf(handle.path, "%s.daslog", ident);
    if ((handle.fp = fopen(handle.path, "wb")) == NULL) {
        LogMsg("ERROR: %s: fopen: %s: %s", fid, handle.path, strerror(errno));
        Exit(MY_MOD_ID + 2);
    }
}

/* Revision History
 *
 * $Log: lm.c,v $
 * Revision 1.3  2011/02/25 18:54:58  dechavez
 * initial release
 *
 */
