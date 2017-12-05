#pragma ident "$Id: qio.c,v 1.6 2016/08/26 20:15:38 dechavez Exp $"
/*======================================================================
 *
 *  Maintain status thread with digitizer
 *
 *====================================================================*/
#include "qmon.h"

BOOL isActiveDigitizer(DIGITIZER *digitizer)
{
BOOL retval;

    MUTEX_LOCK(&digitizer->mutex);
        retval = (digitizer->win[0] != NULL || digitizer->win[1] != NULL);
    MUTEX_UNLOCK(&digitizer->mutex);

    return retval;
}

static void ClearAllActiveDigitizers(LNKLST *avail)
{
int i;
DIGITIZER *digitizer;
LNKLST_NODE *crnt;

    crnt = listFirstNode(avail);
    while (crnt != NULL) {
        digitizer = (DIGITIZER *) crnt->payload;
        MUTEX_LOCK(&digitizer->mutex);
            digitizer->win[0] = digitizer->win[1] = NULL;
        MUTEX_UNLOCK(&digitizer->mutex);
        crnt = listNextNode(crnt);
    }
}

void Activate(QMON *qmon, DIGITIZER *top, DIGITIZER *bot)
{
#define STATE_SINGLE 1
#define STATE_DUAL   2
DIGITIZER *digitizer;
LNKLST_NODE *crnt;
int NewState;
static int PrevState = STATE_SINGLE;

    ClearAllActiveDigitizers(qmon->avail);

/* single digitizer gets assigned to both windows */

    if (bot == NULL) {

        NewState = STATE_SINGLE;

        MUTEX_LOCK(&top->mutex);
            top->win[0] = qmon->win.top;
            top->win[1] = qmon->win.bot;
        MUTEX_UNLOCK(&top->mutex);

        if (PrevState != NewState) {
            top->clrbot = TRUE;
            PrevState = NewState;
        } else {
            top->clrbot = FALSE;;
        }
        SEM_POST(&top->sem);

    } else {

/* otherwise the first gets the top and the second the bottom */

        NewState = STATE_DUAL;

        MUTEX_LOCK(&top->mutex);
            top->win[0] = qmon->win.top;
        MUTEX_UNLOCK(&top->mutex);

        MUTEX_LOCK(&bot->mutex);
            bot->win[0] = qmon->win.bot;
        MUTEX_UNLOCK(&bot->mutex);

        if (PrevState != NewState) {
            top->clrbot = TRUE;
            PrevState = NewState;
        } else {
            top->clrbot = FALSE;;
        }

        SEM_POST(&top->sem);
        SEM_POST(&bot->sem);
    }
}

static DIGITIZER *Digitizer(QMON *qmon, char *name)
{
DIGITIZER *digitizer;
LNKLST_NODE *crnt;

    crnt = listFirstNode(qmon->avail);
    while (crnt != NULL) {
        digitizer = (DIGITIZER *) crnt->payload;
        if (strcmp(digitizer->addr->connect.ident, name) == 0) return digitizer;
        crnt = listNextNode(crnt);
    }

    return NULL;
}

BOOL UpdateDigitizerSelection(QMON *qmon, char *top, char *bot)
{
DIGITIZER *digitizer[2];

    if (bot == NULL) {
        if ((digitizer[0] = Digitizer(qmon, top)) == NULL) return FALSE;
        Activate(qmon, digitizer[0], NULL);
    } else {
        if ((digitizer[0] = Digitizer(qmon, top)) == NULL) return FALSE;
        if ((digitizer[1] = Digitizer(qmon, bot)) == NULL) return FALSE;
        Activate(qmon, digitizer[0], digitizer[1]);
    }
    DrawDigitizerList(qmon);

    return TRUE;
}

static void GetCurrentSelection(QMON *qmon, DIGITIZER **ptop, DIGITIZER **pbot, DIGITIZER **pnxt, DIGITIZER **pprv)
{
int i, top, bot, nxt, prv;
DIGITIZER *digitizer;
static char *fid = "GetCurrentSelection";

    *ptop = *pbot = *pnxt = *pprv = NULL;

    for (top=bot=nxt=prv=-1, i = 0; i < qmon->avail->count; i++) {
        digitizer = (DIGITIZER *) qmon->avail->array[i];
        if (top < 0 && digitizer->win[0] == qmon->win.top) top = i;
        if (bot < 0 && digitizer->win[0] == qmon->win.bot) bot = i;
    }

    if (top < 0) {
        *ptop = *pbot = *pnxt = *pprv = NULL;
        return;
    } else {
        prv = top > 0 ? top - 1 : qmon->avail->count-1;
        *ptop = (DIGITIZER *) qmon->avail->array[top];
        *pprv = (DIGITIZER *) qmon->avail->array[prv];
        if (bot < 0) {
            nxt = (top + 1) % qmon->avail->count;
        } else {
            *pbot = (DIGITIZER *) qmon->avail->array[bot];
            nxt = (bot + 1) % qmon->avail->count;
        }
        *pnxt = (DIGITIZER *) qmon->avail->array[nxt];
    }

    return;
}

/* Shift current digitizer selection to the right (with wrap around) */

void ShiftSelectionRight(QMON *qmon)
{
DIGITIZER *top, *bot, *nxt, *prv;

    GetCurrentSelection(qmon, &top, &bot, &nxt, &prv);
    if (bot == NULL) {
        Activate(qmon, nxt, NULL);
    } else {
        Activate(qmon, bot, nxt);
    }
    DrawDigitizerList(qmon);
}

/* Shift current digitizer selection to the left (with wrap around) */

void ShiftSelectionLeft(QMON *qmon)
{
DIGITIZER *top, *bot, *nxt, *prv;

    GetCurrentSelection(qmon, &top, &bot, &nxt, &prv);
    if (bot == NULL) {
        Activate(qmon, prv, NULL);
    } else {
        Activate(qmon, prv, top);
    }
    DrawDigitizerList(qmon);
}

/* If one digitizer displayed, add other.  If two displayed, remove the bottom one. */

void AddRemDigitizer(QMON *qmon)
{
DIGITIZER *top, *bot, *nxt, *prv;
char *fid = "ShiftSelection";

    GetCurrentSelection(qmon, &top, &bot, &nxt, &prv);
    if (bot == NULL) {
        Activate(qmon, top, nxt);
    } else {
        Activate(qmon, top, NULL);
    }
    DrawDigitizerList(qmon);
}

/* Add next (or specific) digitizer to display, shifting off top if necessary */

BOOL AddDigitizer(QMON *qmon, char *nxtname)
{
char *topname, *botname;
DIGITIZER *top, *bot, *nxt, *prv;

    GetCurrentSelection(qmon, &top, &bot, &nxt, &prv);

/* if user just gave the + command, add next digitizer */

    if (strlen(nxtname) == 0) {
        if (bot == NULL) {
            Activate(qmon, top, nxt);
        } else {
            Activate(qmon, bot, nxt);
        }

/* otherwise, add the specific digitizer */

    }  else {

        topname = top != NULL ? top->addr->connect.ident : NULL;
        botname = bot != NULL ? bot->addr->connect.ident : NULL;

        if (botname == NULL) {
            UpdateDigitizerSelection(qmon, topname, nxtname);
        } else {
            UpdateDigitizerSelection(qmon, botname, nxtname);
        }
    }

    DrawDigitizerList(qmon);

    return TRUE;
}

/* change state */

static void SetState(DIGITIZER *digitizer, int value)
{
static char *fid = "SetState";

    MUTEX_LOCK(&digitizer->mutex);
        digitizer->state = value;
    MUTEX_UNLOCK(&digitizer->mutex);
}

static QDP *Register(DIGITIZER *digitizer)
{
QDP *qdp;
int errcode, regerr, state;

    if (digitizer->addr->serialno == QDP_INVALID_SERIALNO) {
        SetState(digitizer, QMON_STATE_BADSERIALNO);
        return NULL;
    }

    if (digitizer->state == QMON_STATE_UNCONNECTED) SetState(digitizer, QMON_STATE_CONNECTING);
    if ((qdp =  q330Register(digitizer->addr, QDP_SFN_PORT, digitizer->debug, dbglp(), &errcode, &regerr)) == NULL) {
        state = (errcode == QDP_ERR_BUSY ? QMON_STATE_BUSY : QMON_STATE_REGERR);
    } else {
        state = QMON_STATE_CONNECTED;
    }

    SetState(digitizer, state);
    return qdp;
}

static QDP *Deregister(DIGITIZER *digitizer, QDP *qdp, int state)
{
    qdpShutdown(qdp);
    SetState(digitizer, state);
    return NULL;
}

static BOOL DigitizerSelected(DIGITIZER *digitizer)
{
BOOL retval;

    MUTEX_LOCK(&digitizer->mutex);
        retval = (digitizer->win[0] != NULL) ? TRUE : FALSE;
    MUTEX_UNLOCK(&digitizer->mutex);

    return retval;
}

static THREAD_FUNC DigitizerThread(void *arg)
{
QDP *qdp = NULL;
DIGITIZER *my;
QDP_TYPE_MONITOR mon;
UINT32 expected[QDP_NSC];
static char *fid = "DigitizerThread";

    my = (DIGITIZER *) arg;

    MUTEX_LOCK(&my->mutex);
        memset(&mon, 0, sizeof(QDP_TYPE_MONITOR));
    MUTEX_UNLOCK(&my->mutex);

    while (1) {

    /* deregister and quit if shutdown in progress */

        if (ShutdownInProgress()) {
            if (qdp != NULL) {
                qdp = Deregister(my, qdp, QMON_STATE_SHUTDOWN);
                THREAD_EXIT(0);
            }
        }

    /* don't do anything until we are selected */

        if (!DigitizerSelected(my)) {
            if (qdp != NULL) qdp = Deregister(my, qdp, QMON_STATE_UNCONNECTED);
            SEM_WAIT(&my->sem); /* block until we are needed */
        }

    /* if not already registered, do so now */

        if (qdp == NULL) qdp = Register(my);

    /* update status */

        if (qdp != NULL) {
            if (qdpRequestMonitor(qdp, &mon)) {
                MUTEX_LOCK(&my->mutex);
                    my->mon = mon;
                    if (my->flags & REBOOT_IN_PROGRESS) my->flags &= ~REBOOT_IN_PROGRESS;
                    my->fresh = TRUE;
                MUTEX_UNLOCK(&my->mutex);
            } else {
                qdp = Deregister(my, qdp, QMON_STATE_UNCONNECTED);
            }
        }

    /* wait a bit and do it all again */

        sleep(my->interval);
    }
}

BOOL StartDigitizerThread(QMON *qmon, Q330_ADDR *addr)
{
THREAD tid;
LNKLST_NODE *last;
DIGITIZER new, *this;
static char *fid = "StartDigitizerThread";

    new.addr     = addr;
    new.state    = QMON_STATE_UNCONNECTED;
    new.debug    = qmon->debug;
    new.interval = qmon->interval;
    new.win[0]   = NULL;
    new.win[1]   = NULL;
    new.clrbot   = FALSE;
    memset(&new.mon, 0, sizeof(QDP_TYPE_MONITOR));
    new.fresh    = FALSE;
    new.flags    = 0;

    if (!listAppend(qmon->avail, &new, sizeof(DIGITIZER))) {
        perror("listAppend");
        return FALSE;
    }

    if ((last = listLastNode(qmon->avail)) == NULL) {
        perror("listLastNode");
        return FALSE;
    }
    this = (DIGITIZER *) last->payload;

    MUTEX_INIT(&this->mutex);
    SEM_INIT(&this->sem, 0, 1);

    if (!THREAD_CREATE(&tid, DigitizerThread, (void *) this)) {
        perror("THREAD_CREATE");
        return FALSE;
    }

    return TRUE;
}

/*-----------------------------------------------------------------------+
 |                                                                       |
 | Copyright (C) 2011 Regents of the University of California            |
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
 * $Log: qio.c,v $
 * Revision 1.6  2016/08/26 20:15:38  dechavez
 * Use qdpShutdown() instead of qdpDeregister() when breaking connections
 *
 * Revision 1.5  2016/07/20 22:09:42  dechavez
 * added suberr to q330Register() calls for libqdp 3.13.0 compatibility
 *
 * Revision 1.4  2015/12/23 22:44:54  dechavez
 * Don't bother connecting if the system lacks an internal serial number, set
 * state to QMON_STATE_BADSERIALNO instead.  This gets reflected in the display.
 *
 * Revision 1.3  2015/12/07 19:43:44  dechavez
 * fixed CVS log message for previous commit
 *
 * Revision 1.2  2015/12/07 19:29:15  dechavez
 * added missing return from AddDigitizer()
 *
 * Revision 1.1  2011/07/27 23:42:49  dechavez
 * created from former q330.c
 *
 */
