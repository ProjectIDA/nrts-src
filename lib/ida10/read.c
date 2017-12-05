#pragma ident "$Id: read.c,v 1.15 2017/10/19 23:49:58 dauerbach Exp $"
/*======================================================================
 *
 *  Read a variable length record of any format rev
 *
 * - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 * Copyright (c) 1999 Regents of the University of California.
 * All rights reserved.
 *====================================================================*/
#include "ida10.h"

typedef struct {
    FILE *fp;
    gzFile *gz;
} FPGZ_HANDLE;

#define READ_FPGZ(fpgz, buf, want) ((fpgz->fp == NULL) ? gzread(fpgz->gz, buf, want) : fread(buf, 1, want, fpgz->fp))
#define FPGZ_EOF(fpgz) ((fpgz->fp == NULL) ? gzeof(fpgz->gz) : feof(fpgz->fp))

static int ReadIspLogRec(FPGZ_HANDLE *fpgz, UINT8 *buf, int buflen)
{
int ReadLen;

    if (buflen < IDA10_ISPLOGLEN) {
        errno = EINVAL;
        return IDA10_EINVAL;
    }
    ReadLen = IDA10_ISPLOGLEN - IDA10_PREAMBLE_LEN;
    if (READ_FPGZ(fpgz, buf, ReadLen) != (size_t) ReadLen) return IDA10_IOERR;
    return IDA10_PREAMBLE_LEN + IDA10_ISPLOGLEN;
}

static int ReadRecord(FPGZ_HANDLE *fpgz, UINT8 *buffer, int buflen, int *pType, BOOL TSheaders)
{
UINT8 *ptr;
IDA10_CMNHDR cmn;
int ReadLen, CmnHdrLen, nread = 0;

    if (buffer == NULL || pType == NULL) {
        errno = EINVAL;
        return IDA10_EINVAL;
    }

    if (buflen < IDA10_PREAMBLE_LEN) {
        errno = EMSGSIZE;
        return IDA10_EMSGSIZE;
    }

/* Read the preamble and determine the type */

    ptr = buffer;
    ReadLen = IDA10_PREAMBLE_LEN;
    if (READ_FPGZ(fpgz, ptr, ReadLen) != (size_t) ReadLen) return FPGZ_EOF(fpgz) ? IDA10_EOF : IDA10_IOERR;
    nread += ReadLen;

    if ((*pType = ida10Type(ptr)) == IDA10_TYPE_UNKNOWN) return IDA10_DATAERR;
    ptr += ReadLen;

/* Special handling for the ISP logs */

    if (*pType == IDA10_TYPE_ISPLOG) return ReadIspLogRec(fpgz, ptr, buflen);

/* Remaining records must follow IDA10 format rules */

    switch (ida10SubFormatCode(buffer)) {
      case IDA10_SUBFORMAT_0:
        CmnHdrLen = IDA10_SUBFORMAT_0_HDRLEN;
        break;
      case IDA10_SUBFORMAT_1:
        CmnHdrLen = IDA10_SUBFORMAT_1_HDRLEN;
        break;
      case IDA10_SUBFORMAT_2:
        CmnHdrLen = IDA10_SUBFORMAT_2_HDRLEN;
        break;
      case IDA10_SUBFORMAT_3:
        CmnHdrLen = IDA10_SUBFORMAT_3_HDRLEN;
        break;
      case IDA10_SUBFORMAT_4:
        CmnHdrLen = IDA10_SUBFORMAT_4_HDRLEN;
        break;
      case IDA10_SUBFORMAT_5:
        CmnHdrLen = IDA10_SUBFORMAT_5_HDRLEN;
        break;
      case IDA10_SUBFORMAT_6:
        CmnHdrLen = IDA10_SUBFORMAT_6_HDRLEN;
        break;
      case IDA10_SUBFORMAT_7:
        CmnHdrLen = IDA10_SUBFORMAT_7_HDRLEN;
        break;
      case IDA10_SUBFORMAT_8:
        CmnHdrLen = IDA10_SUBFORMAT_8_HDRLEN;
        break;
      case IDA10_SUBFORMAT_10:
        CmnHdrLen = IDA10_SUBFORMAT_10_HDRLEN;
        break;
      case IDA10_SUBFORMAT_11:
        CmnHdrLen = IDA10_SUBFORMAT_11_HDRLEN;
        break;
      case IDA10_SUBFORMAT_12:
        CmnHdrLen = IDA10_SUBFORMAT_12_HDRLEN;
        break;
      default:
        return IDA10_DATAERR;
    }

/* Read and decode common header */

    if (buflen < CmnHdrLen) {
        errno = EMSGSIZE;
        return IDA10_EMSGSIZE;
    }

    ReadLen = CmnHdrLen - IDA10_PREAMBLE_LEN;

    if (READ_FPGZ(fpgz, ptr, ReadLen) != (size_t) ReadLen) return IDA10_IOERR;
    ptr += ReadLen;
    nread += ReadLen;

    if (ida10UnpackCmnHdr(buffer, &cmn) == 0) return IDA10_DATAERR;

/* Read the rest of the record */

    ReadLen = (TSheaders) ? IDA10_TSHEADLEN - nread : cmn.nbytes;
    if (buflen < nread + ReadLen) {
        errno = EMSGSIZE;
        return IDA10_EMSGSIZE;
    }
    if (READ_FPGZ(fpgz, ptr, ReadLen) != (size_t) ReadLen) return IDA10_IOERR;
    nread += ReadLen;
    if (TSheaders) {
        ptr += ReadLen;
        memset(ptr, 0, ida10PacketLength(&cmn) - IDA10_TSHEADLEN);
        nread = ida10PacketLength(&cmn);
    }

    return nread;
}

int ida10ReadRecord(FILE *fp, UINT8 *buffer, int buflen, int *pType, BOOL TSheaders)
{
FPGZ_HANDLE fpgz;

    if (fp == NULL) {
        errno = EINVAL;
        return IDA10_EINVAL;
    }

    fpgz.fp = fp;
    fpgz.gz = NULL;

    return ReadRecord(&fpgz, buffer, buflen, pType, TSheaders);
}

int ida10ReadGz(gzFile *gz, UINT8 *buffer, int buflen, int *pType, BOOL TSheaders)
{
FPGZ_HANDLE fpgz;

    if (gz == NULL) {
        errno = EINVAL;
        return IDA10_EINVAL;
    }

    fpgz.fp = NULL;
    fpgz.gz = gz;
    return ReadRecord(&fpgz, buffer, buflen, pType, TSheaders);
}

/* Revision History
 *
 * $Log: read.c,v $
 * Revision 1.15  2017/10/19 23:49:58  dauerbach
 * added IDA10_SUBFORMAT_12 to ReadRecord()
 *
 * Revision 1.14  2013/09/20 16:15:02  dechavez
 * added IDA10_SUBFORMAT_11 to ReadRecord()
 *
 * Revision 1.13  2013/05/11 22:51:43  dechavez
 * IDA10.10 support
 *
 * Revision 1.12  2011/03/17 17:31:55  dechavez
 * fixed bug in ReadRecord() that assumed caller allocated space for entire packet,
 * even when only headers were being read (would cause unwarranted IDA10_EMSGSIZE errors)
 *
 * Revision 1.11  2011/02/25 19:40:11  dechavez
 * added TSheader option to ida10ReadGz() and ida10ReadRecord()
 *
 * Revision 1.10  2011/02/24 17:26:04  dechavez
 * added ida10ReadGz()
 *
 * Revision 1.9  2008/04/02 20:35:31  dechavez
 * added ida10.5 support
 *
 * Revision 1.8  2006/12/08 17:26:43  dechavez
 * 10.4 support
 *
 * Revision 1.7  2006/11/10 06:19:47  dechavez
 * removed some debug printfs
 *
 * Revision 1.6  2006/08/19 02:18:34  dechavez
 * removed trailing CVS junk from remarks
 *
 * Revision 1.5  2006/08/19 02:17:46  dechavez
 * further 10.3 support
 *
 * Revision 1.4  2005/08/26 18:41:44  dechavez
 * added 10.2 support
 *
 * Revision 1.3  2005/05/25 22:38:06  dechavez
 * mods to calm Visual C++ warnings
 *
 * Revision 1.2  2004/06/24 18:18:40  dechavez
 * removed unnecessary includes (aap)
 *
 * Revision 1.1  2002/03/15 22:41:20  dec
 * created
 *
 */
