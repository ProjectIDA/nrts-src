#pragma ident "$Id: meta.c,v 1.1 2010/09/10 22:56:31 dechavez Exp $"
/*======================================================================
 *
 *  Meta-data managment.  We need to save the sequence numbers of the
 *  most recent packet received from each remote site in order to know
 *  where to pick up on reconnects.
 *
 *====================================================================*/
#define INCLUDE_ISI_STATIC_SEQNOS
#include "isimerge.h"

#define MY_MOD_ID ISIMERGE_MOD_META

typedef struct {
    ISI_SEQNO last;
    UINT32 crc;
} METADATA;

BOOL SetMetaStartSeqno(SITE_PAR *site)
{
int len;
METADATA data, empty;
UINT32 crc;
static char *fid = "SetMetaStartSeqno";

    fseek(site->mfp, 0, SEEK_END);
    len = ftell(site->mfp);
    if (len == 0) {
        site->next = ISI_OLDEST_SEQNO;
        return TRUE;
    }

    if (len != sizeof(METADATA)) {
        fprintf(stderr, "%s: corrupt meta file: len=%d != %d\n", fid, site->mpath, len, sizeof(METADATA));
        return FALSE;
    }

    rewind(site->mfp);
    if (fread(&data, 1, sizeof(METADATA), site->mfp) != sizeof(METADATA)) {
        fprintf(stderr, "%s: ERROR: fread:", fid);
        perror(site->mpath);
        return FALSE;
    }

    crc = data.crc;
    data.crc = 0;

    if (utilCRC((UINT8 *) &data, sizeof(METADATA)) != crc) {
        fprintf(stderr, "%s: corrupt meta file: %s: CRC error\n", fid, site->mpath);
        return FALSE;
    }
    data.crc = crc;

    memset(&empty, 0, sizeof(METADATA));
    if (memcmp(&data, &empty, sizeof(METADATA)) == 0) {
        fprintf(stderr, "%s: corrupt meta file: %s: all zeros\n", fid, site->mpath);
        return FALSE;
    }

    site->last = data.last;
    site->next = data.last; isiIncrSeqno(&site->next);

    return TRUE;
}

void SaveLastSeqno(SITE_PAR *site)
{
METADATA data;
static char *fid = "SaveLastSeqno";

    rewind(site->mfp);
    data.last = site->last;
    data.crc = 0;
    data.crc = utilCRC((UINT8 *) &data, sizeof(METADATA));
    errno = 0;
    if (fwrite(&data, 1, sizeof(METADATA), site->mfp) != sizeof(METADATA)) {
        LogMsg(LOG_ERR, "*** FATAL ERROR *** %s: fwrite: %s: %s", fid, site->mpath, strerror(errno));
        Abort(MY_MOD_ID + 1);
    }

    site->next = site->last; isiIncrSeqno(&site->next); /* in case we need to reconnect */
}

/* Revision History
 *
 * $Log: meta.c,v $
 * Revision 1.1  2010/09/10 22:56:31  dechavez
 * initial release
 *
 */
