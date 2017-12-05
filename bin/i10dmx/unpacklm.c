#pragma ident "$Id: unpacklm.c,v 1.2 2015/12/07 19:55:33 dechavez Exp $"
/*======================================================================
 *
 *  Unpack LM (Log Message) records.
 *
 *====================================================================*/
#include <stdio.h>
#include <errno.h>
#include <memory.h>
#include "i10dmx.h"

extern char *Buffer;

void unpackLM(UINT8 *buf, UINT32 recno)
{
FILE *fp;
char *path;
IDA10_LM lm;
static char *fid = "unpackLM";

    if (!ida10UnpackLM(buf, &lm)) {
        sprintf(Buffer, "%s: can't decode LM record no. %u\n", fid, recno);
        logmsg(Buffer);
        return;
    }

    path = logname(lm.sname);
    if ((fp = fopen(path, "a")) == NULL) {
        sprintf(Buffer, "%s: fopen: %s: %s", fid, path, strerror(errno));
        logmsg(Buffer);
    } else {
        fprintf(fp, "%s", lm.text);
        fclose(fp);
    }
}

/* Revision History
 *
 * $Log: unpacklm.c,v $
 * Revision 1.2  2015/12/07 19:55:33  dechavez
 * all sorts of changes to calm OS X builds
 *
 * Revision 1.1.1.1  2000/02/08 20:20:01  dec
 * import existing IDA/NRTS sources
 *
 */
