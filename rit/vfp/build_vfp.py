#!/usr/bin/python

#build generated random test

import subprocess
import string
import random
import sys

def build(src, dst=None, opts=None):
    if not opts:
        with open(src, 'r') as f:
            lastline = f.readlines()[-1]
            if lastline.startswith('#gcc-opts'):
                opts = string.split(lastline)[1:]
    #print opts
    cmd = ['arm-none-linux-gnueabi-gcc'] + opts + [src]
    if dst:
        cmd += ['-o', dst]
    ret, output = execCMD(cmd)
    if ret != 0:
        sys.exit('build fail: %s' % ' '.join(cmd))

def addChkCode(src, bin, sym):
    cmd = ['nm', bin]
    ret, output = execCMD(cmd)
    exit_eip = 0
    for line in output:
        if sym in line:
            substrs = string.split(line)
            if substrs[-1] == sym:
                exit_eip = int(substrs[0], 16)
    #print hex(exit_eip)
    
    #get cpu info before exit_eip from qemu.log
    cmd = ['qemu-arm', '-singlestep', '-addr', hex(exit_eip), '-d', 'cpu', bin]
    ret, output = execCMD(cmd)
    #TODO check ret code

    #load cpu state from qemu.log
    cpustate = {}
    qemulog = open('qemu.log', 'r').readlines()
    for line in qemulog:
        if line.startswith('[CPUSTATE]'):
            for str in string.split(line):
                if '=' in str:
                    subs = str.split('=')
                    cpustate[subs[0]] = int(subs[1], 16)
    chk_instrs = ["    %-8s%s\n" % (op, rest) for op, rest in genChkCode(cpustate)]

    with open(src, 'r') as sfile:
        content = sfile.readlines()
    for idx in range(len(content)):
        if content[idx].startswith(sym+':'):
            break
    ncontent = content[:idx+1] + chk_instrs + content[idx+1:]
    #update src file with checking code
    with open(src, 'w') as sfile:
        for line in ncontent:
            sfile.write(line)

def genChkCode(state):
    instrs = []
    #check r0-r14
    reg_list = range(15)
    tmp_reg = 0
    reg_list.remove(tmp_reg)
    for i in reg_list:
        instrs.append(('ldr', 'r%d, =0x%x' % (tmp_reg, state['R'+'%02d' % i])))
        instrs.append(('cmp', 'r%d, r%d' % (i, tmp_reg)))
        instrs.append(('bne', 'failexit'))
    return instrs

def processTest(test_src, isAddChkCode=True):
    if '.' in test_src:
        test_bin = '.'.join(test_src.split('.')[:-1]) + '.bin'
    else:
        test_bin = test_src + '.bin'
    build(test_src, test_bin)
    if isAddChkCode:
        addChkCode(test_src, test_bin, 'chk_l0')
        build(test_src, test_bin)
        addChkCode(test_src, test_bin, 'chk_l1')
        build(test_src, test_bin)
        addChkCode(test_src, test_bin, 'chk_l2')
        build(test_src, test_bin)

def execCMD(cmd):
    #print " ".join(cmd)
    proc = subprocess.Popen(tuple(cmd), stdout=subprocess.PIPE)
    proc.wait()
    outdata = proc.stdout.readlines()
    retcode = proc.returncode
    return (retcode, outdata)

def main():
    for test in sys.argv[1:]:
        processTest(test)


if __name__ == '__main__':
    main()
