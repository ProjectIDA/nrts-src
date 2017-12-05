#pragma ident "$Id: list.c,v 1.2 2011/10/12 17:57:13 dechavez Exp $"
/*======================================================================
 *
 *  Various list related convenience functions
 *
 *====================================================================*/
#include "txtoida10.h"

static LNKLST_NODE *LocateProtoPacket(LNKLST *head, LINE_SAMPLE *sample)
{
LNKLST_NODE *crnt;
PROTO_PACKET*pkt;

    crnt = listFirstNode(head);
    while (crnt != NULL) {
        pkt = (PROTO_PACKET *) crnt->payload;
        if (strcmp(sample->ident, pkt->ident) == 0) return crnt;
        crnt = listNextNode(crnt);
    }

    errno = ENOENT;
    return NULL;
}
static LNKLST_NODE *CreateProtoPacket(LNKLST *head, LINE_SAMPLE *sample)
{
PROTO_PACKET *new;

    if ((new = (PROTO_PACKET *) malloc(sizeof(PROTO_PACKET))) == NULL) {
        LogMsg(LOG_INFO, "CreateProtoPacket: malloc: %s", strerror(errno));
        exit(1);
    }

    InitProtoPacket(new, sample);

    return listAppend(head, new, sizeof(PROTO_PACKET)) ? LocateProtoPacket(head, sample) : NULL;
}

LNKLST_NODE *GetProtoPacket(LNKLST *head, LINE_SAMPLE *sample)
{
LNKLST_NODE *node;
PROTO_PACKET*pkt;
static char *fid = "GetProtoPacket";


/* If we have a packet in progress, ensure time and sample rate are OK */

    if ((node = LocateProtoPacket(head, sample)) != NULL) {
        pkt = (PROTO_PACKET *) node->payload;
        if (!ConsistentPacket(pkt, sample)) {
            FlushPacket(pkt);
            node = NULL;
        }
    }

    if (node == NULL && (node = CreateProtoPacket(head, sample)) == NULL) {
        LogMsg(LOG_INFO, "ERROR: %s: %s\n", fid, strerror(errno));
        exit(1);
    }
    return node;
}

/* Revision History
 *
 * $Log: list.c,v $
 * Revision 1.2  2011/10/12 17:57:13  dechavez
 * warnings and errors via logio instead of stderr
 *
 * Revision 1.1  2011/10/04 19:49:28  dechavez
 * initial release
 *
 */
