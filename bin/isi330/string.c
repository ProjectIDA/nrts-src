/*======================================================================
 *
 *  String functions and constants
 *
 *====================================================================*/
#include "isi330.h"


static LIB330_TEXT_MAP PacketClassMap[] = {
    {"PKC_DATA",       PKC_DATA}, 
    {"PKC_EVENT",      PKC_EVENT}, 
    {"PKC_CALIBRATE",  PKC_CALIBRATE}, 
    {"PKC_TIMING",     PKC_TIMING}, 
    {"PKC_MESSAGE",    PKC_MESSAGE}, 
    {"PKC_OPAQUE",     PKC_OPAQUE},
    {NULL, 0}
};


static LIB330_TEXT_MAP MiniseedActionMap[] = {
    {"MSA_512: new 512 byte packet",                         MSA_512}, 
    {"MSA_ARC: new archival packet, non-incremental",        MSA_ARC}, 
    {"MSA_FIRST: new archival packet, incremental",          MSA_FIRST}, 
    {"MSA_INC: incremental update to archival packet",       MSA_INC}, 
    {"MSA_FINAL: final incremental update",                  MSA_FINAL}, 
    {"MSA_GETARC: request for last archival packet written", MSA_GETARC}, 
    {"MSA_RETARC: client is returning last packet written",  MSA_RETARC}, 
    {NULL, 0}
};

static LIB330_TEXT_MAP StateTypeMap[] = {
    {"ST_STATE: new operational state",               ST_STATE},
    {"ST_STATUS: new status available",               ST_STATUS},
    {"ST_CFG: new configuration available",           ST_CFG},
    {"ST_STALL: change in stalled comlink state",     ST_STALL},
    {"ST_PING: ping result",                          ST_PING},
    {"ST_TICK: info has seconds, subtype has usecs",  ST_TICK},
    {"ST_OPSTAT: new operational status minute",      ST_OPSTAT},
    {"ST_TUNNEL: * tunnel response available",        ST_TUNNEL},
    {NULL, 0}
};

static LIB330_TEXT_MAP LibErrMap[] = {
    {"LIBERR_NOERR: No error",                                             LIBERR_NOERR},
    {"LIBERR_PERM: No Permission",                                         LIBERR_PERM},
    {"LIBERR_TMSERV: Port in Use",                                         LIBERR_TMSERV},
    {"LIBERR_NOTR: You are not registered",                                LIBERR_NOTR},
    {"LIBERR_INVREG: Invalid Registration Request",                        LIBERR_INVREG},
    {"LIBERR_PAR: Parameter Error",                                        LIBERR_PAR},
    {"LIBERR_SNV: Structure not valid",                                    LIBERR_SNV},
    {"LIBERR_CTRL: Control Port Only",                                     LIBERR_CTRL},
    {"LIBERR_SPEC: Special Port Only",                                     LIBERR_SPEC},
    {"LIBERR_MEM: Memory operation already in progress",                   LIBERR_MEM},
    {"LIBERR_CIP: Calibration in Progress",                                LIBERR_CIP},
    {"LIBERR_DNA: Data not available",                                     LIBERR_DNA},
    {"LIBERR_DB9: Console Port Only",                                      LIBERR_DB9},
    {"LIBERR_MEMEW: Memory erase or Write Error",                          LIBERR_MEMEW},
    {"LIBERR_THREADERR: Could not create thread",                          LIBERR_THREADERR},
    {"LIBERR_BADDIR: Bad continuity directory",                            LIBERR_BADDIR},
    {"LIBERR_REGTO: Registration Timeout",                                 LIBERR_REGTO},
    {"LIBERR_STATTO: Status Timeout",                                      LIBERR_STATTO},
    {"LIBERR_DATATO: Data Timeout",                                        LIBERR_DATATO},
    {"LIBERR_NOSTAT: Your requested status is not yet available",          LIBERR_NOSTAT},
    {"LIBERR_INVSTAT: Your requested status in not a valid selection",     LIBERR_INVSTAT},
    {"LIBERR_CFGWAIT: Your requested configuration is not yet available",  LIBERR_CFGWAIT},
    {"LIBERR_INVCFG: Your can't set that configuration",                   LIBERR_INVCFG},
    {"LIBERR_TOKENS_CHANGE: Tokens Changed",                               LIBERR_TOKENS_CHANGE},
    {"LIBERR_INVAL_TOKENS: Invalid Tokens",                                LIBERR_INVAL_TOKENS},
    {"LIBERR_BUFSHUT: Shutdown due to reaching buffer percentage",         LIBERR_BUFSHUT},
    {"LIBERR_CONNSHUT: Shutdown due to reaching buffer percentage",        LIBERR_CONNSHUT},
    {"LIBERR_CLOSED: Closed by host",                                      LIBERR_CLOSED},
    {"LIBERR_NETFAIL: Networking Failure",                                 LIBERR_NETFAIL},
    {"LIBERR_TUNBUSY: Tunnelling in progress",                             LIBERR_TUNBUSY},
    {"LIBERR_INVCTX: Invalid Context",                                     LIBERR_INVCTX},
    {NULL, 0}
};

static LIB330_TEXT_MAP LibStateMap[] = {
    {"LIBSTATE_IDLE: Not connected to Q330",                                   LIBSTATE_IDLE},
    {"LIBSTATE_TERM: Terminated",                                              LIBSTATE_TERM},
    {"LIBSTATE_PING: Un-registered Ping, returns to LIBSTATE_IDLE when done",  LIBSTATE_PING},
    {"LIBSTATE_CONN: TCP Connect wait",                                        LIBSTATE_CONN},
    {"LIBSTATE_ANNC: Announce baler",                                          LIBSTATE_ANNC},
    {"LIBSTATE_REG: Requesting Registration",                                  LIBSTATE_REG},
    {"LIBSTATE_READCFG: Reading Configuration",                                LIBSTATE_READCFG},
    {"LIBSTATE_READTOK: Reading Tokens",                                       LIBSTATE_READTOK},
    {"LIBSTATE_DECTOK: Decoding Tokens and allocating structures",             LIBSTATE_DECTOK},
    {"LIBSTATE_RUNWAIT: Waiting for command to run",                           LIBSTATE_RUNWAIT},
    {"LIBSTATE_RUN: Running",                                                  LIBSTATE_RUN},
    {"LIBSTATE_DEALLOC: De-allocating structures",                             LIBSTATE_DEALLOC},
    {"LIBSTATE_DEREG: De-registering",                                         LIBSTATE_DEREG},
    {"LIBSTATE_WAIT: Waiting for a new registration",                          LIBSTATE_WAIT},
    {NULL, 0}
};


static char *LocateString(int code, LIB330_TEXT_MAP *map, char *DefaultMessage)
{
    int i;

    for (i = 0; map[i].text != NULL; i++) if (map[i].code == code) return map[i].text;
    return DefaultMessage;
}

char *lib330LibStateString(UINT8 code)
{
    static char *DefaultMessage = "<unknown>";

    return LocateString(code, LibStateMap, DefaultMessage);
}


char *lib330LibErrString(UINT8 code)
{
    static char *DefaultMessage = "<unknown>";

    return LocateString(code, LibErrMap, DefaultMessage);
}


char *lib330StateTypeString(UINT8 code)
{
    static char *DefaultMessage = "<unknown>";

    return LocateString(code, StateTypeMap, DefaultMessage);
}


char *lib330MiniseedActionString(UINT8 code)
{
    static char *DefaultMessage = "<unknown>";

    return LocateString(code, MiniseedActionMap, DefaultMessage);
}


char *lib330PacketClassString(UINT8 code)
{
    static char *DefaultMessage = "<unknown>";

    return LocateString(code, PacketClassMap, DefaultMessage);
}



