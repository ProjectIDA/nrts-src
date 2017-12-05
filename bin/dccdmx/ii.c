#pragma ident "$Id: ii.c,v 1.3 2011/02/25 18:54:58 dechavez Exp $"
/*======================================================================
 *
 * Deal with ISPLOG records
 *
 *====================================================================*/
#include "dccdmx.h"

#define MY_MOD_ID DCCDMX_MOD_II

static FILE_HANDLE handle;

void ProcessII(UINT8 *buf, INT32 buflen, COUNTER *count)
{
static char *fid = "ProcessII";

    ++count->ii;

	fprintf(handle.fp, "%s", buf+2);
    //if (fwrite(buf, 1, buflen, handle.fp) != buflen) {
        //LogMsg("ERROR: %s: fwrite: %s: %s\n", fid, handle.path, strerror(errno));
        //Exit(MY_MOD_ID + 1);
    //}

}

void CloseII(void)
{
    if (handle.fp == NULL) fclose(handle.fp);
}

void InitII(char *ident)
{
static char *fid = "InitII";

    sprintf(handle.path, "%s.isplog", ident);
    if ((handle.fp = fopen(handle.path, "wb")) == NULL) {
        LogMsg("ERROR: %s: fopen: %s: %s", fid, handle.path, strerror(errno));
        Exit(MY_MOD_ID + 2);
    }
}

/* Revision History
 *
 * $Log: ii.c,v $
 * Revision 1.3  2011/02/25 18:54:58  dechavez
 * initial release
 *
 */
