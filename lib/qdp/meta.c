#pragma ident "$Id: meta.c,v 1.10 2017/01/30 23:40:31 dechavez Exp $"
/*======================================================================
 *
 * QDP meta data
 *
 *====================================================================*/
#include "qdp.h"

#define FailedRead(fp) (feof(fp) ? 0 : -1)

static int PackMemblk(UINT8 *start, QDP_MEMBLK *memblk)
{
int i;
UINT8 *ptr;
UINT16 len, nseg;

    ptr = start;

    ptr += utilPackUINT16(ptr, memblk->type);
    ptr += utilPackUINT16(ptr, memblk->nbyte);
    ptr += utilPackUINT16(ptr, memblk->remain);
    len = QDP_TOKEN_BUFLEN;
    ptr += utilPackUINT16(ptr, len);
    ptr += utilPackBytes(ptr, memblk->data, len);
    nseg = QDP_MAX_MEM_SEG;
    ptr += utilPackUINT16(ptr, nseg);
    for (i = 0; i < nseg; i++) ptr += utilPackUINT16(ptr, memblk->pending[i]);

    return (int) (ptr - start);
}

static int UnpackMemblk(UINT8 *start, QDP_MEMBLK *memblk)
{
int i;
UINT8 *ptr;
UINT16 len, nseg;

    ptr = start;

    ptr += utilUnpackUINT16(ptr, &memblk->type);
    ptr += utilUnpackINT16(ptr, &memblk->nbyte);
    ptr += utilUnpackINT16(ptr, &memblk->remain);
    ptr += utilUnpackUINT16(ptr, &len);
    if (len > QDP_TOKEN_BUFLEN) return -1;
    ptr += utilUnpackBytes(ptr, memblk->data, len);
    ptr += utilUnpackUINT16(ptr, &nseg);
    if (nseg > QDP_MAX_MEM_SEG) return -1;
    for (i = 0; i < nseg; i++) ptr += utilUnpackINT16(ptr, &memblk->pending[i]);
    memblk->full = (memblk->remain == 0) ? TRUE : FALSE;

    return (int) (ptr - start);
}

int qdpReadMeta(FILE *fp, QDP_META *meta)
{
UINT16 ident;
UINT32 len;
int nitems = 0;
UINT8 *buf;
static char *fid = "qdpReadMeta";

    if (fp == NULL || meta == NULL) {
        errno = EINVAL;
        return -1;
    }

    memset(meta, 0, sizeof(QDP_META));

    while (1) {

        if (!utilReadUINT16(fp, &ident)) return FailedRead(fp);
        if (ident == QDP_META_TAG_EOF || ident == QDP_META_OLDTAG_EOF) return nitems;

        if (!utilReadUINT32(fp, &len)) return FailedRead(fp);

        switch (ident) {
          case QDP_META_TAG_TOKEN:
          case QDP_META_OLDTAG_TOKEN:
            if ((buf = (UINT8 *) malloc(len)) == NULL) return -10;
            if (fread(buf, 1, len, fp) != len) return -11;
            if (UnpackMemblk(buf, &meta->raw.token) < 0) return -11;
            if (!qdpUnpackTokens(&meta->raw.token, &meta->token)) return -12;
            free(buf);
            meta->state |= QDP_META_STATE_HAVE_TOKEN;
            ++nitems;
            break;

          case QDP_META_TAG_COMBO:
          case QDP_META_OLDTAG_COMBO:
            if (len > QDP_MAX_PAYLOAD) {
                errno = ENOMEM;
                return -20;
            }
            if (fread(meta->raw.combo, 1, len, fp) != len) return -21;
            qdpDecode_C1_COMBO(meta->raw.combo, &meta->combo);
            meta->state |= QDP_META_STATE_HAVE_COMBO;
            ++nitems;
            break;

          case QDP_META_TAG_EPD:
            if (len > QDP_MAX_PAYLOAD) {
                errno = ENOMEM;
                return -22;
            }
            if (fread(meta->raw.epd, 1, len, fp) != len) return -23;
            qdpDecode_C2_EPD(meta->raw.epd, &meta->epd);
            meta->state |= QDP_META_STATE_HAVE_EPD;
            ++nitems;
            break;

          default:
            if (fseek(fp, len, SEEK_CUR) != 0) return -100;
        }
    }
}

int qdpReadMetaFile(char *path, QDP_META *meta)
{
FILE *fp;
int nitems;

    if ((fp = fopen(path, "rb")) == NULL) return -1;
    nitems = qdpReadMeta(fp, meta);
    fclose(fp);
    return nitems;
}

int qdpWriteMeta(FILE *fp, QDP_META *meta)
{
UINT32 len;
int nitems = 0;
#define QDP_MEMBLK_TMP_BUFLEN (sizeof(QDP_MEMBLK) * 2) /* leave plenty of extra space */
UINT8 buf[QDP_MEMBLK_TMP_BUFLEN];

    if (fp == NULL || meta == NULL) {
        errno = EINVAL;
        return -1;
    }

/* save token data */

    len = PackMemblk(buf, &meta->raw.token);
    if (!utilWriteUINT16(fp, QDP_META_TAG_TOKEN)) return -10;
    if (!utilWriteUINT32(fp, len)) return -11;
    if (fwrite(buf, 1, len, fp) != len) return -12;
    ++nitems;

/* save combo packet */

    len = QDP_MAX_PAYLOAD;
    if (!utilWriteUINT16(fp, QDP_META_TAG_COMBO)) return -20;
    if (!utilWriteUINT32(fp, len)) return -21;
    if (fwrite(meta->raw.combo, 1, len, fp) != len) return -22;
    ++nitems;

/* save ep delays */

    len = QDP_MAX_PAYLOAD;
    if (!utilWriteUINT16(fp, QDP_META_TAG_EPD)) return -23;
    if (!utilWriteUINT32(fp, len)) return -24;
    if (fwrite(meta->raw.epd, 1, len, fp) != len) return -25;
    ++nitems;

    if (!utilWriteUINT16(fp, QDP_META_TAG_EOF)) return -100;
    return nitems;
}

INT32 qdpLookupFilterDelay(QDP_META *meta, UINT8 *src)
{
int i, chan;

/* Environmental processor delays come from the QDP_TYPE_C2_EPD "epd" field of QDP_META */

    if (src[0] == QDP_EP_SRC0) {
        for (i = 0; i < meta->epd.chancnt; i++) {
            if (meta->epd.entry[i].channel == src[1]) return meta->epd.entry[i].usec;
        }
        return 0;
    }

/* Sensor A and B delays come from the QDP_TYPE_C1_COMBO "combo" field of QDP_META */

    if ((chan = qdpMainDigitizerChanNum(src[0])) < 0) return 0;
    return (chan <= 2) ? meta->combo.fix.ch13_delay[src[1]] : meta->combo.fix.ch46_delay[src[1]];
}

/* Root time tag, in nanoseconds */

UINT64 qdpRootTimeTag(QDP_MN232_DATA *mn232)
{
UINT64 tag;

    tag = ((UINT64) mn232->seqno + (UINT64) mn232->sec) * NANOSEC_PER_SEC;
    tag += (INT64) (mn232->usec * NANOSEC_PER_USEC);

    return tag;
}

/* Time tag corrected for filter delay */

UINT64 qdpDelayCorrectedTime(QDP_MN232_DATA *mn232, INT32 delay)
{
UINT64 tag;

    tag = qdpRootTimeTag(mn232) - (delay * NANOSEC_PER_USEC);
    return tag;
}

/*-----------------------------------------------------------------------r
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
 * $Log: meta.c,v $
 * Revision 1.10  2017/01/30 23:40:31  dechavez
 * support both QDP_META_TAG_x and QDP_META_OLDTAG_x tags in meta-data files
 *
 * Revision 1.9  2016/01/28 00:49:08  dechavez
 * fixed up handling of C2_EPD packets
 *
 * Revision 1.8  2016/01/27 00:22:09  dechavez
 * completed support for C2_EPD packets
 *
 * Revision 1.7  2016/01/23 00:16:35  dechavez
 * support for QDP_TYPE_C2_EPD added to QDP_META (untested)
 *
 * Revision 1.6  2015/12/04 23:15:12  dechavez
 * casts, format fixes and the like, all to calm OS X compiles
 *
 * Revision 1.5  2012/06/24 18:10:59  dechavez
 * use qdpMainDigitizerChanNum() to compute chan from src[0]
 *
 * Revision 1.4  2007/05/18 18:00:57  dechavez
 * initial production release
 *
 */
