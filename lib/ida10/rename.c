#pragma ident "$Id: rename.c,v 1.3 2015/12/04 22:05:01 dechavez Exp $"
/*======================================================================
 *
 *  Rename things
 *
 * - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 * Copyright (c) 2012 Regents of the University of California.
 * All rights reserved.
 *====================================================================*/
#include "ida10.h"

void ida10PrintChannelMap(LNKLST *map)
{
IDA10_NAME_MAP *entry;
LNKLST_NODE *crnt;

    crnt = listFirstNode(map);
    while (crnt != NULL) {
        entry = (IDA10_NAME_MAP *) crnt->payload;
        printf("old='%s' new='%s'\n", entry->old, entry->new);
        crnt = listNextNode(crnt);
    }
}

static BOOL OneToOne(LNKLST *map)
{
IDA10_NAME_MAP *e1, *e2;
LNKLST_NODE *c1, *c2;

    c1 = listFirstNode(map);
    while (c1 != NULL) {
        e1 = (IDA10_NAME_MAP *) c1->payload;
        c2 = listFirstNode(map);
        while (c2 != NULL) {
            if (c1 != c2) {
                e2 = (IDA10_NAME_MAP *) c2->payload;
                if (strcmp(e1->new, e2->new) == 0) return FALSE;
                if (strcmp(e1->old, e2->old) == 0) return FALSE;
            }
            c2 = listNextNode(c2);
        }
        c1 = listNextNode(c1);
    }
    return TRUE;
}

static char *NewCname(char *cname, LNKLST *map)
{
IDA10_NAME_MAP *entry;
LNKLST_NODE *crnt;

    if (cname == NULL || map == NULL) return NULL;

    crnt = listFirstNode(map);
    while (crnt != NULL) {
        entry = (IDA10_NAME_MAP *) crnt->payload;
        if (strcmp(entry->old, cname) == 0) return entry->new;
        crnt = listNextNode(crnt);
    }

    return NULL;
}

char *ida10Cname(char *cname, LNKLST *map)
{
char *new;

    if ((new = NewCname(cname, map)) != NULL) {
        memcpy(cname, new, IDA10_CNAMLEN);
        cname[IDA10_CNAMLEN] = 0;
    }

    return cname;
}

/* Update the raw packet with new channel name */

int ida10ReplaceCname(UINT8 *payload, LNKLST *map)
{
#define CNAME_LEN    6
#define CNAME_OFFSET 50 /* All IDA10 TS packets have channel name here */
UINT8 *ptr;
char old[CNAME_LEN+1], *new;

    if (payload == NULL || map == NULL) return 0;

    if (ida10Type(payload) != IDA10_TYPE_TS) return 0;
    ptr = payload + CNAME_OFFSET;

    memcpy(old, ptr, CNAME_LEN); old[CNAME_LEN] = 0;
    if ((new = NewCname(old, map)) == NULL) return 0;

    memset(ptr, 0, CNAME_LEN);
    memcpy(ptr, new, CNAME_LEN);
    return 1;
}

/* Update the raw packet with new station name */

int ida10ReplaceSname(UINT8 *payload, char *new)
{
#define SNAME_LEN    4
#define SNAME_OFFSET 4 /* All packets with station code have it here */
UINT8 *ptr;
int subformat;

    if (payload == NULL || new == NULL) return 0;
    if (ida10Type(payload) != IDA10_TYPE_TS) return 0;

    subformat = ida10SubFormatCode(payload);
    if (subformat != IDA10_SUBFORMAT_5 && subformat != IDA10_SUBFORMAT_8) return 0;

    ptr = payload + SNAME_OFFSET;
    memcpy(ptr, new, SNAME_LEN);
    ptr[SNAME_LEN] = 0;
    return 1;
}

LNKLST *ida10BuildChannelMap(char *string)
{
int ntoken;
LNKLST_NODE *crnt;
LNKLST *map, *pairlist;
char *token[2];
IDA10_NAME_MAP entry;

    if (string == NULL) {
        errno = EINVAL;
        return NULL;
    }

    if ((map = listCreate()) == NULL) return NULL;

/* Split the input string into a list of ,: delimited old=new pairs */

    if ((pairlist = utilStringTokenList(string, ",:", 0)) == NULL) return NULL;

/* Walk the list of pairs, building the channel map */

    crnt = listFirstNode(pairlist);
    while (crnt != NULL) {
        ntoken = utilParse((char *) crnt->payload, token, "=", 2, 0);
        if (ntoken != 2) {
            errno = EINVAL;
            listDestroy(pairlist);
            return NULL;
        }
        if ((entry.old = strdup(token[0])) == NULL) {
            listDestroy(pairlist);
            return NULL;
        }
        if ((entry.new = strdup(token[1])) == NULL) {
            listDestroy(pairlist);
            return NULL;
        }
        if (!listAppend(map, &entry, sizeof(IDA10_NAME_MAP))) {
            listDestroy(pairlist);
            return NULL;
        }
        crnt = listNextNode(crnt);
    }
    listDestroy(pairlist);

/* Do not allow multiple entries */

    if (!OneToOne(map)) {
        listDestroy(map);
        map = NULL;
    }

    return map;
}

/* Revision History
 *
 * $Log: rename.c,v $
 * Revision 1.3  2015/12/04 22:05:01  dechavez
 * fixed == typo in assigning errno
 *
 * Revision 1.2  2014/08/08 15:34:06  dechavez
 * added ida10Cname()
 *
 * Revision 1.1  2012/12/13 21:45:51  dechavez
 * created, with ida10ReplaceCname(), ida10ReplaceSname(), ida10BuildChannelMap()
 *
 */
