#pragma ident "$Id: util.h,v 1.64 2017/10/04 23:53:38 dechavez Exp $"
/*======================================================================
 *
 *  Defines, data structures, and function prototypes for use
 *  with the util library.
 *
 * - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 * Copyright (c) 1997 Regents of the University of California.
 * All rights reserved.
 *====================================================================*/
#ifndef util_h_included
#define util_h_included

#include "platform.h"
#include "logio.h"
#include "list.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifndef MAXPATHLEN
#define MAXPATHLEN 1024
#endif

#ifndef SEC_PER_MIN
#define SEC_PER_MIN 60
#endif

#ifndef MIN_PER_HOUR
#define MIN_PER_HOUR 60
#endif

#ifndef SEC_PER_HOUR
#define SEC_PER_HOUR (SEC_PER_MIN * MIN_PER_HOUR)
#endif

#ifndef HOUR_PER_DAY
#define HOUR_PER_DAY 24
#endif

#ifndef MIN_PER_DAY
#define MIN_PER_DAY (MIN_PER_HOUR * HOUR_PER_DAY)
#endif

#ifndef SEC_PER_DAY
#define SEC_PER_DAY (SEC_PER_MIN * MIN_PER_DAY)
#endif

#ifndef MSEC_PER_SEC
#define MSEC_PER_SEC (UINT64) 1000
#endif

#ifndef USEC_PER_SEC
#define USEC_PER_SEC (MSEC_PER_SEC * (UINT64) 1000)
#endif

#ifndef USEC_PER_MSEC
#define USEC_PER_MSEC (USEC_PER_SEC / MSEC_PER_SEC)
#endif

#ifndef NANOSEC_PER_SEC
#define NANOSEC_PER_SEC (USEC_PER_SEC * (UINT64) 1000)
#endif

#ifndef USEC_PER_NANOSEC
#define USEC_PER_NANOSEC (NANOSEC_PER_SEC / USEC_PER_SEC)
#endif

#define utilNsToS(nsec) ((REAL64) (nsec) / NANOSEC_PER_SEC)

#ifndef NANOSEC_PER_MSEC
#define NANOSEC_PER_MSEC (NANOSEC_PER_SEC / MSEC_PER_SEC)
#endif

#ifndef NANOSEC_PER_USEC
#define NANOSEC_PER_USEC (NANOSEC_PER_SEC / USEC_PER_SEC)
#endif

#ifndef METERS_PER_KILOMETER
#define METERS_PER_KILOMETER 1000
#endif

#ifdef _WIN32
#define UTIL_MIN_TIMESTAMP       0x8000000000000000L
#define UTIL_UNDEFINED_TIMESTAMP 0x7fffffffffffffffL
#else
#define UTIL_MIN_TIMESTAMP       0x8000000000000000LL
#define UTIL_UNDEFINED_TIMESTAMP 0x7fffffffffffffffLL
#endif

#define UTIL_MAX_TIMESTAMP      (UTIL_UNDEFINED_TIMESTAMP - 1)

typedef struct {
    MUTEX mutex;
    UINT64 tstamp;   /* utilTimeStamp() time timer was started */
    UINT64 interval; /* event duration in nanoseconds */
} UTIL_TIMER;

typedef struct {
    MUTEX mutex;
    clock_t start;
    clock_t finish;
    clock_t lap;
    clock_t total;
} UTIL_CPU_TIMER;

/* lifted from CSS csstime.h */

#define ISLEAP(yr)      (!(yr % 4) && yr % 100 || !(yr % 400))

struct css_date_time{
        double epoch;
        UINT32 date;
        int year;
        int month;
        char mname[4];
        int day;
        int doy;
        int hour;
        int minute;
        float second;
};

/* end csstime.h */

typedef struct
    {
#define UTIL_OUTPUTSTREAM_BUFFSIZE 131072
    char cBuff[UTIL_OUTPUTSTREAM_BUFFSIZE];
    char cFileName[4096];
    int  nBytes;
    long nTotalBytes;
    } BufferedStream ;

typedef struct {
    void *ptr;         /* address at which mapping is placed */
#ifdef _WIN32
    HANDLE fd;         /* file descriptor of mmap'd file */
    HANDLE hMapHandle; /* Handle to the file-mapping object */
#else
    int fd;            /* file descriptor of mmap'd file */
#endif
    size_t len;        /* size of region being mapped */
} UTIL_MMAP;

/*  Typical machine byte orders  */

#ifndef LTL_ENDIAN_ORDER
#define LTL_ENDIAN_ORDER 0x10325476  /* VAX, 80x86   */
#endif

#ifndef BIG_ENDIAN_ORDER
#define BIG_ENDIAN_ORDER 0x76543210  /* Sun, MC680x0 */
#endif

#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif

#ifndef SEEK_SET
#define SEEK_SET 0
#endif

#ifndef SEEK_CUR
#define SEEK_CUR 1
#endif

#ifndef SEEK_END
#define SEEK_END 2
#endif

/* Macros */

#define utilTcpSocket(server, port) util_connect((server), NULL, (port), "tcp", 0, 0)

#ifndef leap_year
#define leap_year(i) ((i % 4 == 0 && i % 100 != 0) || i % 400 == 0)
#endif

#ifndef daysize
#define daysize(i) (365 + leap_year(i))
#endif

#define year_secs(year) ((long) util_ydhmsmtod((int)year, 0, 0, 0, 0, 0) - 86400)

#define util_ctoi(c) (c - '0')

#define util_llswap(input, count) utilSwapUINT64((UINT64 *)(input), count)
#define util_lswap(input, count) utilSwapUINT32((UINT32 *)(input), count)
#define util_sswap(input, count) utilSwapUINT16((UINT16 *)(input), count)
#define utilSwapINT16(input, count) utilSwapUINT16((UINT16 *)(input), count)
#define utilSwapINT32(input, count) utilSwapUINT32((UINT32 *)(input), count)
#define utilSwapINT64(input, count) utilSwapUINT64((UINT64 *)(input), count)
#define util_noblock(sd) utilSetNonBlockingSocket(sd)

#ifdef WIN32
#define utilFileExists(path) utilCheckStat(path, _S_IFREG)
#define utilDirectoryExists(path) utilCheckStat(path, _S_IFDIR)
#else
#define utilFileExists(path) utilCheckStat(path, S_IFREG)
#define utilDirectoryExists(path) utilCheckStat(path, S_IFDIR)
#define utilNetworkInit() TRUE
#endif

#ifdef MSDOS
#include <fcntl.h>
#include <io.h>
#define SETMODE(fd, mode) setmode((fd), (mode))
#else
#define SETMODE(fd, mode) 1
#ifndef O_TEXT
#define O_TEXT 0x4000
#endif
#ifndef O_BINARY
#define O_BINARY 0x8000
#endif
#endif /* ifdef MSDOS */

/* Various record locking macros */

#define util_rlock(fd, offset, whence, len) \
        util_lock(fd, F_SETLK, F_RDLCK, offset, whence, len)

#define util_wlock(fd, offset, whence, len) \
        util_lock(fd, F_SETLK, F_WRLCK, offset, whence, len)

#define util_rlockw(fd, offset, whence, len) \
        util_lock(fd, F_SETLKW, F_RDLCK, offset, whence, len)

#define util_wlockw(fd, offset, whence, len) \
        util_lock(fd, F_SETLKW, F_WRLCK, offset, whence, len)

#define util_unlock(fd, offset, whence, len) \
        util_lock(fd, F_SETLKW, F_UNLCK, offset, whence, len)

/*  Misc. useful constants  */

#define BYTES_PER_KBYTE 1024
#define BYTES_PER_MBYTE 1048576
#define BYTES_PER_GBYTE 1073741824

/* complex data type */

typedef struct {
    REAL64 r;
    REAL64 i;
} UTIL_COMPLEX;

#define UTIL_COMPLEX_1 {1.0, 0.0}
#define UTIL_COMPLEX_0 {0.0, 0.0}

/*  Function prototypes  */

/* atolk.c */
long util_atolk(char *string);

/* basename.c */
char *utilBasename(char *path);

/* bcd.c */
int util_bcd2int( UINT8 *input, int numDigits, int nibble);
int util_int2bcd(UINT8 *output, int len, int value, int numDigits);

/* bground.c */
int utilFork(VOID);
int util_bground(int ignsigcld, int closefd);
BOOL utilBackGround(VOID);

/* binprint.c */
char *utilBinString(UINT8 value, char *buf);
void utilPrintBinUINT8(FILE *fp, UINT8 value);
void utilPrintBinUINT16(FILE *fp, UINT16 value);
void utilPrintBinUINT32(FILE *fp, UINT32 value);

/* BufferedStream.c */
void utilInitOutputStreamBuffers();
void utilDestroyOutputStreamBuffers(void);
int utilFlushAllBufferedStream ();
BufferedStream  *utilOpenBufferedStream(char *pFileName);
int utilWriteBufferedStream(BufferedStream  *p, char *pBuff, int n);

/* case.c */
char *util_lcase(char *c);
char *util_ucase(char *c);

/* cat.c */
int util_cat(char *src1, char *src2, char *dest);

/* cfgpair.c */
int util_cfgpair(FILE *fp, int *lineno, char **identifier, char **value);

/* checksum.c */
UINT16 utilIpHeaderChecksum(UINT8 *hdr, int len);
UINT16 utilUdpChecksum(UINT8 *udphdr, UINT32 srcip, UINT32 dstip, UINT8 *payload, int len);
UINT32 util_fletcher32(UINT16 *data, size_t len);
void util_fletcher16(UINT8 *checkA, UINT8 *checkB, UINT8 *data, size_t len);

/* chncmp.c */
int utilChnCompare(char *a, char *b);

/* complex.c */
BOOL utilComplexMultiply(UTIL_COMPLEX *x, UTIL_COMPLEX *y, UTIL_COMPLEX *result);
REAL64 utilComplexMagnitude(UTIL_COMPLEX *complex);

/* compress.c */
UINT32 utilIdaCompressINT32(UINT8 *dest, INT32 *src, UINT32 nsamp);
UINT32 utilIdaExpandINT32(INT32 *dest, UINT8 *src, UINT32 nsamp);
INT32 util_ldcmp(INT32 *dest, UINT8 *src, INT32 nsamp);
INT32 util_lcomp(UINT8 *dest, INT32 *src, INT32 nsamp);
INT32 util_scomp(UINT8 *dest, INT16 *src, INT32 nsamp);
INT16 util_sdcmp(short *dest, UINT8 *src, INT32 nsamp);

/* connect.c */
int util_connect(char *server, char *service, int port, char *protocol, int sndbuf, int rcvbuf);

/* copy.c */
BOOL utilCopyfile(char *dst, char *src, int buflen);

/* crc.c */
UINT32 utilCRC(UINT8 *buf, int len);

/* dir.c */
LNKLST *utilListDirectory(char *path);
BOOL utilRemoveDirectoryTree(char *dirname);

/* dump.c */
VOID utilPrintHexDump(FILE *fp, UINT8 *ptr, int count);
VOID utilLogHexDump(LOGIO *lp, int level, UINT8 *ptr, int count);
int util_bindmp(UINT8 *ptr, long count, long offset, char off_base);
int util_fhexdmp(FILE *fp, UINT8 *ptr, long count, long offset, char off_base);
int util_octdmp(UINT8 *ptr, long count, long offset, char off_base);

/* email.c */
int util_email(char *address, char *subject, char *fname);

/* etoh.c */
void util_etoh(struct css_date_time *dt);
void util_month_day(struct css_date_time *dt);

/* fletcher.c */
UINT32 util_fletcher32(UINT16 *data, size_t len);
void util_fletcher16(UINT8 *checkA, UINT8 *checkB, UINT8 *data, size_t len);

/* getline.c */
int utilGetLine(FILE *fp, char *buffer, int  buflen, char comment, int  *lineno);
int util_getline(FILE *fp, char *buffer, int  buflen, char comment, int  *lineno);

/* gsecmp.c */
INT32 util_cm6(INT32 *input, char *output, INT32 nsamp, INT32 maxout, int ndif, int clip);
INT32 util_dcm6(char *input, INT32 *output, INT32 nbyte, INT32 nsamp, int ndif);
INT32 util_cm8(INT32 *input, char *output, INT32 nsamp, INT32 maxout, int ndif, int clip);
INT32 util_dcm8(char *input, INT32 *output, INT32 nbyte, INT32 nsamp, int ndif);
INT32 util_chksum(INT32 *input, INT32 nsamp);

/* htoe.c */
void util_htoe(struct css_date_time *dt);
INT64 utilYDHMSNTo1999EpochTime(int year, int day, int hr, int mn, int sc, int ns);

/* io.c */
#define utilWriteINT16(fp, value) utilWriteUINT16(fp, (UINT16) value)
#define utilReadINT16(fp, value) utilReadUINT16(fp, (UINT16 *) value)
#define utilWriteINT32(fp, value) utilWriteUINT32(fp, (UINT32) value)
#define utilReadINT32(fp, value) utilReadUINT32(fp, (UINT32 *) value)
#define utilWriteINT64(fp, value) utilWriteUINT64(fp, (UINT32) value)
#define utilReadINT64(fp, value) utilReadUINT64(fp, (UINT32 *) value)
#define utilWriteBOOL(fp, value) utilWriteUINT16(fp, (UINT16) value)
#define utilReadBOOL(fp, value) utilReadUINT16(fp, (UINT16 *) value)
BOOL utilWriteUINT16(FILE *fp, UINT16 value);
BOOL utilReadUINT16(FILE *fp, UINT16 *out);
BOOL utilWriteUINT32(FILE *fp, UINT32 value);
BOOL utilReadUINT32(FILE *fp, UINT32 *out);
BOOL utilWriteUINT64(FILE *fp, UINT64 value);
BOOL utilReadUINT64(FILE *fp, UINT64 *out);
BOOL utilWriteREAL32(FILE *fp, REAL32 value);
BOOL utilReadREAL32(FILE *fp, REAL32 *out);
BOOL utilWriteREAL64(FILE *fp, REAL64 value);
BOOL utilReadREAL64(FILE *fp, REAL64 *out);

/* isint.c */
int util_isinteger(char *string);

/* kcap.c */
#define utilKcapBytes(ptr, string, length) utilPackBytes(ptr, string, length)
int utilKcapUINT16(UINT8 *ptr, UINT16 value);
int utilUnkcapUINT16(UINT8 *ptr, UINT16 *out);
int utilKcapINT16(UINT8 *ptr, INT16 value);
int utilUnkcapINT16(UINT8 *ptr, INT16 *out);
int utilKcapUINT32(UINT8 *ptr, UINT32 value);
int utilUnkcapUINT32(UINT8 *ptr, UINT32 *out);
int utilKcapINT32(UINT8 *ptr, INT32 value);
int utilUnkcapINT32(UINT8 *ptr, INT32 *out);
int utilKcapUINT64(UINT8 *ptr, UINT64 value);
int utilUnkcapUINT64(UINT8 *ptr, UINT64 *out);
int utilKcapINT64(UINT8 *ptr, INT64 value);
int utilUnkcapINT64(UINT8 *ptr, INT64 *out);
int utilKcapREAL32(UINT8 *ptr, REAL32 value);
int utilUnkcapREAL32(UINT8 *ptr, REAL32 *out);
int utilKcapREAL64(UINT8 *ptr, REAL64 value);
int utilUnkcapREAL64(UINT8 *ptr, REAL64 *out);

/* ldump.c */
int util_hexlog(int level, UINT8 *ptr, long count, long offset, char off_base, char *buf);

/* lenprt.c */
int util_lenprt(FILE *fp, char *text, int len, char cont);

/* lock.c */
int util_lock(int fd, int cmd, int type, off_t offset, int whence, off_t len);
BOOL utilLockFileWait(FILE *fp);
void utilUnlockFile(FILE *fp);
BOOL utilWriteLockWait(FILE *fp);
BOOL utilWriteLockTry(FILE *fp);
BOOL utilReadLockWait(FILE *fp);
BOOL utilReadLockTry(FILE *fp);
void utilReleaseLock(FILE *fp);

/* log.c */
void util_incrloglevel(void);
void util_rsetloglevel(void);
int util_logopen(char *file, int min_level, int max_level, int log_level, char *tfmt, char *fmt, ...);
void util_logclose(void);
void util_log(int level, char *fmt, ...);
void utilHookOldLog(LOGIO *ptr);

/* misc.c */
#ifdef WIN32
BOOL utilNetworkInit(VOID);
#endif
VOID utilSetNonBlockingSocket(int sd);
BOOL utilCheckStat(char *path, mode_t mode);
BOOL utilDeleteFile(char *path);
SOCKET utilCloseSocket(int sd);
BOOL utilSetHostAddr(struct sockaddr_in *in_addr, char *host, int port);

/* mkfile.c */
BOOL utilCreateFile(char *path, UINT32 len, BOOL fill, UINT8 datum);

/* mkpath.c */
int util_mkpath(char *path, int mode);

/* mmap.c */
BOOL utilMmap(char *path, off_t offset, size_t len, UTIL_MMAP *map);
VOID utilUnmap(UTIL_MMAP *map);

/* move.c */
int util_move(char *src, char *dest);

/* myip.c */
UINT32 utilMyIpAddr(void);

/* pack.c */
int utilPackBytes(UINT8 *ptr, UINT8 *string, int length);
int utilUnpackBytes(UINT8 *ptr, UINT8 *string, int length);
int utilPackUINT16(UINT8 *ptr, UINT16 value);
int utilUnpackUINT16(UINT8 *ptr, UINT16 *out);
int utilPackINT16(UINT8 *ptr, INT16 value);
int utilUnpackINT16(UINT8 *ptr, INT16 *out);
int utilPackUINT32(UINT8 *ptr, UINT32 value);
int utilUnpackUINT32(UINT8 *ptr, UINT32 *out);
int utilPackINT32(UINT8 *ptr, INT32 value);
int utilUnpackINT32(UINT8 *ptr, INT32 *out);
int utilPackUINT64(UINT8 *ptr, UINT64 value);
int utilUnpackUINT64(UINT8 *ptr, UINT64 *out);
int utilPackINT64(UINT8 *ptr, INT64 value);
int utilUnpackINT64(UINT8 *ptr, INT64 *out);
int utilPackREAL32(UINT8 *ptr, REAL32 value);
int utilUnpackREAL32(UINT8 *ptr, REAL32 *out);
int utilPackREAL64(UINT8 *ptr, REAL64 value);
int utilUnpackREAL64(UINT8 *ptr, REAL64 *out);
int utilPackPascalString(UINT8 *ptr, char *string, int fieldlen);
int utilUnpackPascalString(UINT8 *ptr, char *string, int fieldlen);
int utilPackUINT8Array(UINT8 *start, UINT8 *array, UINT16 length);
int utilUnpackUINT8Array(UINT8 *start, UINT8 *dest, UINT16 maxlen);

/* parse.c */
LNKLST *utilStringTokenList(char *input, char *delimiters, char quote);
int utilParse(char *input, char **argv, char *delimiters, int max_tokens, char quote);
int util_sparse(char *input, char **argv, char *delimiters, int max_tokens);
int util_parse(char *input, char **argv, char *delimiters, int max_tokens, char quote);
BOOL utilParseServer(char *string, char *pserver, int *pport);

/* peer.c */
char *util_peer(int sd, char *buffer, int buflen);
char *utilPeerAddr(int sd, char *buffer, int buflen);
char *utilPeerName(int sd, char *buffer, int buflen);
int utilPeerPort(int sd);

/* power.c */
int util_powerof(long value, int base);

/* query.c */
int utilQuery(char *string);
char utilPause(BOOL prompt);

/* rwtimeo.c */
INT32 utilWrite(int fd, UINT8 *buffer, UINT32 n, INT32 to);
INT32 utilRead(int fd, UINT8 *buffer, INT32 n, INT32 to);
long util_write(int fd, void *buffer, long n, int to);
long util_read(int fd, void *buffer, long n, int to);

/* setid.c */
BOOL utilSetIdentity(char *user);

/* signal.c */
char *util_sigtoa(int signal);

/* sockinfo.c */
void util_sockinfo(int sd, int loglevel);

/* steim.c */
INT32 util_csteim1(UINT8 *dest, INT32 destlen, INT32 *src, INT32 count, INT32 *used);
INT32 util_fcsteim1(UINT8 *dest, INT32 destlen, FILE *fp, int type, double gain, INT32 *used, int *done);
int util_dsteim1(INT32 *dest, INT32 destlen, UINT8 *src, INT32 srclen, UINT32 order, INT32 count);
int util_dsteim2(INT32 *dest, INT32 destlen, UINT8 *src, INT32 srclen, UINT32 order, INT32 count);

/* string.c */
void utilTrimNewline(char *string);
BOOL utilIsBlankString(char *string);
char *utilPadString(char *input, int maxlen, int padchar);
char *utilDotDecimalString(UINT32 iaddr, char *dest);
char *utilTrimString(char *input);
char *util_strpad(char *input, int maxlen, int padchar);
char *util_strtrm(char *input);
char *utilBoolToString(BOOL value);
char *utilProcessPriorityToString(int priority);
char *utilThreadPriorityToString(int priority);
int utilStringToProcessPriority(char *string, BOOL *status);
int utilStringToThreadPriority(char *string, BOOL *status);

/* swap.c */
VOID utilSwapUINT16(UINT16 *input, long count);
VOID utilSwapUINT32(UINT32 *input, long count);
VOID utilSwapUINT64(UINT64 *input, long count);
VOID utilSwapREAL32(REAL32 *input, long count);
VOID utilSwapREAL64(REAL64 *input, long count);
void util_iftovf(UINT32 *input, long number);
void util_vftoif(UINT32 *input, long number);
UINT32 util_order(void);

/* syserr.c */
char *syserrmsg(int code);

/* targ.c */
BOOL utilTarg(char *arg, char *target, BOOL CaseSensitive, char **result);

/* timefunc.c */
double utilAttodt(char *string);
char *utilDttostr(double dtime, int code, char *buf);
char *utilLttostr(long ltime, int code, char *buf);
VOID utilTsplit(double dtime, int *yr, int *da, int *hr, int *mn, int *sc, int *ms);
VOID utilTsplit2(double dtime, int *yr, int *da, int *hr, int *mn, int *sc, int *us);
INT32 utilYearDay(double dtime);
void utilDecomposeTimestamp(INT64 tstamp, int *pyr, int *pda, int *phr, int *pmn, int *psc, int *pns);
INT64 utilComposeTimestamp(int year, int day, int hr, int mn, int sc, int ns);
REAL64 utilYdhmsmtod(int yr, int da, int hr, int mn, int sc, int ms);
VOID utilJdtomd(int year, int day, int *m_no, int *d_no);
UINT32 utilYmdtojd(int year, int mo, int da);
UINT32 utilToday(VOID);
double util_attodt(char *string);
char *util_dttostr(double dtime, int code);
char *util_lttostr(long ltime, int code);
void util_tsplit(double dtime, int *yr, int *da, int *hr, int *mn, int *sc, int *ms);
double util_ydhmsmtod(int yr, int da, int hr, int mn, int sc, int ms);
int util_jdtomd(int year, int day, int *m_no, int *d_no);
int util_ymdtojd(int year, int mo, int da);
UINT32 util_today(VOID);
char *utilTimeString(INT64 value, int code, char *buf, int buflen);
INT64 utilConvertFrom1970SecsTo1999Nsec(double tstamp);
REAL64 utilConvertFrom1999NsecTo1970Secs(INT64 nsec);

/* timer.c */
void utilDelayMsec(UINT32 interval);
INT64 utilTimeStamp(void);
INT64 utilCurrentSecond(void);
void utilInitTimer(UTIL_TIMER *timer);
void utilStartTimer(UTIL_TIMER *timer, UINT64 interval);
void utilResetTimer(UTIL_TIMER *timer);
UINT64 utilElapsedTime(UTIL_TIMER *timer);
BOOL utilTimerExpired(UTIL_TIMER *timer);
void utilMsecToTimeval(struct timeval *dest, UINT32 msec);
void utilInitCPUtimer(UTIL_CPU_TIMER *timer);
void utilStartCPUtimer(UTIL_CPU_TIMER *timer);
void utilStopCPUtimer(UTIL_CPU_TIMER *timer);

/* version.c */

VERSION *utilVersion(VOID);
char *utilVersionString(VOID);

#ifdef __cplusplus
}
#endif

#endif

/* Revision History
 *
 * $Log: util.h,v $
 * Revision 1.64  2017/10/04 23:53:38  dechavez
 * deleted some trailing spaces
 *
 * Revision 1.63  2015/08/24 19:11:10  dechavez
 * defined UTIL_COMPLEX_1 and UTIL_COMPLEX_0 constants, added utilYearDay() prototype
 *
 * Revision 1.62  2015/07/10 17:47:22  dechavez
 * added prototypes for complex.c functions
 *
 * Revision 1.61  2015/03/06 23:07:39  dechavez
 * added prototypes for new functions (version 3.18.0)
 *
 * Revision 1.60  2014/08/11 17:47:26  dechavez
 * added USEC_PER_NANOSEC constant and updated prototypes
 *
 * Revision 1.59  2014/04/28 21:28:08  dechavez
 * added UTIL_CPU_TIMER and related prototypes
 *
 * Revision 1.58  2014/04/15 20:46:14  dechavez
 * updated prototypes
 *
 * Revision 1.57  2013/07/30 19:10:55  dechavez
 * updated prototypes
 *
 * Revision 1.56  2013/03/07 21:14:06  dechavez
 * added kcap.c prototypes
 *
 * Revision 1.55  2012/12/14 18:58:52  dechavez
 * added utilCurrentSecond() prototype
 *
 * Revision 1.54  2012/05/02 18:19:58  dechavez
 * added utilMsecToTimeval() prototype
 *
 * Revision 1.53  2012/04/25 21:04:44  dechavez
 * added utilParseServer() prototype
 *
 * Revision 1.52  2012/02/14 19:48:03  dechavez
 * use portable integer types (eg, long to INT32, unsigned long to UINT32)
 *
 * Revision 1.51  2011/11/03 17:44:24  dechavez
 * added utilPackUINT8Array(), utilUnpackUINT8Array() prototypes
 *
 * Revision 1.50  2011/03/17 17:24:53  dechavez
 * changed "struct date" to "struct css_date" (to avoid conficts with Antelope software)
 *
 * Revision 1.49  2011/02/01 20:59:48  dechavez
 * updated prototypes
 *
 * Revision 1.48  2011/01/25 17:50:40  dechavez
 * updated checksum.c prototypes
 *
 * Revision 1.47  2010/09/30 17:34:28  dechavez
 * added utilTcpSocket() macro
 *
 * Revision 1.46  2010/09/10 22:42:22  dechavez
 * added utilCRC prototype
 *
 * Revision 1.45  2010/03/22 21:35:56  dechavez
 * updated prototypes (utilFork)
 *
 * Revision 1.44  2009/11/03 22:18:50  dechavez
 * updated prototypes (fletcher.c)
 *
 * Revision 1.43  2009/09/28 18:03:31  dechavez
 * added utilTrimNewline() prototype
 *
 * Revision 1.42  2009/09/28 17:29:50  dechavez
 * added new function prototoypes
 *
 * Revision 1.41  2009/03/17 17:17:16  dechavez
 * updated lock.c prototypes
 *
 * Revision 1.40  2008/10/09 20:35:20  dechavez
 * updated prototypes
 *
 * Revision 1.39  2008/03/14 06:15:53  dechavez
 * updated lock.c prototypes
 *
 * Revision 1.38  2007/12/20 22:44:31  dechavez
 * added various time interval constants
 *
 * Revision 1.37  2007/10/31 17:18:40  dechavez
 * updated prototypes
 *
 * Revision 1.36  2007/09/14 19:21:41  dechavez
 * updated prototypes
 *
 * Revision 1.35  2007/02/20 02:17:20  dechavez
 * aap (2007-02-19)
 *
 * Revision 1.34  2007/01/07 17:35:39  dechavez
 * removed unused util_isfloat prototype
 *
 * Revision 1.33  2007/01/04 23:31:21  dechavez
 * Changes to accomodate OpenBSD builds
 *
 * Revision 1.32  2007/01/04 18:00:01  dechavez
 * added utilNsToS() macro and some Windows/Unix macros
 *
 * Revision 1.31  2006/12/12 22:34:57  dechavez
 * added io.c and dir.c prototypes
 *
 * Revision 1.30  2006/12/06 22:34:31  dechavez
 * updated prototypes
 *
 * Revision 1.29  2006/11/10 05:46:09  dechavez
 * made time conversion macros UINT64
 * added util_llswap macro
 *
 * Revision 1.28  2006/06/26 23:51:02  dechavez
 * updated prototypes
 *
 * Revision 1.27  2006/06/26 22:21:08  dechavez
 * updated prototypes
 *
 * Revision 1.26  2006/05/16 00:06:07  dechavez
 * updated prototypes
 *
 * Revision 1.25  2006/05/04 20:33:53  dechavez
 * updated prototypes
 *
 * Revision 1.24  2006/04/27 19:56:26  dechavez
 * added NANOSEC_PER_MSEC, UTIL_TIMER, and various timer.c prototypes
 *
 * Revision 1.23  2006/02/14 17:05:04  dechavez
 * Change LIST to LNKLIST to avoid name clash with third party code
 *
 * Revision 1.22  2005/09/30 18:05:05  dechavez
 * updated prototypes
 *
 * Revision 1.21  2005/08/26 18:10:01  dechavez
 * updated prototypes
 *
 * Revision 1.20  2005/07/06 15:36:20  dechavez
 * 2.9.1
 *
 * Revision 1.19  2005/06/30 01:20:00  dechavez
 * added INT64 time stamp support
 *
 * Revision 1.18  2005/05/27 00:14:08  dechavez
 * updated prototypes
 *
 * Revision 1.17  2005/05/25 00:26:06  dechavez
 * updated prototypes
 *
 * Revision 1.16  2005/05/13 17:20:36  dechavez
 * added mmap functions
 *
 * Revision 1.15  2005/02/07 19:09:58  dechavez
 * added BufferedStream support
 *
 * Revision 1.14  2004/09/28 22:32:13  dechavez
 * redefine year_secs macro to conform to new behavior of util_ydhmsmtod
 *
 * Revision 1.13  2004/08/19 19:42:22  dechavez
 * redfine util_noblock as utilSetNonBlockingSocket()
 *
 * Revision 1.12  2004/07/26 23:33:37  dechavez
 * added util_htoe and util_etoh prototypes
 *
 * Revision 1.11  2004/06/25 18:34:57  dechavez
 * C++ compatibility
 *
 * Revision 1.10  2003/12/10 05:35:26  dechavez
 * added includes and prototypes to calm the Solaris cc compiler
 *
 * Revision 1.9  2003/11/21 19:38:04  dechavez
 * removed fnmatch.h
 *
 * Revision 1.8  2003/11/19 23:27:54  dechavez
 * updated prototypes, made util_lswap and util_sswap macros
 *
 * Revision 1.7  2003/11/13 19:27:27  dechavez
 * removed uneeded includes and updated prototypes
 *
 * Revision 1.6  2003/11/03 23:09:35  dechavez
 * updated utilCloseSocket() prototype
 *
 * Revision 1.5  2003/10/16 17:43:04  dechavez
 * updated function prototypes
 *
 * Revision 1.4  2003/06/09 23:42:09  dechavez
 * Begining to merge in ESSW functions
 *
 * Revision 1.3  2001/05/07 23:06:36  dec
 * util_chksum() prototype added
 *
 * Revision 1.2  2001/05/07 22:30:28  dec
 * updated prototypes
 *
 * Revision 1.1.1.1  2000/02/08 20:20:22  dec
 * import existing IDA/NRTS sources
 *
 */
