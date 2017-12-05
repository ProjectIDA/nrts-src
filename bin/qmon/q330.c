#pragma ident "$Id: q330.c,v 1.19 2017/09/06 22:06:50 dechavez Exp $"
/*======================================================================
 *
 * Q330 display
 *
 *====================================================================*/
#include "qmon.h"

static BOOL PrintPercentResent = FALSE;
static BOOL PrintDecimalDegrees = TRUE;

void TogglePercentResentView()
{
    PrintPercentResent = !PrintPercentResent;
}

void ToggleDecimalDegreesView()
{
    PrintDecimalDegrees = !PrintDecimalDegrees;
}

static void PrintTagLine(WINDOW *win, int row, DIGITIZER *digitizer, QDP_TYPE_MONITOR *mon)
{
    wmove(win, 0, 0);
    if (digitizer->state == QMON_STATE_CONNECTED) wattron(win, A_BOLD);
    wprintw(win, "%-6s", digitizer->addr->connect.ident);
    wattroff(win, A_BOLD);
    
    switch (digitizer->state) {
      case QMON_STATE_CONNECTED:
      case QMON_STATE_UNCONNECTED: /* to leave the info on the screen during reboot */
      case QMON_STATE_SHUTDOWN:    /* to leave the info on the screen after exit */
      case QMON_STATE_REBOOT:
        if (digitizer->fresh) {
            mvwprintw(win, row,  7, "S/N     ::                 Seqno:          Sys Ver:        Slv Ver:      ");
            mvwprintw(win, row, 10, "%5lu",     mon->fix.proper_tag);
            mvwprintw(win, row, 17, "%016llX",  mon->fix.sys_num);
            mvwprintw(win, row, 40, "%9lu",     mon->stat.gbl.cur_sequence);
            wmove(win, row, 50);
            if (digitizer->flags & REBOOT_IN_PROGRESS) {
                wattron(win, A_STANDOUT);
                wprintw(win, "      Reboot in progress      ");
                wattroff(win, A_STANDOUT);
            } else {
                if (digitizer->flags & REBOOT_REQUIRED) {
                    wattron(win, A_STANDOUT);
                    wprintw(win, "Reboot Required");
                    wattroff(win, A_STANDOUT);
                } else {
                    wprintw(win, "Sys Ver: %2d.%03d", mon->fix.sys_ver.major, mon->fix.sys_ver.minor);
                }
                wmove(win, row, 66);
                if (digitizer->flags & UNSAVED_CHANGES) {
                    wattron(win, A_STANDOUT);
                    wprintw(win, "Unsaved Config");
                    wattroff(win, A_STANDOUT);
                } else {
                    wprintw(win, "Slv Ver: %2d.%02d", mon->fix.sp_ver.major, mon->fix.sp_ver.minor);
                }
            }
        }
        break;
      case QMON_STATE_BUSY:
        waddstr(win, " : registration port busy");
        break;
      case QMON_STATE_REGERR:
        waddstr(win, " : registration error");
        break;
      case QMON_STATE_BADSERIALNO:
        wprintw(win, " : no q330/auth file entry for Q330 S/N %lu\n", digitizer->addr->connect.tag);
        break;
      default:
        waddstr(win, " : unregistered");
        break;
    }
}

static void PrintTimestampLine(WINDOW *win, int row, QDP_TYPE_MONITOR *mon)
{
UINT32 uptime;
char tmpbuf[QMON_COLS + 1];

    ClearToEol(win, row, 0);
    mvwprintw(win, row, 0, "Uptime:                 Timestamp:                   Reboots:      CAL:");

    uptime = mon->stat.gbl.tstamp - mon->fix.last_reboot;
    mvwprintw(win, row,  8, "%s", utilLttostr(uptime, 2, tmpbuf));
    mvwprintw(win, row, 35, "%s", utilLttostr((UINT32) mon->stat.gbl.tstamp + QDP_EPOCH_TO_1970_EPOCH, 0, tmpbuf));
    mvwprintw(win, row, 61, "%5u", mon->fix.reboots);
    wattron(win, A_STANDOUT);
    switch (mon->stat.gbl.cal_stat) {
      case 0:
        wattroff(win, A_STANDOUT);
        mvwprintw(win, row, 72, "off");
        break;
      case 1:
        mvwprintw(win, row, 72, "enabled");
        break;
      case 2:
        mvwprintw(win, row, 72, "ON(n/e)");
        break;
      case 3:
        mvwprintw(win, row, 72, "ON");
        break;
      default:
        mvwprintw(win, row, 72, "0x%02x?", mon->stat.gbl.cal_stat);
    }
    wattroff(win, A_STANDOUT);
}

static void PrintGpsStatus(WINDOW *win, int row, QDP_TYPE_MONITOR *mon, UINT8 qual)
{
char tmpbuf[QMON_COLS + 1];

    ClearToEol(win, row, 0);
    mvwprintw(win, row, 0, "GPS:");

    if (mon->stat.gps.gpson) {
        mvwprintw(win, row,  5, "%s", mon->stat.gps.date);
        mvwprintw(win, row, 16, "%s", mon->stat.gps.time);
        mvwprintw(win, row, 25, "qual=%d%%", qual);
        mvwprintw(win, row, 35, "sv=%d/%d", (int) mon->stat.gps.sat_used, (int) mon->stat.gps.sat_view);
        mvwprintw(win, row, 44, "Fix=%-4s", mon->stat.gps.fix);
        mvwprintw(win, row, 53, " LastPPS=%s", utilLttostr(mon->stat.gps.last_good + QDP_EPOCH_TO_1970_EPOCH, 0, tmpbuf));
        row += 1;
        if (PrintDecimalDegrees) {
            mvwprintw(win, row,  5, "Lat= %8.4lf ", mon->stat.gps.lat.deg);
            mvwprintw(win, row, 20, "Lon= %9.4lf ", mon->stat.gps.lon.deg);
        } else {
            mvwprintw(win, row,  5, "Lat=%s", mon->stat.gps.lat.str);
            mvwprintw(win, row, 20, "Lon=%s", mon->stat.gps.lon.str);
        }
        mvwprintw(win, row, 36, "Elev=%s", mon->stat.gps.height);
        mvwprintw(win, row, 54, "mode=");
        switch (mon->gps.mode &0x0007) {
          case 0: mvwprintw(win, row, 59, "internal"); break;
          case 1: mvwprintw(win, row, 59, "external"); break;
          case 2: mvwprintw(win, row, 59, "SeaScan");  break;
          case 3: mvwprintw(win, row, 59, "Network");  break;
          case 4: mvwprintw(win, row, 59, "Tx/Rx/In"); break;
          default: mvwprintw(win, row, 59, "0x%04x?", mon->gps.mode & 0x0007);
        }
        mvwprintw(win, row, 68, " export=%s", (mon->gps.mode & 0x0008) ? "ON " : "off");
    } else {
        mvwprintw(win, row, 5, "OFF %d min", mon->stat.gps.gpstime);
    }
    row += 1;
    ClearToEol(win, row, 0);
    mvwprintw(win, row, 0, "VCO:%5d", mon->stat.gbl.cur_vco & 0x7ffff);
    mvwprintw(win, row, 9, "%s", (mon->stat.gbl.cur_vco & 0x8000) ? "*" : " ");
}

static void PrintDataPortStatus(WINDOW *win, int row, QDP_TYPE_MONITOR *mon)
{

    ClearToEol(win, row, 0);
    mvwprintw(win, row, 0, "            Sent  Re-Sent SeqErr  Backlog         Sent  Re-Sent SeqErr  Backlog");

    row += 1;
    ClearToEol(win, row, 0);
    mvwprintw(win, row, 0, "Port 1:                                    2:");
    mvwprintw(win, row,  7, "%9lu", mon->stat.dport[0].sent);
    if (PrintPercentResent) {
        mvwprintw(win, row, 17, "%7.1f%%", mon->stat.dport[0].percent);
    } else {
        mvwprintw(win, row, 17, "%8lu", mon->stat.dport[0].resends);
    }
    mvwprintw(win, row, 26, "%6lu", mon->stat.dport[0].seq);
    mvwprintw(win, row, 33, "%8lu", mon->stat.dport[0].pack_used);
    mvwprintw(win, row, 45, "%9lu", mon->stat.dport[1].sent);
    if (PrintPercentResent) {
        mvwprintw(win, row, 55, "%7.1f%%", mon->stat.dport[1].percent);
    } else {
        mvwprintw(win, row, 55, "%8lu", mon->stat.dport[1].resends);
    }
    mvwprintw(win, row, 64, "%6lu", mon->stat.dport[1].seq);
    mvwprintw(win, row, 71, "%8lu", mon->stat.dport[1].pack_used);

    row += 1;
    ClearToEol(win, row, 0);
    mvwprintw(win, row, 0, "     3:                                    4:");
    mvwprintw(win, row,  7, "%9lu", mon->stat.dport[2].sent);
    if (PrintPercentResent) {
        mvwprintw(win, row, 17, "%7.1f%%", mon->stat.dport[2].percent);
    } else {
        mvwprintw(win, row, 17, "%8lu", mon->stat.dport[2].resends);
    }
    mvwprintw(win, row, 26, "%6lu", mon->stat.dport[2].seq);
    mvwprintw(win, row, 33, "%8lu", mon->stat.dport[2].pack_used);
    mvwprintw(win, row, 45, "%9lu", mon->stat.dport[3].sent);
    if (PrintPercentResent) {
        mvwprintw(win, row, 55, "%7.1f%%", mon->stat.dport[3].percent);
    } else {
        mvwprintw(win, row, 55, "%8lu", mon->stat.dport[3].resends);
    }
    mvwprintw(win, row, 64, "%6lu", mon->stat.dport[3].seq);
    mvwprintw(win, row, 71, "%8lu", mon->stat.dport[3].pack_used);
}

static void PrintAnalogStatus(WINDOW *win, int row, QDP_TYPE_MONITOR *mon)
{
int i, col;

    ClearToEol(win, row, 0);
    mvwprintw(win, row, 0, "Analog Status In:      V  Supply:     V  MainCur:    ma  AntCur:   ma  Temp:   C");
    mvwprintw(win, row, 18, "%5.2f", (float) mon->stat.boom.supply * 150.0 / 1000.0);
    mvwprintw(win, row, 33, "%5.2f", (float) mon->stat.boom.amb_pos / 100.0);
    mvwprintw(win, row, 49, "%4hu", mon->stat.boom.main_cur);
    mvwprintw(win, row, 64, "%3hu", mon->stat.boom.ant_cur);
    mvwprintw(win, row, 76, "%3hu", mon->stat.boom.sys_temp);

    row += 1;
    ClearToEol(win, row, 0);
    mvwprintw(win, row, 0, "Boom Positions");
    for (i = 0, col = 16; i < QDP_NCHAN; i++, col += 5) {
        mvwprintw(win, row, col, "%4d", mon->stat.boom.pos[i]);
    }
}

static void PrintEpStatus(WINDOW *win, int row, QDP_STAT_EP *ep, int index)
{
int col;
REAL32 pressure, temp;
char string[QMON_COLS+1];

    if (ep->version != 0) {
        pressure = (REAL32) ep->pressure / 1000.0;
        temp = (REAL32) ep->temperature / 10.0;
        sprintf(string, "EP%d: %7.2f mbar %5.1fC %3d%%", index, pressure, temp, ep->humidity);
    } else {
        sprintf(string, "EP1: n/a                     ");
    }
    col = 51;
    mvwprintw(win, row, col, "%s", string);
}

static void PrintSensors(WINDOW *win, int row, Q330_ADDR *addr)
{
    ClearToEol(win, row, 0);
    mvwprintw(win, row,  0, "Sensor Type");
    mvwprintw(win, row, 12, "A: %s", addr->input.a.name);
    mvwprintw(win, row, 46, "B: %s", addr->input.b.name);
}

static void PrintSensorControl(WINDOW *win, int row, QDP_TYPE_MONITOR *mon, BOOL mismatch)
{
int i, col, line;
UINT32 value;
static char *idle = "  ";
static char   *v5 = "5v";
static char   *v0 = "0v";
char *active;
static int order[QDP_NSC] = { 1, 2, 5, 6, 3, 4, 7, 8 };

    ClearToEol(win, row, 0);
    if (mismatch) {
        wattron(win, A_STANDOUT);
    } else {
        wattroff(win, A_STANDOUT);
    }
    mvwprintw(win, row--, 0, "Sensor Ctrl");
    wattroff(win, A_STANDOUT);
    for (i = 0; i < QDP_NSC; i++) {
        if (i % 4 == 0) wmove(win, ++row, col=12);
        line = order[i]; value = mon->sc[line-1];
        if (value == QDP_SC_IDLE) {
            active = idle;
        } else {
            active = value & QDP_SC_ACTIVE_HIGH_BIT ? v5 : v0;
        }
        mvwprintw(win, row, col, "%d: %s %s", line, qdpSensorControlString2(value), active);
        col += 17;
    }
}

static void PrintOperationalLimits(WINDOW *win, int row, QDP_TYPE_MONITOR *mon)
{
    ClearToEol(win, row, 0);
    mvwprintw(win, row, 0, "Limits  MaxMainCur:    ma MinOffTime:    sec MinPsVoltage:     V MaxAntCur:   ma");

    mvwprintw(win, row, 19, "%4hu",  mon->spp.max_main_current);
    mvwprintw(win, row, 37, "%3hu",  mon->spp.min_off_time);
    mvwprintw(win, row, 58, "%5.2f", (float) mon->spp.min_ps_voltage * 150.0 / 1000.0);
    mvwprintw(win, row, 75, "%3hu",  mon->spp.max_antenna_current);

    row += 1;
    ClearToEol(win, row, 0);
    mvwprintw(win, row, 0, "        MinTemp:    C     MaxTemp: +  C      TempHyst:   C       VoltHyst:     V");
    mvwprintw(win, row, 16, "%4hd",  mon->spp.min_temp);
    mvwprintw(win, row, 36, "%2hu",  mon->spp.max_temp);
    mvwprintw(win, row, 54, "%3hu",  mon->spp.temp_hysteresis);
    mvwprintw(win, row, 74, "%5.2f", (float) mon->spp.volt_hysteresis * 150.0 / 1000.0);
}

static void PrintInfoSet1(WINDOW *win, DIGITIZER *digitizer, QDP_TYPE_MONITOR *mon)
{
UINT8 qual;
QDP_MN232_DATA mn232;
QDP_TOKEN_CLOCK dp_clock = QDP_DEFAULT_TOKEN_CLOCK;

    PrintTagLine(win, 0, digitizer, mon);
    switch (digitizer->state) {
      case QMON_STATE_CONNECTED:
      case QMON_STATE_SHUTDOWN:
        if (digitizer->fresh) {
            PrintTimestampLine(win, 1, mon);
            if (mon->stat.gps.gpson) {
                mn232.qual = mon->stat.gbl.clock_qual;
                mn232.loss = mon->stat.gbl.clock_loss;
                qual = qdpClockQuality(&dp_clock, &mn232);
            } else {
                qual = 0;
            }
            PrintGpsStatus(win, 3, mon, qual);
            PrintDataPortStatus(win, 6, mon);
        }
        break;
      default:
        wclrtobot(win);
    }
    wrefresh(win);
}

static void PrintInfoSet2(WINDOW *win, DIGITIZER *digitizer, QDP_TYPE_MONITOR *mon)
{
    if (digitizer->fresh) {
        PrintAnalogStatus(win, 0, mon);
        PrintEpStatus(win, 1, &mon->stat.ep[0], 1);
        PrintSensors(win, 3, digitizer->addr);
        PrintSensorControl(win, 4, mon, SensorControlMismatch(digitizer, mon->sc));
        PrintOperationalLimits(win, 7, mon);
    } else {
        wclrtobot(win);
    }
    wrefresh(win);
}

void UpdateQ330(QMON *qmon)
{
BOOL active;
char sp2char;
int ndigitizer = 0;
LNKLST_NODE *crnt;
DIGITIZER *digitizer;

/* always update the timestamp */

    UpdateHts(qmon);

/* and nothing more if there is a pop up in the way */

    if (ActivePopup()) return;

/* update monitor data */

    crnt = listFirstNode(qmon->avail);

    while (crnt != NULL) {
        digitizer = (DIGITIZER *) crnt->payload;
        MUTEX_LOCK(&digitizer->mutex);
            active = (digitizer->win[0] != NULL || digitizer->win[1] != NULL) ? TRUE : FALSE;
            if (digitizer->clrbot) {
                wclear(qmon->win.bot);
                digitizer->clrbot = FALSE;
            }
            if (digitizer->win[0] != NULL) PrintInfoSet1(digitizer->win[0], digitizer, &digitizer->mon);
            if (digitizer->win[1] != NULL) PrintInfoSet2(digitizer->win[1], digitizer, &digitizer->mon);
            digitizer->fresh = FALSE;
        MUTEX_UNLOCK(&digitizer->mutex);
        if (active) ++ndigitizer;
        crnt = listNextNode(crnt);
    }

/* separator depends on how many digitizers are showing */

    sp2char = ndigitizer == 1 ? QMON_WIN_SP2_BLANK : QMON_WIN_SP2_DASH;
    if (qmon->sp2char != sp2char) {
        qmon->sp2char = sp2char;
        DrawSeparator(qmon->win.sp2, qmon->sp2char, QMON_SP1_COLS);
    }

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
 * $Log: q330.c,v $
 * Revision 1.19  2017/09/06 22:06:50  dechavez
 * added ToggleDecimalDegreesView(), and use it to control how coordinates are displayed
 *
 * Revision 1.18  2016/02/03 17:54:46  dechavez
 * changes related to the introduction of QDP_SOFTVER structures for version numbers
 *
 * Revision 1.17  2016/01/29 00:20:35  dechavez
 * changed bar to mbar in PrintEpStatus()
 *
 * Revision 1.16  2016/01/28 22:13:06  dechavez
 * include EP1 environmental processor status (if available)
 *
 * Revision 1.15  2015/12/23 22:46:09  dechavez
 * print a helpful message if the serial number is missing, instead of just
 * sitting there saying "unregistered" for all eternity
 *
 * Revision 1.14  2013/01/24 22:21:11  dechavez
 * added TogglePercentResentView(), and use it to control how resent packets are displayed
 *
 * Revision 1.13  2011/07/27 23:43:46  dechavez
 * original contents moved over to qio.c, now used home for Q330 display code
 * moved over from display.c
 *
 */
