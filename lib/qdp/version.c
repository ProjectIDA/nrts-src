#pragma ident "$Id: version.c,v 1.119 2017/10/11 20:41:29 dechavez Exp $"
/*======================================================================
 *
 * library version management
 *
 *====================================================================*/
#include "qdp.h"

static VERSION version = {3, 17, 0};
char *libqdpBuildIdent = "libqdp release 3.17.0, compiled " __DATE__ " " __TIME__;

/* qdp library release notes

3.17.0 10/11/2017
       qdp.h: added qdpPrintUnsupportedQEP() prototype
       action.c: reworked ActionAPPCMD() to fail after repeated attempts instead of restarting machine,
                 also, don't restart machine after automaton generated commands fail (ActionCERR)
       config.c: print version info message when skipping C2_EPCFG updates for peers which don't support QEP
       io.c: qdpRecvPkt() changed to log qioRecv() return value after failure
       print.c: introduced qdpPrintUnsupportedQEP()

3.16.1 09/20/2017
       config.c: added check for QEP support before updating C2_EPCFG

3.16.0 09/12/2017
       qdp.h: updated prototypes
       qdp/codes.h: introduced QDP_PREAMP_x constants
       print.c: introduced qdpPrintPreampBitmap()
       string.c: introduced qdpPreampMuxString(), qdpPreampBitmapString()

3.15.0 09/06/2017
       qdp.h: added qdpGpsStringToDecDeg() prototype
       qdp/status.h: introduced QDP_GPS_COORD to track both GPS coordinate strings and values,
          and used same for lat and lon fields in QDP_STAT_GPS
       print.c: fixed typo in format string in qdpPrintLCQ()
       status.c: introduced qdpGpsStringToDecDeg(), include decimal degree coordinats in PrintGPS()
          output, compute decimal degrees from GPS degmin strings, save in QDP_GPS_COORD

3.14.4 4/13/2017
       action.c: added deregister when aborting handshake with a disabled data port

3.14.3 2/03/2017
       qdp.h: added qdpTerseStateString() prototype
       fsa.c: extended invalid action code message to include information about context, so I can figure out where it actually got hit
       string.c: introduced qdpTerseStateString()

3.14.2 2/01/2017
       action.c: fixed bug in ActionLD_C1_LOG() (inroduced in 3.13.0) that was
                 causing QEP filter delays to get lost

3.14.1 1/30/2017
       qdp.h: Defined QDP_META_OLDTAG_EOF/TOKEN/COMBO to restore the old constants used in
              libqdp 3.10.1 and before.  This to permit qpp to read meta-data files tagged
              with both the old and new constants.
       meta.c: support both QDP_META_TAG_x and QDP_META_OLDTAG_x tags in meta-data files

3.14.0 1/20/2017 (a day that will live in infamy)
       qdp/lcq.h: added QDP_HLP_RULE_FLAG_NOHLP_1SECHK
       hlp.c: added QDP_HLP_RULE_FLAG_NOHLP_1SECHK to BuildUncompressed()
       print.c: added crc to qdpPrint_CMNHDR(), mn232 to qdpPrintLCQ()

3.13.6 9/23/2016
       fsa.c: added missing QDP_ACTION_RQ_C1_LOG case to EvenHandler() dispatcher

3.13.5 9/07/2016
       action.c: check for QEP support in ActionLD_C1_FIX() instead of ActionLD_C1_LOG()
                 (other wise non-data connections could not query the device), restart
                 automaton (instead of failing) when an automaton command fails (such as
                 the automaton "heartbeat" status requests

3.13.4 8/26/2016
       qdp/fsa.h: Removed QDP_ACTION_HBEAT as the first action for registered DTO events

3.13.3 08/19/2016
       qdp.h: changed QDP_DEFAULT_CTRL_TIMEOUT to 1 second, added 'attempts' field to QDP_CMD,
              eliminated QDP_DEFAULT_MAXTRY
       qdp/codes.h: introduced QDP_CERR_TIMEOUT
       action.c: ActionTLD() no longer sends C1_DSRV deregistration (that's now done in qdpShutdown())
                 ActionAPPCMD() lots number of times command has been sent and restarts automaton
                 when that exceeds QDP_MAX_CMD_ATTEMPTS (currently 2, set in qdp.h)
       debug.c: introduced qdpDebug2()
       fsa.c: eliminated CloseSockets(), now TerminateSelf() closes all the sockets when last
              thread exits. TerminateSelf() is now the *only* place where these sockets can get closed.
              qdpShutdown() now calls qdpDeregister() (fixes some persistent "busy server" problems)
              AppcmdThread() initializes new QDP_CMD 'attempts' field to zero when new command received
       msgq.c: log packet buffer activity using qdpDebug2() instead of qdpDebug() (quieter)
       string.c: added QDP_CERR_TIMEOUT support to qdpErrString()

3.13.2 08/15/2016
       qdp.h: eliminated QDP_PAR_OPTION_QUICK_EXITS option
       action.c: added a few more debug messages
       fsa.c: replaced extended sleeps with 5msec polling loops, thus eliminating the need
           for QDP_PAR_OPTION_QUICK_EXITS option

3.13.1 08/04/2016 (after a lot more testing, now appears solid... knock, knock)
       qdp.h: added tld to QDP_FSA, removed QDP_PAR_OPTION_QUITBUSY, added option QDP_PAR_OPTION_QUICK_EXITS,
          got rid of unused EventThread and CtrlThread semaphores, added reboots counter to QDP
       qdp/codes.h: removed QDP_SHUTDOWN
       qdp/fsa.h: found/fixed many(!) problems with QDP_STATE_MACHINE contents
       action.c: various changes related to changes in fsa.h, removed some uneeded debug code, added reboot detector
       action.h: updated prototypes to correspond to changes in action.c Revision 1.32
       cmds.c: removed qdpShutdown() (moved to fsa.c)
       fsa.c: a whole lot more debugging done... seems ready now
       init.c: got rid of unused EventThread and CtrlThread semaphores, added reboots counter
       process.c: moved some lines around in qdpProcessPacket() in the QDP_C1_MEM branch... cosmetic only
       string.c: added qdpTerseEventString()

3.13.0 07/20/2016 (checkpoint following reworking of state machine... not fully tested)
       qdp.h: replaced maxtry with watchdog interval in QDP_PAR, added QDP_DEFAULT_WATCHDOG_INTERVAL,
           removed QDP_DEFAULT_TRASH_x constants, introduced QDP_FSA structure to hold stuff
           used by the automaton (not sure if I'm going to keep that, however), removed ntry from
           the QDP handle, renamed regerr to suberr and include a pointer to that to qdpConnect()
           the idea being that errcode explains the general reason and suberr has additional informatin
           the definition of which is determined by the value of errcode (eg, C1_CERR in the case of
           Q330 rejections and current machine state in case of other failures...).  Not sure I'm going
           to keep that either (probably should have a QDP_FSA_ERROR structure instead).
       qdp/errno.h: removed QDP_ERR_NOREPLY_SRVRQ, QDP_ERR_NOREPLY_CHRSP, QDP_ERR_REJECTED, QDP_ERR_NODPMEM
           and added QDP_ERR_DPORT_DISABLED, QDP_ERR_REGERR, QDP_ERR_WATCHDOG. QDP_ERR_BADAUTH, QDP_ERR_NORESPONSE
       qdp/fsa.h:   MAJOR rework of design, mostly to handle handshake failure modes not previously encountered,
           but to also (hopefully) make it easier for somebody other than the original developer to understand.
        action.h: updated prototypes following rework of state machine
        action.c: updated action handlers to match the rework of the state machine
        cmds.c: qdpShutdown() calls qdpDestroyHandle() (addresses previously unrecognized memory leak, if it works)
        connect.c: added suberr argument to qdpConnect()
        destroy.c: created, introducing qdpDestroyHandle()
        fsa.c: Reworked how all the various threads get started (introducing the "launch" feature from the new START
           action handler, which I'll probably back out now that I started thinking about how machine restarts should work).
           Added smarts to figure out if the machine failed due to bad auth code (QDP_ERR_BADAUTH) or lack of a
           responding peer (QDP_ERR_NORESPONSE).  That I'll keep.  Added retry logic to optionally deal with busy
           servers (which previously was being handled at the application level).
        init.c: support for new QDP_FSA field added to handle in qdp.h rev 1.89
        io.c: qdpRecvPkt() now includes strerror() in I/O error message
        msgq.c: added a comment about the use of qdpForwardFullPktMsg()
        par.c: removed qdpSetMaxtry(), added qdpSetWatchdogInterval() and related stuff stemming from replacing maxtry with watchdog interval
        reorder.c: ActionDATA:DuplicateData() sanity check fails with ActionTLD() instead of no longer used ActionFAIL()
        string.c: added qdpStateString(), qdpEventString(), and qdpActionString()

3.12.1 06/23/2016
       qdp.h: added qdp_C1_RQCOMBO() prototype
       qdp/errno.h: added QDP_ERR_NODPMEM
       pkts.c: introduced qdp_C1_RQCOMBO()
       string.c: added QDP_ERR_NODPMEM to ErrcodeMap[]

3.12.0 06/15/2016
       qdp.h: introduced QDP_DEBUG2 and QDP_DEBUG3 states and associated options QDP_PAR_OPTION_QIO_TERSE and QDP_PAR_OPTION_QIO_VERBOSE
       qdp/errno.h: added QDP_ERR_REJECTED
       qdp/fsa.h: added QDP_ACTION_GIVEUP and modifed state 4 CERR response to invoke same
       action.h: added ActionGIVEUP() prototype
       action.c: introduced ActionGIVEUP()
       fsa.c: added QDP_ACTION_GIVEUP branch to EventHandler()
       init.c: QDP_PAR_OPTION_QIO_x based debug messages added
       par.c: overloaded qdpSetDebug() to use new QDP_DEBUGx states to set QDP_PAR_OPTION_QIO_x options
       string.c: added QDP_ERR_REJECTED to ErrcodeMap[]

3.11.4 02/12/2016
       qdp.h: add c1_fix to QDP handle
       qdp/fsa.h: added DTO handlers for early states of data connections
       action.c: changed actionLDFIX() to store C1_FIX in QDP handle's new c1_fix field

3.11.3 02/11/2016
       qdp.h:       changed QDP nsrq to ntry and QDP_PAR maxresp to maxtry as now we are
                    tracking challege responses as well as server request attempts
       qdp/fsa.h:   introduced QDP_ACTION_DECODECH, reworked QDP_STATE_SRVRSP fsa entries to handle
                    no replies to challenge response (ie, bad auth code)
       qdp/errno.h: changed QDP_ERR_NOREPLY to QDP_ERR_NOREPLY_SRVRQ, introduced QDP_ERR_NOREPLY_CHRSP
       action.h: updated prototypes
       action.c: changed QDP nsrq to ntry and QDP_PAR maxresp to maxtry, added ActionDECODECH(),
                 added code to ActionBLDRSP to track attempts and fail when excessive
       fsa.c:    changed QDP nsrq to ntry and QDP_PAR maxresp to maxtry, added ActionDECODECH()
       par.c:    changed maxresp to maxtry, qdpSetMaxsrq() to qdpSetMaxtry()
       string.c: replaced QDP_ERR_NOREPLY with QDP_ERR_NOREPLY_SRVRQ and QDP_ERR_NOREPLY_CHRSP in ErrcodeMap[]

3.11.2 02/05/2016
       qdp/errno.h: introduced QDP_ERR_NOREPLY
       qdp.h: added maxsrq to QDP_PAR, defined QDP_DEFAULT_MAXSRQ (0), added nsrq to QDP
       action.c: ActionSRVRQ() tracks nsrq and brings down the automaton when it exceeds maxsrq
       fsa.c: qdpStartFSA() checks qp->errcode instead of qp->regerr for machine start-up status
       par.c: introduced qdpSetMaxsrq(), include 'maxsrq' and 'options' to qdpLogPar() and qdpPrintPar() output
       string.c: added QDP_ERR_NOREPLY to ErrcodeMap[]

3.11.1 02/03/2016
       qdp.h: removed QDP_META_STATE_COMPLETE
       process.c: modified GetLC() to only load when complete meta-data are available

3.11.0 02/03/2016
       qdp.h: introduced QDP_SOFTVER structure for managing software versions,
              added constants to determine level of environmental processor support
              removed un-used "combo" field from QDP handle and added a new 'flags'
              field with session options (which is currently just QDP_FLAGS_EP_SUPPORTED)
        qdp/fsa.h: Reworked automaton to include bringing in C1_FIX as part of the
              handshake and then added QDP_ACTION_DECIDE to both eliminate the need for
              separate C&C and data link automatons (hurrah!).
        action.h: updated prototypes
        action.c: changed ActionRQCNF() to ActionRQCMB(), ActionLDCNF() to ActionLDCMB()
              added ActionRQFIX(), ActionLDFIX(), and ActionDecide()
        decode.c: changes related to introduction of QDP_SOFTVER for versions
        fsa.c: changed ActionRQCNF() to ActionRQCMB(), ActionLDCNF() to ActionLDCMB()
               added QDP_ACTION_DECIDE, QDP_ACTION_RQFIX, QDP_ACTION_LDFIX support to EventHandler()
        init.c: only the one automaton to assign to qp->fsa
        pkts.c: check handle for EP support and fake replies from qdp_C2_RQEPD() and qdp_C2_RQEPCFG() if appropriate
        print.c: changes related to introduction of QDP_SOFTVER for versions

3.10.2 01/28/2016
       qdp.h:
         Changed all QDP_STAT_SDI_x contants to QDP_STAT_EP_SDI_x equivalents.
         Changed all QDP_STAT_ADC_x contants to QDP_STAT_EP_ADC_x equivalents.
         Defined (essentially empty) QDP_STAT_MET3 and added a met3 field to QDP_STAT_SS
         so that it more correctly reflects the design of the serial sensor status blockette.
         Changed the design of QDP_STAT_GSV, QDP_STAT_ARP, and QDP_STAT_THR to use fixed
         size arrays instead of pointers (no more malloc and free).
       qdp/status.h:
         Changed all QDP_STAT_SDI_x contants to QDP_STAT_EP_SDI_x equivalents.
         Changed all QDP_STAT_ADC_x contants to QDP_STAT_EP_ADC_x equivalents.
         Added QDP_STAT_MET3 met3 field to QDP_STATUS_SS.
       pkts.c:
         Evidently adding QDP_SRB_EP (bit 20) to the status bitmap doesn't work when
         in the presence of other items requested (result too big?).  Anyway, worked
         around this in qdpRequestMonitor() by making a single QDP_SRB_EP status
         request and merging all the results into a single QDP_TYPE_C1_STAT.
       status.c:
         Changed all QDP_STAT_SDI_x contants to QDP_STAT_EP_SDI_x equivalents.
         Changed all QDP_STAT_ADC_x contants to QDP_STAT_EP_ADC_x equivalents.
         Reworked DecodeARP(), DecodeGSV(), and DecodeTHR() to use static arrays instead of malloc,
         and massively simplified qdpClear_C1_STAT() and qdpInit_C1_STAT() now that pointers are gone.
         Added stubs for QDP_STAT_MET3 support.
       string.c:
         Changed all QDP_STAT_SDI_x contants to QDP_STAT_EP_SDI_x equivalents.
         Changed all QDP_STAT_ADC_x contants to QDP_STAT_EP_ADC_x equivalents.

3.10.1 01/27/2016 (bug fixes related to C2_EPD meta-data)
       qdp.h: changed QDP_META_TAG_x definitions to values more easily searched for (debug aid)
              changed QDP_META_STATE_COMPLETE to include QDP_META_STATE_HAVE_EPD
              added epd to QDP_META 'raw' field
       action.c: fixed up handling of C2_EPD packet in ActionLDEPD(), forward onto application for saving
       process.c: fixed up handling of C2_EPD packets in qdpProcessPacket()
       meta.c: fixed up handling of C2_EPD packets

3.10.0 01/26/2016 (first functional Environmental Processor Support, delays included)
       qdp/fsa.h: Introduced recovering C2_EPD as part of the data FSA handshake.
       action.h: added ActionRQEPD() and ActionLDEPD() prototypes
       action.c: added ActionRQEPD() and ActionLDEPD()
       fsa.c: added code to generate QDP_EVENT_EPD and invoke QDP_ACTION_RQEPD, QDP_ACTION_LDEPD
       meta.c: completed support for C2_EPD packets
       process.c: added pragma ident back to the top, don't know when it got lost
       string.c: added QDP_C2_RQEPD and QDP_C2_EPD to CmdMap[]

3.9.2  01/22/2016
       qdp.h: added QDP_TYPE_C2_EPD support, reworked QDP_TYPE_C2_EPCFG
       qdp/codes.h: added QDP_C2_EPD and QDP_C2_RQEPD
       qdp/lcq.h:  changed QDP_SP_ENVDAT channel type to QDP_EPCHN
       data.c: added support for compressed QDP_SP_ENVDAT data
       decode.c: added QDP_TYPE_C2_EPD, plus changes related to reworking of QDP_TYPE_C2_EPCFG structure layout
       encode.c: changes related to reworking of QDP_TYPE_C2_EPCFG structure layout
       meta.c: support for QDP_TYPE_C2_EPD added to QDP_META (untested)
       pkts.c: added qdp_C2_RQEPD()
       print.c: added qdpPrint_C2_EPD(), include tokens in qdpPrint_C2_EPCFG()
       process.c: added support for compressed QDP_SP_ENVDAT data, QDP_C2_EPD packets (latter untested)
       steim.c: added qdpDecompressENVDAT()

3.9.1  01/21/2016
       qdp.h: reworked QDP_EP_CHANNEL structure, updated prototypes
       qdp/codes.h: added QDP_C2_EPCFG, QDP_C2_RQEPCFG, QDP_C2_SEPCFG
       qdp/xml.h: updated prototypes
       config.c: added environmental processor support
       decode.c: added qdpExpandRawEPchannel() and qdpDecode_C2_EPCFG()
       encode.c: added qdpEncode_C2_EPCFG()
       pkts.c: added qdp_C2_RQEPCFG()
       print.c: added qdpPrint_C2_EPCFG()
       string.c: added qdpEPSubchanString()

3.9.0  01/19/2016
       qdp.h:      initial environmental processor support
       qdp/lcq.h:      "         "            "       "
       qdp/status.h:   "         "            "       "
       data.c: environmental processor support (1-Hz only)
       print.c: environmental processor support
       process.c: environmental processor support (1-Hz only)
       status.c: environmental processor support
       string.c: qdpSdiPhaseString(), qdpSdiDriverString()

3.8.5  01/14/2016
       print.c: qdpPrintTokenChannelMap() print src2 string in hex (motivated by EP sources)

3.8.4  12/23/2015
       qdp/errno.h: added QDP_ERR_TTYIO
       init.c: set errno to be either QDP_ERR_UDPIO or QDP_ERR_TTYIO depending on connection type
       string.c: QDP_ERR_TTYIO support

3.8.3  12/05/2015
       action.c    casts, format fixes and the like, all to calm OS X compiles
       clock.c       "
       config.c      "
       data.c        "
       decode.c      "
       ida10.c       "
       io.c          "
       md5.c         "
       meta.c        "
       par.c         "
       print.c       "
       pt.c          "
       stats.c       "
       status.c      "
       steim.c       "
       tokens.c      "
       version.c     "
       webpage.c     "

3.8.2  12/03/2015
       decode.c: Fixed byte order problem building dot decimal string in Decode_DP_IPR
       fsa.c:    Added various qdpDebug() statements
       misc.c:   Added additional input checks and immediately return if no output defined
       status.c: made InitCounter() static

3.8.1  08/24/2015
       qdp.h:  added QDP_CAL_x constants, qdpCalState() protype
       misc.c: introduced qdpCalState()

3.8.0  08/11/2014 MAJOR CHANGE TO AUTOMATON TO SUPPORT SERIAL DATA COMM
       I made these changes in fall of 2013, and resisted committing them
       for nearly year because I wasn't confident I hadn't broken anything
       with the UDP I/O, and because the serial I/O wasn't perfect (I was
       able to get great results with one digitizer but not another and
       had no idea why).  This was all done to support the ADDOSS project
       but I then got sidetracked with the transition to the Linux VM after
       Judy was laid off and still have not gotten around to working on that
       anymore.  In the meantime I have deployed a lot of code built using
       the library in its current state so I have decided to freeze things
       as they are and use this as my baseline for when I finally get around
       to revisiting ADDOSS.

3.7.0   01/23/2014
        msgq.c: changed debug message contents to make them a (little bit) more sensible
        string.c: added support for STS-5A control lines
        a whole bunch of stuff with serial io that i started in october and did not commit (sigh)

3.6.0   02/07/2013
        hlp.c: toss the 1-sec overlaps that follow reconnects (this addresses the
               short packet followed by overlap mess that comes with bad comm links)

3.5.0   02/05/2013
        qdp/status.h: added percent field to QDP_STAT_DPORT
        status.c: added QDP_STAT_DPORT percent support to DecodeDPORT() and PrintDPORT()

3.4.1   08/07/2012
        hlp.c: fixed qdpDestroyHLP() declaration (removed extraneous *)

3.4.0   06/24/2012 Bound C1_LOG and tokens together with QDP_TYPE_DPORT
        qdp.h: replaced QDP_TYPE_FULL_CONFIG log and token fields with dport,
            added misc.c prototypes
        qdp/tokens.h: renamed QDP_TOKEN_LCQ sint to dsint, added nsint
        cmds.c: added port value to qdpEncode_C1_LOG() in qdpFlushDataPort()
        config.c: introduce qdpRequestDportConfig() and qdpConfigureDataPort(), also
          major cleanup of qdpSetConfig() to use qdpConfigureDataPort()
        decode.c: qdpDecode_C1_LOG() leaves decoded port number 0-based
        encode.c: qdpEncode_C1_LOG() packs port number using supplied argument, ignoring C1_LOG field
        meta.c: use qdpMainDigitizerChanNum() to compute chan from src[0]
        misc.c: introduce qdpConvertSampleRate(), qdpMainDigitizerChanNum(), qdpUpdateDataPortChannelFreqs()
        print.c:  lcq sint renamed to dsint
        status.c: PrintDPORT() uses qdpDataPortString() for data port identifier
        tokens.c: UnpackTokenLcq() sets derived fields with qdpConvertSampleRate()
        webpage.c: config->dport[i].token instead of config->token[i]


3.3.0   01/17/2012
        qdp.h: added qdpDataPortString() prototype
        qdp/status.h: added clock_perc to QDP_STAT_GBL
        status.c: added prefixes to make all "Input Voltage" strings unique,
           compute and print clock quality percentage instead of bitmap
        string.c: added qdpDataPortString()

3.2.1   02/14/2011
        string.c: fixed toggled locked/unlocked message errors for Sensor A control

3.2.0   02/07/2011 fixed state machine restart
        fsa.h: added QDP_EVENT_DOWN, invoke QDP_ACTION_TLD in all cases
        qdp.h: removed unused qdpClose() prototype
        fsa.c: qdpStartFSA() brings down machine if registration fails,
           while last instance of TerminateSelf() closes all sockets
        io.c: removed qdpClose()

3.1.0   02/01/2011
        config.c: qdpSetConfig() to accept caller imposed override of ethernet IP
        fsa.c: fixed bug preventing delivery of C1_MYSN and C1_PHY after registration

3.0.0   01/31/2011 Q330 console support
        qdp.h: defined QDP_BROADCAST_IP and QDP_BROADCAST_PORT
        fsa.h: added support for determining serial IP address
        action.c: added ActionRQPHY() and ActionLDPHY() event handlers
        fsa: added support for determining console port IP address
        init.c: set destination IP to broadcast when not using network I/O
        io.c: when ip is broadcast address, override qdpSendPkt() destination port with broadcast port

2.4.4   01/25/2011 (checkpoint on the way to adding Q330 console support)
        qdp.h: eliminated QDPIO in favor of QIO (from new libqio), introduced QDP_CONNECT
        init.c: replaced InitIO with InitConnections(), and use QDP_CONNECT "connect"
        io.c: switched to libqio calls instead of local qdpioX
        par.c: added qdpInitConnect(), qdpSetConnect(), removed qdpSetIdent()
        qdpio.c: removed (functionality move to libqio)

2.4.3   01/13/2011 (checkpoint on the way to adding Q330 console support)
        qdp.h: changed name of QDP_PAR "host" field to more generic "input"
               added macros for checking serial number and inferring console
        action.c: update peer ident with serial number in ActionSAVESN()
        connect.c: removed old qdpConnect() and gave name to qdpConnectWithPar()
        init.c: added InitIO to handle the details of I/O set up and to hide
               knowledge about console connection or not
        par.c: par "host" to "ident", qdpSetHost() to qdpSetIdent()
        tokens.c: changed name of qdpSetIdent() to qdpSetTokenIdent()

2.4.2   01/13/2011 (checkpoint on the way to adding Q330 console support)
        qdp.h: added QDP_TYPE_C1_MYSN
        fsa.h: added support for polling digitizer for serial number
        action.c: added ActionPOLLSN() and ActionSAVESN()
        decode.c: added qdpDecode_C1_MYSN() (untested)
        fsa.c: added QDP_EVENT_MYSN, QDP_ACTION_POLLSN, and QDP_ACTION_SAVESN support
        init.c: initial state for UDP connections changed to QDP_STATE_UNREG

2.4.1   01/12/2011 (checkpoint on the way to adding Q330 console support)
        qdp.h: added qdpEncode_C1_POLLSN prototype
        encode.c: added qdpEncode_C1_POLLSN()
        qdpio.c: changes associated with evolving slpio routines... not yet tested

2.4.0   01/11/2011 (checkpoint on the way to adding Q330 console support)
        I/O data structures generalized to support both UDP and SLIP.  BUT, still
        no way to actually specify a SLIP connection.  This is a checkpoint only
        commit to preserve UDP I/O under the new data structures.
        qdp.h: introduced QDPIO
        init.c: all libudpio structures and library calls changed to qdpio
          equivalents, but no support for anything other than UDP yet provided.
        io.c: all libudpio structures and library calls changed to qdpio equivalents
        qdpio.c: created

2.3.6   01/07/2011
        qdp.h: added QDP_DEFAULT_C1_SPP

2.3.5   01/03/2011
        qdp.h: added QDP_TYPE_C1_GLOB glob to QDP_TYPE_MONITOR
        pkts.c: added qdp_C1_RQGLOB to qdpRequestMonitor()
        print.c: changed qdpPrintInputBitmap() to use qdpInputBitmapString()
        string.c: added qdpInputBitmapString(), qdpInputMuxCode(), qdpInputMuxString2()

2.3.4   12/23/2010
        action.c: fixed HORRIBLE off by one bug in ActionRQCNF call to qdpEncode_C1_RQFLGS() port parameter
        cmds.c: use only symbolic values for port numbers in qdpFlushDataPort()
        config.c: replaced QDP_NUM_LOGICAL_PORTS with QDP_NLP, replaced explicit
          numerical indices in config->log and config->token with symbolic values
        par.c: added qdpValidPort()

2.3.3   12/22/2010
        qdp.h: cleaned up port numbering some more, defined QDP_NUM_LOGICAL_PORTS
        config.c: use explicit symbolic port numbers when requesting data port config
        par.c: added qdpDataPortNumber()

2.3.2   12/13/2010
        qdp.h: renumbered the port number defines so they could be used as array indicies
        qdp/code.h: changed QDP_SC_SENSOR_UNDEFINED to more appropriate QDP_SC_UNDEFINED

2.3.1   12/13/2010
        qdp.h: removed scMisMatch from QDP_TYPE_MONITOR
        qdp/codes.h: added QDP_SC_SENSOR_UNDEFINED

2.3.0   11/15/2010
        cmds.c: added qdpFlushDataPort()

2.2.3   11/02/2010
        status.c: fixed unpack error in DecodeUMSG() that was causing qdpDecode_C1_STAT()
          to seg fault when present

2.2.2   10/20/2010
        qdp.h: added QDP_DEFAULT_TOKEN_CLOCK
        par.c: added qdpAddOption(), removed unused arg arg from qdpSetOptionFlags()
        pkts.c: added QDP_SRB_SS to status request bitmap in qdpRequestMonitor()

2.2.1   10/19/2010
        string.c: added qdpSensorControlString2()

2.2.0   04/07/2010
        qdp/codes.h: added QDP_INPUT_MUX_x stuff
        print.c: added qdpPrintInputBitmap()
        strings.c: added qdpInputMuxString()

2.1.0   04/02/2010
        qdp/errno.h: added QDP_ERR_UDPIO
        init.c: set return ports to port.ctrl.value, port.data.value, set errcode to
            QDP_ERR_UDPIO in the event of udpioInit() failures
        par.c: restored qdpSetMyCtrlPort() and qdpSetMyDataPort().  Changed default
            vaues for port.ctrl.value and port.data.value to 0 to still permit OS
            assigned return ports.
        string.c: qdpErrcodeString() support for QDP_ERR_UDPIO

2.0.0   03/31/2010
        qdp.h: added QDP_SHUTDOWN, removed trash link parameters
        cmds.c: added qdpShutdown()
        connect.c: moved qdpLogPar to before qdpInit
        data.c: added conditional (ENABLE_DEBUG_BLOCKETTES) blockkette dumps
        fsa.c: changed qdpState() to qdpFsaState(), added QDP_SHUTDOWN support, removed link trasher
        hlp.c: added qdpFlushLCQ()
        init.c: get OS assigned return port numbers in udioInit() (finally!)
        io.c: removed trash link options, added call to qdpShutdown() in qdpClose()
        msgq.c: changed qdpState() call to qdpFsaState()
        par.c: removed trash link parameters and functions, removed unused
            qdpParseArgString(), qdpSetMyCtrlPort() and qdpSetMyDataPort() functions,
            changed qdpLogPar() and qdpPrintPar() to take QDP_PAR as the argument
        string.c: added QDP_SHUTDOWN to CmdMap

1.11.0  03/22/2010
        qdp.h: added options flag to QDP_PAR (default QDP_PAR_OPTION_QUITBUSY)
        action.c: added ActionBUSY
        connect.c: added errcode arg to qdpConnect(), qdpConnectWithPar()
        data.c: added qdpLenBT_COMP(), qdpLenBT_SPEC(), qdpHaveCalNotifyBlockette()
        fsa.c: added QDP_ACTION_BUSY branch, terminate registration if busy and QDP_PAR_OPTION_QUITBUSY set
        io.c: added qdpClose()
        par.c: added qdpSetOptionFlags(), defined options flag in qdpInitPar()
        string.c: added qdpErrcodeString(), made all PortMap[] text lowercase

1.10.0  03/11/2010
       hlp.c: added LCQ src bytes to QDP_HLP
       ida10.c: added LCQ src bytes to IDA10.4 header

1.9.4   11/14/2009
        qdp.h: added C1_CERR err cmd sub-field to QDP handle
        action.c: use C1_CERR err field in handle to communicate errors back to application (eg, memory busy)
        cmd.c: added qdpMemCmd() (for memory busy retries)
        config.c: changed commentary, added new qdpPostMemBlk() args (for memory busy retries)
        mem.c: fixed initialization bug in qdpSaveMem() introduced in 1.9.3, cleaned up and fixed qdpPostMemBlk()
        pkts.c: use qdpMemCmd() instead of qdpPostCmd() in qdp_C1_RQMEM (for memory busy retries)
        process.c: backed out resid=2 change in ProcessMN38 (release 1.9.2)

1.9.3   11/05/2009 (checkpoint)
        qdp/tokens.h: added names for token types 193 and 194 (but no code)
        qdp.h: added new prototypes
        config.c: added qdpSetConfig() (webpage not implemented yet... gets error response from Q330)
        init.c: new type argument for qdpInitMemBlk()
        mem.c: implemented qdpPostMemBlk()
        pkts.c: new type argument for qdpInitMemBlk()
        print.c: removed clock processing parameters from qdpPrintTokenReport()
        process.c: new type argument for qdpInitMemBlk()
        pt.c: implemented qdpPackTokenMemblk()
        string.c: added qdpTokenTypeString()
        webpage.c: in qdpPackWebpageMemblk() set missing type, allow for NULL set names

1.9.2   10/29/2009
        config.c: removed qdpRequestWebpage() (moved to webpage.c)
        data.c: decode C1_FIX, C1_GLOB, and config SSCFG in BT_SPEC blockettes
        encode.c: added qdpEncode_C1_LOG(), qdpEncode_C2_AMASS(), qdpEncode_C2_PHY(), qdpEncode_C3_ANNC()
        init.c: initialize QDP dbgpkt
        pkts.c: use dbgpkt if available in qdp_C1_RQGLOB(), qdp_C1_RQSC(), qdp_C1_RQPHY(),
            qdp_C1_RQLOG(), qdp_C1_RQSPP(), qdp_C2_RQAMASS(), qdp_C2_RQPHY, qdp_C3_RQANNC()
        print.c: print decoded configuration blockettes in qdpPrintBT_SPEC(),
           cleaned up detect formatting in qdpPrintTokenLcq
        process.c: instead of ignoring resid=2 for in ProcessMN38, assume 0.1 sec sint,
           use flags instead of obsolete reqtoken rule in InitLC
        pt.c: initial release
        string.c: use QDP_TEXT_MAP from qdp/codes.h instead of local TEXT_MAP type, also QDP_WEB_PAGE_MAP from codes.h
        tokens.c: added QDP_TOKEN_TYPE_NONCOMP support, use QDP_DSS_PASSWD_LEN
          instead of hardcoded number in decoding dss password fields
        webpage.c: initial release

1.9.1   10/20/2009
        status.c: fixed bug initializing serial sensor status

1.9.0   10/20/2009
        config.c: better qdpInitConfig()
        data.c: use QDP_CFG_x macros when ignoring (for now) the special packet config change blockettes
        decode.c: qdpDecode_SSCFG() (not yet tested)
        print.c: qdpBaudCode(), qdpPrintTokenCds(), qdpPrint_SSCFG(), use QDP_CFG_x in qdpPrintBT_SPEC()
        process: recognize QDP_SP_ENVDAT (sets QDP_LCQ_EVENT_ENVDAT)
        status.c: serial sensor status support
        string.c: qdpLogicalOperatorCode(), qdpSSPortString(), qdpSSTypeString(), qdpSSUnitsString()
        token.c: qdpInitIdent(), qdpSetIdent(), qdpInitTokenDet(), qdpInitLcq(),
            qdpInitTokens(), qdpFirFilterCode(), qdpLcqCode(), qdpLcqName(), qdpIirFilterCode(),
            qdpControlDetectorCode(), qdpEventDetectorCode(), qdpCommEventCode(), qdpLcqName(),
            qdpControlDetectorFilterName(), qdpSetLcqIdentName(), qdpFinalizeToken()

1.8.2   10/08/2009
        config.c: added support for set bitmask
        xml.c: moved off to libqdpxml

1.8.1   10/02/2009
        qdp/fsa.h: fixed the behavior in response to server busy during registration
        config.c: initial release
        decode.c: added web page support to qdpDecode_C1_MEM()
        mem.c: use QDP_MAX_C1_MEM_SEGLEN in place of former QDP_MAX_C1_MEM_CFGDATA
        pkts.c: moved qdpRequestFullConfig() to config.c
        print.c: added qdpBaud()
        string.c: added qdpWebPageMacro(), qdpLogicalOperatorString()
        tokens.c: added qdpControlDetectorString() and fixed comfr and pebuf defaults to agree with XML
        xml.c: initial release

1.8.0   09/28/2009
        qdp/codes.h: fixed QDP_C2_TERR, added QDP_C2_SNAPT, QDP_C3_ANNC and QDP_CX_XXXX
        qdp/tokens.h: gave QDP_TOKEN_TYPE_x names to the various token identifiers
            combined mdh and tds fields into a generic detector det field, added cen
        qdp.h: completed C2_PHY definition, updated QDP_TYPE_FULL_CONFIG, prototypes
        decode.c: completed qdpDecode_C2_PHY(), added qdpDecode_C3_ANNC()
        pkts.c: added qdpRequestTokens(), qdp_C3_RQANNC(), updated qdpRequestFullConfig()
        print.c: added support for det field, changed output ordering to more closely follow Willard ordering.
        stringc.c: added qdpDetectorString(), qdpTercmdCode(), qdpCmdCode(), qdpTercmdString()
        tokens.c: support for det, cen, added qdpLcqName(), qdpCommEventName(), qdpEventDetectorName(),
            qdpControlDetectorName(), qdpIirFilterName(), qdpFirFilterName()

1.7.4   09/22/2009
        decode.c: checkpoint (qdpDecode_C2_PHY)
        encode.c: added qdpEncode_C1_RQLOG(), qdpEncode_C2_RQPHY()
        pkts.c: added qdp_C1_RQLOG(), qdp_C2_RQPHY(), qdpRequestFullConfig()

1.7.3   09/14/2009
        pkts.c: added qdp_C1_RQMEM()
        print.c: added qdpPrintTokenChannelMap(), qdpPrintTokenReport(), changed
           format of qdpPrintTokenClock() output to make it more understandable

1.7.2   09/04/2009
        status.c: fixed bug in qdpGpsDateTimeToEpoch() sanity checks

1.7.1   07/13/2009
        status.c: sanity checks on qdpGpsDateTimeToEpoch() input

1.7.0   07/09/2009
        data.c: added code to load contents of QDP_BLOCKETTE_SPEC
        hlp.c:  added cal_in_progress flag
        print.c: introduced qdpPrintBlockettes(), added support for new
           QDP_BLOCKETTE_SPEC fields in qdpPrintBT_SPEC
        process.c: set special packet events in ProcessSPEC, changed logic in
           qdpProcessPacket() so that all blockettes get run through the
           processor even if we lack full meta data (those processors which
           require meta data just return immediately).
        status.c: added support for new tstamp fields in global and gps status

1.6.1   07/02/2009
        qdp.h: changed automatic status poll interval to 30 seconds
        status.c: added qdpCalibrationInProgress()

1.6.0   07/02/2009
        pkts.c: added qdpReadPktgz()

1.5.4   07/02/2009
        io.c: explicitly set errno to zero on normal qdpRead() returns

1.5.3   06/19/2009
        steim.c: fixed nasty B01 little-endian byte order bug,
            deleted the dead code that had been #ifdef'd out in release 1.1.1

1.5.2   03/17/2009
        fsa.c: THREAD_DETACH all threads
        version.c: added libqdpBuildIdent

1.5.1   02/23/2009
        encode.c: added qdpEncode_C1_SPP(), qdpEncode_C2_GPS()
        print.c: print all fields int qdpPrint_C2_GPS() and added new funcs
            qdpPrint_CMNHDR(), qdpPrintLCQ(), qdpPrintPkt()

1.5.0   02/03/2009
        decode.c: added qdpDecode_C2_GPS(), qdpDecode_C1_DCP(), qdpDecode_C1_MAN(),
            qdpDecode_C1_SPP(), qdpDecode_C1_GID()
        pkts.c: added qdpRequestCheckout(), qdp_C1_RQSTAT(), qdp_C2_RQGPS(),
            qdp_C1_RQMAN, qdp_C1_RQSPP(), qdp_C1_RQDCP(), qdp_C1_RQGID(),
            qdp_C1_RQFIX(), qdp_C1_RQPHY(), qdp_C1_RQSC(), qdp_C1_RQGLOB()
        print.c: added qdpPrintCheckoutPackets(), qdpPrint_C2_GPS(), qdpPrint_C1_MAN(),
            qdpPrint_C1_SPP(), qdpPrint_C1_DCPMAN(), qdpPrint_C1_DCP(), qdpPrint_C1_GID()
            and added underlines to section headers
        process.c: log ProcessBlockette() error returns
        status.c: fixed up QDP_BOOM report
        string.c: added qdpAuxtypeString(), qdpClocktypeString(), qdpCalibratorString()

1.4.0   01/23/2009
        decode.c: fixed typo with qpdDecode_C1_FIX() name, added
            qdpDecode_C1_SC(), qdpDecode_C2_AMASS()
        encode.c: added qdpEncode_C1_PULSE(), qdpEncode_C1_SC(), qdpEncode_C1_GLOB()
        print.c: added qdpPrint_C1_SC(), qdpPrint_C2_AMASS_SENSOR(), qdpPrint_C2_AMASS()
        string.c: added qdpSensorControlString(), fixed C1_GLOB typo

1.3.2   01/06/2009
        encode.c: added qdpEncode_C1_PHY()

1.3.1   12/09/2008
        reorder.c: made "out of window" messages debug comments instead of warnings

1.3.0   10/02/2008.3.0   10/02/2008
        cmds.c: added qdpNoParCmd, qdpRqstat
        decode.c: added qdpDecode_C1_PHY
        encode.c: added qdpEncode_NoParCmd
        init.c: used QDP_STATUS_DATA_PORT_x instead of QDP_LOGICAL_PORT_x_STATUS
        print.c: added qdpPrint_C1_PHY
        process.c: removed unused dlen parameter from call to qdpDecode_C1_STAT
        status.c: removed unused dlen parameter from qdpDecode_C1_STAT,
            moved GSV report to under GPS in qdpPrint_C1_STAT.

1.2.0   03/05/2008
        process.c: added support for CNP316 (aux packets)

1.1.1   01/07/2008 *** First version tested on little-endian platform ***
        data.c: little-endian bug fixes
        ida10.c: little-endian bug fixes
        print.c: adding missing argument to detect[] string in qdpPrintTokenLcq
        steim.c: #ifdef'd out some dead code

1.1.0   12/20/2007
        qdp/limits.h: created
        qdp/status.h: created
        qdp.h: moved various limits to new qdp/limits.h, added support for
               C1_STAT, defined physical ports
        proocess.c: added support for C1_STAT
        status.c: created
        string.c: added qdpPLLStateString(), qdpPhysicalPortString()

1.0.5   12/14/2007
        qdp/lcq.h: changed blockette datum types from unsigned to signed

1.0.4   10/31/2007
        encode.c: added qdpEncode_C1_QCAL()
        init.c: replaced sprintf with snprintf
        md5.c: replaced sprintf with snprintf
        stats.c: added buffer length to utilTimeString() calls

1.0.3   09/06/2007
        init.c: include port number in udpioInit failure messages

1.0.2   06/26/2007
        steim.c: fixed dnib 01 (4 8-bit differences) bug

1.0.1   06/14/2007
        hlp.c: flush partial packets
        ida10.c: generate fixed-length 1024 byte packets

1.0.0   05/18/2007
        Initial production release

 */

char *qdpVersionString()
{
static char string[] = "qdp library version 100.100.100 and slop";

    snprintf(string, strlen(string), "qdp library version %d.%d.%d",
        version.major, version.minor, version.patch
    );

    return string;
}

VERSION *qdpVersion()
{
    return &version;
}
