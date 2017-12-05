#pragma ident "$Id: cf.c,v 1.3 2011/02/25 18:54:58 dechavez Exp $"
/*======================================================================
 *
 * Deal with CF records (not that any really exist)
 *
 *====================================================================*/
#include "dccdmx.h"

#define MY_MOD_ID DCCDMX_MOD_CF

static FILE_HANDLE raw, tmp, prt;

static int DecodeFilter(UINT8 *start, SAN_FILTER *out)
{
int i;
UINT16 sval;
UINT8 *ptr;

    ptr = start;
    ptr += utilUnpackUINT16(ptr, &sval); out->fid   = (UINT32) sval;
    ptr += utilUnpackUINT16(ptr, &sval); out->ntaps = (UINT32) sval;
    ptr += utilUnpackUINT16(ptr, &sval); out->decim = (UINT32) sval;

    for (i = 0; i < out->ntaps; i++) {
        ptr += utilUnpackREAL32(ptr, &out->coeff[i]);
    }
    
    return (int) (ptr - start);
}

static int DecodeFtable(UINT8 *start, SANIO_CONFIG *out)
{
int i;
UINT16 sval;
UINT8 *ptr;

    ptr = start;
    ptr += utilUnpackUINT16(ptr, &sval); out->nfilter = (INT32) sval;
    for (i = 0; i < out->nfilter; i++) {
        ptr += DecodeFilter(ptr, &out->filter[i]);
    }

    return (int) (ptr - start);
}

static int DecodeDetector(UINT8 *start, SANIO_CONFIG *out)
{
int i;
UINT32 lval;
UINT8 *ptr;

    ptr = start;
    ptr += utilUnpackUINT32(ptr, &lval); out->detector.stalen = (INT32) lval;
    ptr += utilUnpackUINT32(ptr, &lval); out->detector.ltalen = (INT32) lval;
    ptr += utilUnpackREAL32(ptr, &out->detector.thresh);
    ptr += utilUnpackREAL32(ptr, &out->detector.ethresh);
    ptr += utilUnpackUINT32(ptr, &lval); out->detector.vthresh = (UINT32) lval;
    ptr += utilUnpackUINT32(ptr, &lval); out->detector.chans = (UINT32) lval;
    ptr += utilUnpackUINT32(ptr, &lval); out->detector.pretrig = (INT32) lval;
    ptr += utilUnpackUINT32(ptr, &lval); out->detector.posttrig = (INT32) lval;

    return (int) (ptr - start);
}

static int DecodeStream(UINT8 *start, SAN_DASTREAM *out)
{
int i;
UINT8  bval;
UINT16 sval;
UINT32 lval;
UINT8 *ptr;

    ptr = start;
    ptr += utilUnpackBytes(ptr, (UINT8 *) out->name, 6); out->name[6] = 0;
    ptr += utilUnpackBytes(ptr, &bval, 1); out->dsp.chan = (INT32) bval;
    if (out->dsp.chan == 0xff) out->dsp.chan = -1;
    ptr += utilUnpackBytes(ptr, &bval, 1); out->dsp.flags = (INT32) bval;
    for (i = 0; i < SAN_MAX_CASCADES; i++) {
        ptr += utilUnpackBytes(ptr, &bval, 1); out->dsp.cascade[i] = (INT32) bval;
        if (out->dsp.cascade[i] == 0xff) out->dsp.cascade[i] = -1;
    }
     
    return (int) (ptr - start);
}

static int DecodeStreams(UINT8 *start, SANIO_CONFIG *out)
{
int i;
UINT16 sval;
UINT8 *ptr;
     
    ptr = start;
    ptr += utilUnpackUINT16(ptr, &sval); out->nstream = (INT32) sval;
    for (i = 0; i < out->nstream; i++) ptr += DecodeStream(ptr, &out->stream[i]);

    return (int) (ptr - start);
}

static void PrintCF(SANIO_CONFIG *config)
{
int i, mid, last, count;
char tmpbuf[16];
#define CASCADE(i, count) (config->stream[i].dsp.cascade[count])

    if ((prt.fp = fopen(prt.path, "a+")) == NULL) return;

    fprintf(prt.fp, "*** SAN configuration parameters ***\n");
    fprintf(prt.fp, "\n");
    fprintf(prt.fp, "%d filters:\n", config->nfilter);
    fprintf(prt.fp, "Id  # Taps    Decim    First coeff      " "Mid Coeff        Last Coeff\n");
    for (i = 0; i < config->nfilter; i++) {
        last = config->filter[i].ntaps - 1;
        mid  = (config->filter[i].ntaps / 2);
        fprintf(prt.fp, "%2ld ", config->filter[i].fid);
        fprintf(prt.fp, "%5ld ", config->filter[i].ntaps);
        fprintf(prt.fp, "%8ld ", config->filter[i].decim);
        fprintf(prt.fp, "%16.7e ", config->filter[i].coeff[0]);
        fprintf(prt.fp, "%16.7e ", config->filter[i].coeff[mid]);
        fprintf(prt.fp, "%16.7e\n", config->filter[i].coeff[last]);
    }

    fprintf(prt.fp, "\n");

    fprintf(prt.fp, "Event Detector Parameters:\n");
    fprintf(prt.fp, "Window lengths (samples): STA = %6d  LTA  = %6d\n", config->detector.stalen, config->detector.ltalen);
    fprintf(prt.fp, "STA/LTA Trigger ratios:   On  = %6.2f  Off  = %6.2f\n", config->detector.thresh, config->detector.ethresh);
    fprintf(prt.fp, "No. voters:               %d\n", config->detector.vthresh);
    fprintf(prt.fp, "Detector channels:       ");
    for (i = 0; i < sizeof(config->detector.chans)*8; i++) if (config->detector.chans & (1 << i)) fprintf(prt.fp, " %d", i);
    fprintf(prt.fp, " (bitmask = 0x%x)\n", config->detector.chans);
    fprintf(prt.fp, "Pre-event samples:        %d\n", config->detector.pretrig);
    fprintf(prt.fp, "Post-event samples:       %d\n", config->detector.posttrig);

    fprintf(prt.fp, "\n");

    fprintf(prt.fp, "%d streams:\n", config->nstream);
    fprintf(prt.fp, "  Name  Chan  Flags     Filters\n");
    for (i = 0; i < config->nstream; i++) {
        fprintf(prt.fp, "%6s ",   config->stream[i].name);
        if (config->stream[i].dsp.chan < 0) {
            fprintf(prt.fp, "  n/a   n/a          n/a");
        } else {
            fprintf(prt.fp, "%5ld ", config->stream[i].dsp.chan);
            fprintf(prt.fp, "%4ld ", config->stream[i].dsp.flags);
            tmpbuf[0] = 0;
            count = 0;
            while (count < 16 && CASCADE(i, count) != -1) {
                if (strlen(tmpbuf) != 0) sprintf(tmpbuf+strlen(tmpbuf), ",");
                sprintf(tmpbuf+strlen(tmpbuf), "%ld", CASCADE(i, count));
                ++count;
            }
            if (strlen(tmpbuf) == 0) sprintf(tmpbuf, "none");
            fprintf(prt.fp, "  %11s ", tmpbuf);
        }
        fprintf(prt.fp, "\n");
    }

    fprintf(prt.fp, "\n");
    fclose(prt.fp);
}

static void DecodeCF(void)
{
int nbytes;
SANIO_CONFIG config;
UINT8 work[sizeof(SANIO_CONFIG)], *ptr;

    rewind(tmp.fp);

    nbytes = fread(work, 1, sizeof(SANIO_CONFIG), tmp.fp);

    ptr = work;
    ptr += DecodeFtable(ptr, &config);
    ptr += DecodeDetector(ptr, &config);
    ptr += DecodeStreams(ptr, &config);

    unlink(tmp.path);

    PrintCF(&config);
}

void ProcessCF(UINT8 *buf, INT32 buflen, COUNTER *count)
{
IDA10_CF cf;
static char *fid = "ProcessCF";

    ++count->cf;

    if (fwrite(buf, 1, buflen, raw.fp) != buflen) {
        LogMsg("ERROR: %s: fwrite: %s: %s\n", fid, raw.path, strerror(errno));
        Exit(MY_MOD_ID + 1);
    }

    if (!ida10UnpackCF(buf, &cf)) {
        LogMsg("WARNING: %s: ida10UnpackCF: %s\n", fid, strerror(errno));
    } else {
        if ((tmp.fp = fopen(tmp.path, "a+")) == NULL) {
            LogMsg("WARNING: %s: fopen: %s: %s\n", fid, tmp.path, strerror(errno));
            return;
        } else {
            if (fwrite(cf.data, 1, cf.nbytes, tmp.fp) != cf.nbytes) {
                LogMsg("WARNING: %s: fwrite: %s: %s\n", fid, tmp.path, strerror(errno));
            } else {
                if (cf.seqno == cf.done) DecodeCF();
            }
            fclose(tmp.fp);
            tmp.fp = NULL;
        }
    }
}

void CloseCF(void)
{
    if (raw.fp == NULL) fclose(raw.fp);
}

void InitCF(char *ident)
{
static char *fid = "InitCF";

    sprintf(raw.path, "%s.cf", ident);
    if ((raw.fp = fopen(raw.path, "wb")) == NULL) {
        LogMsg("ERROR: %s: fopen: %s: %s", fid, raw.path, strerror(errno));
        Exit(MY_MOD_ID + 2);
    }
    sprintf(tmp.path, "%s.cftmp", ident);
    tmp.fp = NULL;
    sprintf(prt.path, "%s.config", ident);
    prt.fp == NULL;
}

/* Revision History
 *
 * $Log: cf.c,v $
 * Revision 1.3  2011/02/25 18:54:58  dechavez
 * initial release
 *
 */
