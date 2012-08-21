#!/usr/bin/env python

#import pyinotify
import sys, os
import subprocess
import argparse
import datetime
import shutil

RIT_ROOT=os.path.realpath(os.path.join(os.path.dirname(__file__), '../..'))
ARCHIVE_DIR=os.path.join(RIT_ROOT, 'archive_seeds/'+datetime.datetime.now().strftime("%m%d%H"))
COSIM_QEMU="cosim-qemu/cosim-qemu"
COSIM_UBT="cosim/cms.elf"
TIA_UBT="tia/cms.elf"
REL_TIA_UBT="rel_tia/cms.elf"
INTRP_UBT="intrp/cms.elf"

parser = argparse.ArgumentParser(description='Launch a script to process generated seeds')
parser.add_argument('-s', '--seeds_dir', dest='directory', 
                    help='monitored directory containing seeds',
                    default=os.path.join(RIT_ROOT, 'tmp'))
parser.add_argument('-f', '--fail_dir', dest='fail_dir', 
                    help='specific directory to contain failed seeds',
                    default=os.path.join(RIT_ROOT, 'fail_seeds'))
parser.add_argument('-l', '--log', help='log file', default=sys.stdout,
                    dest='log', type=argparse.FileType('a'))
parser.add_argument('-t', '--timeout', default=5, dest='timeout', type=int,
                    help='timeout for sub-command')
parser.add_argument('--with-cosim-qemu', action='store_true', default=False, 
                    dest='cosim_qemu', help='run seed with cosim-qemu')
parser.add_argument('--without-cosim-ubt', action='store_false', default=True,
                    dest='cosim_ubt', help='run seed without cosim-ubt')
parser.add_argument('--with-intrp', action='store_true', default=False,
                    dest='intrp', help='run seed with intrp mode')
parser.add_argument('--without-tia', action='store_false', default=True,
                    dest='tia', help='run seed without tia mode')
parser.add_argument('--with-rel-tia', action='store_true', default=False,
                    dest='rel_tia', help='run seed with tia release mode')
parser.add_argument('-d', '--dest', dest='dest', help='dest dir to store processed seeds',
                    default=None)
parser.add_argument('-u', '--ubt_dir', dest='ubt_dir', help='ubt dir to load ubt bin',
                    default=None)
options = parser.parse_args()

def process_seed(seedfile):
    print "process seed:", seedfile
    cmds = ['timeout', '-s', '9', str(options.timeout)]
    failseed = False
    if options.cosim_qemu:
        if options.ubt_dir is None:
            bin = os.path.join(RIT_ROOT, 'ubt', COSIM_QEMU)
        else:
            bin = os.path.join(options.ubt_dir, COSIM_QEMU)
        qemu_cmds = cmds + [bin, seedfile]
        proc = subprocess.Popen(qemu_cmds, cwd = os.path.dirname(bin),
                               stdout = open('/dev/null', 'w'))
        proc.wait()
        if proc.returncode is 0:
            options.log.write("seed %s pass (cosim-qemu)\n" %
                             os.path.basename(seedfile))
        else:
            failseed = True
            options.log.write("seed %s fail (cosim-qemu)\n" %
                             os.path.basename(seedfile))
    if options.cosim_ubt:
        if options.ubt_dir is None:
            bin = os.path.join(RIT_ROOT, 'ubt', COSIM_UBT)
        else:
            bin = os.path.join(options.ubt_dir, COSIM_UBT)
        ubt_cmds = cmds + [bin, seedfile]
        proc = subprocess.Popen(ubt_cmds, cwd = os.path.dirname(bin),
                               stdout = open('/dev/null', 'w'))
        proc.wait()
        if proc.returncode is 0:
            options.log.write("seed %s pass (cosim-ubt)\n" %
                             os.path.basename(seedfile))
        else:
            failseed = True
            options.log.write("seed %s fail (cosim-ubt)\n" %
                             os.path.basename(seedfile))

    if options.intrp:
        if options.ubt_dir is None:
            bin = os.path.join(RIT_ROOT, 'ubt', INTRP_UBT)
        else:
            bin = os.path.join(options.ubt_dir, INTRP_UBT)
        ubt_cmds = cmds + [bin, seedfile]
        proc = subprocess.Popen(ubt_cmds, cwd = os.path.dirname(bin),
                               stdout = open('/dev/null', 'w'))
        proc.wait()
        if proc.returncode is 0:
            options.log.write("seed %s pass (intrp-ubt)\n" %
                             os.path.basename(seedfile))
        else:
            failseed = True
            options.log.write("seed %s fail (intrp-ubt)\n" %
                             os.path.basename(seedfile))

    if options.rel_tia:
        if options.ubt_dir is None:
            bin = os.path.join(RIT_ROOT, 'ubt', REL_TIA_UBT)
        else:
            bin = os.path.join(options.ubt_dir, REL_TIA_UBT)
        ubt_cmds = cmds + [bin, seedfile]
        proc = subprocess.Popen(ubt_cmds, cwd = os.path.dirname(bin),
                               stdout = open('/dev/null', 'w'))
        proc.wait()
        if proc.returncode is 0:
            options.log.write("seed %s pass (rel-tia-ubt)\n" %
                             os.path.basename(seedfile))
        else:
            failseed = True
            options.log.write("seed %s fail (rel-tia-ubt)\n" %
                             os.path.basename(seedfile))

    if options.tia:
        if options.ubt_dir is None:
            bin = os.path.join(RIT_ROOT, 'ubt', TIA_UBT)
        else:
            bin = os.path.join(options.ubt_dir, TIA_UBT)
        ubt_cmds = cmds + [bin, seedfile]
        proc = subprocess.Popen(ubt_cmds, cwd = os.path.dirname(bin),
                               stdout = open('/dev/null', 'w'))
        proc.wait()
        if proc.returncode is 0:
            options.log.write("seed %s pass (tia-ubt)\n" %
                             os.path.basename(seedfile))
        else:
            failseed = True
            options.log.write("seed %s fail (tia-ubt)\n" %
                             os.path.basename(seedfile))

    if failseed:
        shutil.move(seedfile, options.fail_dir)
    else:
        if options.dest is None:
            os.remove(seedfile)
        else:
            if not os.access(options.dest, os.F_OK):
                os.makedirs(options.dest)
            shutil.move(seedfile, options.dest)

#class EventHandler(pyinotify.ProcessEvent):
#    def process_IN_CREATE(self, event):
#        process_seed(event.pathname)

if not os.path.exists(options.fail_dir):
    os.makedirs(options.fail_dir)
#process existed seeds
for f in os.listdir(options.directory):
    fname = os.path.join(options.directory, f)
    if os.path.isfile(fname):
        process_seed(fname)
options.log.close()
sys.exit(0)

#print "Waiting for new seeds......"

#wm = pyinotify.WatchManager()
#handler = EventHandler()
#notifier = pyinotify.Notifier(wm, handler)

#wm.add_watch(options.directory, pyinotify.IN_CREATE)

#notifier.loop()
#try:
#    while True:
#        notifier.process_events()
#        if notifier.check_events():
#            notifier.read_events()
#except KeyboardInterrupt:
#    notifier.stop()
#    print "Stop processing seeds"
#    options.log.close()
#    sys.exit(0)
