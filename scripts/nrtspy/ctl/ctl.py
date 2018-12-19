#!/usr/bin/env python

from __future__ import print_function
import argparse
#import psutil
import os
import sys
import yaml
#import signal
from nrtspy.ctl.IsidlCmd import IsidlCmd


def procs_yaml(procs):

    pcmdlist = []
    for p in procs:
        pcmdlist.append({ p.getSite(): p.cmddict })

    return yaml.safe_dump(pcmdlist, default_flow_style=False, explicit_start=True)


if __name__ == '__main__':
    parser = argparse.ArgumentParser()

    subparsers = parser.add_subparsers(help='commands', dest='command')

    proc_start_parser = subparsers.add_parser('proc-start', help='Start isidl process from isidl yaml config file')
    proc_start_parser.add_argument('site', action='store', help='SITE for which isidl should be started', default='')
    proc_start_parser.add_argument('-d', '--dir', action='store',
                              help='Override default directory ($NRTS_HOME/etc/isidl) for ISIDL yaml config files',
                              default='')
    proc_start_parser.add_argument('-q', '--quiet', action='store_true', help="Run in 'quiet' mode", default=False)

    proc_restart_parser = subparsers.add_parser('proc-restart',
                                           help='Restart the curent isidl process for SITE using same command line.')
    proc_restart_parser.add_argument('site', action='store', help='SITE for which isidl should be restarted.', default='')
    proc_restart_parser.add_argument('-y', '--yes', action='store_true', help='Auto-answer YES to interactive prompts.',
                                default=False)

    proc_stop_parser = subparsers.add_parser('proc-stop', help='Stop isidl process for SITE')
    proc_stop_parser.add_argument('site', action='store', help='SITE for which isidl should be stopped', default='')
    proc_stop_parser.add_argument('-y', '--yes', action='store_true', help='Auto-answer YES to interactive prompts.',
                             default=False)

    proc_info_parser = subparsers.add_parser('proc-info',
                                             help='Write isidl config of currently running isidl process to STDOUT. ' + \
                                             'All sites shown unless SITE is specified')
    proc_info_parser.add_argument('site', action='store',
                                  help='SITE for which isidl process should be output', default='', nargs='?')
    proc_info_parser.add_argument('-y', '--yaml', action='store_true', help='Output process info in YAML format',
                                  default=False)

    cfg_info_parser = subparsers.add_parser('cfg-info',
                                             help='Write existing isidl yaml config for SITE to STDOUT')
    cfg_info_parser.add_argument('site', action='store',
                                  help='SITE for which isidl yaml config should be output', default='')
    cfg_info_parser.add_argument('-d', '--dir', action='store',
                                 help='Override default directory ($NRTS_HOME/etc/isidl) for ISIDL yaml config files',
                                 default='')

    cmdline = parser.parse_args()

    if cmdline.command == 'proc-stop':

        procs = IsidlCmd.get_isidl_procs('nrts', site=cmdline.site)

        if len(procs) == 0:
            print('ERROR: No matching processes found', file=sys.stderr)

        elif len(procs) == 1:
            procs[0].stop(quiet=cmdline.yes)

        else:
            print('ERROR: Too many matching processes', file=sys.stderr)
            for p in procs:
                print(p.getYAML())

    elif cmdline.command == 'proc-restart':

        procs = IsidlCmd.get_isidl_procs('nrts', site=cmdline.site)

        if len(procs) == 0:
            print('ERROR: No matching processes found', file=sys.stderr)
            sys.exit(1)

        elif len(procs) == 1:
            procs[0].restart(quiet=cmdline.yes)

        else:
            print('ERROR: Too many matching processes', file=sys.stderr)
            for p in procs:
                print(p.getYAML())

    elif cmdline.command == 'proc-start':

        if not cmdline.dir:
            nrts_home = os.getenv('NRTS_HOME')
            if not nrts_home:
                print('ERROR: Config directory not specified and NRTS_HOME not set.', file=sys.stderr)
            else:
                cfgdir = os.path.join(nrts_home, 'etc', 'isidl')
        else:
            cfgdir = cmdline.dir

        if not os.path.exists(cfgdir):
            print('ERROR: Config directory does not exist:', cfgdir, file=sys.stderr)
            sys.exit(1)

        cgdir = os.path.normpath(cfgdir)
        cfgfile = os.path.join(cfgdir, cmdline.site+'.isidl.cfg')

        if not os.path.isfile(cfgfile):
            print('ERROR: Config file does not exist:', cfgfile, file=sys.stderr)
            sys.exit(1)

        cmd = IsidlCmd.FromYAML(cfgfile, cmdline.site)
        if cmd:
            cmd.start(quiet=cmdline.quiet)

    elif cmdline.command == 'proc-info':

        procs = IsidlCmd.get_isidl_procs('nrts', site=cmdline.site)

        if len(procs) == 0:
            print('ERROR: No matching processes found', file=sys.stderr)

        else:
            if cmdline.yaml:
                yaml = procs_yaml(procs)
                print(yaml)
            else:
                for p in procs:
                    print(p.cmdstr)

    elif cmdline.command == 'cfg-info':

        if not cmdline.dir:
            nrts_home = os.getenv('NRTS_HOME')
            if not nrts_home:
                print('ERROR: Config directory not specified and NRTS_HOME not set.', file=sys.stderr)
            else:
                cfgdir = os.path.join(nrts_home, 'etc', 'isidl')
        else:
            cfgdir = cmdline.dir

        if not os.path.exists(cfgdir):
            print('ERROR: Config directory does not exist:', cfgdir, file=sys.stderr)
            sys.exit(1)

        cgdir = os.path.normpath(cfgdir)
        cfgfile = os.path.join(cfgdir, cmdline.site+'.isidl.cfg')

        if not os.path.isfile(cfgfile):
            print('ERROR: Config file does not exist:', cfgfile, file=sys.stderr)
            sys.exit(1)

        cmd = IsidlCmd.FromYAML(cfgfile, cmdline.site)

        if cmd:
            print('YAML CONFIG:\n' + cmd.getYAML())
            print('\nCOMMAND LINE:', cmd.getCmdLine())
        else:
            print("ERROR invalid isidl yaml config file:", cfgfile, file=sys.stderr)

