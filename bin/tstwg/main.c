#pragma ident "$Id: main.c,v 1.1 2012/08/06 21:22:54 dechavez Exp $"
#include "wg.h"
#include "iacp.h"

int main()
{
static char *server = "mcuc.wg.ucsd.edu";
static int port = WG_DEFAULT_PORT;
static char *site = "site";
IACP *iacp;
IACP_FRAME frame;
UINT8 buf1[1024], buf2[1024];
WG_OBSPKT_SEQNO seqno;
WG_WGOBS1 wgobs1;

    if ((iacp = iacpOpen(server, port, NULL, NULL, 0)) == NULL) {
        perror("iacpOpen");
        exit(1);
    }

    frame.payload.type = WG_IACP_WGID;
    frame.payload.len  = strlen(site);
    frame.payload.data = site;

    if (!iacpSendFrame(iacp, &frame)) {
        perror("iacpSendFrame");
        exit(1);
    }
    printf("sent WG_IACP_WGID '%s'\n", site);

    if (!iacpRecvFrame(iacp, &frame, buf1, 1024)) {
        perror("iacpRecvFrame");
        exit(1);
    }

    if (frame.payload.type != WG_IACP_SEQNO) {
        printf("expected type %d (WG_IACP_SEQNO) but received %d\n", WG_IACP_SEQNO, frame.payload.type);
        exit(1);
    }

    wgUnpackSeqno(frame.payload.data, &seqno);
    printf("recv seqno.valid = %d", seqno.valid);
    if (seqno.valid) printf(", seqno.value = %hu\n", seqno.value);
    printf("\n");

    wgobs1.seqno = 12345;
    wgobs1.len   = 512;
    wgobs1.payload = buf2;
    memset(wgobs1.payload, 0xee, wgobs1.len);

    frame.payload.type = WG_IACP_OBSPKT;
    frame.payload.data = buf1;
    frame.payload.len = wgPackWGOBS1(frame.payload.data, &wgobs1);

    if (!iacpSendFrame(iacp, &frame)) {
        perror("iacpSendFrame");
        exit(1);
    }
    printf("sent WG_IACP_OBSKPT, seqno = %d, len=%hd\n", wgobs1.seqno, wgobs1.len);

    iacpSendNull(iacp);
    printf("sent IACP_TYPE_NULL\n");

    while (iacpRecvFrame(iacp, &frame, buf1, 1024)) {
        if (frame.payload.type == IACP_TYPE_NULL) {
            printf("recv IACP_TYPE_NULL\n");
            iacpClose(iacp);
            exit(0);
        }
        printf("recv type = %d, len=%d\n", frame.payload.type, frame.payload.len);
    }

    perror("iacpRecvFrame");
    exit(1);
}
