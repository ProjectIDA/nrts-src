#!/bin/tcsh -f
#
# Build and optionally deploy NRTS binaries and scripts
# MUST BE RUN FROM THE NRTS SRC Root directory


set myname = "build"

# General setup
set debug = 0
set doinstall = 0

# Command line setup
set error = 0
if ($#argv == 0) then
else if ($#argv == 1) then
    set nrtsroot = $argv[1]
    set doinstall = 1
else
    set error = 1
endif

if ($error) then
    set message = "bad command line, error $error"
    echo "${myname}: $message"
    echo "usage: $myname nrtsrootdir"
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
make remove
make

# install to nrtsroot, if requested
if ($doinstall) then

    # deploy binaries
    if (! -d ${nrtsroot}/bin) mkdir -p ${nrtsroot}/bin
    chmod 755 ${nrtsroot}/bin/* >& /dev/null
    \cp ../bin/${PLATFORM}/* ${nrtsroot}/bin/ >& /dev/null
    chmod 555 ${nrtsroot}/bin/*  >& /dev/null

    # deploy scripts
    if (! -d ${nrtsroot}/scripts) mkdir -p ${nrtsroot}/scripts
    chmod 755 ${nrtsroot}/scripts/*  >& /dev/null
    \cp scripts/* ${nrtsroot}/scripts/  >& /dev/null
    chmod 555 ${nrtsroot}/scripts/*  >& /dev/null
    # copy init.d scripts
    if (! -d ${nrtsroot}/scripts/init.d) mkdir -p ${nrtsroot}/scripts/init.d
    chmod -R 755 ${nrtsroot}/scripts/init.d  >& /dev/null
    \cp scripts/init.d/* ${nrtsroot}/scripts/init.d/  >& /dev/null
    chmod 555 ${nrtsroot}/scripts/init.d/*  >& /dev/null
endif

# Normal exit

success:
echo "completed OK"
exit 0

failure:
exit 1

