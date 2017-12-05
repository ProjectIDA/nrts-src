#pragma ident "$Id: ca.c,v 1.2 2011/02/25 18:54:58 dechavez Exp $"
/*======================================================================
 *
 * Deal with CA records (not that any really exist)
 *
 *====================================================================*/
#include "dccdmx.h"

#define MY_MOD_ID DCCDMX_MOD_CA

static FILE_HANDLE handle;

void ProcessCA(UINT8 *buf, INT32 buflen, COUNTER *count)
{
static char *fid = "ProcessCA";

    ++count->ca;

    if (fwrite(buf, 1, buflen, handle.fp) != buflen) {
        LogMsg("ERROR: %s: fwrite: %s: %s\n", fid, handle.path, strerror(errno));
        Exit(MY_MOD_ID + 1);
    }
}

void CloseCA(void)
{
    if (handle.fp == NULL) fclose(handle.fp);
}

void InitCA(char *ident)
{
static char *fid = "InitCA";

    sprintf(handle.path, "%s.ca", ident);
    if ((handle.fp = fopen(handle.path, "wb")) == NULL) {
        LogMsg("ERROR: %s: fopen: %s: %s", fid, handle.path, strerror(errno));
        Exit(MY_MOD_ID + 2);
    }
}

/* Revision History
 *
 * $Log: ca.c,v $
 * Revision 1.2  2011/02/25 18:54:58  dechavez
 * initial release
 *
 */
