#!/usr/bin/python

from vfp import OP
import vfp
import random
import instrgen as ig
import randomblock as rb
import code
from code import CODETYPE
import options
import testgen

VFP_VECTOR_OP = (OP.VMLA, OP.VMLS, OP.VMUL, OP.VNMLA, OP.VNMLS, OP.VNMUL,
                 OP.VABS, OP.VNEG, OP.VSQRT, OP.VADD, OP.VSUB, OP.VDIV, OP.VMOV)

class VFPRBlock(code.RBlock):
    def makeRandomBlock(self, ctx):
        super(VFPRBlock, self).makeRandomBlock(ctx)
        limit = ig.get_instr_limit(ctx['num'])
        #limit = 5
        instrs = gen_instrs(limit)
        self.code.append((code.CODETYPE.INST, instrs))
        ctx['num'] -= len(instrs)


class VFPSingleRBlock(code.RBlock):
    def makeRandomBlock(self, ctx):
        super(VFPSingleRBlock, self).makeRandomBlock(ctx)
        instrs = []
        is_sp = random.choice([True, False])
        op = random.choice(VFP_VECTOR_OP)
        instrs += set_fpscr_vector(is_sp)
        instrs += gen_random_instr(is_sp, op, fix_nan=True)
        self.code.append((code.CODETYPE.INST, instrs))

def set_fpscr_vector(is_sp, stride=None):
    """set FPSCR.STRIDE and FPSCR.LEN for vfp vector operation"""
    tmp_reg = random.randint(0, 12)
    stride = random.choice([0, 3])
    if is_sp:
        if stride:
            len = random.randint(0, 3)
        else:
            len = random.randint(0, 7)
    else:
        if stride:
            len = random.randint(0, 1)
        else:
            len = random.randint(0, 3)
    mask = '0x00%x%x0000' % (stride, len)
    inst = [('vmrs', 'r%d, fpscr' % tmp_reg),
            ('bic', 'r%d, r%d, #0x00370000' % (tmp_reg, tmp_reg)),
            ('orr', 'r%d, r%d, #%s' % (tmp_reg, tmp_reg, mask)),
            ('vmsr', 'fpscr, r%d' % tmp_reg)]
    return inst
    
def output_instr(is_sp, str_op, oprand_num, instr_fmt, fix_fpscr=False, fix_nan=False):
    oprand = []
    max_reg = 31
    if is_sp:
        max_scalar = 7
        reg_type = 'S'
    else:
        max_scalar = 3
        reg_type = 'D'

    for i in range(oprand_num):
        is_scalar = random.choice([True, False])
        if is_scalar:
            reg = random.randint(0, max_scalar)
        else:
            reg = random.randint(max_scalar+1, max_reg)
        oprand.append('%s%d' % (reg_type, reg))
    rinst = instr_fmt % tuple(oprand)
    rinst = (str_op, rinst)
    return [rinst]

def gen_random_instr(is_sp, op, fix_fpscr=None, fix_nan=None):
    str_op = vfp.VFP_OP_STR[op]
    if is_sp:
        str_op += '.f32'
    else:
        str_op += '.f64'
    if op in VFP_VECTOR_OP:
        if op is OP.VABS or op is OP.VNEG or op is OP.VSQRT:
            num = 2
            fmt = '%s, %s'
        elif op is OP.VADD or op is OP.VSUB or op is OP.VDIV or op is OP.VMUL or op is OP.VNMUL:
            num = 3
            fmt = '%s, %s, %s'
        elif op is OP.VMLA or op is OP.VNMLA or op is OP.VMLS or op is OP.VNMLS:
            num = 3
            fmt = '%s, %s, %s'
        elif op is OP.VMOV:
            is_imm_mov = random.choice([True, False])
            if is_imm_mov:
                num = 1
                if is_sp:
                    fmt = '%s, ' + '#0x%x' % vfp.get_rand_VFPExpandImm(is_sp)
                else:
                    #TODO qemu bug?
                    fmt = '%s, ' + '#0x%x' % random.randint(0, 255)
                    str_op = 'fconstd'
                    #return None
            else:
                num = 2
                fmt = '%s, %s'
        else:
            print "Unknown vfp vector op: %s" % str(op) 
            return None
        return output_instr(is_sp, str_op, num, fmt, fix_fpscr, fix_nan)
    else:
        return None

def getFSM(rbtype, fsm_ctx):
    return (ig.FSMType.VFP_VECTOR, None)

def getRBlock(label, nest):
    if nest > 0:
        #rb_types = [rb.RBType.Basic, rb.RBType.ByPass, rb.RBType.Hammock]
        rb_types = [rb.RBType.ByPass, rb.RBType.Hammock]*nest + [rb.RBType.Basic]
        rval = random.choice(rb_types)
        #rval = rb.RBType.Basic
        if rval is rb.RBType.ByPass:
            return rb.ByPassRBlock(label)
        elif rval is rb.RBType.Hammock:
            return rb.HammockRBlock(label)
        else:
            return VFPRBlock(label)
    elif nest is -2:
        return VFPSingleRBlock(label)
    else:
        return VFPRBlock(label)

def getVFPExitBlock():
    exit_b = code.Block()
    exit_b.code.append((CODETYPE.ATTR, '.arm'))
    exit_b.code.append((CODETYPE.LABEL, 'exit'))
    exit_b.code.append((CODETYPE.LABEL, 'l0'))
    exit_b.code.append((CODETYPE.INST,
                        [('vmov', 'r2, r3, s2, s3'),
                         ('vmov', 'r4, r5, s4, s5'),
                         ('vmov', 'r6, r7, s6, s7'),
                         ('vmov', 'r8, r9, s8, s9'),
                         ('vmov', 'r10, r11, s10, s11'),
                         ('vmov', 'r12, r13, s12, s13'),
                         ('b', 'chk_l0')]))
    exit_b.code.append((CODETYPE.LABEL, 'l1'))
    exit_b.code.append((CODETYPE.INST,
                        [('vmov', 'r2, r3, s16, s17'),
                         ('vmov', 'r4, r5, s18, s19'),
                         ('vmov', 'r6, r7, s20, s21'),
                         ('vmov', 'r8, r9, s22, s23'),
                         ('vmov', 'r10, r11, s24, s25'),
                         ('vmov', 'r12, r13, s26, s27'),
                         ('b', 'chk_l1')]))
    exit_b.code.append((CODETYPE.LABEL, 'l2'))
    exit_b.code.append((CODETYPE.INST,
                        [('vmov', 'r2, r3, s0, s1'),
                         ('vmov', 'r4, r5, s14, s15'),
                         ('vmov', 'r6, r7, s28, s29'),
                         ('vmov', 'r8, r9, s30, s31'),
                         ('vmrs', 'r10, fpscr'),
                         ('ldr', 'r1, =0xffff0000'),
                         ('and', 'r10, r10, r1'),
                         ('b', 'chk_l2')]))
    exit_b.code.append((CODETYPE.LABEL, 'l3'))
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

    exit_b.code.append((CODETYPE.INST, [('.align', '4')]))
    exit_b.code.append((CODETYPE.LABEL, 'chk_l0'))
    exit_b.code.append((CODETYPE.INST, [('@chk_l0', ''),
                                        ('b', 'l1')]))
    exit_b.code.append((CODETYPE.LABEL, 'chk_l1'))
    exit_b.code.append((CODETYPE.INST, [('@chk_l1', ''),
                                        ('b', 'l2')]))
    exit_b.code.append((CODETYPE.LABEL, 'chk_l2'))
    exit_b.code.append((CODETYPE.INST, [('@chk_l2', ''),
                                        ('b', 'l3')]))
    opts = "-nostdlib -Wl,-Ttext=0x400000 -Wl,-Tdata=0x03fb0000"
    exit_b.code.append((CODETYPE.ATTR, "#gcc-opts %s" % opts))
    return exit_b

def gen_instrs(num, is_sp=None):
    insts = []
    for i in range(num):
        rval = random.randint(0, 100)
        if rval < 20:
            rinst = set_fpscr_vector(False)
        else:
            is_sp = random.choice([True, False])
            #is_sp = True
            op = random.choice(VFP_VECTOR_OP)
            #OP.VMLA, OP.VMLS, OP.VMUL, OP.VNMLA, OP.VNMLS, OP.VNMUL,
            #OP.VABS, OP.VNEG, OP.VSQRT, OP.VADD, OP.VSUB,
            #OP.VDIV, OP.VMOV
            #op = OP.VADD
            #op = OP.VMOV
            rinst = gen_random_instr(is_sp, op, fix_nan=True)
        if rinst:
            insts += rinst
    return insts

def test_vfp_vector():
    #gen_instrs(10)
    options.parseOptions()
    ctx = options.getInitContext(getFSM, getRBlock)
    output = options.getOption('output')
    exit_b = getVFPExitBlock()
    testgen.genRandomTest(ctx, output, exitblock=exit_b)


if __name__ == '__main__':
    test_vfp_vector()

