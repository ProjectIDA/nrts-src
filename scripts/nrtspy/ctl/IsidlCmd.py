from __future__ import print_function
import copy
import os
import yaml
import subprocess
import time
import sys
import psutil


class IsidlCmd():

    MINIMUM_TREC = 3000
    TERM_TIMEOUT = 10

    def __init__(self, procinfo, **kwargs):

        self.cmddict = {
            "cmd": '',
            "isi_host": '',
            "isi_port": '',
            "isi_site": '',
            "site": '',
            "bg": '',
            "slink": '-noseedlink',
            "trec": 0,
            'beg': ''
        }
        self.cmdstr = ''

        if isinstance(procinfo, psutil.Process):
            self.psproc = procinfo
            self.cmdstr = ' '.join(self.psproc.cmdline())
            self._parse(self.cmdstr)
        elif isinstance(procinfo, dict):
            self.cmddict['cmd'] = procinfo.get('cmd', self.cmddict['cmd'])
            self.cmddict['isi_host'] = procinfo.get('isi_host', self.cmddict['isi_host'])
            self.cmddict['isi_port'] = procinfo.get('isi_port', self.cmddict['isi_port'])
            self.cmddict['isi_site'] = procinfo.get('isi_site', self.cmddict['isi_site'])
            self.cmddict['site'] = procinfo.get('site', self.cmddict['site'])
            self.cmddict['bg'] = procinfo.get('bg', self.cmddict['bg'])
            self.cmddict['slink'] = procinfo.get('slink', self.cmddict['slink'])
            self.cmddict['trec'] = procinfo.get('trec', self.cmddict['trec'])
            self.cmddict['beg'] = procinfo.get('beg', self.cmddict['beg'])

    @classmethod
    def get_isidl_procs(cls, user, **kwargs): # site=''):

        plist = []
        site = ''
        my_pid = os.getpid()

        for key in kwargs:
            if key == 'site':
                site = kwargs[key]

        for proc in psutil.process_iter(attrs=['pid', 'username', 'name']):
            match = user and proc.info['username'] == user and proc.info['name'].startswith('isidl')

            if match and proc.info['pid'] != my_pid:
                cmd = IsidlCmd(proc)
                if (not site) or (site == cmd.getSite()):
                    plist.append(IsidlCmd(proc))

        if plist:
            plist = sorted(plist, key=lambda k: k.getSite())

        return plist

    @classmethod
    def FromYAML(cls, yamlfn, site):

        try:
            cfgobj = yaml.load(file(yamlfn, 'r'))
        except yaml.YAMLError, exc:
            print("ERROR parsing isidl yaml config file:", cfgfile, file=sys.stderr)
            return None

        if isinstance(cfgobj, list) and \
            len(cfgobj) == 1 and \
            cfgobj[0].get(site):
            return IsidlCmd(cfgobj[0][site])
        else:
            print("ERROR invalid isidl yaml config file:", cfgfile, file=sys.stderr)
            return None



    def isvalid(self):

        valid = self.cmddict['cmd'] and \
                self.cmddict['isi_host'] and \
                self.cmddict['site'] and \
                self.cmddict['slink']

        if self.cmddict['trec']:
            try:
                trecval = int(self.cmddict['trec'])
                valid = valid and trecval >= self.MINIMUM_TREC
            except:
                valid = False


        if self.cmddict['beg']:
            try:
                _ = int(self.cmddict['beg'], 16)
            except:
                valid = False

        return valid


    def _build_cmdline(self):

        cmdline = ''
        cmdline += self.cmddict['cmd']
        cmdline += ' '
        if self.cmddict['isi_site']:
            cmdline += self.cmddict['isi_site'] + '@'
            cmdline += ' '
        cmdline += self.cmddict['isi_host']
        if self.cmddict['isi_port']:
            cmdline += ':' + self.cmddict['isi_port']
            cmdline += ' '
        cmdline += self.cmddict['site']
        cmdline += self.cmddict['bg']
        cmdline += ' '
        cmdline += self.cmddict['slink']
        cmdline += ' '
        if self.cmddict['beg']:
            cmdline += self.cmddict['beg']
            cmdline += ' '
        if self.cmddict['trec']:
            cmdline += self.cmddict['trec']
            cmdline += ' '


    def _parse(self, cmdline):

        words = cmdline.split()
        word_cnt = len(words)
        ndx = 0

        if word_cnt > 0:
            self.cmddict['cmd'] = words[0]
            ndx += 1

        while ndx < word_cnt:

            if words[ndx].startswith('isi='):
                self.cmddict['isi_host'] = words[ndx][4:]

                # check for numeric TCP port
                try:
                    self.cmddict['isi_port'] = int(words[ndx+1])
                    ndx += 1
                except ValueError:
                    self.cmddict['isi_port'] = ''

            elif words[ndx] == '-bd':
                self.cmddict['bg'] = words[ndx]

            elif words[ndx].startswith('seedlink='):
                self.cmddict['slink'] = words[ndx][len('seedlink='):]

            elif words[ndx] == '-noseedlink':
                self.cmddict['slink'] = '-noseedlink'

            elif words[ndx].startswith('trec='):
                trec = words[ndx][len('trec='):]
                # make sure it's numeric
                try:
                    self.cmddict['trec'] = int(trec)
                except ValueError:
                    self.cmddict['trec'] = 0

            elif words[ndx].startswith('beg='):
                beg = words[ndx][len('beg='):]
                # make sure it's numeric or 'oldest' or 'newest'
                try:
                    self.cmddict['beg'] = int(beg, 16)
                except ValueError:
                    if beg in ['oldest', 'newest']:
                        self.cmddict['beg'] = beg
                    else:
                        self.cmddict['beg'] = ''

            else:  # must be site...
                self.cmddict['site'] = words[ndx]

            ndx += 1


    def __str__(self):

        return self.getCmdLine()


    def getYAML(self):

        return yaml.safe_dump([{ self.getSite(): self.cmddict }], default_flow_style=False, explicit_start=True)


    def getPSProc(self):
        return self.psproc

    def getCmdLine(self):

        cmd = self.cmddict['cmd']

        if self.cmddict['isi_host']:
            cmd += ' isi=' + self.cmddict['isi_host']
            if self.cmddict['isi_port']:
                cmd += ':' + str(self.cmddict['isi_port'])

        if self.cmddict['site']:
            cmd += ' ' + self.cmddict['site']

        if self.cmddict['bg']:
            cmd += ' ' + self.cmddict['bg']

        if self.cmddict['slink']:
            if self.cmddict['slink'] == '-noseedlink':
                cmd += ' ' + self.cmddict['slink']
            else:
                cmd += ' seedlink=' + self.cmddict['slink']

        if self.cmddict['trec']:
            cmd += ' trec=' + str(self.cmddict['trec'])

        if self.cmddict['beg']:
            cmd += ' beg=' + str(self.cmddict['beg'])

        return cmd

    def getCmdDict(self):
        return copy.copy(self.cmddict)

    def getSite(self):
        return copy.copy(self.cmddict['site'])

    def start(self, quiet=False):

        if self.isvalid():
            cur = self.get_isidl_procs('nrts', site=self.cmddict['site'])
            if len(cur) > 0:
                print('ERROR: isidl already running for site ', self.cmddict['site'])
                print(cur[0])
                return False
        else:
            print('ERROR: IsidlCmd not initializedi', self.cmddict['site'])
            return False

        cmd = self.getCmdLine()
        if not quiet:
            print(cmd, end=' ...')
        newproc = subprocess.Popen(cmd, shell=True)
        if newproc.pid > 1:
            if not quiet:
                print('started.')
            self.psproc = psutil.Process(newproc.pid)
            return True
        else:
            print('ERROR: Process did not start successfully.', file=sys.stderr)
            return False

    def stop(self, quiet=False):

        if not self.psproc:
            print('ERROR: ISIDL Proecss not running', file=sys.stderr)
            return False

        if not self.psproc.is_running():
            print('ERROR: ISIDL Proecss no longer running', file=sys.stderr)
            return False

        if not quiet:
            print(self.getCmdLine())
            ans = ''
            while ans not in ['y','Y','n','N']:
                ans = raw_input('Do you REALLY wish to stop this process [' + \
                                str(self.getPSProc().pid) + '] ? (y/n): ')
                print()

            if ans not in ['y', 'Y']:
                return False

        self.psproc.terminate()
        delay = 0
        while self.psproc.is_running() and not delay > self.TERM_TIMEOUT:
            time.sleep(0.1)
            delay += 0.1

        if self.psproc.is_running():
            print('WARNING: Process [' + self.psproc.pid + \
                  '] does not want to TERMinate: ' + \
                  self.psproc.cmdline(),
                  file=sys.stderr)
            print('WARNING: Not restarting.', file=sys.stderr)
            return False

        return True

    def restart(self, quiet=False):

        stopped = self.stop(quiet=quiet)

        if stopped:
            started = self.start(quiet=quiet)
            if not started:
                print('ERROR: Process did not start successfully.', file=sys.stderr)
                return False
        else:
            print('ERROR: Process did not stop successfully.', file=sys.stderr)
            return False
