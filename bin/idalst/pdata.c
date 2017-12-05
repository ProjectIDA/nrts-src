#pragma ident "$Id: pdata.c,v 1.8 2012/02/14 21:13:51 dechavez Exp $"
/*======================================================================
 *
 *  Print data record header contents.
 *
 *====================================================================*/
#include "idalst.h"

void pdata (IDA_DREC *drec, struct counters *count, int print)
{
INT16 *sdata;
INT32  *ldata;
IDA_DHDR *head;
char tbuf[64];
static INT32 tagno = DATA_TAG;
char *patch;
#define PATCH_NONE " "
#define PATCHED_LEAP_YEAR "L"
#define PATCHED_910100    "%"
#define PATCHED_BOTH      "B"

    sdata = (INT16 *) drec->data;
    ldata = (INT32  *) drec->data;
    head  = &drec->head;

    if (tagno == DATA_TAG) {
        tag(tagno++); printf("\n");
        tag(tagno++); printf("DATA RECORD HEADER SUMMARY\n");
        tag(tagno++); printf("Id Record DAC B Pts   Sint   Sys ");
                      printf("Time-Tag    Q  Corrected Time-Tag");
                      if (print & P_SEQNO) printf("     Sequence #   Creation Tstamp");
                      printf("\n");
    }

    tag(tagno++);
    printf("%02hd", head->dl_stream);
    printf("%7ld",  count->rec);
    if (head->atod == IDA_DAS) {
        printf(" DAS");
    } else if (head->atod == IDA_ARS) {
        printf(" ARS");
    } else {
        printf(" ???");
    }
    printf("%2d",   head->wrdsiz);
    printf("%4d",   head->nsamp);
    printf("%7.3f", head->sint);
    printf(" %s",   sys_timstr(head->beg.sys, head->beg.mult, NULL));
    printf("  %d",  head->beg.qual);
    patch = PATCH_NONE;
    if (head->beg.error & TTAG_LEAP_YEAR_PATCH) {
        patch = PATCHED_LEAP_YEAR;
    }
    if (head->beg.error & TTAG_91011_PATCH) {
        patch = (head->beg.error & TTAG_LEAP_YEAR_PATCH) ? PATCHED_BOTH : PATCHED_910100;
    }
    printf("%s", patch);
    printf("%s",   tru_timstr(&head->beg, NULL));
    if (print & P_SEQNO && head->extra.valid) printf("   0x%08x  %s", head->extra.seqno, utilLttostr(head->extra.tstamp, 1000, tbuf));
    printf("\n");

}

/* Revision History
 *
 * $Log: pdata.c,v $
 * Revision 1.8  2012/02/14 21:13:51  dechavez
 * use portable integer types (eg, long to INT32, unsigned long to UINT32)
 *
 * Revision 1.7  2011/10/13 18:04:53  dechavez
 * print % if 9/10/11 patch in place, B if both 9/10/11 and Leap year patch are present
 *
 * Revision 1.6  2008/03/05 23:02:16  dechavez
 * note packets with leap year bug bit set by appending and L after clock quality
 *
 * Revision 1.5  2008/01/16 23:54:00  dechavez
 * add new xxx_timstr() buffer args
 *
 * Revision 1.4  2006/02/09 20:17:15  dechavez
 * support for libida 4.0.0 (IDA handle) and libisidb (proper database)
 *
 * Revision 1.3  2005/08/26 20:22:42  dechavez
 * print sequence number and creation time stamps, if applicable
 *
 * Revision 1.2  2003/06/11 20:26:21  dechavez
 * Cleaned up includes and Makefile
 *
 * Revision 1.1.1.1  2000/02/08 20:20:04  dec
 * import existing IDA/NRTS sources
 *
 */
