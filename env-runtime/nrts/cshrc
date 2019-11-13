set filec
set fignore=( .o )
umask 002
set ignoreeof
set noclobber
set notify
unset autologout

if (`whoami` == root) then
        set prompt="`hostname`# "
else
    set myname = nrts
    set home   = $NRTS_HOME
    set rcfiles = ""
    set rcfiles = "$rcfiles ~/.platform"
    set rcfiles = "$rcfiles ~/.envrc"
    set rcfiles = "$rcfiles ~/.pathrc"
    set rcfiles = "$rcfiles ~/.aliases"

    foreach rcfile ($rcfiles)
        if (-e $rcfile) source $rcfile
    end

    if ( $?prompt ) then
        stty erase ^H
        set prompt="$myname@$nodename \!> "
        set history = 500
        set DefaultSite = `nrtsDefaultSystem`
        if ($status == 0) then
            setenv ISP_SYSTEM $DefaultSite
            set cdpath = (~)
            set cdpath = ($cdpath ~/etc)
            set cdpath = ($cdpath ~/$DefaultSite)
            set cdpath = ($cdpath ~/$DefaultSite/iso)
        endif
        set boot = $home/etc/start/boot
        set Boot = $boot
        cd $home
    endif
endif
