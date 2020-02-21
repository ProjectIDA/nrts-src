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
if ($doinstall) then
    # Work in the built binary directory
    set bindir = ../bin/$PLATFORM
    if (! -d $bindir) then
        echo "ERROR: $bindir does not exist"
        goto failure
    endif
    cd $bindir

    # deploy binaries
    if (! -d ${deployroot}/bin) mkdir -p ${deployroot}/bin
    chmod 755 ${deployroot}/bin/*
    ls -l ${deployroot}/bin
    \cp $bindir/* ${deployroot}/bin/
    chmod 555 ${deployroot}/bin/*

    # deploy scripts
    if (! -d ${deployroot}/scripts) mkdir -p ${deployroot}/scripts
    chmod 755 ${deployroot}/scripts/*
    \cp scripts/* ${deployroot}/scripts/
    chmod 555 ${deployroot}/scripts/*
    # copy init.d scripts
    if (! -d ${deployroot}/scripts/init.d) mkdir -p ${deployroot}/scripts/init.d
    chmod -R 755 ${deployroot}/scripts/init.d
    \cp scripts/init.d/* ${deployroot}/scripts/init.d/
    chmod 555 ${deployroot}/scripts/init.d/*
endif

# Normal exit

success:
echo "completed OK"
exit 0

failure:
exit 1

