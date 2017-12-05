#pragma ident "$Id: config.c,v 1.13 2017/09/20 19:18:35 dauerbach Exp $"
/*======================================================================
 * 
 * Full configuration stuff
 *
 *====================================================================*/
#include "qdp.h"

static void ExtractTokensetName(LNKLST *list, QDP_DP_TOKEN *token, int port)
{
int i, count;
char *line, *beg, *end;
char begtag[] = "<!-- TS1 --> plus slop";
char endtag[] = "<!-- /TS1 --> plus slop";
static char *BaseTagPrefix = "<h3>Configuration Base Tag: ";

    sprintf(begtag, "<!-- TS%d -->", port);
    sprintf(endtag, "<!-- /TS%d -->", port);

    for (i = 0; i < list->count; i++) {
        line = (char *) list->array[i];
        beg = strstr(line, begtag);
        end = strstr(line, endtag);
        if (beg != NULL && end != NULL && end > beg) {
            if ((count = end - beg - strlen(begtag)) > QDP_MAX_TOKENSET_NAME) count = QDP_MAX_TOKENSET_NAME;
            strncpy(token->setname, &beg[strlen(begtag)], count);
            token->setname[count] = 0;
            sprintf(line, "<h3>Token Set Name: %s%s</h3>", begtag, endtag);
            return;
        } else if (strncmp(line, BaseTagPrefix, strlen(BaseTagPrefix)) == 0) {
            sprintf(line, "%s<!-- CBT --><!-- /CBT --></h3>", BaseTagPrefix);
        }
    }
}

BOOL qdpInitConfig(QDP_TYPE_FULL_CONFIG *config)
{
int i;
#define QDP_STRUCT_SIG "QDP_STRUCT_INIT"

    if (config == NULL) {
        errno = EINVAL;
        return FALSE;
    }

    memset(config, 0, sizeof(QDP_TYPE_FULL_CONFIG));
    if (strcmp(config->signature, QDP_STRUCT_SIG) == 0) {
        listClear(&config->webpage);
    } else {
        for (i = 0; i < QDP_NLP; i++) qdpInitTokens(&config->dport[i].token);
        if (!listInit(&config->webpage)) return FALSE;
        sprintf(config->signature, "%s", QDP_STRUCT_SIG);
        config->signature[QDP_DP_TOKEN_SIG_LEN] = 0;
    }
    config->basetag = NULL;
    config->set = 0;
    return TRUE;
}

BOOL qdpRequestDportConfig(QDP *qdp, QDP_TYPE_DPORT *dport, UINT16 port)
{
UINT16 memtype;

    dport->set = 0;
    dport->valid = FALSE;

    switch (port) {
      case QDP_LOGICAL_PORT_1: memtype = QDP_MEM_TYPE_CONFIG_DP1; break;
      case QDP_LOGICAL_PORT_2: memtype = QDP_MEM_TYPE_CONFIG_DP2; break;
      case QDP_LOGICAL_PORT_3: memtype = QDP_MEM_TYPE_CONFIG_DP3; break;
      case QDP_LOGICAL_PORT_4: memtype = QDP_MEM_TYPE_CONFIG_DP4; break;
      default:
        errno = ENOENT;
        return FALSE;
    }

    if (!qdp_C1_RQLOG(qdp, &dport->log, port)) return FALSE;
    dport->set |= QDP_DPORT_DEFINED_LOG;

    if (!qdpRequestTokens(qdp, &dport->token, memtype)) return FALSE;
    dport->set |= QDP_DPORT_DEFINED_TOKEN;

    dport->valid = TRUE;

    return TRUE;
}

BOOL qdpRequestFullConfig(QDP *qdp, QDP_TYPE_FULL_CONFIG *config)
{
int i;

    if (qdp == NULL || config == NULL) {
        errno = EINVAL;
        return FALSE;
    }

    if (!qdpInitConfig(config)) return FALSE;

    if (!qdp_C1_RQFIX(qdp, &config->fix)) return FALSE;

    if (!qdp_C1_RQGLOB(qdp, &config->glob)) return FALSE;
    config->set |= QDP_CONFIG_DEFINED_GLOBAL;

    if (!qdp_C1_RQPHY(qdp, &config->phy)) return FALSE;
    config->set |= QDP_CONFIG_DEFINED_INTERFACES;

    if (!qdpRequestDportConfig(qdp, &config->dport[QDP_LOGICAL_PORT_1], QDP_LOGICAL_PORT_1)) return FALSE;
    if (!qdpRequestDportConfig(qdp, &config->dport[QDP_LOGICAL_PORT_2], QDP_LOGICAL_PORT_2)) return FALSE;
    if (!qdpRequestDportConfig(qdp, &config->dport[QDP_LOGICAL_PORT_3], QDP_LOGICAL_PORT_3)) return FALSE;
    if (!qdpRequestDportConfig(qdp, &config->dport[QDP_LOGICAL_PORT_4], QDP_LOGICAL_PORT_4)) return FALSE;

    if (!qdp_C1_RQSC(qdp,  config->sc)) return FALSE;
    config->set |= QDP_CONFIG_DEFINED_SENSCTRL;

    if (!qdp_C1_RQSPP(qdp, &config->spp)) return FALSE;
    config->set |= QDP_CONFIG_DEFINED_SLAVE;

    if (!qdp_C2_RQPHY(qdp, &config->phy2[QDP_PP_SERIAL_1], QDP_PP_SERIAL_1)) return FALSE;
    config->set |= QDP_CONFIG_DEFINED_ADVSER1;

    if (!qdp_C2_RQPHY(qdp, &config->phy2[QDP_PP_SERIAL_2], QDP_PP_SERIAL_2)) return FALSE;
    config->set |= QDP_CONFIG_DEFINED_ADVSER2;

    if (!qdp_C2_RQPHY(qdp, &config->phy2[QDP_PP_ETHERNET], QDP_PP_ETHERNET)) return FALSE;
    config->set |= QDP_CONFIG_DEFINED_ADVETHER;

    if (!qdp_C2_RQGPS(qdp, &config->gps)) return FALSE;
    config->set |= QDP_CONFIG_DEFINED_GPS | QDP_CONFIG_DEFINED_PLL;

    if (!qdp_C2_RQAMASS(qdp, &config->amass)) return FALSE;
    config->set |= QDP_CONFIG_DEFINED_AUTOMASS;

    if (!qdp_C3_RQANNC(qdp,  &config->annc)) return FALSE;
    config->set |= QDP_CONFIG_DEFINED_ANNOUNCE;

    if (!qdp_C2_RQEPCFG(qdp, &config->epcfg)) return FALSE;
    config->set |= QDP_CONFIG_DEFINED_EPCFG;

    if (!qdpRequestWebpage(qdp, &config->webpage)) return FALSE;
    config->set |= QDP_CONFIG_DEFINED_WEBPAGE;

    for (i = 0; i < QDP_NLP; i++) ExtractTokensetName(&config->webpage, &config->dport[i].token, i+1);

    return TRUE;
}

static BOOL DumpBoth(QDP_PKT *orig, QDP_PKT *copy)
{
    printf("%d bytes from orig\n", orig->hdr.dlen);
    utilPrintHexDump(stdout, orig->payload, orig->hdr.dlen);
    printf("%d bytes from copy\n", copy->hdr.dlen);
    utilPrintHexDump(stdout, copy->payload, copy->hdr.dlen);
    return FALSE;
}

static BOOL ComparePackets(QDP_PKT *orig, QDP_PKT *copy, char *desc)
{
    if (orig->hdr.dlen != copy->hdr.dlen) {
        printf("%s length mismatch:  orig = %d, copy = %d\n", desc, orig->hdr.dlen, copy->hdr.dlen);
        return DumpBoth(orig, copy);
    }

    if (memcmp(orig->payload, copy->payload, orig->hdr.dlen) != 0) {
        printf("%s content mismatch.\n", desc);
        return DumpBoth(orig, copy);
    }

    printf("%s OK\n", desc);
    return TRUE;
}

BOOL qdpConfigureDataPort(QDP *qdp, QDP_TYPE_DPORT *dport, UINT16 port, QDP_TYPE_C1_FIX *fix, UINT32 delay, int ntry)
{
QDP_PKT pkt;
QDP_MEMBLK blk;
UINT16 memtype;

    errno = 0;

    if (!dport->valid) {
        errno = ENOENT;
        return FALSE;
    }

    switch (port) {
      case QDP_LOGICAL_PORT_1: memtype = QDP_MEM_TYPE_CONFIG_DP1; break;
      case QDP_LOGICAL_PORT_2: memtype = QDP_MEM_TYPE_CONFIG_DP2; break;
      case QDP_LOGICAL_PORT_3: memtype = QDP_MEM_TYPE_CONFIG_DP3; break;
      case QDP_LOGICAL_PORT_4: memtype = QDP_MEM_TYPE_CONFIG_DP4; break;
      default:
        errno = ENOENT;
        return FALSE;
    }
     
    qdpUpdateDataPortChannelFreqs(dport, fix); /* ensure tokens and data port output freqs are consistent */

    qdpEncode_C1_LOG(&pkt, &dport->log, port);
    if (!qdpPostCmd(qdp, &pkt, TRUE)) return FALSE;
     
    qdpPackTokenMemblk(&blk, &dport->token, memtype);
    if (!qdpPostMemBlk(qdp, &blk, delay, ntry, NULL)) return FALSE;
     
    return TRUE;
}

static BOOL ConfigDataPort(QDP *qdp, QDP_TYPE_DPORT *dport, UINT16 port, QDP_TYPE_C1_FIX *fix, UINT32 delay, int ntry, BOOL verbose)
{
    if (!dport->valid) {
        if (verbose) printf("SKIPPING data port %s I/O parameters and tokens\n", qdpDataPortString(port));
        return TRUE;
    }

    if (verbose) {printf("Loading data port %s I/O parameters and tokens", qdpDataPortString(port)); fflush(stdout);}
    if (!qdpConfigureDataPort(qdp, dport, port, fix, delay, ntry)) {
        if (verbose) printf(" *** FAILED ***\n");
        return FALSE;
    }
    if (verbose) printf("\n");

    return TRUE;
}

BOOL qdpSetConfig(QDP *qdp, QDP_TYPE_FULL_CONFIG *config, BOOL verbose)
{
int i;
QDP_PKT pkt;
QDP_MEMBLK blk;
struct {
    QDP_TYPE_C1_PHY c1_phy;
    QDP_TYPE_C2_GPS c2_gps;
} crnt;
char tmpbuf[32];
static UINT32 delay = 250;
static int ntry = 10;

/* C1_GLOB */

    if (config->set & QDP_CONFIG_DEFINED_GLOBAL) {
        qdpEncode_C1_GLOB(&pkt, &config->glob);
        if (verbose) {printf("Loading global programming"); fflush(stdout);}
        if (!qdpPostCmd(qdp, &pkt, TRUE)) {
            if (verbose) printf(" *** FAILED ***\n");
            return FALSE;
        }
        if (verbose) printf("\n");
    }

/* C1_PHY */

    if (config->set & QDP_CONFIG_DEFINED_INTERFACES) {
        if (!qdp_C1_RQPHY(qdp, &crnt.c1_phy)) {
            if (verbose) printf("Unable to determine current physical interface configuration!\n");
            return FALSE;
        }
        config->phy.serialno = crnt.c1_phy.serialno;
        for (i = 0; i < 6; i++) config->phy.ethernet.mac[i] = crnt.c1_phy.ethernet.mac[i];
        for (i = 0; i < 3; i++) if (config->phy.serial[i].baud == 0) config->phy.serial[i].baud = crnt.c1_phy.serial[i].baud;
        if (config->phy.ethernet.ip == 0) config->phy.ethernet.ip = crnt.c1_phy.ethernet.ip;
        qdpEncode_C1_PHY(&pkt, &config->phy);
        if (verbose) {printf("Loading physical interfaces (ethernet IP = %s)", utilDotDecimalString(config->phy.ethernet.ip, tmpbuf)); fflush(stdout);}
        if (!qdpPostCmd(qdp, &pkt, TRUE)) {
            if (verbose) printf(" *** FAILED ***\n");
            return FALSE;
        }
        if (verbose) printf("\n");
    }

/* Data ports */

    if (!ConfigDataPort(qdp, &config->dport[0], QDP_LOGICAL_PORT_1, &config->fix, delay, ntry, verbose)) return FALSE;
    if (!ConfigDataPort(qdp, &config->dport[1], QDP_LOGICAL_PORT_2, &config->fix, delay, ntry, verbose)) return FALSE;
    if (!ConfigDataPort(qdp, &config->dport[2], QDP_LOGICAL_PORT_3, &config->fix, delay, ntry, verbose)) return FALSE;
    if (!ConfigDataPort(qdp, &config->dport[3], QDP_LOGICAL_PORT_4, &config->fix, delay, ntry, verbose)) return FALSE;

/* C1_SC */

    if (config->set & QDP_CONFIG_DEFINED_SENSCTRL) {
        qdpEncode_C1_SC(&pkt, config->sc);
        if (verbose) {printf("Loading sensor control map"); fflush(stdout);}
        if (!qdpPostCmd(qdp, &pkt, TRUE)) {
            if (verbose) printf(" *** FAILED ***\n");
            return FALSE;
        }
        if (verbose) printf("\n");
    }

/* C1_SPP */

    if (config->set & QDP_CONFIG_DEFINED_SLAVE) {
        qdpEncode_C1_SPP(&pkt, &config->spp);
        if (verbose) {printf("Loading slave processor parameters"); fflush(stdout);}
        if (!qdpPostCmd(qdp, &pkt, TRUE)) {
            if (verbose) printf(" *** FAILED ***\n");
            return FALSE;
        }
        if (verbose) printf("\n");
    }

/* C2_PHY serial 1 */

    if (config->set & QDP_CONFIG_DEFINED_ADVSER1) {
        qdpEncode_C2_PHY(&pkt, &config->phy2[QDP_PP_SERIAL_1]);
        if (verbose) {printf("Loading advanced serial port 1 parameters"); fflush(stdout);}
        if (!qdpPostCmd(qdp, &pkt, TRUE)) {
            if (verbose) printf(" *** FAILED ***\n");
            return FALSE;
        }
        if (verbose) printf("\n");
    }

/* C2_PHY serial 2 */

    if (config->set & QDP_CONFIG_DEFINED_ADVSER2) {
        qdpEncode_C2_PHY(&pkt, &config->phy2[QDP_PP_SERIAL_2]);
        if (verbose) {printf("Loading advanced serial port 2 parameters"); fflush(stdout);}
        if (!qdpPostCmd(qdp, &pkt, TRUE)) {
            if (verbose) printf(" *** FAILED ***\n");
            return FALSE;
        }
        if (verbose) printf("\n");
    }

/* C2_PHY ethernet */

    if (config->set & QDP_CONFIG_DEFINED_ADVETHER) {
        qdpEncode_C2_PHY(&pkt, &config->phy2[QDP_PP_ETHERNET]);
        if (verbose) {printf("Loading advanced ethernet parameters"); fflush(stdout);}
        if (!qdpPostCmd(qdp, &pkt, TRUE)) {
            if (verbose) printf(" *** FAILED ***\n");
            return FALSE;
        }
        if (verbose) printf("\n");
    }

/* C2_GPS */

    if (config->set & QDP_CONFIG_DEFINED_GPS || config->set & QDP_CONFIG_DEFINED_PLL) {

    /* The C2_GPS packet is defined in the <gps> and <pll> records of the XML file.  In the
     * event that only one of those is specified we here ensure that the other remains unchanged.
     */
        if (!(config->set & QDP_CONFIG_DEFINED_GPS) || !(config->set & QDP_CONFIG_DEFINED_PLL)) {
            if (!qdp_C2_RQGPS(qdp, &crnt.c2_gps)) {
                if (verbose) printf("Unable to determine current GPS/VCO configuration!\n");
                return FALSE;
            }
            if (!(config->set & QDP_CONFIG_DEFINED_GPS)) {
                config->gps.mode          = crnt.c2_gps.mode;
                config->gps.flags         = crnt.c2_gps.flags;
                config->gps.off_time      = crnt.c2_gps.off_time;
                config->gps.resync        = crnt.c2_gps.resync;
                config->gps.max_on        = crnt.c2_gps.max_on;
            } else {
                config->gps.lock_usec     = crnt.c2_gps.lock_usec;
                config->gps.interval      = crnt.c2_gps.interval;
                config->gps.initial_pll   = crnt.c2_gps.initial_pll;
                config->gps.pfrac         = crnt.c2_gps.pfrac;
                config->gps.vco_slope     = crnt.c2_gps.vco_slope;
                config->gps.vco_intercept = crnt.c2_gps.vco_intercept;
                config->gps.max_ikm_rms   = crnt.c2_gps.max_ikm_rms;
                config->gps.ikm_weight    = crnt.c2_gps.ikm_weight;
                config->gps.km_weight     = crnt.c2_gps.km_weight;
                config->gps.best_weight   = crnt.c2_gps.best_weight;
                config->gps.km_delta      = crnt.c2_gps.km_delta;
            }
        }
        qdpEncode_C2_GPS(&pkt, &config->gps);
        if (verbose) {printf("Loading GPS/PLL parameters"); fflush(stdout);}
        if (!qdpPostCmd(qdp, &pkt, TRUE)) {
            if (verbose) printf(" *** FAILED ***\n");
            return FALSE;
        }
        if (verbose) printf("\n");
    }

/* C2_AMASS */

    if (config->set & QDP_CONFIG_DEFINED_AUTOMASS) {
        qdpEncode_C2_AMASS(&pkt, &config->amass);
        if (verbose) {printf("Loading automatic mass re-centering parameters"); fflush(stdout);}
        if (!qdpPostCmd(qdp, &pkt, TRUE)) {
            if (verbose) printf(" *** FAILED ***\n");
            return FALSE;
        }
        if (verbose) printf("\n");
    }

/* C3_ANNC */

    if (config->set & QDP_CONFIG_DEFINED_ANNOUNCE) {
        qdpEncode_C3_ANNC(&pkt, &config->annc);
        if (verbose) {printf("Loading announcements"); fflush(stdout);}
        if (!qdpPostCmd(qdp, &pkt, TRUE)) {
            if (verbose) printf(" *** FAILED ***\n");
            return FALSE;
        }
        if (verbose) printf("\n");
    }

/* C2_EPCFG */

    if ((qdp->flags & QDP_FLAGS_EP_SUPPORTED) && (config->set & QDP_CONFIG_DEFINED_EPCFG)) {
        qdpEncode_C2_EPCFG(&pkt, &config->epcfg);
        if (verbose) {printf("Loading environmental processor configuration"); fflush(stdout);}
        if (!qdpPostCmd(qdp, &pkt, TRUE)) {
            if (verbose) printf(" *** FAILED ***\n");
            return FALSE;
        }
        if (verbose) printf("\n");
    }

/* Webpage */

#ifdef notdef
    if (config->set & QDP_CONFIG_DEFINED_WEBPAGE) {
        qdpPackWebpageMemblk(&blk, config);
        if (verbose) {printf("Loading web page\n"); fflush(stdout);}
        if (!qdpPostMemBlk(qdp, &blk, delay, ntry, NULL)) {
            if (verbose) printf("FAILED: %s\n", qdpErrString(qdp->cmd.err));
            return FALSE;
        }
    }
#endif

    return TRUE;
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
 * $Log: config.c,v $
 * Revision 1.13  2017/09/20 19:18:35  dauerbach
 * added check for QEP support before updating C2_EPCFG
 *
 * Revision 1.12  2016/01/21 17:17:28  dechavez
 * added environmental processor support
 *
 * Revision 1.11  2015/12/04 23:15:12  dechavez
 * casts, format fixes and the like, all to calm OS X compiles
 *
 * Revision 1.10  2012/06/24 18:05:14  dechavez
 * introduce qdpRequestDportConfig() and qdpConfigureDataPort(), also major cleanup of qdpSetConfig() to use qdpConfigureDataPort()
 *
 * Revision 1.9  2011/02/01 19:40:29  dechavez
 * qdpSetConfig() to accept caller imposed override of ethernet IP
 *
 * Revision 1.8  2010/12/23 21:47:09  dechavez
 * replaced QDP_NUM_LOGICAL_PORTS with QDP_NLP, replaced explicit numerical indices
 * in config->log and config->token with symbolic values
 *
 * Revision 1.7  2010/12/22 23:40:24  dechavez
 * use explicit symbolic port numbers when requesting data port config
 *
 * Revision 1.6  2009/11/13 00:32:10  dechavez
 * changed commentary, added new qdpPostMemBlk() args (for memory busy retries)
 *
 * Revision 1.5  2009/11/05 18:38:11  dechavez
 * added qdpSetConfig() (webpage not implemented yet... gets error response from Q330)
 *
 * Revision 1.4  2009/10/29 17:43:00  dechavez
 * removed qdpRequestWebpage() (moved to webpage.c)
 *
 * Revision 1.3  2009/10/20 22:41:05  dechavez
 * better qdpInitConfig()
 *
 * Revision 1.2  2009/10/10 00:30:46  dechavez
 * added support for config set bitmask
 *
 * Revision 1.1  2009/10/02 18:33:26  dechavez
 * initial release
 *
 */
