#pragma ident "$Id: myip.c,v 1.2 2014/04/14 17:59:29 dechavez Exp $"
/*======================================================================
 * 
 * Determine my IP address.  Very kludgy.
 *
 * Result is in network byte order (regardless of host architecture)
 *
 *====================================================================*/
#include "util.h"

UINT32 utilMyIpAddr(void)
{
#define TMPBUFLEN 1024
char tmp[TMPBUFLEN];
struct hostent *hp;
struct sockaddr_in myaddress;
UINT32 result;

    if (gethostname(tmp, TMPBUFLEN) != 0) return 0;

    hp = gethostbyname(tmp);
    if (hp == (struct hostent *) NULL) return 0;
    memcpy((void *) &myaddress.sin_addr, hp->h_addr, hp->h_length);
    myaddress.sin_family = AF_INET;

    result = (UINT32) inet_addr(inet_ntoa(myaddress.sin_addr));
#ifdef LTL_ENDIAN_HOST
	utilPackUINT32((UINT8 *) &result, result);
#endif /* LTL_ENDIAN_HOST */

    return result;
}

/* Revision History
 *
 * $Log: myip.c,v $
 * Revision 1.2  2014/04/14 17:59:29  dechavez
 * return result in network byte order
 *
 * Revision 1.1  2006/05/04 20:36:52  dechavez
 * initial release
 *
 */
