#!/usr/bin/env bash
#
#
#
#
#
#
#
#
#
function split_host_and_path() {
    local src=$1
    local srch=''
    local srcp=''
    local rethostvar=$2
    local retpathvar=$3

    #local isremote=`echo $src | grep ':'`
    echo $src | grep ':' > /dev/null
    local -i isremote=$?
    if [ $isremote -eq 0 ]
    then
        if [ $debug -gt 0 ]; then echo "'${src}' is remote"; fi
        srch=`echo "$src" | awk -F':' '{print $1}'`
        # srch=${srch}:
        srcp=`echo "$src" | awk -F':' '{print $2}'`
    else
        if [ $debug -gt 0 ]; then echo "'${src}' is not remote"; fi
        srcp=$src
    fi
    if [ $debug -gt 0 ]; then echo SRC: ${srch}${srcp}; fi

    eval "$rethostvar=$srch"
    eval "$retpathvar=$srcp"

    return
}
declare -i debug=1
declare -r myname=$0

if [ $debug -gt 0 ]
then
    echo $0 $@
fi
#if debug; then echo "debug 2"; fi

# check for correct number of args
if [[ $# -ne 2 ]]
then
    echo "usage: $myname stahost:sourcepath targetpath"
    exit 1
fi

if [ ! $NRTS_HOME ]
then
    echo "${myname}: \$NRTS_HOME not set"
    exit 1
fi

tunnel_info=$NRTS_HOME/etc/Tunnels
if [ ! -f $tunnel_info ]
then
    echo "${myname}: missing $tunnel_info"
    exit 1
fi
if [ $debug -gt 0 ]; then echo "tunnel info: $tunnel_info"; fi

declare src=$1
declare trg=$2
declare srchost=''
declare srcpath=''
declare trghost=''
declare trgpath=''

split_host_and_path $src srchost srcpath
split_host_and_path $trg trghost trgpath
if [ $debug -gt 0 ]
then
    echo "    src: $src"
    echo "srchost: $srchost"
    echo "srcpath: $srcpath"
    echo
    echo "    trg: $trg"
    echo "trghost: $trghost"
    echo "trgpath: $trgpath"
fi

if [ "$srchost" ] && [ "$trghost" ]
then
    echo "${myname}: can not copy between two remotes"
    exit 1
fi

declare src_port=22
declare trg_port=22
# check Tunnels file for srchost
if [ $srchost ]
then
    echo "check for source port: for host $srchost "
    port_string=`cat "$tunnel_info" | sed -e 's/#.*$//' -e '/^$/d' | grep -i " $srchost " `
    if [ $debug -gt 0 ]; then echo "port_string: $port_string"; fi
    if [ "$port_string" ]
    then
        srchost='localhost'
        src_port=`echo $port_string | awk '{print $3}'`
    fi
    if [ $debug -gt 0 ]; then echo "src_port: $src_port"; fi

    flist=(`ssh -p $src_port nrts@$srchost "ls $srcpath"`)
    if [ $debug ]; then echo $flist; fi

    for fl in $flist
    do
        scp -P $src_port nrts@${srchost}:$fl $trgpath
    done
fi

if [ $trghost ]
then
    echo "check for target tort: for host $trghost "
    port_string=`cat "$tunnel_info" | sed -e 's/#.*$//' -e '/^$/d' | grep -i " $trghost " `
    if [ $debug -gt 0 ]; then echo "port_string: $port_string"; fi
    trg_port=`echo $port_string | awk '{print $3}'`
    if [ $debug -gt 0 ]; then echo "trg_port: $trg_port"; fi
fi

