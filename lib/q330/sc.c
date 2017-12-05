#pragma ident "$Id: sc.c,v 1.3 2014/01/30 22:37:06 dechavez Exp $"
/*======================================================================
 * 
 * Sensor control utilities
 *
 *====================================================================*/
#include "q330.h"

static BOOL FreeListReturn(LNKLST *list1, LNKLST *list2, BOOL retval)
{
    if (list1 != NULL) listDestroy(list1);
    if (list2 != NULL) listDestroy(list2);
    return retval;
}

BOOL q330ParseSensorControlLines(char which, UINT32 *sc, char *input, BOOL ActiveHigh, int *errcode)
{
int i, index;
BOOL sensorA;
LNKLST *list1, *list2 = NULL;
LNKLST_NODE *crnt;


    if (sc == NULL || input == NULL || errcode == NULL) {
        errno = EINVAL;
        if (errcode != NULL) *errcode = Q330_ERROR;
        return FALSE;
    }

    sensorA = (which == 'a' || which == 'A') ? TRUE : FALSE;

    /* input is a 1:cen,2:idl,3:idl,4:cal type string */

    if ((list1 = utilStringTokenList(input, ",", 0)) == NULL) {
        *errcode = Q330_ERROR;
        return FreeListReturn(list1, list2, FALSE);
    }

    if (list1->count > QDP_NSC) {
        *errcode = Q330_BAD_SENSOR;
        return FreeListReturn(list1, list2, FALSE);
    }

    for (i = 0; i < QDP_NSC; i++) sc[i] = QDP_SC_UNDEFINED;
    if (list1->count == 0) return TRUE;

    /* now parse each line:ctl entry */

    crnt = listFirstNode(list1);
    while (crnt != NULL) {
        if ((list2 = utilStringTokenList((char *) crnt->payload, ":", 0)) == NULL) {
            *errcode = Q330_ERROR;
            return FreeListReturn(list1, list2, FALSE);
        }
        if (list2->count != 2) {
            *errcode = Q330_BAD_SENSOR;
            return FreeListReturn(list1, list2, FALSE);
        }
        if (!listSetArrayView(list2)) {
            *errcode = Q330_ERROR;
            return FreeListReturn(list1, list2, FALSE);
        }
        index = atoi((char *) list2->array[0]);
        if (index < 1 || index > QDP_NSC) {
            *errcode = Q330_BAD_SENSOR;
            return FreeListReturn(list1, list2, FALSE);
        }
        --index; /* to use it in the array */
        if (sc[index] != QDP_SC_UNDEFINED) {
            *errcode = Q330_BAD_SENSOR;
            return FreeListReturn(list1, list2, FALSE);
        }

        if (strcasecmp((char *) list2->array[1], "idl") == 0) {
            sc[index] = sensorA ? QDP_SC_IDLE : QDP_SC_IDLE;

        } else if (strcasecmp((char *) list2->array[1], "cal") == 0) {
            sc[index] = sensorA ? QDP_SC_SENSOR_A_CALIB : QDP_SC_SENSOR_B_CALIB;
            if (ActiveHigh) sc[index] |= QDP_SC_ACTIVE_HIGH_BIT;

        } else if (strcasecmp((char *) list2->array[1], "cen") == 0) {
            sc[index] = sensorA ? QDP_SC_SENSOR_A_CENTER : QDP_SC_SENSOR_B_CENTER;
            if (ActiveHigh) sc[index] |= QDP_SC_ACTIVE_HIGH_BIT;

        } else if (strcasecmp((char *) list2->array[1], "cap") == 0) {
            sc[index] = sensorA ? QDP_SC_SENSOR_A_CAP : QDP_SC_SENSOR_B_CAP;
            if (ActiveHigh) sc[index] |= QDP_SC_ACTIVE_HIGH_BIT;

        } else if (strcasecmp((char *) list2->array[1], "lck") == 0) {
            sc[index] = sensorA ? QDP_SC_SENSOR_A_LOCK : QDP_SC_SENSOR_B_LOCK;
            if (ActiveHigh) sc[index] |= QDP_SC_ACTIVE_HIGH_BIT;

        } else if (strcasecmp((char *) list2->array[1], "unl") == 0) {
            sc[index] = sensorA ? QDP_SC_SENSOR_A_UNLOCK : QDP_SC_SENSOR_B_UNLOCK;
            if (ActiveHigh) sc[index] |= QDP_SC_ACTIVE_HIGH_BIT;

        } else if (strcasecmp((char *) list2->array[1], "ax1") == 0) {
            sc[index] = sensorA ? QDP_SC_SENSOR_A_AUX1 : QDP_SC_SENSOR_B_AUX1;
            if (ActiveHigh) sc[index] |= QDP_SC_ACTIVE_HIGH_BIT;

        } else if (strcasecmp((char *) list2->array[1], "ax2") == 0) {
            sc[index] = sensorA ? QDP_SC_SENSOR_A_AUX2 : QDP_SC_SENSOR_B_AUX2;
            if (ActiveHigh) sc[index] |= QDP_SC_ACTIVE_HIGH_BIT;

        } else if (strcasecmp((char *) list2->array[1], "dpr") == 0) {
            sc[index] = sensorA ? QDP_SC_SENSOR_A_DEPREM : QDP_SC_SENSOR_B_DEPREM;
            if (ActiveHigh) sc[index] |= QDP_SC_ACTIVE_HIGH_BIT;

        } else {
            *errcode = Q330_BAD_SENSOR;
            return FreeListReturn(list1, list2, FALSE);
        }
        crnt = listNextNode(crnt);
    }

    return FreeListReturn(list1, list2, TRUE);
}

UINT16 q330SensorControlBitmap(UINT32 *sc)
{
int i;
BOOL IsIdle, IsActiveHigh;
UINT16 bitmap;

    if (sc == NULL) return 0;

    for (bitmap = 0, i = 0; i < QDP_NSC; i++) {
        IsIdle       = ((UINT32) sc[i] & QDP_SC_MASK) == QDP_SC_IDLE  ? TRUE : FALSE;
        IsActiveHigh = ((UINT32) sc[i] & QDP_SC_ACTIVE_HIGH_BIT) != 0 ? TRUE : FALSE;
        if ((IsIdle && IsActiveHigh) || (!IsIdle && !IsActiveHigh)) bitmap |=  (1 << i);
    }

    return bitmap;
}

BOOL q330InitializeSensorControl(UINT32 *sc, Q330_SENSOR *a, Q330_SENSOR *b)
{
int i;
LNKLST *list;
static char *fid = "q330InitializeSensorControl";

    if (sc == NULL || a == NULL || b == NULL) {
        errno = EINVAL;
        return FALSE;
    }

    for (i = 0; i < QDP_NSC; i++) sc[i] = QDP_SC_UNDEFINED;

    for (i = 0; i < QDP_NSC; i++) {
        if (a->scA[i] != QDP_SC_UNDEFINED) {
            if (sc[i] != QDP_SC_UNDEFINED) {
                return FALSE;
            } else {
                sc[i] = a->scA[i];
            }
        }
        if (b->scB[i] != QDP_SC_UNDEFINED) {
            if (sc[i] != QDP_SC_UNDEFINED) {
                return FALSE;
            } else {
                sc[i] = b->scB[i];
            }
        }
    }

    return TRUE;
}

char *q330SensorCtrlLineString(UINT32 *sc, char *buf)
{
BOOL first = TRUE;
int i, j, line;
static char mt_unsafe[] = "1:idl,2:idl,3:idl,4:idl,5:idl,6:idl,7:idl,9:idl plus slop";
     
    if (buf == NULL) buf = mt_unsafe;
    buf[0] = 0;
     
    for (i = 0, j = 1; i < QDP_NSC; i++, j++) {
        if ((line  = sc[i] & QDP_SC_MASK) == QDP_SC_UNDEFINED) continue;
        if (first) {
            first = FALSE;
        } else {
            strcat(buf, ",");
        }
        switch (line) {
          case QDP_SC_IDLE:
            sprintf(buf+strlen(buf), "%d:idl", j);
            break;
          case QDP_SC_SENSOR_A_CENTER:
          case QDP_SC_SENSOR_B_CENTER:
            sprintf(buf+strlen(buf), "%d:cen", j);
            break;
          case QDP_SC_SENSOR_A_CAP:
          case QDP_SC_SENSOR_B_CAP:
            sprintf(buf+strlen(buf), "%d:cap", j);
            break;
          case QDP_SC_SENSOR_A_CALIB:
          case QDP_SC_SENSOR_B_CALIB:
            sprintf(buf+strlen(buf), "%d:cal", j);
            break;
          case QDP_SC_SENSOR_A_LOCK:
          case QDP_SC_SENSOR_B_LOCK:
            sprintf(buf+strlen(buf), "%d:lck", j);
            break;
          case QDP_SC_SENSOR_A_UNLOCK:
          case QDP_SC_SENSOR_B_UNLOCK:
            sprintf(buf+strlen(buf), "%d:unl", j);
            break;
          case QDP_SC_SENSOR_A_AUX1:
          case QDP_SC_SENSOR_B_AUX1:
            sprintf(buf+strlen(buf), "%d:ax1", j);
            break;
          case QDP_SC_SENSOR_A_AUX2:
          case QDP_SC_SENSOR_B_AUX2:
            sprintf(buf+strlen(buf), "%d:ax2", j);
            break;
          case QDP_SC_SENSOR_A_DEPREM:
          case QDP_SC_SENSOR_B_DEPREM:
            sprintf(buf+strlen(buf), "%d:dpr", j);
            break;
        }
    }

    return buf;
}

char *q330SensorHiLoString(Q330_SENSOR *sensor)
{
int i, line;
static char hi[] = "hi";
static char lo[] = "lo";
static char na[] = "na";
BOOL HaveSomething = FALSE;

    for (i = 0; i < QDP_NSC; i++) {
        line = sensor->scA[i] & QDP_SC_MASK;
        if (line != QDP_SC_IDLE && line != QDP_SC_UNDEFINED) HaveSomething = TRUE;
        if (sensor->scA[i] & QDP_SC_ACTIVE_HIGH_BIT) return hi;
    }

    if (HaveSomething) return lo;

    for (i = 0; i < QDP_NSC; i++) {
        line = sensor->scB[i] & QDP_SC_MASK;
        if (line != QDP_SC_IDLE && line != QDP_SC_UNDEFINED) HaveSomething = TRUE;
        if (sensor->scB[i] & QDP_SC_ACTIVE_HIGH_BIT) return hi;
    }

    return HaveSomething ? lo : na;
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
 * $Log: sc.c,v $
 * Revision 1.3  2014/01/30 22:37:06  dechavez
 * added spaces to make code easier to read
 *
 * Revision 1.2  2014/01/30 20:00:02  dechavez
 * added STS-5A dpr (deploy/remove) support
 *
 * Revision 1.1  2010/12/17 20:00:44  dechavez
 * created
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
