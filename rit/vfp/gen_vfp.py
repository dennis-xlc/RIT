#!/usr/bin/python

import random

P_MAX     = 100
P_SET_FZ  = 20
P_SET_AHP = 20
P_SET_DN  = 20
P_USE_SPE_FP = 40

NZCV_POS  = 28
AHP_POS   = 26
DN_POS    = 25
FZ_POS    = 24
RM_POS    = 22

RAND_DENORMAL = 5

#half precision special nums
HP_NUMS = (
    #+-SNaN and +-QNaN
    '7c01', 'fc01', '7e00', 'fe00',
    #+-Inf
    '7c00', 'fc00',
    #+-0
    '0000', '8000',
    #+-1
    '3c00', 'bc00',
    #+-7fff/+-8000
    '7800', 'f800',
    #+-fp max
    '7bff', 'fbff',
    #+-denormal
    '03ff', '83ff', '0001', '8001'
)

#single precision special nums
SP_NUMS = (
    #+-SNaN and +-QNaN
    '7f800001', 'ff800001', '7fc00000', 'ffc00000',
    #+-Inf
    'ff800000', '7f800000',
    #+-0
    '00000000', '80000000',
    #+-1
    '3f800000', 'bf800000',
    #+-7fff
    '46fffe00', 'c6fffe00',
    #+-8000
    '47000000', 'c7000000',
    #+-7fff ffff / +-8000 0000
    '4f000000', 'cf000000',
    #+-fp max
    'ff7fffff', '7f7fffff',
    #+-denormal value
    '007fffff', '807fffff', '00000001', '80000001'
)

#double precision special nums
DP_NUMS = (
    #+-SNaN and +-QNaN
    '7ff0000000000001', 'fff0000000000001', 
    '7ff8000000000000', 'fff8000000000000',
    #+-Inf
    '7ff0000000000000', 'ffff000000000000',
    #+-0
    '0000000000000000', '8000000000000000',
    #+-1
    '3ff0000000000000', 'bff0000000000000',
    #+-7fff
    '40dfffc000000000', 'c0dfffc000000000',
    #+-8000
    '40e0000000000000', 'c0e0000000000000',
    #+-7fffffff
    '41dfffffffc00000', 'c1dfffffffc00000',
    #+-80000000
    '41e0000000000000', 'c0e0000000000000',
    #+-7fffffffffffffff / +- 8000000000000000
    '43e0000000000000', 'c3e0000000000000',
    #+-fp max value
    '7fefffffffffffff', 'ffefffffffffffff',
    #+-denormal value
    '000fffffffffffff', '800fffffffffffff',
    '0000000000000001', '8000000000000001'
)

class OPTYPE():
    SP_REG   = 1
    DP_REG   = 2
    CORE_REG = 3
    IMM      = 4


class OP():
    VLDR    = 0
    VSTR    = 1
    VLDM    = 2
    VSTM    = 3
    VPUSH   = 4
    VPOP    = 5
    
    VMSR    = 6
    VMRS    = 7
    
    VMLA    = 8
    VMLS    = 9
    VMUL    = 10
    VNMLA   = 11
    VNMLS   = 12
    VNMUL   = 13

    VCMP    = 14

    VABS    = 15
    VNEG    = 16
    VSQRT   = 17

    VADD    = 18
    VSUB    = 19
    VDIV    = 20

    VMOV    = 21

    VCVT    = 22


def get_fp_num(size):
    if random.randint(1, P_MAX) > P_USE_SPE_FP:
        fmt = "%0" + str(size/4) + "x"
        return fmt % random.randint(0, 2**size -1)
    if size == 16:
        #hp
        rval = random.randint(0, len(HP_NUMS) + RAND_DENORMAL - 1)
        if rval < len(HP_NUMS):
            return HP_NUMS[rval]
        else:
            #print "random denormal hp"
            rdenormal = (random.choice((0, 2**5)) << 10) + random.randint(1, 2**10-1)
            return "%04x" % rdenormal 
    elif size == 32:
        #sp
        rval = random.randint(0, len(SP_NUMS) + RAND_DENORMAL - 1)
        if rval < len(SP_NUMS):
            return SP_NUMS[rval]
        else:
            #print "random denormal sp"
            rdenormal = (random.choice((0, 2**8)) << 23) + random.randint(1, 2**23-1)
            return "%08x" % rdenormal
    elif size == 64:
        #dp
        rval = random.randint(0, len(DP_NUMS) + RAND_DENORMAL - 1)
        if rval < len(DP_NUMS):
            return DP_NUMS[rval]
        else:
            #print "random denormal dp"
            rdenormal = (random.choice((0, 2**11)) << 52) + random.randint(1, 2**52-1)
            return "%016x" % rdenormal
    else:
        return None

def get_rand_opnd(opnd_type, min=0, max=31):
    if opnd_type == OPTYPE.SP_REG:
        fmt = 's%d'
    elif opnd_type == OPTYPE.DP_REG:
        fmt = 'd%d'
    elif opnd_type == OPTYPE.CORE_REG:
        fmt = 'r%d'
        if max > 15:
            max = 15
    elif opnd_type == OPTYPE.IMM:
        fmt = '#%d'
    else:
        fmt = '0x%x'
    return fmt % random.randint(min, max)

def get_rand_VFPExpandImm(isF32):
    """get random value of VFPExpandImm"""
    if isF32:
        #F32: abcdefgh -> aBbbbbbc defgh000 00000000 00000000
        #0x1f: a=0, b=1; 0x20: a=0, b=0
        #0x5f: a=1, b=1; 0x60: a=1, b=0
        rval_ab = random.choice([0x1f, 0x20, 0x5f, 0x60])
        rval_cdefgh = random.randint(0, 63)
        rval = (rval_ab * 64 + rval_cdefgh) * (2**19)
    else:
        #F64: abcdefgh -> aBbbbbbb bbcdefgh 00000000 00000000 ...... 00000000
        #0xff: a=0, b=1; 0x100: a=0, b=0
        #0x2ff: a=1, b=1; 0x300: a=1, b=0
        rval_ab = random.choice([0xff, 0x100, 0x2ff, 0x300])
        rval_cdefgh = random.randint(0, 63)
        rval = (rval_ab * 64 + rval_cdefgh) * (2**48)
    return rval

def get_instrs_fix_NaN(is_dp, fp_reg):
    instrs = []
    if is_dp:
        instrs.append(('vcmp.f64', 'd%d, 0' % fp_reg))
        instrs.append(('vmrs', 'apsr_nzcv, fpscr'))
        instrs.append(('vmovvs', 'r0, r1, d%d' % fp_reg))
        instrs.append(('orrvs', 'r1, #0x80000000'))
        instrs.append(('vmovvs', 'd%d, r0, r1' % fp_reg))
    else:
        instrs.append(('vcmp.f32', 's%d, 0' % fp_reg))
        instrs.append(('vmrs', 'apsr_nzcv, fpscr'))
        instrs.append(('vmovvs', 'r1, s%d' % fp_reg))
        instrs.append(('orrvs', 'r1, #0x80000000'))
        instrs.append(('vmovvs', 's%d, r1' % fp_reg))
    return [Instr(i[0], i[1]) for i in instrs]

def gen_vfp_mem(op):
    """VLDR/VSTR"""
    rinst = None
    preinst = None
    postinst = None
    #with_label = random.choice([True, False])
    add = random.choice([True, False])
    dst = get_rand_opnd(random.choice([OPTYPE.SP_REG, OPTYPE.DP_REG]))
    n = random.randint(0, 15)
    imm = random.randint(0, 255) * 4
    if add:
        opnds = "%s, [r%d, #%d]" % (dst, n, imm)
    else:
        if n == 15:
            if imm < 12:
                imm = 0
            opnds = "%s, [r%d, #-%d]" % (dst, n, imm)
        else:
            opnds = "%s, [r%d, #-%d]" % (dst, n, imm)
    rinst = (VFP_OP_STR[op]+'<c>', opnds)
    if n < 15:
        rval = random.randint(0, 64)
        preinst = [Instr('ldr', 'r%d, =mem_data+%d' % (n, rval*4))]
    return (rinst, preinst, postinst)

def gen_vfp_multi_mem(op):
    """VLDM/VSTM/VPOP/VPUSH"""
    rinst = None
    preinst = None
    postinst = None
    #Vop{mode}<c> <Rn>{!}, <list>
    single_regs = random.choice([True, False])
    if single_regs:
        list_l = random.randint(0, 31)
        list_h = random.randint(list_l+1, 32)
        list = ['s'+str(i) for i in range(list_l, list_h)]
    else:
        list_l = random.randint(0, 31)
        list_h = random.randint(list_l+1, list_l + 16)
        if list_h > 32:
            list_h = 32
        list = ['d'+str(i) for i in range(list_l, list_h)]
    mode = ''
    opnds = ''
    if op == OP.VLDM or op == OP.VSTM:
        wback = random.choice([True, False])
        if wback:
            n = random.randint(0, 14)
            mode = random.choice(['ia', 'db'])
            opnds = "%s!, {%s}" % (get_rand_opnd(OPTYPE.CORE_REG, max=n, min=n), 
                                   ', '.join(list))
        else:
            n = random.randint(0, 15)
            opnds = '%s, {%s}' % (get_rand_opnd(OPTYPE.CORE_REG, max=n, min=n),
                                   ', '.join(list))
        rinst = (VFP_OP_STR[op]+mode+'<c>', opnds)
    elif op == OP.VPUSH or op == OP.VPOP:
        n = 13
        opnds = '{%s}' % ', '.join(list)
        rinst = (VFP_OP_STR[op]+mode+'<c>', opnds)
    else:
        print "unknown op in gen_vfp_multi_mem: ", op
        return (None, None, None)
    if n < 15:
        rval = random.randint(0, 64)
        preinst = [Instr('ldr', 'r%d, =mem_data+%d' % (n, rval*4))]
    return (rinst, preinst, postinst)

def gen_vfp_mov(op):
    """VMOV"""
    rinst = None
    preinst = None
    postinst = None
    op_postfix = '<c>'
    opnds = ''
    mov_type = random.randint(1, 7)
    if mov_type == 1:
        #VMOV (immediate)
        #VMOV<c>.F32 <Sd>, #imm
        #VMOV<c>.F64 <Dd>, #imm
        single_register = random.choice([True, False])
        if single_register:
            op_postfix = '<c>.f32'
            opnds = "%s, #0x%x" % (get_rand_opnd(OPTYPE.SP_REG),
                                   get_rand_VFPExpandImm(True))
        else:
            #op_postfix = '<c>.f64'
            #opnds = "%s, #0x%x" % (get_rand_opnd(OPTYPE.DP_REG),
            #                       get_rand_VFPExpandImm(False))
            #FIXIT
            rinst = ("fconstd<c>", "%s, %s" % (get_rand_opnd(OPTYPE.DP_REG),
                                              get_rand_opnd(OPTYPE.IMM, 0, 255)))
    elif mov_type == 2:
        #VMOV (register)
        #VMOV<c>.F32 <Sd>, <Sm>
        #VMOV<c>.F64 <Dd>, <Dm>
        single_register = random.choice([True, False])
        if single_register:
            op_postfix = '<c>.f32'
            opnds = "%s, %s" % (get_rand_opnd(OPTYPE.SP_REG),
                                  get_rand_opnd(OPTYPE.SP_REG))
        else:
            op_postfix = '<c>.f64'
            opnds = "%s, %s" % (get_rand_opnd(OPTYPE.DP_REG),
                                  get_rand_opnd(OPTYPE.DP_REG))
    elif mov_type == 3:
        #ARM core register to scalar
        #VMOV<c>.<size> <Dd[x]>, <Rt>
        op_postfix = '<c>.32'
        opnds = "%s[%d], %s" % (get_rand_opnd(OPTYPE.DP_REG),
                                  random.randint(0, 1),
                                  get_rand_opnd(OPTYPE.CORE_REG, 0, 14))
    elif mov_type == 4:
        #scalar to ARM core register
        #VMOV<c>.<dt> <Rt>, <Dn[x]>
        op_postfix = '<c>.32'
        opnds = "%s, %s[%d]" % (get_rand_opnd(OPTYPE.CORE_REG, 0, 14),
                                  get_rand_opnd(OPTYPE.DP_REG),
                                  random.randint(0, 1))
    elif mov_type == 5:
        #between ARM core register and single-precision VFP
        #VMOV<c> <Rd>, <Sn>
        #VMOV<c> <Sd>, <Rn>
        to_arm_register = random.choice([True, False])
        if to_arm_register:
            opnds = "%s, %s" % (get_rand_opnd(OPTYPE.CORE_REG, max=14),
                                get_rand_opnd(OPTYPE.SP_REG))
        else:
            opnds = "%s, %s" % (get_rand_opnd(OPTYPE.SP_REG),
                                get_rand_opnd(OPTYPE.CORE_REG, max=14))
    elif mov_type == 6:
        #between two ARM registers and two single-precision registers
        #VMOV<c> <Sm>, <Sm1>, <Rt>, <Rt2>
        #VMOV<c> <Rt>, <Rt2>, <Sm>, <Sm1>
        to_arm_register = random.choice([True, False])
        m = random.randint(0, 30)
        regs = range(14)
        if to_arm_register:
            t = random.choice(regs)
            regs.remove(t)
            t2 = random.choice(regs)
            opnds = "r%d, r%d, s%d, s%d" % (t, t2, m, m+1)
        else:
            t = random.choice(regs)
            t2 = random.choice(regs)
            opnds = "s%d, s%d, r%d, r%d" % (m, m+1, t, t2)
    else:
        #between two ARM registers and a doubleword extension register
        #VMOV<c> <Dm>, <Rt>, <Rt2>
        #VMOV<c> <Rt>, <Rt2>, <Dm>
        to_arm_register = random.choice([True, False])
        m_reg = get_rand_opnd(OPTYPE.DP_REG)
        regs = range(14)
        if to_arm_register:
            t = random.choice(regs)
            regs.remove(t)
            t2 = random.choice(regs)
            opnds = "r%d, r%d, %s" % (t, t2, m_reg)
        else:
            t = random.choice(regs)
            t2 = random.choice(regs)
            opnds = "%s, r%d, r%d" % (m_reg, t, t2)
    if not rinst:
        rinst = (VFP_OP_STR[op]+op_postfix, opnds)
    return (rinst, preinst, postinst)

def gen_vfp_sr(op):
    """VMRS/VMSR"""
    rinst = None
    preinst = None
    postinst = None
    #VMRS<c> <Rt>, FPSCR
    #VMSR<c> FPSCR, <Rt>
    opnds = ''
    if op == OP.VMRS:
        rval = random.randint(0, 15)
        if rval < 15:
            opnds = 'r%d, fpscr' % rval
        else:
            opnds = 'APSR_nzcv, fpscr'
    elif op == OP.VMSR:
        opnds = 'fpscr, %s' % get_rand_opnd(OPTYPE.CORE_REG, 0, 14)
    else:
        print "unknown op in gen_vfp_sr: ", op
        return (None, None, None)
    rinst = (VFP_OP_STR[op]+'<c>', opnds)
    return (rinst, preinst, postinst)

def gen_vfp_cvt(op):
    """VCVT"""
    rinst = None
    preinst = None
    postinst = None
    op_postfix = '<c>'
    opnds = ''
    cvt_type = random.randint(1, 4)
    if cvt_type == 1:
        #between floating-point and integer
        to_integer = random.choice([True, False])
        dp_operation = random.choice([True, False])
        Tm = random.choice(['.u32', '.s32'])
        int_opnd = get_rand_opnd(OPTYPE.SP_REG)
        if dp_operation:
            float_type = '.f64'
            float_opnd = get_rand_opnd(OPTYPE.DP_REG)
        else:
            float_type = '.f32'
            float_opnd = get_rand_opnd(OPTYPE.SP_REG)
        if to_integer:
            #{R}<c>.S32|U32.F64 <Sd>, <Dm>
            #{R}<c>.S32|U32.F32 <Sd>, <Sm>
            op_postfix = random.choice(['r', ''])+'<c>'+Tm+float_type
            opnds = "%s, %s" % (int_opnd, float_opnd)
        else:
            #<c>.F64.S32|U32 <Dd>, <Sm>
            #<c>.F32.S32|U32 <Sd>, <Sm>
            op_postfix = '<c>' + float_type + Tm
            opnds = "%s, %s" % (float_opnd, int_opnd)
    elif cvt_type == 2:
        #between floating-point and fixed-point
        #<c>.<Td>.F64 <Dd>, <Dd>, #<fbits>
        #<c>.<Td>.F32 <Sd>, <Sd>, #<fbits>
        #<c>.F64.<Td> <Dd>, <Dd>, #<fbits>
        #<c>.F32.<Td> <Sd>, <Sd>, #<fbits>
        to_fixed = random.choice([True, False])
        dp_operation = random.choice([True, False])
        size = random.choice(['16', '32'])
        unsigned = random.choice(['u', 's'])
        if size == '16':
            fbits = random.randint(0, 16)
            #FIXIT gcc build error when size is 16 and fbits is 0
            if fbits == 0:
                if dp_operation:
                    float_type = 'd'
                    d = get_rand_opnd(OPTYPE.DP_REG)
                else:
                    float_type = 's'
                    d = get_rand_opnd(OPTYPE.SP_REG)
                if to_fixed:
                    rinst = ("ftosh"+float_type, "%s, #0" % d)
                else:
                    rinst = ("fshto"+float_type, "%s, #0" % d)
        else:
            fbits = random.randint(1, 32)
        if dp_operation:
            float_type = '.f64'
            d = get_rand_opnd(OPTYPE.DP_REG)
        else:
            float_type = '.f32'
            d = get_rand_opnd(OPTYPE.SP_REG)
        opnds = "%s, %s, #%d" % (d, d, fbits)
        if to_fixed:
            op_postfix = '<c>.'+unsigned+size + float_type
        else:
            op_postfix = '<c>' + float_type + '.'+unsigned+size
    elif cvt_type == 3:
        #between double-precision and single-precision
        #<c>.F64.F32 Dd, Sm
        #<c>.F32.F64 Sd, Dm
        double_to_single = random.choice([True, False])
        dst = random.randint(0, 31)
        if double_to_single:
            op_postfix = '<c>.f32.f64'
            opnds = "%s, %s" % (get_rand_opnd(OPTYPE.SP_REG, min=dst, max=dst),
                                get_rand_opnd(OPTYPE.DP_REG))
            postinst = get_instrs_fix_NaN(False, dst)
        else:
            op_postfix = '<c>.f64.f32'
            opnds = "%s, %s" % (get_rand_opnd(OPTYPE.DP_REG, min=dst, max=dst),
                                get_rand_opnd(OPTYPE.SP_REG))
            postinst = get_instrs_fix_NaN(True, dst)
    else:
        #between half-precision and single-precision
        #<y><c>.F32.F16 <Sd>, <Sm>
        #<y><c>.F16.F32 <Sd>, <Sm>
        lowbit = random.choice(['b', 't'])
        half_to_single = random.choice(['.f32.f16', '.f16.f32'])
        op_postfix = lowbit + '<c>' + half_to_single
        opnds = "%s, %s" % (get_rand_opnd(OPTYPE.SP_REG),
                            get_rand_opnd(OPTYPE.SP_REG))
    if not rinst:
        rinst = (VFP_OP_STR[op]+op_postfix, opnds)
    return (rinst, preinst, postinst)

def gen_vfp_mul(op):
    """VMUL/VMLA/VMLS"""
    #Vop<c>.F32 <Sd>, <Sn>, <Sm>
    #Vop<c>.F64 <Dd>, <Dn>, <Dm>
    rinst = None
    preinst = None
    postinst = None
    dp_operation = random.choice([True, False])
    dst = random.randint(0, 31)
    if dp_operation:
        opcode_postfix = '.f64'
        opnd_type = OPTYPE.DP_REG
    else:
        opcode_postfix = '.f32'
        opnd_type = OPTYPE.SP_REG
    opnds = '%s, %s, %s' % (get_rand_opnd(opnd_type, min=dst, max=dst), get_rand_opnd(opnd_type),
                           get_rand_opnd(opnd_type))
    rinst = (VFP_OP_STR[op]+'<c>'+opcode_postfix, opnds)
    postinst = get_instrs_fix_NaN(dp_operation, dst)
    return (rinst, preinst, postinst)

def gen_vfp_cmp(op):
    """VCMP"""
    rinst = None
    preinst = None
    postinst = None
    #VCMP{E}<c>.F64 <Dd>, <Dm>/#0.0
    #VCMP{E}<c>.F32 <Sd>, <Sm>/#0.0
    dp_operation = random.choice([True, False])
    if dp_operation:
        opcode_postfix = '.f64'
        opnd_type = OPTYPE.DP_REG
    else:
        opcode_postfix = '.f32'
        opnd_type = OPTYPE.SP_REG
    with_zero = random.choice([True, False])
    if with_zero:
        opnds = '%s, #0' % get_rand_opnd(opnd_type)
    else:
        opnds = '%s, %s' % (get_rand_opnd(opnd_type), get_rand_opnd(opnd_type))
    quiet_nan_exc = random.choice(['e', ''])
    rinst = (VFP_OP_STR[op]+quiet_nan_exc+'<c>'+opcode_postfix, opnds)
    return (rinst, preinst, postinst)

def gen_vfp_binary(op):
    """VABS/VNEG/VSQRT"""
    rinst = None
    preinst = None
    postinst = None
    #Vop<c>.F32 <Sd>, <Sm>
    #Vop<c>.F64 <Dd>, <Dm>
    dp_operation = random.choice([True, False])
    dst = random.randint(0, 31)
    if dp_operation:
        opcode_postfix = '.f64'
        opnd_type = OPTYPE.DP_REG
    else:
        opcode_postfix = '.f32'
        opnd_type = OPTYPE.SP_REG
    opnds = '%s, %s' % (get_rand_opnd(opnd_type, min=dst, max=dst), get_rand_opnd(opnd_type))
    rinst = (VFP_OP_STR[op]+'<c>'+opcode_postfix, opnds)
    if op == OP.VSQRT:
        postinst = get_instrs_fix_NaN(dp_operation, dst)
    return (rinst, preinst, postinst)

def gen_vfp_ternay(op):
    """VADD/VSUB/VDIV"""
    rinst = None
    preinst = None
    postinst = None
    #Vop<c>.F64 <Dd>, <Dn>, <Dm>
    #Vop<c>.F32 <Sd>, <Sn>, <Sm>
    dp_operation = random.choice([True, False])
    dst = random.randint(0, 31)
    if dp_operation:
        opcode_postfix = '.f64'
        opnd_type = OPTYPE.DP_REG
    else:
        opcode_postfix = '.f32'
        opnd_type = OPTYPE.SP_REG
    opnds = '%s, %s, %s' % (get_rand_opnd(opnd_type, min=dst, max=dst), get_rand_opnd(opnd_type),
                           get_rand_opnd(opnd_type))
    rinst = (VFP_OP_STR[op]+'<c>'+opcode_postfix, opnds)
    postinst = get_instrs_fix_NaN(dp_operation, dst)
    return (rinst, preinst, postinst)


VFP_OP_STR = {
    OP.VLDR : 'vldr',
    OP.VSTR : 'vstr',
    OP.VLDM : 'vldm',
    OP.VSTM : 'vstm',
    OP.VPUSH: 'vpush',
    OP.VPOP : 'vpop',
    OP.VMSR : 'vmsr',
    OP.VMRS : 'vmrs',
    OP.VMLA : 'vmla',
    OP.VMLS : 'vmls',
    OP.VMUL : 'vmul',
    OP.VNMLA: 'vnmla',
    OP.VNMLS: 'vnmls',
    OP.VNMUL: 'vnmul',
    OP.VCMP : 'vcmp',
    OP.VABS : 'vabs',
    OP.VNEG : 'vneg',
    OP.VSQRT: 'vsqrt',
    OP.VADD : 'vadd',
    OP.VSUB : 'vsub',
    OP.VDIV : 'vdiv',
    OP.VMOV : 'vmov',
    OP.VCVT : 'vcvt'
}

VFP_GEN_HELPER = {
    OP.VLDR : gen_vfp_mem,
    OP.VSTR : gen_vfp_mem,
    OP.VLDM : gen_vfp_multi_mem,
    OP.VSTM : gen_vfp_multi_mem,
    OP.VPUSH: gen_vfp_multi_mem,
    OP.VPOP : gen_vfp_multi_mem,
    OP.VMSR : gen_vfp_sr,
    OP.VMRS : gen_vfp_sr,
    OP.VMLA : gen_vfp_mul,
    OP.VMLS : gen_vfp_mul,
    OP.VMUL : gen_vfp_mul,
    OP.VNMLA: gen_vfp_mul,
    OP.VNMLS: gen_vfp_mul,
    OP.VNMUL: gen_vfp_mul,
    OP.VCMP : gen_vfp_cmp,
    OP.VABS : gen_vfp_binary,
    OP.VNEG : gen_vfp_binary,
    OP.VSQRT: gen_vfp_binary,
    OP.VADD : gen_vfp_ternay,
    OP.VSUB : gen_vfp_ternay,
    OP.VDIV : gen_vfp_ternay,
    OP.VMOV : gen_vfp_mov,
    OP.VCVT : gen_vfp_cvt
}

def Instr(op, rest):
    """docstring for Instr"""
    return "%-16s%s" % (op, rest)

def RandomData(size):
    """docstring for RandomData"""
    inst = []
    for i in range(size/4):
        inst.append(
            Instr('.word', ', '.join(["0x%08x" % random.randint(0, 0xffffffff) for
                                     i in xrange(4)])))
    if size % 4:
        inst.append(
            Instr('.word', ', '.join(["0x%08x" % random.randint(0, 0xffffffff) for
                                     i in xrange(size%4)])))
    return "\n    ".join(inst)

def gen_init_code():
    code = """
    .text
    .align 4
    .type _start, #function
    .global _start
_start:
    %s
    b               rit_instr
    .ltorg
    """
    init_inst = []
    for i in range(32):
        for j in range(2):
            rval = random.randint(0, 0xffffffff)
            init_inst.append(Instr('ldr', 'r%d, =0x%x' % (j, rval)))
        init_inst.append(Instr('vmov', 'd%d, r0, r1' % (i)))
    for i in range(15):
        rval = random.randint(0, 64)
        init_inst.append(Instr('ldr', 'r%d, =mem_data+%d' % (i, rval * 4)))
    return code % ("\n    ".join(init_inst))

def gen_adv_rit_code(preinst, rinst):
    code = """
    .text
    .align 4
    .type _start, #function
    .global _start
_start:
    %s
    b               rit_instr
    .ltorg

    .section .rit, "awx"
prerit:
    %s

rit_instr:@%s
    %s
    b               exit
    
postrit:
    %s
    """
    fpscr_val = random.randint(0, 15) << NZCV_POS
    if random.randint(1, P_MAX) < P_SET_AHP:
        fpscr_val |= 1 << AHP_POS
    if random.randint(1, P_MAX) < P_SET_DN:
        fpscr_val |= 1 << DN_POS
    if random.randint(1, P_MAX) < P_SET_FZ:
        fpscr_val |= 1 << FZ_POS
    fpscr_val |= random.randint(0, 3) << RM_POS
    init_inst = []
    init_inst.append(Instr('ldr', 'r0, =0x%x' % fpscr_val))
    init_inst.append(Instr('vmsr', 'fpscr, r0'))
    for i in range(32):
        t = random.choice(xrange(16))
        if t == 0 or t == 1:
            fp_data = get_fp_num(16) + get_fp_num(16) + get_fp_num(16) + get_fp_num(16)
        elif t == 2 or t == 3 or t == 4 or t == 5:
            fp_data = get_fp_num(32) + get_fp_num(32)
        elif t == 6:
            fp_data = get_fp_num(32) + get_fp_num(16) + get_fp_num(16)
        elif t == 7:
            fp_data = get_fp_num(16) + get_fp_num(16) + get_fp_num(32)
        else:
            fp_data = get_fp_num(64)
        init_inst.append(Instr('ldr', 'r0, =0x%s' % fp_data[8:]))
        init_inst.append(Instr('ldr', 'r1, =0x%s' % fp_data[:8]))
        init_inst.append(Instr('vmov', 'd%d, r0, r1' % (i)))
    for i in range(15):
        rval = random.randint(0, 0xffffffff)
        init_inst.append(Instr('ldr', 'r%d, =0x%x' % (i, rval)))
        #rval = random.randint(0, 64)
        #init_inst.append(Instr('ldr', 'r%d, =mem_data+%d' % (i, rval * 4)))
    if preinst:
        init_inst += preinst
    return code % ("\n    ".join(init_inst), RandomData(256),
                  rinst[0], rinst[1], RandomData(256))

def gen_exit_code(postinst):
    code = """
exit:
.l0:
    %s
    vmov            r2, r3, s2, s3
    vmov            r4, r5, s4, s5
    vmov            r6, r7, s6, s7
    vmov            r8, r9, s8, s9
    vmov            r10, r11, s10, s11
    vmov            r12, r13, s12, s13
    b               chk_l0
.l1:
    vmov            r2, r3, s16, s17
    vmov            r4, r5, s18, s19
    vmov            r6, r7, s20, s21
    vmov            r8, r9, s22, s23
    vmov            r10, r11, s24, s25
    vmov            r12, r13, s26, s27
    b               chk_l1
.l2:
    vmov            r2, r3, s0, s1
    vmov            r4, r5, s14, s15
    vmov            r6, r7, s28, s29
    vmov            r8, r9, s30, s31
    vmrs            r10, fpscr
    ldr             r1, =0xf1c00000
    and             r10, r10, r1
    b               chk_l2
.l3:
    mov             r0, #1
    adr             r1, str_pass
    mov             r2, #6
    mov             r7, #4
    swi             #0
    mov             r0, #0
    mov             r7, #1
    swi             #0

failexit:
    mov             r0, #1
    adr             r1, str_fail
    mov             r2, #6
    mov             r7, #4
    swi             #0
    mov             r0, #1
    mov             r7, #1
    swi             #0

str_pass:
    .string "Pass!\\n\"
str_fail:
    .string "Fail!\\n\"

    .align 4
chk_l0:
    @chk_l0
    b               .l1

chk_l1:
    @chk_l1
    b               .l2

chk_l2:
    @chk_l2
    b               .l3

    .data
    .align 4
b_mem_data:
    %s

    .align 4
mem_data:
    %s

    .align 4
a_mem_data:
    %s
"""
    insts = []
    if postinst:
        insts += postinst
    return code % ("\n    ".join(insts), RandomData(253), RandomData(64), RandomData(256))

def gen_test(op_str='', input_seed=None, cond=''):
    random.seed(input_seed)

    is_op_init = False
    if op_str:
        for k in VFP_OP_STR.keys():
            if VFP_OP_STR[k] == op_str:
                is_op_init = True
                input_op = k
    if not is_op_init:
        input_op = random.choice(VFP_OP_STR.keys())

    rinst, preinst, postinst = VFP_GEN_HELPER[input_op](input_op)

    if rinst:
        opcode, rest = rinst
        gen_instr = Instr(opcode.replace('<c>', cond), rest)
        print gen_adv_rit_code(preinst, (VFP_OP_STR[input_op], gen_instr))
        print gen_exit_code(postinst)
        print "#gcc-opts -mfpu=vfpv3 -mfloat-abi=softfp -nostdlib -Ttext=0x400000 -Tdata=0x40a000 -Wl,--section-start=.rit=0x410000"
    else:
        sys.exit("bad case")

def test_gen_func():
    """docstring for main"""
    print gen_vfp_sr(OP.VMRS)
    print gen_vfp_sr(OP.VMSR)
    print gen_vfp_mem(OP.VLDR)
    print gen_vfp_mem(OP.VSTR)
    print gen_vfp_multi_mem(OP.VLDM)
    print gen_vfp_multi_mem(OP.VSTM)
    print gen_vfp_multi_mem(OP.VPUSH)
    print gen_vfp_multi_mem(OP.VPOP)
    print gen_vfp_mov(OP.VMOV)
    print gen_vfp_cvt(OP.VCVT)
    print gen_vfp_mul(OP.VMUL)
    print gen_vfp_mul(OP.VMLA)
    print gen_vfp_mul(OP.VMLS)
    print gen_vfp_mul(OP.VNMUL)
    print gen_vfp_mul(OP.VNMLA)
    print gen_vfp_mul(OP.VNMLS)
    print gen_vfp_cmp(OP.VCMP)
    print gen_vfp_binary(OP.VABS)
    print gen_vfp_binary(OP.VNEG)
    print gen_vfp_binary(OP.VSQRT)
    print gen_vfp_ternay(OP.VADD)
    print gen_vfp_ternay(OP.VSUB)
    print gen_vfp_ternay(OP.VDIV)

def test_vfp_gen():
    """docstring for test_vfp_gen"""
    print gen_vfp_instr()

def main():
    """docstring for main"""
    #test_gen_func()
    #test_vfp_gen()
    #print gen_adv_init_code()
    gen_test('')
    #print get_instrs_fix_NaN(False, 8)

if __name__ == '__main__':
    main()
