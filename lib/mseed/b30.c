#pragma ident "$Id: b30.c,v 1.4 2015/08/24 19:01:39 dechavez Exp $"
/*======================================================================
 * 
 * Blockette 30 - Data Format Dictionary Blockette
 *
 *====================================================================*/
#include "mseed.h"

static BOOL AddBlockette30(LNKLST *list, int code, int family, char *ddl, char *desc)
{
MSEED_B30 new;

    new.code = code;
    new.family = family;
    strncpy(new.ddl.text, ddl, MSEED_B30_KEYS_LEN);
    strncpy(new.desc, desc, MSEED_B30_DESC_LEN);

// If and when mseed.h::MSEED_B30 is changed to include ddl.list, add something here
// to do that:
//    if ((new.ddl.list = CreateDDLTokenList(new.ddl.text)) == NULL) return FALSE;


    if (!listAppend(list, &new, sizeof(MSEED_B30))) return FALSE;

    return TRUE;
}

LNKLST *mseedDestroyBlockette30List(LNKLST *list)
{
LNKLST_NODE *crnt;
MSEED_B30 *b30;

    if (list == NULL) return NULL;

// uncomment this when mseed.h::MSEED_B30 is changed to actually include ddl.list
//
//    crnt = listFirstNode(list);
//    while (crnt != NULL) {
//        b30 = (MSEED_B30 *) crnt->payload;
//        listDestroy(b30->ddl.list);
//    }

    listDestroy(list);
    return NULL;
}

LNKLST *mseedDefaultBlockette30List(void)
{
LNKLST *list;

    if ((list = listCreate()) == NULL) return NULL;

    if (!AddBlockette30(list, MSEED_B30_INT16_CODE, MSEED_B30_FAMILY_INTEGER, MSEED_B30_INT16_DDL, MSEED_B30_INT16_DESC)) {
        mseedDestroyBlockette30List(list);
        return NULL;
    }

    if (!AddBlockette30(list, MSEED_B30_INT32_CODE, MSEED_B30_FAMILY_INTEGER, MSEED_B30_INT32_DDL, MSEED_B30_INT32_DESC)) {
        mseedDestroyBlockette30List(list);
        return NULL;
    }

    if (!AddBlockette30(list, MSEED_B30_STEIM1_CODE, MSEED_B30_FAMILY_COMPRESSED, MSEED_B30_STEIM1_DDL, MSEED_B30_STEIM1_DESC)) {
        mseedDestroyBlockette30List(list);
        return NULL;
    }

    if (!AddBlockette30(list, MSEED_B30_STEIM2_CODE, MSEED_B30_FAMILY_COMPRESSED, MSEED_B30_STEIM2_DDL, MSEED_B30_STEIM2_DESC)) {
        mseedDestroyBlockette30List(list);
        return NULL;
    }

#ifdef MSEED_FLOAT_FAMILY_SUPPORTED
    if (!AddBlockette30(list, MSEED_B30_REAL32_CODE, MSEED_B30_FAMILY_IEEEFLOAT, MSEED_B30_REAL32_DDL, MSEED_B30_REAL32_DESC)) {
        mseedDestroyBlockette30List(list);
        return NULL;
    }

    if (!AddBlockette30(list, MSEED_B30_REAL64_CODE, MSEED_B30_FAMILY_IEEEFLOAT, MSEED_B30_REAL64_DDL, MSEED_B30_REAL64_DESC)) {
        mseedDestroyBlockette30List(list);
        return NULL;
    }
#endif /* MSEED_FLOAT_FAMILY_SUPPORTED */

    return list;
}

BOOL mseedWriteBlockette30(FILE *fp, MSEED_B30 *b30)
{
#define TILDE '~'
int i, len, nkeys;

    len = 3                       /* type */
        + 4                       /* length */
        + strlen(b30->desc)       /* variable length description */
        + 1                       /* tilde */
        + 4                       /* code */
        + 3                       /* family */
        + 2                       /* number of DDL keys */
        + strlen(b30->ddl.text)   /* pre-delimited string of DDL keys */
        ;

    for (i=0, nkeys=0; i < strlen(b30->ddl.text); i++) if (b30->ddl.text[i] == TILDE) ++nkeys;

    fprintf(fp, "030");
    fprintf(fp, "%4d", len);
    fprintf(fp, "%s~", b30->desc);
    fprintf(fp, "%4d", b30->code);
    fprintf(fp, "%3d", b30->family);
    fprintf(fp, "%2d", nkeys);
    fprintf(fp, "%s", b30->ddl.text);
    fprintf(fp, "\n");

    return TRUE;
}

BOOL mseedWriteBlockette30List(FILE *fp, LNKLST *list)
{
LNKLST_NODE *crnt;

    if (fp == NULL || list == NULL) {
        errno = EINVAL;
        return FALSE;
    }

    crnt = listFirstNode(list);
    while (crnt != NULL) {
        mseedWriteBlockette30(fp, (MSEED_B30 *) crnt->payload);
        crnt = listNextNode(crnt);
    }

    return TRUE;
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
 * $Log: b30.c,v $
 * Revision 1.4  2015/08/24 19:01:39  dechavez
 * ifdef out MSEED_B30_FAMILY_IEEEFLOAT from mseedDefaultBlockette30List()
 *
 * Revision 1.3  2015/07/16 19:49:14  dechavez
 * fixed extra tilde problem
 *
 * Revision 1.2  2015/07/15 17:01:40  dechavez
 * remove leading zeros on integers to improve readability
 *
 * Revision 1.1  2014/10/29 21:27:12  dechavez
 * created
 *
 */
