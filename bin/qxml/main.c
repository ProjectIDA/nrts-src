#pragma ident "$Id: main.c,v 1.2 2016/01/21 20:53:27 dechavez Exp $"
/*==============================================================================
 *  
 *  Test harness for QDP XML I/O
 *
 *============================================================================*/
#include <qdp.h>
#include <qdp/xml.h>

extern char *VersionIdentString;

static void help(char *myname)
{
    fprintf(stderr, "usage: %s [ input [ output ] ]\n", myname);
    exit(1);
}

static void ListContents(QDP_TYPE_FULL_CONFIG *config)
{
int i;

    if (config->set & QDP_CONFIG_DEFINED_WRITER) printf("QDP_CONFIG_DEFINED_WRITER\n");
    if (config->set & QDP_CONFIG_DEFINED_GLOBAL) printf("QDP_CONFIG_DEFINED_GLOBAL\n");
    if (config->set & QDP_CONFIG_DEFINED_INTERFACES) printf("QDP_CONFIG_DEFINED_INTERFACES\n");
    for (i = 0; i < QDP_NLP; i++) if (config->dport[i].set & QDP_DPORT_DEFINED_LOG) printf("QDP_DPORT_DEFINED_LOG %d\n", i);
    if (config->set & QDP_CONFIG_DEFINED_SENSCTRL  ) printf("QDP_CONFIG_DEFINED_SENSCTRL\n");
    if (config->set & QDP_CONFIG_DEFINED_SLAVE     ) printf("QDP_CONFIG_DEFINED_SLAVE\n");
    if (config->set & QDP_CONFIG_DEFINED_ADVSER1   ) printf("QDP_CONFIG_DEFINED_ADVSER1\n");
    if (config->set & QDP_CONFIG_DEFINED_ADVSER2   ) printf("QDP_CONFIG_DEFINED_ADVSER2\n");
    if (config->set & QDP_CONFIG_DEFINED_ADVETHER  ) printf("QDP_CONFIG_DEFINED_ADVETHER\n");
    if (config->set & QDP_CONFIG_DEFINED_GPS       ) printf("QDP_CONFIG_DEFINED_GPS\n");
    if (config->set & QDP_CONFIG_DEFINED_AUTOMASS  ) printf("QDP_CONFIG_DEFINED_AUTOMASS\n");
    if (config->set & QDP_CONFIG_DEFINED_ANNOUNCE  ) printf("QDP_CONFIG_DEFINED_ANNOUNCE\n");
    if (config->set & QDP_CONFIG_DEFINED_EPCFG     ) printf("QDP_CONFIG_DEFINED_EPCFG\n");
    for (i = 0; i < QDP_NLP; i++) if (config->dport[i].set & QDP_DPORT_DEFINED_TOKEN) printf("QDP_DPORT_DEFINED_TOKEN %d\n", i);
    if (config->set & QDP_CONFIG_DEFINED_WEBPAGE   ) printf("QDP_CONFIG_DEFINED_WEBPAGE\n");
}

int main(int argc, char **argv)
{
int i;
MYXML_HANDLE *xp;
char *iname = NULL, *oname = NULL;
QDP_TYPE_FULL_CONFIG config;
    
    for (i = 1; i < argc; i++) {
        if (iname == NULL) {
            iname = argv[i];
        } else if (oname == NULL) {
            oname = argv[i];
        } else {
            fprintf(stderr, "%s: unexpected argument '%s'\n", argv[0], argv[i]);
            help(argv[0]);
        }
    }

    if ((xp = XmlOpen(oname)) == NULL) {
        fprintf(stderr, "XmlOpen: ");
        perror(oname == NULL ? "stdout" : oname);
        exit(1);
    }

    if (!qdpXmlConfigFromFile(iname, &config, stderr)) {
        fprintf(stderr, "qdpXmlConfigFromFile: ");
        perror(iname == NULL ? "stdin" : iname);
        exit(1);
    }

    qdpXmlPrintFullConfig(xp, &config, NULL);
}

/* Revision History
 *
 * $Log: main.c,v $
 * Revision 1.2  2016/01/21 20:53:27  dechavez
 * fixed == instead of = typo
 *
 * Revision 1.1  2016/01/21 19:42:26  dechavez
 * initial release
 *
 */
