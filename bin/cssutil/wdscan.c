#pragma ident "$Id: wdscan.c,v 1.2 2014/04/18 21:22:19 dechavez Exp $"
/*======================================================================
 *
 *  Print or reject all records in a wfdisc file which match the field
 *  values given on the command line.
 *
 *====================================================================*/
#include "cssio.h"
#include "list.h"

#define REJECT 0
#define KEEP   1

typedef struct {
    int field;
    LNKLST *list;
} ELEMENT;

static void help(char *myname)
{
int i, j;
char *field;

    fprintf(stderr, "usage: %s [ -r -nocase ] field=value[,value,...] ... < CSS_wfdisc\n", myname);
    fprintf(stderr, "\n");
    fprintf(stderr, "Options: -r      => reject all records which match the field values\n");
    fprintf(stderr, "         -nocase => ignore case when comparing text fields\n");
    fprintf(stderr, "\n");
    fprintf(stderr, "The following field names are recognized:\n");

    for (i = 0, j = 0; (field = cssioFieldString(i)) != NULL; i++) {
        fprintf(stderr, "%-8s ", field);
        if (++j % 8 == 0) fprintf(stderr, "\n");
    }
    fprintf(stderr, "\n");

    exit(0);
}

static BOOL FieldMatch(ELEMENT *element, WFDISC_C *wd)
{
char *value;
LNKLST_NODE *crnt;

    crnt = listFirstNode(element->list);
    while (crnt != NULL) {
        value = (char *) crnt->payload;
        if (cssioFieldMatch(wd, element->field, (char *) crnt->payload)) return TRUE;
        crnt = listNextNode(crnt);
    }

    return FALSE;
}

static BOOL PerfectMatch(LNKLST *list, WFDISC_C *wd)
{
ELEMENT *element;
LNKLST_NODE *crnt;

    crnt = listFirstNode(list);
    while (crnt != NULL) {
        element = (ELEMENT *) crnt->payload;
        if (!FieldMatch(element, wd)) return FALSE;
        crnt = listNextNode(crnt);
    }

    return TRUE;
}

static LNKLST *NewValueList(char *string)
{
#define MAXTOKEN 256
int i, nvalue;
char *value[MAXTOKEN];
LNKLST *list;

    if ((list = listCreate()) == NULL) {
        perror("NewValueList:listCreate");
        exit(1);
    }

    if ((nvalue = utilParse(string, value, ",", MAXTOKEN, 0)) < 0) {
        perror("NewValueList:utilParse");
        exit(2);
    }

    for (i = 0; i < nvalue; i++) {
        if (!listAppend(list, (void *) value[i], strlen(value[i])+1)) {
            perror("NewValueList:listAppend");
            exit(3);
        }
    }

    return list;
}

static void AddElement(LNKLST *list, char *string)
{
#define NUMTOKEN 2
int i, ntoken;
char *token[NUMTOKEN];
ELEMENT new;
static char badformat[MAXPATHLEN+1];

/* utilParse will clobber the input string so build the potential error message now */

    sprintf(badformat, "ERROR: unrecognized argument '%s'", string);

/* parse, assuming the string is in field=value[,value...] format */

    if ((ntoken = utilParse(string, token, "=", NUMTOKEN, 0)) < 0) {
        perror("AddElement:utilParse");
        exit(4);
    } else if (ntoken != NUMTOKEN) {
        fprintf(stderr, "%s\n", badformat);
        exit(5);
    }

/* identify the field and build the element */

    if ((new.field = cssioFieldCode(token[0])) == CSSIO_FIELD_UNKNOWN) {
        fprintf(stderr, "unrecognized field name: '%s'\n", token[0]);
        exit(6);
    }

/* add the valid value(s) for this field */

    new.list = NewValueList(token[1]);

/* and add the element to the list */

    if (!listAppend(list, (void *) &new, sizeof(ELEMENT))) {
        perror("AddElement:listAppend");
        exit(7);
    }
}

int main(int argc, char **argv)
{
LNKLST *list;
int i, action = KEEP;
int flags = CSSIO_RETAIN_CASE;
WFDISC_C wd;

    if ((list = listCreate()) == NULL) {
        perror("listCreate");
        exit(1);
    }

    if (argc < 2) help(argv[0]);

    for (i = 1; i < argc; i++) {
        if (strcasecmp(argv[i], "-r") == 0) {
            action = REJECT;
        } else if (strcasecmp(argv[i], "-nocase") == 0) {
            flags = CSSIO_UCASE;
        } else if (strcasecmp(argv[i], "-h") == 0 || strcasecmp(argv[i], "-help") == 0) {
            help(argv[0]);
        } else {
            AddElement(list, argv[i]);
        }
    }

    if (action == KEEP) {
        while (cssioReadWfdiscC(stdin, &wd, flags) == 0) {
            if (PerfectMatch(list, &wd)) printf("%s", wd.string);
        }
    } else {
        while (cssioReadWfdiscC(stdin, &wd, flags) == 0) {
            if (!PerfectMatch(list, &wd)) printf("%s", wd.string);
        }
    }

    exit(0);
}

/* Revision History
 *
 * $Log: wdscan.c,v $
 * Revision 1.2  2014/04/18 21:22:19  dechavez
 * removed double space bug
 *
 * Revision 1.1  2014/04/15 20:59:51  dechavez
 * initial release
 *
 */
