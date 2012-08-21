#!/usr/bin/python

from ctypes import *
import random
import options, arminfo

class LR_OPT():
    OPT_ISA = 0
    OPT_ARCH_VERSION = 1
    OPT_OPKIND       = 2
    OPT_OPCODE       = 3
    OPT_SBIT         = 4
    OPT_COND         = 5
    OPT_SREG         = 6
    OPT_DREG         = 7
    OPT_REGS_LIST    = 8
    OPT_IMM          = 9
    OPT_SHRTREG      = 10
    OPT_SHIMM        = 11
    OPT_SHRTTYPE     = 12
    OPT_FP_SREG_EXT  = 13
    OPT_FP_DREG_EXT  = 14
    OPT_FP_SZ_Q_EXT  = 15
    OPT_MEM_BASE_VAL = 16
    OPT_MEM_BASE_MSK = 17
    OPT_MEM_OFFSET_VAL    = 18
    OPT_ALLOW_UNALIGN_MEM = 19
    OPT_ALLOW_ALL_INSTR   = 20
    OPT_ALLOW_BIAS_OPND   = 21
    OPT_ALLOW_BIAS_REG    = 22
    OPT_NOT_ALLOW_FIX_MEM = 23
    OPT_NOT_ALLOW_RELAX   = 24
    OPT_INSTR_BIN_INIT    = 25
    OPT_RAND_SEED         = 26
    OPT_VERBOSE           = 27

class LR_ISA():
    ISA_ARMv5    = 0
    ISA_ARMv7    = 1
    ISA_THUMB    = 2
    ISA_THUMB_32 = 3
    ISA_RAND     = 4

class LR_REG():
    R_0    = 0
    R_1    = 1
    R_2    = 2
    R_3    = 3
    R_4    = 4
    R_5    = 5
    R_6    = 6
    R_7    = 7
    R_8    = 8
    R_9    = 9
    R_10   = 10
    R_11   = 11
    R_12   = 12
    R_13   = 13
    R_14   = 14
    R_15   = 15
    R_RAND = 16

class LR_FP_REG_EXT():
    FP_SZ_Q_REG_UNSET  = 0
    FP_SZ_Q_REG_SET    = 1
    FP_SZ_Q_REG_RAND   = 2

class LR_SH_RT_TYPE():
    SH_LSL_RT_0      = 0
    SH_LSR_RT_8      = 1
    SH_ASR_RT_16     = 2
    SH_ROR_RRX_RT_24 = 3
    SH_RT_RAND       = 4

class LR_COND():
    COND_EQ   = 0
    COND_NE   = 1
    COND_CS   = 2
    COND_CC   = 3
    COND_MI   = 4
    COND_PL   = 5
    COND_VS   = 6
    COND_VC   = 7
    COND_HI   = 8
    COND_LS   = 9
    COND_GE   = 10
    COND_LT   = 11
    COND_GT   = 12
    COND_LE   = 13
    COND_AL   = 14
    COND_RAND = 15

class LR_SBIT():
    SBIT_UNSET = 0
    SBIT_SET   = 1
    SBIT_RAND  = 2

class RIT(object):
    def __init__(self, libpath, callback=None):
        #Load librit.so
        cdll.LoadLibrary(libpath)
        self.librit = CDLL(libpath)
        #gen vfp inst
        self.vfpgen = callback
    
    def gen_single_inst_with_opkind(self, isARM, opkind, cond, sbit, src_reg, dst_reg, allow_fix=True):
        if isARM:
            isa = 0
        else:
            isa = 1
            #if not options.getOption('thumb2'):
            #    isa = 2
        output = create_string_buffer(1024)
        if opkind is None:
            opkind = random.choice(list(arminfo.ALL_OP))
        c_str_opkind = c_char_p(opkind)
        if cond is None:
            cond = random.randint(0, 32)
            if cond > 14:
                cond = 14
        if sbit is None:
            sbit = random.choice([0, 1])
        if src_reg is None:
            src_reg = 16
        if dst_reg is None:
            dst_reg = 16
        retry = options.getOption('maxretry')
        if opkind == 'VFP_OP':
            inst = self.vfpgen(isARM, cond, src_reg, dst_reg, allow_fix)
            #print inst
            return inst
        #return self.vfpgen(isARM, cond, src_reg, dst_reg, allow_fix)
        while retry > 0:
            hexcode = random.randint(0, 2**32-1)
            seed = random.randint(0, 2**32-1)
            #print "<<<call py_gen_instr>>>"
            self.librit.py_gen_instr(output, isa, c_str_opkind, hexcode, cond, sbit, src_reg, dst_reg, seed)
            #print "<<<finish py_gen_instr>>>"
            status, inst = parseRIT(output.value, allow_fix)
            if status:
                #arg = '_'.join([str(isa), str(opkind), str(hexcode), str(cond),
                #               str(sbit), str(src_reg), str(dst_reg), str(seed)])
                #inst.insert(0, (('#%s' % arg, '')))
                #print isa, opkind, hexcode, cond, sbit, src_reg, dst_reg, seed
                #print inst
                return inst
            else:
                retry -= 1
        #print "[WARNING] gen instruction failed with options:"
        #print isa, opkind, cond, sbit, src_reg, dst_reg
        return None

    def gen_single_inst_with_opcode(self, isARM, opcode, cond, sbit, src_reg, dst_reg, allow_fix=True, seed=None, hexcode=None, invalid=False):
        #if isARM:
        #    isa = 0
        #else:
        #    isa = 1
        output = create_string_buffer(1024)
        if opcode is None:
            #TODO choose opcode from list
            opcode = ""
        if cond is None:
            cond = random.randint(0, 32)
            if cond > 14:
                cond = 14
        if sbit is None:
            sbit = random.choice([0, 1])
        if src_reg is None:
            src_reg = 16
        if dst_reg is None:
            dst_reg = 16
        retry = options.getOption('maxretry')
        #return self.vfpgen(isARM, cond, src_reg, dst_reg, allow_fix)
        while retry > 0:
            if hexcode is None:
                hexcode = random.randint(0, 2**32-1)
            if seed is None:
                seed = random.randint(0, 2**32-1)
            #print "<<<call py_gen_instr_with_op>>>"
            self.librit.py_gen_instr_with_op(output, opcode, hexcode, cond, sbit, src_reg, dst_reg, seed)
            #print opcode, hexcode, cond, sbit, src_reg, dst_reg, seed
            #print "<<<finish py_gen_instr_with_op>>>"
            status, inst = parseRIT(output.value, allow_fix, isARM, invalid)
            if status:
                #print isa, opcode, hexcode, cond, sbit, src_reg, dst_reg, seed
                #print inst
                return inst
            else:
                retry -= 1
        #print "[WARNING] gen instruction failed with options:"
        #print isa, opcode, cond, sbit, src_reg, dst_reg
        return None

def parseRIT(output, allow_fix, isARM=True, invalid=False):
    status = False
    inst = []
    substrs = output.splitlines()
    err_line = ''
    for s in substrs:
        if s.startswith('[INFO]'):
            #if 'VALID' in s:
            #    status = True
            #else:
            #    err_line = s
            tmps = s.strip().split(' ')
            if tmps[1] == "VALID":
                status = True
            elif tmps[1] == "UNSUPPORTED" and invalid and isARM:
                inst.append(('', ' '.join(tmps[3:])))
                status = True
            else:
                err_line = s
        elif s.startswith('[CODE]'):
            if allow_fix:
                tmps = s.strip().split(' ')
                inst.append((tmps[1], ' '.join(tmps[2:])))
            else:
                #omit fix inst
                pass
        elif s.startswith('[INST]'):
            tmps = s.strip().split(' ')
            inst.append((tmps[1], ' '.join(tmps[2:])))
        else:
            pass
            #print s
    if status:
        return status, inst
    else:
        #print "<%s>" % err_line
        return status, None

