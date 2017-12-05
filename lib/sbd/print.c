#pragma ident "$Id: print.c,v 1.3 2015/12/04 22:53:02 dechavez Exp $"
/*======================================================================
 *
 *  Print SBD IE contents
 *
 *====================================================================*/
#include "sbd.h"

void sbdPrintMessageIE(FILE *fp, SBD_MESSAGE *message)
{
    if (fp == NULL || message == NULL) return;

    if (message->mo.header.valid) {
        fprintf(fp, "MO header: ");
        fprintf(fp, "reference number = %u ",  message->mo.header.cdr);
        fprintf(fp,             "IMEI = %s ",  message->mo.header.imei);
        fprintf(fp,           "status = %d ",  message->mo.header.status);
        fprintf(fp,            "momsn = %hd ", message->mo.header.momsn);
        fprintf(fp,            "mtmsn = %hd ", message->mo.header.mtmsn);
        fprintf(fp,           "tstamp = %s\n", utilLttostr(message->mo.header.tstamp, 0, NULL));
    }

    if (message->mo.location.valid) {
        fprintf(fp, "MO location: ");
        fprintf(fp,   "latitude = %.4f ",  message->mo.location.lat);
        fprintf(fp,  "longitude = %.4f ",  message->mo.location.lon);
        fprintf(fp, "CEP radius = %d\n",   message->mo.location.cep);
    }

    if (message->mo.confirm.valid) {
        fprintf(fp, "MO confirmation = %s\n", message->mo.confirm.status ? "Success" : "FAILURE");
    }

    if (message->mo.payload.valid) {
        fprintf(fp, "%d bytes of MO payload\n", message->mo.payload.len);
        utilPrintHexDump(fp, message->mo.payload.data, message->mo.payload.len);
    }

    if (message->mt.header.valid) {
        fprintf(fp, "MT header: ");
        fprintf(fp, "unique client ID = %s ",      message->mt.header.myid);
        fprintf(fp,             "IMEI = %s ",      message->mt.header.imei);
        fprintf(fp,            "flags = 0x%04x\n", message->mt.header.flags);
    }

    if (message->mt.priority.valid) {
        fprintf(fp, "MT priority = %d\n", message->mt.priority.level);
    }

    if (message->mt.confirm.valid) {
        fprintf(fp, "MT conformation: ");
        fprintf(fp, "unique client ID = %s ",  message->mt.confirm.myid);
        fprintf(fp,             "IMEI = %s ",  message->mt.confirm.imei);
        fprintf(fp, "reference number = %u ",  message->mt.confirm.cdr);
        fprintf(fp,           "status = %d\n", message->mt.confirm.status);
    }

    if (message->mt.payload.valid) {
        fprintf(fp, "%d bytes of MT payload\n", message->mt.payload.len);
        utilPrintHexDump(fp, message->mt.payload.data, message->mt.payload.len);
    }
}

/* Revision History
 *
 * $Log: print.c,v $
 * Revision 1.3  2015/12/04 22:53:02  dechavez
 * casts, format fixes and the like, all to calm OS X compiles
 *
 * Revision 1.2  2013/03/15 21:37:47  dechavez
 * fixed bug with mo.location.cep formatting
 *
 * Revision 1.1  2013/03/13 21:27:09  dechavez
 * created, introduces sbdPrintMessageIE()
 *
 */
