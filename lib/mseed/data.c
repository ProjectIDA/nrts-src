#pragma ident "$Id: data.c,v 1.4 2014/10/29 21:25:41 dechavez Exp $"
/*======================================================================
 * 
 * Pack waveform data
 *
 * The API allows the caller to specify any output data format for any
 * input format, and code exists to support all possible combinations,
 * even though it is unlikely that several of them would ever be needed
 * IEEE_D_to_INT_16(), for example.  In practice I expect that only
 * packing in the same format as the input, or INT32 or INT16 to STEIM2
 * will be all that is ever used.
 *
 *====================================================================*/
#include "mseed.h"

/* INT16 output */

static BOOL INT_16_to_INT_16(MSEED_HANDLE *handle, MSEED_PACKED *packed, MSEED_RECORD *record, int *pnsamp, int *pnframe)
{
UINT8 *dest;
int i, maxcopy, ncopy;
static char *fid = "mseedPackData:INT_16_to_INT_16";

    dest = &packed->data[record->hdr.bod];
    maxcopy = (packed->len - record->hdr.bod) /sizeof(INT16);
    ncopy = (record->hdr.nsamp > maxcopy) ? maxcopy : record->hdr.nsamp;
    for (i = 0; i < ncopy; i++) dest += utilPackINT16(dest, record->dat.int16[i]);
    
    *pnsamp = ncopy;
    *pnframe = ncopy / MSEED_STEIM_FRAME_LEN;

    mseedLog(handle, 3, "%s: *pnsamp = %d, *pnframe = %d\n", fid, *pnsamp, *pnframe);
    return TRUE;
}

static BOOL INT_32_to_INT_16(MSEED_HANDLE *handle, MSEED_PACKED *packed, MSEED_RECORD *record, int *pnsamp, int *pnframe)
{
UINT8 *dest;
int i, maxcopy, ncopy;
static char *fid = "mseedPackData:INT_32_to_INT_16";

    dest = &packed->data[record->hdr.bod];
    maxcopy = (packed->len - record->hdr.bod)/sizeof(INT16);
    ncopy = (record->hdr.nsamp > maxcopy) ? maxcopy : record->hdr.nsamp;
    for (i = 0; i < ncopy; i++) dest += utilPackINT16(dest, (INT16) record->dat.int32[i]);
    
    *pnsamp = ncopy;
    *pnframe = ncopy / MSEED_STEIM_FRAME_LEN;

    mseedLog(handle, 3, "%s: *pnsamp = %d, *pnframe = %d\n", fid, *pnsamp, *pnframe);
    return TRUE;
}

static BOOL IEEE_F_to_INT_16(MSEED_HANDLE *handle, MSEED_PACKED *packed, MSEED_RECORD *record, int *pnsamp, int *pnframe)
{
UINT8 *dest;
int i, maxcopy, ncopy;
static char *fid = "mseedPackData:IEEE_F_to_INT_16";

    dest = &packed->data[record->hdr.bod];
    maxcopy = (packed->len - record->hdr.bod)/sizeof(INT16);
    ncopy = (record->hdr.nsamp > maxcopy) ? maxcopy : record->hdr.nsamp;
    for (i = 0; i < ncopy; i++) dest += utilPackINT16(dest, (INT16) record->dat.real32[i]);
    
    *pnsamp = ncopy;
    *pnframe = ncopy / MSEED_STEIM_FRAME_LEN;

    mseedLog(handle, 3, "%s: *pnsamp = %d, *pnframe = %d\n", fid, *pnsamp, *pnframe);
    return TRUE;
}

static BOOL IEEE_D_to_INT_16(MSEED_HANDLE *handle, MSEED_PACKED *packed, MSEED_RECORD *record, int *pnsamp, int *pnframe)
{
UINT8 *dest;
int i, maxcopy, ncopy;
static char *fid = "mseedPackData:IEEE_D_to_INT_16";

    dest = &packed->data[record->hdr.bod];
    maxcopy = (packed->len - record->hdr.bod)/sizeof(INT16);
    ncopy = (record->hdr.nsamp > maxcopy) ? maxcopy : record->hdr.nsamp;
    for (i = 0; i < ncopy; i++) dest += utilPackINT16(dest, (INT16) record->dat.real64[i]);
    
    *pnsamp = ncopy;
    *pnframe = ncopy / MSEED_STEIM_FRAME_LEN;

    mseedLog(handle, 3, "%s: *pnsamp = %d, *pnframe = %d\n", fid, *pnsamp, *pnframe);
    return TRUE;
}

/* INT32 output */

static BOOL INT_16_to_INT_32(MSEED_HANDLE *handle, MSEED_PACKED *packed, MSEED_RECORD *record, int *pnsamp, int *pnframe)
{
UINT8 *dest;
int i, maxcopy, ncopy;
static char *fid = "mseedPackData:INT_16_to_INT_32";

    dest = &packed->data[record->hdr.bod];
    maxcopy = (packed->len - record->hdr.bod)/sizeof(INT32);
    ncopy = (record->hdr.nsamp > maxcopy) ? maxcopy : record->hdr.nsamp;
    for (i = 0; i < ncopy; i++) dest += utilPackINT32(dest, (INT32) record->dat.int16[i]);
    
    *pnsamp = ncopy;
    *pnframe = ncopy / MSEED_STEIM_FRAME_LEN;

    mseedLog(handle, 3, "%s: *pnsamp = %d, *pnframe = %d\n", fid, *pnsamp, *pnframe);
    return TRUE;
}

static BOOL INT_32_to_INT_32(MSEED_HANDLE *handle, MSEED_PACKED *packed, MSEED_RECORD *record, int *pnsamp, int *pnframe)
{
UINT8 *dest;
int i, maxcopy, ncopy;
static char *fid = "mseedPackData:INT_32_to_INT_32";

    dest = &packed->data[record->hdr.bod];
    maxcopy = (packed->len - record->hdr.bod)/sizeof(INT32);
    ncopy = (record->hdr.nsamp > maxcopy) ? maxcopy : record->hdr.nsamp;
    for (i = 0; i < ncopy; i++) dest += utilPackINT32(dest, (INT32) record->dat.int32[i]);
    
    *pnsamp = ncopy;
    *pnframe = ncopy / MSEED_STEIM_FRAME_LEN;

    mseedLog(handle, 3, "%s: *pnsamp = %d, *pnframe = %d\n", fid, *pnsamp, *pnframe);
    return TRUE;
}

static BOOL IEEE_F_to_INT_32(MSEED_HANDLE *handle, MSEED_PACKED *packed, MSEED_RECORD *record, int *pnsamp, int *pnframe)
{
UINT8 *dest;
int i, maxcopy, ncopy;
static char *fid = "mseedPackData:IEEE_F_to_INT_32";

    dest = &packed->data[record->hdr.bod];
    maxcopy = (packed->len - record->hdr.bod)/sizeof(INT32);
    ncopy = (record->hdr.nsamp > maxcopy) ? maxcopy : record->hdr.nsamp;
    for (i = 0; i < ncopy; i++) dest += utilPackINT32(dest, (INT32) record->dat.real32[i]);
    
    *pnsamp = ncopy;
    *pnframe = ncopy / MSEED_STEIM_FRAME_LEN;

    mseedLog(handle, 3, "%s: *pnsamp = %d, *pnframe = %d\n", fid, *pnsamp, *pnframe);
    return TRUE;
}

static BOOL IEEE_D_to_INT_32(MSEED_HANDLE *handle, MSEED_PACKED *packed, MSEED_RECORD *record, int *pnsamp, int *pnframe)
{
UINT8 *dest;
int i, maxcopy, ncopy;
static char *fid = "mseedPackData:IEEE_D_to_INT_32";

    dest = &packed->data[record->hdr.bod];
    maxcopy = (packed->len - record->hdr.bod)/sizeof(INT32);
    ncopy = (record->hdr.nsamp > maxcopy) ? maxcopy : record->hdr.nsamp;
    for (i = 0; i < ncopy; i++) dest += utilPackINT32(dest, (INT32) record->dat.real64[i]);
    
    *pnsamp = ncopy;
    *pnframe = ncopy / MSEED_STEIM_FRAME_LEN;

    mseedLog(handle, 3, "%s: *pnsamp = %d, *pnframe = %d\n", fid, *pnsamp, *pnframe);
    return TRUE;
}

/* REAL32 output */

static BOOL INT_16_to_IEEE_F(MSEED_HANDLE *handle, MSEED_PACKED *packed, MSEED_RECORD *record, int *pnsamp, int *pnframe)
{
UINT8 *dest;
int i, maxcopy, ncopy;
static char *fid = "mseedPackData:INT_16_to_IEEE_F";

    dest = &packed->data[record->hdr.bod];
    maxcopy = (packed->len - record->hdr.bod)/sizeof(REAL32);
    ncopy = (record->hdr.nsamp > maxcopy) ? maxcopy : record->hdr.nsamp;
    for (i = 0; i < ncopy; i++) dest += utilPackREAL32(dest, (REAL32) record->dat.int16[i]);
    
    *pnsamp = ncopy;
    *pnframe = ncopy / MSEED_STEIM_FRAME_LEN;

    mseedLog(handle, 3, "%s: *pnsamp = %d, *pnframe = %d\n", fid, *pnsamp, *pnframe);
    return TRUE;
}

static BOOL INT_32_to_IEEE_F(MSEED_HANDLE *handle, MSEED_PACKED *packed, MSEED_RECORD *record, int *pnsamp, int *pnframe)
{
UINT8 *dest;
int i, maxcopy, ncopy;
static char *fid = "mseedPackData:INT_32_to_IEEE_F";

    dest = &packed->data[record->hdr.bod];
    maxcopy = (packed->len - record->hdr.bod)/sizeof(REAL32);
    ncopy = (record->hdr.nsamp > maxcopy) ? maxcopy : record->hdr.nsamp;
    for (i = 0; i < ncopy; i++) dest += utilPackREAL32(dest, (REAL32) record->dat.int32[i]);
    
    *pnsamp = ncopy;
    *pnframe = ncopy / MSEED_STEIM_FRAME_LEN;

    mseedLog(handle, 3, "%s: *pnsamp = %d, *pnframe = %d\n", fid, *pnsamp, *pnframe);
    return TRUE;
}

static BOOL IEEE_F_to_IEEE_F(MSEED_HANDLE *handle, MSEED_PACKED *packed, MSEED_RECORD *record, int *pnsamp, int *pnframe)
{
UINT8 *dest;
int i, maxcopy, ncopy;
static char *fid = "mseedPackData:IEEE_F_to_IEEE_F";

    dest = &packed->data[record->hdr.bod];
    maxcopy = (packed->len - record->hdr.bod)/sizeof(REAL32);
    ncopy = (record->hdr.nsamp > maxcopy) ? maxcopy : record->hdr.nsamp;
    for (i = 0; i < ncopy; i++) dest += utilPackREAL32(dest, (REAL32) record->dat.real32[i]);
    
    *pnsamp = ncopy;
    *pnframe = ncopy / MSEED_STEIM_FRAME_LEN;

    mseedLog(handle, 3, "%s: *pnsamp = %d, *pnframe = %d\n", fid, *pnsamp, *pnframe);
    return TRUE;
}

static BOOL IEEE_D_to_IEEE_F(MSEED_HANDLE *handle, MSEED_PACKED *packed, MSEED_RECORD *record, int *pnsamp, int *pnframe)
{
UINT8 *dest;
int i, maxcopy, ncopy;
static char *fid = "mseedPackData:IEEE_D_to_IEEE_F";

    dest = &packed->data[record->hdr.bod];
    maxcopy = (packed->len - record->hdr.bod)/sizeof(REAL32);
    ncopy = (record->hdr.nsamp > maxcopy) ? maxcopy : record->hdr.nsamp;
    for (i = 0; i < ncopy; i++) dest += utilPackREAL32(dest, (REAL32) record->dat.real64[i]);
    
    *pnsamp = ncopy;
    *pnframe = ncopy / MSEED_STEIM_FRAME_LEN;

    mseedLog(handle, 3, "%s: *pnsamp = %d, *pnframe = %d\n", fid, *pnsamp, *pnframe);
    return TRUE;
}

/* REAL64 output */

static BOOL INT_16_to_IEEE_D(MSEED_HANDLE *handle, MSEED_PACKED *packed, MSEED_RECORD *record, int *pnsamp, int *pnframe)
{
UINT8 *dest;
int i, maxcopy, ncopy;
static char *fid = "mseedPackData:INT_16_to_IEEE_D";

    dest = &packed->data[record->hdr.bod];
    maxcopy = (packed->len - record->hdr.bod)/sizeof(REAL64);
    ncopy = (record->hdr.nsamp > maxcopy) ? maxcopy : record->hdr.nsamp;
    for (i = 0; i < ncopy; i++) dest += utilPackREAL64(dest, (REAL64) record->dat.int16[i]);
    
    *pnsamp = ncopy;
    *pnframe = ncopy / MSEED_STEIM_FRAME_LEN;

    mseedLog(handle, 3, "%s: *pnsamp = %d, *pnframe = %d\n", fid, *pnsamp, *pnframe);
    return TRUE;
}

static BOOL INT_32_to_IEEE_D(MSEED_HANDLE *handle, MSEED_PACKED *packed, MSEED_RECORD *record, int *pnsamp, int *pnframe)
{
UINT8 *dest;
int i, maxcopy, ncopy;
static char *fid = "mseedPackData:INT_32_to_IEEE_D";

    dest = &packed->data[record->hdr.bod];
    maxcopy = (packed->len - record->hdr.bod)/sizeof(REAL64);
    ncopy = (record->hdr.nsamp > maxcopy) ? maxcopy : record->hdr.nsamp;
    for (i = 0; i < ncopy; i++) dest += utilPackREAL64(dest, (REAL64) record->dat.int32[i]);
    
    *pnsamp = ncopy;
    *pnframe = ncopy / MSEED_STEIM_FRAME_LEN;

    mseedLog(handle, 3, "%s: *pnsamp = %d, *pnframe = %d\n", fid, *pnsamp, *pnframe);
    return TRUE;
}

static BOOL IEEE_F_to_IEEE_D(MSEED_HANDLE *handle, MSEED_PACKED *packed, MSEED_RECORD *record, int *pnsamp, int *pnframe)
{
UINT8 *dest;
int i, maxcopy, ncopy;
static char *fid = "mseedPackData:IEEE_F_to_IEEE_D";

    dest = &packed->data[record->hdr.bod];
    maxcopy = (packed->len - record->hdr.bod)/sizeof(REAL64);
    ncopy = (record->hdr.nsamp > maxcopy) ? maxcopy : record->hdr.nsamp;
    for (i = 0; i < ncopy; i++) dest += utilPackREAL64(dest, (REAL64) record->dat.real32[i]);
    
    *pnsamp = ncopy;
    *pnframe = ncopy / MSEED_STEIM_FRAME_LEN;

    mseedLog(handle, 3, "%s: *pnsamp = %d, *pnframe = %d\n", fid, *pnsamp, *pnframe);
    return TRUE;
}

static BOOL IEEE_D_to_IEEE_D(MSEED_HANDLE *handle, MSEED_PACKED *packed, MSEED_RECORD *record, int *pnsamp, int *pnframe)
{
UINT8 *dest;
int i, maxcopy, ncopy;
static char *fid = "mseedPackData:IEEE_D_to_IEEE_D";

    dest = &packed->data[record->hdr.bod];
    maxcopy = (packed->len - record->hdr.bod)/sizeof(REAL64);
    ncopy = (record->hdr.nsamp > maxcopy) ? maxcopy : record->hdr.nsamp;
    for (i = 0; i < ncopy; i++) dest += utilPackREAL64(dest, (REAL64) record->dat.real64[i]);
    
    *pnsamp = ncopy;
    *pnframe = ncopy / MSEED_STEIM_FRAME_LEN;

    mseedLog(handle, 3, "%s: *pnsamp = %d, *pnframe = %d\n", fid, *pnsamp, *pnframe);
    return TRUE;
}

/* STEIM1 compression */

static BOOL INT_16_to_STEIM1(MSEED_HANDLE *handle, MSEED_PACKED *packed, MSEED_RECORD *record, int *pnsamp, int *pnframe)
{
UINT8 *dest;
INT32 d0, input[MSEED_MAX_BUFLEN];
MSEED_DATUM *datum;
int i, nf, pad=1, nframes, nsamples, retval, nbytes;
static char *fid = "mseedPackData:INT_16_to_STEIM1";

    if ((datum = mseedGetHistory(handle, record, NULL)) == NULL) {
        mseedLog(handle, -1, "%s: mseedGetHistory\n", fid);
        return FALSE;
    }
    dest  = &packed->data[record->hdr.bod];
    for (i = 0; i < record->hdr.nsamp; i++) input[i] = (INT32) record->dat.int16[i];
    d0 = input[0] - datum->value;
    nf = (packed->len - record->hdr.bod) / MSEED_STEIM_FRAME_LEN;

    if ((retval = mseedPackSteim1(dest, input, d0, record->hdr.nsamp, nf, pad, pnframe, pnsamp)) < 0) return FALSE;
    datum->value = input[*pnsamp - 1];

    mseedLog(handle, 3, "%s: *pnsamp = %d, *pnframe = %d\n", fid, *pnsamp, *pnframe);
    return TRUE;
}

static BOOL INT_32_to_STEIM1(MSEED_HANDLE *handle, MSEED_PACKED *packed, MSEED_RECORD *record, int *pnsamp, int *pnframe)
{
UINT8 *dest;
INT32 d0,  *input;
MSEED_DATUM *datum;
int nf, pad=1, nframes, nsamples, retval, nbytes;
static char *fid = "mseedPackData:INT_32_to_STEIM1";

    if ((datum = mseedGetHistory(handle, record, NULL)) == NULL) {
        mseedLog(handle, -1, "%s: mseedGetHistory\n", fid);
        return FALSE;
    }
    dest  = &packed->data[record->hdr.bod];
    input = record->dat.int32;
    d0 = input[0] - datum->value;
    nf = (packed->len - record->hdr.bod) / MSEED_STEIM_FRAME_LEN;

    if ((retval = mseedPackSteim1(dest, input, d0, record->hdr.nsamp, nf, pad, pnframe, pnsamp)) < 0) return FALSE;
    datum->value = input[*pnsamp - 1];

    mseedLog(handle, 3, "%s: *pnsamp = %d, *pnframe = %d\n", fid, *pnsamp, *pnframe);
    return TRUE;
}

static BOOL IEEE_F_to_STEIM1(MSEED_HANDLE *handle, MSEED_PACKED *packed, MSEED_RECORD *record, int *pnsamp, int *pnframe)
{
UINT8 *dest;
INT32 d0, input[MSEED_MAX_BUFLEN];
MSEED_DATUM *datum;
int i, nf, pad=1, nframes, nsamples, retval, nbytes;
static char *fid = "mseedPackData:IEEE_F_to_STEIM1";

    if ((datum = mseedGetHistory(handle, record, NULL)) == NULL) {
        mseedLog(handle, -1, "%s: mseedGetHistory\n", fid);
        return FALSE;
    }
    dest  = &packed->data[record->hdr.bod];
    for (i = 0; i < record->hdr.nsamp; i++) input[i] = (INT32) record->dat.real32[i];
    d0 = input[0] - datum->value;
    nf = (packed->len - record->hdr.bod) / MSEED_STEIM_FRAME_LEN;

    if ((retval = mseedPackSteim1(dest, input, d0, record->hdr.nsamp, nf, pad, pnframe, pnsamp)) < 0) return FALSE;
    datum->value = input[*pnsamp - 1];

    mseedLog(handle, 3, "%s: *pnsamp = %d, *pnframe = %d\n", fid, *pnsamp, *pnframe);
    return TRUE;
}

static BOOL IEEE_D_to_STEIM1(MSEED_HANDLE *handle, MSEED_PACKED *packed, MSEED_RECORD *record, int *pnsamp, int *pnframe)
{
UINT8 *dest;
INT32 d0, input[MSEED_MAX_BUFLEN];
MSEED_DATUM *datum;
int i, nf, pad=1, nframes, nsamples, retval, nbytes;
static char *fid = "mseedPackData:IEEE_D_to_STEIM1";

    if ((datum = mseedGetHistory(handle, record, NULL)) == NULL) {
        mseedLog(handle, -1, "%s: mseedGetHistory\n", fid);
        return FALSE;
    }
    dest  = &packed->data[record->hdr.bod];
    for (i = 0; i < record->hdr.nsamp; i++) input[i] = (INT32) record->dat.real64[i];
    d0 = input[0] - datum->value;
    nf = (packed->len - record->hdr.bod) / MSEED_STEIM_FRAME_LEN;

    if ((retval = mseedPackSteim1(dest, input, d0, record->hdr.nsamp, nf, pad, pnframe, pnsamp)) < 0) return FALSE;
    datum->value = input[*pnsamp - 1];

    mseedLog(handle, 3, "%s: *pnsamp = %d, *pnframe = %d\n", fid, *pnsamp, *pnframe);
    return TRUE;
}

/* STEIM2 compression */

static BOOL INT_16_to_STEIM2(MSEED_HANDLE *handle, MSEED_PACKED *packed, MSEED_RECORD *record, int *pnsamp, int *pnframe)
{
UINT8 *dest;
INT32 d0, input[MSEED_MAX_BUFLEN];
MSEED_DATUM *datum;
int i, nf, pad=1, nframes, nsamples, retval, nbytes;
static char *fid = "mseedPackData:INT_16_to_STEIM2";

    if ((datum = mseedGetHistory(handle, record, NULL)) == NULL) {
        mseedLog(handle, -1, "%s: mseedGetHistory\n", fid);
        return FALSE;
    }
    dest  = &packed->data[record->hdr.bod];
    for (i = 0; i < record->hdr.nsamp; i++) input[i] = (INT32) record->dat.int16[i];
    d0 = input[0] - datum->value;
    nf = (packed->len - record->hdr.bod) / MSEED_STEIM_FRAME_LEN;

    if ((retval = mseedPackSteim2(dest, input, d0, record->hdr.nsamp, nf, pad, pnframe, pnsamp)) < 0) return FALSE;
    datum->value = input[*pnsamp - 1];

    mseedLog(handle, 3, "%s: *pnsamp = %d, *pnframe = %d\n", fid, *pnsamp, *pnframe);
    return TRUE;
}

static BOOL INT_32_to_STEIM2(MSEED_HANDLE *handle, MSEED_PACKED *packed, MSEED_RECORD *record, int *pnsamp, int *pnframe)
{
UINT8 *dest;
INT32 d0,  *input;
MSEED_DATUM *datum;
int nf, pad=1, nframes, nsamples, retval, nbytes;
static char *fid = "mseedPackData:INT_32_to_STEIM2";

    if ((datum = mseedGetHistory(handle, record, NULL)) == NULL) {
        mseedLog(handle, -1, "%s: mseedGetHistory\n", fid);
        return FALSE;
    }
    dest  = &packed->data[record->hdr.bod];
    input = record->dat.int32;
    d0 = input[0] - datum->value;
    nf = (packed->len - record->hdr.bod) / MSEED_STEIM_FRAME_LEN;

    if ((retval = mseedPackSteim2(dest, input, d0, record->hdr.nsamp, nf, pad, pnframe, pnsamp)) < 0) {
        mseedLog(handle, 0, "%s: warning: mseedPackSteim2 failure for %s_%s_%s_%s, nsamp = %d\n", fid, record->hdr.netid, record->hdr.staid, record->hdr.chnid, record->hdr.locid, record->hdr.nsamp);
        return FALSE;
    }
    datum->value = input[*pnsamp - 1];

    mseedLog(handle, 3, "%s: *pnsamp = %d, *pnframe = %d\n", fid, *pnsamp, *pnframe);
    return TRUE;
}

static BOOL IEEE_F_to_STEIM2(MSEED_HANDLE *handle, MSEED_PACKED *packed, MSEED_RECORD *record, int *pnsamp, int *pnframe)
{
UINT8 *dest;
INT32 d0, input[MSEED_MAX_BUFLEN];
MSEED_DATUM *datum;
int i, nf, pad=1, nframes, nsamples, retval, nbytes;
static char *fid = "mseedPackData:IEEE_F_to_STEIM2";

    if ((datum = mseedGetHistory(handle, record, NULL)) == NULL) {
        mseedLog(handle, -1, "%s: mseedGetHistory\n", fid);
        return FALSE;
    }
    dest  = &packed->data[record->hdr.bod];
    for (i = 0; i < record->hdr.nsamp; i++) input[i] = (INT32) record->dat.real32[i];
    d0 = input[0] - datum->value;
    nf = (packed->len - record->hdr.bod) / MSEED_STEIM_FRAME_LEN;

    if ((retval = mseedPackSteim2(dest, input, d0, record->hdr.nsamp, nf, pad, pnframe, pnsamp)) < 0) return FALSE;
    datum->value = input[*pnsamp - 1];

    mseedLog(handle, 3, "%s: *pnsamp = %d, *pnframe = %d\n", fid, *pnsamp, *pnframe);
    return TRUE;
}

static BOOL IEEE_D_to_STEIM2(MSEED_HANDLE *handle, MSEED_PACKED *packed, MSEED_RECORD *record, int *pnsamp, int *pnframe)
{
UINT8 *dest;
INT32 d0, input[MSEED_MAX_BUFLEN];
MSEED_DATUM *datum;
int i, nf, pad=1, nframes, nsamples, retval, nbytes;
static char *fid = "mseedPackData:IEEE_D_to_STEIM2";

    if ((datum = mseedGetHistory(handle, record, NULL)) == NULL) {
        mseedLog(handle, -1, "%s: mseedGetHistory\n", fid);
        return FALSE;
    }
    dest  = &packed->data[record->hdr.bod];
    for (i = 0; i < record->hdr.nsamp; i++) input[i] = (INT32) record->dat.real64[i];
    d0 = input[0] - datum->value;
    nf = (packed->len - record->hdr.bod) / MSEED_STEIM_FRAME_LEN;

    if ((retval = mseedPackSteim2(dest, input, d0, record->hdr.nsamp, nf, pad, pnframe, pnsamp)) < 0) return FALSE;
    datum->value = input[*pnsamp - 1];

    mseedLog(handle, 3, "%s: *pnsamp = %d, *pnframe = %d\n", fid, *pnsamp, *pnframe);
    return TRUE;
}

BOOL mseedPackData(MSEED_HANDLE *handle, MSEED_PACKED *packed, MSEED_RECORD *record, int *pnsamp, int *pnframe, int *format)
{
BOOL ok;
static char *fid = "mseedPackData";

    if (handle == NULL || packed == NULL || record == NULL || pnsamp == NULL || pnframe == NULL || format == NULL) {
        errno = EINVAL;
        return FALSE;
    }

    switch (*format) {

      case MSEED_FORMAT_INT_16:
        switch (record->hdr.format) {
          case MSEED_FORMAT_INT_16: ok = INT_16_to_INT_16(handle, packed, record, pnsamp, pnframe); break;
          case MSEED_FORMAT_INT_32: ok = INT_32_to_INT_16(handle, packed, record, pnsamp, pnframe); break;
          case MSEED_FORMAT_IEEE_F: ok = IEEE_F_to_INT_16(handle, packed, record, pnsamp, pnframe); break;
          case MSEED_FORMAT_IEEE_D: ok = IEEE_D_to_INT_16(handle, packed, record, pnsamp, pnframe); break;
        }
        break;

      case MSEED_FORMAT_INT_32:
        switch (record->hdr.format) {
          case MSEED_FORMAT_INT_16: ok = INT_16_to_INT_32(handle, packed, record, pnsamp, pnframe); break;
          case MSEED_FORMAT_INT_32: ok = INT_32_to_INT_32(handle, packed, record, pnsamp, pnframe); break;
          case MSEED_FORMAT_IEEE_F: ok = IEEE_F_to_INT_32(handle, packed, record, pnsamp, pnframe); break;
          case MSEED_FORMAT_IEEE_D: ok = IEEE_D_to_INT_32(handle, packed, record, pnsamp, pnframe); break;
        }
        break;

      case MSEED_FORMAT_IEEE_F:
        switch (record->hdr.format) {
          case MSEED_FORMAT_INT_16: ok = INT_16_to_IEEE_F(handle, packed, record, pnsamp, pnframe); break;
          case MSEED_FORMAT_INT_32: ok = INT_32_to_IEEE_F(handle, packed, record, pnsamp, pnframe); break;
          case MSEED_FORMAT_IEEE_F: ok = IEEE_F_to_IEEE_F(handle, packed, record, pnsamp, pnframe); break;
          case MSEED_FORMAT_IEEE_D: ok = IEEE_D_to_IEEE_F(handle, packed, record, pnsamp, pnframe); break;
        }
        break;

      case MSEED_FORMAT_IEEE_D:
        switch (record->hdr.format) {
          case MSEED_FORMAT_INT_16: ok = INT_16_to_IEEE_D(handle, packed, record, pnsamp, pnframe); break;
          case MSEED_FORMAT_INT_32: ok = INT_32_to_IEEE_D(handle, packed, record, pnsamp, pnframe); break;
          case MSEED_FORMAT_IEEE_F: ok = IEEE_F_to_IEEE_D(handle, packed, record, pnsamp, pnframe); break;
          case MSEED_FORMAT_IEEE_D: ok = IEEE_D_to_IEEE_D(handle, packed, record, pnsamp, pnframe); break;
        }
        break;

      case MSEED_FORMAT_STEIM1:
        switch (record->hdr.format) {
          case MSEED_FORMAT_INT_16: ok = INT_16_to_STEIM1(handle, packed, record, pnsamp, pnframe); break;
          case MSEED_FORMAT_INT_32: ok = INT_32_to_STEIM1(handle, packed, record, pnsamp, pnframe); break;
          case MSEED_FORMAT_IEEE_F: ok = IEEE_F_to_STEIM1(handle, packed, record, pnsamp, pnframe); break;
          case MSEED_FORMAT_IEEE_D: ok = IEEE_D_to_STEIM1(handle, packed, record, pnsamp, pnframe); break;
        }
        break;

      case MSEED_FORMAT_STEIM2:
        switch (record->hdr.format) {
          case MSEED_FORMAT_INT_16: ok = INT_16_to_STEIM2(handle, packed, record, pnsamp, pnframe); break;
          case MSEED_FORMAT_INT_32: ok = INT_32_to_STEIM2(handle, packed, record, pnsamp, pnframe); break;
          case MSEED_FORMAT_IEEE_F: ok = IEEE_F_to_STEIM2(handle, packed, record, pnsamp, pnframe); break;
          case MSEED_FORMAT_IEEE_D: ok = IEEE_D_to_STEIM2(handle, packed, record, pnsamp, pnframe); break;
        }
        break;

    }

/* Check for failed Steim compression */

    if (ok) return TRUE;

    if (*format == MSEED_FORMAT_STEIM1 || *format == MSEED_FORMAT_STEIM2) {
        mseedLog(handle, 0, "%s: warning: Steim compression failed, repacking using native encoding\n", fid);
        switch (record->hdr.format) {
          case MSEED_FORMAT_INT_16: ok = INT_16_to_INT_16(handle, packed, record, pnsamp, pnframe); break;
          case MSEED_FORMAT_INT_32: ok = INT_32_to_INT_32(handle, packed, record, pnsamp, pnframe); break;
          case MSEED_FORMAT_IEEE_F: ok = IEEE_F_to_IEEE_F(handle, packed, record, pnsamp, pnframe); break;
          case MSEED_FORMAT_IEEE_D: ok = IEEE_D_to_IEEE_D(handle, packed, record, pnsamp, pnframe); break;
          default:                  ok = FALSE;                                                     break;
        }
        if (ok) {
            *format = record->hdr.format;
            return TRUE;
        }
    }

    errno = ENOTSUP;
    return FALSE;
}

/*-----------------------------------------------------------------------+
 |                                                                       |
 | Copyright (C) 2014 Regents of the University of California            |
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
 * Revision 1.4  2014/10/29 21:25:41  dechavez
 * check for failed Steim compression (prevents seg fault) and repack natively
 *
 * Revision 1.3  2014/08/26 17:32:10  dechavez
 * fixed bug packing non-Steim data
 *
 * Revision 1.2  2014/08/19 18:02:32  dechavez
 * rework to use MSEED_PACKED
 *
 * Revision 1.1  2014/08/11 18:18:01  dechavez
 * initial release
 *
 */
