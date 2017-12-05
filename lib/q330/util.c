#pragma ident "$Id: util.c,v 1.19 2017/01/26 19:48:06 dechavez Exp $"
/*======================================================================
 * 
 * Some useful commands
 *
 *====================================================================*/
#define INCLUDE_Q330_INPUT_LIST
#include "q330.h"

static int DeregisterAndReturn(QDP *qdp, int retval)
{
    qdpDeregister(qdp, TRUE);
    qdpShutdown(qdp);
    return retval;
}

int q330CalibrationStatus(Q330_ADDR *addr, int *errcode)
{
QDP *qdp;
int retval;
QDP_TYPE_C1_STAT stat;

    if (addr == NULL || errcode == NULL) {
        errno = EINVAL;
        *errcode = Q330_ERROR;
        return Q330_ERROR;
    }

    qdpInit_C1_STAT(&stat);

    if ((qdp = q330Register(addr, QDP_CFG_PORT, 0, NULL, errcode, NULL)) == NULL) {
        return (*errcode == QDP_ERR_BUSY) ? (*errcode = Q330_PORT_BUSY) : Q330_ERROR;
    }

    if (!qdp_C1_RQSTAT(qdp, QDP_SRB_GBL, &stat)) return DeregisterAndReturn(qdp, Q330_ERROR);

    switch (stat.gbl.cal_stat) {
      case 0:  retval = Q330_CAL_OFF;    break;
      case 1:  retval = Q330_CAL_ENABLE; break;
      case 2:  retval = Q330_CAL_ACTIVE; break;
      case 3:  retval = Q330_CAL_ACTIVE; break;
      default: retval = Q330_CAL_OTHER;  break;
    }

    return DeregisterAndReturn(qdp, retval);

}

static void InitInput(Q330_INPUT *input, char *name)
{
int i;

    strlcpy(input->name, name, Q330_INPUT_NAME_LEN+1);
    input->sensor.name[0] = 0;
    input->e300.present = FALSE;
    memset(input->e300.name, 0, Q330_E300_NAME_LEN+1);
    input->extra.present = FALSE;
    memset(input->extra.name, 0, Q330_INPUT_NAME_LEN+1);
}

static BOOL FreeListReturn(LNKLST *list, BOOL retval)
{
    if (list != NULL) listDestroy(list);
    return retval;
}

BOOL q330ParseComplexInputName(Q330_INPUT *input, char *name, int *errcode)
{
LNKLST *list = NULL;


    if (input == NULL || name == NULL || errcode == NULL) {
        errno = EINVAL;
        if (errcode != NULL) *errcode = Q330_ERROR;
        return FreeListReturn(list, FALSE);
    }

    if ((list = utilStringTokenList(name, ":", 0)) == NULL) {
        *errcode = Q330_ERROR;
        return FreeListReturn(list, FALSE);
    }

    if (!listSetArrayView(list)) {
        *errcode = Q330_ERROR;
        return FreeListReturn(list, FALSE);
    }

    if (list->count < 1 || list->count > 3) {
        *errcode = Q330_BAD_INPUT;
        fprintf(stderr, "ERROR: '%s' is not a recognized sensor name\n", name);
        return FreeListReturn(list, FALSE);
    }

    InitInput(input, name);

    strlcpy(input->sensor.name, (char *) list->array[0], Q330_SENSOR_NAME_LEN+1);

    if (list->count == 1) return FreeListReturn(list, TRUE);

    if (strstr(name, "::") == NULL) {
        strlcpy(input->e300.name, (char *) list->array[1], Q330_E300_NAME_LEN+1);
        input->e300.present = strlen(input->e300.name) > 0 ? TRUE : FALSE;
    } else {
        strlcpy(input->extra.name, (char *) list->array[1], Q330_INPUT_NAME_LEN+1);
        input->extra.present = strlen(input->extra.name) > 0 ? TRUE : FALSE;
    }

    if (list->count == 2) return FreeListReturn(list, TRUE);

    strlcpy(input->extra.name, (char *) list->array[2], Q330_INPUT_NAME_LEN+1);
    input->extra.present = strlen(input->extra.name) > 0 ? TRUE : FALSE;

    return FreeListReturn(list, TRUE);
}

void q330PrintErrcode(FILE *fp, char *prefix, char *root, int errcode)
{
static char *DefaultRoot = Q330_DEFAULT_ROOT;
static char path[MAXPATHLEN+1];

    if (root == NULL) root = DefaultRoot;
    if (prefix != NULL) fprintf(fp, "%s", prefix);
    fprintf(fp, "root='%s': ", root);

    switch (errcode) {

      case Q330_IOE_ADDR:     fprintf(fp, "%s: %s\n", q330PathName(root, Q330_NAME_ADDR,     path), strerror(errno)); break;
      case Q330_IOE_AUTH:     fprintf(fp, "%s: %s\n", q330PathName(root, Q330_NAME_AUTH,     path), strerror(errno)); break;
      case Q330_IOE_SENSOR:   fprintf(fp, "%s: %s\n", q330PathName(root, Q330_NAME_SENSOR,   path), strerror(errno)); break;
      case Q330_IOE_CALIB:    fprintf(fp, "%s: %s\n", q330PathName(root, Q330_NAME_CALIB,    path), strerror(errno)); break;
      case Q330_IOE_DETECTOR: fprintf(fp, "%s: %s\n", q330PathName(root, Q330_NAME_DETECTOR, path), strerror(errno)); break;
      case Q330_IOE_LCQ:      fprintf(fp, "%s: %s\n", q330PathName(root, Q330_NAME_LCQ,      path), strerror(errno)); break;

      case Q330_BAD_ADDR:     fprintf(fp, "bad %s entry(ies)\n", q330PathName(root, Q330_NAME_ADDR,     path)); break;
      case Q330_BAD_AUTH:     fprintf(fp, "bad %s entry(ies)\n", q330PathName(root, Q330_NAME_AUTH,     path)); break;
      case Q330_BAD_SENSOR:   fprintf(fp, "bad %s entry(ies)\n", q330PathName(root, Q330_NAME_SENSOR,   path)); break;
      case Q330_BAD_CALIB:    fprintf(fp, "bad %s entry(ies)\n", q330PathName(root, Q330_NAME_CALIB,    path)); break;
      case Q330_BAD_DETECTOR: fprintf(fp, "bad %s entry(ies)\n", q330PathName(root, Q330_NAME_DETECTOR, path)); break;
      case Q330_BAD_LCQ:      fprintf(fp, "bad %s entry(ies)\n", q330PathName(root, Q330_NAME_LCQ,      path)); break;

      case Q330_DUP_ADDR:     fprintf(fp, "dup %s entry(ies)\n", q330PathName(root, Q330_NAME_ADDR,     path)); break;
      case Q330_DUP_AUTH:     fprintf(fp, "dup %s entry(ies)\n", q330PathName(root, Q330_NAME_AUTH,     path)); break;
      case Q330_DUP_SENSOR:   fprintf(fp, "dup %s entry(ies)\n", q330PathName(root, Q330_NAME_SENSOR,   path)); break;
      case Q330_DUP_CALIB:    fprintf(fp, "dup %s entry(ies)\n", q330PathName(root, Q330_NAME_CALIB,    path)); break;
      case Q330_DUP_DETECTOR: fprintf(fp, "dup %s entry(ies)\n", q330PathName(root, Q330_NAME_DETECTOR, path)); break;
      case Q330_DUP_LCQ:      fprintf(fp, "bad %s entry(ies)\n", q330PathName(root, Q330_NAME_LCQ,      path)); break;

      case Q330_BAD_INPUT:
        fprintf(fp, "\nunrecognized sensor(s) in %s ", q330PathName(root, Q330_NAME_ADDR, path));
        fprintf(fp, "or %s\n", q330PathName(root, Q330_NAME_CALIB, path));
        break;

      case Q330_NO_AUTH:
        fprintf(fp, "\ntag numbers in %s ", q330PathName(root, Q330_NAME_ADDR, path));
        fprintf(fp, "lack corresponding %s entries\n", q330PathName(root, Q330_NAME_AUTH, path));
        break;

      case Q330_ERROR: printf("I/O error: %s\n", strerror(errno)); break;

      case Q330_NO_LCQMEM:    fprintf(fp, "insufficient memory to create LCQ list\n"); break;

      default:
        printf("error %d: %s\n", errcode, strerror(errno));
        break;
    }

    fflush(fp);
}

UINT16 q330ChannelBitmap(char *string)
{
UINT16 shift, result = 0;
char *ptr;

    if (strcasecmp(string, "A") == 0) return Q330_SENSOR_A_BITMAP;
    if (strcasecmp(string, "B") == 0) return Q330_SENSOR_B_BITMAP;

    ptr = string;
    while (*ptr != 0) {
        switch (*ptr++) {
          case '1': shift = 0; break;
          case '2': shift = 1; break;
          case '3': shift = 2; break;
          case '4': shift = 3; break;
          case '5': shift = 4; break;
          case '6': shift = 5; break;
          default:
            return 0;
        }
        result |= 1 << shift;
    }

    return result;
}

/*-----------------------------------------------------------------------+
 |                                                                       |
 | Copyright (C) 2010 Regents of the University of California            |
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
 * $Log: util.c,v $
 * Revision 1.19  2017/01/26 19:48:06  dechavez
 * added Q330_NO_LCQMEM support
 *
 * Revision 1.18  2017/01/26 18:59:35  dechavez
 * fixed newline type in q330PrintErrcode()
 *
 * Revision 1.17  2016/07/20 22:03:42  dechavez
 * qdpConnect() suberr argument added for libqdp 3.13.0 compatibility
 *
 * Revision 1.16  2016/05/18 16:26:11  dechavez
 * Fixed incorrect strlcpy length parameter in q330ParseComplexInputName()
 *
 * Revision 1.15  2015/12/04 23:20:35  dechavez
 * casts, format fixes and the like, all to calm OS X compiles
 *
 * Revision 1.14  2014/09/08 18:32:52  dechavez
 * added some stderr commentary when q330ParseComplexInputName() encounters problems
 *
 * Revision 1.13  2013/06/19 22:20:27  dechavez
 * added q330ChannelBitmap() support for "A" and "B" strings
 *
 * Revision 1.12  2011/04/16 04:51:59  dechavez
 * added q330ChannelBitmap()
 *
 * Revision 1.11  2011/04/12 20:54:00  dechavez
 * added Q330_DUP_LCQ, Q330_BAD_LCQ, Q330_IOE_LCQ to q330PrintErrcode()
 *
 * Revision 1.10  2011/02/03 17:52:26  dechavez
 * 3.1.0
 *
 * Revision 1.9  2010/12/27 22:37:35  dechavez
 * added support for populating Q330_INPUT "extra" field in q330ParseComplexInputName()
 *
 * Revision 1.8  2010/12/21 21:33:32  dechavez
 * removed unused resistors field from Q330_INPUT
 *
 * Revision 1.7  2010/12/21 19:56:21  dechavez
 * added q330PrintErrcode(), removed some dead code
 *
 * Revision 1.6  2010/12/17 19:33:00  dechavez
 * libq330 2.0.0
 *
 * Revision 1.5  2010/12/13 22:55:31  dechavez
 * added q330SensorControlBitmap()
 *
 * Revision 1.4  2010/12/02 20:26:03  dechavez
 * added STS2.5 support
 *
 * Revision 1.3  2010/11/24 21:59:19  dechavez
 * fixed bug parse sts1::r strings
 *
 * Revision 1.2  2010/11/24 18:36:16  dechavez
 * added q330SensorNameString(), q330SensorType(), q330ParseComplexSensorName(), q330ConfirmSensorControl()
 *
 * Revision 1.1  2010/04/12 21:12:50  dechavez
 * created
 *
 */
