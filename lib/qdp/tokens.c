#pragma ident "$Id: tokens.c,v 1.13 2018/01/09 01:08:11 dechavez Exp $"
/*======================================================================
 *
 * Decode DP tokens
 *
 *====================================================================*/
#include "qdp.h"

#define IsVariableLength(ident) ((ident & 0x80) != 0 ? TRUE : FALSE)
#define SizeOfLengthField(ident) (IsVariableLength(ident) ? (((ident & 0x40) == 0) ? 1 : 2) : 0)

/* Initialize an instance of a QDP_TYPE_DT_DATA structure.  We use a known
 * string in the signature field of this structure to determine if the linked
 * list field has been initialized or not.  This is to avoid memory leaks or
 * unpredictable behavior accessing the list.
 */

static void ClearScalerFields(QDP_DP_TOKEN *token)
{
    token->ver.valid = FALSE;
    token->site.valid = FALSE;
    token->dss.valid = FALSE;
    token->clock.valid = FALSE;
    token->logid.valid = FALSE;
    token->cnfid.valid = FALSE;
    token->srvr.net.valid = FALSE;
    token->srvr.web.valid = FALSE;
    token->srvr.data.valid = FALSE;
    memset(token->setname, 0, QDP_MAX_TOKENSET_NAME+1);
    token->noncomp = 0;
}

static void InitListFields(QDP_DP_TOKEN *token)
{
    listInit(&token->lcq);
    listInit(&token->iir);
    listInit(&token->fir);
    listInit(&token->cds);
    listInit(&token->det);
    listInit(&token->cen);
}

static void ClearListFields(QDP_DP_TOKEN *token)
{
    listClear(&token->lcq);
    listClear(&token->iir);
    listClear(&token->fir);
    listClear(&token->cds);
    listClear(&token->det);
    listClear(&token->cen);
}

void qdpInitIdent(QDP_TOKEN_IDENT *ident)
{
    if (ident == NULL) return;
    ident->code = QDP_UNDEFINED_ID;
    ident->name[0] = 0;
}

void qdpSetTokenIdent(QDP_TOKEN_IDENT *ident, UINT8 code, char *name)
{
    if (ident == NULL) return;
    ident->code = code;
    if (name == NULL) {
        ident->name[0] = 0;
    } else {
        strlcpy(ident->name, name, QDP_PASCALSTRING_LEN);
    }
}

void qdpInitTokenDet(QDP_TOKEN_DET *det)
{
    if (det == NULL) return;
    memset(det, 0, sizeof(QDP_TOKEN_DET));
    det->id = QDP_UNDEFINED_ID;
    det->type = QDP_UNDEFINED_ID;
}

void qdpInitLcq(QDP_TOKEN_LCQ *lcq)
{
    if (lcq == NULL) return;
    memset(lcq, 0, sizeof(QDP_TOKEN_LCQ));
    qdpInitIdent(&lcq->ident);
    qdpInitIdent(&lcq->cntrl);
    qdpInitIdent(&lcq->ave.filt);
    qdpInitIdent(&lcq->decim.src);
    qdpInitIdent(&lcq->decim.fir);
}

void qdpInitTokens(QDP_DP_TOKEN *token)
{
#define QDP_STRUCT_SIG "QDP_STRUCT_INIT"

    if (strcmp(token->signature, QDP_STRUCT_SIG) == 0) {
        ClearListFields(token);
    } else {
        InitListFields(token);
        sprintf(token->signature, "%s", QDP_STRUCT_SIG);
        token->signature[QDP_DP_TOKEN_SIG_LEN] = 0;
    }

    ClearScalerFields(token);
}

static int UnpackPascalString(UINT8 *start, char *dest)
{
UINT8 *ptr, nchar;

    ptr = start;
    nchar = *ptr++;
    ptr += utilUnpackBytes(ptr, (UINT8 *) dest, nchar);
    dest[nchar] = 0;

    return (int) (ptr - start);
}

static int UnpackLengthField(UINT8 *start, int *dest, int key)
{
UINT8 *ptr, clen;
UINT16 slen;

    ptr = start;
    if (SizeOfLengthField(key) == 1) {
        clen = *ptr++;
        *dest = (int) clen;
    } else {
        ptr += utilUnpackUINT16(ptr, &slen);
        *dest = (int) slen;
    }

    return (int) (ptr - start);
}

static int UnpackTokenVersion(UINT8 *start, QDP_TOKEN_VERSION *dest)
{
UINT8 *ptr;

    ptr = start;
    dest->version = *ptr++;

    dest->valid = TRUE;
    return (int) (ptr - start);
}

static int UnpackTokenSite(UINT8 *start, QDP_TOKEN_SITE *dest)
{
UINT8 *ptr;

    ptr = start;
    ptr += utilUnpackBytes(ptr, (UINT8 *) dest->nname, QDP_NNAME_LEN);
    dest->nname[QDP_NNAME_LEN] = 0;
    ptr += utilUnpackBytes(ptr, (UINT8 *) dest->sname, QDP_SNAME_LEN);
    dest->sname[QDP_SNAME_LEN] = 0;

    dest->valid = TRUE;
    return (int) (ptr - start);
}

static int UnpackTokenServer(UINT8 *start, QDP_TOKEN_SERVER *dest)
{
UINT8 *ptr;

    ptr = start;
    ptr += utilUnpackINT16(ptr, &dest->port);

    dest->valid = TRUE;
    return (int) (ptr - start);
}

static int UnpackTokenDss(UINT8 *start, QDP_TOKEN_DSS *dest)
{
UINT8 *ptr;

    ptr = start;

    ptr += utilUnpackBytes(ptr, (UINT8 *) dest->passwd.hi, QDP_DSS_PASSWD_LEN);
    dest->passwd.hi[QDP_DSS_PASSWD_LEN] = 0;
    ptr += utilUnpackBytes(ptr, (UINT8 *) dest->passwd.mi, QDP_DSS_PASSWD_LEN);
    dest->passwd.mi[QDP_DSS_PASSWD_LEN] = 0;
    ptr += utilUnpackBytes(ptr, (UINT8 *) dest->passwd.lo, QDP_DSS_PASSWD_LEN);
    dest->passwd.lo[QDP_DSS_PASSWD_LEN] = 0;
    ptr += utilUnpackUINT32(ptr, &dest->timeout);
    ptr += utilUnpackUINT32(ptr, &dest->maxbps);
    dest->verbosity = *ptr++;
    dest->maxcpu = *ptr++;
    ptr += utilUnpackUINT16(ptr, &dest->port);
    ptr += utilUnpackUINT16(ptr, &dest->maxmem);

    dest->valid = TRUE;
    return (int) (ptr - start);
}

static int UnpackTokenClock(UINT8 *start, QDP_TOKEN_CLOCK *dest)
{
UINT8 *ptr;

    ptr = start;

    ptr += utilUnpackINT32(ptr, &dest->offset);
    ptr += utilUnpackUINT16(ptr, &dest->maxlim);
    dest->pll.locked = *ptr++;
    dest->pll.track = *ptr++;
    dest->pll.hold = *ptr++;
    dest->pll.off = *ptr++;
    dest->pll.spare = *ptr++;
    dest->maxhbl = *ptr++;
    dest->minhbl = *ptr++;
    dest->nbl = *ptr++;
    ptr += utilUnpackUINT16(ptr, &dest->clkqflt);

    dest->valid = TRUE;
    return (int) (ptr - start);
}

static int UnpackTokenLogid(UINT8 *start, QDP_TOKEN_LOGID *dest)
{
UINT8 *ptr;

    ptr = start;

    ptr += utilUnpackBytes(ptr, (UINT8 *) dest->mesg.loc, QDP_LNAME_LEN);
    dest->mesg.loc[QDP_LNAME_LEN] = 0;
    ptr += utilUnpackBytes(ptr, (UINT8 *) dest->mesg.chn, QDP_CNAME_LEN);
    dest->mesg.chn[QDP_CNAME_LEN] = 0;
    ptr += utilUnpackBytes(ptr, (UINT8 *) dest->time.loc, QDP_LNAME_LEN);
    dest->time.loc[QDP_LNAME_LEN] = 0;
    ptr += utilUnpackBytes(ptr, (UINT8 *) dest->time.chn, QDP_CNAME_LEN);
    dest->time.chn[QDP_CNAME_LEN] = 0;

    dest->valid = TRUE;
    return (int) (ptr - start);
}

static int UnpackTokenCnfid(UINT8 *start, QDP_TOKEN_CNFID *dest)
{
UINT8 *ptr;

    ptr = start;

    ptr += utilUnpackBytes(ptr, (UINT8 *) dest->loc, QDP_LNAME_LEN);
    dest->loc[QDP_LNAME_LEN] = 0;
    ptr += utilUnpackBytes(ptr, (UINT8 *) dest->chn, QDP_CNAME_LEN);
    dest->chn[QDP_CNAME_LEN] = 0;
    dest->flags = *ptr++;
    ptr += utilUnpackUINT16(ptr, &dest->interval);

    dest->valid = TRUE;
    return (int) (ptr - start);
}

static int UnpackLcqAvepar(UINT8 *start, QDP_LCQ_AVEPAR *dest)
{
UINT8 *ptr;

    ptr = start;

    ptr += utilUnpackUINT32(ptr, &dest->len);
    dest->filt.code = *ptr++; dest->filt.name[0] = 0;

    return (int) (ptr - start);
}

static int UnpackLcqDecim(UINT8 *start, QDP_LCQ_DECIM *dest)
{
UINT8 *ptr;

    ptr = start;

    dest->src.code = *ptr++; dest->src.name[0] = 0;
    dest->fir.code = *ptr++; dest->fir.name[0] = 0;

    return (int) (ptr - start);
}

static int UnpackLcqDetect(UINT8 *start, QDP_LCQ_DETECT *dest)
{
UINT8 *ptr;

    ptr = start;

    dest->base.code = *ptr++; dest->base.name[0] = 0;
    dest->use = *ptr++;
    dest->options = *ptr++;
    dest->set = TRUE;

    return (int) (ptr - start);
}

static int UnpackTokenLcq(UINT8 *start, QDP_TOKEN_LCQ *dest, int key)
{
UINT8 i, *ptr;

    ptr = start;
    qdpInitLcq(dest);

    ptr += UnpackLengthField(ptr, &dest->len, key);
    ptr += utilUnpackBytes(ptr, (UINT8 *) dest->loc, QDP_LNAME_LEN);
    dest->loc[QDP_LNAME_LEN] = 0;
    ptr += utilUnpackBytes(ptr, (UINT8 *) dest->chn, QDP_CNAME_LEN);
    dest->chn[QDP_CNAME_LEN] = 0;
    dest->ident.code = *ptr++;
    qdpSetLcqIdentName(dest);
    dest->src[0] = *ptr++;
    dest->src[1] = *ptr++;
    ptr += utilUnpackUINT32(ptr, &dest->options);
    ptr += utilUnpackINT16(ptr, &dest->rate);

/* So that XML output will agree with Willard */

    dest->comfr = QDP_LCQ_DEFAULT_COMFR;
    dest->pebuf = 0;

/* optional fields */

    if (dest->options & QDP_LCQ_HAVE_PRE_EVENT_BUFFERS) ptr += utilUnpackUINT16(ptr, &dest->pebuf);
    if (dest->options & QDP_LCQ_HAVE_GAP_THRESHOLD)     ptr += utilUnpackREAL32(ptr, &dest->gapthresh);
    if (dest->options & QDP_LCQ_HAVE_CALIB_DELAY)       ptr += utilUnpackUINT16(ptr, &dest->caldly);
    if (dest->options & QDP_LCQ_HAVE_FRAME_COUNT)       dest->comfr = *ptr++;
    if (dest->options & QDP_LCQ_HAVE_FIR_MULTIPLIER)    ptr += utilUnpackREAL32(ptr, &dest->firfix);
    if (dest->options & QDP_LCQ_HAVE_AVEPAR)            ptr += UnpackLcqAvepar(ptr, &dest->ave);
    if (dest->options & QDP_LCQ_HAVE_CNTRL_DETECTOR)    dest->cntrl.code = *ptr++;
    if (dest->options & QDP_LCQ_HAVE_DECIM_ENTRY)       ptr += UnpackLcqDecim(ptr, &dest->decim);
    for (i = 0; i < QDP_LCQ_NUM_DETECT; i++) dest->detect[i].set = FALSE;
    if (dest->options & QDP_LCQ_HAVE_DETECTOR_1)        ptr += UnpackLcqDetect(ptr, &dest->detect[0]);
    if (dest->options & QDP_LCQ_HAVE_DETECTOR_2)        ptr += UnpackLcqDetect(ptr, &dest->detect[1]);
    if (dest->options & QDP_LCQ_HAVE_DETECTOR_3)        ptr += UnpackLcqDetect(ptr, &dest->detect[2]);
    if (dest->options & QDP_LCQ_HAVE_DETECTOR_4)        ptr += UnpackLcqDetect(ptr, &dest->detect[3]);
    if (dest->options & QDP_LCQ_HAVE_DETECTOR_5)        ptr += UnpackLcqDetect(ptr, &dest->detect[4]);
    if (dest->options & QDP_LCQ_HAVE_DETECTOR_6)        ptr += UnpackLcqDetect(ptr, &dest->detect[5]);
    if (dest->options & QDP_LCQ_HAVE_DETECTOR_7)        ptr += UnpackLcqDetect(ptr, &dest->detect[6]);
    if (dest->options & QDP_LCQ_HAVE_DETECTOR_8)        ptr += UnpackLcqDetect(ptr, &dest->detect[7]);

/* derived fields */

    qdpConvertSampleRate(dest->rate, &dest->frate, &dest->dsint, &dest->nsint);

    return dest->len; // (int) (ptr - start);
}

static int UnpackIIRDatum(UINT8 *start, QDP_IIR_DATA *dest)
{
UINT8 *ptr;

    ptr = start;

    ptr += utilUnpackREAL32(ptr, &dest->ratio);
    dest->npole = *ptr++;
    dest->type = dest->npole & 0x80 ? QDP_IIR_HIPASS : QDP_IIR_LOPASS;
    dest->npole &= 0x7f;

    return (int) (ptr - start);
}

static int UnpackTokenIIR(UINT8 *start, QDP_TOKEN_IIR *dest, int key)
{
UINT8 i, *ptr;

    ptr = start;

    ptr += UnpackLengthField(ptr, &dest->len, key);
    dest->id = *ptr++;
    ptr += UnpackPascalString(ptr, dest->name);
    dest->nsection = *ptr++;
    ptr += utilUnpackREAL32(ptr, &dest->gain);
    ptr += utilUnpackREAL32(ptr, &dest->refreq);
    for (i = 0; i < dest->nsection; i++) ptr += UnpackIIRDatum(ptr, &dest->data[i]);

    return dest->len; // (int) (ptr - start);
}

static int UnpackTokenFIR(UINT8 *start, QDP_TOKEN_FIR *dest, int key)
{
UINT8 *ptr;

    ptr = start;

    ptr += UnpackLengthField(ptr, &dest->len, key);
    dest->id = *ptr++;
    ptr += UnpackPascalString(ptr, dest->name);

    return dest->len; // (int) (ptr - start);
}

static int UnpackTokenCDS(UINT8 *start, QDP_TOKEN_CDS *dest, int key)
{
int i;
UINT8 *ptr;

    ptr = start;

    ptr += UnpackLengthField(ptr, &dest->len, key);
    dest->id = *ptr++;
    dest->options = *ptr++;
    ptr += UnpackPascalString(ptr, dest->name);
    dest->nentry = dest->len - (int) (ptr - start) - 1;
    for (i = 0; i < dest->nentry; i++) {
        dest->equation[i].code = *ptr++;
        dest->equation[i].name[0] = 0;
    }

    return dest->len; // (int) (ptr - start);
}

static int UnpackTokenDetector(UINT8 *start, QDP_TOKEN_DET *dest, int key)
{
UINT8 *ptr;

    ptr = start;

    ptr += UnpackLengthField(ptr, &dest->len, key);
    if ((dest->type = key) == QDP_TOKEN_TYPE_MHD) {
        dest->id = *ptr++;
        dest->detf.code = *ptr++;
        dest->iw = *ptr++;
        dest->nht = *ptr++;
        ptr += utilUnpackUINT32(ptr, &dest->fhi);
        ptr += utilUnpackUINT32(ptr, &dest->flo);
        ptr += utilUnpackUINT16(ptr, &dest->wa);
        ptr += 2; // skip over spare
        ptr += utilUnpackUINT16(ptr, &dest->tc);
        dest->x1 = *ptr++;
        dest->x2 = *ptr++;
        dest->x3 = *ptr++;
        dest->xx = *ptr++;
        dest->av = *ptr++;
        ptr += UnpackPascalString(ptr, dest->name);
    } else {
        dest->id = *ptr++;
        dest->detf.code = *ptr++;
        dest->iw = *ptr++;
        dest->nht = *ptr++;
        ptr += utilUnpackUINT32(ptr, &dest->fhi);
        ptr += utilUnpackUINT32(ptr, &dest->flo);
        ptr += utilUnpackUINT16(ptr, &dest->wa);
        ptr += 2; // skip over spare
        ptr += UnpackPascalString(ptr, dest->name);
    }
    dest->detf.name[0] = 0;

    return dest->len; // (int) (ptr - start);
}

static int UnpackTokenCenList(UINT8 *start, LNKLST *dest, int key)
{
int len;
UINT8 *ptr;
QDP_TOKEN_CEN cen;

    ptr = start;

    ptr += UnpackLengthField(ptr, &len, key);
    while (ptr - start < len) {
        cen.id = *ptr++;
        ptr += UnpackPascalString(ptr, cen.name);
        listAppend(dest, &cen, sizeof(QDP_TOKEN_CEN));
    }

    return len; // (int) (ptr - start);
}

static int UnpackTokenNoncomp(UINT8 *start, int *dest, int key)
{
int len;
UINT8 *ptr;

    ptr = start;

    ptr += UnpackLengthField(ptr, &len, key);
    *dest = 1;

    return len; // (int) (ptr - start);
}

static int SkipToken(UINT8 *start, int key)
{
int len;
UINT8 *ptr;

    ptr = start;
    ptr += UnpackLengthField(ptr, &len, key);
    return len;
}

BOOL qdpUnpackTokens(QDP_MEMBLK *blk, QDP_DP_TOKEN *token)
{
UINT8 *ptr, key;
QDP_TOKEN_LCQ lcq;
QDP_TOKEN_IIR iir;
QDP_TOKEN_FIR fir;
QDP_TOKEN_CDS cds;
QDP_TOKEN_DET det;
QDP_TOKEN_CEN cen;

    if (blk == NULL || token == NULL) {
        errno = EINVAL;
        return FALSE;
    }

    qdpInitTokens(token);

    ptr = blk->data;
    while (ptr < blk->data + blk->nbyte) {
        key = *ptr++;
        switch (key) {

          case QDP_TOKEN_TYPE_IGNORE:
            break;

          case QDP_TOKEN_TYPE_VER:
            ptr += UnpackTokenVersion(ptr, &token->ver);
            break;

          case QDP_TOKEN_TYPE_SITE:
            ptr += UnpackTokenSite(ptr, &token->site);
            break;

          case QDP_TOKEN_TYPE_NETSERVER:
            ptr += UnpackTokenServer(ptr, &token->srvr.net);
            break;

          case QDP_TOKEN_TYPE_DSS:
            ptr += UnpackTokenDss(ptr, &token->dss);
            break;

          case QDP_TOKEN_TYPE_WEBSERVER:
            ptr += UnpackTokenServer(ptr, &token->srvr.web);
            break;

          case QDP_TOKEN_TYPE_CLOCK:
            ptr += UnpackTokenClock(ptr, &token->clock);
            break;

          case QDP_TOKEN_TYPE_LOGID:
            ptr += UnpackTokenLogid(ptr, &token->logid);
            break;

          case QDP_TOKEN_TYPE_CNFID:
            ptr += UnpackTokenCnfid(ptr, &token->cnfid);
            break;

          case QDP_TOKEN_TYPE_DATSERVER:
            ptr += UnpackTokenServer(ptr, &token->srvr.data);
            break;

          case QDP_TOKEN_TYPE_LCQ:
            ptr += UnpackTokenLcq(ptr, &lcq, key);
            if (!listAppend(&token->lcq, &lcq, sizeof(QDP_TOKEN_LCQ))) return FALSE;
            break;

          case QDP_TOKEN_TYPE_IIR:
            ptr += UnpackTokenIIR(ptr, &iir, key);
            if (!listAppend(&token->iir, &iir, sizeof(QDP_TOKEN_IIR))) return FALSE;
            break;

          case QDP_TOKEN_TYPE_FIR:
            ptr += UnpackTokenFIR(ptr, &fir, key);
            if (!listAppend(&token->fir, &fir, sizeof(QDP_TOKEN_FIR))) return FALSE;
            break;

          case QDP_TOKEN_TYPE_CDS:
            ptr += UnpackTokenCDS(ptr, &cds, key);
            if (!listAppend(&token->cds, &cds, sizeof(QDP_TOKEN_CDS))) return FALSE;
            break;

          case QDP_TOKEN_TYPE_MHD:
          case QDP_TOKEN_TYPE_TDS:
            ptr += UnpackTokenDetector(ptr, &det, key);
            if (!listAppend(&token->det, &det, sizeof(QDP_TOKEN_DET))) return FALSE;
            break;

          case QDP_TOKEN_TYPE_CEN:
            ptr += UnpackTokenCenList(ptr, &token->cen, key);
            break;

          case QDP_TOKEN_TYPE_NONCOMP:
            ptr += UnpackTokenNoncomp(ptr, &token->noncomp, key);
            break;

          default:
            if (SizeOfLengthField(key) != 0) {
                ptr += SkipToken(ptr, key);
            } else {
                errno = EINVAL;
                return FALSE;
            }
        }

        ++token->count;
    }

    return qdpFinalizeToken(token);
}

static void DestroyListFields(QDP_DP_TOKEN *token)
{
    listDestroy(&token->lcq);
    listDestroy(&token->iir);
    listDestroy(&token->fir);
    listDestroy(&token->cds);
    listDestroy(&token->det);
    listDestroy(&token->cen);
}

void qdpDestroyTokens(QDP_DP_TOKEN *token)
{
    if (token == NULL) return;
    ClearScalerFields(token);
    DestroyListFields(token);
    memset(token->signature, 0, QDP_DP_TOKEN_SIG_LEN);
}

UINT8 qdpFirFilterCode(QDP_DP_TOKEN *token, char *name)
{
int i;
QDP_TOKEN_FIR *fir;
static UINT8 DefaultCode = QDP_UNDEFINED_ID;

    for (i = 0; i < token->fir.count; i++) {
        fir = (QDP_TOKEN_FIR *) token->fir.array[i];
        if (strcmp(fir->name, name) == 0) return fir->id;
    }

    return DefaultCode;
}

char *qdpFirFilterName(QDP_DP_TOKEN *token, int number)
{
int i;
QDP_TOKEN_FIR *fir;
static char *DefaultName = "";
static char *DEC10 = "DEC10";

    if (number == 0) return DEC10;

    for (i = 0; i < token->fir.count; i++) {
        fir = (QDP_TOKEN_FIR *) token->fir.array[i];
        if (fir->id == number) return fir->name;
    }

    return DefaultName;
}

static void CompleteFirIdent(QDP_DP_TOKEN *token, QDP_TOKEN_IDENT *ident)
{
int i;
QDP_TOKEN_FIR *fir;

    for (i = 0; i < token->fir.count; i++) {
        fir = (QDP_TOKEN_FIR *) token->fir.array[i];
        if (ident->code == QDP_UNDEFINED_ID) {
            ident->code = qdpFirFilterCode(token, ident->name);
            return;
        } else if (ident->name[0] == 0) {
            strlcpy(ident->name, qdpFirFilterName(token, ident->code), QDP_PASCALSTRING_LEN);
            return;
        }
    }
}

UINT8 qdpLcqCode(QDP_DP_TOKEN *token, char *name)
{
int i;
QDP_TOKEN_LCQ *lcq;
static UINT8 DefaultCode = QDP_UNDEFINED_ID;

    for (i = 0; i < token->lcq.count; i++) {
        lcq = (QDP_TOKEN_LCQ *) token->lcq.array[i];
        if (strcmp(lcq->ident.name, name) == 0) return lcq->ident.code;
    }

    return DefaultCode;
}

char *qdpLcqName(QDP_DP_TOKEN *token, int number)
{
int i;
QDP_TOKEN_LCQ *lcq;
static char *DefaultName = "";

    for (i = 0; i < token->lcq.count; i++) {
        lcq = (QDP_TOKEN_LCQ *) token->lcq.array[i];
        if (lcq->ident.code == number) return lcq->ident.name;
    }

    return DefaultName;
}

static void CompleteLcqIdent(QDP_DP_TOKEN *token, QDP_TOKEN_IDENT *ident)
{
int i;
QDP_TOKEN_LCQ *lcq;

    for (i = 0; i < token->lcq.count; i++) {
        lcq = (QDP_TOKEN_LCQ *) token->lcq.array[i];
        if (ident->code == QDP_UNDEFINED_ID) {
            ident->code = qdpLcqCode(token, ident->name);
            return;
        } else if (ident->name[0] == 0) {
            strlcpy(ident->name, qdpLcqName(token, ident->code), QDP_PASCALSTRING_LEN);
            return;
        }
    }
}

UINT8 qdpIirFilterCode(QDP_DP_TOKEN *token, char *name)
{
int i;
QDP_TOKEN_IIR *iir;
static UINT8 DefaultCode = QDP_UNDEFINED_ID;

    for (i = 0; i < token->iir.count; i++) {
        iir = (QDP_TOKEN_IIR *) token->iir.array[i];
        if (strcmp(iir->name, name) == 0) return iir->id;
    }

    return DefaultCode;
}

char *qdpIirFilterName(QDP_DP_TOKEN *token, int number)
{
int i;
QDP_TOKEN_IIR *iir;
static char *DefaultName = "";

    for (i = 0; i < token->iir.count; i++) {
        iir = (QDP_TOKEN_IIR *) token->iir.array[i];
        if (iir->id == number) return iir->name;
    }

    return DefaultName;
}

static void CompleteIirIdent(QDP_DP_TOKEN *token, QDP_TOKEN_IDENT *ident)
{
int i;
QDP_TOKEN_IIR *iir;

    for (i = 0; i < token->iir.count; i++) {
        iir = (QDP_TOKEN_IIR *) token->iir.array[i];
        if (ident->code == QDP_UNDEFINED_ID) {
            ident->code = qdpIirFilterCode(token, ident->name);
            return;
        } else if (ident->name[0] == 0) {
            strlcpy(ident->name, qdpIirFilterName(token, ident->code), QDP_PASCALSTRING_LEN);
            return;
        }
    }
}

UINT8 qdpControlDetectorCode(QDP_DP_TOKEN *token, char *name)
{
int i;
QDP_TOKEN_CDS *cds;
static UINT8 DefaultCode = QDP_UNDEFINED_ID;

    for (i = 0; i < token->cds.count; i++) {
        cds = (QDP_TOKEN_CDS *) token->cds.array[i];
        if (strcmp(cds->name, name) == 0) return cds->id;
    }

    return DefaultCode;
}

char *qdpControlDetectorName(QDP_DP_TOKEN *token, int number)
{
int i;
QDP_TOKEN_CDS *cds;
static char *DefaultName = "";

    for (i = 0; i < token->cds.count; i++) {
        cds = (QDP_TOKEN_CDS *) token->cds.array[i];
        if (cds->id == number) return cds->name;
    }

    return DefaultName;
}

UINT8 qdpEventDetectorCode(QDP_DP_TOKEN *token, char *name)
{
int i;
QDP_TOKEN_DET *det;
static UINT8 DefaultCode = QDP_UNDEFINED_ID;

    for (i = 0; i < token->det.count; i++) {
        det = (QDP_TOKEN_DET *) token->det.array[i];
        if (strcmp(det->name, name) == 0) return det->id;
    }

    return DefaultCode;
}

char *qdpEventDetectorName(QDP_DP_TOKEN *token, int number)
{
int i;
QDP_TOKEN_DET *det;
static char *DefaultName = "";

    for (i = 0; i < token->det.count; i++) {
        det = (QDP_TOKEN_DET *) token->det.array[i];
        if (det->id == number) return det->name;
    }

    return DefaultName;
}

UINT8 qdpCommEventCode(QDP_DP_TOKEN *token, char *name)
{
int i;
QDP_TOKEN_CEN *cen;
static UINT8 DefaultCode = QDP_UNDEFINED_ID;

    for (i = 0; i < token->cen.count; i++) {
        cen = (QDP_TOKEN_CEN *) token->cen.array[i];
        if (strcmp(cen->name, name) == 0) return cen->id;
    }

    return DefaultCode;
}

char *qdpCommEventName(QDP_DP_TOKEN *token, int number)
{
int i;
QDP_TOKEN_CEN *cen;
static char *DefaultName = "";

    for (i = 0; i < token->cen.count; i++) {
        cen = (QDP_TOKEN_CEN *) token->cen.array[i];
        if (cen->id == number) return cen->name;
    }

    return DefaultName;
}

UINT8 qdpControlDetectorFilterCode(QDP_DP_TOKEN *token, char *string)
{
    int i, j;
    UINT8 filter_token_ndx = 0;
    char *filter;
    QDP_TOKEN_LCQ *lcq;
    static UINT8 DefaultCode = QDP_UNDEFINED_ID;

    for (i = 0; i < token->lcq.count; i++) {
        lcq = (QDP_TOKEN_LCQ *) token->lcq.array[i];

        /* loop through all detectors of all lcq's in this token       */
        /* need to count the number of detectors in token because      */
        /* the index of the detector IN THE ENTIRE TOKEN is the 'code' */
        /* that gets or'd with the the equation det element code       */
        for (j = 0; j < QDP_LCQ_NUM_DETECT; j++) {

            if (lcq->detect[j].set) {

                if (strncmp(lcq->ident.name, string, strlen(lcq->ident.name)) == 0) { 

                    filter = string + strlen(lcq->ident.name) + 1;

                    if (strcmp(filter, lcq->detect[j].base.name) == 0) {

                        lcq->detect[j].use = filter_token_ndx;                        
                        return filter_token_ndx; /* lcq->detect[j].use; */

                    }

                }
                filter_token_ndx++;   /* ok, got index of this detector */
            }
        }
    }
    return DefaultCode;
}

char *qdpControlDetectorFilterName(QDP_DP_TOKEN *token, int number, char *buf)
{
int i, j;
QDP_TOKEN_LCQ *lcq;
static char *DefaultName = "";
static char Mt_Unsafe[QDP_LCQ_MAX_DETECTOR_STRING_LEN];

    if (buf == NULL) buf = Mt_Unsafe;

    for (i = 0; i < token->lcq.count; i++) {
        lcq = (QDP_TOKEN_LCQ *) token->lcq.array[i];
        for (j = 0; j < QDP_LCQ_NUM_DETECT; j++) {
            if (lcq->detect[j].set && lcq->detect[j].use == number) {
                sprintf(buf, "%s:%s", lcq->ident.name, lcq->detect[j].base.name);
                return buf;
            }
        }
    }

    return DefaultName;
}

static void CompleteEventDetectorIdent(QDP_DP_TOKEN *token, QDP_TOKEN_IDENT *ident)
{
int i;
QDP_TOKEN_DET *det;

    for (i = 0; i < token->det.count; i++) {
        det = (QDP_TOKEN_DET *) token->det.array[i];
        if (ident->code == QDP_UNDEFINED_ID) {
            ident->code = qdpEventDetectorCode(token, ident->name);
            return;
        } else if (ident->name[0] == 0) {
            strlcpy(ident->name, qdpEventDetectorName(token, ident->code), QDP_PASCALSTRING_LEN);
            return;
        }
    }
}

static void CompleteDetDetf(QDP_DP_TOKEN *token)
{
int i;
QDP_TOKEN_DET *det;

    if (token == NULL) return;
    for (i = 0; i < token->det.count; i++) {
        det = (QDP_TOKEN_DET *) token->det.array[i];
        CompleteIirIdent(token, &det->detf);
    }
}

static void CompleteDetect(QDP_DP_TOKEN *token, QDP_TOKEN_LCQ *lcq, int index)
{
static int mask[QDP_LCQ_NUM_DETECT] = {
    QDP_LCQ_HAVE_DETECTOR_1, QDP_LCQ_HAVE_DETECTOR_2, QDP_LCQ_HAVE_DETECTOR_3, QDP_LCQ_HAVE_DETECTOR_4,
    QDP_LCQ_HAVE_DETECTOR_5, QDP_LCQ_HAVE_DETECTOR_6, QDP_LCQ_HAVE_DETECTOR_7, QDP_LCQ_HAVE_DETECTOR_8
};

    if (!lcq->detect[index].set) return;

    lcq->options |= mask[index];
    CompleteEventDetectorIdent(token, &lcq->detect[index].base);
}

void qdpSetLcqIdentName(QDP_TOKEN_LCQ *lcq)
{
    if (lcq == NULL) return;
    if (utilIsBlankString(lcq->loc)) {
        strlcpy(lcq->ident.name, lcq->chn, QDP_CNAME_LEN);
    } else {
        sprintf(lcq->ident.name, "%2s-%s", lcq->loc, lcq->chn);
    }
}

static void CompleteLcq(QDP_DP_TOKEN *token)
{
int i, j;
QDP_TOKEN_LCQ *lcq;

    if (token == NULL) return;
    for (i = 0; i < token->lcq.count; i++) {
        lcq = (QDP_TOKEN_LCQ *) token->lcq.array[i];
        if (lcq->cntrl.code == QDP_UNDEFINED_ID) {
            lcq->cntrl.code = qdpControlDetectorCode(token, lcq->cntrl.name);
        } else if (lcq->cntrl.name[0] == 0) {
            strlcpy(lcq->cntrl.name, qdpControlDetectorName(token, lcq->cntrl.code), QDP_PASCALSTRING_LEN);
        }
        for (j = 0; j < QDP_LCQ_NUM_DETECT; j++) CompleteDetect(token, lcq, j);
        CompleteIirIdent(token, &lcq->ave.filt);
        CompleteFirIdent(token, &lcq->decim.fir);
    }

    for (i = 0; i < token->lcq.count; i++) {
        lcq = (QDP_TOKEN_LCQ *) token->lcq.array[i];
        CompleteLcqIdent(token, &lcq->decim.src);
    }
}

static void CompleteControlDetectorEquation(QDP_DP_TOKEN *token, QDP_TOKEN_CDS *cds)
{
int i;
QDP_TOKEN_IDENT *entry;
char *opstr, buf[QDP_PASCALSTRING_LEN+1];

    for (i = 0; i < cds->nentry; i++) {
        entry = &cds->equation[i];
        if (entry->name[0] == 0) {
            switch ((entry->code & 0xC0) >> 6) {
              case QDP_CDS_NIB_COMM:
                strlcpy(entry->name, qdpCommEventName(token, entry->code & 0x0F), QDP_PASCALSTRING_LEN);
                break;
              case QDP_CDS_NIB_DET:
                strlcpy(entry->name, qdpControlDetectorFilterName(token, entry->code & 0x1F, buf), QDP_PASCALSTRING_LEN);
                break;
              case QDP_CDS_NIB_CAL:
                strlcpy(entry->name, qdpLcqName(token, entry->code & 0x1F), QDP_PASCALSTRING_LEN);
                break;
              case QDP_CDS_NIB_OP:
                if ((opstr = qdpLogicalOperatorString(entry->code & 0x1F)) == NULL) {
                    sprintf(cds->equation[i].name, "op %d?", entry->code & 0x1F);
                } else {
                    strlcpy(entry->name, opstr, QDP_PASCALSTRING_LEN);
                }
            }
        } else {
            switch ((entry->code & 0xC0) >> 6) {
              case QDP_CDS_NIB_COMM:
                entry->code |= qdpCommEventCode(token, entry->name);
                break;
              case QDP_CDS_NIB_DET:
                entry->code |= qdpControlDetectorFilterCode(token, entry->name);
                break;
              case QDP_CDS_NIB_CAL:
                entry->code |= qdpLcqCode(token, entry->name);
                break;
              case QDP_CDS_NIB_OP:
                entry->code |= qdpLogicalOperatorCode(entry->name);
            }
        }
    }
}

static void CompleteControlDetector(QDP_DP_TOKEN *token)
{
int i;
QDP_TOKEN_CDS *cds;

    if (token == NULL) return;
    for (i = 0; i < token->cds.count; i++) {
        cds = (QDP_TOKEN_CDS *) token->cds.array[i];
        CompleteControlDetectorEquation(token, cds);
    }
}

BOOL qdpFinalizeToken(QDP_DP_TOKEN *token)
{
    if (token == NULL) {
        errno = EINVAL;
        return FALSE;
    }

    if (!listSetArrayView(&token->lcq)) return FALSE;
    if (!listSetArrayView(&token->iir)) return FALSE;
    if (!listSetArrayView(&token->fir)) return FALSE;
    if (!listSetArrayView(&token->cds)) return FALSE;
    if (!listSetArrayView(&token->det)) return FALSE;
    if (!listSetArrayView(&token->cen)) return FALSE;

    CompleteLcq(token);
    CompleteDetDetf(token);
    CompleteControlDetector(token);

    return TRUE;
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
 * $Log: tokens.c,v $
 * Revision 1.13  2018/01/09 01:08:11  dechavez
 * fixed bug setting lcq control detector
 *
 * Revision 1.12  2015/12/04 23:15:12  dechavez
 * casts, format fixes and the like, all to calm OS X compiles
 *
 * Revision 1.11  2012/08/02 17:55:44  dechavez
 * changed "new" to "det" in qdpInitTokenDet() to avoid name clashes
 *
 * Revision 1.10  2012/06/24 18:06:14  dechavez
 * UnpackTokenLcq() sets derived fields with qdpConvertSampleRate()
 *
 * Revision 1.9  2011/01/14 00:22:10  dechavez
 * changed name of qdpSetIdent() to qdpSetTokenIdent()
 *
 * Revision 1.8  2009/10/29 17:25:32  dechavez
 * added QDP_TOKEN_TYPE_NONCOMP support, use QDP_DSS_PASSWD_LEN instead of hardcoded number in decoding dss password fields
 *
 * Revision 1.7  2009/10/20 22:34:29  dechavez
 * qdpInitIdent(), qdpSetIdent(), qdpInitTokenDet(), qdpInitLcq(),
 * qdpInitTokens(), qdpFirFilterCode(), qdpLcqCode(), qdpLcqName(), qdpIirFilterCode(),
 * qdpControlDetectorCode(), qdpEventDetectorCode(), qdpCommEventCode(), qdpLcqName(),
 * qdpControlDetectorFilterName(), qdpSetLcqIdentName(), qdpFinalizeToken()
 *
 * Revision 1.6  2009/10/02 18:47:13  dechavez
 * added qdpControlDetectorString() and fixed comfr and pebuf defaults to agree with XML
 *
 * Revision 1.5  2009/09/28 17:36:09  dechavez
 * support for det, cen, added qdpLcqName(), qdpCommEventName(), qdpEventDetectorName(),
 * qdpControlDetectorName(), qdpIirFilterName(), qdpFirFilterName()
 *
 * Revision 1.4  2007/05/18 18:00:57  dechavez
 * initial production release
 *
 */
