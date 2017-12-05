#!/bin/csh -f
# $Id: install.csh,v 1.1 2016/01/07 17:39:02 dechavez Exp $
#
# Format disk, install boot code and restore file systems
# When completed, remove thumbdrive, reboot into single user
# mode and run /etc/FinishInstall

set disk = ada0

# make sure we have all the dump files

set base = `pwd`
if ($status != 0) then
    echo "can't establish base directory"
    goto failure
endif

set log = $base/INSTALL.LOG
cp /dev/null $log

foreach file (root.dmp var.dmp)
    if (! -e $base/$file) then
        echo "missing $file dump file"
        goto failure
    endif
end

# create the partition table

# This first one is to ensure we have something to destroy
# in the next step, which we want to do in order to have
# an unambiguous starting point. 

gpart create -s gpt ${disk} >& /dev/null

set command = "gpart destroy -F ${disk}"
echo $command
echo "# $command" >> $log
$command >>& $log
if ($status != 0) goto failure

set command = "gpart create -s gpt ${disk}"
echo $command
echo "# $command" >> $log
$command >>& $log
if ($status != 0) goto failure

set command = "gpart add -t freebsd-boot -l gpboot -s 512K ${disk}"
echo $command
echo "# $command" >> $log
$command >>& $log
if ($status != 0) goto failure

set command = "gpart bootcode -b /boot/pmbr -p /boot/gptboot -i 1 ${disk}"
echo $command
echo "# $command" >> $log
$command >>& $log
if ($status != 0) goto failure

set command = "gpart add -t freebsd-ufs -b 1M -l gproot -s 6G ${disk}"
echo $command
echo "# $command" >> $log
$command >>& $log
if ($status != 0) goto failure

set command = "gpart add -t freebsd-swap -l gpswap -s 4G ${disk}"
echo $command
echo "# $command" >> $log
$command >>& $log
if ($status != 0) goto failure

set command = "gpart add -t freebsd-ufs -l gpvar -s 2G ${disk}"
echo $command
echo "# $command" >> $log
$command >>& $log
if ($status != 0) goto failure

set command = "gpart add -t freebsd-ufs -l gpvarj -s 2G ${disk}"
echo $command
echo "# $command" >> $log
$command >>& $log
if ($status != 0) goto failure

set command = "gpart add -t freebsd-ufs -l gptmp -s 1G ${disk}"
echo $command
echo "# $command" >> $log
$command >>& $log
if ($status != 0) goto failure

set command = "gpart add -t freebsd-ufs -l gphome ${disk}"
echo $command
echo "# $command" >> $log
$command >>& $log
if ($status != 0) goto failure

# create all the filesystems

foreach partition (2 4 5 6 7)
    set command = "newfs /dev/${disk}p${partition}"
    echo $command
    echo "# $command" >> $log
    $command >>& $log
    if ($status != 0) then
        echo "command failed"
        goto failure
    endif
end

# ensure restore mount point is present

set restore = /mnt/restore
if (! -d $restore) then
    mkdir $restore
    if ($status != 0) then
        echo "Can't create $restore mount point"
        goto failure
    endif
endif

# restore /

set partition = /dev/${disk}p2
mount $partition $restore
if ($status != 0) then
    echo "can't mount $partition"
    goto failure
endif

cd $restore
if ($status != 0) then
    echo "can't chdir $restore"
    goto failure
endif

set command = "restore rf $base/root.dmp"
echo $command
echo "# $command" >> $log
$command >>& $log
if ($status != 0) then
    echo "restore failed"
    goto failure
endif
rm restoresymtable

# Fix up /tmp

chmod a+rwxt /tmp
set partition = /dev/${disk}p6
mount $partition $restore/tmp
if ($status != 0) then
    echo "can't mount $partition (/tmp)"
    goto failure
endif
chmod a+rwxt $restore/tmp
umount $restore/tmp

# Set the initial fstab (non-journaled var, no /usr/home)

cp etc/fstab.first etc/fstab
if ($status != 0) then
    echo "can't set initial fstab"
    goto failure
endif
cd $base
sync
umount $restore

# restore /var

set partition = /dev/${disk}p4
mount $partition $restore
if ($status != 0) then
    echo "can't mount $partition"
    goto failure
endif

cd $restore
if ($status != 0) then
    echo "can't chdir $restore"
    goto failure
endif

set command = "restore rf $base/var.dmp"
echo $command
echo "# $command" >> $log
$command >>& $log
if ($status != 0) then
    echo "restore failed"
    goto failure
endif
rm restoresymtable
cd $base
sync
umount $restore

success:
echo "installation successful, log file is $log"
echo "Remove thumbdrive and reboot into single user mode,"
echo "then run /etc/FinishInstall"
exit 0

failure:
echo "installation failed, log file is $log"
exit 1

# $Log: install.csh,v $
# Revision 1.1  2016/01/07 17:39:02  dechavez
# initial production release for FreeBSD 10 systems
#
