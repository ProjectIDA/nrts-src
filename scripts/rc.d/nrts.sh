#!/bin/sh -

# PROVIDE: nrts
# REQUIRE: LOGIN
# $Id: nrts.sh,v 1.1 2012/07/30 22:55:13 dechavez Exp $

. /etc/rc.subr

name="nrts"
stop_cmd="nrts_stop"
start_cmd="nrts_start"
required_files="/usr/nrts/etc/start/boot"

nrts_start()
{
    NRTSRC="/usr/nrts/etc/start/boot"
    if [ -f ${NRTSRC} ]; then
        chown nrts /dev/ttyd[01]*
        echo "Starting NRTS"
        csh ${NRTSRC} > /dev/null &
    fi
}

nrts_stop()
{
    echo "*** STOPPING NRTS ***"
    pkill -TERM -x tristar10
    pkill -TERM -x isidl
    pkill -TERM -x qhlp
    pkill -TERM -x isid
    pkill -TERM -x ispd
    pkill -TERM -x AlphaSend
    pkill -TERM -x drmd
    pkill -TERM -x nrts_alpha
    pkill -TERM -x nrts_edes
    pkill -TERM -x nrts_wrtdl
}

load_rc_config $name
run_rc_command "$1"
