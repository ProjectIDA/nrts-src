#pragma ident "$Id: readata.c,v 1.3 2015/11/04 22:13:43 dechavez Exp $"
/*======================================================================
 *
 *  Given information from wfdisc record read data into a REAL64 array
 *
 *====================================================================*/
#include "cssio.h"

static BOOL ReadINT16(FILE *fp, INT32 nsamp, INT16 *dest, BOOL swap)
{
    if (fread(dest, sizeof(INT16), nsamp, fp) != nsamp) return FALSE;
    if (swap) utilSwapINT16(dest, nsamp);

    return TRUE;
}

static BOOL ReadINT32(FILE *fp, INT32 nsamp, INT32 *dest, BOOL swap)
{
    if (fread(dest, sizeof(INT32), nsamp, fp) != nsamp) return FALSE;
    if (swap) utilSwapINT32(dest, nsamp);

    return TRUE;
}

static BOOL ReadREAL32(FILE *fp, INT32 nsamp, REAL32 *dest, BOOL swap)
{
    if (fread(dest, sizeof(REAL32), nsamp, fp) != nsamp) return FALSE;
    if (swap) utilSwapREAL32(dest, nsamp);

    return TRUE;
}

static BOOL ReadREAL64(FILE *fp, INT32 nsamp, REAL64 *dest, BOOL swap)
{
    if (fread(dest, sizeof(REAL64), nsamp, fp) != nsamp) return FALSE;
    if (swap) utilSwapREAL64(dest, nsamp);

    return TRUE;
}

static BOOL ReadS2(FILE *fp, INT32 nsamp, INT16 *dest)
{
#ifdef BIG_ENDIAN_HOST
BOOL swap = FALSE;
#else
BOOL swap = TRUE;
#endif

    if (!ReadINT16(fp, nsamp, dest, swap)) return FALSE;

    return TRUE;
}

static BOOL ReadI2(FILE *fp, INT32 nsamp, INT16 *dest)
{
#ifdef BIG_ENDIAN_HOST
BOOL swap = TRUE;
#else
BOOL swap = FALSE;
#endif

    if (!ReadINT16(fp, nsamp, dest, swap)) return FALSE;

    return TRUE;
}

static BOOL ReadS4(FILE *fp, INT32 nsamp, INT32 *dest)
{
#ifdef BIG_ENDIAN_HOST
BOOL swap = FALSE;
#else
BOOL swap = TRUE;
#endif

    if (!ReadINT32(fp, nsamp, dest, swap)) return FALSE;

    return TRUE;
}

static BOOL ReadI4(FILE *fp, INT32 nsamp, INT32 *dest)
{
#ifdef BIG_ENDIAN_HOST
BOOL swap = TRUE;
#else
BOOL swap = FALSE;
#endif

    if (!ReadINT32(fp, nsamp, dest, swap)) return FALSE;

    return TRUE;
}

static BOOL ReadT4(FILE *fp, INT32 nsamp, REAL32 *dest)
{
#ifdef BIG_ENDIAN_HOST
BOOL swap = FALSE;
#else
BOOL swap = TRUE;
#endif

    if (!ReadREAL32(fp, nsamp, dest, swap)) return FALSE;

    return TRUE;
}

static BOOL ReadF4(FILE *fp, INT32 nsamp, REAL32 *dest)
{
#ifdef BIG_ENDIAN_HOST
BOOL swap = TRUE;
#else
BOOL swap = FALSE;
#endif

    if (!ReadREAL32(fp, nsamp, dest, swap)) return FALSE;

    return TRUE;
}

static BOOL ReadU4(FILE *fp, INT32 nsamp, REAL32 *dest)
{
    return ReadF4(fp, nsamp, dest);

    return TRUE;
}

static BOOL ReadT8(FILE *fp, INT32 nsamp, REAL64 *dest)
{
#ifdef BIG_ENDIAN_HOST
BOOL swap = FALSE;
#else
BOOL swap = TRUE;
#endif

    if (!ReadREAL64(fp, nsamp, dest, swap)) return FALSE;

    return TRUE;
}

static BOOL ReadF8(FILE *fp, INT32 nsamp, REAL64 *dest)
{
#ifdef BIG_ENDIAN_HOST
BOOL swap = TRUE;
#else
BOOL swap = FALSE;
#endif

    if (!ReadREAL64(fp, nsamp, dest, swap)) return FALSE;

    return TRUE;
}

static BOOL ReadU8(FILE *fp, INT32 nsamp, REAL64 *dest)
{
    return ReadF8(fp, nsamp, dest);
}

BOOL cssioReadDataIntoReal64(WFDISC *wd, REAL64 *dest)
{
int i;
char path[MAXPATHLEN+1];
FILE *fp;
INT16 *int16;
INT32 *int32;
REAL32 *real32;

    if (wd == NULL || dest == NULL) {
        errno = EINVAL;
        return FALSE;
    }

/* open file and move to read position */

    sprintf(path, "%s/%s", wd->dir, wd->dfile);
    if ((fp = fopen(path, "rb")) == NULL) return FALSE;

    if (fseek(fp, wd->foff, 0) == -1) return FALSE;

/* read all the supported data types */

    switch (cssioDatatypeCode(wd->datatype)) {

      case CSSIO_DATATYPE_S2:
        if ((int16 = (INT16 *) malloc(wd->nsamp * sizeof(INT16))) == NULL) {
            fclose(fp);
            return FALSE;
        }
        if (!ReadS2(fp, wd->nsamp, int16)) return FALSE;
        for (i = 0; i < wd->nsamp; i++) dest[i] = (REAL64) int16[i];
        free(int16);
        break;

      case CSSIO_DATATYPE_I2:
        if ((int16 = (INT16 *) malloc(wd->nsamp * sizeof(INT16))) == NULL) {
            fclose(fp);
            return FALSE;
        }
        if (!ReadI2(fp, wd->nsamp, int16)) return FALSE;
        for (i = 0; i < wd->nsamp; i++) dest[i] = (REAL64) int16[i];
        free(int16);
        break;

      case CSSIO_DATATYPE_S4:
        if ((int32 = (INT32 *) malloc(wd->nsamp * sizeof(INT32))) == NULL) {
            fclose(fp);
            return FALSE;
        }
        if (!ReadS4(fp, wd->nsamp, int32)) return FALSE;
        for (i = 0; i < wd->nsamp; i++) dest[i] = (REAL64) int32[i];
        free(int32);
        break;

      case CSSIO_DATATYPE_I4:
        if ((int32 = (INT32 *) malloc(wd->nsamp * sizeof(INT32))) == NULL) {
            fclose(fp);
            return FALSE;
        }
        if (!ReadI4(fp, wd->nsamp, int32)) return FALSE;
        for (i = 0; i < wd->nsamp; i++) dest[i] = (REAL64) int32[i];
        free(int32);
        break;

      case CSSIO_DATATYPE_T4:
        if ((real32 = (REAL32 *) malloc(wd->nsamp * sizeof(REAL32))) == NULL) {
            fclose(fp);
            return FALSE;
        }
        if (!ReadT4(fp, wd->nsamp, real32)) return FALSE;
        for (i = 0; i < wd->nsamp; i++) dest[i] = (REAL64) real32[i];
        free(real32);
        break;

      case CSSIO_DATATYPE_F4:
        if ((real32 = (REAL32 *) malloc(wd->nsamp * sizeof(REAL32))) == NULL) {
            fclose(fp);
            return FALSE;
        }
        if (!ReadF4(fp, wd->nsamp, real32)) return FALSE;
        for (i = 0; i < wd->nsamp; i++) dest[i] = (REAL64) real32[i];
        free(real32);
        break;

      case CSSIO_DATATYPE_T8:
        if (!ReadT8(fp, wd->nsamp, dest)) return FALSE;
        break;

      case CSSIO_DATATYPE_F8:
        if (!ReadF8(fp, wd->nsamp, dest)) return FALSE;
        break;

      default:
        errno = ENOTSUP;
        return FALSE;
    }

    fclose(fp);
    return TRUE;
}

/* Revision History
 *
 * $Log: readata.c,v $
 * Revision 1.3  2015/11/04 22:13:43  dechavez
 * fixed a whole bunch of missing return values for the ReadXX() subfunctions
 *
 * Revision 1.2  2014/04/18 18:26:29  dechavez
 * reworked to use CSSIO_DATATYPE_x enums instead of datatype strcmp's
 *
 * Revision 1.1  2014/04/15 19:33:02  dechavez
 * created, with cssioReadDataIntoReal64()
 *
 */
