#!/usr/bin/python

import random
import code

enable_vcvt_float_fix = False
enable_vcvt_dp_sp = False

#convert NaN to integer
enable_vcvt_nan_int = False


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

def get_instrs_fix_NaN(is_dp, fp_reg, isARM, reinit=False):
    instrs = []
    if is_dp:
        regs = range(12)
        ra = random.choice(regs)
        regs.remove(ra)
        rb = random.choice(regs)
        instrs.append(('vcmp.f64', 'd%d, 0' % fp_reg))
        instrs.append(('vmrs', 'apsr_nzcv, fpscr'))
        if not isARM:
            instrs.append(('itttt', 'vs'))
        instrs.append(('vmovvs', 'r%d, r%d, d%d' % (ra, rb, fp_reg)))
        #instrs.append(('orrvs', 'r1, r1, #0x80000000'))
        if reinit:
            instrs.append(('ldrvs', 'r%d, =0x%x' % (ra, code.getRandRegVal())))
            instrs.append(('ldrvs', 'r%d, =0x%x' % (rb, code.getRandRegVal())))
        else:
            instrs.append(('mvnvs', 'r%d, #0' % ra))
            instrs.append(('mvnvs', 'r%d, #0' % rb))
        instrs.append(('vmovvs', 'd%d, r%d, r%d' % (fp_reg, ra, rb)))
    else:
        regs = range(12)
        ra = random.choice(regs)
        instrs.append(('vcmp.f32', 's%d, 0' % fp_reg))
        instrs.append(('vmrs', 'apsr_nzcv, fpscr'))
        if not isARM:
            instrs.append(('ittt', 'vs'))
        instrs.append(('vmovvs', 'r%d, s%d' % (ra, fp_reg)))
        #instrs.append(('orrvs', 'r1, r1, #0x80000000'))
        if reinit:
            instrs.append(('ldrvs', 'r%d, =0x%x' % (ra, code.getRandRegVal())))
        else:
            instrs.append(('mvnvs', 'r%d, #0' % ra))
        instrs.append(('vmovvs', 's%d, r%d' % (fp_reg, ra)))
    #return [Instr(i[0], i[1]) for i in instrs]
    return instrs

def get_instrs_fix_FPSCR(reg, isARM):
    instrs = []
    instrs.append(('orr', 'r%d, r%d, #0xff' % (reg, reg)))
    return instrs

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

def set_mem_base(reg, isARM=True):
    inst = []
    if reg >= 15:
        return inst
    if isARM:
        inst.append(('bic', 'r%d, r%d, #0xfc000003' % (reg, reg)))
        inst.append(('orr', 'r%d, r%d, #0x03fc0000' % (reg, reg)))
    else:
        rval = random.randint(256, 1024)
        if reg != 13:
            #inst.append(('bic', 'r%d, r%d, #0xfc000003' % (reg, reg)))
            #inst.append(('orr', 'r%d, r%d, #0x03fc0000' % (reg, reg)))
            inst.append(('ldr', 'r%d, =ritdata+%d' % (reg, rval*4)))
        else:
            r_reg = random.randint(0, 12)
            inst.append(('ldr', 'r%d, =ritdata+%d' % (r_reg, rval*4)))
            inst.append(('mov', 'sp, r%d' % r_reg))
    return inst

def set_reg_align(reg, isARM=True):
    inst = []
    if isARM:
        if reg < 15:
            inst.append(('bic', 'r%d, r%d, #3' % (reg, reg)))
    else:
        if reg != 13 and reg != 15:
            inst.append(('bic', 'r%d, r%d, #3' % (reg, reg)))
    return inst

def gen_vfp_mem(op, isARM=True, src_reg=None, dst_reg=None, mfunc=set_mem_base):
    """VLDR/VSTR"""
    rinst = None
    preinst = None
    postinst = None
    #with_label = random.choice([True, False])
    add = random.choice([True, False])
    dst = get_rand_opnd(random.choice([OPTYPE.SP_REG, OPTYPE.DP_REG]))
    sregs = range(16)
    if src_reg:
        sregs = src_reg[:]
    if op == OP.VSTR:
        #VSTR n == 15
        #ARM: not support
        #THUMB: unpredictable
        if 15 in sregs:
            sregs.remove(15)
    n = random.choice(sregs)
    imm = random.randint(0, 255) * 4 
    if add:
        opnds = "%s, [r%d, #%d]" % (dst, n, imm)
    else:
        opnds = "%s, [r%d, #-%d]" % (dst, n, imm)
    rinst = (VFP_OP_STR[op]+'<c>', opnds)
    if n < 15: 
        #preinst = [Instr('ldr', 'r%d, =mem_data+%d' % (n, rval*4))]
        preinst = mfunc(n, isARM)
    return (rinst, preinst, postinst)

def gen_vfp_multi_mem(op, isARM=True, src_reg=None, dst_reg=None, mfunc=set_mem_base):
    """VLDM/VSTM/VPOP/VPUSH"""
    rinst = None
    preinst = None
    postinst = None
    #Vop{mode}<c> <Rn>{!}, <list>
    single_regs = random.choice([True, False])
    sregs = range(16)
    if src_reg:
        sregs = src_reg[:]
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
    if not isARM:
        if 15 in sregs:
            sregs.remove(15)
    mode = ''
    opnds = ''
    if op == OP.VLDM:
        wback = random.choice([True, False])
        if wback:
            if 15 in sregs:
                sregs.remove(15)
            n = random.choice(sregs)
            mode = random.choice(['ia', 'db'])
            opnds = "%s!, {%s}" % (get_rand_opnd(OPTYPE.CORE_REG, max=n, min=n),
                                   ', '.join(list))
        else:
            mode = 'ia'
            n = random.choice(sregs)
            opnds = '%s, {%s}' % (get_rand_opnd(OPTYPE.CORE_REG, max=n, min=n),
                                   ', '.join(list))
        rinst = (VFP_OP_STR[op]+mode+'<c>', opnds)
    elif op == OP.VSTM:
        if 15 in sregs:
            sregs.remove(15)
        n = random.choice(sregs)
        wback = random.choice([True, False])
        if wback:
            mode = random.choice(['ia', 'db'])
            opnds = "%s!, {%s}" % (get_rand_opnd(OPTYPE.CORE_REG, max=n, min=n),
                                   ', '.join(list))
        else:
            mode = 'ia'
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
        #rval = random.randint(0, 64)
        #preinst = [Instr('ldr', 'r%d, =mem_data+%d' % (n, rval*4))]
        preinst = mfunc(n, isARM)
    return (rinst, preinst, postinst)

def gen_vfp_sr(op, isARM=True, src_reg=None, dst_reg=None):
    """VMRS/VMSR"""
    rinst = None
    preinst = None
    postinst = None
    #VMRS<c> <Rt>, FPSCR
    #VMSR<c> FPSCR, <Rt>
    sregs = range(16)
    dregs = range(16)
    if src_reg:
        sregs = src_reg[:]
    if dst_reg:
        dregs = dst_reg[:]
    if not isARM:
        if 13 in sregs:
            sregs.remove(13)
        if 13 in dregs:
            dregs.remove(13)
    if not dregs or not sregs:
        return (None, None, None)
    opnds = ''
    if op == OP.VMRS:
        rval = random.choice(dregs)
        if rval < 15:
            opnds = 'r%d, fpscr' % rval
            postinst = get_instrs_fix_FPSCR(rval, isARM)
        else:
            opnds = 'APSR_nzcv, fpscr'
    elif op == OP.VMSR:
        if 15 in sregs:
            sregs.remove(15)
        rval = random.choice(sregs)
        opnds = 'fpscr, r%d' % rval
        sregs.remove(rval)
        if sregs:
            tmp = random.choice(sregs)
            preinst = [('ldr', 'r%d, =0xf7c00000' % tmp),
                       ('and', 'r%d, r%d, r%d' % (rval, rval, tmp))]
        else:
            mask = random.choice(['0x37c00000', '0xf7000000'])
            preinst = [('and', 'r%d, r%d, #%s' % (rval, rval, mask))]
    else:
        print "unknown op in gen_vfp_sr: ", op
        return (None, None, None)
    rinst = (VFP_OP_STR[op]+'<c>', opnds)
    return (rinst, preinst, postinst)

def gen_vfp_mul(op, isARM=True, src_reg=None, dst_reg=None):
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
    opnds = '%s, %s, %s' % (get_rand_opnd(opnd_type, min=dst, max=dst), get_rand_opnd(opnd_type), get_rand_opnd(opnd_type))
    rinst = (VFP_OP_STR[op]+'<c>'+opcode_postfix, opnds)
    postinst = get_instrs_fix_NaN(dp_operation, dst, isARM)
    return (rinst, preinst, postinst)

def gen_vfp_cmp(op, isARM=True, src_reg=None, dst_reg=None):
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

def gen_vfp_binary(op, isARM=True, src_reg=None, dst_reg=None):
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
        postinst = get_instrs_fix_NaN(dp_operation, dst, isARM)
    return (rinst, preinst, postinst)

def gen_vfp_ternay(op, isARM=True, src_reg=None, dst_reg=None):
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
    opnds = '%s, %s, %s' % (get_rand_opnd(opnd_type, min=dst, max=dst), get_rand_opnd(opnd_type), get_rand_opnd(opnd_type))
    rinst = (VFP_OP_STR[op]+'<c>'+opcode_postfix, opnds)
    postinst = get_instrs_fix_NaN(dp_operation, dst, isARM)
    return (rinst, preinst, postinst)

def gen_vfp_mov(op, isARM=True, src_reg=None, dst_reg=None):
    """VMOV"""
    rinst = None
    preinst = None
    postinst = None
    op_postfix = '<c>'
    opnds = ''
    mov_type = random.randint(1, 7)
    sregs = range(16)
    dregs = range(16)
    if src_reg:
        sregs = src_reg[:]
    if dst_reg:
        dregs = dst_reg[:]
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
        if 15 in sregs:
            sregs.remove(15)
        if not isARM:
            if 13 in sregs:
                sregs.remove(13)
        rval = random.choice(sregs)
        opnds = "%s[%d], r%d" % (get_rand_opnd(OPTYPE.DP_REG),
                                  random.randint(0, 1), rval)
    elif mov_type == 4:
        #scalar to ARM core register
        #VMOV<c>.<dt> <Rt>, <Dn[x]>
        op_postfix = '<c>.32'
        if 15 in dregs:
            dregs.remove(15)
        if not isARM:
            if 13 in dregs:
                dregs.remove(13)
        rval = random.choice(dregs)
        opnds = "r%d, %s[%d]" % (rval, get_rand_opnd(OPTYPE.DP_REG),
                                  random.randint(0, 1))
    elif mov_type == 5:
        #between ARM core register and single-precision VFP
        #VMOV<c> <Rd>, <Sn>
        #VMOV<c> <Sd>, <Rn>
        to_arm_register = random.choice([True, False])
        if 15 in sregs:
            sregs.remove(15)
        if 15 in dregs:
            dregs.remove(15)
        if not isARM:
            if 13 in sregs:
                sregs.remove(13)
            if 13 in dregs:
                dregs.remove(13)
        if to_arm_register:
            rval = random.choice(dregs)
            opnds = "r%d, %s" % (rval, get_rand_opnd(OPTYPE.SP_REG))
        else:
            rval = random.choice(sregs)
            opnds = "%s, r%d" % (get_rand_opnd(OPTYPE.SP_REG), rval)
    elif mov_type == 6:
        #between two ARM registers and two single-precision registers
        #VMOV<c> <Sm>, <Sm1>, <Rt>, <Rt2>
        #VMOV<c> <Rt>, <Rt2>, <Sm>, <Sm1>
        to_arm_register = random.choice([True, False])
        m = random.randint(0, 30)
        if 15 in sregs:
            sregs.remove(15)
        if 15 in dregs:
            dregs.remove(15)
        if not isARM:
            if 13 in sregs:
                sregs.remove(13)
            if 13 in dregs:
                dregs.remove(13)
        if to_arm_register:
            t = random.choice(dregs)
            dregs.remove(t)
            t2 = random.choice(dregs)
            opnds = "r%d, r%d, s%d, s%d" % (t, t2, m, m+1)
        else:
            t = random.choice(sregs)
            t2 = random.choice(sregs)
            opnds = "s%d, s%d, r%d, r%d" % (m, m+1, t, t2)
    else:
        #between two ARM registers and a doubleword extension register
        #VMOV<c> <Dm>, <Rt>, <Rt2>
        #VMOV<c> <Rt>, <Rt2>, <Dm>
        to_arm_register = random.choice([True, False])
        m_reg = get_rand_opnd(OPTYPE.DP_REG)
        if 15 in sregs:
            sregs.remove(15)
        if 15 in dregs:
            dregs.remove(15)
        if not isARM:
            if 13 in sregs:
                sregs.remove(13)
            if 13 in dregs:
                dregs.remove(13)
        if to_arm_register:
            t = random.choice(dregs)
            dregs.remove(t)
            t2 = random.choice(dregs)
            opnds = "r%d, r%d, %s" % (t, t2, m_reg)
        else:
            t = random.choice(sregs)
            t2 = random.choice(sregs)
            opnds = "%s, r%d, r%d" % (m_reg, t, t2)
    if not rinst:
        rinst = (VFP_OP_STR[op]+op_postfix, opnds)
    return (rinst, preinst, postinst)

def gen_vfp_cvt(op, isARM=True, src_reg=None, dst_reg=None):
    """VCVT"""
    rinst = None
    preinst = None
    postinst = None
    op_postfix = '<c>'
    opnds = ''
    alltypes = range(1, 5)
    if not enable_vcvt_float_fix:
        alltypes.remove(2)
    cvt_type = random.choice(alltypes)
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
            if enable_vcvt_nan_int:
                op_postfix = random.choice(['r', ''])+'<c>'+Tm+float_type
                opnds = "%s, %s" % (int_opnd, float_opnd)
            else:
                op_postfix = random.choice(['r', ''])+'vc'+Tm+float_type
                opnds = "%s, %s" % (int_opnd, float_opnd)
                if dp_operation:
                    preinst = [('vcmp.f64', '%s, 0' % float_opnd)]
                else:
                    preinst = [('vcmp.f32', '%s, 0' % float_opnd)]
                preinst.append(('vmrs', 'apsr_nzcv, fpscr'))
                if not isARM:
                    preinst.append(('it', 'vc'))
                #print preinst

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
            postinst = get_instrs_fix_NaN(False, dst, isARM)
        else:
            op_postfix = '<c>.f64.f32'
            opnds = "%s, %s" % (get_rand_opnd(OPTYPE.DP_REG, min=dst, max=dst),
                                get_rand_opnd(OPTYPE.SP_REG))
            postinst = get_instrs_fix_NaN(True, dst, isARM)
    else:
        #between half-precision and single-precision
        #<y><c>.F32.F16 <Sd>, <Sm>
        #<y><c>.F16.F32 <Sd>, <Sm>
        lowbit = random.choice(['b', 't'])
        if enable_vcvt_dp_sp:
            half_to_single = random.choice(['.f32.f16', '.f16.f32'])
        else:
            half_to_single = '.f32.f16' 
        op_postfix = lowbit + '<c>' + half_to_single
        opnds = "%s, %s" % (get_rand_opnd(OPTYPE.SP_REG),
                            get_rand_opnd(OPTYPE.SP_REG))
    if not rinst:
        rinst = (VFP_OP_STR[op]+op_postfix, opnds)
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

MEM_VFP_OP = (OP.VLDR, OP.VSTR, OP.VLDM, OP.VSTM, OP.VPUSH, OP.VPOP)

def getRegs(reg):
    if reg:
        if reg >= 0 and reg < 4:
            return range(0, 4)
        elif reg >= 4 and reg < 8:
            return range(4, 8)
        elif reg >= 8 and reg < 13:
            return range(8, 13)
        elif reg == 13:
            return [13]
        else:
            return range(16)
    else:
        return range(16)

def set_inst_cond(instr, cond):
    op, rest = instr
    str_cond = code.CondCode[cond]
    if cond == 14:
        str_cond = ''
    return (op.replace('<c>', str_cond), rest)

def getOpcodes(isARM, src_reg, dst_reg):
    opcodes = VFP_GEN_HELPER.keys()
    if src_reg:
        if src_reg > 12:
            opcodes.remove(OP.VLDR)
            opcodes.remove(OP.VSTR)
            opcodes.remove(OP.VLDM)
            opcodes.remove(OP.VSTM)
        if src_reg > 13:
            opcodes.remove(OP.VPUSH)
            opcodes.remove(OP.VPOP)
        if src_reg == 15:
            opcodes.remove(OP.VMSR)
        if src_reg == 13:
            if not isARM:
                opcodes.remove(OP.VMSR)
                opcodes.remove(OP.VMOV)
    if dst_reg:
        if dst_reg > 12 and OP.VMOV in opcodes:
            opcodes.remove(OP.VMOV)
        if dst_reg == 13:
            if not isARM:
                opcodes.remove(OP.VMRS)
    return opcodes

def reinitVFPRegs(isARM, label, limit):
    base_reg = random.randint(0, 12)
    if limit < 64:
        offset = 0
    else:
        offset = random.randint(0, (limit-64))*4
    insts = [('ldr', 'r%d, =%s+%d' % (base_reg, label, offset))]
    is_single = random.choice([True, False])
    if is_single:
        list_l = random.randint(0, 8)
        list_h = random.randint(list_l+1, 32)
        rlist = ['s'+str(i) for i in range(list_l, list_h)]
    else:
        list_l = random.randint(0, 23)
        list_h = random.randint(list_l+9, list_l + 16)
        if list_h > 32:
            list_h = 32
        rlist = ['d'+str(i) for i in range(list_l, list_h)]
    insts.append(('vldmia', 'r%d, {%s}' % (base_reg, ', '.join(rlist))))
    #print insts
    return insts

def gen_instr(isARM, cond, src_reg, dst_reg, allow_fix=True):
    retinst = []
    s = getRegs(src_reg)
    d = getRegs(dst_reg)
    opcodes = getOpcodes(isARM, src_reg, dst_reg)
    if not isARM:
        cond = 14  
    if allow_fix:
        op = random.choice(opcodes)
        rinst, preinst, postinst = VFP_GEN_HELPER[op](op, isARM, s, d)
    else:
        if OP.VMRS in opcodes:
            opcodes.remove(OP.VMRS)
        op = random.choice(opcodes)
        if op in MEM_VFP_OP:
            rinst, preinst, postinst = VFP_GEN_HELPER[op](op, isARM, s, d, set_reg_align)
        else:
            rinst, preinst, postinst = VFP_GEN_HELPER[op](op, isARM, s, d)
    if rinst:
        #reinit vfp regs with boundary value
        rval = random.randint(0, 100)
        if rval > 80:
            retinst += reinitVFPRegs(isARM, 'bv_vfp', 512)
        if preinst:
            retinst += preinst
        retinst.append(set_inst_cond(rinst, cond))
        if postinst:
            retinst += postinst
    else:
        return None
    #print isARM, cond, src_reg, dst_reg
    #print retinst
    return retinst

def test_gen_func():
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

def test_gen_single_vfp_instr():
    for i in range(1000):
        #isARM = random.choice([True, False])
        isARM = False
        cond = random.randint(0, 14)
        src = random.randint(0, 15)
        dst = random.randint(0, 15)
        isFix = random.choice([True, False])
        print gen_instr(isARM, cond, src, dst, isFix)[0]

if __name__ == '__main__':
    test_gen_func()
    test_gen_single_vfp_instr()
