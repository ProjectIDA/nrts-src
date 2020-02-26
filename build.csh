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
    set deployroot = $argv[1]
    set doinstall = 1
else
    set error = 1
endif

if ($error) then
    set message = "bad command line, error $error"
    echo "${myname}: $message"
    echo "usage: $myname build-root deploy-root"
    goto failure
endif

# Check deploy directory exists
if ($doinstall) then
    if (! -d $deployroot) then
        echo "INFO: $deployroot does not exist"
        mkdir -p $deployroot >& /dev/null
        if ($status != 0) then
            echo "ERROR: can not create the deploy directory: $deployroot"
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
# make remove
make


# install to deployroot, if requested
if ($doinstall) then

    # deploy binaries
    if (! -d ${deployroot}/bin) mkdir -p ${deployroot}/bin
    chmod 755 ${deployroot}/bin/* >& /dev/null
    \cp ../bin/${PLATFORM}/* ${deployroot}/bin/ >& /dev/null
    chmod 555 ${deployroot}/bin/*  >& /dev/null

    # deploy scripts
    if (! -d ${deployroot}/scripts) mkdir -p ${deployroot}/scripts
    chmod 755 ${deployroot}/scripts/*  >& /dev/null
    \cp scripts/* ${deployroot}/scripts/  >& /dev/null
    chmod 555 ${deployroot}/scripts/*  >& /dev/null
    # copy init.d scripts
    if (! -d ${deployroot}/scripts/init.d) mkdir -p ${deployroot}/scripts/init.d
    chmod -R 755 ${deployroot}/scripts/init.d  >& /dev/null
    \cp scripts/init.d/* ${deployroot}/scripts/init.d/  >& /dev/null
    chmod 555 ${deployroot}/scripts/init.d/*  >& /dev/null
endif

# Normal exit

success:
echo "completed OK"
exit 0

failure:
exit 1

