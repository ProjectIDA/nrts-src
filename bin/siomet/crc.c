#pragma ident "$Id: crc.c,v 1.1 2017/09/28 18:20:44 dauerbach Exp $"
/*======================================================================
 * 
 * CRC utilities (checksum code courtesy of Alan Nance)
 *
 *====================================================================*/
#include "siomet.h"

static void crc8_add(UINT8 *crc, UINT8 new_data)
{
UINT8 i;
#define CRC8_POLY 0x07

    *crc ^= new_data;

    i = 8;
    do {
        if (*crc & 0x80) {
            *crc <<= 1;
            *crc ^= CRC8_POLY;
        } else {
            *crc <<= 1;
        }
    } while(--i);
}

BOOL ChecksumOK(UINT8 *pmsg)
{
UINT16 i, len;
UINT8 crc = 0x00, expected;

    len = strlen(pmsg) - 2;
    for(i = 0 ; i < len ; i ++) crc8_add(&crc, *pmsg++);

    expected = (UINT8) strtol(pmsg, NULL, 16);

    return crc == expected ? TRUE : FALSE;
}

/* Revision History
 *
 * $Log: crc.c,v $
 * Revision 1.1  2017/09/28 18:20:44  dauerbach
 * initial release
 *
 */
