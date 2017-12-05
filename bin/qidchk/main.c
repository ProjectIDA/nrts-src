#pragma ident "$Id: main.c,v 1.6 2016/08/04 22:34:48 dechavez Exp $"
/*======================================================================
 *
 *  Update lookup table with Q330 identifiers
 *
 *====================================================================*/
#define INCLUDE_ISI_STATIC_SEQNOS
#include "q330.h"
#include "qdplus.h"
#include "zlib.h"

extern char *VersionIdentString;
BOOL Verbose = FALSE;

#define TSTRING_LEN 18
typedef struct {
    char tstring[TSTRING_LEN];
    char name[MAXPATHLEN+1];
    int ptag;
    UINT64 serialno;
    double sys;
    double sp;
    QDP_TYPE_C1_FIX fix;
} ENTRY;

LNKLST *list = NULL;
char *path = NULL;
char *ident = NULL;

static void PrintTable()
{
FILE *fp;
ENTRY *entry;
LNKLST_NODE *crnt;
static char *hdr = "#   Time Stamp     Q330 Tag#   Serial Number    Sys  Slv";

    if ((fp = fopen(path, "w")) == NULL) {
        perror(path);
        exit(1);
    }
    fprintf(fp, "%s\n", hdr);

    crnt = listFirstNode(list);
    while (crnt != NULL) {
        entry = (ENTRY *) crnt->payload;
        fprintf(fp, "%s", entry->tstring);
        fprintf(fp, "%6s", entry->name);
        fprintf(fp, " %4lu", entry->ptag);
        fprintf(fp, " %016llX", entry->serialno);
        fprintf(fp, " %.3f", entry->sys);
        fprintf(fp, " %.2f", entry->sp);
        fprintf(fp, "\n");
        crnt = listNextNode(crnt);
    }
    fclose(fp);
}

static void ReadTable()
{
#define COMMENT    '#'
#define DELIMITERS " "
#define QUOTE      '"'
#define MAXLINELEN 80
#define MAX_TOKEN  6
char *token[MAX_TOKEN];
char input[MAXLINELEN+1];
int status, ntoken;
ENTRY new;
FILE *fp;

    if ((list = listCreate()) == NULL) {
        perror("listCreate");
        exit(1);
    }

    if ((fp = fopen(path, "r")) == NULL) return; /* nothing to read */
    while ((status = utilGetLine(fp, input, MAXLINELEN, COMMENT, NULL)) == 0) {
        ntoken = utilParse(input, token, DELIMITERS, MAX_TOKEN, QUOTE);
        if (ntoken != MAX_TOKEN) {
            printf("illegal line '%s' in file '%s', ignored\n", input, path);
        } else {
            strlcpy(new.tstring, token[0], TSTRING_LEN);
            strlcpy(new.name, token[1], MAXPATHLEN+1);
            new.ptag = atoi(token[2]);
            new.serialno = (UINT64) strtoll(token[3], NULL, 16);
            new.sys = atof(token[4]);
            new.sp = atof(token[5]);
            if (!listAppend(list, &new, sizeof(ENTRY))) {
                perror("listApppend");
                exit(1);
            }
        }
    }
    fclose(fp);
}

static BOOL AddUniqueEntry(ENTRY *new)
{
ENTRY *crnt;
LNKLST_NODE *last;

    last = listLastNode(list);
    if (last != NULL) {
        crnt = (ENTRY *) last->payload;
        if (crnt->serialno == new->serialno) return;
    }
    if (!listAppend(list, new, sizeof(ENTRY))) {
        perror("listAppend");
        exit(1);
    }
}

static void help(char *myname)
{
    fprintf(stderr, "usage: %s ident output\n", myname);
    exit(1);
}

int main(int argc, char **argv)
{
int i, errcode;
QDP *qdp;
QDP_PKT pkt;
QDP_TYPE_C1_FIX fix;
Q330_CFG *cfg;
Q330_ADDR addr;
ENTRY entry;

    if (argc != 3) help(argv[0]);
    ident = argv[1];
    path  = argv[2];

    ReadTable(path);

    if ((cfg =  q330ReadCfg(NULL, &errcode)) == NULL) {
        q330PrintErrcode(stderr, "q330ReadCfg: ", NULL, errcode);
        exit(1);
    }

    if (!q330GetAddr(ident, cfg, &addr)) {
        perror("q330GetAddr");
        exit(1);
    }

    if ((qdp = q330Register(&addr, QDP_CFG_PORT, QDP_TERSE, NULL, NULL, NULL)) == NULL) {
        ("q330Register");
        exit(1);
    }

    if (!qdp_C1_RQFIX(qdp, &fix)) {
        perror("qdp_C1_RQFIX");
        exit(1);
    }

    qdpDeregister(qdp, TRUE);

    utilTimeString(utilTimeStamp(), 100, entry.tstring, TSTRING_LEN);
    strlcpy(entry.name, ident, MAXPATHLEN+1);    
    entry.ptag = fix.proper_tag;
    entry.serialno = fix.sys_num;
    entry.sys = (double) fix.sys_ver.major + ((double) fix.sys_ver.minor / 1000.0);
    entry.sp =  (double) fix.sp_ver.major  + ((double) fix.sp_ver.minor / 100.0);

    AddUniqueEntry(&entry);

    PrintTable(path);
    exit(0);
}

/*-----------------------------------------------------------------------+
 |                                                                       |
 | Copyright (C) 2009 Regents of the University of California            |
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
 * $Log: main.c,v $
 * Revision 1.6  2016/08/04 22:34:48  dechavez
 * libqdp 3.13.1 and libq330 3.7.1 compatibility changes (basically adding regerr to q330Register() calls)
 *
 * Revision 1.5  2010/12/21 20:14:31  dechavez
 * use q330PrintErrcode() for more informative q330ReadCfg() failures
 *
 * Revision 1.4  2010/12/17 19:47:15  dechavez
 * support libq330 2.0.0 Q330 configuration database and structure formats
 *
 * Revision 1.3  2010/11/24 22:04:51  dechavez
 * q330ReadCfg with error code
 *
 * Revision 1.2  2010/03/22 21:55:57  dechavez
 * added errcode to qdpConnect
 *
 * Revision 1.1  2009/10/02 19:48:51  dechavez
 * initial release
 *
 */
