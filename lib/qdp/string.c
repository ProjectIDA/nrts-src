#pragma ident "$Id: string.c,v 1.42 2017/09/14 00:27:18 dauerbach Exp $"
/*======================================================================
 *
 * Stirng conversions
 *
 *====================================================================*/
#include "qdp.h"
#include "qdp/fsa.h"

static QDP_TEXT_MAP TerseStateMap[] = {
    {"QDP_STATE_NEEDSN",       QDP_STATE_NEEDSN},
    {"QDP_STATE_WAITSN",       QDP_STATE_WAITSN},
    {"QDP_STATE_UNREG",        QDP_STATE_UNREG},
    {"QDP_STATE_RQSRV",        QDP_STATE_RQSRV},
    {"QDP_STATE_SRVRSP",       QDP_STATE_SRVRSP},
    {"QDP_STATE_WAIT_C1_FIX",  QDP_STATE_WAIT_C1_FIX},
    {"QDP_STATE_WAIT_C1_LOG",  QDP_STATE_WAIT_C1_LOG},
    {"QDP_STATE_WAIT_C2_EPD",  QDP_STATE_WAIT_C2_EPD},
    {"QDP_STATE_WAIT_C1_FLGS", QDP_STATE_WAIT_C1_FLGS},
    {"QDP_STATE_WAITTOKEN",    QDP_STATE_WAITTOKEN},
    {"QDP_STATE_READY",        QDP_STATE_READY},
    {"QDP_STATE_OFF",          QDP_STATE_OFF},
    {NULL, 0}
};

static QDP_TEXT_MAP StateMap[] = {
    {"Need Serial Number",                                 QDP_STATE_NEEDSN},
    {"Waiting For Serial Number",                          QDP_STATE_WAITSN},
    {"Unregistered",                                       QDP_STATE_UNREG},
    {"Server Request Sent",                                QDP_STATE_RQSRV},
    {"Challenge Response Sent",                            QDP_STATE_SRVRSP},
    {"Requesting Fixed Parameters After Reboot (C1_FIX)",  QDP_STATE_WAIT_C1_FIX},
    {"Requesting Data Port Configuration (C1_LOG)",        QDP_STATE_WAIT_C1_LOG},
    {"Requesting Environmental Processor Delays (C2_EPD)", QDP_STATE_WAIT_C2_EPD},
    {"Requesting Combo Packet (C1_FLGS)",                  QDP_STATE_WAIT_C1_FLGS},
    {"Requesting DP Tokens",                               QDP_STATE_WAITTOKEN},
    {"Registered",                                         QDP_STATE_READY},
    {"Off",                                                QDP_STATE_OFF},
    {NULL, 0}
};

static QDP_TEXT_MAP TerseEventMap[] = {
    {"QDP_EVENT_UP",         QDP_EVENT_UP},
    {"QDP_EVENT_MYSN",       QDP_EVENT_MYSN},
    {"QDP_EVENT_SRVCH",      QDP_EVENT_SRVCH},
    {"QDP_EVENT_NOTREG",     QDP_EVENT_NOTREG},
    {"QDP_EVENT_CERR",       QDP_EVENT_CERR},
    {"QDP_EVENT_CACK",       QDP_EVENT_CACK},
    {"QDP_EVENT_C1_FIX",     QDP_EVENT_C1_FIX},
    {"QDP_EVENT_C1_LOG",     QDP_EVENT_C1_LOG},
    {"QDP_EVENT_C2_EPD",     QDP_EVENT_C2_EPD},
    {"QDP_EVENT_C1_FLGS",    QDP_EVENT_C1_FLGS},
    {"QDP_EVENT_TOKEN_SOME", QDP_EVENT_TOKEN_SOME},
    {"QDP_EVENT_TOKEN_DONE", QDP_EVENT_TOKEN_DONE},
    {"QDP_EVENT_CTRL",       QDP_EVENT_CTRL},
    {"QDP_EVENT_CRCERR",     QDP_EVENT_CRCERR},
    {"QDP_EVENT_IOERR",      QDP_EVENT_IOERR},
    {"QDP_EVENT_WATCHDOG",   QDP_EVENT_WATCHDOG},
    {"QDP_EVENT_CTO",        QDP_EVENT_CTO},
    {"QDP_EVENT_DTO",        QDP_EVENT_DTO},
    {"QDP_EVENT_DATA",       QDP_EVENT_DATA},
    {"QDP_EVENT_FILL",       QDP_EVENT_FILL},
    {"QDP_EVENT_UKNDATA",    QDP_EVENT_UKNDATA},
    {"QDP_EVENT_HBEAT",      QDP_EVENT_HBEAT},
    {"QDP_EVENT_APPCMD",     QDP_EVENT_APPCMD},
    {"QDP_EVENT_OFF",        QDP_EVENT_OFF},
    {NULL, 0}
};

static QDP_TEXT_MAP EventMap[] = {
    {"Automaton Started",                 QDP_EVENT_UP},
    {"Serial Number Received",            QDP_EVENT_MYSN},
    {"Server Challenge Received",         QDP_EVENT_SRVCH},
    {"Not Registered",                    QDP_EVENT_NOTREG},
    {"C1_CERR Received",                  QDP_EVENT_CERR},
    {"C1_CACK Received",                  QDP_EVENT_CACK},
    {"C1_FIX Received",                   QDP_EVENT_C1_FIX},
    {"C1_LOG Received",                   QDP_EVENT_C1_LOG},
    {"C2_EPD Received",                   QDP_EVENT_C2_EPD},
    {"C1_FLGS Received",                  QDP_EVENT_C1_FLGS},
    {"Partial DP Token Data Received",    QDP_EVENT_TOKEN_SOME},
    {"Final DP Token Data Received",      QDP_EVENT_TOKEN_DONE},
    {"Control Port Packet Received",      QDP_EVENT_CTRL},
    {"CRC Error",                         QDP_EVENT_CRCERR},
    {"I/O Error",                         QDP_EVENT_IOERR},
    {"Watchdog Timeout",                  QDP_EVENT_WATCHDOG},
    {"Control Port Timeout",              QDP_EVENT_CTO},
    {"Data Port Timeout",                 QDP_EVENT_DTO},
    {"DT_DATA Received",                  QDP_EVENT_DATA},
    {"DT_FILL Received",                  QDP_EVENT_FILL},
    {"Unknown Data Port Packet Received", QDP_EVENT_UKNDATA},
    {"Heartbeat",                         QDP_EVENT_HBEAT},
    {"Command Received From App",         QDP_EVENT_APPCMD},
    {"Automaton Shutdown Started",        QDP_EVENT_OFF},
    {NULL, 0}
};

static QDP_TEXT_MAP ActionMap[] = {
    {"No Action (list complete)",           QDP_ACTION_done},
    {"Start Registration Watchdog Timer",   QDP_ACTION_START},
    {"Send Automaton Handshake Command",    QDP_ACTION_SEND},
    {"Poll For Serial Number",              QDP_ACTION_POLLSN},
    {"Load Serial Number",                  QDP_ACTION_LDSN},
    {"Issue Server Request",                QDP_ACTION_SRVRQ},
    {"Decode Server Challenge",             QDP_ACTION_DECODECH},
    {"Build Challenge Response",            QDP_ACTION_BLDRSP},
    {"Registered",                          QDP_ACTION_REGISTERED},
    {"Request C1_FIX",                      QDP_ACTION_RQ_C1_FIX},
    {"Load C1_FIX",                         QDP_ACTION_LD_C1_FIX},
    {"Request C1_LOG",                      QDP_ACTION_RQ_C1_LOG},
    {"Load C1_LOG",                         QDP_ACTION_LD_C1_LOG},
    {"Request C2_EPD",                      QDP_ACTION_RQ_C2_EPD},
    {"Load C2_EPD",                         QDP_ACTION_LD_C2_EPD},
    {"Request C1_FLGS",                     QDP_ACTION_RQ_C1_FLGS},
    {"Load C1_FLGS",                        QDP_ACTION_LD_C1_FLGS},
    {"Request DP Tokens",                   QDP_ACTION_RQ_TOKENS},
    {"Load DP Tokens",                      QDP_ACTION_LD_TOKENS},
    {"This Layer Up",                       QDP_ACTION_TLU},
    {"This Layer Down",                     QDP_ACTION_TLD},
    {"Registration Error",                  QDP_ACTION_REGERR},
    {"Process C1_CERR",                     QDP_ACTION_CERR},
    {"Process C1_ACK",                      QDP_ACTION_CACK},
    {"Process Control Port Packet",         QDP_ACTION_CTRL},
    {"Open Data Port",                      QDP_ACTION_OPEN},
    {"Handle Data Port Timeout",            QDP_ACTION_DTO},
    {"Generate Heartbeat (Request Status)", QDP_ACTION_HBEAT},
    {"Process User Command",                QDP_ACTION_APPCMD},
    {"Acknowledge Data Packets",            QDP_ACTION_DACK},
    {"Process DT_DATA Packet",              QDP_ACTION_DATA},
    {"Process DT_FILL Packet",              QDP_ACTION_FILL},
    {"Drop Spurious Packet",                QDP_ACTION_DROP},
    {"Flush Reorder Buffer",                QDP_ACTION_FLUSH},
    {"Note CRC Error",                      QDP_ACTION_CRCERR},
    {"Note I/O Error",                      QDP_ACTION_IOERR},
    {"Service Watchdog Timeout",            QDP_ACTION_WATCHDOG},
    {NULL, 0}
};

static QDP_TEXT_MAP CmdMap[] = {
    {"C1_CACK",    QDP_C1_CACK}, 
    {"C1_SRVCH",   QDP_C1_SRVCH},
    {"C1_CERR",    QDP_C1_CERR}, 
    {"C1_MYSN",    QDP_C1_MYSN},
    {"C1_PHY",     QDP_C1_PHY}, 
    {"C1_LOG",     QDP_C1_LOG},
    {"C1_GLOB",    QDP_C1_GLOB}, 
    {"C1_FIX",     QDP_C1_FIX},
    {"C1_MAN",     QDP_C1_MAN}, 
    {"C1_STAT",    QDP_C1_STAT},
    {"C1_RT",      QDP_C1_RT}, 
    {"C1_THN",     QDP_C1_THN},
    {"C1_GID",     QDP_C1_GID}, 
    {"C1_RCNP",    QDP_C1_RCNP},
    {"C1_SPP",     QDP_C1_SPP}, 
    {"C1_SC",      QDP_C1_SC},
    {"C1_FLGS",    QDP_C1_FLGS}, 
    {"C1_DCP",     QDP_C1_DCP},
    {"C1_DEV",     QDP_C1_DEV}, 
    {"C1_MEM",     QDP_C1_MEM},
    {"C1_MOD",     QDP_C1_MOD}, 
    {"C1_FREE",    QDP_C1_FREE},
    {"C2_PHY",     QDP_C2_PHY}, 
    {"C2_GPS",     QDP_C2_GPS},
    {"C2_WIN",     QDP_C2_WIN}, 
    {"C2_AMASS",   QDP_C2_AMASS},
    {"C2_POC",     QDP_C2_POC}, 
    {"C2_BACK",    QDP_C2_BACK},
    {"C2_VACK",    QDP_C2_VACK}, 
    {"C2_BCMD",    QDP_C2_BCMD},
    {"C2_REGRESP", QDP_C2_REGRESP}, 
    {"C2_QV",      QDP_C2_QV},
    {"C2_MD5",     QDP_C2_MD5}, 
    {"C2_SNAPT",   QDP_C2_SNAPT}, 
    {"C2_TERR",    QDP_C2_TERR},
    {"C2_RQEPD",   QDP_C2_RQEPD},
    {"C2_EPD",     QDP_C2_EPD},
    {"C2_RQEPCFG", QDP_C2_RQEPCFG},
    {"C2_EPCFG",   QDP_C2_EPCFG},
    {"C2_SEPCFG",  QDP_C2_SEPCFG},
    {"C1_RQSRV",   QDP_C1_RQSRV}, 
    {"C1_SRVRSP",  QDP_C1_SRVRSP},
    {"C1_DSRV",    QDP_C1_DSRV}, 
    {"C1_SAUTH",   QDP_C1_SAUTH},
    {"C1_POLLSN",  QDP_C1_POLLSN}, 
    {"C1_SPHY",    QDP_C1_SPHY},
    {"C1_RQPHY",   QDP_C1_RQPHY}, 
    {"C1_SLOG",    QDP_C1_SLOG},
    {"C1_RQLOG",   QDP_C1_RQLOG}, 
    {"C1_CTRL",    QDP_C1_CTRL},
    {"C1_SGLOB",   QDP_C1_SGLOB}, 
    {"C1_RQGLOB",  QDP_C1_RQGLOB},
    {"C1_RQFIX",   QDP_C1_RQFIX}, 
    {"C1_SMAN",    QDP_C1_SMAN},
    {"C1_RQMAN",   QDP_C1_RQMAN}, 
    {"C1_RQSTAT",  QDP_C1_RQSTAT},
    {"C1_WSTAT",   QDP_C1_WSTAT}, 
    {"C1_VCO",     QDP_C1_VCO},
    {"C1_PULSE",   QDP_C1_PULSE}, 
    {"C1_QCAL",    QDP_C1_QCAL},
    {"C1_STOP",    QDP_C1_STOP}, 
    {"C1_RQRT",    QDP_C1_RQRT},
    {"C1_MRT",     QDP_C1_MRT}, 
    {"C1_RQTHN",   QDP_C1_RQTHN},
    {"C1_RQGID",   QDP_C1_RQGID}, 
    {"C1_SCNP",    QDP_C1_SCNP},
    {"C1_SRTC",    QDP_C1_SRTC}, 
    {"C1_SOUT",    QDP_C1_SOUT},
    {"C1_SSPP",    QDP_C1_SSPP}, 
    {"C1_RQSPP",   QDP_C1_RQSPP},
    {"C1_SSC",     QDP_C1_SSC}, 
    {"C1_RQSC",    QDP_C1_RQSC},
    {"C1_UMSG",    QDP_C1_UMSG}, 
    {"C1_WEB",     QDP_C1_WEB},
    {"C1_RQFLGS",  QDP_C1_RQFLGS}, 
    {"C1_RQDCP",   QDP_C1_RQDCP},
    {"C1_RQDEV",   QDP_C1_RQDEV}, 
    {"C1_SDEV",    QDP_C1_SDEV},
    {"C1_PING",    QDP_C1_PING}, 
    {"C1_SMEM",    QDP_C1_SMEM},
    {"C1_RQMEM",   QDP_C1_RQMEM}, 
    {"C1_ERASE",   QDP_C1_ERASE},
    {"C1_RQMOD",   QDP_C1_RQMOD}, 
    {"C1_RQFREE",  QDP_C1_RQFREE},
    {"C2_SPHY",    QDP_C2_SPHY}, 
    {"C2_RQPHY",   QDP_C2_RQPHY},
    {"C2_SGPS",    QDP_C2_SGPS}, 
    {"C2_RQGPS",   QDP_C2_RQGPS},
    {"C2_SWIN",    QDP_C2_SWIN}, 
    {"C2_RQWIN",   QDP_C2_RQWIN},
    {"C2_SAMASS",  QDP_C2_SAMASS}, 
    {"C2_RQAMASS", QDP_C2_RQAMASS},
    {"C2_SBPWR",   QDP_C2_SBPWR}, 
    {"C2_BRDY",    QDP_C2_BRDY},
    {"C2_BOFF",    QDP_C2_BOFF}, 
    {"C2_BRESP",   QDP_C2_BRESP},
    {"C2_REGCHK",  QDP_C2_REGCHK}, 
    {"C2_INST",    QDP_C2_INST},
    {"C2_RQQV",    QDP_C2_RQQV}, 
    {"C2_RQMD5",   QDP_C2_RQMD5},
    {"C2_TERC",    QDP_C2_TERC}, 
    {"DT_DATA",    QDP_DT_DATA},
    {"DT_FILL",    QDP_DT_FILL}, 
    {"DT_USER",    QDP_DT_USER}, 
    {"DT_DACK",    QDP_DT_DACK},
    {"DT_OPEN",    QDP_DT_OPEN},
    {NULL, 0}
};

static QDP_TEXT_MAP DetectorMap[] = {
    {"Murdock Hutt", QDP_TOKEN_TYPE_MHD}, 
    {"threshold",    QDP_TOKEN_TYPE_TDS}, 
    {NULL, 0}
};

static QDP_TEXT_MAP TercmdMap[] = {
    {"C3_ANNC",    QDP_C3_ANNC}, 
    {"C3_RQANNC",  QDP_C3_RQANNC},
    {"C3_SANNC",   QDP_C3_SANNC},
    {NULL, 0}
};

static QDP_TEXT_MAP ErrMap[] = {
    {"no error",                      QDP_CERR_NOERR},
    {"permission denied",             QDP_CERR_PERM},
    {"too many servers",              QDP_CERR_TMSERV},
    {"not registered",                QDP_CERR_NOTR},
    {"invalid registration response", QDP_CERR_INVREG},
    {"parameter error",               QDP_CERR_PAR},
    {"invalid EEPROM structure",      QDP_CERR_SNV},
    {"not on config port",            QDP_CERR_CTRL},
    {"not on special functions port", QDP_CERR_SPEC},
    {"memory operation in progress",  QDP_CERR_MEM},
    {"calibration in progress",       QDP_CERR_CIP},
    {"data not (yet) available",      QDP_CERR_DNA},
    {"not on console",                QDP_CERR_DB9},
    {"flash write/erase error",       QDP_CERR_FLASH},
    {"command timeout",               QDP_CERR_TIMEOUT},
    {NULL, 0}
};

static QDP_TEXT_MAP CtrlMap[] = {
    {"save",          QDP_CTRL_SAVE},
    {"reboot",        QDP_CTRL_REBOOT},
    {"resync",        QDP_CTRL_RESYNC},
    {"GPS On",        QDP_CTRL_GPS_ON},
    {"GPS Off",       QDP_CTRL_GPS_OFF},
    {"GPS Coldstart", QDP_CTRL_GPS_COLDSTART},
    {NULL, 0}
};

static QDP_TEXT_MAP PortMap[] = {
    {"configuration port",     QDP_CFG_PORT},
    {"special functions port", QDP_SFN_PORT},
    {"data port 1",            QDP_LOGICAL_PORT_1},
    {"data port 2",            QDP_LOGICAL_PORT_2},
    {"data port 3",            QDP_LOGICAL_PORT_3},
    {"data port 4",            QDP_LOGICAL_PORT_4},
    {NULL, 0}
};

static QDP_TEXT_MAP DataPortMap[] = {
    {"1", QDP_LOGICAL_PORT_1},
    {"2", QDP_LOGICAL_PORT_2},
    {"3", QDP_LOGICAL_PORT_3},
    {"4", QDP_LOGICAL_PORT_4},
    {NULL, 0}
};

static QDP_TEXT_MAP ErrcodeMap[] = {
    {"no error",                   QDP_ERR_NO_ERROR},
    {"invalid argument",           QDP_ERR_INVALID},
    {"no such item",               QDP_ERR_NOSUCH},
    {"no memory",                  QDP_ERR_NOMEM},
    {"port already in use",        QDP_ERR_BUSY},
    {"UDP I/O error",              QDP_ERR_UDPIO},
    {"TTY I/O error",              QDP_ERR_TTYIO},
    {"data port not enabled",      QDP_ERR_DPORT_DISABLED},
    {"handshake error",            QDP_ERR_REGERR},
    {"registration timeout",       QDP_ERR_WATCHDOG},
    {"invalid auth code?",         QDP_ERR_BADAUTH},
    {"no response from digitizer", QDP_ERR_NORESPONSE},
    {NULL, 0}
};

static QDP_TEXT_MAP DebugMap[] = {
    {"terse",  QDP_TERSE},
    {"normal", QDP_INFO},
    {"debug",  QDP_DEBUG},
    {NULL, 0}
};

static QDP_TEXT_MAP MemTypeMap[] = {
    {"Flash Memory",                         QDP_MEM_TYPE_FLASH},
    {"Configuration Memory for Data Port 1", QDP_MEM_TYPE_CONFIG_DP1},
    {"Configuration Memory for Data Port 2", QDP_MEM_TYPE_CONFIG_DP2},
    {"Configuration Memory for Data Port 3", QDP_MEM_TYPE_CONFIG_DP3},
    {"Configuration Memory for Data Port 4", QDP_MEM_TYPE_CONFIG_DP4},
    {"Web Page Memory",                      QDP_MEM_TYPE_WEBPAGE},
    {"Slave Processor EEPROM",               QDP_MEM_TYPE_SLAVE_EEPROM},
    {"Slave Processor PIC Internal Memory",  QDP_MEM_TYPE_SLAVE_PIC},
    {"Clock Chip RAM",                       QDP_MEM_TYPE_CLOCK_RAM},
    {"Calibrator PIC Internal Memory",       QDP_MEM_TYPE_CALIB_PIC},
    {"QAPCHP EEPROM",                        QDP_MEM_TYPE_QAPCHP_EEPROM},
    {"Packet Buffer Memory",                 QDP_MEM_TYPE_PKTBUF_MEM},
    {"DSP Program Memory",                   QDP_MEM_TYPE_DSP_PROG_MEM},
    {"DSP Data Memory",                      QDP_MEM_TYPE_DSP_DATA_MEM},
    {"*** INVALID ***",                      QDP_MEM_TYPE_INVALID},
    {NULL, 0}
};

static QDP_TEXT_MAP BlocketteIdentMap[] = {
    {"QDP_DC_ST38  ",    QDP_DC_ST38},
    {"QDP_DC_ST816 ",   QDP_DC_ST816},
    {"QDP_DC_ST32  ",    QDP_DC_ST32},
    {"QDP_DC_ST232 ",   QDP_DC_ST232},
    {"QDP_DC_MN38  ",    QDP_DC_MN38},
    {"QDP_DC_MN816 ",   QDP_DC_MN816},
    {"QDP_DC_MN32  ",    QDP_DC_MN32},
    {"QDP_DC_MN232 ",   QDP_DC_MN232},
    {"QDP_DC_AG38  ",    QDP_DC_AG38},
    {"QDP_DC_AG816 ",   QDP_DC_AG816},
    {"QDP_DC_AG32  ",    QDP_DC_AG32},
    {"QDP_DC_AG232 ",   QDP_DC_AG232},
    {"QDP_DC_CNP38 ",   QDP_DC_CNP38},
    {"QDP_DC_CNP816",  QDP_DC_CNP816},
    {"QDP_DC_CNP316",  QDP_DC_CNP316},
    {"QDP_DC_CNP232",  QDP_DC_CNP232},
    {"QDP_DC_D32   ",     QDP_DC_D32},
    {"QDP_DC_COMP  ",    QDP_DC_COMP},
    {"QDP_DC_MULT  ",    QDP_DC_MULT},
    {"QDP_DC_SPEC  ",    QDP_DC_SPEC},
    {NULL, 0}
};

static QDP_TEXT_MAP PLLStateMap[] = {
    {"off",      QDP_PLL_OFF},
    {"hold",     QDP_PLL_HOLD},
    {"tracking", QDP_PLL_TRACK},
    {"locked",   QDP_PLL_LOCK},
    {NULL, 0}
};

static QDP_TEXT_MAP PhysicalPortMap[] = {
    {"Serial 1", QDP_PP_SERIAL_1},
    {"Serial 2", QDP_PP_SERIAL_2},
    {"Serial 3", QDP_PP_SERIAL_3},
    {"Ethernet", QDP_PP_ETHERNET},
    {NULL, 0}
};

static QDP_TEXT_MAP SensorControlMap[] = {
    {"Idle",                   QDP_SC_IDLE},
    {"Sensor A Calibrate",     QDP_SC_SENSOR_A_CALIB},
    {"Sensor A Centering",     QDP_SC_SENSOR_A_CENTER},
    {"Sensor A Capacitive",    QDP_SC_SENSOR_A_CAP},
    {"Sensor A Unlock",        QDP_SC_SENSOR_A_UNLOCK},
    {"Sensor A Lock",          QDP_SC_SENSOR_A_LOCK},
    {"Sensor A Deploy/Remove", QDP_SC_SENSOR_A_DEPREM},
    {"Sensor A Auxiliary 1",   QDP_SC_SENSOR_A_AUX1},
    {"Sensor A Auxiliary 2",   QDP_SC_SENSOR_A_AUX2},
    {"Sensor B Calibrate",     QDP_SC_SENSOR_B_CALIB},
    {"Sensor B Centering",     QDP_SC_SENSOR_B_CENTER},
    {"Sensor B Capacitive",    QDP_SC_SENSOR_B_CAP},
    {"Sensor B Unlock",        QDP_SC_SENSOR_B_UNLOCK},
    {"Sensor B Lock",          QDP_SC_SENSOR_B_LOCK},
    {"Sensor B Deploy/Remove", QDP_SC_SENSOR_B_DEPREM},
    {"Sensor B Auxiliary 1",   QDP_SC_SENSOR_B_AUX1},
    {"Sensor B Auxiliary 2",   QDP_SC_SENSOR_B_AUX2},
    {NULL, 0}
};

static QDP_TEXT_MAP SensorControlMap2[] = {
    {"     -  ", QDP_SC_IDLE},
    {"A calib ", QDP_SC_SENSOR_A_CALIB},
    {"A center", QDP_SC_SENSOR_A_CENTER},
    {"A  cap  ", QDP_SC_SENSOR_A_CAP},
    {"A unlock", QDP_SC_SENSOR_A_UNLOCK},
    {"A  lock ", QDP_SC_SENSOR_A_LOCK},
    {"A deprem", QDP_SC_SENSOR_A_DEPREM},
    {"A  aux1 ", QDP_SC_SENSOR_A_AUX1},
    {"A  aux2 ", QDP_SC_SENSOR_A_AUX2},
    {"B calib ", QDP_SC_SENSOR_B_CALIB},
    {"B center", QDP_SC_SENSOR_B_CENTER},
    {"B  cap  ", QDP_SC_SENSOR_B_CAP},
    {"B unlock", QDP_SC_SENSOR_B_UNLOCK},
    {"B  lock ", QDP_SC_SENSOR_B_LOCK},
    {"A deprem", QDP_SC_SENSOR_B_DEPREM},
    {"B  aux1 ", QDP_SC_SENSOR_B_AUX1},
    {"B  aux2 ", QDP_SC_SENSOR_B_AUX2},
    {NULL, 0}
};

static QDP_TEXT_MAP AuxtypeMap[] = {
    {"AuxAD", QDP_AUXAD_DEFAULT},
    {NULL, 0}
};

static QDP_TEXT_MAP ClocktypeMap[] = {
    {"No clock", QDP_CLOCK_NONE},
    {"Motorola M12", QDP_CLOCK_M12},
    {NULL, 0}
};

static QDP_TEXT_MAP CalibratorMap[] = {
    {"QCAL330", QDP_CALIBRATOR_QCAL330},
    {NULL, 0}
};

static QDP_TEXT_MAP WebPageMap[] = QDP_WEB_PAGE_MAP;

static QDP_TEXT_MAP LogicalOperatorMap[] = {
    { "leftpar", QDP_CDS_EQ_LEFTPAR},
    {"rightpar", QDP_CDS_EQ_RIGHTPAR},
    {     "not", QDP_CDS_EQ_NOT},
    {     "and", QDP_CDS_EQ_AND},
    {      "or", QDP_CDS_EQ_OR},
    {     "eor", QDP_CDS_EQ_EOR},
    {    "done", QDP_CDS_EQ_DONE},
    {NULL, 0xff}
};

static QDP_TEXT_MAP SSPortMap[] = {
    { "serial port 1", 0 },
    { "serial port 2", 1 },
    {NULL, 0}
};

static QDP_TEXT_MAP SSTypeMap[] = {
    {     "absolute", QDP_SS_TYPE_ABSOLUTE     },
    {        "guage", QDP_SS_TYPE_GUAGE        },
    { "differential", QDP_SS_TYPE_DIFFERENTIAL },
    {NULL, 0}
};

static QDP_TEXT_MAP TokenTypeMap[] = {
    {                             "ignore", QDP_TOKEN_TYPE_IGNORE},
    {               "Token Version Number", QDP_TOKEN_TYPE_VER},
    {             "Network and Station ID", QDP_TOKEN_TYPE_SITE},
    {                       "DP Netserver", QDP_TOKEN_TYPE_NETSERVER},
    {                       "DP Webserver", QDP_TOKEN_TYPE_WEBSERVER},
    {                    "DataServer Port", QDP_TOKEN_TYPE_DATSERVER},
    {"Data Subscription Server Parameters", QDP_TOKEN_TYPE_DSS},
    {        "Clock Processing Parameters", QDP_TOKEN_TYPE_CLOCK},
    {      "Log and Timing Identification", QDP_TOKEN_TYPE_LOGID},
    {       "Configuration Identification", QDP_TOKEN_TYPE_CNFID},
    {              "Logical Channel Queue", QDP_TOKEN_TYPE_LCQ},
    {           "IIR Filter Specification", QDP_TOKEN_TYPE_IIR},
    {           "FIR Filter Specification", QDP_TOKEN_TYPE_FIR},
    {     "Control Detector Specification", QDP_TOKEN_TYPE_CDS},
    {"Murdock Hutt Detector Specification", QDP_TOKEN_TYPE_MHD},
    {   "Threshold Detector Specification", QDP_TOKEN_TYPE_TDS},
    {                   "Non-compliant DP", QDP_TOKEN_TYPE_NONCOMP},
    {                   "Comm Event Names", QDP_TOKEN_TYPE_CEN},
    {          "Email Alert Configuration", QDP_TOKEN_TYPE_EMAIL},
    {               "Opaque Configuration", QDP_TOKEN_TYPE_OPAQUE},
    {NULL, 0}
};

static QDP_TEXT_MAP SSUnitsMap[] = {
    {  "user", QDP_SS_UNITS_USER},
    {   "psi", QDP_SS_UNITS_PSI},
    {  "mbar", QDP_SS_UNITS_MBAR},
    {   "bar", QDP_SS_UNITS_BAR},
    {   "kPa", QDP_SS_UNITS_KPA},
    {   "MPa", QDP_SS_UNITS_MPA},
    { "in Hg", QDP_SS_UNITS_INHG},
    { "mm Hg", QDP_SS_UNITS_MMHG},
    { "m H20", QDP_SS_UNITS_MH2O},
    {NULL, 0}
};

static QDP_TEXT_MAP InputMuxMap[] = {
    {       "1pps", QDP_INPUT_MUX_1PPS},
    {  "reference", QDP_INPUT_MUX_REFERENCE},
    { "calibrator", QDP_INPUT_MUX_CALIBRATOR},
    {   "grounded", QDP_INPUT_MUX_GROUNDED},
    {NULL, 0}
};

static QDP_TEXT_MAP InputMuxMap2[] = {
    { "pps", QDP_INPUT_MUX_1PPS},
    { "ref", QDP_INPUT_MUX_REFERENCE},
    { "cal", QDP_INPUT_MUX_CALIBRATOR},
    { "gnd", QDP_INPUT_MUX_GROUNDED},
    {NULL, 0}
};

static QDP_TEXT_MAP PreampMuxMap[] = {
    {"CHANOFF", QDP_PREAMP_CHANDISABLED},
    {    "off", QDP_PREAMP_OFF},
    {     "on", QDP_PREAMP_ON},
    {NULL, 0}
};

static QDP_TEXT_MAP SdiPhaseMap[] = {
    {   "ready", QDP_STAT_EP_SDI_PHASE_READY},
    {"sampling", QDP_STAT_EP_SDI_PHASE_SAMPLING},
    { "waiting", QDP_STAT_EP_SDI_PHASE_WAITING},
    {NULL, 0}
};

static QDP_TEXT_MAP SdiDriverMap[] = {
    {"WXT520", QDP_STAT_EP_SDI_DRIVER_WXT520},
    {NULL, 0}
};

static QDP_TEXT_MAP EPSubchanMap[] = {
    { "analog channel 1 @ 40 Hz", C2_EP_SUBCHAN_ANALOG1_40HZ},
    { "analog channel 1 @ 20 Hz", C2_EP_SUBCHAN_ANALOG1_20HZ},
    { "analog channel 1 @ 10 Hz", C2_EP_SUBCHAN_ANALOG1_10HZ},
    {  "analog channel 1 @ 1 Hz", C2_EP_SUBCHAN_ANALOG1_1HZ},
    { "analog channel 2 @ 40 Hz", C2_EP_SUBCHAN_ANALOG2_40HZ},
    { "analog channel 2 @ 20 Hz", C2_EP_SUBCHAN_ANALOG2_20HZ},
    { "analog channel 2 @ 10 Hz", C2_EP_SUBCHAN_ANALOG2_10HZ},
    {  "analog channel 2 @ 1 Hz", C2_EP_SUBCHAN_ANALOG2_1HZ},
    { "analog channel 3 @ 40 Hz", C2_EP_SUBCHAN_ANALOG3_40HZ},
    { "analog channel 3 @ 20 Hz", C2_EP_SUBCHAN_ANALOG3_20HZ},
    { "analog channel 3 @ 10 Hz", C2_EP_SUBCHAN_ANALOG3_10HZ},
    {  "analog channel 3 @ 1 Hz", C2_EP_SUBCHAN_ANALOG3_1HZ},
    {        "internal pressure", C2_EP_SUBCHAN_INTERNAL_PRESSURE},
    {     "internal temperature", C2_EP_SUBCHAN_INTERNAL_TEMP},
    {        "internal humidity", C2_EP_SUBCHAN_INTERNAL_HUMIDITY},
    {            "input voltage", C2_EP_SUBCHAN_INPUT_VOLTAGE},
    {               "PLL offset", C2_EP_SUBCHAN_PLL_OFFSET},
    {              "VCO control", C2_EP_SUBCHAN_VCO_CONTROL},
    {    "WXT520 wind direction", C2_EP_SUBCHAN_WXT520_WIND_DIR},
    {        "WXT520 wind speed", C2_EP_SUBCHAN_WXT520_WIND_SPEED},
    {          "WXT520 pressure", C2_EP_SUBCHAN_WXT520_PRESSURE},
    {       "WXT520 temperature", C2_EP_SUBCHAN_WXT520_TEMP},
    {          "WXT520 humidity", C2_EP_SUBCHAN_WXT520_HUMIDITY},
    {    "WXT520 rain intensity", C2_EP_SUBCHAN_WXT520_RAIN_INTENSITY},
    {    "WXT520 hail intensity", C2_EP_SUBCHAN_WXT520_HAIL_INTENSITY},
    {"WXT520 heater temperature", C2_EP_SUBCHAN_WXT520_HEATER_TEMP},
    {    "WXT520 heater voltage", C2_EP_SUBCHAN_WXT520_HEATER_VOLTAGE},
    {    "WXT520 supply voltage", C2_EP_SUBCHAN_WXT520_SUPPLY_VOLTAGE},
    { "WXT520 reference voltage", C2_EP_SUBCHAN_WXT520_REF_VOLTAGE},
    {NULL, 0}
};

static char *LocateString(int code, QDP_TEXT_MAP *map, char *DefaultMessage)
{
int i;

    for (i = 0; map[i].text != NULL; i++) if (map[i].code == code) return map[i].text;
    return DefaultMessage;
}

static int LocateCode(char *string, QDP_TEXT_MAP *map, int DefaultCode)
{
int i;

    for (i = 0; map[i].text != NULL; i++) if (strcasecmp(map[i].text, string) == 0) return map[i].code;
    return DefaultCode;
}

char *qdpTerseStateString(UINT8 code)
{
static char *DefaultMessage = "<unknown>";

    return LocateString(code, TerseStateMap, DefaultMessage);
}

char *qdpStateString(UINT8 code)
{
static char *DefaultMessage = "<unknown>";

    return LocateString(code, StateMap, DefaultMessage);
}

char *qdpTerseEventString(UINT8 code)
{
static char *DefaultMessage = "<unknown>";

    return LocateString(code, TerseEventMap, DefaultMessage);
}

char *qdpEventString(UINT8 code)
{
static char *DefaultMessage = "<unknown>";

    return LocateString(code, EventMap, DefaultMessage);
}

char *qdpActionString(UINT8 code)
{
static char *DefaultMessage = "<unknown>";

    return LocateString(code, ActionMap, DefaultMessage);
}

char *qdpCmdString(UINT8 code)
{
static char *DefaultMessage = "<unknown>";

    return LocateString(code, CmdMap, DefaultMessage);
}

char *qdpTercmdString(UINT8 code)
{
static char *DefaultMessage = "<unknown>";

    return LocateString(code, TercmdMap, DefaultMessage);
}

int qdpCmdCode(char *string)
{
static int DefaultCode = QDP_CX_XXXX;

    return LocateCode(string, CmdMap, DefaultCode);
}

int qdpTercmdCode(char *string)
{
static int DefaultCode = 0xff;

    return LocateCode(string, TercmdMap, DefaultCode);
}

char *qdpErrString(UINT16 code)
{
static char *DefaultMessage = "<unknown>";

    return LocateString(code, ErrMap, DefaultMessage);
}

char *qdpCtrlString(UINT8 code)
{
static char *DefaultMessage = "<unknown>";

    return LocateString(code, CtrlMap, DefaultMessage);
}

char *qdpPortString(int code)
{
static char *DefaultMessage = "<unknown>";

    return LocateString(code, PortMap, DefaultMessage);
}

char *qdpDataPortString(int code)
{
static char *DefaultMessage = "<unknown>";

    return LocateString(code, DataPortMap, DefaultMessage);
}

char *qdpDebugString(UINT8 code)
{
static char *DefaultMessage = "<unknown>";

    return LocateString(code, DebugMap, DefaultMessage);
}

char *qdpMemTypeString(UINT8 code)
{
static char *DefaultMessage = "<unknown>";

    return LocateString(code, MemTypeMap, DefaultMessage);
}

char *qdpBlocketteIdentString(UINT8 code)
{
static char *DefaultMessage = "<unknown>";

    return LocateString(code, BlocketteIdentMap, DefaultMessage);
}

char *qdpPLLStateString(UINT16 code)
{
static char *DefaultMessage = "<unknown>";

    return LocateString((UINT8) code, PLLStateMap, DefaultMessage);
}

char *qdpPhysicalPortString(UINT16 code)
{
static char *DefaultMessage = "<unknown>";

    return LocateString((UINT8) code, PhysicalPortMap, DefaultMessage);
}

char *qdpSensorControlString(UINT32 code)
{
static char *DefaultMessage = "<unknown>";

    return LocateString((UINT8) (code & QDP_SC_MASK), SensorControlMap, DefaultMessage);
}

char *qdpSensorControlString2(UINT32 code)
{
static char *DefaultMessage = "? ? ? ? ";

    return LocateString((UINT8) (code & QDP_SC_MASK), SensorControlMap2, DefaultMessage);
}

char *qdpAuxtypeString(UINT16 code)
{
static char *DefaultMessage = "<unknown>";

    return LocateString(code, AuxtypeMap, DefaultMessage);
}

char *qdpClocktypeString(UINT16 code)
{
static char *DefaultMessage = "<unknown>";

    return LocateString(code, ClocktypeMap, DefaultMessage);
}

char *qdpDetectorString(UINT16 code)
{
static char *DefaultMessage = "<unknown>";

    return LocateString(code, DetectorMap, DefaultMessage);
}

char *qdpCalibratorString(UINT16 code)
{
static char *DefaultMessage = "<unknown>";

    return LocateString(code, CalibratorMap, DefaultMessage);
}

char *qdpWebPageMacro(UINT8 code)
{
static char *DefaultMessage = NULL;

    return LocateString(code, WebPageMap, DefaultMessage);
}

char *qdpLogicalOperatorString(UINT8 code)
{
static char *DefaultMessage = NULL;

    return LocateString(code, LogicalOperatorMap, DefaultMessage);
}

int qdpLogicalOperatorCode(char *string)
{
static int DefaultCode = 0xff;

    return LocateCode(string, LogicalOperatorMap, DefaultCode);
}

char *qdpSSPortString(UINT16 code)
{
static char *DefaultMessage = "<unknown>";

    return LocateString(code, SSPortMap, DefaultMessage);
}

char *qdpSSTypeString(UINT16 code)
{
static char *DefaultMessage = "<unknown>";

    return LocateString(code, SSTypeMap, DefaultMessage);
}

char *qdpSSUnitsString(UINT16 code)
{
static char *DefaultMessage = "<unknown>";

    return LocateString(code, SSUnitsMap, DefaultMessage);
}

char *qdpTokenTypeString(UINT8 code)
{
static char *DefaultMessage = "<unknown>";

    return LocateString(code, TokenTypeMap, DefaultMessage);
}

char *qdpErrcodeString(int code)
{
static char *DefaultMessage = "<unknown>";

    return LocateString(code, ErrcodeMap, DefaultMessage);
}

char *qdpPreampMuxString(int code)
{
static char *DefaultMessage = "<unknown>";

    return LocateString(code, PreampMuxMap, DefaultMessage);
}

char *qdpInputMuxString(int code)
{
static char *DefaultMessage = "<unknown>";

    return LocateString(code, InputMuxMap, DefaultMessage);
}

char *qdpInputMuxString2(int code)
{
static char *DefaultMessage = "<unknown>";

    return LocateString(code, InputMuxMap2, DefaultMessage);
}

int qdpInputMuxCode(char *string)
{
static int DefaultCode = QDP_INPUT_MUX_UNDEFINED;

    return LocateCode(string, InputMuxMap2, DefaultCode);
}

char *qdpSdiPhaseString(int code)
{
static char *DefaultMessage = "<unknown>";

    return LocateString(code, SdiPhaseMap, DefaultMessage);
}

char *qdpSdiDriverString(int code)
{
static char *DefaultMessage = "<unknown>";

    return LocateString(code, SdiDriverMap, DefaultMessage);
}

char *qdpEPSubchanString(int code)
{
static char *DefaultMessage = "<unknown>";

    return LocateString(code, EPSubchanMap, DefaultMessage);
}

char *qdpInputBitmapString(UINT16 bitmap, char *buf)
{
int chan, shift;
static char mt_unsafe[] = "1=calibrator 2=calibrator 3=calibrator 4=calibrator 5=calibrator 6=calibrator + slop";

    if (buf == NULL) buf = mt_unsafe;
    buf[0] = 0;

    for (chan = 1, shift=0; chan <= 3; chan++, shift++) {
        sprintf(buf+strlen(buf), "%d=", chan);
        if (bitmap & (1 << shift)) {
            sprintf(buf+strlen(buf), "%-10s ", qdpInputMuxString(qdpInputMuxA(bitmap)));
        } else {
            sprintf(buf+strlen(buf), "%-10s ", "normal");
        }
    }
    for (chan = 4, shift=3; chan <= 6; chan++, shift++) {
        sprintf(buf+strlen(buf), "%d=", chan);
        if (bitmap & (1 << shift)) {
            sprintf(buf+strlen(buf), "%-10s ", qdpInputMuxString(qdpInputMuxB(bitmap)));
        } else {
            sprintf(buf+strlen(buf), "%-10s ", "normal");
        }
    }
    return buf;
}

char *qdpPreampBitmapString(UINT16 bitmap, char *buf)
{
int chan, shift, value, mask=0x3;
static char mt_unsafe[] = "1=disabled 2=disabled 3=disabled 4=disabled 5=disabled 6=disabled + slop";

    if (buf == NULL) buf = mt_unsafe;
    buf[0] = 0;

    for (chan = 1, shift=0; chan <=  6; chan++, shift += 2) {
        value = (bitmap & (mask << shift)) >> shift;
        sprintf(buf+strlen(buf), "%d=%s ", chan, qdpPreampMuxString(value));
    }

    return buf;
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
 * $Log: string.c,v $
 * Revision 1.42  2017/09/14 00:27:18  dauerbach
 * introduced qdpPreampMuxString(), qdpPreampBitmapString()
 *
 * Revision 1.41  2017/02/03 20:44:04  dechavez
 * introduced qdpTerseStateString()
 *
 * Revision 1.40  2016/08/19 16:18:36  dechavez
 * added QDP_CERR_TIMEOUT support to qdpErrString()
 *
 * Revision 1.39  2016/08/04 21:37:07  dechavez
 * dded qdpTerseEventString()
 *
 * Revision 1.38  2016/07/20 17:24:19  dechavez
 * added qdpStateString(), qdpEventString(), and qdpActionString()
 *
 * Revision 1.37  2016/06/23 19:52:39  dechavez
 * added QDP_ERR_NODPMEM to ErrcodeMap[]
 *
 * Revision 1.36  2016/06/16 15:37:28  dechavez
 * added QDP_ERR_REJECTED to ErrcodeMap[]
 *
 * Revision 1.35  2016/02/11 19:04:48  dechavez
 * replaced QDP_ERR_NOREPLY with QDP_ERR_NOREPLY_SRVRQ and QDP_ERR_NOREPLY_CHRSP in ErrcodeMap[]
 *
 * Revision 1.34  2016/02/05 16:46:54  dechavez
 * added QDP_ERR_NOREPLY to ErrcodeMap[]
 *
 * Revision 1.33  2016/01/29 23:32:11  dechavez
 * fixed typo in C2_EP_SUBCHAN_WXT520_HEATER_VOLTAGE string
 *
 * Revision 1.32  2016/01/28 22:02:56  dechavez
 * Changed all QDP_STAT_SDI_x contants to QDP_STAT_EP_SDI_x equivalents.
 * Changed all QDP_STAT_ADC_x contants to QDP_STAT_EP_ADC_x equivalents.
 *
 * Revision 1.31  2016/01/27 00:19:29  dechavez
 * added QDP_C2_RQEPD and QDP_C2_EPD to CmdMap[]
 *
 * Revision 1.30  2016/01/21 17:43:28  dechavez
 * added qdpEPSubchanString()
 *
 * Revision 1.29  2016/01/19 22:54:00  dechavez
 * qdpSdiPhaseString(), qdpSdiDriverString()
 *
 * Revision 1.28  2015/12/23 20:23:07  dechavez
 * QDP_ERR_TTYIO support
 *
 * Revision 1.27  2014/01/24 00:21:26  dechavez
 * added support for STS-5A control lines
 *
 * Revision 1.26  2012/01/17 18:44:47  dechavez
 * added qdpDataPortString()
 *
 * Revision 1.25  2011/02/14 16:50:36  dechavez
 * fixed toggled locked/unlocked message errors for Sensor A control
 *
 * Revision 1.24  2011/01/03 21:25:59  dechavez
 * added qdpInputBitmapString(), qdpInputMuxCode(), qdpInputMuxString2()
 *
 * Revision 1.23  2010/10/19 21:12:49  dechavez
 * added qdpSensorControlString2()
 *
 * Revision 1.22  2010/04/07 19:48:57  dechavez
 * added qdpInputMuxString()
 *
 * Revision 1.21  2010/04/02 18:19:51  dechavez
 * qdpErrcodeString() support for QDP_ERR_UDPIO
 *
 * Revision 1.20  2010/03/31 20:26:25  dechavez
 * added QDP_SHUTDOWN to CmdMap
 *
 * Revision 1.19  2010/03/22 21:34:25  dechavez
 * added qdpErrcodeString(), made all PortMap[] text lowercase
 *
 * Revision 1.18  2009/11/05 18:31:26  dechavez
 * added qdpTokenTypeString()
 *
 * Revision 1.17  2009/10/29 17:27:27  dechavez
 * use QDP_TEXT_MAP from qdp/codes.h instead of local TEXT_MAP type, also QDP_WEB_PAGE_MAP from codes.h
 *
 * Revision 1.16  2009/10/20 22:37:58  dechavez
 * qdpLogicalOperatorCode(), qdpSSPortString(), qdpSSTypeString(), qdpSSUnitsString()
 *
 * Revision 1.15  2009/10/02 18:37:53  dechavez
 * added qdpWebPageMacro(), qdpLogicalOperatorString()
 *
 * Revision 1.14  2009/09/28 17:38:06  dechavez
 * added qdpDetectorString(), qdpTercmdCode(), qdpCmdCode(), qdpTercmdString()
 *
 * Revision 1.13  2009/02/03 23:01:04  dechavez
 * dded qdpAuxtypeString(), qdpClocktypeString(), qdpCalibratorString()
 *
 * Revision 1.12  2009/01/24 00:11:08  dechavez
 * added qdpSensorControlString(), fixed C1_GLOB typo
 *
 * Revision 1.11  2007/12/21 18:54:26  dechavez
 * Fixed PhysicalPortMap entries
 *
 * Revision 1.10  2007/12/20 22:49:14  dechavez
 * added qdpPLLStateString(), qdpPhysicalPortString()
 *
 * Revision 1.9  2007/05/18 18:00:57  dechavez
 * initial production release
 *
 */
