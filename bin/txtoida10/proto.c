#pragma ident "$Id: proto.c,v 1.3 2015/12/08 22:38:23 dechavez Exp $"
/*======================================================================
 *
 *  proto-packet functions
 *
 *====================================================================*/
#define INCLUDE_IACP_DEFAULT_ATTR
#include "txtoida10.h"

#define MAXSAMP (IDA10_DEFDATALEN / sizeof(INT32))
#define MAXBYTE (MAXSAMP * sizeof(INT32))

#define NsecToSec(nsec) ((REAL64) nsec / NANOSEC_PER_SEC)

static ISI_PUSH *ph = NULL;

static BOOL ParseServerPortDepth(char **server, int *port, int *depth, char *arg)
{
#define MAX_TOKEN 3
#define MIN_TOKEN 2
char *token[MAX_TOKEN];
#define DELIMITERS ":"
int ntoken;

    if ((ntoken = utilParse(arg, token, DELIMITERS, MAX_TOKEN, 0)) < MIN_TOKEN) {
        fprintf(stderr, "bad server:port[:depth] string '%s'\n", arg);
        return FALSE;
    }

    if ((*server = strdup(token[0])) == NULL) {
        perror("strdup");
        return FALSE;
    }

    if ((*port = atoi(token[1])) <= 0) {
        fprintf(stderr, "illegal port number from '%s'\n", arg);
        return FALSE;
    }

    if (ntoken == MAX_TOKEN) {
        if ((*depth = atoi(token[MAX_TOKEN-1])) <= 0) {
            fprintf(stderr, "illegal depth from '%s'\n", arg);
            return FALSE;
        }
    }

    return TRUE;
}

BOOL SetDlOutput(char *string, LOGIO *lp)
{
BOOL block = FALSE;
char *server;
int port = 0;
int depth = TXTOIDA10_DEFAULT_DEPTH;
IACP_ATTR attr = IACP_DEFAULT_ATTR;

    if (!ParseServerPortDepth(&server, &port, &depth, string)) return FALSE;

    if ((ph = isiPushInit(server, port, &attr, lp, LOG_INFO, IDA10_FIXEDRECLEN, depth, block)) == NULL) {
        perror("isiPushInit");
        return FALSE;
    }

    return TRUE;
}

static void ResetPacket(PROTO_PACKET *pkt)
{
    pkt->nsamp = 0;
    utilPackINT16(&pkt->buf[pkt->offset.nsamp], pkt->nsamp);
    memset(&pkt->buf[pkt->offset.data], 0xee, MAXBYTE);
}

BOOL ConsistentPacket(PROTO_PACKET *pkt, LINE_SAMPLE *sample)
{
INT64 timetear;

    if (pkt->sint != sample->sint) {
        LogMsg(LOG_INFO, "WARNING: %s sample interval change from ", pkt->ident);
        LogMsg(LOG_INFO, "%.3lf to %.3lf\n", NsecToSec(pkt->sint), NsecToSec(sample->sint));
        return FALSE;
    }

    if ((timetear = (pkt->nsamp > 0) ? sample->tstamp.gentag.tstamp - pkt->tons : 0) != 0) {
        LogMsg(LOG_INFO, "WARNING: %s time tear of %.3lf sec\n", pkt->ident, NsecToSec(timetear));
        return FALSE;
    }

    return TRUE;
}

void InitProtoPacket(PROTO_PACKET *new, LINE_SAMPLE *sample)
{
INT16 srfact, srmult;
UINT8 *ptr, descriptor;

    strcpy(new->ident, sample->ident);
    new->sint = sample->sint;
    new->tons = sample->tstamp.gentag.tstamp;

    memset(new->buf, 0, IDA10_FIXEDRECLEN);
    ptr = new->buf;

    /* 10.8 common header */

    ptr += utilPackBytes(ptr, (UINT8 *) "TS", 2);
    *ptr++ = 10; /* format 10 */
    *ptr++ = 8;  /* subformat 8 */
    ptr += utilPackBytes(ptr, (UINT8 *) sample->sname, IDA105_SNAME_LEN);
    ptr += utilPackBytes(ptr, (UINT8 *) sample->nname, IDA105_NNAME_LEN);
    new->offset.tofs = (int) (ptr - new->buf);
    ptr += ida10PackGenericTtag(ptr, &sample->tstamp.gentag);
    ptr += utilPackUINT32(ptr, 0); /* seqno will get updated when written to disk */
    ptr += utilPackUINT32(ptr, 0); /* timestamp will get updated when written to disk */
    ptr += IDA108_RESERVED_BYTES;
    ptr += utilPackUINT16(ptr, IDA10_FIXED_NBYTES);

    /* end of common header */

    /* use chnloc for stream name */

    ptr += utilPackBytes(ptr, (UINT8 *) sample->chnloc, IDA10_CNAMLEN);

    /* data format/status descriptor */

    descriptor = 0;
    descriptor |= IDA10_COMP_NONE;
    descriptor |= IDA10_A2D_24;
    descriptor |= IDA10_MASK_INT32;
    *ptr++ = descriptor;

    /* conversion gain */

    *ptr++ = sample->gain;

    /* number of samples will get updated with total when flushed */

    new->offset.nsamp = (int) (ptr - new->buf);
    ptr += utilPackINT16(ptr, new->nsamp = 0);

    /* sample rate */

    ida10SintToFactMult(((REAL64) sample->sint / NANOSEC_PER_SEC), &srfact, &srmult);
    ptr += utilPackINT16(ptr, srfact);
    ptr += utilPackINT16(ptr, srmult);

    /* the first sample */

    new->offset.data = (int) (ptr - new->buf);
    
    ResetPacket(new);
}

void FlushPacket(PROTO_PACKET *pkt)
{
    if (pkt->nsamp > 0) {
        utilPackINT16(&pkt->buf[pkt->offset.nsamp], pkt->nsamp);
        if (ph == NULL) {
            fwrite(pkt->buf, 1, IDA10_FIXEDRECLEN, stdout);
        } else {
            isiPushRawPacket(ph, pkt->buf, IDA10_FIXEDRECLEN, ISI_TYPE_IDA10);
        }
    }
    ResetPacket(pkt);
}

void AppendSample(LNKLST *head, LINE_SAMPLE *sample)
{
LNKLST_NODE *node;
PROTO_PACKET *pkt;

    node = GetProtoPacket(head, sample);
    pkt = (PROTO_PACKET *) node->payload;

    if (pkt->nsamp == 0) ida10PackGenericTtag(&pkt->buf[pkt->offset.tofs], &sample->tstamp.gentag);
    utilPackINT32(&pkt->buf[pkt->offset.data + (pkt->nsamp * sizeof(INT32))], sample->value);
    pkt->tons = sample->tstamp.gentag.tstamp + sample->sint;
    if (++pkt->nsamp == MAXSAMP) FlushPacket(pkt);
}

/* Revision History
 *
 * $Log: proto.c,v $
 * Revision 1.3  2015/12/08 22:38:23  dechavez
 * various minor tweaks to get clean Mac OS X builds
 *
 * Revision 1.2  2011/10/12 17:55:22  dechavez
 * build IDA10.8 instead of IDA10.5 packets
 * warnings and error messages via logio instead of stderr
 * added support for for writing directly to a remote disk loop via ISI_PUSH
 *
 * Revision 1.1  2011/10/04 19:49:28  dechavez
 * initial release
 *
 */
