#pragma ident "$Id: data.c,v 1.13 2016/01/23 00:12:25 dechavez Exp $"
/*======================================================================
 *
 * Decode DT_DATA packets
 *
 *====================================================================*/
#include "qdp.h"

#ifdef ENABLE_DEBUG_BLOCKETTES
extern BOOL DebugFlag;
#endif /* ENABLE_DEBUG_BLOCKETTES */

#define IsStatusBlockette(byte) (!((byte) & QDP_DCM_STATUS))
#define BlocketteIdent(byte) ((byte) & (IsStatusBlockette(byte) ?  QDP_DCM_ST :  QDP_DCM))
#define BlocketteResid(byte) ((byte) & (IsStatusBlockette(byte) ? ~QDP_DCM_ST : ~QDP_DCM))

#ifdef ENABLE_DEBUG_BLOCKETTES
static void DumpBlockette(UINT8 *start, int len, char *tag)
{
UINT8 ident, resid;

    ident = BlocketteIdent(start[0]);
    resid = BlocketteResid(start[0]);
    printf("%d bytes of %s, ident=", len, tag); utilPrintBinUINT8(stdout, ident);
    printf(", resid=%d\n", resid);
    utilPrintHexDump(stdout, start, len);
}
#endif /* ENABLE_DEBUG_BLOCKETTES */

static int Decode_BT_38(UINT8 *start, QDP_DT_BLOCKETTE *blk, QDP_BLOCKETTE_38 *dest, BOOL *error)
{
UINT8 *ptr;

#ifdef ENABLE_DEBUG_BLOCKETTES
    if (DebugFlag) DumpBlockette(start, qdpLenBT_38(start), "BT_38");
#endif /* ENABLE_DEBUG_BLOCKETTES */


    *error = FALSE;
    ptr = start + 1; /* skip over channel */

    dest->bit8[0] = *ptr++;
    dest->bit8[1] = *ptr++;
    dest->bit8[2] = *ptr++;

    dest->seqno = blk->seqno;
    return (int) (ptr - start);
}

static int Decode_BT_816(UINT8 *start, QDP_DT_BLOCKETTE *blk, QDP_BLOCKETTE_816 *dest, BOOL *error)
{
UINT8 *ptr;

#ifdef ENABLE_DEBUG_BLOCKETTES
    if (DebugFlag) DumpBlockette(start, qdpLenBT_816(start), "BT_816");
#endif /* ENABLE_DEBUG_BLOCKETTES */

    *error = FALSE;
    ptr = start + 1; /* skip over channel */

    dest->bit8 = *ptr++;
    ptr += utilUnpackINT16(ptr, &dest->bit16);

    dest->seqno = blk->seqno;
    return (int) (ptr - start);
}

static int Decode_BT_316(UINT8 *start, QDP_DT_BLOCKETTE *blk, QDP_BLOCKETTE_316 *dest, BOOL *error)
{
UINT8 *ptr;

#ifdef ENABLE_DEBUG_BLOCKETTES
    if (DebugFlag) DumpBlockette(start, qdpLenBT_316(start), "BT_316");
#endif /* ENABLE_DEBUG_BLOCKETTES */

    *error = FALSE;
    ptr = start + 1; /* skip over channel */

    dest->bit8 = *ptr++;
    ptr += utilUnpackINT16(ptr, &dest->bit16[0]);
    ptr += utilUnpackINT16(ptr, &dest->bit16[1]);
    ptr += utilUnpackINT16(ptr, &dest->bit16[2]);

    dest->seqno = blk->seqno;
    return (int) (ptr - start);
}

static int Decode_BT_32(UINT8 *start, QDP_DT_BLOCKETTE *blk, QDP_BLOCKETTE_32 *dest, BOOL *error)
{
UINT8 *ptr;

#ifdef ENABLE_DEBUG_BLOCKETTES
    if (DebugFlag) DumpBlockette(start, qdpLenBT_32(start), "BT_32");
#endif /* ENABLE_DEBUG_BLOCKETTES */

    *error = FALSE;
    ptr = start + 1; /* skip over channel */

    dest->bit8 = *ptr++;
    ptr += utilUnpackINT16(ptr, &dest->bit16);
    ptr += utilUnpackINT32(ptr, &dest->bit32);

    dest->seqno = blk->seqno;
    return (int) (ptr - start);
}

static int Decode_BT_232(UINT8 *start, QDP_DT_BLOCKETTE *blk, QDP_BLOCKETTE_232 *dest, BOOL *error)
{
UINT8 *ptr;

#ifdef ENABLE_DEBUG_BLOCKETTES
    if (DebugFlag) DumpBlockette(start, qdpLenBT_232(start), "BT_232");
#endif /* ENABLE_DEBUG_BLOCKETTES */

    *error = FALSE;
    ptr = start + 1; /* skip over channel */

    dest->bit8 = *ptr++;
    ptr += utilUnpackINT16(ptr, &dest->bit16);
    ptr += utilUnpackINT32(ptr, &dest->bit32[0]);
    ptr += utilUnpackINT32(ptr, &dest->bit32[1]);

    dest->seqno = blk->seqno;
    return (int) (ptr - start);
}

static int Decode_BT_COMP(UINT8 *start, QDP_DT_BLOCKETTE *blk, QDP_BLOCKETTE_COMP *dest, BOOL *error)
{
UINT8 *ptr, *save;

#ifdef ENABLE_DEBUG_BLOCKETTES
    if (DebugFlag) DumpBlockette(start, qdpLenBT_COMP(start), "BT_COMP");
#endif /* ENABLE_DEBUG_BLOCKETTES */

    *error = FALSE;
    ptr = start + 1; /* skip over channel */

    dest->bit8 = *ptr++;
    save = ptr += utilUnpackUINT16(ptr, &dest->bit16);
    ptr += utilUnpackINT32(ptr, &dest->prev);
    ptr += utilUnpackUINT16(ptr, &dest->doff);

    dest->map = ptr;                                  /* flags start here */
    dest->comp.data = start + dest->doff;             /* data start here */
    dest->size = dest->bit16 & QDP_DMSZ;              /* blockette size */
    dest->comp.nseq = (dest->size - dest->doff) >> 2; /* number of 32 bit sequences */
    dest->filt = dest->bit8 & QDP_DMFBIT;             /* filter code (freq bit number) */
    dest->chan = blk->resid;                          /* input channel comes from "channel" residual */
    ptr = save + ((dest->size - 1) & 0xfffc);         /* blockettes are on 4-byte boundaries */

    dest->maplen = (int) (dest->comp.data - dest->map);
    dest->comp.seq = (UINT32 *) dest->comp.data;
#ifdef LTL_ENDIAN_HOST
    utilSwapUINT32(dest->comp.seq, dest->comp.nseq);
#endif

    dest->seqno = blk->seqno;
    return (int) (ptr - start);
}

int qdpLenBT_COMP(UINT8 *start)
{
UINT16 bit16, size;
UINT8 *ptr;

    ptr = start + 1; /* skip over channel */
    ptr++;           /* skip over bit 8 */
    ptr += utilUnpackUINT16(ptr, &bit16);
    size = bit16 & QDP_DMSZ;
    ptr += ((bit16 - 1) & 0xfffc);
    return (int) (ptr - start);
}

static int Decode_BT_MULT(UINT8 *start, QDP_DT_BLOCKETTE *blk, QDP_BLOCKETTE_MULT *dest, BOOL *error)
{
UINT8 *ptr, *save;

#ifdef ENABLE_DEBUG_BLOCKETTES
    if (DebugFlag) DumpBlockette(start, qdpLenBT_MULT(start), "BT_MULT");
#endif /* ENABLE_DEBUG_BLOCKETTES */

    *error = FALSE;
    ptr = start + 1; /* skip over channel */

    dest->bit8 = *ptr++;
    save = ptr += utilUnpackUINT16(ptr, &dest->bit16);
    dest->comp.seq = (UINT32 *) ptr;

    dest->filt = dest->bit8 & QDP_DMFBIT;
    dest->chan = blk->resid;                          /* input channel comes from "channel" residual */
    dest->size = dest->bit16 & QDP_DMSZ;
    dest->segno = dest->bit8 >> 3;
    dest->lastseg = dest->bit16 & QDP_DMLS ? TRUE : FALSE;
    ptr = save + ((dest->size - 1) & 0xfffc); /* blockettes are on 4-byte boundaries */

    dest->comp.nseq = (dest->size - sizeof(UINT32)) >> 2; /* number of 32 bit sequences */

    dest->seqno = blk->seqno;
    return (int) (ptr - start);
}

/* decode special purpose blockettes */

static int Decode_BT_SP_CALBEG(UINT8 *start, QDP_SP_CALBEG *dest)
{
int i;
UINT8 *ptr;

    ptr = start;

    dest->channel = *ptr++;
    ++ptr; /* skip over unused byte */
    ptr += utilUnpackUINT16(ptr, &dest->waveform);
    ptr += utilUnpackUINT16(ptr, &dest->amplitude);
    ptr += utilUnpackUINT16(ptr, &dest->duration);
    ptr += utilUnpackUINT16(ptr, &dest->chans);
    ptr += utilUnpackUINT16(ptr, &dest->monitor);
    ptr += utilUnpackUINT16(ptr, &dest->divisor);
    ptr += 2; /* skip over spare */
    for (i = 0; i < QDP_SP_NCOUPLE; i++) dest->coupling[i] = *ptr++;

    return (int) (ptr - start);
}

static int Decode_BT_SP_CALEND(UINT8 *start, QDP_SP_CALEND *dest)
{
UINT8 *ptr;

    ptr = start;

    dest->channel = *ptr++;
    ++ptr; /* skip over unused byte */
    ptr += utilUnpackUINT16(ptr, &dest->chans);
    ptr += utilUnpackUINT16(ptr, &dest->monitor);
    ptr += 2; /* skip over spare */

    return (int) (ptr - start);
}

static int Decode_BT_SP_CNPBLK(UINT8 *start, QDP_SP_CNPBLK *dest)
{
UINT8 *ptr;

    ptr = start;

    dest->channel = *ptr++;
    dest->flags = *ptr++;
    ptr += utilUnpackUINT16(ptr, &dest->size);
    dest->port = *ptr++;
    dest->unitid = *ptr++;
    dest->status = *ptr++;
    dest->type = *ptr++;
    /* ignore data for now */
    ptr = start + dest->size;

    return (int) dest->size;
}

static int Decode_BT_SP_CFGBLK(UINT8 *start, QDP_SP_CFGBLK *dest)
{
UINT8 *ptr;

    ptr = start;

    dest->channel = *ptr++;
    dest->type = *ptr++;
    ptr += utilUnpackUINT16(ptr, &dest->size);
    switch (dest->type) {
      case QDP_CFG_FIX: qdpDecode_C1_FIX(ptr, &dest->fix); break;
      case QDP_CFG_GLOB: qdpDecode_C1_GLOB(ptr, &dest->glob); break;
      case QDP_CFG_AUXAD: break;
      case QDP_CFG_SS1: qdpDecode_SSCFG(ptr, &dest->ss1); break;
      case QDP_CFG_SS2: qdpDecode_SSCFG(ptr, &dest->ss2); break;
    }

    return (int) dest->size;
}

static UINT64 EnvdatSampleInterval(UINT8 subchan)
{
    switch (subchan) {
      case C2_EP_SUBCHAN_ANALOG1_40HZ:
      case C2_EP_SUBCHAN_ANALOG2_40HZ:
      case C2_EP_SUBCHAN_ANALOG3_40HZ: return NANOSEC_PER_SEC / 40;

      case C2_EP_SUBCHAN_ANALOG1_20HZ:
      case C2_EP_SUBCHAN_ANALOG2_20HZ:
      case C2_EP_SUBCHAN_ANALOG3_20HZ: return NANOSEC_PER_SEC / 20;

      case C2_EP_SUBCHAN_ANALOG1_10HZ:
      case C2_EP_SUBCHAN_ANALOG2_10HZ:
      case C2_EP_SUBCHAN_ANALOG3_10HZ: return NANOSEC_PER_SEC / 10;
    }

    return NANOSEC_PER_SEC;
}

static int Decode_BT_SP_ENVDAT(UINT8 *start, QDP_SP_ENVDAT *dest, QDP_DT_BLOCKETTE *blk)
{
UINT8 *ptr, *save;

    dest->seqno = blk->seqno; /* for timing */

    ptr = start;

    dest->fc = *ptr++; /* Channel = $FC */
    dest->channel = *ptr++;
    dest->nsint = EnvdatSampleInterval(dest->channel & QDP_EP_SUBCHAN_MASK);
    save = ptr += utilUnpackUINT16(ptr, &dest->size);

    /* 8 byte blocks contain a single 32-bit sample value */

    if (dest->size == 8) {
        ptr += utilUnpackINT32(ptr, &dest->value);
        return (int) dest->size;
    }

    /* Otherwise we have variable length compressed data */

    ptr += utilUnpackINT32(ptr, &dest->prev);
    ptr += utilUnpackUINT16(ptr, &dest->doff);

    dest->map = ptr;                                  /* flags start here */
    dest->comp.data = start + dest->doff;             /* data start here */
    dest->comp.nseq = (dest->size - dest->doff) >> 2; /* number of 32 bit sequences */
    ptr = save + ((dest->size - 1) & 0xfffc);         /* blockettes are on 4-byte boundaries */

    dest->maplen = (int) (dest->comp.data - dest->map);
    dest->comp.seq = (UINT32 *) dest->comp.data;
#ifdef LTL_ENDIAN_HOST
    utilSwapUINT32(dest->comp.seq, dest->comp.nseq);
 #endif
    
    return (int) dest->size;
}

static int Decode_BT_SPEC(UINT8 *start, QDP_DT_BLOCKETTE *blk, QDP_BLOCKETTE_SPEC *dest, BOOL *error)
{
int i;
UINT8 *ptr;
UINT16 size;
static char *fid = "qdpDecode_DT_DATA:DecodeBlockette:Decode_BT_SPEC";

#ifdef ENABLE_DEBUG_BLOCKETTES
    if (DebugFlag) DumpBlockette(start, qdpLenBT_SPEC(start), "BT_SPEC");
#endif /* ENABLE_DEBUG_BLOCKETTES */

    *error = FALSE;

    ptr = start; /* include channel, unlike the others */

    switch (dest->type = blk->resid) {

      case QDP_SP_TYPE_CALBEG:
        ptr += Decode_BT_SP_CALBEG(ptr, &dest->data.calbeg);
        break;

      case QDP_SP_TYPE_CALEND:
        ptr += Decode_BT_SP_CALEND(ptr, &dest->data.calend);
        break;

      case QDP_SP_TYPE_CNPBLK:
        ptr += Decode_BT_SP_CNPBLK(ptr, &dest->data.cnpblk);
        break;

      case QDP_SP_TYPE_CFGBLK:
        ptr += Decode_BT_SP_CFGBLK(ptr, &dest->data.cfgblk);
        break;

      case QDP_SP_TYPE_ENVDAT:
        ptr += Decode_BT_SP_ENVDAT(ptr, &dest->data.envdat, blk);
        break;

      default:
        errno = EINVAL;
        *error = TRUE;
    }

    dest->seqno = blk->seqno;
    return (int) (ptr - start);
}

int qdpLenBT_SPEC(UINT8 *start)
{
int i;
UINT8 *ptr;
UINT16 size;
        
    ptr = start;
    switch (BlocketteResid(start[0])) {
      case QDP_SP_TYPE_CALBEG: size = 28; break;
      case QDP_SP_TYPE_CALEND: size =  8; break;
      case QDP_SP_TYPE_CNPBLK: utilUnpackUINT16(&start[2], &size); break;
      case QDP_SP_TYPE_CFGBLK: utilUnpackUINT16(&start[2], &size); break;
      case QDP_SP_TYPE_ENVDAT: utilUnpackUINT16(&start[2], &size); break;
      default:
        size = -1;
    }
     
    return (int) size;
}

/* Figure out which of the 8 possible formats a blockette takes */

static int BlocketteFormat(UINT8 ident)
{
int i;
static struct {
    UINT8 ident;
    int   format;
} BlocketteIdentMap[] = {
    {QDP_DC_ST38,    QDP_BT_38},
    {QDP_DC_ST816,   QDP_BT_816},
    {QDP_DC_ST32,    QDP_BT_32},
    {QDP_DC_ST232,   QDP_BT_232},
    {QDP_DC_MN38,    QDP_BT_38},
    {QDP_DC_MN816,   QDP_BT_816},
    {QDP_DC_MN32,    QDP_BT_32},
    {QDP_DC_MN232,   QDP_BT_232},
    {QDP_DC_AG38,    QDP_BT_38},
    {QDP_DC_AG816,   QDP_BT_816},
    {QDP_DC_AG32,    QDP_BT_32},
    {QDP_DC_AG232,   QDP_BT_232},
    {QDP_DC_CNP38,   QDP_BT_38},
    {QDP_DC_CNP816,  QDP_BT_816},
    {QDP_DC_CNP316,  QDP_BT_316},
    {QDP_DC_CNP232,  QDP_BT_232},
    {QDP_DC_D32,     QDP_BT_32},
    {QDP_DC_COMP,    QDP_BT_COMP},
    {QDP_DC_MULT,    QDP_BT_MULT},
    {QDP_DC_SPEC,    QDP_BT_SPEC},
    {0,              QDP_BT_UNKNOWN}
};

    for (i = 0; BlocketteIdentMap[i].format != QDP_BT_UNKNOWN; i++) {
        if (ident == BlocketteIdentMap[i].ident) return BlocketteIdentMap[i].format;
    }

    return QDP_BT_UNKNOWN;
}

/* Decode a single blockette and add it to the list */

static int DecodeBlockette(UINT8 *start, UINT32 seqno, LNKLST *list, BOOL *error)
{
int len;
QDP_DT_BLOCKETTE blk;
static char *fid = "qdpDecode_DT_DATA:DecodeBlockette";

    *error = FALSE;

    blk.seqno   = seqno;
    blk.channel = start[0];
    blk.ident = BlocketteIdent(blk.channel);
    blk.resid = BlocketteResid(blk.channel);
    blk.is_status = IsStatusBlockette(blk.channel);
    if ((blk.format = BlocketteFormat(blk.ident)) == QDP_BT_UNKNOWN) {
        *error = TRUE;
        errno = EINVAL;
        return 0;
    }

    blk.root = blk.channel;
    switch (blk.format) {
      case QDP_BT_38:
        len = Decode_BT_38(start, &blk, &blk.data.bt_38, error); break;
      case QDP_BT_816:
        len = Decode_BT_816(start, &blk, &blk.data.bt_816, error); break;
      case QDP_BT_316:
        len = Decode_BT_316(start, &blk, &blk.data.bt_316, error); break;
      case QDP_BT_32:
        len = Decode_BT_32(start, &blk, &blk.data.bt_32, error); break;
      case QDP_BT_232:
        len = Decode_BT_232(start, &blk, &blk.data.bt_232, error); break;
      case QDP_BT_COMP:
        blk.root = blk.channel & 0xe7;
        len = Decode_BT_COMP(start, &blk, &blk.data.bt_comp, error); break;
      case QDP_BT_MULT:
        blk.root = blk.channel & 0xe7;
        len = Decode_BT_MULT(start, &blk, &blk.data.bt_mult, error); break;
      case QDP_BT_SPEC:
        blk.root = 0;
        len = Decode_BT_SPEC(start, &blk, &blk.data.bt_spec, error); break;
      default:
        errno = EINVAL;
        *error = TRUE;
        break;
    }

    if (*error) return 0;

    if (!listAppend(list, &blk, sizeof(QDP_DT_BLOCKETTE))) {
        *error = TRUE;
        return 0;
    }

    return len;
}

/* Initialize an instance of a QDP_TYPE_DT_DATA structure.  We use a known
 * string in the signature field of this structure to determine if the linked
 * list field has been initialized or not.  This is to avoid memory leaks or
 * unpredictable behavior accessing the list.
 */

static void InitDtData(QDP_TYPE_DT_DATA *dt_data)
{
static char *fid = "qdpDecode_DT_DATA:InitDtData";

    if (strcmp(dt_data->signature, QDP_DT_DATA_SIGNATURE) == 0) {
        listClear(&dt_data->blist);
    } else {
        listInit(&dt_data->blist);
        sprintf(dt_data->signature, "%s", QDP_DT_DATA_SIGNATURE);
        dt_data->signature[QDP_DT_DATA_SIG_LEN] = 0;
    }

    dt_data->seqno = 0;
    dt_data->used = 0;
}

/* Load in the MN232 data */

void qdpLoadMN232(QDP_BLOCKETTE_232 *blk, QDP_MN232_DATA *dest)
{
    if (blk == NULL || dest == NULL) return;

    dest->seqno = blk->seqno;
    dest->sec   = blk->bit32[0];
    dest->usec  = blk->bit32[1];
    dest->qual  = blk->bit8;
    dest->loss  = blk->bit16;
}

/* Decode a QDP DT_DATA payload into a QDP_TYPE_DT_DATA structure */

BOOL qdpDecode_DT_DATA(UINT8 *start, UINT16 dlen, QDP_TYPE_DT_DATA *dest)
{
BOOL error;
UINT8 *ptr;
static char *fid = "qdpDecode_DT_DATA";

    if (start == NULL || dlen == 0 || dest == NULL) {
        errno = EINVAL;
        return FALSE;
    }

    InitDtData(dest);

    ptr = start;
    ptr += utilUnpackUINT32(ptr, &dest->seqno);
    do {
        ptr += DecodeBlockette(ptr, dest->seqno, &dest->blist, &error);
        if (error) return FALSE;
    } while((UINT16) (ptr - start) < dlen);

    dest->used = (int) (ptr - start);

    if (!listSetArrayView(&dest->blist)) return FALSE;

    return TRUE;
}

/* Scan through a DT_DATA payload looking for calibration transition blockette */

static int TestCalNotifyAG816(UINT8 *start, UINT8 *abort, UINT16 *bitmap, BOOL *match)
{
UINT8 *ptr, channel;

    *match = FALSE;

    ptr = start;
    channel = *ptr++;
    switch (BlocketteIdent(channel)) {
      case QDP_DC_AG816:
        if (BlocketteResid(channel)!= 0) return qdpLenBT_816(start);
        *match = TRUE;
        *abort = *ptr++;
        ptr += utilUnpackUINT16(ptr, bitmap);
        return (int) (ptr - start);
      case QDP_DC_ST38:    return qdpLenBT_38(start);
      case QDP_DC_ST816:   return qdpLenBT_816(start);
      case QDP_DC_ST32:    return qdpLenBT_32(start);
      case QDP_DC_ST232:   return qdpLenBT_232(start);
      case QDP_DC_MN38:    return qdpLenBT_38(start);
      case QDP_DC_MN816:   return qdpLenBT_816(start);
      case QDP_DC_MN32:    return qdpLenBT_32(start);
      case QDP_DC_MN232:   return qdpLenBT_232(start);
      case QDP_DC_AG38:    return qdpLenBT_38(start);
      case QDP_DC_AG32:    return qdpLenBT_32(start);
      case QDP_DC_AG232:   return qdpLenBT_232(start);
      case QDP_DC_CNP38:   return qdpLenBT_38(start);
      case QDP_DC_CNP816:  return qdpLenBT_816(start);
      case QDP_DC_CNP316:  return qdpLenBT_316(start);
      case QDP_DC_CNP232:  return qdpLenBT_232(start);
      case QDP_DC_D32:     return qdpLenBT_32(start);
      case QDP_DC_COMP:    return qdpLenBT_COMP(start);
      case QDP_DC_MULT:    return qdpLenBT_MULT(start);
      case QDP_DC_SPEC:    return qdpLenBT_SPEC(start);
    }
    errno = EINVAL;
    return -1;
}

BOOL qdpHaveCalNotifyBlockette(UINT8 *start, UINT16 dlen, UINT8 *abort, UINT16 *bitmap)
{
UINT8 *ptr;
BOOL match;

    if (start == NULL || dlen == 0 || abort == NULL || bitmap == NULL) {
        errno = EINVAL;
        return FALSE;
    }
    errno = 0;

    ptr = start;
    do {
        ptr += TestCalNotifyAG816(ptr, abort, bitmap, &match);
        if (errno != 0) return FALSE;
        if (match) return TRUE;
    } while((UINT16) (ptr - start) < dlen);

    return FALSE;
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
 * $Log: data.c,v $
 * Revision 1.13  2016/01/23 00:12:25  dechavez
 * added support for compressed QDP_SP_ENVDAT data
 *
 * Revision 1.12  2016/01/19 22:53:04  dechavez
 * environmental processor support (1-Hz streams only)
 *
 * Revision 1.11  2015/12/04 23:15:12  dechavez
 * casts, format fixes and the like, all to calm OS X compiles
 *
 * Revision 1.10  2010/03/31 20:38:42  dechavez
 * added conditional (ENABLE_DEBUG_BLOCKETTES) blockkette dumps
 *
 * Revision 1.9  2010/03/22 21:41:19  dechavez
 * added qdpLenBT_COMP(), qdpLenBT_SPEC(), qdpHaveCalNotifyBlockette()
 *
 * Revision 1.8  2009/10/29 17:39:52  dechavez
 * decode C1_FIX, C1_GLOB, and config SSCFG in BT_SPEC blockettes
 *
 * Revision 1.7  2009/10/20 22:42:10  dechavez
 * use QDP_CFG_x macros instead of numbers when ignoring (for now) the special packet config change blockettes
 *
 * Revision 1.6  2009/07/09 18:16:39  dechavez
 * added code to load contents of QDP_BLOCKETTE_SPEC
 *
 * Revision 1.5  2008/01/07 21:23:01  dechavez
 * little-endian bug fixes
 *
 * Revision 1.4  2007/10/31 23:11:48  dechavez
 * fixed benign type error in BlocketteFormat() BlocketteIdentMap[]
 *
 * Revision 1.3  2007/05/18 18:00:57  dechavez
 * initial production release
 *
 */
