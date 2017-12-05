#pragma ident "$Id: main.c,v 1.8 2009/07/09 18:42:26 dechavez Exp $"
/*======================================================================
 *
 *  Dump ISI_RAW_PACKETs from ISI disk loops
 *
 *====================================================================*/
#define INCLUDE_ISI_STATIC_SEQNOS
#include "ida.h"
#include "isi.h"
#include "isi/dl.h"
#include "util.h"
#include "qdplus.h"

extern char *VersionIdentString;
static BOOL verbose = FALSE;
static BOOL dumpdata = TRUE;
static char *Myname;

static void help(char *myname)
{
    fprintf(stderr, "usage: %s site [ { -V | -v } beg=seqno {count=value | end=seqno} ]\n", myname);
    exit(1);
}

static void DecodeAndPrintIDA(FILE *fp, IDA *ida, UINT8 *buf)
{
IDA_DHDR head;
int status;
char tbuf[1024];

    switch (ida_rtype(buf, ida->rev.value)) {
      case IDA_DATA:
        if ((status = ida_dhead(ida, &head, buf)) != 0) {
            fprintf(fp, "IDA_DATA: ida_dhead error status %d\n", status);
        } else {
            fprintf(fp, "%02hd", head.dl_stream);
            if (head.atod == IDA_DAS) {
                fprintf(fp, " DAS");
            } else if (head.atod == IDA_ARS) {
                fprintf(fp, " ARS");
            } else {
                fprintf(fp, " ???");
            }
            fprintf(fp, "%2d",   head.wrdsiz);
            fprintf(fp, "%4d",   head.nsamp);
            fprintf(fp, "%7.3f", head.sint);
            fprintf(fp, " %s",   sys_timstr(head.beg.sys, head.beg.mult, NULL));
            fprintf(fp, "  %d",  head.beg.qual);
            fprintf(fp, " %s",   tru_timstr(&head.beg, NULL));
            if (head.extra.valid) fprintf(fp, "   0x%08x  %s", head.extra.seqno, utilLttostr(head.extra.tstamp, 1000, tbuf));
            fprintf(fp, "\n");

        }
        break;
      case IDA_CALIB:
        fprintf(fp, "IDA_CALIB\n");
        break;
      case IDA_EVENT:
        fprintf(fp, "IDA_EVENT\n");
        break;
      case IDA_IDENT:
        fprintf(fp, "IDA_IDENT\n");
        break;
      case IDA_CONFIG:
        fprintf(fp, "IDA_CONFIG\n");
        break;
      case IDA_LOG:
        fprintf(fp, "IDA_LOG\n");
        break;
      case IDA_ISPLOG:
        fprintf(fp, "IDA_ISPLOG\n");
        break;
      case IDA_DASSTAT:
        fprintf(fp, "IDA_DASSTAT\n");
        break;
      default:
        fprintf(fp, "IDA_UNKNOWN\n");
        break;
    }
}

static void DecodeAndPrintIDA10(FILE *fp, UINT8 *buf)
{
IDA10_CMNHDR cmnhdr;
char tmpbuf[1024];

    ida10UnpackCmnHdr(buf, &cmnhdr);
    fprintf(fp, "%s\n", ida10CMNHDRtoString(&cmnhdr, tmpbuf));
}

static void DecodeAndPrintQDPLUS(FILE *fp, UINT8 *buf)
{
QDPLUS_PKT pkt;

    qdplusUnpackWrappedQDP(buf, &pkt);
    if (!qdpVerifyCRC(&pkt.qdp)) fprintf(fp, "*** CRC ERROR *** ");
    qdplusPrintPkt(fp, &pkt, QDP_PRINT_HDR);
}

static void DecodeAndPrintRaw(FILE *fp, ISI_DL *dl, ISI_RAW_PACKET *raw)
{
int i;
int rev = 0;
static IDA *ida = NULL;

    for (i = 0; i < raw->hdr.len.used && raw->payload[i] == 0; i++);
    if (i == raw->hdr.len.used) {
        fprintf(fp, "NULL packet (all zeros)\n");
        return;
    }

    if (rev != -1 && ida == NULL) {
        switch (raw->hdr.desc.type) {
          case ISI_TYPE_IDA8: rev = 8; break;
          case ISI_TYPE_IDA9: rev = 9; break;
          case ISI_TYPE_IDA10: rev = 10; break;
          default: rev = -1;
        }
        if (rev > 0) {
            ida = idaCreateHandle(dl->sys->site, rev, NULL, dl->glob->db, dl->lp, 0);
            if (ida == NULL) {
                fprintf(stderr, "ABORT: idaCreateHandle: site=%s, rev=%d: %s\n", dl->sys->site, rev, strerror(errno));
                exit(1);
            }
        }
    }
    switch (raw->hdr.desc.type) {
      case ISI_TYPE_IDA8: DecodeAndPrintIDA(fp, ida, raw->payload); break;
      case ISI_TYPE_IDA9: DecodeAndPrintIDA(fp, ida, raw->payload); break;
      case ISI_TYPE_IDA10: DecodeAndPrintIDA10(fp, raw->payload); break;
      case ISI_TYPE_QDPLUS: DecodeAndPrintQDPLUS(fp, raw->payload); break;
      default: fprintf(fp, "missing DecodeAndPrint function for ISI type %d\n", raw->hdr.desc.type);
    }
}

static void ProcessRange(ISI_DL *dl, UINT32 beg, UINT32 end, ISI_RAW_PACKET *raw)
{
FILE *fp;
UINT32 index;

    fp = dumpdata ? stderr : stdout;

    if (verbose) fprintf(fp, "dumping %lu packets from inidices %lu through %lu\n", end - beg + 1, beg, end);

    for (index = beg; index <= end; index++) {
        if (!isidlReadDiskLoop(dl, raw, index)) {
            fprintf(stderr, "isidlReadDiskLoop failed for site=%s index=%lu\n", dl->sys->site, index);
            exit(1);
        }

        if (verbose) {
            fprintf(fp, "%lu (%s): ", index, isiSeqnoString(&raw->hdr.seqno, NULL));
            DecodeAndPrintRaw(fp, dl, raw);
        }

        if (dumpdata) {
            if (fwrite(raw->payload, sizeof(UINT8), raw->hdr.len.used, stdout) !=  raw->hdr.len.used) {
                fprintf(stderr, "%s: fwrite: %s\n", Myname, strerror(errno));
                exit(1);
            }
        }
    }
}

int main(int argc, char **argv)
{
char *site = NULL;
struct {
    UINT32 index;
    ISI_SEQNO seqno;
    BOOL set;
} beg, end;
UINT32 i, count = 0;;
ISI_RAW_PACKET *raw;
ISI_GLOB glob;
ISI_DL *dl, snapshot;
ISI_DL_SYS sys;
char tmp[1024];
char *string;
char *dbspec = NULL;

    Myname = argv[0];
    beg.set = end.set = FALSE;

    if (argc < 1) help(Myname);

    for (i = 1; i < argc; i++) {
        if (strcasecmp(argv[i], "-h") == 0) {
            help(Myname);
        } else if (strcmp(argv[i], "-v") == 0) {
            verbose = TRUE;
            dumpdata = FALSE;
        } else if (strcmp(argv[i], "-V") == 0) {
            verbose = TRUE;
            dumpdata = TRUE;
        } else if (strncmp(argv[i], "db=", strlen("db=")) == 0) {
            dbspec = argv[i] + strlen("db=");
        } else if (strncmp(argv[i], "beg=", strlen("beg=")) == 0) {
            string = argv[i] + strlen("beg=");
            if (!isiStringToSeqno(string, &beg.seqno)) {
                fprintf(stderr, "%s: illegal beg string '%s'\n", Myname, string);
                exit(1);
            }
            beg.set = TRUE;
        } else if (strncmp(argv[i], "end=", strlen("end=")) == 0) {
            string = argv[i] + strlen("end=");
            if (!isiStringToSeqno(string, &end.seqno)) {
                fprintf(stderr, "%s: illegal end string '%s'\n",  Myname,string);
                exit(1);
            }
            end.set = TRUE;
        } else if (strncmp(argv[i], "count=", strlen("count=")) == 0) {
            count = (UINT32) strtol(argv[i]+strlen("count="), NULL, 0);
        } else if (site == NULL) {
            site = argv[i];
        } else {
            help(Myname);
        }
    }

    if (site == NULL) {
        fprintf(stderr, "%s: missing site name\n", Myname);
        exit(1);
    }

    if (!isidlSetGlobalParameters(dbspec, Myname, &glob)) {
        fprintf(stderr, "%s: isidlSetGlobalParameters: %s\n", Myname, strerror(errno));
        exit(1);
    }

    if ((raw = isiAllocateRawPacket(IDA_BUFLEN)) == NULL) {
        fprintf(stderr, "%s: isiAllocateRawPacket: %s\n", Myname, strerror(errno));
        exit(1);
    }
    if ((dl = isidlOpenDiskLoop(&glob, site, NULL, ISI_RDONLY)) == NULL) {
        fprintf(stderr, "%s: isidlOpenDiskLoop failed for site=%s\n", Myname, site);
        exit(1);
    }

    if (!isidlSnapshot(dl, &snapshot, &sys)) {
        fprintf(stderr, "%s: isidlSnapshot failed for site=%s\n", Myname, site);
        exit(1);
    }
    if (dl->sys->count == 0) {
        fprintf(stderr, "%s: %s disk loop is empty\n", Myname, site);
        exit(1);
    }

    if (!beg.set) {
        beg.index = snapshot.sys->index.oldest;
    } else {
        beg.index = isidlSearchDiskLoopForSeqno(&snapshot, &beg.seqno, ISI_UNDEFINED_INDEX, ISI_UNDEFINED_INDEX);
        if (beg.index == ISI_CORRUPT_INDEX || beg.index == ISI_UNDEFINED_INDEX) {
            fprintf(stderr, "%s: Unable to locate packet with seqno = %s\n", Myname, isiSeqnoString(&raw->hdr.seqno, tmp));
            if (beg.index == ISI_CORRUPT_INDEX) fprintf(stderr, "Disk loop appears to be corrupt.\n");
            exit(1);
        }
    }

    if (count != 0 && end.set) {
        fprintf(stderr, "%s: count and end options are mutually exclusive\n", Myname);
        exit(1);
    }

    if (count) {
        end.index = beg.index + count - 1;
        end.set = TRUE;
    } else if (end.set) {
        end.index = isidlSearchDiskLoopForSeqno(&snapshot, &end.seqno, ISI_UNDEFINED_INDEX, ISI_UNDEFINED_INDEX);
        if (end.index == ISI_CORRUPT_INDEX || end.index == ISI_UNDEFINED_INDEX) {
            fprintf(stderr, "%s: Unable to locate packet with seqno = %s\n", Myname, isiSeqnoString(&raw->hdr.seqno, tmp));
            if (end.index == ISI_CORRUPT_INDEX) fprintf(stderr, "Disk loop appears to be corrupt.\n");
            exit(1);
        }
    } else {
        end.index = snapshot.sys->index.yngest;
    }

    if (beg.index <= end.index) {
        ProcessRange(&snapshot, beg.index, end.index, raw);
    } else {
        ProcessRange(&snapshot, beg.index, snapshot.sys->index.lend, raw);
        ProcessRange(&snapshot, 0, end.index, raw);
    }

    exit (0);
}

/* Revision History
 *
 * $Log: main.c,v $
 * Revision 1.8  2009/07/09 18:42:26  dechavez
 * Added QDPLUS support, changed -v option to just print headers and added
 * -V option to print headers and dump to stdout.
 *
 * Revision 1.7  2009/02/23 22:05:20  dechavez
 * Fixed bug specifying start/end range
 *
 * Revision 1.6  2009/01/26 21:20:47  dechavez
 * renamed all ISI_DL instances used in isidlSnapshot(), for improved clarity
 *
 * Revision 1.5  2008/01/16 23:56:50  dechavez
 * Relink with current libraries after adding new xxx_timstr buffer args
 *
 * Revision 1.4  2007/01/11 22:02:23  dechavez
 * switch to isidb and/or isidl prefix on functions in isidb and isidl libraries
 *
 * Revision 1.3  2006/06/23 18:21:07  dechavez
 * It's been so long, I don't remember.
 *
 * Revision 1.2  2006/02/10 01:02:43  dechavez
 * preliminary work on beg/end/count options.
 * link with libida 4.0.0, libisidb 1.0.0 and neighbors
 *
 * Revision 1.1  2005/10/18 18:59:30  dechavez
 * initial release
 *
 */
