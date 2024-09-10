#!/usr/bin/tcsh -f
#
# Build and optionally deploy NRTS binaries and scripts
# MUST BE RUN FROM THE NRTS SRC Root directory where this script lives

set myname = "build"

# General setup
set debug = 0
set doinstall = 0
set builddcc = 0

# Command line setup
set error = 0
if ($#argv == 0) then
    pass
else if ($#argv == 1) then
    set doinstall = 1
    set nrtsroot = $argv[1]
else if ($#argv == 2) then
    set doinstall = 1
    set nrtsroot = $argv[1]
    if ($argv[2] == "dcc") then
        set builddcc = 1
    endif
else
    set error = 1
endif

if ($error) then
    set message = "bad command line, error $error"
    echo "${myname}: $message"
    echo "usage: $myname [ nrts_home_dir ]"
    goto failure
endif

# Check deploy directory exists
if ($doinstall) then
    if (! -d $nrtsroot) then
        echo "INFO: $nrtsroot does not exist"
        mkdir -p $nrtsroot >& /dev/null
        if ($status != 0) then
            echo "ERROR: can not create the deploy directory: $nrtsroot"
            goto failure
        endif
    endif
endif


# Setup environment
source env-build/platform
source env-build/envrc
source env-build/pathrc
source env-build/aliases

# do build
make

# install to nrtsroot, if requested
if ($doinstall) then

    # deploy binaries
    if (! -d ${nrtsroot}/bin) sudo mkdir -p ${nrtsroot}/bin
#    sudo chmod 755 ${nrtsroot}/bin/* >& /dev/null
    sudo \cp ../bin/${PLATFORM}/* ${nrtsroot}/bin/ >& /dev/null
#    sudo chmod 555 ${nrtsroot}/bin/*  >& /dev/null

    if ($builddcc != 1) then
        # deploy scripts
        if (! -d ${nrtsroot}/scripts) sudo mkdir -p ${nrtsroot}/scripts
#        sudo chmod 755 ${nrtsroot}/scripts/*  >& /dev/null
        sudo \cp scripts/* ${nrtsroot}/scripts/  >& /dev/null
#        sudo chmod 555 ${nrtsroot}/scripts/*  >& /dev/null
    endif

endif

# Normal exit

success:
echo "completed OK"
exit 0

failure:
exit 1

