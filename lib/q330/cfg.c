#pragma ident "$Id: cfg.c,v 1.34 2017/01/26 19:49:37 dechavez Exp $"
/*======================================================================
 * 
 * Q330 cfg file support
 *
 *====================================================================*/
#include "q330.h"

#define DELIMITERS " =\t"
#define MAX_TOKEN  32
#define MAXLINELEN 256
#define COMMENT    '#'
#define QUOTE      '"'

char *q330RootPath(char *root)
{
static char *DefaultRootPath = Q330_DEFAULT_ROOT;

    if (root == NULL) root = getenv(Q330_ROOT_ENV_STRING);
    if (root == NULL) root = DefaultRootPath;

    return root;
}

static LNKLST *DestroyAddr(LNKLST *list)
{
    if (list == NULL) return NULL;
    listDestroy(list);
    return NULL;
}

static LNKLST *DestroyAuth(LNKLST *list)
{
    if (list == NULL) return NULL;
    listDestroy(list);
    return NULL;
}

static LNKLST *DestroySensor(LNKLST *list)
{
    if (list == NULL) return NULL;
    listDestroy(list);
    return NULL;
}

static LNKLST *DestroyDetector(LNKLST *list)
{
    if (list == NULL) return NULL;
    listDestroy(list);
    return NULL;
}

static LNKLST *DestroyCalib(LNKLST *list)
{
    if (list == NULL) return NULL;
    listDestroy(list);
    return NULL;
}

static LNKLST *DestroyLcq(LNKLST *list)
{
Q330_LCQ *lcq;
LNKLST_NODE *crnt;

    if (list == NULL) return NULL;

    crnt = listFirstNode(list);
    while (crnt != NULL) {
        lcq = (Q330_LCQ *) crnt->payload;
        listDestroy(lcq->list);
        crnt = listNextNode(crnt);
    }
    listDestroy(list);
    return NULL;
}

Q330_CFG *q330DestroyCfg(Q330_CFG *cfg)
{
int i;

    if (cfg == NULL) return NULL;

    cfg->addr = DestroyAddr(cfg->addr);
    cfg->auth = DestroyAuth(cfg->auth);
    cfg->sensor = DestroySensor(cfg->sensor);
    cfg->detector = DestroyDetector(cfg->detector);
    cfg->calib = DestroyCalib(cfg->calib);
    cfg->lcq = DestroyLcq(cfg->lcq);

    free(cfg);

    return NULL;
}

char *q330PathName(char *root, char *name, char *buf)
{
static char mt_unsafe[MAXPATHLEN+1];

    if (buf == NULL) buf = mt_unsafe;
    root = q330RootPath(root);

    if (name == NULL || strcasecmp(name, Q330_NAME_ADDR) == 0) {
        sprintf(buf, "%s/%s", root, Q330_NAME_ADDR);
    } else {
        sprintf(buf, "%s/%s/%s", root, Q330_NAME_SUBDIR, name);
    }

    return buf;
}

static LNKLST *ListAndPath(char *path, char *root, char *name)
{
LNKLST *list;
static char *fid = "ReadCfg:CreateCfg:ListAndPath";

    q330PathName(root, name, path);
    if (!utilFileExists(path)) {
        fprintf(stderr, "missing %s\n", path);
        return NULL;
    }
    if ((list = listCreate()) == NULL) {
        fprintf(stderr, "%s: ", fid);
        perror("listCreate()");
        return NULL;
    }

    return list;
}

static Q330_CFG *CreateCfg(char *root)
{
Q330_CFG *cfg;

    if ((cfg = (Q330_CFG *) malloc(sizeof(Q330_CFG))) == NULL) return NULL;
    strlcpy(cfg->root, root, MAXPATHLEN+1);

    cfg->addr     = NULL;
    cfg->auth     = NULL;
    cfg->sensor   = NULL;
    cfg->detector = NULL;
    cfg->calib    = NULL;
    cfg->lcq      = NULL;

    if ((cfg->addr     = ListAndPath(cfg->path.addr,     root, NULL              )) == NULL) return q330DestroyCfg(cfg);
    if ((cfg->auth     = ListAndPath(cfg->path.auth,     root, Q330_NAME_AUTH    )) == NULL) return q330DestroyCfg(cfg);
    if ((cfg->sensor   = ListAndPath(cfg->path.sensor,   root, Q330_NAME_SENSOR  )) == NULL) return q330DestroyCfg(cfg);
    if ((cfg->detector = ListAndPath(cfg->path.detector, root, Q330_NAME_DETECTOR)) == NULL) return q330DestroyCfg(cfg);
    if ((cfg->calib    = ListAndPath(cfg->path.calib,    root, Q330_NAME_CALIB   )) == NULL) return q330DestroyCfg(cfg);
    if ((cfg->lcq      = ListAndPath(cfg->path.lcq,      root, Q330_NAME_LCQ     )) == NULL) return q330DestroyCfg(cfg);

    return cfg;
}

static BOOL ReadCfgCleanAbort(FILE *fp, Q330_CFG *cfg, int unused)
{
    if (fp != NULL) fclose(fp);
    if (cfg != NULL) q330DestroyCfg(cfg);

    return FALSE;
}

BOOL q330AddAddr(Q330_CFG *cfg, Q330_ADDR *new)
{
Q330_ADDR *old;

    if ((old = q330LookupAddr(new->connect.ident, cfg)) == NULL) {
        return listAppend(cfg->addr, new, sizeof(Q330_ADDR));
    } else {
        *old = *new;
    }
    return TRUE;
}

static BOOL AddAddr(Q330_CFG *cfg, char **token, int ntoken, int instance, int *errcode)
{
int errors = 0;
Q330_ADDR addr;

    if (ntoken != 4) {
        *errcode = Q330_BAD_ADDR;
        errno = EINVAL;
        return FALSE;
    }

    strlcpy(addr.connect.ident, token[0], MAXPATHLEN+1);
    if (q330LookupAddr(addr.connect.ident, cfg) != NULL) {
        *errcode = Q330_DUP_ADDR;
        return FALSE;
    }
    if ((addr.connect.tag = (UINT32) atol(token[1])) != 0) {
        addr.connect.type = QIO_UDP;
        addr.connect.iostr[0] = 0;
    } else {
        strncpy(addr.connect.iostr, token[1], MAXPATHLEN+1);
        addr.connect.type = qioConnectionType(token[1], NULL, NULL);
    }
    if (!q330ParseComplexInputName(&addr.input.a, token[2], errcode)) {
        fprintf(stderr, "'%s' is an unrecognized sensor name\n", token[2]);
        ++errors;
    }
    if (!q330ParseComplexInputName(&addr.input.b, token[3], errcode)) {
        fprintf(stderr, "'%s' is an unrecognized sensor name\n", token[3]);
        ++errors;
    }
    if (errors) return FALSE;

    addr.watchdog = Q330_DEFAULT_WATCHDOG;
    addr.instance = instance;

    return listAppend(cfg->addr, &addr, sizeof(Q330_ADDR));
}

static BOOL ReadAddr(Q330_CFG *cfg, int *errcode)
{
FILE *fp = NULL;
int ntoken, status, instance = 0;
char *token[MAX_TOKEN];
char input[MAXLINELEN];
char ConsoleString[] = "console /dev/something none none";

    if ((fp = fopen(cfg->path.addr, "r")) == NULL) return ReadCfgCleanAbort(fp, cfg, *errcode = Q330_IOE_ADDR);

/* Insert the built in console entry first */

      ntoken = utilParse(ConsoleString, token, " \t", MAX_TOKEN, QUOTE);
      token[0][0] = 0;
      if (!AddAddr(cfg, token, ntoken, instance, errcode)) return ReadCfgCleanAbort(fp, cfg, *errcode );
      instance += 10;

/* Read the contents of the file into the list */

    while ((status = utilGetLine(fp, input, MAXLINELEN, COMMENT, NULL)) == 0) {
        ntoken = utilParse(input, token, " \t", MAX_TOKEN, QUOTE);
        if (!AddAddr(cfg, token, ntoken, instance, errcode)) return ReadCfgCleanAbort(fp, cfg, *errcode);
        instance += 10;
    }

    if (status != 1) return ReadCfgCleanAbort(fp, cfg, *errcode = Q330_IOE_ADDR);
    fclose(fp);

    return TRUE;
}

BOOL q330AddAuth(Q330_CFG *cfg, Q330_AUTH *new)
{
Q330_AUTH *old;

    if ((old = q330LookupAuth(new->tag, cfg)) == NULL) {
        return listAppend(cfg->auth, new, sizeof(Q330_ADDR));
    } else {
        *old = *new;
    }
    return TRUE;
}

static BOOL AddAuth(Q330_CFG *cfg, char **token, int ntoken, int *errcode)
{
Q330_AUTH auth;

    if (ntoken != 8) {
        errno = EINVAL;
        return FALSE;
    }

    auth.tag = (UINT32) atol(token[0]);
    if (q330LookupAuth(auth.tag, cfg) != NULL) {
        *errcode = Q330_DUP_AUTH;
        return FALSE;
    }

    auth.serialno = (UINT64) strtoll(token[1], NULL, 16);

    auth.code[QDP_CFG_PORT]       = (UINT64) atoi(token[2]);
    auth.code[QDP_SFN_PORT]       = (UINT64) atoi(token[3]);
    auth.code[QDP_LOGICAL_PORT_1] = (UINT64) atoi(token[4]);
    auth.code[QDP_LOGICAL_PORT_2] = (UINT64) atoi(token[5]);
    auth.code[QDP_LOGICAL_PORT_3] = (UINT64) atoi(token[6]);
    auth.code[QDP_LOGICAL_PORT_4] = (UINT64) atoi(token[7]);

    return listAppend(cfg->auth, &auth, sizeof(Q330_AUTH));
}

static BOOL ReadAuth(Q330_CFG *cfg, int *errcode)
{
FILE *fp = NULL;
int ntoken, status;
char *token[MAX_TOKEN];
char input[MAXLINELEN];

    if ((fp = fopen(cfg->path.auth, "r")) == NULL) return ReadCfgCleanAbort(fp, cfg, *errcode = Q330_IOE_AUTH);

/* Read the contents of the file into the list */

    while ((status = utilGetLine(fp, input, MAXLINELEN, COMMENT, NULL)) == 0) {
        ntoken = utilParse(input, token, " \t", MAX_TOKEN, QUOTE);
        if (!AddAuth(cfg, token, ntoken, errcode)) return ReadCfgCleanAbort(fp, cfg, *errcode);
    }

    if (status != 1) return ReadCfgCleanAbort(fp, cfg, *errcode = Q330_IOE_AUTH);
    fclose(fp);

    return TRUE;
}

static BOOL AddSensor(Q330_CFG *cfg, char **token, int ntoken, int *errcode)
{
int i;
BOOL ActiveHigh;
Q330_SENSOR sensor;
static int type = 0;

    if (ntoken != 5) {
        *errcode = Q330_BAD_SENSOR;
        return FALSE;
    }

    strlcpy(sensor.name, token[0], Q330_SENSOR_NAME_LEN+1);
    if (q330LookupSensor(sensor.name, cfg) != NULL) {
        *errcode = Q330_DUP_SENSOR;
        return FALSE;
    }

    ActiveHigh = strcasecmp(token[1], "hi") == 0 ? TRUE : FALSE;
    if (!q330ParseSensorControlLines('a', sensor.scA, token[2], ActiveHigh, errcode)) return FALSE;
    if (!q330ParseSensorControlLines('b', sensor.scB, token[3], ActiveHigh, errcode)) return FALSE;

    strlcpy(sensor.desc, token[4], Q330_SENSOR_DESC_LEN+1);
    if (sensor.desc[0] == '"') for (i = 0; i < strlen(sensor.desc); i++) sensor.desc[i] = sensor.desc[i+1];
    if (sensor.desc[strlen(sensor.desc)-1] == '"') sensor.desc[strlen(sensor.desc)-1] = 0;

    sensor.type = type++;
    return listAppend(cfg->sensor, &sensor, sizeof(Q330_SENSOR));
}

static BOOL ReadSensor(Q330_CFG *cfg, int *errcode)
{
FILE *fp = NULL;
int ntoken, status;
char *token[MAX_TOKEN];
char input[MAXLINELEN];

    if ((fp = fopen(cfg->path.sensor, "r")) == NULL) return ReadCfgCleanAbort(fp, cfg, *errcode = Q330_IOE_SENSOR);

/* Read the contents of the file into the list */

    while ((status = utilGetLine(fp, input, MAXLINELEN, COMMENT, NULL)) == 0) {
        ntoken = utilParse(input, token, " \t", MAX_TOKEN, QUOTE);
        if (!AddSensor(cfg, token, ntoken, errcode)) return ReadCfgCleanAbort(fp, cfg, *errcode);
    }

    if (status != 1) return ReadCfgCleanAbort(fp, cfg, *errcode = Q330_IOE_SENSOR);
    fclose(fp);

    return TRUE;
}

static BOOL AddDetector(Q330_CFG *cfg, char **token, int ntoken, int *errcode)
{
Q330_DETECTOR detector;

    if (ntoken < 6) {
        *errcode = Q330_BAD_DETECTOR;
        return FALSE;
    }

    strlcpy(detector.name, token[0], ISI_STALEN+1);
    if (q330LookupDetector(detector.name, cfg) != NULL) {
        *errcode = Q330_DUP_DETECTOR;
        return FALSE;
    }
    strlcpy(detector.channels, token[1], MAXPATHLEN+1);
    detector.votes = atol(token[2]);
    detector.pre = (UINT32) atol(token[3]);
    detector.pst = (UINT32) atol(token[4]);

    if (strcasecmp(token[5], "stalta") == 0) {
        if (ntoken != 10) {
            *errcode = Q330_BAD_DETECTOR;
            return FALSE;
        }
        detector.engine.type = DETECTOR_TYPE_STALTA;
        detector.engine.stalta.config.len.sta = (UINT32) atol(token[6]);
        detector.engine.stalta.config.len.lta = (UINT32) atol(token[7]);
        detector.engine.stalta.config.ratio.on = (REAL32) atof(token[8]);
        detector.engine.stalta.config.ratio.off = (REAL32) atof(token[9]);
        detector.enabled = TRUE;
    } else {
        *errcode = Q330_BAD_DETECTOR;
        return FALSE;
    }

    return listAppend(cfg->detector, &detector, sizeof(Q330_DETECTOR));
}

static BOOL ReadDetector(Q330_CFG *cfg, int *errcode)
{
FILE *fp = NULL;
int ntoken, status;
char *token[MAX_TOKEN];
char input[MAXLINELEN];

    if ((fp = fopen(cfg->path.detector, "r")) == NULL) return ReadCfgCleanAbort(fp, cfg, *errcode = Q330_IOE_DETECTOR);

/* Read the contents of the file into the list */

    while ((status = utilGetLine(fp, input, MAXLINELEN, COMMENT, NULL)) == 0) {
        ntoken = utilParse(input, token, " \t", MAX_TOKEN, QUOTE);
        if (!AddDetector(cfg, token, ntoken, errcode)) return ReadCfgCleanAbort(fp, cfg, *errcode);
    }
    if (status != 1) return ReadCfgCleanAbort(fp, cfg, *errcode = Q330_IOE_DETECTOR);
    fclose(fp);

    return TRUE;
}

static BOOL AddCalib(Q330_CFG *cfg, char **token, int ntoken, int *errcode)
{
int i;
Q330_CALIB calib;

    if (ntoken != 8) {
        *errcode = Q330_BAD_CALIB;
        return FALSE;
    }
    i = 0;
    strlcpy(calib.name, token[i++], ISI_STALEN+1);
    if (!q330ParseComplexInputName(&calib.input, token[i++], errcode)) return FALSE;

    calib.qcal.starttime = Q330_CAL_START_NOW;;
    calib.qcal.chans     = Q330_CAL_SHORTCUT_CHANS_BITMAP;
    calib.qcal.monitor   = Q330_CAL_SHORTCUT_MONITOR_BITMAP;

    if ((calib.qcal.waveform = (UINT16) atoi(token[i++])) > Q330_MAX_CAL_WAVEFORM) {
        *errcode = Q330_BAD_CALIB;
        return FALSE;
    }
    calib.qcal.amplitude = (UINT16) atoi(token[i++]);
    if ((calib.qcal.duration = (UINT16) atoi(token[i++])) > Q330_MAX_CAL_DURATION) {
        *errcode = Q330_BAD_CALIB;
        return FALSE;
    }
    calib.qcal.settle = (UINT16) atoi(token[i++]);
    calib.qcal.trailer = (UINT16) atoi(token[i++]);
    if ((calib.qcal.divisor = (UINT16) atoi(token[i++])) > Q330_MAX_CAL_DIVISOR) {
        *errcode = Q330_BAD_CALIB;
        return FALSE;
    }

    return listAppend(cfg->calib, &calib, sizeof(Q330_CALIB));
}

static BOOL ReadCalib(Q330_CFG *cfg, int *errcode)
{
FILE *fp = NULL;
int ntoken, status;
char *token[MAX_TOKEN];
char input[MAXLINELEN];

    if ((fp = fopen(cfg->path.calib, "r")) == NULL) return ReadCfgCleanAbort(fp, cfg, *errcode = Q330_IOE_CALIB);

/* Read the contents of the file into the list */

    while ((status = utilGetLine(fp, input, MAXLINELEN, COMMENT, NULL)) == 0) {
        ntoken = utilParse(input, token, " \t", MAX_TOKEN, QUOTE);
        if (!AddCalib(cfg, token, ntoken, errcode)) return ReadCfgCleanAbort(fp, cfg, *errcode);
    }

    if (status != 1) return ReadCfgCleanAbort(fp, cfg, *errcode = Q330_IOE_CALIB);
    fclose(fp);

    return TRUE;
}

static BOOL AddLcqEntry(Q330_LCQ *lcq, char **token)
{
int i;
Q330_LCQ_ENTRY entry;

    i = 0;
    strlcpy(entry.chn,  token[i++], QDP_CNAME_LEN+1);
    strlcpy(entry.loc,  token[i++], QDP_LNAME_LEN+1);
    entry.src[0] = (UINT8) strtol(token[i++], NULL, 16);
    entry.src[1] = (UINT8) atoi(token[i++]);
    entry.sint = atof(token[i++]);
    if (entry.sint == 0.0) {
        entry.rate = 0;
    } else {
        entry.rate = (entry.sint <= 1.0) ? (INT16) (1.0 / entry.sint) : - (INT16) entry.sint;
    }

    return listAppend(lcq->list, &entry, sizeof(Q330_LCQ_ENTRY));
}

static BOOL AddLcq(Q330_CFG *cfg, Q330_LCQ *lcq)
{
int retval;

    if (lcq->list == NULL) return TRUE;

    retval = listAppend(cfg->lcq, lcq, sizeof(Q330_LCQ));
    lcq->list = NULL; /* don't destroy since the pointer is still valid in the cfg->lcq list */

    return retval;
}

/* ReadLcq() has a different pattern than the others because it contains lists */

static BOOL ReadLcq(Q330_CFG *cfg, int *errcode)
{
FILE *fp = NULL;
int ntoken, status;
char *token[MAX_TOKEN];
char input[MAXLINELEN];
Q330_LCQ lcq;

    lcq.list = NULL;

    if ((fp = fopen(cfg->path.lcq, "r")) == NULL) return ReadCfgCleanAbort(fp, cfg, *errcode = Q330_IOE_LCQ);

/* Read the contents of the file into the list */

    while ((status = utilGetLine(fp, input, MAXLINELEN, COMMENT, NULL)) == 0) {
        ntoken = utilParse(input, token, " \t", MAX_TOKEN, QUOTE);
        switch (ntoken) {
          case 1:
            if (!AddLcq(cfg, &lcq)) return ReadCfgCleanAbort(fp, cfg, *errcode); /* flush previous */
            strlcpy(lcq.name, token[0], Q330_LCQ_NAME_LEN+1);
            if ((lcq.list = listCreate()) == NULL) return ReadCfgCleanAbort(fp, cfg, *errcode = Q330_NO_LCQMEM);
            break;
          case 5:
            if (lcq.list == NULL) {
                return ReadCfgCleanAbort(fp, cfg, *errcode = Q330_BAD_LCQ);
            } else if (!AddLcqEntry(&lcq, token)) {
                return ReadCfgCleanAbort(fp, cfg, Q330_IOE_LCQ);
            }
            break;
          default:
            return ReadCfgCleanAbort(fp, cfg, *errcode = Q330_BAD_LCQ);
        }
    }

    if (status != 1) return ReadCfgCleanAbort(fp, cfg, *errcode = Q330_IOE_LCQ);
    if (!AddLcq(cfg, &lcq)) return ReadCfgCleanAbort(fp, cfg, *errcode = Q330_IOE_LCQ);
    fclose(fp);

    return TRUE;
}

Q330_CFG *q330ReadCfg(char *root, int *errcode)
{
Q330_CFG *cfg = NULL;
Q330_ADDR *addr;
Q330_AUTH *auth;
Q330_CALIB *calib;
LNKLST_NODE *crnt;

    if (errcode == NULL) {
        errno = EINVAL;
        return NULL;
    }

/* Create the config structure and ensure the files are there */

    root = q330RootPath(root);
    if ((cfg = CreateCfg(root)) == NULL) return NULL;

/* Read each file */

    if (!ReadAddr    (cfg, errcode)) return NULL;
    if (!ReadAuth    (cfg, errcode)) return NULL;
    if (!ReadSensor  (cfg, errcode)) return NULL;
    if (!ReadDetector(cfg, errcode)) return NULL;
    if (!ReadCalib   (cfg, errcode)) return NULL;
    if (!ReadLcq     (cfg, errcode)) return NULL;

/* Complete the addr field */

    *errcode = Q330_NO_ERROR;
    crnt = listFirstNode(cfg->addr);
    while (crnt != NULL) {
        addr = (Q330_ADDR *) crnt->payload;
        if (!q330GetSensor(addr->input.a.sensor.name, cfg, &addr->input.a.sensor)) {
            fprintf(stderr, "ERROR: '%s' is not a recognized sensor name\n", addr->input.a.sensor.name);
            *errcode = Q330_BAD_INPUT;
            return q330DestroyCfg(cfg);
        }
        if (!q330GetSensor(addr->input.b.sensor.name, cfg, &addr->input.b.sensor)) {
            fprintf(stderr, "ERROR: '%s' is not a recognized sensor name\n", addr->input.b.sensor.name);
            *errcode = Q330_BAD_INPUT;
            return q330DestroyCfg(cfg);
        }
        if ((auth = q330LookupAuth(addr->connect.tag, cfg)) != NULL) {
            addr->serialno = auth->serialno;
            addr->authcode = auth->code[0]; /* FOR NOW, SINCE ALL CODES ARE THE SAME FOR IDA */
        } else {
            addr->serialno = QDP_INVALID_SERIALNO;
        }
        crnt = listNextNode(crnt);
    }

/* Complete the input sensors in calib */

    *errcode = Q330_NO_ERROR;
    crnt = listFirstNode(cfg->calib);
    while (crnt != NULL) {
        calib = (Q330_CALIB *) crnt->payload;
        if (!q330GetSensor(calib->input.sensor.name, cfg, &calib->input.sensor)) {
            *errcode = Q330_BAD_INPUT;
            return q330DestroyCfg(cfg);
        }
        crnt = listNextNode(crnt);
    }

/* All done, return the newly created structure */

    return cfg;
}

/*-----------------------------------------------------------------------+
 |                                                                       |
 | Copyright (C) 2006 Regents of the University of California            |
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
 * $Log: cfg.c,v $
 * Revision 1.34  2017/01/26 19:49:37  dechavez
 * fixed erroneous no such file error code when encountering duplicate entries
 *
 * Revision 1.33  2017/01/26 19:00:09  dechavez
 * fixed double free bug REINTRODUCED(!) in 3.7.5 (sigh)
 *
 * Revision 1.32  2017/01/24 22:03:40  dechavez
 * fixed sloppy bugs introduced in 3.7.4
 *
 * Revision 1.31  2016/12/22 20:39:53  dechavez
 * fixed double free bug when q330ReadCfg() fails
 *
 * Revision 1.30  2016/08/15 19:47:03  dechavez
 * AddAddr() sets watchdog to Q330_DEFAULT_WATCHDOG
 *
 * Revision 1.29  2016/07/20 21:56:49  dechavez
 * replaced Q330_ADDR:maxtry with watchdog (see libqdp 3.13.0 release notes)
 *
 * Revision 1.28  2016/02/11 19:08:07  dechavez
 * changed maxsrq to maxtry (to follow libqdp 3.11.3 names)
 *
 * Revision 1.27  2016/02/05 16:53:46  dechavez
 * added initialization for new Q330_ADDR maxsrq field in AddAddr()
 *
 * Revision 1.26  2015/12/04 23:18:49  dechavez
 * fixed spurious semicolon in AddSensor() affecting sensor.desc NULL termination
 *
 * Revision 1.25  2014/09/08 18:32:03  dechavez
 * added some stderr commentary when AddAddr() and q330ReadCfg() encounter problems
 *
 * Revision 1.24  2014/08/11 17:45:19  dechavez
 * added q330RootPath() (to permit use of Q330_ROOT_ENV_STRING environment variable)
 * and reworked q330PathName() and q330ReadCfg() to use same
 *
 * Revision 1.23  2012/08/06 21:36:32  dechavez
 * fixed q330DestroyCfg() return without value bug (aap)
 *
 * Revision 1.22  2011/04/12 20:57:03  dechavez
 * added Q330_LCQ support (does NOT check for dup entries!)
 *
 * Revision 1.21  2011/02/01 20:19:06  dechavez
 * make first entry in the addr list the console
 *
 * Revision 1.20  2011/01/25 18:11:25  dechavez
 * use new QDP_CONNECT "connect" field of Q330_ADDR
 *
 * Revision 1.19  2010/12/27 22:39:03  dechavez
 * complete the sensor entries in the Q330_CALIB input field
 *
 * Revision 1.18  2010/12/21 20:00:46  dechavez
 * added q330PathName(), q330AddAuth().  Added Q330_IOE_x errors.  Don't
 * make lack of auth file serial numbers a fatal error in q330ReadCfg(), just
 * set serial number to 0.
 *
 * Revision 1.17  2010/12/17 19:33:00  dechavez
 * libq330 2.0.0
 *
 * Revision 1.16  2010/11/24 18:59:15  dechavez
 * added sensors to addr and calib
 *
 * Revision 1.15  2010/09/30 17:37:17  dechavez
 * initialize starttime parameter in AddCalib()
 *
 * Revision 1.14  2010/09/29 21:26:01  dechavez
 * eliminated automatic "default" tag, added support for e300 calibration parameter
 *
 * Revision 1.13  2010/04/02 18:17:27  dechavez
 * restored support for instance parameter
 *
 * Revision 1.12  2010/03/31 20:22:53  dechavez
 * removed calibration port and instance parameters
 *
 * Revision 1.11  2010/03/22 21:44:00  dechavez
 * added support for printing non-default calibration ports
 *
 * Revision 1.10  2010/03/12 00:30:54  dechavez
 * added support for calibration stuff in Q330_ADDR
 *
 * Revision 1.9  2009/07/25 17:32:36  dechavez
 * moved lookup routines to lookup.c, reworked arrays into linked lists
 *
 * Revision 1.8  2008/03/11 20:48:45  dechavez
 * silently ensure calib duration does not exceed hardware max
 *
 * Revision 1.7  2007/12/20 23:08:47  dechavez
 * Silently ensure calib divisor does not exceed hardware max
 *
 * Revision 1.6  2007/12/14 21:35:28  dechavez
 * added calib support
 *
 * Revision 1.5  2007/10/31 17:16:01  dechavez
 * q330LookupAddr() changed to pass Q330_ADDR
 *
 * Revision 1.4  2007/09/26 23:05:58  dechavez
 * set errno correctly when unable to find a cfg file, fix q330LookupDetector() return code when lookup fails
 *
 * Revision 1.3  2007/09/25 20:54:46  dechavez
 * cleaned up detector initialization so it works OK with libdetect 1.0.0
 *
 * Revision 1.2  2007/09/22 02:37:02  dechavez
 * added detector support
 *
 * Revision 1.1  2007/09/06 18:25:09  dechavez
 * created
 *
 */
