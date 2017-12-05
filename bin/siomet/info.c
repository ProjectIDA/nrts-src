#pragma ident "$Id: info.c,v 1.2 2017/09/29 16:15:02 dechavez Exp $"
/*======================================================================
 *
 * Process the device/channel specification string
 *
 *====================================================================*/
#include "siomet.h"

static SIOMET_INFO Info;

SIOMET_INFO *ParseInfoString(char *string)
{
int i;
int ntoken;
#define NUMTOKEN 6
#define DELIMITERS ":"
char *token[NUMTOKEN];
char copy[MAXPATHLEN+1];

    strncpy(copy, string, MAXPATHLEN);
    if ((ntoken = utilParse(copy, token, DELIMITERS, NUMTOKEN, 0)) != NUMTOKEN) return NULL;

    strcpy(Info.device, token[0]);
    if ((Info.speed = atoi(token[1])) <= 0) {
        fprintf(stderr, "ERROR: illegal baud rate '%s'\n", token[1]);
        return NULL;
    }
    if ((Info.sint = atoi(token[2])) <= 0) {
        fprintf(stderr, "ERROR: illegal sample interval '%s'\n", token[2]);
        return NULL;
    }
    if (Info.sint < SIOMET_MIN_SINT) {
        fprintf(stderr, "ERROR: minimum sample interval is %d msec\n", SIOMET_MIN_SINT);
        return NULL;
    }

    Info.nsint = Info.sint * NANOSEC_PER_MSEC;
    strncpy(Info.ta, token[3], MAXPATHLEN);
    strncpy(Info.ua, token[4], MAXPATHLEN);
    strncpy(Info.pa, token[5], MAXPATHLEN);

    if (strcmp(Info.ta, Info.ua) == 0 || strcmp(Info.ta, Info.pa) == 0 || strcmp(Info.ua, Info.pa) == 0) {
        fprintf(stderr, "ERROR: duplicate channel names!\n");
        return NULL;
    }
    return &Info;
}

/*-----------------------------------------------------------------------+
 |                                                                       |
 | Copyright (C) 2017 Regents of the University of California            |
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
 * $Log: info.c,v $
 * Revision 1.2  2017/09/29 16:15:02  dechavez
 * check for duplicate channel names
 *
 * Revision 1.1  2017/09/28 18:20:44  dauerbach
 * initial release
 *
 */
