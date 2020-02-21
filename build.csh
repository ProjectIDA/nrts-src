#!/bin/tcsh -f
#
# Build and optionally deploy NRTS binaries and scripts
# MUST BE RUN FROM THE NRTS SRC Root directory


set myname = "build"

# General setup
set debug = 0
set install = 0

# Command line setup
set error = 0
if ($#argv == 0) then
else if ($#argv == 1) then
    set deployroot = $argv[1]
    set install = 1
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
if ($install) then
    if (! -d $deployroot) then
        echo "ERROR: $deployroot does not exist"
        echo "ERROR: required directories are missing"
        goto failure
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


# install to deployroot, if requested
if ($install) then
    # Work in the built binary directory
    set bindir = ../bin/$PLATFORM
    cd $bindir
    if (! -d $bindir) then
        echo "ERROR: $bindir does not exist"
        goto failure
    endif

    # copy binaries
    cp $bindir/* ${deployroot}/bin/

    # copy scripts
    cp * scripts/* ${deployroot}/scripts/

    # copy init.d scripts
    cp * scripts/init.d/* ${deployroot}/scripts/init.d/
endif

# Normal exit

success:
echo "completed OK"
exit 0

failure:
exit 1

