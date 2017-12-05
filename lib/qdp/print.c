#pragma ident "$Id: print.c,v 1.37 2017/09/14 00:25:17 dauerbach Exp $"
/*======================================================================
 * 
 * Print stuff for debugging
 *
 *====================================================================*/
#include "qdp.h"

typedef struct {
    char *text;
    char *description;
    UINT16 code;
} TEXT_MAP;

static TEXT_MAP WaveformMap[] = {
    {"sine",  "sine wave",     QDP_QCAL_SINE},
    {"red",   "red noise",     QDP_QCAL_RED},
    {"white", "white noise",   QDP_QCAL_WHITE},
    {"step",  "step",          QDP_QCAL_STEP},
    {"rb",    "random binary", QDP_QCAL_RB},
    {NULL, NULL, -1}
};

static char *WaveformString(UINT16 value)
{
int i;
static char *undefined = "UNDEFINED";

    for (i = 0; WaveformMap[i].text != NULL; i++) {
        if (value == WaveformMap[i].code) return WaveformMap[i].description;
    }
    return undefined;
}

static char *ChannelString(UINT16 bitmap)
{
int i;
static char none[] = " none";
static char string[64];

    if (bitmap == 0) return none;

    string[0] = 0;
    for (i = 0; i < QDP_NCHAN; i++) {
        if (bitmap & (1 << i)) sprintf(string+strlen(string), " %d", i+1);
    }
    return string;
}

static float FrequencyValue(UINT16 waveform, UINT16 divisor)
{
float base, result;

    base = waveform == QDP_QCAL_SINE ? 1.0 : 125.0;
    result = base / divisor;

    return result;
}

#define MAXBAUD 8
static int baud[MAXBAUD+2] = {-1, 1200, 2400, 4800, 9600, 19200, 38400, 57600, 115200, -12345};

int qdpBaud(int code)
{
    if (code < 0 || code > MAXBAUD) code = MAXBAUD;
     
    return baud[code];
}

int qdpBaudCode(int value)
{
int i;

    for (i = 0; i < MAXBAUD+2; i++) if (value == baud[i]) return i;
    return baud[0];
}

void qdpPrintBT_SPEC(FILE *fp, QDP_BLOCKETTE_SPEC *blk)
{
float freq;

    switch (blk->type) {
      case QDP_SP_TYPE_CALBEG:
        freq = FrequencyValue(blk->data.calbeg.waveform, blk->data.calbeg.divisor);
        fprintf(fp, "Calibration Start Blockette\n");
        fprintf(fp, "           channel = %02x\n", blk->data.calbeg.channel);
        fprintf(fp, "          waveform = %s\n", WaveformString(blk->data.calbeg.waveform));
        fprintf(fp, "         amplitude = -%ddb\n", 6 * (blk->data.calbeg.amplitude + 1));
        fprintf(fp, "          duration = %hu seconds\n", blk->data.calbeg.duration);
        fprintf(fp, "   calibrate chans = %s\n", ChannelString(blk->data.calbeg.chans));
        fprintf(fp, "     monitor chans = %s\n", ChannelString(blk->data.calbeg.monitor));
        fprintf(fp, "         frequency = %.4f Hz (%.4f sec period)\n", freq, 1.0 / freq);
        break;

      case QDP_SP_TYPE_CALEND:
        fprintf(fp, "Calibration Abort Blockette\n");
        fprintf(fp, "           channel = %02x\n", blk->data.calend.channel);
        fprintf(fp, "   calibrate chans = %s\n", ChannelString(blk->data.calend.chans));
        fprintf(fp, "     monitor chans = %s\n", ChannelString(blk->data.calend.monitor));
        break;

      case QDP_SP_TYPE_CNPBLK:
        fprintf(fp, "CNP block data\n");
        fprintf(fp, "   0x%02x", blk->data.cnpblk.channel);
        fprintf(fp, " 0x%02x", blk->data.cnpblk.flags);
        fprintf(fp, " %3d", blk->data.cnpblk.port);
        fprintf(fp, " %3d", blk->data.cnpblk.unitid);
        fprintf(fp, " 0x%02x", blk->data.cnpblk.status);
        fprintf(fp, " %02x", blk->data.cnpblk.type);
        fprintf(fp, " %5hu\n", blk->data.cnpblk.size);
        break;

      case QDP_SP_TYPE_CFGBLK:
        fprintf(fp, "Configuration change data follow\n");
        switch (blk->data.cfgblk.type) {
          case QDP_CFG_FIX:  qdpPrint_C1_FIX(fp, &blk->data.cfgblk.fix);   break;
          case QDP_CFG_GLOB: qdpPrint_C1_GLOB(fp, &blk->data.cfgblk.glob); break;
          case QDP_CFG_SS1:  qdpPrint_SSCFG(fp, &blk->data.cfgblk.ss1);    break;
          case QDP_CFG_SS2:  qdpPrint_SSCFG(fp, &blk->data.cfgblk.ss2);    break;
          case QDP_CFG_AUXAD:  fprintf(fp, "%hu bytes of AuxAD configuration data ignored\n", blk->data.cfgblk.size); break;
          default: fprintf(fp, "Unknown configuration change (type %d)\n", blk->data.cfgblk.type);
        }
        break;

      case QDP_SP_TYPE_ENVDAT:
        fprintf(fp, "Environmental processor data\n");
        fprintf(fp, "        fc = 0x%02x\n", blk->data.envdat.fc);
        fprintf(fp, "   channel = 0x%02x\n", blk->data.envdat.channel);
        if (blk->data.envdat.size == 8) {
            fprintf(fp, "     value = %d\n", blk->data.envdat.value);
        } else {
            fprintf(fp, "     %d bytes of compressed EP data ignored\n", blk->data.envdat.size - 4);
        }
        break;

      default:
        fprintf(fp, "unknown special blockette type 0x%0x ignored\n", blk->type);
    }
}

static void DumpSteimStuff(FILE *fp, QDP_BLOCKETTE_COMP *blk)
{
int i, result;
UINT32 nsamp;
INT32 output[QDP_MAX_SPS];

    fprintf(fp, "%d bytes of map\n", blk->maplen);
    for (i = 0; i < blk->maplen; i++) {
        fprintf(fp, "%2d: %02x = ", i+1, blk->map[i]);
        utilPrintBinUINT8(stdout, blk->map[i]);
        fprintf(fp, "\n");
    }

    fprintf(fp, "%lu bytes (%d sequences) of data\n", blk->comp.nseq * sizeof(UINT32), blk->comp.nseq);
    for (i = 0; i < blk->comp.nseq; i++) {
        fprintf(fp, "%2d: %08x = ", i+1, blk->comp.seq[i]);
        utilPrintBinUINT32(stdout, blk->comp.seq[i]);
        fprintf(fp, "\n");
    }

    result = qdpDecompressCOMP(blk, output, QDP_MAX_SPS, &nsamp);
    if (result != QDP_OK) {
        fprintf(fp, "*** decompression failure *** result code = %d\n", result);
        return;
    }
    fprintf(fp, "%u samples recovered\n", nsamp);
    for (i = 0; i < nsamp; i++) {
        fprintf(fp, "%10d ", output[i]);
        if (i > 0 && (i+1) % 10 == 0) fprintf(fp, "\n");
    }
}

void qdpPrintBT_38(FILE *fp, QDP_BLOCKETTE_38 *blk)
{
    fprintf(fp, "bit8[0]=0x%02x ", blk->bit8[0]);
    fprintf(fp, "bit8[1]=0x%02x ", blk->bit8[1]);
    fprintf(fp, "bit8[2]=0x%02x ", blk->bit8[2]);
    fprintf(fp, "\n");
}

void qdpPrintBT_816(FILE *fp, QDP_BLOCKETTE_816 *blk)
{
    fprintf(fp, "bit8=0x%02x ", blk->bit8);
    fprintf(fp, "bit16=0x%04x ", blk->bit16);
    fprintf(fp, "\n");
}

void qdpPrintBT_316(FILE *fp, QDP_BLOCKETTE_316 *blk)
{
    fprintf(fp, "bit8=0x%02x ", blk->bit8);
    fprintf(fp, "bit16[0]=0x%04x ", blk->bit16[0]);
    fprintf(fp, "bit16[1]=0x%04x ", blk->bit16[1]);
    fprintf(fp, "bit16[2]=0x%04x ", blk->bit16[2]);
    fprintf(fp, "\n");
}

void qdpPrintBT_32(FILE *fp, QDP_BLOCKETTE_32 *blk)
{
    fprintf(fp, "bit8=0x%02x ", blk->bit8);
    fprintf(fp, "bit16=0x%04x ", blk->bit16);
    fprintf(fp, "bit32=0x%08x ", blk->bit32);
    fprintf(fp, "\n");
}

void qdpPrintBT_232(FILE *fp, QDP_BLOCKETTE_232 *blk)
{
    fprintf(fp, "bit8=0x%02x ", blk->bit8);
    fprintf(fp, "bit16=0x%04x ", blk->bit16);
    fprintf(fp, "bit32[0]=0x%08x ", blk->bit32[0]);
    fprintf(fp, "bit32[1]=0x%08x ", blk->bit32[1]);
    fprintf(fp, "\n");
}

void qdpPrintBT_COMP(FILE *fp, QDP_BLOCKETTE_COMP *blk)
{
int len;

    fprintf(fp, "bit8=0x%02x ", blk->bit8);
    fprintf(fp, "bit16=0x%04x ", blk->bit16);
    fprintf(fp, "prev=%d ", blk->prev);
    fprintf(fp, "doff=%hu ", blk->doff);
    fprintf(fp, "filt=%d ", blk->filt);
    fprintf(fp, "chan=%d ", blk->chan);
    fprintf(fp, "size=%d ", blk->size);
    fprintf(fp, "nseq=%d ", blk->comp.nseq);
    fprintf(fp, "\n");
    len = blk->comp.nseq * sizeof(UINT32);
}

void qdpPrintBT_MULT(FILE *fp, QDP_BLOCKETTE_MULT *blk)
{
    fprintf(fp, "bit8=0x%02x ", blk->bit8);
    fprintf(fp, "bit16=0x%04x ", blk->bit16);
    fprintf(fp, "filt=%d ", blk->filt);
    fprintf(fp, "size=%d ", blk->size);
    fprintf(fp, "segno=%d ", blk->segno);
    fprintf(fp, "lastseg=%s ", blk->lastseg ? "yes" : "no");
    fprintf(fp, "\n");
}

void qdpPrintBlockette(FILE *fp, QDP_DT_BLOCKETTE *blk, UINT32 seqno)
{
REAL64 tstamp;
QDP_MN232_DATA mn232;

    fprintf(fp, "channel=0x%02x ", blk->channel);
    fprintf(fp, "root=0x%02x ", blk->root);
    fprintf(fp, "ident=0x%02x ", blk->ident);
    fprintf(fp, "resid=0x%02x ", blk->resid);
    fprintf(fp, "is_status=%s ", blk->is_status ? "TRUE" : "FALSE");
    fprintf(fp, "\n");
    fprintf(fp, "%s: ", qdpBlocketteIdentString(blk->ident));
    switch (blk->format) {
      case QDP_BT_UNKNOWN: fprintf(fp, "UNKNOWN_BLOCKETTE\n"); break;
      case QDP_BT_38:      qdpPrintBT_38(fp, &blk->data.bt_38); break;
      case QDP_BT_816:     qdpPrintBT_816(fp, &blk->data.bt_816); break;
      case QDP_BT_316:     qdpPrintBT_316(fp, &blk->data.bt_316); break;
      case QDP_BT_32:      qdpPrintBT_32(fp, &blk->data.bt_32); break;
      case QDP_BT_232:     qdpPrintBT_232(fp, &blk->data.bt_232); break;
      case QDP_BT_COMP:    qdpPrintBT_COMP(fp, &blk->data.bt_comp); break;
      case QDP_BT_MULT:    qdpPrintBT_MULT(fp, &blk->data.bt_mult); break;
      case QDP_BT_SPEC:    qdpPrintBT_SPEC(fp, &blk->data.bt_spec); break;
      default:             fprintf(fp, "UNRECOGNIZED BLOCKETTE\n"); break;
    }
    if (blk->channel == QDP_DC_MN232) {
        qdpLoadMN232(&blk->data.bt_232, &mn232);
        tstamp = ((REAL64) qdpRootTimeTag(&mn232) / (REAL64) NANOSEC_PER_SEC) + QDP_EPOCH_TO_1970_EPOCH;
        fprintf(fp, "tstamp = %s, qual=0x%02x, loss=%d\n", utilDttostr(tstamp, 0, NULL), mn232.qual, mn232.loss);
    }
}

void qdpPrintBlockettes(FILE *fp, QDP_TYPE_DT_DATA *dt_data)
{
LNKLST_NODE *crnt;

    crnt = listFirstNode(&dt_data->blist);
    while (crnt != NULL) {
        qdpPrintBlockette(fp, (QDP_DT_BLOCKETTE *) crnt->payload, dt_data->seqno);
        crnt = listNextNode(crnt);
    }
}

void qdpPrintTokenVersion(FILE *fp, QDP_TOKEN_VERSION *src)
{
    if (src == NULL) {
        fprintf(fp, "NULL QDP_TOKEN_VERSION\n");
        return;
    }
    if (src->valid) {
        fprintf(fp, "version = %d\n", src->version);
    } else {
        fprintf(fp, "QDP_TOKEN_VERSION not set\n");
    }
}

void qdpPrintTokenSite(FILE *fp, QDP_TOKEN_SITE *src)
{
    if (src == NULL) {
        fprintf(fp, "NULL QDP_TOKEN_SITE\n");
        return;
    }
    if (src->valid) {
        fprintf(fp, "network = %s, station name = %s\n", src->nname, src->sname);
    } else {
        fprintf(fp, "QDP_TOKEN_SITE not set\n");
    }
}

void qdpPrintTokenServer(FILE *fp, QDP_TOKEN_SERVER *src)
{
    if (src == NULL) {
        fprintf(fp, "NULL QDP_TOKEN_SERVER\n");
        return;
    }
    if (src->valid) {
        fprintf(fp, "port = %hd\n", src->port);
    } else {
        fprintf(fp, "QDP_TOKEN_SERVER not set\n");
    }
}

void qdpPrintTokenServices(FILE *fp, QDP_TOKEN_SERVICES *src)
{
    fprintf(fp, "net: "); qdpPrintTokenServer(fp, &src->net);
    fprintf(fp, "web: "); qdpPrintTokenServer(fp, &src->web);
    fprintf(fp, "data: "); qdpPrintTokenServer(fp, &src->data);
}

void qdpPrintTokenDss(FILE *fp, QDP_TOKEN_DSS *src)
{
    if (src == NULL) {
        fprintf(fp, "NULL QDP_TOKEN_DSS\n");
        return;
    }
    if (src->valid) {
        fprintf(fp, "DSS parameters:\n");
        fprintf(fp, "passwords = %s,%s,%s\n", src->passwd.hi, src->passwd.mi, src->passwd.lo);
        fprintf(fp, "  timeout = %u\n", src->timeout);
        fprintf(fp, "   maxbps = %u\n", src->maxbps);
        fprintf(fp, "verbosity = %hhu\n", src->verbosity);
        fprintf(fp, "   maxcpu = %hhu\n", src->maxcpu);
        fprintf(fp, "     port = %hu\n", src->port);
        fprintf(fp, "   maxmem = %hu\n", src->maxmem);
    } else {
        fprintf(fp, "QDP_TOKEN_DSS not set\n");
    }
}

void qdpPrintTokenClock(FILE *fp, QDP_TOKEN_CLOCK *src)
{
    if (src == NULL) {
        fprintf(fp, "NULL QDP_TOKEN_CLOCK\n");
        return;
    }
    if (src->valid) {
        fprintf(fp, "Clock Processing parameters:\n");
        fprintf(fp, "         Timezone offset from GMT = %d seconds\n", src->offset);
        fprintf(fp, "    Loss before downgrading by 1%% = %hd minutes\n", src->maxlim);
        fprintf(fp, "               PLL Locked quality = %d%%\n", src->pll.locked);
        fprintf(fp, "             PLL Tracking quality = %d%%\n", src->pll.track);
        fprintf(fp, "                 PLL Hold quality = %d%%\n", src->pll.hold);
        fprintf(fp, "                  PLL Off quality = %d%%\n", src->pll.off);
        fprintf(fp, "Highest 'has been locked' quality = %d%%\n", src->maxhbl);
        fprintf(fp, " Lowest 'has been locked' quality = %d%%\n", src->minhbl);
        fprintf(fp, "        Never been locked quality = %d%%\n", src->nbl);
        fprintf(fp, "             Clock Quality Filter = %hu seconds\n", src->clkqflt);
    } else {
        fprintf(fp, "QDP_TOKEN_CLOCK not set\n");
    }
}

void qdpPrintTokenLogid(FILE *fp, QDP_TOKEN_LOGID *src)
{
    if (src == NULL) {
        fprintf(fp, "NULL QDP_TOKEN_LOGID\n");
        return;
    }
    if (src->valid) {
        fprintf(fp, "Log and Timing Identification:\n");
        fprintf(fp, "message logid = '%s'%s'\n", src->mesg.chn, src->mesg.loc);
        fprintf(fp, " timing logid = '%s'%s'\n", src->time.chn, src->time.loc);
    } else {
        fprintf(fp, "QDP_TOKEN_LOGID not set\n");
    }
}

void qdpPrintTokenCnfid(FILE *fp, QDP_TOKEN_CNFID *src)
{
    if (src == NULL) {
        fprintf(fp, "NULL QDP_TOKEN_CNFID\n");
        return;
    }
    if (src->valid) {
        fprintf(fp, "Configuration Identification:\n");
        fprintf(fp, "    name = '%s'%s'\n", src->chn, src->loc);
        fprintf(fp, "   flags = 0x%02x\n", src->flags);
        fprintf(fp, "interval = %hu\n", src->interval);
    } else {
        fprintf(fp, "QDP_TOKEN_CNFID not set\n");
    }
}

void qdpPrintTokenLcq(FILE *fp, QDP_TOKEN_LCQ *src)
{
int i, count;

    if (src == NULL) {
        fprintf(fp, "NULL QDP_TOKEN_LCQ\n");
        return;
    }
    fprintf(fp, "chn='%s' ", src->chn);
    fprintf(fp, "loc='%s' ", src->loc);
    fprintf(fp, "src=(0x%02x, %d) ", src->src[0], src->src[1]);
    fprintf(fp, "sint=%7.3lf ",  src->dsint);
    fprintf(fp, "mode=%s ", src->options & QDP_LCQ_TRIGGERED_MODE ? "trig" : "cont");
    fprintf(fp, "opt="); utilPrintBinUINT32(fp, src->options);
    fprintf(fp, "\n");
    if (src->options) fprintf(fp, "    ");
    if (src->options & QDP_LCQ_HAVE_PRE_EVENT_BUFFERS) fprintf(fp, "pebuf=%hu ", src->pebuf);
    if (src->options & QDP_LCQ_HAVE_GAP_THRESHOLD) fprintf(fp, "gapthresh=%.3g ", src->gapthresh);
    if (src->options & QDP_LCQ_HAVE_CALIB_DELAY) fprintf(fp, "caldly=%hu ", src->caldly);
    if (src->options & QDP_LCQ_HAVE_FRAME_COUNT) fprintf(fp, "comfr=%u ", src->comfr);
    if (src->options & QDP_LCQ_HAVE_FIR_MULTIPLIER) fprintf(fp, "firfix=%.3g ", src->firfix);
    if (src->options & QDP_LCQ_HAVE_CNTRL_DETECTOR) fprintf(fp, "cntrl=%hhu ", src->cntrl.code);
    fprintf(fp, "\n");
    for (count=0, i = 0; i < QDP_LCQ_NUM_DETECT; i++) {
        if (src->detect[i].set) {
            if (++count == 1) {
                fprintf(fp, "    ");
            } else {
                fprintf(fp, " ");
            }
            fprintf(fp, "detect[%d]=%s,%d, options=", i, src->detect[i].base.name, src->detect[i].use);
            utilPrintBinUINT8(fp, src->detect[i].options);
        }
    }
    if (count) fprintf(fp, "\n");
}

void qdpPrintTokenLcqList(FILE *fp, LNKLST *src)
{
int i;
QDP_TOKEN_LCQ *lcq;

    if (src == NULL) {
        fprintf(fp, "NULL LCP LNKLIST\n");
        return;
    }
    fprintf(fp, "%d Logical Channel Queues:\n", src->count);
    for (i = 0; i < src->count; i++) {
        lcq = (QDP_TOKEN_LCQ *) src->array[i];
        fprintf(fp, "%2d: ", i+1);
        qdpPrintTokenLcq(fp, lcq);
    }
}

void qdpPrintTokenIir(FILE *fp, QDP_TOKEN_IIR *iir)
{
int i;
static char *hp = "hi-pass";
static char *lp = "lo-pass";

    fprintf(fp, "name='%s', ", iir->name);
    fprintf(fp, "gain=%.3g, ", iir->gain);
    fprintf(fp, "refreq=%.3g, ", iir->refreq);
    fprintf(fp, "%d sections\n", iir->nsection);
    for (i = 0; i < iir->nsection; i++) {
        fprintf(fp, "   %2d: ", i+1);
        fprintf(fp, "type=%s ", iir->data[i].type == QDP_IIR_HIPASS ? hp : lp);
        fprintf(fp, "npole=%2d ", iir->data[i].npole);
        fprintf(fp, "ratio=%10.3g\n", iir->data[i].ratio);
    }
}

void qdpPrintTokenIirList(FILE *fp, LNKLST *src)
{
int i;
QDP_TOKEN_IIR *iir;

    if (src == NULL) {
        fprintf(fp, "NULL IIR LNKLIST\n");
        return;
    }
    fprintf(fp, "%d IIR Filters:\n", src->count);
    for (i = 0; i < src->count; i++) {
        iir = (QDP_TOKEN_IIR *) src->array[i];
        fprintf(fp, "%2d: ", i+1);
        qdpPrintTokenIir(fp, iir);
    }
}

void qdpPrintTokenFir(FILE *fp, QDP_TOKEN_FIR *fir)
{
    fprintf(fp, "id=%d, name=%s\n", fir->id, fir->name);
}

void qdpPrintTokenFirList(FILE *fp, LNKLST *src)
{
int i;
QDP_TOKEN_FIR *fir;

    if (src == NULL) {
        fprintf(fp, "NULL FIR LNKLIST\n");
        return;
    }
    fprintf(fp, "%d FIR Filters:\n", src->count);
    for (i = 0; i < src->count; i++) {
        fir = (QDP_TOKEN_FIR *) src->array[i];
        fprintf(fp, "%2d: ", i+1);
        qdpPrintTokenFir(fp, fir);
    }
}

void qdpPrintTokenCds(FILE *fp, QDP_TOKEN_CDS *cds)
{
int i;

    fprintf(fp, "id=%d, ", cds->id);
    fprintf(fp, "options=0x%02x(", cds->options);
    utilPrintBinUINT8(fp, cds->options);
    fprintf(fp, "), name=%s ", cds->name);
    fprintf(fp, "nentry=%d\n", cds->nentry);
    for (i = 0; i < cds->nentry; i++) {
        fprintf(fp, "%d: code=%02x (", i+1, cds->equation[i].code);
        utilPrintBinUINT8(fp, cds->equation[i].code);
        fprintf(fp, ") =");
        switch ((cds->equation[i].code & 0xC0) >> 6) {
          case QDP_CDS_NIB_COMM: fprintf(fp, " comm "); break;
          case QDP_CDS_NIB_DET:  fprintf(fp, " det  "); break;
          case QDP_CDS_NIB_CAL:  fprintf(fp, " cal  "); break;
          case QDP_CDS_NIB_OP:   fprintf(fp, " op   "); break;
          default:               fprintf(fp, " ???? "); break;
        }
        fprintf(fp, "%s\n", cds->equation[i].name);
    }
}

void qdpPrintTokenCdsList(FILE *fp, LNKLST *src)
{
int i;
QDP_TOKEN_CDS *cds;

    if (src == NULL) {
        fprintf(fp, "NULL CDS LNKLIST\n");
        return;
    }
    fprintf(fp, "%d Control Detector Specifications:\n", src->count);
    for (i = 0; i < src->count; i++) {
        cds = (QDP_TOKEN_CDS *) src->array[i];
        fprintf(fp, "%2d: ", i+1);
        qdpPrintTokenCds(fp, cds);
    }
}

void qdpPrintTokenDet(FILE *fp, QDP_TOKEN_DET *det)
{
    fprintf(fp, "id=%d, ", det->id);
    fprintf(fp, "name=%s, ", det->name);
    fprintf(fp, "type=%s, ", qdpDetectorString(det->type));
    fprintf(fp, "detf=%d (%s), ", det->detf.code, det->detf.name);
    fprintf(fp, "iw=%d, ", det->iw);
    fprintf(fp, "nht=%d, ", det->nht);
    fprintf(fp, "fhi=%d, ", det->fhi);
    fprintf(fp, "flo=%d, ", det->flo);
    if (det->type == QDP_TOKEN_TYPE_MHD) {
        fprintf(fp, "wa=%d, ", det->wa);
        fprintf(fp, "tc=%d, ", det->tc);
        fprintf(fp, "x1=%d, ", det->x1);
        fprintf(fp, "x2=%d, ", det->x2);
        fprintf(fp, "x3=%d, ", det->x3);
        fprintf(fp, "xx=%d, ", det->xx);
        fprintf(fp, "av=%d\n", det->av);
    }
}

void qdpPrintTokenDetList(FILE *fp, LNKLST *src)
{
int i;
QDP_TOKEN_DET *det;

    if (src == NULL) {
        fprintf(fp, "NULL DET LNKLIST\n");
        return;
    }
    fprintf(fp, "%d Event Detector Specifications:\n", src->count);
    for (i = 0; i < src->count; i++) {
        det = (QDP_TOKEN_DET *) src->array[i];
        fprintf(fp, "%2d: ", i+1);
        qdpPrintTokenDet(fp, det);
    }
}

void qdpPrintTokenCenList(FILE *fp, LNKLST *src)
{
int i;
QDP_TOKEN_CEN *cen;

    if (src == NULL) {
        fprintf(fp, "NULL CEN LNKLIST\n");
        return;
    }
    fprintf(fp, "%d Comm Event Names:\n", src->count);
    for (i = 0; i < src->count; i++) {
        cen = (QDP_TOKEN_CEN *) src->array[i];
        fprintf(fp, "%2d: id=%d, name=%s\n", i+1, cen->id, cen->name);
    }
}

void qdpPrintTokens(FILE *fp, QDP_DP_TOKEN *src)
{
    if (src == NULL) {
        fprintf(fp, "NULL QDP_DP_TOKEN\n");
        return;
    }
    qdpPrintTokenVersion(fp, &src->ver);
    qdpPrintTokenLogid(fp, &src->logid);
    qdpPrintTokenCnfid(fp, &src->cnfid);
    qdpPrintTokenDss(fp, &src->dss);
    qdpPrintTokenClock(fp, &src->clock);
    qdpPrintTokenCenList(fp, &src->cen);
    qdpPrintTokenFirList(fp, &src->fir);
    qdpPrintTokenIirList(fp, &src->iir);
    qdpPrintTokenDetList(fp, &src->det);
    qdpPrintTokenLcqList(fp, &src->lcq);
    qdpPrintTokenCdsList(fp, &src->cds);
    qdpPrintTokenSite(fp, &src->site);
    qdpPrintTokenServices(fp, &src->srvr);
}

void qdpPrintTokenChannelMap(FILE *fp, LNKLST *src)
{
int i;
QDP_TOKEN_LCQ *lcq;

    if (src == NULL) {
        fprintf(fp, "NULL LCP LNKLIST\n");
        return;
    }
    fprintf(fp, "    %d logical channels\n", src->count);
    fprintf(fp, "    Chn Lc Src 1   2   Sint\n");
    fprintf(fp, "    --- -- ----- ---- ------\n");

    for (i = 0; i < src->count; i++) {
        lcq = (QDP_TOKEN_LCQ *) src->array[i];
        fprintf(fp, "    ");
        fprintf(fp, "%3s", util_lcase(lcq->chn));
        fprintf(fp, " %2s", util_lcase(lcq->loc));
        fprintf(fp, "  0x%02x", lcq->src[0]);
        fprintf(fp, " 0x%02x", lcq->src[1]);
        fprintf(fp, " %6.3lf", lcq->dsint);
        if (lcq->src[0] == 0 && lcq->src[1] == 0) fprintf(fp, " (not produced in Q330)");
        fprintf(fp, "\n");
    }
}

void qdpPrintTokenReport(FILE *fp, QDP_DP_TOKEN *src)
{
    fprintf(fp, "Station: %s  Network: %s\n", src->site.sname, src->site.nname);
    fprintf(fp, "\n");
    qdpPrintTokenChannelMap(fp, &src->lcq);
    //fprintf(fp, "\n");
    //qdpPrintTokenClock(fp, &src->clock);
}

void qdpPrint_C1_FIX(FILE *fp, QDP_TYPE_C1_FIX *fix)
{
int i, chan, freq;
char string[64];

    fprintf(fp, "Fixed values after reboot:\n");
    fprintf(fp, "--------------------------\n");
    utilLttostr(fix->last_reboot + QDP_EPOCH_TO_1970_EPOCH, 0, string);
    fprintf(fp, "  last_reboot = %s (%u)\n", string, fix->last_reboot);
    fprintf(fp, "      reboots = %u\n", fix->reboots);
    fprintf(fp, "   backup_map = 0x%08x\n", fix->backup_map);
    fprintf(fp, "  default_map = 0x%08x\n", fix->default_map);
    fprintf(fp, "     cal_type = %hu\n", fix->cal_type);
    fprintf(fp, "      cal_ver = %d.%d (%hu)\n", fix->cal_ver.major, fix->cal_ver.minor, fix->cal_ver.raw);
    fprintf(fp, "     aux_type = %hu\n", fix->aux_type);
    fprintf(fp, "      aux_ver = %d.%d (%hu)\n", fix->aux_ver.major, fix->aux_ver.minor, fix->aux_ver.raw);
    fprintf(fp, "     clk_type = %hu\n", fix->clk_type);
    fprintf(fp, "        flags = 0x%04x\n", fix->flags);
    fprintf(fp, "      sys_ver = %d.%d (%hu)\n", fix->sys_ver.major, fix->sys_ver.minor, fix->sys_ver.raw);
    fprintf(fp, "       sp_ver = %d.%d (%hu)\n", fix->sp_ver.major, fix->sp_ver.minor, fix->sp_ver.raw);
    fprintf(fp, "      pld_ver = %d.%d (%hu)\n", fix->pld_ver.major, fix->pld_ver.minor, fix->pld_ver.raw);
    fprintf(fp, "    mem_block = %hu\n", fix->mem_block);
    fprintf(fp, "   proper_tag = %u\n", fix->proper_tag);
    fprintf(fp, "      sys_num = %016llX\n", fix->sys_num);
    fprintf(fp, "      amb_num = %016llX\n", fix->amb_num);
    for (i = 0; i < QDP_NSENSOR; i++) fprintf(fp, "sensor_num[%d] = %016llX\n", i, fix->sensor_num[i]);
    fprintf(fp, "  qapchp1_num = %08X\n", fix->qapchp1_num);
    fprintf(fp, "       int_sz = %u\n", fix->int_sz);
    fprintf(fp, "     int_used = %u\n", fix->int_used);
    for (i = 0; i < QDP_NLP; i++) fprintf(fp, "    log_sz[%d] = %u\n", i, fix->log_sz[i]);
    for (freq = QDP_NFREQ - 1; freq >= 0; freq--) fprintf(fp, "      freq[%d] = %7.3lf (0x%02x)\n", freq, fix->freq[freq].dsint, fix->freq[i].raw);
    for (freq = QDP_NFREQ - 1; freq >= 0; freq--) fprintf(fp, "ch13_delay[%d] = %d\n", freq, fix->ch13_delay[freq]);
    for (freq = QDP_NFREQ - 1; freq >= 0; freq--) fprintf(fp, "ch46_delay[%d] = %d\n", freq, fix->ch46_delay[freq]);
}

void qdpPrint_C1_GID(FILE *fp, QDP_TYPE_C1_GID *gid)
{
int i, numstr;

    fprintf(fp, "GPS ID Strings\n");
    fprintf(fp, "--------------\n");
    for (numstr = 0, i = 0; i < QDP_NGID; i++) {
        if (strlen(gid[i]) > 0) {
            fprintf(fp, "%s\n", gid[i]);
            ++numstr;
        }
    }

    if (numstr == 0) fprintf(fp, "<none>\n");
}

void qdpPrint_C1_GLOB(FILE *fp, QDP_TYPE_C1_GLOB *glob)
{
int i, j;

    fprintf(fp, "Global Programming\n");
    fprintf(fp, "------------------\n");
    fprintf(fp, "      clock_to = %hu\n", glob->clock_to);
    fprintf(fp, "   initial_vco = %hu\n", glob->initial_vco);
    fprintf(fp, "    gps_backup = %hu\n", glob->gps_backup);
    fprintf(fp, "    samp_rates = %hu\n", glob->samp_rates);
    fprintf(fp, "      gain_map = "); utilPrintBinUINT16(fp, glob->gain_map); fprintf(fp, "\n");
    fprintf(fp, "    filter_map = "); utilPrintBinUINT16(fp, glob->filter_map); fprintf(fp, "\n");
    fprintf(fp, "     input_map = "); utilPrintBinUINT16(fp, glob->input_map); fprintf(fp, "\n");
    fprintf(fp, "      web_port = %hu\n", glob->web_port);
    fprintf(fp, "     server_to = %hu\n", glob->server_to);
    fprintf(fp, "     drift_tol = %hu\n", glob->drift_tol);
    fprintf(fp, "     jump_filt = %hu\n", glob->jump_filt);
    fprintf(fp, "   jump_thresh = %hu\n", glob->jump_thresh);
    fprintf(fp, "    cal_offset = %hd\n", glob->cal_offset);
    fprintf(fp, "    sensor_map = "); utilPrintBinUINT16(fp, glob->sensor_map); fprintf(fp, "\n");
    fprintf(fp, "sampling_phase = %hu\n", glob->sampling_phase);
    fprintf(fp, "      gps_cold = %hu\n", glob->gps_cold);
    fprintf(fp, "      user_tag = %hd\n", glob->cal_offset);
    for (i = 0; i < QDP_NCHAN; i++) {
        for (j = 0; j < QDP_NFREQ; j++) {
            fprintf(fp, " scaling[%d][%d] = %hd\n", i, j, glob->scaling[i][j]);
        }
    }
    for (i = 0; i < QDP_NCHAN; i++) fprintf(fp, "     offset[%d] = %hd\n", i, glob->offset[i]);
    for (i = 0; i < QDP_NCHAN; i++) fprintf(fp, "       gain[%d] = %hd\n", i, glob->gain[i]);
    fprintf(fp, "       msg_map = %u\n", glob->msg_map);
}

void qdpPrintInputBitmap(FILE *fp, UINT16 bitmap)
{
static char buf[] = "1=calibrator 2=calibrator 3=calibrator 4=calibrator 5=calibrator 6=calibrator + slop";

    fprintf(fp, "%s\n", qdpInputBitmapString(bitmap, buf));
}

void qdpPrintPreampBitmap(FILE *fp, UINT16 bitmap)
{
static char buf[] = "1=disabled 2=disabled 3=disabled 4=disabled 5=disabled 6=disabled + slop";

    fprintf(fp, "%s\n", qdpPreampBitmapString(bitmap, buf));
}

void qdpPrint_C1_LOG(FILE *fp, QDP_TYPE_C1_LOG *log)
{
int i;

    fprintf(fp, "C1_LOG\n");
    fprintf(fp, "------\n");
    fprintf(fp, "        port = %hu\n", log->port);
    fprintf(fp, "       flags = 0x%04x\n", log->flags);
    fprintf(fp, "        perc = %.1f%% (%hu)\n", log->perc / 2.56, log->perc);
    fprintf(fp, "         mtu = %hu\n", log->mtu);
    fprintf(fp, "   group_cnt = %hu\n", log->group_cnt);
    fprintf(fp, "    rsnd_max = %hu\n", log->rsnd_max);
    fprintf(fp, "      grp_to = %.2f (%hu)\n", log->grp_to * 0.1, log->grp_to);
    fprintf(fp, "    rsnd_min = %hu\n", log->rsnd_min);
    fprintf(fp, "      window = %hu\n", log->window);
    fprintf(fp, "     dataseq = %hu\n", log->dataseq);
    for (i = 0; i < QDP_NCHAN; i++) fprintf(fp, "    freqs[%d] = 0x%04x\n", i, log->freqs[i]);
    fprintf(fp, "     ack_cnt = %hu\n", log->ack_cnt);
    fprintf(fp, "      ack_to = %.2f (%hu)\n", log->ack_to * 0.1, log->ack_to);
    fprintf(fp, "     olddata = %u\n", log->olddata);
    if (log->eth_throttle) {
        fprintf(fp, "eth_throttle = %d (%hu)\n", 1024000 / log->eth_throttle, log->eth_throttle);
    } else {
        fprintf(fp, "eth_throttle = disabled (%hu)\n", log->eth_throttle);
    }
    fprintf(fp, "  full_alert = %.1f%% (%hu)\n", log->full_alert / 2.56, log->full_alert);
    fprintf(fp, " auto_filter = %hu\n", log->auto_filter);
    fprintf(fp, "  man_filter = %hu\n", log->man_filter);
}

void qdpPrint_C1_COMBO(FILE *fp, QDP_TYPE_C1_COMBO *combo)
{
    qdpPrint_C1_FIX(fp, &combo->fix);
    qdpPrint_C1_GLOB(fp, &combo->glob);
    qdpPrint_C1_LOG(fp, &combo->log);
}

void qdpPrint_C1_PHY(FILE *fp, QDP_TYPE_C1_PHY *phy)
{
int i;

    fprintf(fp, "Physical Interfaces\n");
    fprintf(fp, "-------------------\n");
    fprintf(fp, "           serial no = %016llx\n", phy->serialno);
    fprintf(fp, "           base port = %hu\n", phy->baseport);
    fprintf(fp, "            ethernet = %s %02x:%02x:%02x:%02x:%02x:%02x 0x%04x (",
        utilDotDecimalString(phy->ethernet.ip, NULL),
        phy->ethernet.mac[0], phy->ethernet.mac[1], phy->ethernet.mac[2], 
        phy->ethernet.mac[3], phy->ethernet.mac[4], phy->ethernet.mac[5],
        phy->ethernet.flags
    );
    utilPrintBinUINT16(fp, phy->ethernet.flags);
    fprintf(fp, ")\n");
    for (i = 0; i < 3; i++) {
        fprintf(fp, "       serial port %d = %s %d baud 0x%04x (",
            i+1, utilDotDecimalString(phy->serial[i].ip, NULL), qdpBaud(phy->serial[i].baud), phy->serial[i].flags
        );
        utilPrintBinUINT16(fp, phy->serial[i].flags);
        fprintf(fp, ")\n");
    }
}

void qdpPrint_C1_SC(FILE *fp, UINT32 *sc)
{
int i;

    fprintf(fp, "Sensor Control Mapping\n");
    fprintf(fp, "----------------------\n");
    for (i = 0; i < QDP_NSC; i++) {
        fprintf(fp, "line %d is %s", i+1, qdpSensorControlString(sc[i]));
        if ((sc[i] & QDP_SC_MASK) == QDP_SC_IDLE) {
            fprintf(fp, "\n");
        } else {
            fprintf(fp, ", active %s\n", sc[i] & QDP_SC_ACTIVE_HIGH_BIT ? "high" : "low");
        }
    }
}

void qdpPrint_C1_DCP(FILE *fp, QDP_TYPE_C1_DCP *dcp)
{
int i;

    fprintf(fp, "Digitizer Calibration Results\n");
    fprintf(fp, "-----------------------------\n");
    fprintf(fp, "Channel   Grounded   Reference\n");

    for (i = 0; i < QDP_NCHAN; i++) fprintf(fp, "%4d %13d %11d\n", i+1, dcp->grounded[i], dcp->reference[i]);
}

void qdpPrint_C1_DCPMAN(FILE *fp, QDP_TYPE_C1_DCP *dcp, QDP_TYPE_C1_MAN *man)
{
int i;
double error;

    fprintf(fp, "Digitizer Calibration Results\n");
    fprintf(fp, "-----------------------------\n");
    fprintf(fp, "Channel   DC Offset   Gain Error (%%)\n");

    for (i = 0; i < QDP_NCHAN; i++) {
        error = 100.0 * ((double) (dcp->reference[i] - man->reference[i]) / (double) man->reference[i]);
        fprintf(fp, "%4d %13d %11.3lf\n", i+1, dcp->grounded[i], error);
    }
}

void qdpPrint_C1_SPP(FILE *fp, QDP_TYPE_C1_SPP *spp)
{
    fprintf(fp, "Operational Limits\n");
    fprintf(fp, "------------------\n");
    fprintf(fp, "        Max Main Current = ");
    if (spp->max_main_current == 0) fprintf(fp, "<disabled>\n"); else fprintf(fp, "%huma\n", spp->max_main_current);
    fprintf(fp, "            Min Off Time = %hu sec\n", spp->min_off_time);
    fprintf(fp, "Min Power Supply Voltage = %.2fV\n", (float) spp->min_ps_voltage * 150.0 / 1000.0);
    fprintf(fp, "     Max Antenna Current = %huma\n", spp->max_antenna_current);
    fprintf(fp, "         Min Temperature = %hd C\n", spp->min_temp);
    fprintf(fp, "         Max Temperature = +%hd C\n", spp->max_temp);
    fprintf(fp, "  Temperature Hysteresis = %hu C\n", spp->temp_hysteresis);
    fprintf(fp, "      Voltage Hysteresis = %.2fV\n", (float) spp->volt_hysteresis * 150.0 / 1000.0);
    fprintf(fp, "       Default VCO value = %hu\n", spp->default_vco);
}

void qdpPrint_C1_MAN(FILE *fp, QDP_TYPE_C1_MAN *man)
{
int i;

    fprintf(fp, "Manufacturer's Area\n");
    fprintf(fp, "-------------------\n");
    fprintf(fp, "         Password =");
    for (i = 0; i < 4; i++) fprintf(fp, " %016X", man->password[i]);
    fprintf(fp, "\n");
    fprintf(fp, "      Clock Model = %s\n", qdpClocktypeString(man->clk_type));
    fprintf(fp, "     System Model = %u\n", man->model);
    fprintf(fp, "Calibrator Offset = %hd\n", man->default_calib_offset);
    fprintf(fp, "    Packet Memory = %u Mb\n", man->packet_memory / (1024 * 1024));
    fprintf(fp, " KMI Property Tag = %u\n", man->proper_tag);
    fprintf(fp, "            Flags = "); utilPrintBinUINT16(fp, man->flags); fprintf(fp, "\n");
    fprintf(fp, "QAPCHP 1 type = %hu, version = %d.%d", man->qapchp1_type, man->qapchp1_ver.major, man->qapchp1_ver.minor);
    if (man->flags & QDP_MAN_CHN13_26BIT) fprintf(fp, "  26 bits");
    fprintf(fp, "\n");
    fprintf(fp, "  serial number = %u\n", (UINT32) (0xffffffff & man->qapchp1_num));
    for (i = 0; i < 3; i++) fprintf(fp, "  Channel %d Ref. Counts = %d\n", i+1, man->reference[i]);

    fprintf(fp, "QAPCHP 2 type = %hu, version = %d.%d", man->qapchp2_type, man->qapchp2_ver.major, man->qapchp2_ver.minor);
    if (man->flags & QDP_MAN_CHN46_26BIT) fprintf(fp, "  26 bits");
    fprintf(fp, "\n");
    fprintf(fp, "  serial number = %u\n", (UINT32) (0xffffffff & man->qapchp2_num));
    for (i = 3; i < 6; i++) fprintf(fp, "  Channel %d Ref. Counts = %d\n", i+1, man->reference[i]);
}

void qdpPrint_C2_AMASS_SENSOR(FILE *fp, QDP_TYPE_C2_AMASS_SENSOR *sensor)
{
int i;

    fprintf(fp, "Tolerances =");
    for (i = 0; i < 3; i++) fprintf(fp, " %3hu", sensor->tolerance[i]);
    fprintf(fp, "\n");
    fprintf(fp, "In range reset interval = %hu, ", sensor->interval.normal);
    fprintf(fp, "integration squelch = %hu (minutes)\n", sensor->interval.squelch);
    fprintf(fp, "Maximum tries = %hu\n", sensor->maxtry);
    fprintf(fp, "Sensor control bitmap = "); utilPrintBinUINT16(fp, sensor->bitmap); fprintf(fp, "\n");
    fprintf(fp, "Duration = %.2lf sec\n", (float) sensor->duration / 100.0);
}

void qdpPrint_C2_AMASS(FILE *fp, QDP_TYPE_C2_AMASS *amass)
{
int i, j;
BOOL off = TRUE;
static char label[QDP_NSENSOR] = {'A', 'B'};

    for (i = 0; i < QDP_NSENSOR; i++) {
        for (off = TRUE, j = 0; j < 3; j++) if (amass->sensor[i].tolerance[j] != 0) off = FALSE;
        if (off) {
            fprintf(fp, "Sensor %c automatic recentering is not enabled\n", label[i]);
         } else {
            fprintf(fp, "Sensor %c Automatic Mass Recenter parameters\n", label[i]);
            fprintf(fp, "-------------------------------------------\n");
            qdpPrint_C2_AMASS_SENSOR(fp, &amass->sensor[i]);
        }
    }
}

void qdpPrint_C2_GPS(FILE *fp, QDP_TYPE_C2_GPS *gps)
{
#define GPS_MODE_MASK 0x0007

    fprintf(fp, "GPS Configuration\n");
    fprintf(fp, "-----------------\n");
    fprintf(fp, "             main mode = ");
    switch (gps->mode & 0x0007) {
      case 0: fprintf(fp, "Internal GPS\n"); break;
      case 1: fprintf(fp, "External GPS\n"); break;
      case 2: fprintf(fp, "External SeaScan\n"); break;
      case 3: fprintf(fp, "Network Timing\n"); break;
      case 4: fprintf(fp, "External Tx/Rx access to internal GPS\n"); break;
      default: fprintf(fp, "??? 0x%04x ???\n", gps->mode);
    }
    fprintf(fp, "  export NMEA and 1PPS = %s\n", (gps->mode & 0x0008) ? "ON" : "off");
    fprintf(fp, "             interface = %s\n", (gps->mode & 0x0010) ? "RS-422" : "RS-232");
    fprintf(fp, "     serial DGPS input = %s\n", (gps->mode & 0x0020) ? "ON" : "off");
    fprintf(fp, "QDP network DGPS input = %s\n", (gps->mode & 0x0040) ? "ON" : "off");
    fprintf(fp, "    Power cycling mode = ");
    switch (gps->flags) {
      case 0: fprintf(fp, "continuous\n"); break;
      case 1: fprintf(fp, "off after %hu minutes\n", gps->max_on); break;
      case 2: fprintf(fp, "off after %hu minutes or PLL lock\n", gps->max_on); break;
      case 3: fprintf(fp, "off after %hu minutes or GPS lock\n", gps->max_on); break;
    }
    fprintf(fp, "          GPS Off Time = %hu minutes\n", gps->off_time);
    fprintf(fp, "      GPS Re-Sync Hour = %hu\n", gps->resync);
    fprintf(fp, "         PLL lock usec = %hu\n", gps->lock_usec);
    fprintf(fp, "   PLL update interval = %hu sec\n", gps->interval);
    fprintf(fp, "             PLL flags = "); utilPrintBinUINT8(fp, (UINT8) gps->initial_pll);
    if (gps->initial_pll & 0x1) fprintf(fp, " | enable PLL");
    if (gps->initial_pll & 0x2) fprintf(fp, " | 2-D lock OK");
    if (gps->initial_pll & 0x4) fprintf(fp, " | enable VCO temperature compensation");
    fprintf(fp, "\n");
    fprintf(fp, "                 Pfrac = %8.4f\n", gps->pfrac);
    fprintf(fp, "             VCO Slope = %8.4f\n", gps->vco_slope);
    fprintf(fp, "         VCO Intercept = %8.4f\n", gps->vco_intercept);
    fprintf(fp, "   Max. Initial KM RMS = %8.4f\n", gps->max_ikm_rms);
    fprintf(fp, "     Initial KM Weight = %8.4f\n", gps->ikm_weight);
    fprintf(fp, "             KM Weight = %8.4f\n", gps->km_weight);
    fprintf(fp, "       Best VCO Weight = %8.4f\n", gps->best_weight);
    fprintf(fp, "              KM Delta = %8.4f\n", gps->km_delta);
}

void qdpPrintCheckoutPackets(FILE *fp, QDP_TYPE_CHECKOUT *co)
{
    fprintf(fp, "Q330 Checkout Results for digitizer '%s'\n", co->ident);
    fprintf(fp, "%s\n", utilDttostr((long) time(NULL), 1, NULL));
    fprintf(fp, "\n");
    fprintf(fp, "Hardware and Software\n");
    fprintf(fp, "---------------------\n");
    fprintf(fp, "     Q330 Serial Number = %016llX\n", co->fix.sys_num);
    fprintf(fp, "      AMB Serial Number = %016llX\n", co->fix.amb_num);
    fprintf(fp, "  QAPCHP1 Serial Number = %u\n", (UINT32) (0x00000000FFFFFFFF & co->fix.qapchp1_num));;
    fprintf(fp, "  QAPCHP2 Serial Number = %u\n", (UINT32) (0x00000000FFFFFFFF & co->fix.qapchp2_num));;
    fprintf(fp, "       KMI Property Tag = %u\n", co->fix.proper_tag);
    fprintf(fp, "System Software Version = %d.%d\n", co->fix.sys_ver.major, co->fix.sys_ver.minor);
    fprintf(fp, "Slave Processor Version = %d.%d\n", co->fix.sp_ver.major, co->fix.sp_ver.minor);
    fprintf(fp, "        Calibrator type = %s\n", qdpCalibratorString(co->fix.cal_type));
    fprintf(fp, "     Calibrator Version = %d.%d\n", co->fix.cal_ver.major, co->fix.cal_ver.minor);
    fprintf(fp, "   Auxiliary Board Type = %s\n", qdpAuxtypeString(co->fix.aux_type));
    fprintf(fp, "Auxiliary Board Version = %d.%d\n", co->fix.aux_ver.major, co->fix.aux_ver.minor);
    fprintf(fp, "             Clock Type = %s\n", qdpClocktypeString(co->fix.clk_type));
    fprintf(fp, "            PLD Version = %d.%d\n", co->fix.pld_ver.major, co->fix.pld_ver.minor);
    fprintf(fp, "\n");
    qdpPrint_C1_SPP(fp, &co->spp);
    fprintf(fp, "\n");
    qdpPrint_C1_GID(fp, co->gid);
    fprintf(fp, "\n");
    qdpPrint_C2_GPS(fp, &co->gps);
    fprintf(fp, "\n");
    qdpPrint_C1_DCPMAN(fp, &co->dcp, &co->man);
    fprintf(fp, "\n");
    qdpPrint_C1_SC(fp, co->sc);
    fprintf(fp, "\n");
    qdpPrint_C2_AMASS(fp, &co->amass);
    fprintf(fp, "\n");
    qdpPrint_C1_STAT(fp, &co->status);
}

void qdpPrint_CMNHDR(FILE *fp, QDP_CMNHDR *hdr)
{
    fprintf(fp, "cmd=%-10s", qdpCmdString(hdr->cmd));
    fprintf(fp, " len=%hu", hdr->dlen);
    fprintf(fp, " seqno=%hu", hdr->seqno);
    fprintf(fp, " ack=%hu", hdr->ack);
    fprintf(fp, " crc=0x%08x", hdr->crc);
    fprintf(fp, "\n");
}

void qdpPrintPkt(FILE *fp, QDP_PKT *pkt, UINT16 print)
{
    if (print == 0) print = QDP_PRINT_HDR;

    if (print & QDP_PRINT_HDR) qdpPrint_CMNHDR(fp, &pkt->hdr);
    if (print & QDP_PRINT_PAYLOAD) utilPrintHexDump(fp, pkt->payload, pkt->hdr.dlen);
    if (print & QDP_PRINT_RAW) utilPrintHexDump(fp, pkt->raw, pkt->len);
}

void qdpPrintLCQ(FILE *fp, QDP_LCQ *lcq, UINT16 print)
{
QDP_LC *lc;
int i, count;
REAL64 tstamp;
LNKLST_NODE *crnt;

    tstamp = ((REAL64) qdpRootTimeTag(&lcq->mn232) / (REAL64) NANOSEC_PER_SEC) + QDP_EPOCH_TO_1970_EPOCH;
    fprintf(fp, "%016llX", lcq->meta.combo.fix.sys_num);
    fprintf(fp, " %s", utilDttostr(tstamp, 0, NULL));
    fprintf(fp, " (mn232:seqno=%u", lcq->mn232.seqno);
    fprintf(fp, " :sec=%u", lcq->mn232.sec);
    fprintf(fp, " :usec=%6u", lcq->mn232.usec);
    fprintf(fp, " :qual=0x%02x", lcq->mn232.qual);
    fprintf(fp, " :loss=%u)", lcq->mn232.loss);
    fprintf(fp, " qual=%d%%", lcq->qual);
    fprintf(fp, " nchan=%d", lcq->lc->count);
    fprintf(fp, "\n");

    count = 1;
    if ((print & QDP_PRINT_LCQLC) == QDP_PRINT_LCQLC) {
        crnt = listFirstNode(lcq->lc);
        while (crnt != NULL) {
            lc = (QDP_LC *) crnt->payload;
            fprintf(fp, "%4d: ", count++);
            fprintf(fp, "src=(0x%02x, %d): ", lc->src[0], lc->src[1]);
            fprintf(fp, "delay=%d ", lc->delay);
            fprintf(fp, "chn='%s' ", lc->chn);
            fprintf(fp, "loc='%s' ", lc->loc);
            fprintf(fp, "nsamp=%d\n", lc->nsamp);
            if ((print & QDP_PRINT_LCQDATA) == QDP_PRINT_LCQDATA) {
                for (i = 0; i < lc->nsamp; i++) {
                    fprintf(fp, "%10d ", lc->data[i]);
                    if (i > 0 && (i+1) % 10 == 0) fprintf(fp, "\n");
                }
                if (i < 10) fprintf(fp, "\n");
            }
            crnt = listNextNode(crnt);
        }
    }
}

void qdpPrint_SSCFG(FILE *fp, QDP_TYPE_SSCFG *sscfg)
{
    fprintf(fp, "Serial Sensor Configuration\n");
    fprintf(fp, "---------------------------\n");
    fprintf(fp, "                       port = %s\n", qdpSSPortString(sscfg->port));
    fprintf(fp, "                       type = %s\n", qdpSSTypeString(sscfg->type));
    fprintf(fp, "            sample interval = %hu seconds\n", sscfg->si);
    fprintf(fp, "                      units = %s\n", qdpSSUnitsString(sscfg->un));
    fprintf(fp, "           integration time = %hu ms\n", sscfg->it);
    fprintf(fp, "          fractional digits = %hu\n", sscfg->fd);
    fprintf(fp, "              serial number = %u\n", sscfg->sn);
    fprintf(fp, "        full scale pressure = %f\n", sscfg->fp);
    fprintf(fp, "             pressure adder = %f\n", sscfg->pa);
    fprintf(fp, "        pressure multiplier = %f\n", sscfg->pm);
    fprintf(fp, "time-base correction factor = %f\n", sscfg->tc);
    fprintf(fp, "    C1 pressure coefficient = %f\n", sscfg->c1);
    fprintf(fp, "    C2 pressure coefficient = %f\n", sscfg->c2);
    fprintf(fp, "    C3 pressure coefficient = %f\n", sscfg->c3);
    fprintf(fp, "    D1 pressure coefficient = %f\n", sscfg->d1);
    fprintf(fp, "    D2 pressure coefficient = %f\n", sscfg->d2);
    fprintf(fp, "    T1 pressure coefficient = %f\n", sscfg->t1);
    fprintf(fp, "    T2 pressure coefficient = %f\n", sscfg->t2);
    fprintf(fp, "    T3 pressure coefficient = %f\n", sscfg->t3);
    fprintf(fp, "    T4 pressure coefficient = %f\n", sscfg->t4);
    fprintf(fp, "    T5 pressure coefficient = %f\n", sscfg->t5);
    fprintf(fp, "    U0 pressure coefficient = %f\n", sscfg->u0);
    fprintf(fp, "    Y1 pressure coefficient = %f\n", sscfg->y1);
    fprintf(fp, "    Y2 pressure coefficient = %f\n", sscfg->y2);
    fprintf(fp, "    Y3 pressure coefficient = %f\n", sscfg->y3);
}

void qdpPrint_C2_EPD(FILE *fp, QDP_TYPE_C2_EPD *epd)
{
int i, src;
UINT8 subchan;

    fprintf(fp, "Environmental Processor Filter Delays\n");
    fprintf(fp, "-------------------------------------\n");
    fprintf(fp, "EP1: reserved=0x%08x\n", epd->res1);
    fprintf(fp, "EP2: reserved=0x%08x\n", epd->res2);
    fprintf(fp, "\n");
    fprintf(fp, "channel     delay  src description\n");
    fprintf(fp, "-------  --------  --- ------------------------\n");
    for (i = 0; i < epd->chancnt; i++) {
        subchan = epd->entry[i].channel & QDP_EP_SUBCHAN_MASK;
        src = (subchan == epd->entry[i].channel) ? 1 : 2;
        fprintf(fp, "  0x%02x", epd->entry[i].channel);
        fprintf(fp, "   %8d", epd->entry[i].usec);
        fprintf(fp, "  EP%d", src);
        fprintf(fp, " %s\n", qdpEPSubchanString(subchan));
    }
}

static void SetEpChnLoc(QDP_EPCHN channel, QDP_DP_TOKEN *token, char *chn, char *loc)
{
int i;
QDP_TOKEN_LCQ *lcq;

    sprintf(chn, "%03x", QDP_EP_SRC0);
    sprintf(loc, "%02x", channel);

    if (token == NULL) return;

    for (i = 0; i < token->lcq.count; i++) {
        lcq = (QDP_TOKEN_LCQ *) token->lcq.array[i];
        if (lcq->src[0] == QDP_EP_SRC0 && lcq->src[1] == channel) {
            strncpy(chn, lcq->chn, QDP_CNAME_LEN + 1);
            strncpy(loc, lcq->loc, QDP_LNAME_LEN + 1);
            util_lcase(chn);
            util_lcase(loc);
            return;
        }
    }
}

void qdpPrint_C2_EPCFG(FILE *fp, QDP_TYPE_C2_EPCFG *epcfg, QDP_DP_TOKEN *token)
{
int i;
char chn[QDP_CNAME_LEN+1], loc[QDP_LNAME_LEN+1];

    fprintf(fp, "Environmental Processor Configuration\n");
    fprintf(fp, "-------------------------------------\n");
    fprintf(fp, "spare=0x%04x\n", epcfg->spare);
    fprintf(fp, "EP1: reserved=0x%08x flags=0x%08x\n", epcfg->res1, epcfg->flags1);
    fprintf(fp, "EP2: reserved=0x%08x flags=0x%08x\n", epcfg->res2, epcfg->flags2);
    fprintf(fp, "\n");
    fprintf(fp, "               data port\n");
    fprintf(fp, "     chnlc src  1 2 3 4  description\n");
    fprintf(fp, "     ----- ---  -------  ------------------------\n");
    for (i = 0; i < epcfg->chancnt; i++) {
        SetEpChnLoc(epcfg->entry[i].channel, token, chn, loc);
        fprintf(fp, "%3d:", i+1);
        fprintf(fp, " %3s%2s", chn, loc);
        fprintf(fp, " EP%d ", epcfg->entry[i].source);
        fprintf(fp, " %c", (epcfg->entry[i].dpmask & 0x01) ? 'x' : '-');
        fprintf(fp, " %c", (epcfg->entry[i].dpmask & 0x02) ? 'x' : '-');
        fprintf(fp, " %c", (epcfg->entry[i].dpmask & 0x04) ? 'x' : '-');
        fprintf(fp, " %c", (epcfg->entry[i].dpmask & 0x08) ? 'x' : '-');
        fprintf(fp, "  %s", qdpEPSubchanString(epcfg->entry[i].channel & QDP_EP_SUBCHAN_MASK));
        fprintf(fp, "\n");
    }
}

/*-----------------------------------------------------------------------+
 |                                                                       |
 | Copyright (C) 2006 Regents of the University of California            |
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
 * $Log: print.c,v $
 * Revision 1.37  2017/09/14 00:25:17  dauerbach
 * introduced qdpPrintPreampBitmap()
 *
 * Revision 1.36  2017/09/06 21:55:53  dechavez
 * fixed typo in format string in qdpPrintLCQ()
 *
 * Revision 1.35  2017/01/20 17:15:23  dechavez
 * added crc to qdpPrint_CMNHDR(), mn232 to qdpPrintLCQ()
 *
 * Revision 1.34  2016/02/03 17:28:09  dechavez
 * changes related to introduction of QDP_SOFTVER for versions
 *
 * Revision 1.33  2016/01/23 00:18:06  dechavez
 * added qdpPrint_C2_EPD(), include tokens in qdpPrint_C2_EPCFG()
 *
 * Revision 1.32  2016/01/21 20:48:16  dechavez
 * fixed format string to calm OS X compile
 *
 * Revision 1.31  2016/01/21 17:42:45  dechavez
 * added qdpPrint_C2_EPCFG()
 *
 * Revision 1.30  2016/01/19 22:58:05  dechavez
 * environmental processor support
 *
 * Revision 1.29  2016/01/14 19:04:24  dechavez
 * qdpPrintTokenChannelMap() print src2 string in hex (motivated by EP sources)
 *
 * Revision 1.28  2016/01/04 21:35:13  dechavez
 * qdpPrintTokenChannelMap() changed to make src2 string two characters long (EP sources, for example)
 *
 * Revision 1.27  2015/12/04 23:15:12  dechavez
 * casts, format fixes and the like, all to calm OS X compiles
 *
 * Revision 1.26  2012/06/24 18:10:07  dechavez
 * lcq sint renamed to dsint
 *
 * Revision 1.25  2011/01/03 21:25:24  dechavez
 * changed qdpPrintInputBitmap() to use qdpInputBitmapString()
 *
 * Revision 1.24  2010/04/07 19:48:45  dechavez
 * added qdpPrintInputBitmap()
 *
 * Revision 1.23  2009/11/05 18:36:14  dechavez
 * removed clock processing parameters from qdpPrintTokenReport()
 *
 * Revision 1.22  2009/10/29 17:34:25  dechavez
 * print decoded configuration blockettes in qdpPrintBT_SPEC(), cleaned up detect formatting in qdpPrintTokenLcq
 *
 * Revision 1.21  2009/10/20 23:01:38  dechavez
 * qdpBaudCode(), qdpPrintTokenCds(), qdpPrint_SSCFG(), use QDP_CFG_x in qdpPrintBT_SPEC()
 *
 * Revision 1.20  2009/10/02 18:37:17  dechavez
 * added qdpPrint_C1_PHY()
 *
 * Revision 1.19  2009/09/28 17:39:50  dechavez
 * added support for det field, changed output ordering to more closely follow Willard ordering.
 *
 * Revision 1.18  2009/09/15 23:11:02  dechavez
 * added qdpPrintTokenChannelMap(), qdpPrintTokenReport(), changed format of qdpPrintTokenClock() output to make it more understandable
 *
 * Revision 1.17  2009/07/09 18:19:33  dechavez
 * introduced qdpPrintBlockettes(), added support for new QDP_BLOCKETTE_SPEC fields in qdpPrintBT_SPEC
 *
 * Revision 1.16  2009/02/23 21:55:55  dechavez
 * print all fields int qdpPrint_C2_GPS() and added new funcs qdpPrint_CMNHDR(), qdpPrintLCQ(), qdpPrintPkt()
 *
 * Revision 1.15  2009/02/04 17:28:59  dechavez
 * added qdpPrintCheckoutPackets(), qdpPrint_C2_GPS(), qdpPrint_C1_MAN(),
 * qdpPrint_C1_SPP(), qdpPrint_C1_DCPMAN(), qdpPrint_C1_DCP(), qdpPrint_C1_GID()
 * and added underlines to section headers
 *
 * Revision 1.14  2009/01/24 00:10:20  dechavez
 * added qdpPrint_C1_SC(), qdpPrint_C2_AMASS_SENSOR(), qdpPrint_C2_AMASS()
 *
 * Revision 1.13  2008/10/02 23:06:07  dechavez
 * changed utilIPString calls to utilDotDecimalString
 *
 * Revision 1.12  2008/10/02 22:47:20  dechavez
 * added qdpPrint_C1_PHY
 *
 * Revision 1.11  2008/01/07 20:38:10  dechavez
 * adding missing argument to detect[] string in qdpPrintTokenLcq
 *
 * Revision 1.10  2007/05/18 18:00:57  dechavez
 * initial production release
 *
 */
