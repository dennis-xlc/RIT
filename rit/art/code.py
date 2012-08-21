#!/usr/bin/python

import random
import arminfo
import options
import instrutil


class CODETYPE():
    BLOCK = 0
    INST = 1
    ATTR = 2
    LABEL = 3
    HEXINST = 4


class COND():
    EQ = 0
    NE = 1
    CS = 2
    CC = 3
    MI = 4
    PL = 5
    VS = 6
    VC = 7
    HI = 8
    LS = 9
    GE = 10
    LT = 11
    GT = 12
    LE = 13
    AL = 14


CondCode = {
    COND.EQ: 'eq',
    COND.NE: 'ne',
    COND.CS: 'cs',
    COND.CC: 'cc',
    COND.MI: 'mi',
    COND.PL: 'pl',
    COND.VS: 'vs',
    COND.VC: 'vc',
    COND.HI: 'hi',
    COND.LS: 'ls',
    COND.GE: 'ge',
    COND.LT: 'lt',
    COND.GT: 'gt',
    COND.LE: 'le',
    COND.AL: 'al'
}

ISA_A   = '.arm'
ISA_T   = '.thumb'

class Block(object):
    def __init__(self):
        self.code = []
        #self.instnum = 0

    def __str__(self):
        ret = ""
        for e in self.code:
            if e[0] is CODETYPE.BLOCK:
                ret += str(e[1])
            elif e[0] is CODETYPE.INST:
                #for inst in e[1]:
                #    ret += outputinstr(inst)
                ret += Inst(e[1])
            elif e[0] is CODETYPE.ATTR:
                ret += Attr(e[1])
            elif e[0] is CODETYPE.LABEL:
                ret += Label(e[1])
            else:
                print "[WARNING] unknown CODETYPE", e
        return ret


class DataBlock(Block):
    def __init__(self, label, size):
        super(DataBlock, self).__init__()
        self.code.append((CODETYPE.ATTR, '.data'))
        self.code.append((CODETYPE.LABEL, label))
        self.code.append((CODETYPE.INST, [('.space', str(size))]))

    def __str__(self):
        return "\n" + super(DataBlock, self).__str__()

class RDataBlock(Block):
    def __init__(self, label, size, f_fill_rdata):
        super(RDataBlock, self).__init__()
        self.code.append((CODETYPE.ATTR, '.data'))
        self.code.append((CODETYPE.LABEL, label))
        inst = []
        for i in range(size/8):
            rvals = f_fill_rdata()
            str_rvals = ", ".join(["0x%08x" % i for i in rvals])
            inst.append(('.word', str_rvals))
        if size % 8:
            rvals = f_fill_rdata(size % 8)
            str_rvals = ", ".join(["0x%08x" % i for i in rvals])
            inst.append(('.word', str_rvals))
        self.code.append((CODETYPE.INST, inst))

    def __str__(self):
        return "\n" + super(RDataBlock, self).__str__()

class ValueBlock(Block):
    def __init__(self, label, dtype, value):
        super(ValueBlock, self).__init__()
        self.code.append((CODETYPE.ATTR, '.data'))
        self.code.append((CODETYPE.LABEL, label))
        self.code.append((CODETYPE.INST, [('.'+dtype, '0x%x' % value)]))

class RBlock(Block):
    def __init__(self, label):
        super(RBlock, self).__init__()
        self.label = label
        self.label_end = label + '_end'
        self.instnum = 0
        self.isARM = True

    def makeRandomBlock(self, ctx):
        if ctx[options.CTX.ISA]:
            self.isARM = True
        else:
            self.isARM = False
        return []

    def __str__(self):
        ret = Attr('.align')
        ret += Attr(ISA_A if self.isARM else ISA_T)
        ret += Label(self.label)
        ret += "@<%s:%d>\n" % (self.label, self.instnum)
        ret += super(RBlock, self).__str__()
        ret += "@</%s>\n" % self.label
        ret += Attr('.align')
        ret += Label(self.label_end)
        return ret


def Attr(attr):
    return "    %s\n" % attr

def Label(label):
    return "%s:\n" % label

def Inst(insts):
    return "".join(["    %-16s%s\n" % (op, rest) for op, rest in insts])

def get_init_fpscr():
    fpscr_val = random.randint(0, 15) << 28
    stride = random.choice([0, 3])
    if options.getOption('vfp_vector'):
        if stride:
            len = random.randint(0, 1)
        else:
            len = random.randint(0, 3)
        fpscr_val |= stride << 20
        fpscr_val |= len << 16
    else:
        print "vfp vector mode disabled"
    #print "0x%x" % fpscr_val
    return fpscr_val

def genInitBlock(initVFP=True):
    init_b = Block()
    init_inst = []
    init_b.code.append((CODETYPE.ATTR, '.cpu cortex-a9'))
    init_b.code.append((CODETYPE.ATTR, '.fpu vfpv3'))
    init_b.code.append((CODETYPE.ATTR, '.global _start'))
    init_b.code.append((CODETYPE.ATTR, '.syntax unified'))
    init_b.code.append((CODETYPE.ATTR, '.text'))
    init_b.code.append((CODETYPE.INST, [('.space', '4096')]))
    init_b.code.append((CODETYPE.LABEL, '_start'))
    init_inst.append(('ldr', 'r0, =0x%x' % getRandRegVal()))
    init_inst.append(('msr', 'APSR_nzcvqg, r0'))
    if initVFP:
        #fpscr_val = get_init_fpscr()
        #init_inst.append(('ldr', 'r0, =0x%x' % fpscr_val))
        #init_inst.append(('vmsr', 'fpscr, r0'))
        for i in xrange(32):
            init_inst.append(('ldr', 'r0, =0x%x' % getRandRegVal()))
            init_inst.append(('ldr', 'r1, =0x%x' % getRandRegVal()))
            init_inst.append(('vmov', 'd%d, r0, r1' % i))
    for i in xrange(15):
        init_inst.append(('ldr', 'r%d, =0x%x' % (i, getRandRegVal())))
        #init_inst.append(('ldr', 'r%d, =0x%x' % (i,
        #                                         instrutil.getRandNum(arminfo.R_MUL_VAL))))
    init_b.code.append((CODETYPE.INST, init_inst))
    return init_b

def genExitBlock():
    exit_b = Block()
    exit_b.code.append((CODETYPE.ATTR, '.arm'))
    exit_b.code.append((CODETYPE.INST, 
                        [('b', 'exit'),
                        ('.ltorg', ''),
                        ('.space', '0x1000')]))
    exit_b.code.append((CODETYPE.LABEL, 'exit'))
    exit_b.code.append((CODETYPE.INST, 
                        [('mov', 'r0, #1'),
                        ('adr', 'r1, str_pass'),
                        ('mov', 'r2, #6'),
                        ('mov', 'r7, #4'),
                        ('swi', '#0'),
                        ('mov', 'r0, #0'),
                        ('mov', 'r7, #1'),
                        ('svc', '#0')]))
    exit_b.code.append((CODETYPE.LABEL, 'failexit'))
    exit_b.code.append((CODETYPE.INST, 
                        [('mov', 'r0, #1'),
                        ('adr', 'r1, str_fail'),
                        ('mov', 'r2, #6'),
                        ('mov', 'r7, #4'),
                        ('swi', '#0'),
                        ('mov', 'r0, #1'),
                        ('mov', 'r7, #1'),
                        ('svc', '#0')]))
    exit_b.code.append((CODETYPE.LABEL, 'str_pass'))
    exit_b.code.append((CODETYPE.INST, [('.string', "\"Pass!\\n\"")]))
    exit_b.code.append((CODETYPE.LABEL, 'str_fail'))
    exit_b.code.append((CODETYPE.INST, [('.string', "\"Fail!\\n\"")]))
    exit_b.code.append((CODETYPE.INST, [('.ltorg', ''), ('.space', '4096')]))
    exit_b.code.append((CODETYPE.BLOCK, genDataBlock()))
    exit_b.code.append((CODETYPE.INST, [('.bss', ''),('.space',arminfo.ALU_W_PC_VALID_SPACE_SIZE)]))
    opts = "-nostdlib -Wl,-Ttext=0x400000 -Wl,-Tdata=0x03fb0000 -Wl,-Tbss="+arminfo.ALU_W_PC_VALID_BEG_ADDR
    exit_b.code.append((CODETYPE.ATTR, "#gcc-opts %s" % opts))
    return exit_b

def genDataBlock():
    data_b = Block()
    data_b.code.append((CODETYPE.BLOCK, RDataBlock('ritdata', 0x18000,
                                                   getGeneralRandomNum)))
    data_b.code.append((CODETYPE.BLOCK, DataBlock('regbak', 64)))
    data_b.code.append((CODETYPE.BLOCK, DataBlock('cfgdata', 64)))
    data_b.code.append((CODETYPE.BLOCK, DataBlock('excladdr', 4)))
    data_b.code.append((CODETYPE.BLOCK, DataBlock('exclflag', 4)))
    data_b.code.append((CODETYPE.BLOCK, ValueBlock('exclcnt', 'word', arminfo.EXCL_CNT)))
    data_b.code.append((CODETYPE.BLOCK, RDataBlock('bv_general', 512,
                                                   getGeneralRandomNum)))
    data_b.code.append((CODETYPE.BLOCK, RDataBlock('bv_vfp', 512,
                                                   getVFPRandomNum)))
    return data_b

def getRandRegVal(min=0, max=2**32-1):
    return random.randint(min, max)

def getGeneralRandomNum(size=8):
    type = random.choice((0, 1))
    vals = []
    if type is 0:
        for i in range(size):
            vals.append(instrutil.getRandNum(arminfo.R_SHIFT_VAL))
    elif type is 1:
        for i in range(size):
            vals.append(instrutil.getRandNum(arminfo.R_MUL_VAL))
    else:
        for i in range(size):
            vals.append(random.randint(0, 0xffffffff))
    return vals

def getVFPRandomNum(size=8):
    is_sp = random.choice((True, False))
    vals = []
    if is_sp:
        for i in range(size):
            vals.append(instrutil.getRandVFPNum(is_sp)[0])
    else:
        for i in range(size/2):
            vals += instrutil.getRandVFPNum(is_sp)
        if len(vals) < size:
            vals.append(instrutil.getRandVFPNum(is_sp)[0])
    return vals

def test():
    print "Call getGeneralRandomNum..."
    vals = []
    for i in range(100):
        vals += getGeneralRandomNum()
    for i in range(len(vals)):
        print "0x%08x" % vals[i],
        if i % 8 is 7:
            print ""
    print "Call getVFPRandomNum..."
    vals = []
    for i in range(100):
        vals += getVFPRandomNum()
    for i in range(len(vals)):
        print "0x%08x" % vals[i],
        if i % 8 is 7:
            print ""


if __name__ == '__main__':
    test()
