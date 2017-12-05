#pragma ident "$Id: main.c,v 1.32 2017/01/20 17:18:30 dechavez Exp $"
/*======================================================================
 *
 *  List QDPLUS packets
 *
 *====================================================================*/
#include "qpp.h"

#define PRINT_NOTHING    0x0000
#define PRINT_CMNHDR     0x0001
#define PRINT_ACTION     0x0002
#define PRINT_TOKENS     0x0004
#define PRINT_COMBO      0x0008
#define PRINT_STATUS     0x0010
#define PRINT_USER       0x0020
#define PRINT_IGNORED    0x0040
#define PRINT_LCQ_EVENTS 0x0080
#define PRINT_LCQ        0x0100
#define PRINT_LCQLC     (0x0200 | PRINT_LCQ)
#define PRINT_LCQDATA   (0x0400 | PRINT_LCQLC)
#define PRINT_BLOCKETTES 0x0800 
#define PRINT_CALONOFF   0x1000 
#define PRINT_HEX        0x2000
#define PRINT_CFGBLK     0x4000
#define PRINT_EPD        0x8000
#define PRINT_EVERYTHING 0xffff
#define DEFAULT_PRINT    (PRINT_CMNHDR | PRINT_LCQ)

BOOL DebugSteim2A;
BOOL HaveHLP = FALSE;
QDPLUS *qdplus = NULL;

typedef struct {
    char *name;
    UINT16 mask;
    char *description;
} PRINT_MAP;

static PRINT_MAP map[] =
{
    {    "all", PRINT_EVERYTHING, "print everything"},
    {    "hdr", PRINT_CMNHDR,     "print QDP common header"},
    {    "act", PRINT_ACTION,     "print LCQ action flag"},
    {    "cal", PRINT_CALONOFF,   "print calibration on/off transitions"},
    {    "tok", PRINT_TOKENS,     "print tokens"},
    {    "cmb", PRINT_COMBO,      "print combo (FIX, GLOB, LOG) packets"},
    {    "epd", PRINT_EPD,        "print environmental processor delays"},
    {   "stat", PRINT_STATUS,     "print status packets"},
    {   "user", PRINT_USER,       "print user packets"},
    {    "ign", PRINT_IGNORED,    "print ignored packets"},
    {    "evt", PRINT_LCQ_EVENTS, "print LCQ events"},
    {    "lcq", PRINT_LCQ,        "print logical channel header"},
    {    "chn", PRINT_LCQLC,      "print logical channel headers"},
    {    "dat", PRINT_LCQDATA,    "print data samples"},
    {    "blk", PRINT_BLOCKETTES, "print DT_DATA blockettes"},
    {    "ccd", PRINT_CFGBLK,     "print config change data"},
    {    "hex", PRINT_HEX,        "hex dump of each QDP packet"},
    {NULL, 0}
};

static UINT16 print = PRINT_NOTHING;
static UINT16 printlcq = 0;

static void PrintHelp()
{
int i;

    fprintf(stderr, "The 'print' argument is a comma delimited list of any of the following\n");
    for (i = 0; map[i].name != NULL; i++) fprintf(stderr, "%4s - %s\n", map[i].name, map[i].description);
    exit(1);
}

static void DecodePrintArg(char *string)
{
BOOL done;
int i, n, ntoken;
#define MAXTOKEN 32
char *token[MAXTOKEN];
char *arg;

    print = PRINT_NOTHING;

    if ((arg = strdup(string)) == NULL) {
        perror("strdup");
        exit(1);
    }

    if ((ntoken = utilParse(arg, token, ",", MAXTOKEN, 0)) < 0) {
        fprintf(stderr, "error parsing '%s'\n", string);
        exit(1);
    }

    for (n = 0; n < ntoken; n++) {
        if (strcasecmp(token[n], "help") == 0) PrintHelp();
        for (done = FALSE, i = 0; !done && map[i].name != NULL; i++) {
            if (strcasecmp(token[n], map[i].name) == 0) {
                print |= map[i].mask;
                done = TRUE;
            }
        }
        if (!done) {
            fprintf(stderr, "unrecgonized print flag '%s'\n", token[n]);
            PrintHelp();
        }
    }

    if ((print & PRINT_LCQDATA) == PRINT_LCQDATA) {
        printlcq = QDP_PRINT_LCQDATA;
    } else if ((print & PRINT_LCQLC) == PRINT_LCQLC) {
        printlcq = QDP_PRINT_LCQLC;
    } else {
        printlcq = 0;
    }
}

static void PrintUser(UINT8 *payload)
{
QDPLUS_PAROSCI *paro;
QDPLUS_USERPKT userpkt;
UINT32 tstamp;
char tbuf[32];

    qdplusDecodeUser(payload, &userpkt);
    switch (userpkt.type) {
      case QDPLUS_DT_USER_PAROSCI:
        paro = &userpkt.data.parosci;
        tstamp = (paro->tofs.tstamp / NANOSEC_PER_SEC) + SAN_EPOCH_TO_1970_EPOCH;
        printf("parosci:");
        printf(" %s:%s", paro->chn, paro->loc);
        printf(" %s", utilLttostr(tstamp, 0, tbuf));
        printf(" %7d", paro->first);
        printf(" %3d", paro->ndiff);
        printf("\n");
        break;
      default:
        printf("user packet type '%d' is not supported\n", userpkt.type);
    }
}

static void PrintIgnored(QDP_PKT *qdp)
{
int i;
UINT64 sn, auth = 0;
QDP_TYPE_C1_SRVCH c1_srvch;
QDP_TYPE_C1_SRVRSP c1_srvrsp;
QDP_TYPE_C1_RQMEM c1_rqmem;
char tmpbuf[32];
UINT32 seqno;

    switch (qdp->hdr.cmd) {
      case QDP_C1_RQSRV:
        utilUnpackUINT64(qdp->payload, &sn);
        printf("sn = %016llX\n", sn);
        break;

      case QDP_C1_SRVCH:
        qdpDecode_C1_SRVCH(qdp->payload, &c1_srvch);
        printf("challenge = %016llX\n", c1_srvch.challenge);
        printf("       ip = %08X = %s\n", c1_srvch.dp.ip, c1_srvch.dp.dotdecimal);
        printf("     port = %d\n", c1_srvch.dp.port);
        printf("      reg = %d\n", c1_srvch.dp.registration);
        break;

      case QDP_C1_SRVRSP:
        qdpDecode_C1_SRVRSP(qdp->payload, &c1_srvrsp);
        printf(" serialno = %016llX\n", c1_srvrsp.serialno);
        printf("challenge = %016llX\n", c1_srvrsp.challenge);
        printf("       ip = %08X = %s\n", c1_srvrsp.dp.ip, c1_srvrsp.dp.dotdecimal);
        printf("     port = %d\n", c1_srvrsp.dp.port);
        printf("      reg = %d\n", c1_srvrsp.dp.registration);
        printf("   random = %016llX\n", c1_srvrsp.random);
        printf("      md5 = %008X%008X%008X%008X\n", c1_srvrsp.md5[0], c1_srvrsp.md5[1], c1_srvrsp.md5[2], c1_srvrsp.md5[3]);
        qdpMD5(&c1_srvrsp, auth);
        printf("my digest = %008X%008X%008X%008X\n", c1_srvrsp.md5[0], c1_srvrsp.md5[1], c1_srvrsp.md5[2], c1_srvrsp.md5[3]);
        break;

      case QDP_C1_RQMEM:
        qdpDecode_C1_RQMEM(qdp->payload, &c1_rqmem);
        printf("offset = %u\n", c1_rqmem.offset);
        printf(" nbyte = %hu\n", c1_rqmem.nbyte);
        printf("  type = %s (0x%04X)\n", qdpMemTypeString(c1_rqmem.type), c1_rqmem.type);
        printf("passwd = 0x%08X%08X%08X%08x\n", c1_rqmem.passwd[0], c1_rqmem.passwd[1], c1_rqmem.passwd[2], c1_rqmem.passwd[3]);
        break;

      default:
        utilPrintHexDump(stdout, qdp->payload, qdp->hdr.dlen);
    }
}

void Ida10Callback(void *arg, QDP_HLP *hlp)
{
FILE *fp;
static UINT32 seqno = 0;
UINT8 buf[IDA10_FIXEDRECLEN];

    fp = (FILE *) arg;

    if (!qdpHlpToIDA10(buf, hlp, ++seqno)) {
        fprintf(stderr, "qdpHlpToIDA10: %s\n", strerror(errno));
        exit(1);
    }

    if (fwrite(buf, sizeof(char), IDA10_FIXEDRECLEN, fp) != IDA10_FIXEDRECLEN) {
        fprintf(stderr, "fwrite: %s\n", strerror(errno));
        exit(1);
    }

}

BOOL InitIDA10(QDP_HLP_RULES *rules, char *file, BOOL nosechk, BOOL strict)
{
UINT8 format = QDP_HLP_FORMAT_NOCOMP32;
UINT32 flags = QDP_HLP_RULE_FLAG_LCASE;
static FILE *fp;

    if ((fp = fopen(file, "wb")) == NULL) {
        fprintf(stderr, "fopen: %s\n", strerror(errno));
        exit(1);
    }

    if (nosechk) flags |= QDP_HLP_RULE_FLAG_NOHLP_1SECHK;
    if (strict)  flags |= QDP_HLP_RULE_FLAG_STRICT;

    return qdpInitHLPRules(
        rules,
        IDA10_DEFDATALEN,
        format,
        Ida10Callback,
        (void *) fp,
        flags
    );
}

BOOL InitSplit(char *dir)
{
    if (utilDirectoryExists(dir)) {
        fprintf(stderr, "*** ERROR *** split directory '%s' already exists\n", dir);
        return FALSE;
    }

    if (util_mkpath(dir, 0755) != 0) {
        fprintf(stderr, "*** ERROR *** unable to mkdir %s: %s\n", dir, strerror(errno));
        return FALSE;
    }

    return TRUE;
}

BOOL SavePacket(char *dir, QDPLUS_PKT *pkt, UINT32 count)
{
FILE *fp;
char path[MAXPATHLEN+1];

    sprintf(path, "%s/%04u", dir, count);
    if ((fp = fopen(path, "w")) == NULL) {
        perror(path);
        return FALSE;
    }
    if (!qdplusWrite(fp, pkt)) {
        perror("qdplusWrite");
        return FALSE;
    }

    fclose(fp);

    return TRUE;
}

static void help(char *myname)
{
    fprintf(stderr, "usage: %s [ print=list { ida10=file [ -nosechk -strict ] | tee=dir [ trig=cfgstr | trig=off ] } keep=serialno meta=dir count=val skip=val ] < QDPLUS_data_stream\n", myname);
    fprintf(stderr, "\n");
    PrintHelp();
}

static void FlushHLP()
{
QDPLUS_DIGITIZER *digitizer;
LNKLST_NODE *crnt;

    crnt = listFirstNode(qdplus->list.digitizer);
    while (crnt != NULL) {
        digitizer = (QDPLUS_DIGITIZER *) crnt->payload;
        qdpFlushLCQ(&digitizer->lcq);
        crnt = listNextNode(crnt);
    }
}

static void Exit(UINT32 in, UINT32 kept, int status)
{
    printf("%u packets read\n", in);
    printf("%u packets processed\n", kept);
    if (HaveHLP) FlushHLP();
    exit(status);
}

static void process(QDPLUS_PKT *pkt, UINT32 in, char *split)
{
QDP_LCQ *lcq;
REAL64 tstamp;
static BOOL first = TRUE;

    if (split != NULL) SavePacket(split, pkt, in);

    if ((lcq = qdplusProcessPacket(qdplus, pkt)) == NULL) {
        printf("qdplusProcessPacket failed\n");
        exit(1);
    }

    if (print & PRINT_HEX) {
        printf("%016llX %08x%016llx, raw len=%d, payload len=%d\n", pkt->serialno, pkt->seqno.signature, pkt->seqno.counter, pkt->qdp.len, pkt->qdp.hdr.dlen);
        utilPrintHexDump(stdout, pkt->qdp.raw, pkt->qdp.len);
    }
    if (print & PRINT_CMNHDR) qdplusPrintPkt(stdout, pkt, QDP_PRINT_HDR);

    if ((print & PRINT_LCQ_EVENTS) && lcq->event != 0) {
        printf("LCQ events: "); utilPrintBinUINT16(stdout, lcq->event);
        if (lcq->event & QDP_LCQ_EVENT_NO_META) printf(" NO_META");
        if (lcq->event & QDP_LCQ_EVENT_UNSUPPORTED) printf(" UNSUPPORTED");
        if (lcq->event & QDP_LCQ_EVENT_UNDOCUMENTED) printf(" UNDOCUMENTED");
        if (lcq->event & QDP_LCQ_EVENT_NO_TOKENS) printf(" NO_TOKENS");
        if (lcq->event & QDP_LCQ_EVENT_DECOMP_ERR) printf(" DECOMP_ERR");
        if (lcq->event & QDP_LCQ_EVENT_SINT_CHANGE) printf(" SINT_CHANGE");
        if (lcq->event & QDP_LCQ_EVENT_CALBEG) printf(" CALBEG");
        if (lcq->event & QDP_LCQ_EVENT_CALEND) printf(" CALEND");
        if (lcq->event & QDP_LCQ_EVENT_CNPBLK) printf(" CNPBLK");
        if (lcq->event & QDP_LCQ_EVENT_CFGBLK) printf(" CFGBLK");
        if (lcq->event & QDP_LCQ_EVENT_ENVDAT) printf(" ENVDAT");
        if (lcq->event & QDP_LCQ_EVENT_FATAL) {
            printf(" FATAL\n");
            exit(1);
        }
        printf("\n");
    }

    CheckCalibrationStatus(pkt->serialno, lcq);

    switch (lcq->action) {

      case QDP_LCQ_SAVE_MEMBLK:
        if (print & PRINT_ACTION) printf("LCQ action = QDP_LCQ_SAVE_MEMBLK\n");
        break;

      case QDP_LCQ_LOAD_TOKEN:
        if (print & PRINT_ACTION) printf("LCQ action = QDP_LCQ_LOAD_TOKEN\n");
        if (print & PRINT_TOKENS) qdpPrintTokens(stdout, &lcq->meta.token);
        break;

      case QDP_LCQ_LOAD_COMBO:
        if (print & PRINT_ACTION) printf("LCQ action = QDP_LCQ_LOAD_COMBO\n");
        if (print & PRINT_COMBO) qdpPrint_C1_COMBO(stdout, &lcq->meta.combo); break;
        break;

      case QDP_LCQ_LOAD_EPD:
        if (print & PRINT_ACTION) printf("LCQ action = QDP_LCQ_LOAD_EPD\n");
        if (print & PRINT_EPD) qdpPrint_C2_EPD(stdout, &lcq->meta.epd); break;
        break;

      case QDP_LCQ_LOAD_DT_DATA:
           if (print & PRINT_ACTION) printf("LCQ action = QDP_LCQ_LOAD_DT_DATA\n");
           if (print & PRINT_LCQ) qdpPrintLCQ(stdout, lcq, printlcq);
           if (print & PRINT_BLOCKETTES) {
                qdpPrintBlockettes(stdout, &lcq->dt_data);
            } else if ((lcq->event & QDP_LCQ_EVENT_CFGBLK) && (print & PRINT_CFGBLK)) {
                qdpPrintBlockettes(stdout, &lcq->dt_data);
            }
        break;

      case QDP_LCQ_LOAD_C1_STAT:
        if (print & PRINT_ACTION) printf("LCQ action = QDP_LCQ_LOAD_C1_STAT\n"); fflush(stdout);
        if (print & PRINT_STATUS) qdpPrint_C1_STAT(stdout, &lcq->c1_stat); fflush(stdout);
        break;

      case QDP_LCQ_IS_DT_USER:
        if (print & PRINT_ACTION) printf("LCQ action = QDP_LCQ_IS_DT_USER\n");
        if (print & PRINT_USER) PrintUser(pkt->qdp.payload);
        break;

      case QDP_LCQ_IGNORE:
        if (print & PRINT_ACTION) printf("LCQ action = QDP_LCQ_IGNORE\n");
        if (print & PRINT_IGNORED) PrintIgnored(lcq->pkt);
        break;

      default:
        if (print & PRINT_ACTION) printf("ERROR: Unknown LCQ action '%d'\n", lcq->action);
        exit(1);
    }
}

int main (int argc, char **argv)
{
int i;
gzFile *gzin;
UINT32 skip = 0, in = 0, kept = 0, count = 0xffffffff;;
QDPLUS_PKT pkt;
UINT64 keep = 0;
QDPLUS_PAR par = QDPLUS_DEFAULT_PAR;
char *ida10 = NULL;
char *tee = NULL;
char *split = NULL;
char *detect = NULL;
char defdetect[] = DEFAULT_DETECTOR;
char *log = DEFAULT_LOG;
BOOL debug = FALSE;
BOOL nosechk = FALSE;
BOOL strict = FALSE;

    for (i = 1; i < argc; i++) {
        if (strcasecmp(argv[i], "-help") == 0) {
            help(argv[0]);
        } else if (strcmp(argv[i], "-v") == 0) {
            print |= DEFAULT_PRINT;
        } else if (strcmp(argv[i], "-nosechk") == 0) {
            nosechk = TRUE;
        } else if (strcmp(argv[i], "-strict") == 0) {
            strict = TRUE;
        } else if (strncasecmp(argv[i], "print=", strlen("print=")) == 0) {
            DecodePrintArg(argv[i]+strlen("print="));
        } else if (strncmp(argv[i], "keep=", strlen("keep=")) == 0) {
            keep = (UINT64) strtoll((char *) argv[i]+strlen("keep="), NULL, 16);
        } else if (strncmp(argv[i], "count=", strlen("count=")) == 0) {
            count = (UINT32) atol(argv[i]+strlen("count="));
        } else if (strncmp(argv[i], "skip=", strlen("skip=")) == 0) {
            skip = (UINT32) atol(argv[i]+strlen("skip="));
        } else if (strncmp(argv[i], "meta=", strlen("meta=")) == 0) {
            par.path.meta = argv[i] + strlen("meta=");
        } else if (strncmp(argv[i], "ida10=", strlen("ida10=")) == 0) {
            ida10 = argv[i] + strlen("ida10=");
        } else if (strncmp(argv[i], "tee=", strlen("tee=")) == 0) {
            tee = argv[i] + strlen("tee=");
        } else if (strncmp(argv[i], "split=", strlen("split=")) == 0) {
            split = argv[i] + strlen("split=");
        } else if (strncmp(argv[i], "trig=", strlen("trig=")) == 0) {
            if ((detect = strdup(argv[i] + strlen("trig="))) == NULL) {
                perror("strdup");
                exit(1);
            }
        } else if (strncmp(argv[i], "log=", strlen("log=")) == 0) {
            log = argv[i] + strlen("log=");
        } else {
            fprintf(stderr, "%s: unrecognized argument '%s'\n", argv[0], argv[i]);
            help(argv[0]);
        }
    }

    if (detect == NULL) detect = defdetect;

    if (ida10 != NULL && tee != NULL) {
        fprintf(stderr, "%s: ida10 and tee options are mutually exclusive\n", argv[0]);
        exit(1);
    }

    if (ida10 != NULL) {
        if (!InitIDA10(&par.lcq.rules, ida10, nosechk, strict)) {
            fprintf(stderr, "%s: unable to initialize IDA10 packet converter\n", argv[0]);
            exit(1);
        }
        HaveHLP = TRUE;
    } else if (nosechk) {
        fprintf(stderr, "%s: -nosechk option only valid in conjuction with ida10 option\n", argv[0]);
        exit(1);
    }

    if (tee != NULL) {
        if (!InitTee(&par.lcq.rules, tee, detect)) {
            fprintf(stderr, "%s: unable to initialize tee file packet converter\n", argv[0]);
            exit(1);
        }
        HaveHLP = TRUE;
    }

    if (split != NULL) {
        if (!InitSplit(split)) {
            fprintf(stderr, "%s: unable to initialize split directory '%s'\n", argv[0], split);
            exit(1);
        }
    }

    if ((gzin = gzdopen(fileno(stdin), "r")) == NULL) {
        fprintf(stderr, "%s: ", argv[0]);
        perror("gzdopen");
        exit(1);
    }

    if ((par.lcq.lp = InitLogging(argv[0], log, debug)) == NULL) {
        perror("InitLogging");
        exit(1);
    }

    if ((qdplus = qdplusCreateHandle(&par)) == NULL) {
        fprintf(stderr, "%s: ", argv[0]);
        perror("qdplusCreateHandle");
        exit(1);
    }

    if (skip) {
        LogMsg("Skipping the first %lu records", skip);
        do {
            if (!qdplusReadgz(gzin, &pkt)) {
                if (feof(stdin)) {
                    LogMsg("EOF encountered");
                } else {
                    perror("*** ABORT *** qdplusReadgz");
                }
                Exit(in, kept, 1);
            }
        } while (++in < skip);
    }

    if (keep) LogMsg("Only tracking packets from instrument s/n %016llX\n", keep); 
    while (in < count && qdplusReadgz(gzin, &pkt)) {
        ++in;
        if (qdpVerifyCRC(&pkt.qdp)) {
            if (pkt.serialno == 0) {
                fprintf(stderr, "*** ERROR *** corrupt QDP stream (serialno = 0)\n");
                break;
            }
            if (keep == 0 || pkt.serialno == keep) {
                ++kept;
                process(&pkt, in, split);
            }
        } else {
            LogMsg("CRC error, packet no. %lu dropped\n", in); fflush(stdout);
        }
    }
    if (errno != 0) {
        perror("*** ABORT *** qdplusReadgz");
        Exit(in, kept, 1);
    }
    if (tee != NULL) TeeHLP(NULL); /* closes current .gz file */

    Exit(in, kept, 0);
}


/*-----------------------------------------------------------------------+
 |                                                                       |
 | Copyright (C) 2009 Regents of the University of California            |
 |                                                                       |
 | This software is provided 'as-is', without any express or implied     |
 | warranty.  In no event will the authors be held liable for any        |
 | damages arising from the use of this software.                        |
 |                                                                       |
 | Permission is granted to anyone to use this software for any purpose, |
 | including commercial applications, and to alter it and redistribute   |
 | it freely, subject to the following restrictions:                     |
 |                                                                       |
 | 1. The origin of this software must not be misrepresented; you must   |
 |    not claim that you wrote the original software. If you use this    |
 |    software in a product, an acknowledgment in the product            |
 |    documentation of the contribution by Project IDA, UCSD would be    |
 |    appreciated but is not required.                                   |
 | 2. Altered source versions must be plainly marked as such, and must   |
 |    not be misrepresented as being the original software.              |
 | 3. This notice may not be removed or altered from any source          |
 |    distribution.                                                      |
 |                                                                       |
 +-----------------------------------------------------------------------*/

/* Revision History
 *
 * $Log: main.c,v $
 * Revision 1.32  2017/01/20 17:18:30  dechavez
 * added -nosechk support, fixed bug setting strict option in the IDA10 HLP
 * rules, fixed bug with -split option when used in conjuction with the keep
 * option
 *
 * Revision 1.31  2016/01/23 00:24:52  dechavez
 * added "epd" print option (environmental processor delay packet) supprt
 *
 * Revision 1.30  2016/01/19 23:16:20  dechavez
 * changed CALSTART, CALABORT to CALBEG, CALEND
 *
 * Revision 1.29  2015/12/09 18:43:37  dechavez
 * various minor tweaks to get clean Mac OS X builds
 *
 * Revision 1.28  2014/01/27 18:23:24  dechavez
 * removed "strict" option for HLP unpacker (permit decoding channels w/o complete tokens)
 *
 * Revision 1.27  2011/04/07 22:52:58  dechavez
 * added print supprt for QDPLUS_PAROSCI packets
 *
 * Revision 1.26  2010/03/31 20:54:24  dechavez
 * QDP_HLP_RULE_FLAG_STRICT option hardcoded, flush partially completed packets on program end
 *
 * Revision 1.25  2009/10/29 17:47:33  dechavez
 * Added "ccd" (configuration change data) print option, include event bits in event report
 *
 * Revision 1.24  2009/07/09 22:33:47  dechavez
 * moved tee code to its own file (where it got triggering added)
 *
 * Revision 1.23  2009/07/02 17:36:57  dechavez
 * print packet counts, even when aborting
 *
 * Revision 1.22  2009/07/02 17:01:57  dechavez
 * transparent support of gzip compressed input
 *
 * Revision 1.21  2009/07/02 16:36:48  dechavez
 * Report early termination due to qdplusRead() errors
 *
 * Revision 1.20  2009/06/24 21:32:50  dechavez
 * added split option, quit on corrupt data (serial number == 0)
 *
 * Revision 1.19  2009/06/19 18:11:25  dechavez
 * 2.5.1
 *
 * Revision 1.18  2009/02/26 22:47:21  dechavez
 * added tee option
 *
 * Revision 1.17  2009/02/23 22:26:58  dechavez
 * replaced -v option with more elaborate print flags
 *
 * Revision 1.16  2008/10/09 20:42:43  dechavez
 * added ida10 option
 *
 * Revision 1.15  2008/03/05 23:15:05  dechavez
 * fixed printing typo
 *
 * Revision 1.14  2007/12/20 23:15:40  dechavez
 * added C1_STAT support
 *
 * Revision 1.13  2007/06/28 19:43:09  dechavez
 * improved help message
 *
 * Revision 1.12  2007/05/11 16:19:45  dechavez
 * Relink with libqdp with compiled root time tag instead of macro
 *
 * Revision 1.11  2007/01/04 18:07:26  dechavez
 * *** empty log message ***
 *
 * Revision 1.10  2006/12/22 02:43:55  dechavez
 * Changes to accomodate reworked structure fields
 *
 * Revision 1.9  2006/12/15 00:32:07  dechavez
 * use new QDPLUS_PAR, no meta.  Added time stamp to lc prints
 *
 * Revision 1.8  2006/12/13 22:01:06  dechavez
 * added metadata support
 *
 * Revision 1.7  2006/12/06 22:51:10  dechavez
 * Use QDPLUS logical channel queues, much print stuff moved off to library
 *
 * Revision 1.6  2006/11/13 19:02:03  dechavez
 * Added hex dumps of compressed DT_DATA blockettes
 *
 * Revision 1.5  2006/10/13 21:35:42  dechavez
 * prelinimary DT_DATA support
 *
 * Revision 1.4  2006/07/06 16:50:48  dechavez
 * fixed typo in argument search loop
 *
 * Revision 1.3  2006/07/06 16:49:26  dechavez
 * Added -v option (off by default) and changed output format to make it easier to grep
 *
 * Revision 1.2  2006/06/07 22:25:54  dechavez
 * decode DT_USER packets
 *
 * Revision 1.1  2006/06/02 20:54:09  dechavez
 * initial release
 *
 */
