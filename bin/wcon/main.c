#pragma ident "$Id: main.c,v 1.1 2014/04/18 18:23:02 dechavez Exp $"
/*======================================================================
 *
 *  Convert a stream of stdin data of user specified CSS 3.0 format
 *  into other formats, including ascii.
 *
 *====================================================================*/
#include "wcon.h"

#define BUFLEN 10240
static UINT8 iobuf[BUFLEN];

static void help(char *myname)
{
char *text;

    text = cssioDatatypeString(CSSIO_DATATYPE_TEXT);

    fprintf(stderr, "usage: %s i=typestring o=typestring < input > output\n", myname);
    fprintf(stderr, "where typestring is one of: %s\n", cssioDatatypeListString(" ", NULL));
    fprintf(stderr, "\n");
    fprintf(stderr, "For anything other than '%s' output, the input must be of a smaller type.\n", text);
    fprintf(stderr, "In the case where 'i=%s' ", text);
    fprintf(stderr, "the text is assumed to agree with the output type\n");
    exit(1);
}

static void ReductionNotAllowed(void)
{
    fprintf(stderr, "Conversion to smaller data types is not permitted.\n");
    exit(1);
}

size_t READ(void *ptr, size_t count)
{
size_t nbyte;

    if ((nbyte = fread(ptr, sizeof(char), count, stdin)) == count) return nbyte;
    if (feof(stdin)) return nbyte;

    perror("fread");
    exit(1);
}

static int SCAN(char *format, void *dest)
{
    errno = 0;
    fscanf(stdin, format, dest);
    if (errno != 0) {
        perror("fscanf");
        exit(1);
    }
    if (feof(stdin)) return 0;

    return 1;
}

static void WRITE(void *ptr, size_t count)
{
    if (fwrite(ptr, sizeof(char), count, stdout) == count) return;

    perror("fwrite");
    exit(1);
}
static void FAILED_PRINT(void)
{
    perror("printf");
    exit(1);
}

static void EchoInput(void)
{
size_t nbyte;

    while ((nbyte = READ(iobuf, BUFLEN)) > 0) WRITE(iobuf, nbyte);
    exit(0);
}

static void ReadF4WriteF8(void)
{
REAL32 input;
REAL64 output;
size_t ilen = sizeof(input);
size_t olen = sizeof(output);

    while (READ(iobuf, ilen) == ilen) {
        utilUnkcapREAL32(iobuf, &input);
        output = (REAL64) input;
        utilKcapREAL64(iobuf, output);
        WRITE(&iobuf, olen);
    }
    exit(0);
}

static void ReadF4WriteI8(void)
{
REAL32 input;
INT64  output;
size_t ilen = sizeof(input);
size_t olen = sizeof(output);

    while (READ(iobuf, ilen) == ilen) {
        utilUnkcapREAL32(iobuf, &input);
        output = (INT64) input;
        utilKcapINT64(iobuf, output);
        WRITE(&iobuf, olen);
    }
    exit(0);
}

static void ReadF4WriteS8(void)
{
REAL32 input;
INT64  output;
size_t ilen = sizeof(input);
size_t olen = sizeof(output);

    while (READ(iobuf, ilen) == ilen) {
        utilUnkcapREAL32(iobuf, &input);
        output = (INT64) input;
        utilPackINT64(iobuf, output);
        WRITE(&iobuf, olen);
    }
    exit(0);
}

static void ReadF4WriteT4(void)
{
REAL32 input;
size_t ilen = sizeof(input);
size_t olen = sizeof(input);

    while (READ(iobuf, ilen) == ilen) {
        utilUnkcapREAL32(iobuf, &input);
        utilPackINT32(iobuf, input);
        WRITE(&iobuf, olen);
    }
    exit(0);
}

static void ReadF4WriteT8(void)
{
REAL32 input;
REAL64 output;
size_t ilen = sizeof(input);
size_t olen = sizeof(output);

    while (READ(iobuf, ilen) == ilen) {
        utilUnkcapREAL32(iobuf, &input);
        output = (REAL64) input;
        utilPackREAL64(iobuf, output);
        WRITE(&iobuf, olen);
    }
    exit(0);
}

static void ReadF4WriteText(void)
{
REAL32 input;
size_t ilen = sizeof(input);

    while (READ(iobuf, ilen) == ilen) {
        utilUnkcapREAL32(iobuf, &input);
        if (printf("%.8g\n", input) < 0) FAILED_PRINT();
    }
    exit(0);
}

static void ReadF8WriteI8(void)
{
REAL64 input;
INT64  output;
size_t ilen = sizeof(input);
size_t olen = sizeof(output);

    while (READ(iobuf, ilen) == ilen) {
        utilUnkcapREAL64(iobuf, &input);
        output = (INT64) input;
        utilKcapINT64(iobuf, output);
        WRITE(&iobuf, olen);
    }
    exit(0);
}

static void ReadF8WriteS8(void)
{
REAL64 input;
INT64  output;
size_t ilen = sizeof(input);
size_t olen = sizeof(output);

    while (READ(iobuf, ilen) == ilen) {
        utilUnkcapREAL64(iobuf, &input);
        output = (INT64) input;
        utilPackINT64(iobuf, output);
        WRITE(&iobuf, olen);
    }
    exit(0);
}

static void ReadF8WriteT8(void)
{
REAL64 input;
size_t ilen = sizeof(input);
size_t olen = sizeof(input);

    while (READ(iobuf, ilen) == ilen) {
        utilUnkcapREAL64(iobuf, &input);
        utilPackREAL64(iobuf, input);
        WRITE(&iobuf, olen);
    }
    exit(0);
}

static void ReadF8WriteText(void)
{
REAL64 input;
size_t ilen = sizeof(input);

    while (READ(iobuf, ilen) == ilen) {
        utilUnkcapREAL64(iobuf, &input);
        if (printf("%.8lg\n", input) < 0) FAILED_PRINT();
    }
    exit(0);
}

static void ReadI2WriteF4(void)
{
INT16  input;
REAL32 output;
size_t ilen = sizeof(input);
size_t olen = sizeof(output);

    while (READ(iobuf, ilen) == ilen) {
        utilUnkcapINT16(iobuf, &input);
        output = (REAL32) input;
        utilKcapREAL32(iobuf, output);
        WRITE(&iobuf, olen);
    }
    exit(0);
}

static void ReadI2WriteF8(void)
{
INT16  input;
REAL64 output;
size_t ilen = sizeof(input);
size_t olen = sizeof(output);

    while (READ(iobuf, ilen) == ilen) {
        utilUnkcapINT16(iobuf, &input);
        output = (REAL64) input;
        utilKcapREAL64(iobuf, output);
        WRITE(&iobuf, olen);
    }
    exit(0);
}

static void ReadI2WriteI4(void)
{
INT16  input;
INT32  output;
size_t ilen = sizeof(input);
size_t olen = sizeof(output);

    while (READ(iobuf, ilen) == ilen) {
        utilUnkcapINT16(iobuf, &input);
        output = (INT32) input;
        utilKcapINT32(iobuf, output);
        WRITE(&iobuf, olen);
    }
    exit(0);
}

static void ReadI2WriteI8(void)
{
INT16  input;
INT64  output;
size_t ilen = sizeof(input);
size_t olen = sizeof(output);

    while (READ(iobuf, ilen) == ilen) {
        utilUnkcapINT16(iobuf, &input);
        output = (INT64) input;
        utilKcapINT64(iobuf, output);
        WRITE(&iobuf, olen);
    }
    exit(0);
}

static void ReadI2WriteS2(void)
{
INT16  input;
size_t ilen = sizeof(input);
size_t olen = sizeof(input);

    while (READ(iobuf, ilen) == ilen) {
        utilUnkcapINT16(iobuf, &input);
        utilPackINT16(iobuf, input);
        WRITE(&iobuf, olen);
    }
    exit(0);
}

static void ReadI2WriteS4(void)
{
INT16  input;
INT32  output;
size_t ilen = sizeof(input);
size_t olen = sizeof(output);

    while (READ(iobuf, ilen) == ilen) {
        utilUnkcapINT16(iobuf, &input);
        output = (INT32) input;
        utilPackINT32(iobuf, output);
        WRITE(&iobuf, olen);
    }
    exit(0);
}

static void ReadI2WriteS8(void)
{
INT16  input;
INT64  output;
size_t ilen = sizeof(input);
size_t olen = sizeof(output);

    while (READ(iobuf, ilen) == ilen) {
        utilUnkcapINT16(iobuf, &input);
        output = (INT64) input;
        utilPackINT64(iobuf, output);
        WRITE(&iobuf, olen);
    }
    exit(0);
}

static void ReadI2WriteT4(void)
{
INT16  input;
REAL32 output;
size_t ilen = sizeof(input);
size_t olen = sizeof(output);

    while (READ(iobuf, ilen) == ilen) {
        utilUnkcapINT16(iobuf, &input);
        output = (REAL32) input;
        utilPackREAL32(iobuf, output);
        WRITE(&iobuf, olen);
    }
    exit(0);
}

static void ReadI2WriteT8(void)
{
INT16  input;
REAL64 output;
size_t ilen = sizeof(input);
size_t olen = sizeof(output);

    while (READ(iobuf, ilen) == ilen) {
        utilUnkcapINT16(iobuf, &input);
        output = (REAL64) input;
        utilPackREAL64(iobuf, output);
        WRITE(&iobuf, olen);
    }
    exit(0);
}

static void ReadI2WriteText(void)
{
INT16 input;
size_t ilen = sizeof(input);

    while (READ(iobuf, ilen) == ilen) {
        utilUnkcapINT16(iobuf, &input);
        if (printf("%d\n", input) < 0) FAILED_PRINT();
    }
    exit(0);
}

static void ReadI4WriteF4(void)
{
INT32  input;
REAL32 output;
size_t ilen = sizeof(input);
size_t olen = sizeof(output);

    while (READ(iobuf, ilen) == ilen) {
        utilUnkcapINT32(iobuf, &input);
        output = (REAL32) input;
        utilKcapREAL32(iobuf, output);
        WRITE(&iobuf, olen);
    }
    exit(0);
}

static void ReadI4WriteF8(void)
{
INT32  input;
REAL64 output;
size_t ilen = sizeof(input);
size_t olen = sizeof(output);

    while (READ(iobuf, ilen) == ilen) {
        utilUnkcapINT32(iobuf, &input);
        output = (REAL64) input;
        utilKcapREAL64(iobuf, output);
        WRITE(&iobuf, olen);
    }
    exit(0);
}

static void ReadI4WriteI8(void)
{
INT32  input;
INT64  output;
size_t ilen = sizeof(input);
size_t olen = sizeof(output);

    while (READ(iobuf, ilen) == ilen) {
        utilUnkcapINT32(iobuf, &input);
        output = (INT64) input;
        utilKcapINT64(iobuf, output);
        WRITE(&iobuf, olen);
    }
    exit(0);
}

static void ReadI4WriteS4(void)
{
INT32  input;
size_t ilen = sizeof(input);
size_t olen = sizeof(input);

    while (READ(iobuf, ilen) == ilen) {
        utilUnkcapINT32(iobuf, &input);
        utilPackINT32(iobuf, input);
        WRITE(&iobuf, olen);
    }
    exit(0);
}

static void ReadI4WriteS8(void)
{
INT32  input;
INT64  output;
size_t ilen = sizeof(input);
size_t olen = sizeof(output);

    while (READ(iobuf, ilen) == ilen) {
        utilUnkcapINT32(iobuf, &input);
        output = (INT64) input;
        utilPackINT64(iobuf, output);
        WRITE(&iobuf, olen);
    }
    exit(0);
}

static void ReadI4WriteT4(void)
{
INT32  input;
REAL32 output;
size_t ilen = sizeof(input);
size_t olen = sizeof(output);

    while (READ(iobuf, ilen) == ilen) {
        utilUnkcapINT32(iobuf, &input);
        output = (REAL32) input;
        utilPackREAL32(iobuf, output);
        WRITE(&iobuf, olen);
    }
    exit(0);
}

static void ReadI4WriteT8(void)
{
INT32  input;
REAL64 output;
size_t ilen = sizeof(input);
size_t olen = sizeof(output);

    while (READ(iobuf, ilen) == ilen) {
        utilUnkcapINT32(iobuf, &input);
        output = (REAL64) input;
        utilPackREAL64(iobuf, output);
        WRITE(&iobuf, olen);
    }
    exit(0);
}

static void ReadI4WriteText(void)
{
INT32 input;
size_t ilen = sizeof(input);

    while (READ(iobuf, ilen) == ilen) {
        utilUnkcapINT32(iobuf, &input);
        if (printf("%d\n", input) < 0) FAILED_PRINT();
    }
    exit(0);
}

static void ReadI8WriteS8(void)
{
INT64  input;
size_t ilen = sizeof(input);
size_t olen = sizeof(input);

    while (READ(iobuf, ilen) == ilen) {
        utilUnkcapINT64(iobuf, &input);
        utilPackINT64(iobuf, input);
        WRITE(&iobuf, olen);
    }
    exit(0);
}

static void ReadI8WriteF8(void)
{
INT64  input;
REAL64 output;
size_t ilen = sizeof(input);
size_t olen = sizeof(output);

    while (READ(iobuf, ilen) == ilen) {
        utilUnkcapINT64(iobuf, &input);
        output = (REAL64) input;
        utilKcapREAL64(iobuf, output);
        WRITE(&iobuf, olen);
    }
    exit(0);
}

static void ReadI8WriteT8(void)
{
INT64  input;
REAL64 output;
size_t ilen = sizeof(input);
size_t olen = sizeof(output);

    while (READ(iobuf, ilen) == ilen) {
        utilUnkcapINT64(iobuf, &input);
        output = (REAL64) input;
        utilPackREAL64(iobuf, output);
        WRITE(&iobuf, olen);
    }
    exit(0);
}

static void ReadI8WriteText(void)
{
INT64 input;
size_t ilen = sizeof(input);

    while (READ(iobuf, ilen) == ilen) {
        utilUnkcapINT64(iobuf, &input);
        if (printf("%lld\n", input) < 0) FAILED_PRINT();
    }
    exit(0);
}

static void ReadS2WriteF4(void)
{
INT16  input;
REAL32 output;
size_t ilen = sizeof(input);
size_t olen = sizeof(output);

    while (READ(iobuf, ilen) == ilen) {
        utilUnpackINT16(iobuf, &input);
        output = (REAL32) input;
        utilKcapREAL32(iobuf, output);
        WRITE(&iobuf, olen);
    }
    exit(0);
}

static void ReadS2WriteF8(void)
{
INT16  input;
REAL64 output;
size_t ilen = sizeof(input);
size_t olen = sizeof(output);

    while (READ(iobuf, ilen) == ilen) {
        utilUnpackINT16(iobuf, &input);
        output = (REAL64) input;
        utilKcapREAL64(iobuf, output);
        WRITE(&iobuf, olen);
    }
    exit(0);
}

static void ReadS2WriteI2(void)
{
INT16  input;
size_t ilen = sizeof(input);
size_t olen = sizeof(input);

    while (READ(iobuf, ilen) == ilen) {
        utilUnpackINT16(iobuf, &input);
        utilKcapINT16(iobuf, input);
        WRITE(&iobuf, olen);
    }
    exit(0);
}

static void ReadS2WriteI4(void)
{
INT16  input;
INT32  output;
size_t ilen = sizeof(input);
size_t olen = sizeof(output);

    while (READ(iobuf, ilen) == ilen) {
        utilUnpackINT16(iobuf, &input);
        output = (INT32) input;
        utilKcapINT32(iobuf, output);
        WRITE(&iobuf, olen);
    }
    exit(0);
}

static void ReadS2WriteI8(void)
{
INT16  input;
INT64  output;
size_t ilen = sizeof(input);
size_t olen = sizeof(output);

    while (READ(iobuf, ilen) == ilen) {
        utilUnpackINT16(iobuf, &input);
        output = (INT64) input;
        utilKcapINT64(iobuf, output);
        WRITE(&iobuf, olen);
    }
    exit(0);
}

static void ReadS2WriteS4(void)
{
INT16  input;
INT32  output;
size_t ilen = sizeof(input);
size_t olen = sizeof(output);

    while (READ(iobuf, ilen) == ilen) {
        utilUnpackINT16(iobuf, &input);
        output = (INT32) input;
        utilPackINT32(iobuf, output);
        WRITE(&iobuf, olen);
    }
    exit(0);
}

static void ReadS2WriteS8(void)
{
INT16  input;
INT64  output;
size_t ilen = sizeof(input);
size_t olen = sizeof(output);

    while (READ(iobuf, ilen) == ilen) {
        utilUnpackINT16(iobuf, &input);
        output = (INT64) input;
        utilPackINT64(iobuf, output);
        WRITE(&iobuf, olen);
    }
    exit(0);
}

static void ReadS2WriteT4(void)
{
INT16  input;
REAL32 output;
size_t ilen = sizeof(input);
size_t olen = sizeof(output);

    while (READ(iobuf, ilen) == ilen) {
        utilUnpackINT16(iobuf, &input);
        output = (REAL32) input;
        utilPackREAL32(iobuf, output);
        WRITE(&iobuf, olen);
    }
    exit(0);
}

static void ReadS2WriteT8(void)
{
INT16  input;
REAL64 output;
size_t ilen = sizeof(input);
size_t olen = sizeof(output);

    while (READ(iobuf, ilen) == ilen) {
        utilUnpackINT16(iobuf, &input);
        output = (REAL64) input;
        utilPackREAL64(iobuf, output);
        WRITE(&iobuf, olen);
    }
    exit(0);
}

static void ReadS2WriteText(void)
{
INT16 input;
size_t ilen = sizeof(input);

    while (READ(iobuf, ilen) == ilen) {
        utilUnpackINT16(iobuf, &input);
        if (printf("%d\n", input) < 0) FAILED_PRINT();
    }
    exit(0);
}

static void ReadS4WriteF4(void)
{
INT32  input;
REAL32 output;
size_t ilen = sizeof(input);
size_t olen = sizeof(output);

    while (READ(iobuf, ilen) == ilen) {
        utilUnpackINT32(iobuf, &input);
        output = (REAL32) input;
        utilKcapREAL32(iobuf, output);
        WRITE(&iobuf, olen);
    }
    exit(0);
}

static void ReadS4WriteF8(void)
{
INT16  input;
REAL64 output;
size_t ilen = sizeof(input);
size_t olen = sizeof(output);

    while (READ(iobuf, ilen) == ilen) {
        utilUnpackINT16(iobuf, &input);
        output = (REAL64) input;
        utilKcapREAL64(iobuf, output);
        WRITE(&iobuf, olen);
    }
    exit(0);
}

static void ReadS4WriteI4(void)
{
INT32  input;
size_t ilen = sizeof(input);
size_t olen = sizeof(input);

    while (READ(iobuf, ilen) == ilen) {
        utilUnpackINT32(iobuf, &input);
        utilKcapINT32(iobuf, input);
        WRITE(&iobuf, olen);
    }
    exit(0);
}

static void ReadS4WriteI8(void)
{
INT32  input;
INT64  output;
size_t ilen = sizeof(input);
size_t olen = sizeof(output);

    while (READ(iobuf, ilen) == ilen) {
        utilUnpackINT32(iobuf, &input);
        output = (INT64) input;
        utilKcapINT64(iobuf, output);
        WRITE(&iobuf, olen);
    }
    exit(0);
}

static void ReadS4WriteS8(void)
{
INT32  input;
INT64  output;
size_t ilen = sizeof(input);
size_t olen = sizeof(output);

    while (READ(iobuf, ilen) == ilen) {
        utilUnpackINT32(iobuf, &input);
        output = (INT64) input;
        utilPackINT64(iobuf, output);
        WRITE(&iobuf, olen);
    }
    exit(0);
}

static void ReadS4WriteT4(void)
{
INT32  input;
REAL32 output;
size_t ilen = sizeof(input);
size_t olen = sizeof(output);

    while (READ(iobuf, ilen) == ilen) {
        utilUnpackINT32(iobuf, &input);
        output = (REAL32) input;
        utilPackREAL32(iobuf, output);
        WRITE(&iobuf, olen);
    }
    exit(0);
}

static void ReadS4WriteT8(void)
{
INT32  input;
REAL64 output;
size_t ilen = sizeof(input);
size_t olen = sizeof(output);

    while (READ(iobuf, ilen) == ilen) {
        utilUnpackINT32(iobuf, &input);
        output = (REAL64) input;
        utilPackREAL64(iobuf, output);
        WRITE(&iobuf, olen);
    }
    exit(0);
}

static void ReadS4WriteText(void)
{
INT32 input;
size_t ilen = sizeof(input);

    while (READ(iobuf, ilen) == ilen) {
        utilUnpackINT32(iobuf, &input);
        if (printf("%d\n", input) < 0) FAILED_PRINT();
    }
    exit(0);
}

static void ReadS8WriteF8(void)
{
INT64  input;
REAL64 output;
size_t ilen = sizeof(input);
size_t olen = sizeof(output);

    while (READ(iobuf, ilen) == ilen) {
        utilUnpackINT64(iobuf, &input);
        output = (REAL64) input;
        utilKcapREAL64(iobuf, output);
        WRITE(&iobuf, olen);
    }
    exit(0);
}

static void ReadS8WriteI8(void)
{
INT64  input;
size_t ilen = sizeof(input);
size_t olen = sizeof(input);

    while (READ(iobuf, ilen) == ilen) {
        utilUnpackINT64(iobuf, &input);
        utilKcapINT64(iobuf, input);
        WRITE(&iobuf, olen);
    }
    exit(0);
}

static void ReadS8WriteT8(void)
{
INT64  input;
REAL64 output;
size_t ilen = sizeof(input);
size_t olen = sizeof(output);

    while (READ(iobuf, ilen) == ilen) {
        utilUnpackINT64(iobuf, &input);
        output = (REAL64) input;
        utilPackREAL64(iobuf, output);
        WRITE(&iobuf, olen);
    }
    exit(0);
}

static void ReadS8WriteText(void)
{
INT64 input;
size_t ilen = sizeof(input);

    while (READ(iobuf, ilen) == ilen) {
        utilUnpackINT64(iobuf, &input);
        if (printf("%lld\n", input) < 0) FAILED_PRINT();
    }
    exit(0);
}

static void ReadT4WriteF4(void)
{
REAL32 input;
size_t ilen = sizeof(input);
size_t olen = sizeof(input);

    while (READ(iobuf, ilen) == ilen) {
        utilUnpackREAL32(iobuf, &input);
        utilKcapINT32(iobuf, input);
        WRITE(&iobuf, olen);
    }
    exit(0);
}

static void ReadT4WriteF8(void)
{
REAL32 input;
REAL64 output;
size_t ilen = sizeof(input);
size_t olen = sizeof(output);

    while (READ(iobuf, ilen) == ilen) {
        utilUnpackREAL32(iobuf, &input);
        output = (REAL64) input;
        utilKcapREAL64(iobuf, output);
        WRITE(&iobuf, olen);
    }
    exit(0);
}

static void ReadT4WriteI8(void)
{
REAL32 input;
INT64  output;
size_t ilen = sizeof(input);
size_t olen = sizeof(output);

    while (READ(iobuf, ilen) == ilen) {
        utilUnpackREAL32(iobuf, &input);
        output = (INT64) input;
        utilKcapINT64(iobuf, output);
        WRITE(&iobuf, olen);
    }
    exit(0);
}

static void ReadT4WriteS8(void)
{
REAL32 input;
INT64  output;
size_t ilen = sizeof(input);
size_t olen = sizeof(output);

    while (READ(iobuf, ilen) == ilen) {
        utilUnpackREAL32(iobuf, &input);
        output = (INT64) input;
        utilPackINT64(iobuf, output);
        WRITE(&iobuf, olen);
    }
    exit(0);
}

static void ReadT4WriteT8(void)
{
REAL32 input;
REAL64 output;
size_t ilen = sizeof(input);
size_t olen = sizeof(output);

    while (READ(iobuf, ilen) == ilen) {
        utilUnpackREAL32(iobuf, &input);
        output = (REAL64) input;
        utilPackREAL64(iobuf, output);
        WRITE(&iobuf, olen);
    }
    exit(0);
}

static void ReadT4WriteText(void)
{
REAL32 input;
size_t ilen = sizeof(input);

    while (READ(iobuf, ilen) == ilen) {
        utilUnpackREAL32(iobuf, &input);
        if (printf("%.8g\n", input) < 0) FAILED_PRINT();
    }
    exit(0);
}

static void ReadT8WriteF8(void)
{
REAL64 input;
size_t ilen = sizeof(input);

    while (READ(iobuf, ilen) == ilen) {
        utilUnpackREAL64(iobuf, &input);
        if (printf("%.8lg\n", input) < 0) FAILED_PRINT();
    }
    exit(0);
}

static void ReadT8WriteI8(void)
{
REAL64 input;
INT64  output;
size_t ilen = sizeof(input);
size_t olen = sizeof(output);

    while (READ(iobuf, ilen) == ilen) {
        utilUnpackREAL64(iobuf, &input);
        output = (INT64) input;
        utilKcapINT64(iobuf, output);
        WRITE(&iobuf, olen);
    }
    exit(0);
}

static void ReadT8WriteS8(void)
{
REAL64 input;
INT64  output;
size_t ilen = sizeof(input);
size_t olen = sizeof(output);

    while (READ(iobuf, ilen) == ilen) {
        utilUnpackREAL64(iobuf, &input);
        output = (INT64) input;
        utilPackINT64(iobuf, output);
        WRITE(&iobuf, olen);
    }
    exit(0);
}

static void ReadT8WriteText(void)
{
REAL64 input;
size_t ilen = sizeof(input);

    while (READ(iobuf, ilen) == ilen) {
        utilUnpackREAL64(iobuf, &input);
        if (printf("%.8lg\n", input) < 0) FAILED_PRINT();
    }
    exit(0);
}

static void ReadTextWriteF4(void)
{
REAL32 input;
size_t olen = sizeof(input);

    while (SCAN("%f", &input)) {
        utilKcapREAL32(iobuf, input);
        WRITE(&iobuf, olen);
    }
    exit(0);
}

static void ReadTextWriteF8(void)
{
REAL64 input;
size_t olen = sizeof(input);

    while (SCAN("%lf", &input)) {
        utilKcapREAL64(iobuf, input);
        WRITE(&iobuf, olen);
    }
    exit(0);
}

static void ReadTextWriteI2(void)
{
INT16 input;
size_t olen = sizeof(input);

    while (SCAN("%i", &input)) {
        utilKcapINT16(iobuf, input);
        WRITE(&iobuf, olen);
    }
    exit(0);
}

static void ReadTextWriteI4(void)
{
INT32 input;
size_t olen = sizeof(input);

    while (SCAN("%i", &input)) {
        utilKcapINT32(iobuf, input);
        WRITE(&iobuf, olen);
    }
    exit(0);
}

static void ReadTextWriteI8(void)
{
INT64 input;
size_t olen = sizeof(input);

    while (SCAN("%lld", &input)) {
        utilKcapINT64(iobuf, input);
        WRITE(&iobuf, olen);
    }
    exit(0);
}

static void ReadTextWriteS2(void)
{
INT16 input;
size_t olen = sizeof(input);

    while (SCAN("%i", &input)) {
        utilPackINT16(iobuf, input);
        WRITE(&iobuf, olen);
    }
    exit(0);
}

static void ReadTextWriteS4(void)
{
INT32 input;
size_t olen = sizeof(input);

    while (SCAN("%i", &input)) {
        utilPackINT32(iobuf, input);
        WRITE(&iobuf, olen);
    }
    exit(0);
}

static void ReadTextWriteS8(void)
{
INT64 input;
size_t olen = sizeof(input);

    while (SCAN("%lld", &input)) {
        utilPackINT64(iobuf, input);
        WRITE(&iobuf, olen);
    }
    exit(0);
}

static void ReadTextWriteT4(void)
{
REAL32 input;
size_t olen = sizeof(input);

    while (SCAN("%f", &input)) {
        utilPackREAL32(iobuf, input);
        WRITE(&iobuf, olen);
    }
    exit(0);
}

static void ReadTextWriteT8(void)
{
REAL64 input;
size_t olen = sizeof(input);

    while (SCAN("%lf", &input)) {
        utilPackREAL64(iobuf, input);
        WRITE(&iobuf, olen);
    }
    exit(0);
}

int main(int argc, char **argv)
{
int i;
char *typestring;
int Iformat = CSSIO_DATATYPE_UNKNOWN;
int Oformat = CSSIO_DATATYPE_UNKNOWN;

    for (i = 1; i < argc; i++) {
        if (strncasecmp(argv[i], "i=", strlen("i=")) == 0) {
            typestring = argv[i]+strlen("i=");
            if ((Iformat = cssioDatatypeCode(typestring)) == CSSIO_DATATYPE_UNKNOWN) {
                fprintf(stderr, "unrecognized input datatype '%s'\n", typestring);
                help(argv[0]);
            }
        } else if (strncasecmp(argv[i], "o=", strlen("o=")) == 0) {
            typestring = argv[i]+strlen("i=");
            if ((Oformat = cssioDatatypeCode(typestring)) == CSSIO_DATATYPE_UNKNOWN) {
                fprintf(stderr, "unrecognized output datatype '%s'\n", typestring);
                help(argv[0]);
            }
        } else if (strcasecmp(argv[i], "-h") == 0) {
            help(argv[0]);
        } else {
            fprintf(stderr, "unrecognized argument '%s'\n", argv[i]);
            help(argv[0]);
        }
    }

    if (Iformat == CSSIO_DATATYPE_UNKNOWN || Oformat == CSSIO_DATATYPE_UNKNOWN) {
        fprintf(stderr, "ERROR: both input and output formats must be specified\n");
        help(argv[0]);
    }

/* Every possible combination of input and output */

    switch (Iformat) {
      case CSSIO_DATATYPE_S2:
        switch (Oformat) {
          case CSSIO_DATATYPE_S2:   EchoInput();           break;
          case CSSIO_DATATYPE_I2:   ReadS2WriteI2();       break;
          case CSSIO_DATATYPE_S4:   ReadS2WriteS4();       break;
          case CSSIO_DATATYPE_I4:   ReadS2WriteI4();       break;
          case CSSIO_DATATYPE_S8:   ReadS2WriteS8();       break;
          case CSSIO_DATATYPE_I8:   ReadS2WriteI8();       break;
          case CSSIO_DATATYPE_T4:   ReadS2WriteT4();       break;
          case CSSIO_DATATYPE_F4:   ReadS2WriteF4();       break;
          case CSSIO_DATATYPE_T8:   ReadS2WriteT8();       break;
          case CSSIO_DATATYPE_F8:   ReadS2WriteF8();       break;
          case CSSIO_DATATYPE_TEXT: ReadS2WriteText();     break;
          default:
            fprintf(stderr, "ERROR: output type '%s' is unsupported\n", cssioDatatypeString(Iformat));
            exit(1);
        }
        break;

      case CSSIO_DATATYPE_I2:
        switch (Oformat) {
          case CSSIO_DATATYPE_S2:   ReadI2WriteS2();       break;
          case CSSIO_DATATYPE_I2:   EchoInput();           break;
          case CSSIO_DATATYPE_S4:   ReadI2WriteS4();       break;
          case CSSIO_DATATYPE_I4:   ReadI2WriteI4();       break;
          case CSSIO_DATATYPE_S8:   ReadI2WriteS8();       break;
          case CSSIO_DATATYPE_I8:   ReadI2WriteI8();       break;
          case CSSIO_DATATYPE_T4:   ReadI2WriteT4();       break;
          case CSSIO_DATATYPE_F4:   ReadI2WriteF4();       break;
          case CSSIO_DATATYPE_T8:   ReadI2WriteT8();       break;
          case CSSIO_DATATYPE_F8:   ReadI2WriteF8();       break;
          case CSSIO_DATATYPE_TEXT: ReadI2WriteText();     break;
          default:
            fprintf(stderr, "ERROR: output type '%s' is unsupported\n", cssioDatatypeString(Iformat));
            exit(1);
        }
        break;

      case CSSIO_DATATYPE_S4:
        switch (Oformat) {
          case CSSIO_DATATYPE_S2:   ReductionNotAllowed(); break;
          case CSSIO_DATATYPE_I2:   ReductionNotAllowed(); break;
          case CSSIO_DATATYPE_S4:   EchoInput();           break;
          case CSSIO_DATATYPE_I4:   ReadS4WriteI4();       break;
          case CSSIO_DATATYPE_S8:   ReadS4WriteS8();       break;
          case CSSIO_DATATYPE_I8:   ReadS4WriteI8();       break;
          case CSSIO_DATATYPE_T4:   ReadS4WriteT4();       break;
          case CSSIO_DATATYPE_F4:   ReadS4WriteF4();       break;
          case CSSIO_DATATYPE_T8:   ReadS4WriteT8();       break;
          case CSSIO_DATATYPE_F8:   ReadS4WriteF8();       break;
          case CSSIO_DATATYPE_TEXT: ReadS4WriteText();     break;
          default:
            fprintf(stderr, "ERROR: output type '%s' is unsupported\n", cssioDatatypeString(Iformat));
            exit(1);
        }
        break;

      case CSSIO_DATATYPE_I4:
        switch (Oformat) {
          case CSSIO_DATATYPE_S2:   ReductionNotAllowed(); break;
          case CSSIO_DATATYPE_I2:   ReductionNotAllowed(); break;
          case CSSIO_DATATYPE_S4:   ReadI4WriteS4();       break;
          case CSSIO_DATATYPE_I4:   EchoInput();           break;
          case CSSIO_DATATYPE_S8:   ReadI4WriteS8();       break;
          case CSSIO_DATATYPE_I8:   ReadI4WriteI8();       break;
          case CSSIO_DATATYPE_T4:   ReadI4WriteT4();       break;
          case CSSIO_DATATYPE_F4:   ReadI4WriteF4();       break;
          case CSSIO_DATATYPE_T8:   ReadI4WriteT8();       break;
          case CSSIO_DATATYPE_F8:   ReadI4WriteF8();       break;
          case CSSIO_DATATYPE_TEXT: ReadI4WriteText();     break;
          default:
            fprintf(stderr, "ERROR: output type '%s' is unsupported\n", cssioDatatypeString(Iformat));
            exit(1);
        }
        break;

      case CSSIO_DATATYPE_S8:
        switch (Oformat) {
          case CSSIO_DATATYPE_S2:   ReductionNotAllowed(); break;
          case CSSIO_DATATYPE_I2:   ReductionNotAllowed(); break;
          case CSSIO_DATATYPE_S4:   ReductionNotAllowed(); break;
          case CSSIO_DATATYPE_I4:   ReductionNotAllowed(); break;
          case CSSIO_DATATYPE_S8:   EchoInput();           break;
          case CSSIO_DATATYPE_I8:   ReadS8WriteI8();       break;
          case CSSIO_DATATYPE_T4:   ReductionNotAllowed(); break;
          case CSSIO_DATATYPE_F4:   ReductionNotAllowed(); break;
          case CSSIO_DATATYPE_T8:   ReadS8WriteT8();       break;
          case CSSIO_DATATYPE_F8:   ReadS8WriteF8();       break;
          case CSSIO_DATATYPE_TEXT: ReadS8WriteText();     break;
          default:
            fprintf(stderr, "ERROR: output type '%s' is unsupported\n", cssioDatatypeString(Iformat));
            exit(1);
        }
        break;

      case CSSIO_DATATYPE_I8:
        switch (Oformat) {
          case CSSIO_DATATYPE_S2:   ReductionNotAllowed(); break;
          case CSSIO_DATATYPE_I2:   ReductionNotAllowed(); break;
          case CSSIO_DATATYPE_S4:   ReductionNotAllowed(); break;
          case CSSIO_DATATYPE_I4:   ReductionNotAllowed(); break;
          case CSSIO_DATATYPE_S8:   ReadI8WriteS8();       break;
          case CSSIO_DATATYPE_I8:   EchoInput();           break;
          case CSSIO_DATATYPE_T4:   ReductionNotAllowed(); break;
          case CSSIO_DATATYPE_F4:   ReductionNotAllowed(); break;
          case CSSIO_DATATYPE_T8:   ReadI8WriteT8();       break;
          case CSSIO_DATATYPE_F8:   ReadI8WriteF8();       break;
          case CSSIO_DATATYPE_TEXT: ReadI8WriteText();     break;
          default:
            fprintf(stderr, "ERROR: output type '%s' is unsupported\n", cssioDatatypeString(Iformat));
            exit(1);
        }
        break;

      case CSSIO_DATATYPE_T4:
        switch (Oformat) {
          case CSSIO_DATATYPE_S2:   ReductionNotAllowed(); break;
          case CSSIO_DATATYPE_I2:   ReductionNotAllowed(); break;
          case CSSIO_DATATYPE_S4:   ReductionNotAllowed(); break;
          case CSSIO_DATATYPE_I4:   ReductionNotAllowed(); break;
          case CSSIO_DATATYPE_S8:   ReadT4WriteS8();       break;
          case CSSIO_DATATYPE_I8:   ReadT4WriteI8();       break;
          case CSSIO_DATATYPE_T4:   EchoInput();           break;
          case CSSIO_DATATYPE_F4:   ReadT4WriteF4();       break;
          case CSSIO_DATATYPE_T8:   ReadT4WriteT8();       break;
          case CSSIO_DATATYPE_F8:   ReadT4WriteF8();       break;
          case CSSIO_DATATYPE_TEXT: ReadT4WriteText();     break;
          default:
            fprintf(stderr, "ERROR: output type '%s' is unsupported\n", cssioDatatypeString(Iformat));
            exit(1);
        }
        break;

      case CSSIO_DATATYPE_F4:
        switch (Oformat) {
          case CSSIO_DATATYPE_S2:   ReductionNotAllowed(); break;
          case CSSIO_DATATYPE_I2:   ReductionNotAllowed(); break;
          case CSSIO_DATATYPE_S4:   ReductionNotAllowed(); break;
          case CSSIO_DATATYPE_I4:   ReductionNotAllowed(); break;
          case CSSIO_DATATYPE_S8:   ReadF4WriteS8();       break;
          case CSSIO_DATATYPE_I8:   ReadF4WriteI8();       break;
          case CSSIO_DATATYPE_T4:   ReadF4WriteT4();       break;
          case CSSIO_DATATYPE_F4:   EchoInput();           break;
          case CSSIO_DATATYPE_T8:   ReadF4WriteT8();       break;
          case CSSIO_DATATYPE_F8:   ReadF4WriteF8();       break;
          case CSSIO_DATATYPE_TEXT: ReadF4WriteText();     break;
          default:
            fprintf(stderr, "ERROR: output type '%s' is unsupported\n", cssioDatatypeString(Iformat));
            exit(1);
        }
        break;

      case CSSIO_DATATYPE_T8:
        switch (Oformat) {
          case CSSIO_DATATYPE_S2:   ReductionNotAllowed(); break;
          case CSSIO_DATATYPE_I2:   ReductionNotAllowed(); break;
          case CSSIO_DATATYPE_S4:   ReductionNotAllowed(); break;
          case CSSIO_DATATYPE_I4:   ReductionNotAllowed(); break;
          case CSSIO_DATATYPE_S8:   ReadT8WriteS8();       break;
          case CSSIO_DATATYPE_I8:   ReadT8WriteI8();       break;
          case CSSIO_DATATYPE_T4:   ReductionNotAllowed(); break;
          case CSSIO_DATATYPE_F4:   ReductionNotAllowed(); break;
          case CSSIO_DATATYPE_T8:   EchoInput();           break;
          case CSSIO_DATATYPE_F8:   ReadT8WriteF8();       break;
          case CSSIO_DATATYPE_TEXT: ReadT8WriteText();     break;
          default:
            fprintf(stderr, "ERROR: output type '%s' is unsupported\n", cssioDatatypeString(Iformat));
            exit(1);
        }
        break;

      case CSSIO_DATATYPE_F8:
        switch (Oformat) {
          case CSSIO_DATATYPE_S2:   ReductionNotAllowed(); break;
          case CSSIO_DATATYPE_I2:   ReductionNotAllowed(); break;
          case CSSIO_DATATYPE_S4:   ReductionNotAllowed(); break;
          case CSSIO_DATATYPE_I4:   ReductionNotAllowed(); break;
          case CSSIO_DATATYPE_S8:   ReadF8WriteS8();       break;
          case CSSIO_DATATYPE_I8:   ReadF8WriteI8();       break;
          case CSSIO_DATATYPE_T4:   ReductionNotAllowed(); break;
          case CSSIO_DATATYPE_F4:   ReductionNotAllowed(); break;
          case CSSIO_DATATYPE_T8:   ReadF8WriteT8();       break;
          case CSSIO_DATATYPE_F8:   EchoInput();           break;
          case CSSIO_DATATYPE_TEXT: ReadF8WriteText();     break;
          default:
            fprintf(stderr, "ERROR: output type '%s' is unsupported\n", cssioDatatypeString(Iformat));
            exit(1);
        }
        break;

      case CSSIO_DATATYPE_TEXT:
        switch (Oformat) {
          case CSSIO_DATATYPE_S2:   ReadTextWriteS2();     break;
          case CSSIO_DATATYPE_I2:   ReadTextWriteI2();     break;
          case CSSIO_DATATYPE_S4:   ReadTextWriteS4();     break;
          case CSSIO_DATATYPE_I4:   ReadTextWriteI4();     break;
          case CSSIO_DATATYPE_S8:   ReadTextWriteS8();     break;
          case CSSIO_DATATYPE_I8:   ReadTextWriteI8();     break;
          case CSSIO_DATATYPE_T4:   ReadTextWriteT4();     break;
          case CSSIO_DATATYPE_F4:   ReadTextWriteF4();     break;
          case CSSIO_DATATYPE_T8:   ReadTextWriteT8();     break;
          case CSSIO_DATATYPE_F8:   ReadTextWriteF8();     break;
          case CSSIO_DATATYPE_TEXT: EchoInput();           break;
          default:
            fprintf(stderr, "ERROR: output type '%s' is unsupported\n", cssioDatatypeString(Iformat));
            exit(1);
        }
        break;
    }

    fprintf(stderr, "ERROR: input type '%s' is unsupported\n", cssioDatatypeString(Iformat));
    exit(1);
}

/* Revision History
 *
 * $Log: main.c,v $
 * Revision 1.1  2014/04/18 18:23:02  dechavez
 * initial release
 *
 */
