/*======================================================================
 *
 *  Logging facility
 *
 *====================================================================*/
#include "isi330.h"

static LOGIO lp;
static int level = LOG_INFO;

void LogCommandLine(int argc, char **argv)
{
int i;
char message[MAXPATHLEN+1];

    sprintf(message, "working directory: ");
    getcwd(message+strlen(message), MAXPATHLEN-strlen(message));
    LogMsg(message);

    sprintf(message, "command line:     ");
    for (i = 0; i < argc; i++) sprintf(message+strlen(message), " %s", argv[i]);
    LogMsg(message);
}

void LogMsg(char *format, ...)
{
va_list marker;
char *ptr, msgbuf[LOGIO_MAX_MSG_LEN];

    ptr = msgbuf;
    va_start(marker, format);
    vsnprintf(ptr, LOGIO_MAX_MSG_LEN, format, marker);
    va_end(marker);

    logioMsg(&lp, level, msgbuf);
}

void LogMsgLevel(int newlevel)
{
    level = newlevel;
    logioMsg(&lp, level, "log level %d\n", level);
}

LOGIO *InitLogging(char *myname, char *path, char *SITE)
{
static char *fid = "InitLogging";

    if (!logioInit(&lp, path, NULL, myname)) {
        fprintf(stderr, "%s: logioInit: %s\n", fid, strerror(errno));
        exit(1);
    }
    logioSetPrefix(&lp, SITE);
    logioMsg(&lp, level, "%s - %s", myname, VersionIdentString);
    logioMsg(&lp, level, "Build %s %s", __DATE__, __TIME__);

    return &lp;
}

void PrintISI330Config(ISI330_CONFIG *cfg) {
    printf("stacode: %s\n", cfg->stacode);
    printf("q330host: %s\n", cfg->q330host);
    printf("sn_str: %s\n", cfg->sn_str);
    printf("sn (64-bit): %llx\n", cfg->sn);
    printf("sn_hi (32-bit): %llx\n", cfg->sn >> 32);
    printf("sn_lo (32-bit): %llx\n", cfg->sn & 0x00000000FFFFFFFF);
    printf("dp: %hu\n", cfg->dp);
    printf("cfg->tpar_create:\n");
    printf("cfg->tpar_create.q330id_serial[0]:  %x\n", cfg->tpc->q330id_serial[0]);
    printf("cfg->tpar_create.q330id_serial[1]:  %x\n", cfg->tpc->q330id_serial[1]);
    printf("cfg->tpar_create.q330id_dataport:   %hu\n", cfg->tpc->q330id_dataport);
    printf("cfg->tpar_create.q330id_station:    %s\n", cfg->tpc->q330id_station);
    printf("cfg->tpar_create.host_timezone:     %d\n", cfg->tpc->host_timezone);
    printf("cfg->tpar_create.host_software:     %s\n", cfg->tpc->host_software);
    printf("cfg->tpar_create.opt_contfile:      %s\n", cfg->tpc->opt_contfile);
    printf("cfg->tpar_create.opt_verbose:       %d\n", cfg->tpc->opt_verbose);
    printf("cfg->tpar_create.opt_zoneadjust:    %d\n", cfg->tpc->opt_zoneadjust); /* calculate host's timezone automatically */
    printf("cfg->tpar_create.opt_secfilter:     %d\n", cfg->tpc->opt_secfilter); /* OSF_xxx bits */
    printf("cfg->tpar_create.opt_client_msgs:   %d\n", cfg->tpc->opt_client_msgs); /* Number of client message buffers */
    printf("cfg->tpar_create.opt_compat:        %d\n", cfg->tpc->opt_compat); /* Compatibility Mode */
    printf("cfg->tpar_create.opt_minifilter:    %d\n", cfg->tpc->opt_minifilter); /* OMF_xxx bits */
    printf("cfg->tpar_create.opt_aminifilter:   %d\n", cfg->tpc->opt_aminifilter); /* OMF_xxx bits */
    printf("cfg->tpar_create.amini_exponent:    %d\n", cfg->tpc->amini_exponent); /* 2**exp size of archival miniseed, range of 9 to 14 */
    printf("cfg->tpar_create.amini_512highest:  %d\n", cfg->tpc->amini_512highest); /* rates up to this value are updated every 512 bytes */
    printf("cfg->tpar_create.mini_embed:        %d\n", cfg->tpc->mini_embed); /* 1 = embed calibration and event blockettes into data */
    printf("cfg->tpar_create.mini_separate:     %d\n", cfg->tpc->mini_separate); /* 1 = generate separate calibration and event records */

    printf("cfg->tpar_register:\n");
    printf("cfg->tpar_register.q330id_auth:         %x\n", cfg->tpr.q330id_auth[0]); /* authentication code */
    printf("cfg->tpar_register.q330id_auth:         %x\n", cfg->tpr.q330id_auth[1]); /* authentication code */
    printf("cfg->tpar_register.q330id_address:      %s\n", cfg->tpr.q330id_address); /* domain name or IP address in dotted decimal */
    printf("cfg->tpar_register.q330id_baseport:     %d\n", cfg->tpr.q330id_baseport); /* base UDP port number */
    printf("cfg->tpar_register.host_mode:           %d\n", cfg->tpr.host_mode);
    printf("cfg->tpar_register.host_interface:      %s\n", cfg->tpr.host_interface); /* ethernet or serial port path name */
    printf("cfg->tpar_register.host_mincmdretry:    %d\n", cfg->tpr.host_mincmdretry); /* minimum command retry timeout */
    printf("cfg->tpar_register.host_maxcmdretry:    %d\n", cfg->tpr.host_maxcmdretry); /* maximum command retry timeout */
    printf("cfg->tpar_register.host_ctrlport:       %d\n", cfg->tpr.host_ctrlport); /* set non-zero to use specified UDP port at host end */
    printf("cfg->tpar_register.host_dataport:       %d\n", cfg->tpr.host_dataport); /* set non-zero to use specified UDP port at host end */
    /* printf("cfg->tpar_register.serial_flow:         %d\n", cfg->tpr.serial_flow); /1* 1 = hardware flow control *1/ */
    /* printf("cfg->tpar_register.serial_baud:         %ld\n", cfg->tpr.serial_baud); /1* in bps *1/ */
    /* printf("cfg->tpar_register.serial_hostip:       %ld\n", cfg->tpr.serial_hostip); /1* IP address to identify host *1/ */
    printf("cfg->tpar_register.opt_latencytarget:   %d\n", cfg->tpr.opt_latencytarget); /* seconds latency target for low-latency data */
    printf("cfg->tpar_register.opt_closedloop:      %d\n", cfg->tpr.opt_closedloop); /* 1 = enable closed loop acknowledge */
    printf("cfg->tpar_register.opt_dynamic_ip:      %d\n", cfg->tpr.opt_dynamic_ip); /* 1 = dynamic IP address */
    printf("cfg->tpar_register.opt_hibertime:       %d\n", cfg->tpr.opt_hibertime); /* hibernate time in minutes if non-zero */
    printf("cfg->tpar_register.opt_conntime:        %d\n", cfg->tpr.opt_conntime); /* maximum connection time in minutes if non-zero */
    printf("cfg->tpar_register.opt_connwait:        %d\n", cfg->tpr.opt_connwait); /* wait this many minutes after connection time or buflevel shutdown */
    printf("cfg->tpar_register.opt_regattempts:     %d\n", cfg->tpr.opt_regattempts); /* maximum registration attempts before hibernate if non-zero */
    printf("cfg->tpar_register.opt_ipexpire:        %d\n", cfg->tpr.opt_ipexpire); /* dyanmic IP address expires after this many minutes since last POC */
    printf("cfg->tpar_register.opt_buflevel:        %d\n", cfg->tpr.opt_buflevel); /* terminate connection when buffer level reaches this value if non-zero */
    printf("cfg->tpar_register.opt_q330_cont:       %d\n", cfg->tpr.opt_q330_cont); /* Determines how often Q330 continuity is written to disk in minutes */
    printf("cfg->tpar_register.opt_dss_memory:      %d\n", cfg->tpr.opt_dss_memory); /* Maximum DSS memory (in KB) if non-zero */
}

void PrintLib330Tliberr(enum tliberr liberr)
{
static char msgbuf[255];

    if (liberr == LIBERR_NOERR) { snprintf(msgbuf, sizeof(msgbuf), "LIBERR_NOERR: %s", "No error\n"); }
    else if (liberr == LIBERR_PERM) { snprintf(msgbuf, sizeof(msgbuf), "LIBERR_PERM: %s", "No Permission\n"); }
    else if (liberr == LIBERR_TMSERV) { snprintf(msgbuf, sizeof(msgbuf), "LIBERR_TMSERV: %s", "Port in Use\n"); }
    else if (liberr == LIBERR_NOTR) { snprintf(msgbuf, sizeof(msgbuf), "LIBERR_NOTR: %s", "You are not registered\n"); }
    else if (liberr == LIBERR_INVREG) { snprintf(msgbuf, sizeof(msgbuf), "LIBERR_INVREG: %s", "Invalid Registration Request\n"); }
    else if (liberr == LIBERR_PAR) { snprintf(msgbuf, sizeof(msgbuf), "LIBERR_PAR: %s", "Parameter Error\n"); }
    else if (liberr == LIBERR_SNV) { snprintf(msgbuf, sizeof(msgbuf), "LIBERR_SNV: %s", "Structure not valid\n"); }
    else if (liberr == LIBERR_CTRL) { snprintf(msgbuf, sizeof(msgbuf), "LIBERR_CTRL: %s", "Control Port Only\n"); }
    else if (liberr == LIBERR_SPEC) { snprintf(msgbuf, sizeof(msgbuf), "LIBERR_SPEC: %s", "Special Port Only\n"); }
    else if (liberr == LIBERR_MEM) { snprintf(msgbuf, sizeof(msgbuf), "LIBERR_MEM: %s", "Memory operation already in progress\n"); }
    else if (liberr == LIBERR_CIP) { snprintf(msgbuf, sizeof(msgbuf), "LIBERR_CIP: %s", "Calibration in Progress\n"); }
    else if (liberr == LIBERR_DNA) { snprintf(msgbuf, sizeof(msgbuf), "LIBERR_DNA: %s", "Data not available\n"); }
    else if (liberr == LIBERR_DB9) { snprintf(msgbuf, sizeof(msgbuf), "LIBERR_DB9: %s", "Console Port Only\n"); }
    else if (liberr == LIBERR_MEMEW) { snprintf(msgbuf, sizeof(msgbuf), "LIBERR_MEMEW: %s", "Memory erase or Write Error\n"); }
    else if (liberr == LIBERR_THREADERR) { snprintf(msgbuf, sizeof(msgbuf), "LIBERR_THREADERR: %s", "Could not create thread\n"); }
    else if (liberr == LIBERR_BADDIR) { snprintf(msgbuf, sizeof(msgbuf), "LIBERR_BADDIR: %s", "Bad continuity directory\n"); }
    else if (liberr == LIBERR_REGTO) { snprintf(msgbuf, sizeof(msgbuf), "LIBERR_REGTO: %s", "Registration Timeout\n"); }
    else if (liberr == LIBERR_STATTO) { snprintf(msgbuf, sizeof(msgbuf), "LIBERR_STATTO: %s", "Status Timeout\n"); }
    else if (liberr == LIBERR_DATATO) { snprintf(msgbuf, sizeof(msgbuf), "LIBERR_DATATO: %s", "Data Timeout\n"); }
    else if (liberr == LIBERR_NOSTAT) { snprintf(msgbuf, sizeof(msgbuf), "LIBERR_NOSTAT: %s", "Your requested status is not yet available\n"); }
    else if (liberr == LIBERR_INVSTAT) { snprintf(msgbuf, sizeof(msgbuf), "LIBERR_INVSTAT: %s", "Your requested status in not a valid selection\n"); }
    else if (liberr == LIBERR_CFGWAIT) { snprintf(msgbuf, sizeof(msgbuf), "LIBERR_CFGWAIT: %s", "Your requested configuration is not yet available\n"); }
    else if (liberr == LIBERR_INVCFG) { snprintf(msgbuf, sizeof(msgbuf), "LIBERR_INVCFG: %s", "Your can't set that configuration\n"); }
    else if (liberr == LIBERR_TOKENS_CHANGE) { snprintf(msgbuf, sizeof(msgbuf), "LIBERR_TOKENS_CHANGE: %s", "Tokens Changed\n"); }
    else if (liberr == LIBERR_INVAL_TOKENS) { snprintf(msgbuf, sizeof(msgbuf), "LIBERR_INVAL_TOKENS: %s", "Invalid Tokens\n"); }
    else if (liberr == LIBERR_BUFSHUT) { snprintf(msgbuf, sizeof(msgbuf), "LIBERR_BUFSHUT: %s", "Shutdown due to reaching buffer percentage\n"); }
    else if (liberr == LIBERR_CONNSHUT) { snprintf(msgbuf, sizeof(msgbuf), "LIBERR_CONNSHUT: %s", "Shutdown due to reaching buffer percentage\n"); }
    else if (liberr == LIBERR_CLOSED) { snprintf(msgbuf, sizeof(msgbuf), "LIBERR_CLOSED: %s", "Closed by host\n"); }
    else if (liberr == LIBERR_NETFAIL) { snprintf(msgbuf, sizeof(msgbuf), "LIBERR_NETFAIL: %s", "Networking Failure\n"); }
    else if (liberr == LIBERR_TUNBUSY) { snprintf(msgbuf, sizeof(msgbuf), "LIBERR_TUNBUSY: %s", "Tunnelling in progress\n"); }
    else if (liberr == LIBERR_INVCTX) { snprintf(msgbuf, sizeof(msgbuf), "LIBERR_INVCTX: %s","/* Invalid Context\n"); }
    else { snprintf(msgbuf, sizeof(msgbuf), "UNKNOWN tliberr\n"); }

    printf("%s", msgbuf);

}

void PrintLib330Topstat(topstat *popstat)
{
    if (popstat != NULL) {

        printf("opstat.station_name: %s\n", popstat->station_name); /* network and station */
        printf("opstat.station_port: %d\n", popstat->station_port); /* data port number */
        printf("opstat.station_tag: %u\n", popstat->station_tag); /* tagid */
        printf("opstat.station_serial[0]: %x\n", popstat->station_serial[0]); /* q330 serial number */
        printf("opstat.station_serial[1]: %x\n", popstat->station_serial[1]); /* q330 serial number */
        printf("opstat.station_reboot: %u\n", popstat->station_reboot); /* time of last reboot */
        printf("opstat.timezone_offset: %d\n", popstat->timezone_offset); /* seconds to adjust computer's clock */
        /* printf("opstat.accstats: %s\n", popstat->accstats); /1* accumulated statistics *1/ */
        printf("opstat.minutes_of_stats: %d\n", popstat->minutes_of_stats); /* how many minutes of data available to make hour */
        printf("opstat.hours_of_stats: %d\n", popstat->hours_of_stats); /* how many hours of data available to make day */
        printf("opstat.auxinp: %d\n", popstat->auxinp); /* bitmap of Aux. inputs */
        printf("opstat.data_latency: %d\n", popstat->data_latency); /* data latency (calculated based on host clock) in seconds or INVALID_LATENCY */
        printf("opstat.status_latency: %d\n", popstat->status_latency); /* seconds since received status from 330 or INVALID_LATENCY */
        printf("opstat.runtime: %d\n", popstat->runtime); /* running time in seconds since current connection (+) or time it has been down (-) */
        printf("opstat.totalgaps: %d\n", popstat->totalgaps); /* total number of data gaps since context created */
        /* printf("opstat.pkt_full: %s\n", popstat->pkt_full); /1* percent of Q330 packet buffer full *1/ */
        printf("opstat.clock_qual: %d\n", popstat->clock_qual); /* Percent clock quality */
        printf("opstat.clock_drift: %d\n", popstat->clock_drift); /* Clock drift from GPS in microseconds */
        printf("opstat.mass_pos: %d, %d, %d, %d, %d, %d\n",
                popstat->mass_pos[0], popstat->mass_pos[1], popstat->mass_pos[2],
                popstat->mass_pos[3], popstat->mass_pos[4], popstat->mass_pos[5]);
        printf("opstat.calibration_errors: %d\n", popstat->calibration_errors); /* calibration error bitmap */
        printf("opstat.sys_temp: %d\n", popstat->sys_temp); /* Q330 temperature in degrees C */
        printf("opstat.pwr_volt: %f\n", popstat->pwr_volt); /* Q330 power supply voltage in volts */
        printf("opstat.pwr_cur: %f\n", popstat->pwr_cur); /* Q330 power supply current in amps */
        printf("opstat.gps_age: %d\n", popstat->gps_age); /* age in seconds of last GPS clock update, -1 for never updated */
        printf("opstat.gps_stat: %d\n", popstat->gps_stat); /* GPS Status */
        printf("opstat.gps_fix: %d\n", popstat->gps_fix); /* GPS Fix */
        printf("opstat.pll_stat: %d\n", popstat->pll_stat); /* PLL Status */
        printf("opstat.gps_lat: %f\n", popstat->gps_lat); /* Latitude */
        printf("opstat.gps_long: %f\n", popstat->gps_long); /* Longitude */
        printf("opstat.gps_elev: %f\n", popstat->gps_elev); /* Elevation */
        /* printf("opstat.slidecopy: %s\n", popstat->slidecopy); /1* sliding window status *1/ */
        printf("opstat.last_data_timee %u\n", popstat->last_data_time); /* Latest data received, 0 for none */
        string15 ip_dot;
        showdot(popstat->current_ip, &ip_dot);
        printf("opstat.current_ip: %s\n", ip_dot); /* current IP Address of Q330 */
        printf("opstat.current_port: %u\n", popstat->current_port); /* current Q330 UDP Port */
    }
}
/*-----------------------------------------------------------------------+
 |                                                                       |
 | Copyright (C) 2017 Regents of the University of California            |
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
