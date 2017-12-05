#pragma ident "$Id: fsa.h,v 1.22 2016/08/26 20:02:37 dechavez Exp $"
/*======================================================================
 * 
 * Constants for the QDP Finite State Automaton
 *
 *====================================================================*/
#ifndef qdp_fsa_included
#define qdp_fsa_included

#ifdef __cplusplus
extern "C" {
#endif

/* states */
#define QDP_MIN_STATE          0
#define QDP_STATE_NEEDSN       (QDP_MIN_STATE +  0) /* need to poll for serial number */
#define QDP_STATE_WAITSN       (QDP_MIN_STATE +  1) /* waiting for serial number */
#define QDP_STATE_UNREG        (QDP_MIN_STATE +  2) /* unregistered */
#define QDP_STATE_RQSRV        (QDP_MIN_STATE +  3) /* registration request has been sent */
#define QDP_STATE_SRVRSP       (QDP_MIN_STATE +  4) /* challenge response has been sent */
#define QDP_STATE_WAIT_C1_FIX  (QDP_MIN_STATE +  5) /* waiting for C1_FIX    "   "       "        "    */
#define QDP_STATE_WAIT_C1_LOG  (QDP_MIN_STATE +  6) /* waiting for C1_LOG (data port connections only) */
#define QDP_STATE_WAIT_C2_EPD  (QDP_MIN_STATE +  7) /* waiting for C2_EPD    "   "       "        "    */
#define QDP_STATE_WAIT_C1_FLGS (QDP_MIN_STATE +  8) /* waiting for C1_FLGS   "   "       "        "    */
#define QDP_STATE_WAITTOKEN    (QDP_MIN_STATE +  9) /* waiting for DP tokens "   "       "        "    */
#define QDP_STATE_READY        (QDP_MIN_STATE + 10) /* Q330 handshake complete */
#define QDP_STATE_OFF          (QDP_MIN_STATE + 11) /* automaton is off */
#define QDP_NUM_STATE          (QDP_MIN_STATE + 12)

#define QDP_MAX_STATE      (QDP_NUM_STATE - 1)
#define QDP_STATE_nochange (QDP_NUM_STATE    )

/* events */
#define QDP_MIN_EVENT        0
#define QDP_EVENT_UP         (QDP_MIN_EVENT +  0) /* launch automaton */
#define QDP_EVENT_MYSN       (QDP_MIN_EVENT +  1) /* serial number received */
#define QDP_EVENT_SRVCH      (QDP_MIN_EVENT +  2) /* server challenge received */
#define QDP_EVENT_NOTREG     (QDP_MIN_EVENT +  3) /* not registered (C1_CERR code=2) */
#define QDP_EVENT_CERR       (QDP_MIN_EVENT +  4) /* C1_CERR error other than code=2 */
#define QDP_EVENT_CACK       (QDP_MIN_EVENT +  5) /* command acknowledgement received */
#define QDP_EVENT_C1_FIX     (QDP_MIN_EVENT +  6) /* C1_FIX packet received */
#define QDP_EVENT_C1_LOG     (QDP_MIN_EVENT +  7) /* C1_LOG packet received */
#define QDP_EVENT_C2_EPD     (QDP_MIN_EVENT +  8) /* C2_EPD packet received */
#define QDP_EVENT_C1_FLGS    (QDP_MIN_EVENT +  9) /* C1_FLGS packet received */
#define QDP_EVENT_TOKEN_SOME (QDP_MIN_EVENT + 10) /* DP token packet received */
#define QDP_EVENT_TOKEN_DONE (QDP_MIN_EVENT + 11) /* final DP token packet received */
#define QDP_EVENT_CTRL       (QDP_MIN_EVENT + 12) /* general control port packet received */
#define QDP_EVENT_CRCERR     (QDP_MIN_EVENT + 13) /* CRC error */
#define QDP_EVENT_IOERR      (QDP_MIN_EVENT + 14) /* I/O error */
#define QDP_EVENT_WATCHDOG   (QDP_MIN_EVENT + 15) /* registration watchdog timer expired */
#define QDP_EVENT_CTO        (QDP_MIN_EVENT + 16) /* command port timeout */
#define QDP_EVENT_DTO        (QDP_MIN_EVENT + 17) /* data port timeout */
#define QDP_EVENT_DATA       (QDP_MIN_EVENT + 18) /* DT_DATA packet received */
#define QDP_EVENT_FILL       (QDP_MIN_EVENT + 19) /* DT_FILL packet received */
#define QDP_EVENT_UKNDATA    (QDP_MIN_EVENT + 20) /* unsupported data port packet received */
#define QDP_EVENT_HBEAT      (QDP_MIN_EVENT + 21) /* time for status request */
#define QDP_EVENT_APPCMD     (QDP_MIN_EVENT + 22) /* command packet from app */
#define QDP_EVENT_OFF        (QDP_MIN_EVENT + 23) /* shutdown automaton */
#define QDP_NUM_EVENT        (QDP_MIN_EVENT + 24)

#define QDP_MAX_EVENT        (QDP_NUM_EVENT - 1)

/* actions (MUST be non-negative) */
#define QDP_MIN_ACTION         0
#define QDP_ACTION_done       (QDP_MIN_ACTION +  0) /* event processing complete */
#define QDP_ACTION_START      (QDP_MIN_ACTION +  1) /* start registration watchdog timer */
#define QDP_ACTION_SEND       (QDP_MIN_ACTION +  2) /* (re)send automaton command */
#define QDP_ACTION_POLLSN     (QDP_MIN_ACTION +  3) /* poll for serial number */
#define QDP_ACTION_LDSN       (QDP_MIN_ACTION +  4) /* save serial number */
#define QDP_ACTION_SRVRQ      (QDP_MIN_ACTION +  5) /* issue server request */
#define QDP_ACTION_DECODECH   (QDP_MIN_ACTION +  6) /* decode server challenge */
#define QDP_ACTION_BLDRSP     (QDP_MIN_ACTION +  7) /* build server response */
#define QDP_ACTION_REGISTERED (QDP_MIN_ACTION +  8) /* Q330 registration complete */
#define QDP_ACTION_RQ_C1_FIX  (QDP_MIN_ACTION +  9) /* request C1_FIX */
#define QDP_ACTION_LD_C1_FIX  (QDP_MIN_ACTION + 10) /*    save C1_FIX */
#define QDP_ACTION_RQ_C1_LOG  (QDP_MIN_ACTION + 11) /* request C1_LOG */
#define QDP_ACTION_LD_C1_LOG  (QDP_MIN_ACTION + 12) /*    save C1_LOG */
#define QDP_ACTION_RQ_C2_EPD  (QDP_MIN_ACTION + 13) /* request C2_EPD */
#define QDP_ACTION_LD_C2_EPD  (QDP_MIN_ACTION + 14) /*    save C2_EPD */
#define QDP_ACTION_RQ_C1_FLGS (QDP_MIN_ACTION + 15) /* request C1_FLGS */
#define QDP_ACTION_LD_C1_FLGS (QDP_MIN_ACTION + 16) /*    save C1_FLGS */
#define QDP_ACTION_RQ_TOKENS  (QDP_MIN_ACTION + 17) /* request DP tokens */
#define QDP_ACTION_LD_TOKENS  (QDP_MIN_ACTION + 18) /*    save DP tokens */
#define QDP_ACTION_TLU        (QDP_MIN_ACTION + 19) /* this layer up */
#define QDP_ACTION_TLD        (QDP_MIN_ACTION + 20) /* this layer down */
#define QDP_ACTION_REGERR     (QDP_MIN_ACTION + 21) /* process C1_CERR received during registration phase */
#define QDP_ACTION_CERR       (QDP_MIN_ACTION + 22) /* process C1_CERR received on registered link */
#define QDP_ACTION_CACK       (QDP_MIN_ACTION + 23) /* process C1_CACK on registered link */
#define QDP_ACTION_CTRL       (QDP_MIN_ACTION + 24) /* process control port packet on registered link */
#define QDP_ACTION_OPEN       (QDP_MIN_ACTION + 25) /* open data port */
#define QDP_ACTION_DTO        (QDP_MIN_ACTION + 26) /* data port timeout processing */
#define QDP_ACTION_HBEAT      (QDP_MIN_ACTION + 27) /* send a heartbeat */
#define QDP_ACTION_APPCMD     (QDP_MIN_ACTION + 28) /* forward user command (if any) */
#define QDP_ACTION_DACK       (QDP_MIN_ACTION + 29) /* acknowledge data packets */
#define QDP_ACTION_DATA       (QDP_MIN_ACTION + 30) /* process DT_DATA packet */
#define QDP_ACTION_FILL       (QDP_MIN_ACTION + 31) /* process DT_FILL packet */
#define QDP_ACTION_DROP       (QDP_MIN_ACTION + 32) /* drop spurious packet */
#define QDP_ACTION_FLUSH      (QDP_MIN_ACTION + 33) /* flush reorder buffer */
#define QDP_ACTION_CRCERR     (QDP_MIN_ACTION + 34) /* deal with a CRC error */
#define QDP_ACTION_IOERR      (QDP_MIN_ACTION + 35) /* deal with an I/O error */
#define QDP_ACTION_WATCHDOG   (QDP_MIN_ACTION + 36) /* deal with watchdog timer */
#define QDP_NUM_ACTION        (QDP_MIN_ACTION + 37)
 
#define QDP_MAX_ACTION        (QDP_NUM_ACTION - 1)

/* The automaton works by invoking an ordered set of simple actions (ACTION_LIST)
 * and then transitioning to a new state when those actions are completed.
 * The new state can be defined explicitly (and remaining in the current
 * state is a valid choice), or it can be determined by the action handler.
 * The action handlers are all void functions, so they should either be
 * able to complete sucessfully or should explicity bring down the whole
 * machine... nothing in between.
 */

#define MAX_ACTION_PER_EVENT 3

typedef struct {
    int action[MAX_ACTION_PER_EVENT]; /* QDP_ACTION_X */
    int next_state;                   /* transtion to this state */
} ACTION_LIST;

/* A QDP_EVENT_HANDLER handler specifies one ACTION_LIST for every defined event */

typedef struct {
    ACTION_LIST event[QDP_NUM_EVENT]; /* handlers for all possible events */
} QDP_EVENT_HANDLER;

/* The QDP State Machine is an array of event handlers for every possible state.
 * The automaton looks up the action list for the given (state, event) and
 * performs those actions in the order specified.  A list of less than the
 * maximum number of actions (MAX_ACTION_PER_EVENT) is terminated by specifying
 * an action code of QDP_ACTION_done (0).  An action code of -1 in the first column
 * means that the event it corresponds to can NEVER occur in the given state.
 * If the library ever encounters an action code of -1, then it indicates
 * a design flaw.  The library should fail loudly if that ever happens (which it
 * won't, because this code is solid :-).  A transition state of -1 means that
 * the action handler is required to specify the transition state.  This exists
 * because in order to decode the DT_DATA packets received on a connection to a
 * data port we need to have some additional meta-data.  Once the basic Q330
 * registration is completed (ie, the server challenge response is acknowledges
 * by the digitizer) then non-data port connections transition immediately
 * to the ready state while data port continues on for several more steps
 * to request and save those meta-data.
 */

#ifdef INCLUDE_QDP_STATE_MACHINE

static QDP_EVENT_HANDLER QDP_STATE_MACHINE[QDP_NUM_STATE] = {

    { /* QDP_FSA state 0: QDP_STATE_NEEDSN: need to poll for serial number (start here for console connections) */
        /* --- EVENT ---  ----------------------------- ACTIONS -----------------------------  --- NEXT STATE --- */
        /* UP         */  QDP_ACTION_START,      QDP_ACTION_POLLSN,     QDP_ACTION_done,       QDP_STATE_WAITSN,
        /* MYSN       */  QDP_ACTION_DROP,       QDP_ACTION_done,       -1,                    QDP_STATE_nochange,
        /* SRVCH      */  QDP_ACTION_DROP,       QDP_ACTION_done,       -1,                    QDP_STATE_nochange,
        /* NOTREG     */  QDP_ACTION_DROP,       QDP_ACTION_done,       -1,                    QDP_STATE_nochange,
        /* CERR       */  QDP_ACTION_DROP,       QDP_ACTION_done,       -1,                    QDP_STATE_nochange,
        /* CACK       */  QDP_ACTION_DROP,       QDP_ACTION_done,       -1,                    QDP_STATE_nochange,
        /* C1_FIX     */  QDP_ACTION_DROP,       QDP_ACTION_done,       -1,                    QDP_STATE_nochange,
        /* C1_LOG     */  QDP_ACTION_DROP,       QDP_ACTION_done,       -1,                    QDP_STATE_nochange,
        /* C2_EPD     */  QDP_ACTION_DROP,       QDP_ACTION_done,       -1,                    QDP_STATE_nochange,
        /* C1_FLGS    */  QDP_ACTION_DROP,       QDP_ACTION_done,       -1,                    QDP_STATE_nochange,
        /* TOKEN_SOME */  QDP_ACTION_DROP,       QDP_ACTION_done,       -1,                    QDP_STATE_nochange,
        /* TOKEN_DONE */  QDP_ACTION_DROP,       QDP_ACTION_done,       -1,                    QDP_STATE_nochange,
        /* CTRL       */  QDP_ACTION_DROP,       QDP_ACTION_done,       -1,                    QDP_STATE_nochange,
        /* CRCERR     */  QDP_ACTION_done,       -1,                    -1,                    QDP_STATE_nochange,
        /* IOERR      */  QDP_ACTION_IOERR,      QDP_ACTION_done,       -1,                    QDP_STATE_OFF,
        /* WATCHDOG   */  QDP_ACTION_done,       -1,                    -1,                    QDP_STATE_nochange,
        /* CTO        */  QDP_ACTION_done,       -1,                    -1,                    QDP_STATE_nochange,
        /* DTO        */  QDP_ACTION_done,       -1,                    -1,                    QDP_STATE_nochange,
        /* DATA       */  QDP_ACTION_DROP,       QDP_ACTION_done,       -1,                    QDP_STATE_nochange,
        /* FILL       */  QDP_ACTION_DROP,       QDP_ACTION_done,       -1,                    QDP_STATE_nochange,
        /* UKNDATA    */  QDP_ACTION_DROP,       QDP_ACTION_done,       -1,                    QDP_STATE_nochange,
        /* HBEAT      */  QDP_ACTION_done,       -1,                    -1,                    QDP_STATE_nochange,
        /* APPCMD     */  QDP_ACTION_done,       -1,                    -1,                    QDP_STATE_nochange,
        /* OFF        */  QDP_ACTION_TLD,        QDP_ACTION_done,       -1,                    QDP_STATE_OFF
    },

    { /* QDP_FSA state 1: QDP_STATE_WAITSN: waiting for serial number */
        /* --- EVENT ---  ----------------------------- ACTIONS -----------------------------  --- NEXT STATE --- */
        /* UP         */  -1,                    -1,                    -1,                    -1,
        /* MYSN       */  QDP_ACTION_LDSN,       QDP_ACTION_SRVRQ,      QDP_ACTION_done,       QDP_STATE_RQSRV,
        /* SRVCH      */  QDP_ACTION_DROP,       QDP_ACTION_done,       -1,                    QDP_STATE_nochange,
        /* NOTREG     */  QDP_ACTION_DROP,       QDP_ACTION_done,       -1,                    QDP_STATE_nochange,
        /* CERR       */  QDP_ACTION_REGERR,     QDP_ACTION_done,       -1,                    QDP_STATE_OFF,
        /* CACK       */  QDP_ACTION_DROP,       QDP_ACTION_done,       -1,                    QDP_STATE_nochange,
        /* C1_FIX     */  QDP_ACTION_DROP,       QDP_ACTION_done,       -1,                    QDP_STATE_nochange,
        /* C1_LOG     */  QDP_ACTION_DROP,       QDP_ACTION_done,       -1,                    QDP_STATE_nochange,
        /* C2_EPD     */  QDP_ACTION_DROP,       QDP_ACTION_done,       -1,                    QDP_STATE_nochange,
        /* C1_FLGS    */  QDP_ACTION_DROP,       QDP_ACTION_done,       -1,                    QDP_STATE_nochange,
        /* TOKEN_SOME */  QDP_ACTION_DROP,       QDP_ACTION_done,       -1,                    QDP_STATE_nochange,
        /* TOKEN_DONE */  QDP_ACTION_DROP,       QDP_ACTION_done,       -1,                    QDP_STATE_nochange,
        /* CTRL       */  QDP_ACTION_DROP,       QDP_ACTION_done,       -1,                    QDP_STATE_nochange,
        /* CRCERR     */  QDP_ACTION_CRCERR,     QDP_ACTION_POLLSN,     QDP_ACTION_done,       QDP_STATE_nochange,
        /* IOERR      */  QDP_ACTION_IOERR,      QDP_ACTION_done,       -1,                    QDP_STATE_OFF,
        /* WATCHDOG   */  QDP_ACTION_WATCHDOG,   QDP_ACTION_done,       -1,                    QDP_STATE_OFF,
        /* CTO        */  QDP_ACTION_POLLSN,     QDP_ACTION_done,       -1,                    QDP_STATE_nochange,
        /* DTO        */  QDP_ACTION_done,       -1,                    -1,                    QDP_STATE_nochange,
        /* DATA       */  QDP_ACTION_DROP,       QDP_ACTION_done,       -1,                    QDP_STATE_nochange,
        /* FILL       */  QDP_ACTION_DROP,       QDP_ACTION_done,       -1,                    QDP_STATE_nochange,
        /* UKNDATA    */  QDP_ACTION_DROP,       QDP_ACTION_done,       -1,                    QDP_STATE_nochange,
        /* HBEAT      */  QDP_ACTION_done,       -1,                    -1,                    QDP_STATE_nochange,
        /* APPCMD     */  QDP_ACTION_done,       -1,                    -1,                    QDP_STATE_nochange,
        /* OFF        */  QDP_ACTION_TLD,        QDP_ACTION_done,       -1,                    QDP_STATE_OFF
    },

    { /* QDP_FSA state 2: unregistered (start here for non-console connections (when serial number is known) */
        /* --- EVENT ---  ----------------------------- ACTIONS -----------------------------  --- NEXT STATE --- */
        /* UP         */  QDP_ACTION_START,      QDP_ACTION_SRVRQ,      QDP_ACTION_done,       QDP_STATE_RQSRV,
        /* MYSN       */  QDP_ACTION_DROP,       QDP_ACTION_done,       -1,                    QDP_STATE_nochange,
        /* SRVCH      */  QDP_ACTION_DROP,       QDP_ACTION_done,       -1,                    QDP_STATE_nochange,
        /* NOTREG     */  QDP_ACTION_DROP,       QDP_ACTION_done,       -1,                    QDP_STATE_nochange,
        /* CERR       */  QDP_ACTION_DROP,       QDP_ACTION_done,       -1,                    QDP_STATE_nochange,
        /* CACK       */  QDP_ACTION_DROP,       QDP_ACTION_done,       -1,                    QDP_STATE_nochange,
        /* C1_FIX     */  QDP_ACTION_DROP,       QDP_ACTION_done,       -1,                    QDP_STATE_nochange,
        /* C1_LOG     */  QDP_ACTION_DROP,       QDP_ACTION_done,       -1,                    QDP_STATE_nochange,
        /* C2_EPD     */  QDP_ACTION_DROP,       QDP_ACTION_done,       -1,                    QDP_STATE_nochange,
        /* C1_FLGS    */  QDP_ACTION_DROP,       QDP_ACTION_done,       -1,                    QDP_STATE_nochange,
        /* TOKEN_SOME */  QDP_ACTION_DROP,       QDP_ACTION_done,       -1,                    QDP_STATE_nochange,
        /* TOKEN_DONE */  QDP_ACTION_DROP,       QDP_ACTION_done,       -1,                    QDP_STATE_nochange,
        /* CTRL       */  QDP_ACTION_DROP,       QDP_ACTION_done,       -1,                    QDP_STATE_nochange,
        /* CRCERR     */  QDP_ACTION_done,       -1,                    -1,                    QDP_STATE_nochange,
        /* IOERR      */  QDP_ACTION_IOERR,      QDP_ACTION_done,       -1,                    QDP_STATE_OFF,
        /* WATCHDOG   */  QDP_ACTION_done,       -1,                    -1,                    -1,
        /* CTO        */  QDP_ACTION_done,       -1,                    -1,                    -1,
        /* DTO        */  QDP_ACTION_done,       -1,                    -1,                    QDP_STATE_nochange,
        /* DATA       */  QDP_ACTION_DROP,       QDP_ACTION_done,       -1,                    QDP_STATE_nochange,
        /* FILL       */  QDP_ACTION_DROP,       QDP_ACTION_done,       -1,                    QDP_STATE_nochange,
        /* UKNDATA    */  QDP_ACTION_DROP,       QDP_ACTION_done,       -1,                    QDP_STATE_nochange,
        /* HBEAT      */  QDP_ACTION_done,       -1,                    -1,                    QDP_STATE_nochange,
        /* APPCMD     */  QDP_ACTION_done,       -1,                    -1,                    -1,
        /* OFF        */  QDP_ACTION_TLD,        QDP_ACTION_done,       -1,                    QDP_STATE_OFF
    },

    { /* QDP_FSA state 3: QDP_STATE_RQSRV: registration request has been sent */
        /* --- EVENT ---  ----------------------------- ACTIONS -----------------------------  --- NEXT STATE --- */
        /* UP         */  -1,                    -1,                    -1,                    -1,
        /* MYSN       */  QDP_ACTION_DROP,       QDP_ACTION_done,       -1,                    QDP_STATE_nochange,
        /* SRVCH      */  QDP_ACTION_DECODECH,   QDP_ACTION_BLDRSP,     QDP_ACTION_SEND,       QDP_STATE_SRVRSP,
        /* NOTREG     */  QDP_ACTION_DROP,       QDP_ACTION_SEND,       QDP_ACTION_done,       QDP_STATE_nochange,
        /* CERR       */  QDP_ACTION_REGERR,     QDP_ACTION_done,       -1,                    QDP_STATE_OFF,
        /* CACK       */  QDP_ACTION_DROP,       QDP_ACTION_done,       -1,                    QDP_STATE_nochange,
        /* C1_FIX     */  QDP_ACTION_DROP,       QDP_ACTION_done,       -1,                    QDP_STATE_nochange,
        /* C1_LOG     */  QDP_ACTION_DROP,       QDP_ACTION_done,       -1,                    QDP_STATE_nochange,
        /* C2_EPD     */  QDP_ACTION_DROP,       QDP_ACTION_done,       -1,                    QDP_STATE_nochange,
        /* C1_FLGS    */  QDP_ACTION_DROP,       QDP_ACTION_done,       -1,                    QDP_STATE_nochange,
        /* TOKEN_SOME */  QDP_ACTION_DROP,       QDP_ACTION_done,       -1,                    QDP_STATE_nochange,
        /* TOKEN_DONE */  QDP_ACTION_DROP,       QDP_ACTION_done,       -1,                    QDP_STATE_nochange,
        /* CTRL       */  QDP_ACTION_DROP,       QDP_ACTION_done,       -1,                    QDP_STATE_nochange,
        /* CRCERR     */  QDP_ACTION_CRCERR,     QDP_ACTION_SEND,       QDP_ACTION_done,       QDP_STATE_nochange,
        /* IOERR      */  QDP_ACTION_IOERR,      QDP_ACTION_done,       -1,                    QDP_STATE_OFF,
        /* WATCHDOG   */  QDP_ACTION_WATCHDOG,   QDP_ACTION_done,       -1,                    QDP_STATE_OFF,
        /* CTO        */  QDP_ACTION_SEND,       QDP_ACTION_done,       -1,                    QDP_STATE_nochange,
        /* DTO        */  QDP_ACTION_done,       -1,                    -1,                    QDP_STATE_nochange,
        /* DATA       */  QDP_ACTION_DROP,       QDP_ACTION_done,       -1,                    QDP_STATE_nochange,
        /* FILL       */  QDP_ACTION_DROP,       QDP_ACTION_done,       -1,                    QDP_STATE_nochange,
        /* UKNDATA    */  QDP_ACTION_DROP,       QDP_ACTION_done,       -1,                    QDP_STATE_nochange,
        /* HBEAT      */  QDP_ACTION_done,       -1,                    -1,                    QDP_STATE_nochange,
        /* APPCMD     */  QDP_ACTION_done,       -1,                    -1,                    -1,
        /* OFF        */  QDP_ACTION_TLD,        QDP_ACTION_done,       -1,                    QDP_STATE_OFF
    },

    { /* QDP_FSA state 4: QDP_STATE_SRVRSP: challenge response has been sent */
        /* --- EVENT ---  ----------------------------- ACTIONS -----------------------------  --- NEXT STATE --- */
        /* UP         */  -1,                    -1,                    -1,                    -1,
        /* MYSN       */  QDP_ACTION_DROP,       QDP_ACTION_done,       -1,                    QDP_STATE_nochange,
        /* SRVCH      */  QDP_ACTION_DROP,       QDP_ACTION_done,       -1,                    QDP_STATE_nochange,
        /* NOTREG     */  QDP_ACTION_DROP,       QDP_ACTION_done,       -1,                    QDP_STATE_nochange,
        /* CERR       */  QDP_ACTION_REGERR,     QDP_ACTION_done,       -1,                    QDP_STATE_OFF,
        /* CACK       */  QDP_ACTION_RQ_C1_FIX,  QDP_ACTION_done,       -1,                    QDP_STATE_WAIT_C1_FIX,
        /* C1_FIX     */  QDP_ACTION_DROP,       QDP_ACTION_done,       -1,                    QDP_STATE_nochange,
        /* C1_LOG     */  QDP_ACTION_DROP,       QDP_ACTION_done,       -1,                    QDP_STATE_nochange,
        /* C2_EPD     */  QDP_ACTION_DROP,       QDP_ACTION_done,       -1,                    QDP_STATE_nochange,
        /* C1_FLGS    */  QDP_ACTION_DROP,       QDP_ACTION_done,       -1,                    QDP_STATE_nochange,
        /* TOKEN_SOME */  QDP_ACTION_DROP,       QDP_ACTION_done,       -1,                    QDP_STATE_nochange,
        /* TOKEN_DONE */  QDP_ACTION_DROP,       QDP_ACTION_done,       -1,                    QDP_STATE_nochange,
        /* CTRL       */  QDP_ACTION_DROP,       QDP_ACTION_done,       -1,                    QDP_STATE_nochange,
        /* CRCERR     */  QDP_ACTION_CRCERR,     QDP_ACTION_SEND,       QDP_ACTION_done,       QDP_STATE_nochange,
        /* IOERR      */  QDP_ACTION_IOERR,      QDP_ACTION_done,       -1,                    QDP_STATE_OFF,
        /* WATCHDOG   */  QDP_ACTION_WATCHDOG,   QDP_ACTION_done,       -1,                    QDP_STATE_OFF,
        /* CTO        */  QDP_ACTION_SEND,       QDP_ACTION_done,       -1,                    QDP_STATE_nochange,
        /* DTO        */  QDP_ACTION_done,       -1,                    -1,                    QDP_STATE_nochange,
        /* DATA       */  QDP_ACTION_DROP,       QDP_ACTION_done,       -1,                    QDP_STATE_nochange,
        /* FILL       */  QDP_ACTION_DROP,       QDP_ACTION_done,       -1,                    QDP_STATE_nochange,
        /* UKNDATA    */  QDP_ACTION_DROP,       QDP_ACTION_done,       -1,                    QDP_STATE_nochange,
        /* HBEAT      */  QDP_ACTION_done,       -1,                    -1,                    QDP_STATE_nochange,
        /* APPCMD     */  QDP_ACTION_done,       -1,                    -1,                    -1,
        /* OFF        */  QDP_ACTION_TLD,        QDP_ACTION_done,       -1,                    QDP_STATE_OFF
    },

    { /* QDP_FSA state 5: QDP_STATE_WAIT_C1_FIX: waiting for C1_FIX (data port connections only) */
        /* --- EVENT ---  ----------------------------- ACTIONS -----------------------------  --- NEXT STATE --- */
        /* UP         */  -1,                    -1,                    -1,                    -1,
        /* MYSN       */  QDP_ACTION_DROP,       QDP_ACTION_done,       -1,                    QDP_STATE_nochange,
        /* SRVCH      */  QDP_ACTION_DROP,       QDP_ACTION_done,       -1,                    QDP_STATE_nochange,
        /* NOTREG     */  QDP_ACTION_SRVRQ,      QDP_ACTION_done,       -1,                    QDP_STATE_RQSRV,
        /* CERR       */  QDP_ACTION_REGERR,     QDP_ACTION_done,       -1,                    QDP_STATE_OFF,
        /* CACK       */  QDP_ACTION_DROP,       QDP_ACTION_done,       -1,                    QDP_STATE_nochange,
        /* C1_FIX     */  QDP_ACTION_LD_C1_FIX,  QDP_ACTION_REGISTERED, QDP_ACTION_done,       -1, /* state set by QDP_ACTION_REGISTERED */
        /* C1_LOG     */  QDP_ACTION_DROP,       QDP_ACTION_done,       -1,                    QDP_STATE_nochange,
        /* C2_EPD     */  QDP_ACTION_DROP,       QDP_ACTION_done,       -1,                    QDP_STATE_nochange,
        /* C1_FLGS    */  QDP_ACTION_DROP,       QDP_ACTION_done,       -1,                    QDP_STATE_nochange,
        /* TOKEN_SOME */  QDP_ACTION_DROP,       QDP_ACTION_done,       -1,                    QDP_STATE_nochange,
        /* TOKEN_DONE */  QDP_ACTION_DROP,       QDP_ACTION_done,       -1,                    QDP_STATE_nochange,
        /* CTRL       */  QDP_ACTION_DROP,       QDP_ACTION_done,       -1,                    QDP_STATE_nochange,
        /* CRCERR     */  QDP_ACTION_CRCERR,     QDP_ACTION_SEND,       QDP_ACTION_done,       QDP_STATE_nochange,
        /* IOERR      */  QDP_ACTION_IOERR,      QDP_ACTION_done,       -1,                    QDP_STATE_OFF,
        /* WATCHDOG   */  QDP_ACTION_WATCHDOG,   QDP_ACTION_done,       -1,                    QDP_STATE_OFF,
        /* CTO        */  QDP_ACTION_RQ_C1_FIX,  QDP_ACTION_done,       -1,                    QDP_STATE_nochange,
        /* DTO        */  QDP_ACTION_done,       -1,                    -1,                    QDP_STATE_nochange,
        /* DATA       */  QDP_ACTION_DROP,       QDP_ACTION_done,       -1,                    QDP_STATE_nochange,
        /* FILL       */  QDP_ACTION_DROP,       QDP_ACTION_done,       -1,                    QDP_STATE_nochange,
        /* UKNDATA    */  QDP_ACTION_DROP,       QDP_ACTION_done,       -1,                    QDP_STATE_nochange,
        /* HBEAT      */  QDP_ACTION_done,       -1,                    -1,                    QDP_STATE_nochange,
        /* APPCMD     */  QDP_ACTION_done,       -1,                    -1,                    -1,
        /* OFF        */  QDP_ACTION_TLD,        QDP_ACTION_done,       -1,                    QDP_STATE_OFF
    },

    { /* QDP_FSA state 6: QDP_STATE_WAIT_C1_LOG: waiting for C1_LOG (data port connections only) */
        /* --- EVENT ---  ----------------------------- ACTIONS -----------------------------  --- NEXT STATE --- */
        /* UP         */  -1,                    -1,                    -1,                    -1,
        /* MYSN       */  QDP_ACTION_DROP,       QDP_ACTION_done,       -1,                    QDP_STATE_nochange,
        /* SRVCH      */  QDP_ACTION_DROP,       QDP_ACTION_done,       -1,                    QDP_STATE_nochange,
        /* NOTREG     */  QDP_ACTION_SRVRQ,      QDP_ACTION_done,       -1,                    QDP_STATE_RQSRV,
        /* CERR       */  QDP_ACTION_REGERR,     QDP_ACTION_done,       -1,                    QDP_STATE_OFF,
        /* CACK       */  QDP_ACTION_DROP,       QDP_ACTION_done,       -1,                    QDP_STATE_nochange,
        /* C1_FIX     */  QDP_ACTION_DROP,       QDP_ACTION_done,       -1,                    QDP_STATE_nochange,
        /* C1_LOG     */  QDP_ACTION_LD_C1_LOG,  QDP_ACTION_done,       -1,                    -1, /* state set by QDP_ACTION_LD_C1_LOG */
        /* C2_EPD     */  QDP_ACTION_DROP,       QDP_ACTION_done,       -1,                    QDP_STATE_nochange,
        /* C1_FLGS    */  QDP_ACTION_DROP,       QDP_ACTION_done,       -1,                    QDP_STATE_nochange,
        /* TOKEN_SOME */  QDP_ACTION_DROP,       QDP_ACTION_done,       -1,                    QDP_STATE_nochange,
        /* TOKEN_DONE */  QDP_ACTION_DROP,       QDP_ACTION_done,       -1,                    QDP_STATE_nochange,
        /* CTRL       */  QDP_ACTION_DROP,       QDP_ACTION_done,       -1,                    QDP_STATE_nochange,
        /* CRCERR     */  QDP_ACTION_CRCERR,     QDP_ACTION_SEND,       QDP_ACTION_done,       QDP_STATE_nochange,
        /* IOERR      */  QDP_ACTION_IOERR,      QDP_ACTION_done,       -1,                    QDP_STATE_OFF,
        /* WATCHDOG   */  QDP_ACTION_WATCHDOG,   QDP_ACTION_done,       -1,                    QDP_STATE_OFF,
        /* CTO        */  QDP_ACTION_RQ_C1_LOG,  QDP_ACTION_done,       -1,                    QDP_STATE_nochange,
        /* DTO        */  QDP_ACTION_done,       -1,                    -1,                    QDP_STATE_nochange,
        /* DATA       */  QDP_ACTION_DROP,       QDP_ACTION_done,       -1,                    QDP_STATE_nochange,
        /* FILL       */  QDP_ACTION_DROP,       QDP_ACTION_done,       -1,                    QDP_STATE_nochange,
        /* UKNDATA    */  QDP_ACTION_DROP,       QDP_ACTION_done,       -1,                    QDP_STATE_nochange,
        /* HBEAT      */  QDP_ACTION_done,       -1,                    -1,                    QDP_STATE_nochange,
        /* APPCMD     */  QDP_ACTION_done,       -1,                    -1,                    -1,
        /* OFF        */  QDP_ACTION_TLD,        QDP_ACTION_done,       -1,                    QDP_STATE_OFF
    },

    { /* QDP_FSA state 7: QDP_STATE_WAIT_C2_EPD: waiting for C2_EPD (data port connections only) */
        /* --- EVENT ---  ----------------------------- ACTIONS -----------------------------  --- NEXT STATE --- */
        /* UP         */  -1,                    -1,                    -1,                    -1,
        /* MYSN       */  QDP_ACTION_DROP,       QDP_ACTION_done,       -1,                    QDP_STATE_nochange,
        /* SRVCH      */  QDP_ACTION_DROP,       QDP_ACTION_done,       -1,                    QDP_STATE_nochange,
        /* NOTREG     */  QDP_ACTION_SRVRQ,      QDP_ACTION_done,       -1,                    QDP_STATE_RQSRV,
        /* CERR       */  QDP_ACTION_REGERR,     QDP_ACTION_done,       -1,                    QDP_STATE_OFF,
        /* CACK       */  QDP_ACTION_DROP,       QDP_ACTION_done,       -1,                    QDP_STATE_nochange,
        /* C1_FIX     */  QDP_ACTION_DROP,       QDP_ACTION_done,       -1,                    QDP_STATE_nochange,
        /* C1_LOG     */  QDP_ACTION_DROP,       QDP_ACTION_done,       -1,                    QDP_STATE_nochange,
        /* C2_EPD     */  QDP_ACTION_LD_C2_EPD,  QDP_ACTION_RQ_C1_FLGS, QDP_ACTION_done,       QDP_STATE_WAIT_C1_FLGS,
        /* C1_FLGS    */  QDP_ACTION_DROP,       QDP_ACTION_done,       -1,                    QDP_STATE_nochange,
        /* TOKEN_SOME */  QDP_ACTION_DROP,       QDP_ACTION_done,       -1,                    QDP_STATE_nochange,
        /* TOKEN_DONE */  QDP_ACTION_DROP,       QDP_ACTION_done,       -1,                    QDP_STATE_nochange,
        /* CTRL       */  QDP_ACTION_DROP,       QDP_ACTION_done,       -1,                    QDP_STATE_nochange,
        /* CRCERR     */  QDP_ACTION_CRCERR,     QDP_ACTION_SEND,       QDP_ACTION_done,       QDP_STATE_nochange,
        /* IOERR      */  QDP_ACTION_IOERR,      QDP_ACTION_done,       -1,                    QDP_STATE_OFF,
        /* WATCHDOG   */  QDP_ACTION_WATCHDOG,   QDP_ACTION_done,       -1,                    QDP_STATE_OFF,
        /* CTO        */  QDP_ACTION_RQ_C2_EPD,  QDP_ACTION_done,       -1,                    QDP_STATE_nochange,
        /* DTO        */  QDP_ACTION_done,       -1,                    -1,                    QDP_STATE_nochange,
        /* DATA       */  QDP_ACTION_DROP,       QDP_ACTION_done,       -1,                    QDP_STATE_nochange,
        /* FILL       */  QDP_ACTION_DROP,       QDP_ACTION_done,       -1,                    QDP_STATE_nochange,
        /* UKNDATA    */  QDP_ACTION_DROP,       QDP_ACTION_done,       -1,                    QDP_STATE_nochange,
        /* HBEAT      */  QDP_ACTION_done,       -1,                    -1,                    QDP_STATE_nochange,
        /* APPCMD     */  QDP_ACTION_done,       -1,                    -1,                    -1,
        /* OFF        */  QDP_ACTION_TLD,        QDP_ACTION_done,       -1,                    QDP_STATE_OFF
    },

    { /* QDP_FSA state 8: QDP_STATE_WAIT_C1_FLGS: waiting for C1_FLGS (data port connections only) */
        /* --- EVENT ---  ----------------------------- ACTIONS -----------------------------  --- NEXT STATE --- */
        /* UP         */  -1,                    -1,                    -1,                    -1,
        /* MYSN       */  QDP_ACTION_DROP,       QDP_ACTION_done,       -1,                    QDP_STATE_nochange,
        /* SRVCH      */  QDP_ACTION_DROP,       QDP_ACTION_done,       -1,                    QDP_STATE_nochange,
        /* NOTREG     */  QDP_ACTION_SRVRQ,      QDP_ACTION_done,       -1,                    QDP_STATE_RQSRV,
        /* CERR       */  QDP_ACTION_REGERR,     QDP_ACTION_done,       -1,                    QDP_STATE_OFF,
        /* CACK       */  QDP_ACTION_DROP,       QDP_ACTION_done,       -1,                    QDP_STATE_nochange,
        /* C1_FIX     */  QDP_ACTION_DROP,       QDP_ACTION_done,       -1,                    QDP_STATE_nochange,
        /* C1_LOG     */  QDP_ACTION_DROP,       QDP_ACTION_done,       -1,                    QDP_STATE_nochange,
        /* C2_EPD     */  QDP_ACTION_DROP,       QDP_ACTION_done,       -1,                    QDP_STATE_nochange,
        /* C1_FLGS    */  QDP_ACTION_LD_C1_FLGS, QDP_ACTION_RQ_TOKENS,  QDP_ACTION_done,       QDP_STATE_WAITTOKEN,
        /* TOKEN_SOME */  QDP_ACTION_DROP,       QDP_ACTION_done,       -1,                    QDP_STATE_nochange,
        /* TOKEN_DONE */  QDP_ACTION_DROP,       QDP_ACTION_done,       -1,                    QDP_STATE_nochange,
        /* CTRL       */  QDP_ACTION_DROP,       QDP_ACTION_done,       -1,                    QDP_STATE_nochange,
        /* CRCERR     */  QDP_ACTION_CRCERR,     QDP_ACTION_SEND,       QDP_ACTION_done,       QDP_STATE_nochange,
        /* IOERR      */  QDP_ACTION_IOERR,      QDP_ACTION_done,       -1,                    QDP_STATE_OFF,
        /* WATCHDOG   */  QDP_ACTION_WATCHDOG,   QDP_ACTION_done,       -1,                    QDP_STATE_OFF,
        /* CTO        */  QDP_ACTION_RQ_C1_FLGS, QDP_ACTION_done,       -1,                    QDP_STATE_nochange,
        /* DTO        */  QDP_ACTION_done,       -1,                    -1,                    QDP_STATE_nochange,
        /* DATA       */  QDP_ACTION_DROP,       QDP_ACTION_done,       -1,                    QDP_STATE_nochange,
        /* FILL       */  QDP_ACTION_DROP,       QDP_ACTION_done,       -1,                    QDP_STATE_nochange,
        /* UKNDATA    */  QDP_ACTION_DROP,       QDP_ACTION_done,       -1,                    QDP_STATE_nochange,
        /* HBEAT      */  QDP_ACTION_done,       -1,                    -1,                    QDP_STATE_nochange,
        /* APPCMD     */  QDP_ACTION_done,       -1,                    -1,                    -1,
        /* OFF        */  QDP_ACTION_TLD,        QDP_ACTION_done,       -1,                    QDP_STATE_OFF
    },

    { /* QDP_FSA state 9: QDP_STATE_WAITTOKEN: waiting for DP tokens (data port connections only) */
        /* --- EVENT ---  ----------------------------- ACTIONS -----------------------------  --- NEXT STATE --- */
        /* UP         */  -1,                    -1,                    -1,                    -1,
        /* MYSN       */  QDP_ACTION_DROP,       QDP_ACTION_done,       -1,                    QDP_STATE_nochange,
        /* SRVCH      */  QDP_ACTION_DROP,       QDP_ACTION_done,       -1,                    QDP_STATE_nochange,
        /* NOTREG     */  QDP_ACTION_SRVRQ,      QDP_ACTION_done,       -1,                    QDP_STATE_RQSRV,
        /* CERR       */  QDP_ACTION_REGERR,     QDP_ACTION_done,       -1,                    QDP_STATE_OFF,
        /* CACK       */  QDP_ACTION_DROP,       QDP_ACTION_done,       -1,                    QDP_STATE_nochange,
        /* C1_FIX     */  QDP_ACTION_DROP,       QDP_ACTION_done,       -1,                    QDP_STATE_nochange,
        /* C1_LOG     */  QDP_ACTION_DROP,       QDP_ACTION_done,       -1,                    QDP_STATE_nochange,
        /* C2_EPD     */  QDP_ACTION_DROP,       QDP_ACTION_done,       -1,                    QDP_STATE_nochange,
        /* C1_FLGS    */  QDP_ACTION_DROP,       QDP_ACTION_done,       -1,                    QDP_STATE_nochange,
        /* TOKEN_SOME */  QDP_ACTION_LD_TOKENS,  QDP_ACTION_RQ_TOKENS,  QDP_ACTION_done,       QDP_STATE_WAITTOKEN,
        /* TOKEN_DONE */  QDP_ACTION_LD_TOKENS,  QDP_ACTION_OPEN,       QDP_ACTION_TLU,        QDP_STATE_READY,
        /* CTRL       */  QDP_ACTION_DROP,       QDP_ACTION_done,       -1,                    QDP_STATE_nochange,
        /* CRCERR     */  QDP_ACTION_CRCERR,     QDP_ACTION_SEND,       QDP_ACTION_done,       QDP_STATE_nochange,
        /* IOERR      */  QDP_ACTION_IOERR,      QDP_ACTION_done,       -1,                    QDP_STATE_OFF,
        /* WATCHDOG   */  QDP_ACTION_WATCHDOG,   QDP_ACTION_done,       -1,                    QDP_STATE_OFF,
        /* CTO        */  QDP_ACTION_RQ_TOKENS,  QDP_ACTION_done,       -1,                    QDP_STATE_nochange,
        /* DTO        */  QDP_ACTION_done,       -1,                    -1,                    QDP_STATE_nochange,
        /* DATA       */  QDP_ACTION_DROP,       QDP_ACTION_done,       -1,                    QDP_STATE_nochange,
        /* FILL       */  QDP_ACTION_DROP,       QDP_ACTION_done,       -1,                    QDP_STATE_nochange,
        /* UKNDATA    */  QDP_ACTION_DROP,       QDP_ACTION_done,       -1,                    QDP_STATE_nochange,
        /* HBEAT      */  QDP_ACTION_done,       -1,                    -1,                    QDP_STATE_nochange,
        /* APPCMD     */  QDP_ACTION_done,       -1,                    -1,                    -1,
        /* OFF        */  QDP_ACTION_TLD,        QDP_ACTION_done,       -1,                    QDP_STATE_OFF
    },

    { /* QDP_FSA state 10: QDP_STATE_READY: handshake complete */
        /* --- EVENT ---  ----------------------------- ACTIONS -----------------------------  --- NEXT STATE --- */
        /* UP         */  -1,                    -1,                    -1,                    -1,
        /* MYSN       */  QDP_ACTION_CTRL,       QDP_ACTION_done,       -1,                    QDP_STATE_nochange,
        /* SRVCH      */  QDP_ACTION_CTRL,       QDP_ACTION_done,       -1,                    QDP_STATE_nochange,
        /* NOTREG     */  QDP_ACTION_FLUSH,      QDP_ACTION_START,      QDP_ACTION_SRVRQ,      QDP_STATE_RQSRV,
        /* CERR       */  QDP_ACTION_CERR,       QDP_ACTION_done,       -1,                    QDP_STATE_nochange,
        /* CACK       */  QDP_ACTION_CACK,       QDP_ACTION_done,       -1,                    QDP_STATE_nochange,
        /* C1_FIX     */  QDP_ACTION_CTRL,       QDP_ACTION_done,       -1,                    QDP_STATE_nochange,
        /* C1_LOG     */  QDP_ACTION_CTRL,       QDP_ACTION_done,       -1,                    QDP_STATE_nochange,
        /* C2_EPD     */  QDP_ACTION_CTRL,       QDP_ACTION_done,       -1,                    QDP_STATE_nochange,
        /* C1_FLGS    */  QDP_ACTION_CTRL,       QDP_ACTION_done,       -1,                    QDP_STATE_nochange,
        /* TOKEN_SOME */  QDP_ACTION_CTRL,       QDP_ACTION_done,       -1,                    QDP_STATE_nochange,
        /* TOKEN_DONE */  QDP_ACTION_CTRL,       QDP_ACTION_done,       -1,                    QDP_STATE_nochange,
        /* CTRL       */  QDP_ACTION_CTRL,       QDP_ACTION_done,       -1,                    QDP_STATE_nochange,
        /* CRCERR     */  QDP_ACTION_CRCERR,     QDP_ACTION_done,       -1,                    QDP_STATE_nochange,
        /* IOERR      */  QDP_ACTION_IOERR,      QDP_ACTION_done,       -1,                    QDP_STATE_OFF,
        /* WATCHDOG   */  -1,                    -1,                    -1,                    -1,
        /* CTO        */  QDP_ACTION_APPCMD,     QDP_ACTION_done,       -1,                    QDP_STATE_nochange,
        /* DTO        */  QDP_ACTION_DACK,       QDP_ACTION_done,       -1,                    QDP_STATE_nochange,
        /* DATA       */  QDP_ACTION_DATA,       QDP_ACTION_DACK,       QDP_ACTION_done,       QDP_STATE_nochange,
        /* FILL       */  QDP_ACTION_FILL,       QDP_ACTION_DACK,       -1,                    QDP_STATE_nochange,
        /* UKNDATA    */  QDP_ACTION_DROP,       QDP_ACTION_DACK,       -1,                    QDP_STATE_nochange,
        /* HBEAT      */  QDP_ACTION_HBEAT,      QDP_ACTION_done,       -1,                    QDP_STATE_nochange,
        /* APPCMD     */  QDP_ACTION_APPCMD,     QDP_ACTION_done,       -1,                    QDP_STATE_nochange,
        /* OFF        */  QDP_ACTION_TLD,        QDP_ACTION_done,       -1,                    QDP_STATE_OFF
    },

    { /* QDP_FSA state 11: QDP_STATE_OFF: automaton is off */
        /* --- EVENT ---  ----------------------------- ACTIONS -----------------------------  --- NEXT STATE --- */
        /* UP         */  -1,                    -1,                    -1,                    -1,
        /* MYSN       */  -1,                    -1,                    -1,                    -1,
        /* SRVCH      */  -1,                    -1,                    -1,                    -1,
        /* NOTREG     */  -1,                    -1,                    -1,                    -1,
        /* CERR       */  -1,                    -1,                    -1,                    -1,
        /* CACK       */  -1,                    -1,                    -1,                    -1,
        /* C1_FIX     */  -1,                    -1,                    -1,                    -1,
        /* C1_LOG     */  -1,                    -1,                    -1,                    -1,
        /* C2_EPD     */  -1,                    -1,                    -1,                    -1,
        /* C1_FLGS    */  -1,                    -1,                    -1,                    -1,
        /* TOKEN_SOME */  -1,                    -1,                    -1,                    -1,
        /* TOKEN_DONE */  -1,                    -1,                    -1,                    -1,
        /* CTRL       */  -1,                    -1,                    -1,                    -1,
        /* CRCERR     */  -1,                    -1,                    -1,                    -1,
        /* IOERR      */  -1,                    -1,                    -1,                    -1,
        /* WATCHDOG   */  -1,                    -1,                    -1,                    -1,
        /* CTO        */  -1,                    -1,                    -1,                    -1,
        /* DTO        */  -1,                    -1,                    -1,                    -1,
        /* DATA       */  -1,                    -1,                    -1,                    -1,
        /* FILL       */  -1,                    -1,                    -1,                    -1,
        /* UKNDATA    */  -1,                    -1,                    -1,                    -1,
        /* HBEAT      */  -1,                    -1,                    -1,                    -1,
        /* APPCMD     */  -1,                    -1,                    -1,                    -1,
        /* OFF        */  -1,                    -1,                    -1,                    -1
    }
};

#endif /* INCLUDE_QDP_STATE_MACHINE */

#ifdef __cplusplus
}
#endif

#endif /* qdp_fsa_included */

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
 * $Log: fsa.h,v $
 * Revision 1.22  2016/08/26 20:02:37  dechavez
 * Removed QDP_ACTION_HBEAT as the first action for registered DTO events.  Now
 * heartbeats only get generated by HeartbeatThread()
 *
 * Revision 1.21  2016/08/04 21:18:53  dechavez
 * found/fixed many(!) problems with QDP_STATE_MACHINE contents
 *
 * Revision 1.20  2016/07/20 16:22:51  dechavez
 * MAJOR rework of design, mostly to handle handshake failure modes not previously encountered
 * but to also (hopefully) make it easier for somebody other than the original developer to understand.
 *
 * Revision 1.19  2016/06/16 15:32:45  dechavez
 * added QDP_ACTION_GIVEUP and modifed state 4 CERR response to invoke same
 *
 * Revision 1.18  2016/02/12 17:59:53  dechavez
 * added DTO handlers for early states of data connections
 *
 * Revision 1.17  2016/02/11 18:48:21  dechavez
 * introduced QDP_ACTION_DECODECH, reworked QDP_STATE_SRVRSP fsa entries to handle
 * no replies to challenge response (ie, bad auth code)
 *
 * Revision 1.16  2016/02/03 17:24:33  dechavez
 * Reworked automaton to include bringing in C1_FIX as part of the handshake
 * and then added QDP_ACTION_DECIDE to both eliminate the need for separate
 * C&C and data link automatons (hurrah!).
 *
 * Revision 1.15  2016/01/27 00:16:42  dechavez
 * Introduced recovering C2_EPD as part of the data FSA handshake.  This involved
 * added a new QDP_EVENT_EPD event which gets triggered by receipt of a C2_EPD
 * packet (in response to the new QDP_ACTION_RQEPD action, which is invoked once
 * the DP tokens have been read).  The QDP_EVENT_EPD event then causes
 * QDP_ACTION_LDEPD before transitioning to TLU.  All the action takes place
 * in the "DataFSA state 6: QDP_STATE_META" array.  This will break if the
 * Q330 doesn't support the C2_RQEPD command.  Most of our Q330s have version
 * 1.131 and this works with it even though the Quaterra QEP manual says
 * that 1.137 is the minimum.
 *
 * Changed QDP_STATE_META to QDP_STATE_META to more accurately reflect that
 * state (acquiring all QDP_META meta-data, not just the DP tokens).
 *
 * Revision 1.14  2014/08/11 18:08:14  dechavez
 * MAJOR CHANGES TO SUPPORT Q330 DATA COMM OVER SERIAL PORT (see 8/11/2014 comments in version.c)
 *
 * Revision 1.13  2011/02/03 17:48:18  dechavez
 * added QDP_EVENT_DOWN, invoke QDP_ACTION_TLD in all cases
 *
 * Revision 1.12  2011/01/31 21:08:57  dechavez
 * added support for determining serial IP address
 *
 * Revision 1.11  2011/01/13 20:26:33  dechavez
 * added support for polling digitizer for serial number
 *
 * Revision 1.10  2010/03/22 21:30:17  dechavez
 * added QDP_ACTION_BUSY, changed TooMny servers response during handshake to invoke same
 *
 * Revision 1.9  2009/10/02 18:26:49  dechavez
 * Fixed the behavior in response to server busy during registration (it was
 * sending two registration requests... one right after waking up from the
 * sleep and one in response to the timeout.  Now we just wait for the timeout
 * for the next attempt.  The sleep is still there but could probably be
 * removed as well.
 *
 * Revision 1.8  2007/05/17 22:27:44  dechavez
 * initial production release
 *
 */
