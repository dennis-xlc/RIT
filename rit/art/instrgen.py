#!/usr/bin/python

import random

import code
import options
from options import CTX
import randomblock as rb
import arminfo


class FSMType():
    RANDOM      = 0
    #same opkind: mem/mul/mul-shift/...
    SAME_KIND   = 10
    #def/use nzcv flags
    FLAG_DU     = 20
    FLAG_DU_D   = 21
    FLAG_DU_J   = 22
    FLAG_DU_U   = 23
    #def/use registers
    REG_ALLOC   = 30
    REG_ALLOC_D = 31
    REG_ALLOC_J = 32
    REG_ALLOC_U = 33

STR_FSM = {
    FSMType.RANDOM      :'f_random',
    FSMType.SAME_KIND   :'f_sameopk',
    FSMType.FLAG_DU     :'f_flagdu',
    FSMType.FLAG_DU_D   :'f_def_flag',
    FSMType.FLAG_DU_J   :'f_joint_flag',
    FSMType.FLAG_DU_U   :'f_use_flag',
    FSMType.REG_ALLOC   :'f_regalloc',
    FSMType.REG_ALLOC_D :'f_def_reg',
    FSMType.REG_ALLOC_J :'f_joint_reg',
    FSMType.REG_ALLOC_U :'f_use_reg'
}


def genCodeBlocks(blocks, ctx, fsm=None):
    for b in blocks:
        if type(b) == tuple:
            s_fsm = []
            for sub_blocks in b:
                n_fsm = nextFSM(fsm)
                s_fsm.append(genCodeBlocks(sub_blocks, ctx, n_fsm))
            fsm = random.choice(s_fsm)
        else:
            fsm = nextFSM(fsm)
            ctx[CTX.ISA] = b.isARM
            genBlockInstrs(b, fsm, ctx)
    return fsm

#Begin: Xu Lvcai

def genCodeBlocksWithLoadInsts(blocks, ctx, fsm=None):
    for b in blocks:
        if type(b) == tuple:
            s_fsm = []
            for sub_blocks in b:
                n_fsm = nextFSM(fsm)
                s_fsm.append(genCodeBlocksWithLoadInsts(sub_blocks, ctx, n_fsm))
                fsm = random.choice(s_fsm)
        else:
            fsm = nextFSM(fsm)
            ctx[CTX.ISA] = b.isARM
            genBlockInstrsWithLoadInsts(b, fsm, ctx)
    return fsm

def genBlockInstrsWithLoadInsts(block, fsm, ctx):
    instrs = []
    #opkind = []
    ropts = {}

    limit = getBlockLimit(block, ctx)

    regs = [arminfo.REGType.R_0_3, arminfo.REGType.R_4_7, None]

    if ctx[CTX.ISA]:
        regs.append(arminfo.REGType.R_8_12)
        regs.append(arminfo.REGType.R_SP)

    reg = random.choice(regs)
    #instrs = genLoadInstrs(reg, ropts, limit, ctx)
    instrs = genMemInstrs(reg, ropts, limit, ctx)

    print "%s[%s]:    %d" % (block.label, STR_FSM[fsm], len(instrs))
    block.code.append((code.CODETYPE.INST, instrs))

def genLoadInstrs(reg, ropts, limit, ctx):
    inst = []
    isa = ctx[options.CTX.ISA]
    allow_fix = True
    m_lo, m_hi = getMemLimit()
    if reg == arminfo.REGType.R_SP:
        mem_op = list(arminfo.STACK_OP)
        allow_fix = False
        inst += setReg(arminfo.REGType.R_SP, m_lo, m_hi, isa, True)
    elif reg == arminfo.REGType.R_0_3:
        mem_op = list(arminfo.LD_OP)
        allow_fix = False
        for i in range(0, 4):
            inst += setReg(i, m_lo, m_hi, isa, True)
        inst += setReg(arminfo.REGType.R_SP, m_lo, m_hi, isa, True)
    elif reg == arminfo.REGType.R_4_7:
        mem_op = list(arminfo.LD_OP)
        allow_fix = False
        for i in range(4, 8):
            inst += setReg(i, m_lo, m_hi, isa, True)
        inst += setReg(arminfo.REGType.R_SP, m_lo, m_hi, isa, True)
    else:
        mem_op = list(arminfo.LD_OP)
    if allow_fix or isa:
        dst_regs = list(arminfo.CANDIDATE_REGS)
        dst_regs.remove(arminfo.REGType.R_PC)
    else:
        dst_regs = [arminfo.REGType.R_0_3, arminfo.REGType.R_4_7]
    if reg in dst_regs:
        dst_regs.remove(reg)
    maxretry = options.getOption('maxretry')
    cnt = 0
    retry = 0
    while cnt < limit:
        rcond, rsbit, _, _ = extOptions(ropts)
        opkind = random.choice(mem_op)
        dst_reg = random.choice(dst_regs)
        ret = ctx['rit'].gen_single_inst_with_opkind(isa, opkind, rcond, rsbit, reg, dst_reg, allow_fix)
        if ret:
            inst += ret
            cnt += len(ret)
            retry = 0
        else:
            retry+= 1
        if retry > maxretry:
            cnt += 1
            retry = 0

    return inst

#end: Xu Lvcai

def nextFSM(prev_fsm):
    alltypes = (FSMType.RANDOM, FSMType.SAME_KIND, FSMType.FLAG_DU_D,
                FSMType.REG_ALLOC_D)
    
    if prev_fsm is None:
        return random.choice(alltypes)
    else:
        if prev_fsm is FSMType.FLAG_DU_D:
            return random.choice((FSMType.FLAG_DU_J, FSMType.FLAG_DU_U))
        elif prev_fsm is FSMType.FLAG_DU_J:
            return FSMType.FLAG_DU_U
        elif prev_fsm is FSMType.REG_ALLOC_D:
            return random.choice((FSMType.REG_ALLOC_J, FSMType.REG_ALLOC_U))
        elif prev_fsm is FSMType.REG_ALLOC_J:
            return FSMType.REG_ALLOC_U
        else:
            return random.choice(alltypes)

def getBlockLimit(block, ctx):
    bb_num = len(ctx[CTX.BB].keys())
    if bb_num < 10:
        min_val = options.getOption(CTX.NUM) / bb_num
        max_val = options.getOption(CTX.INSTLIMIT) + min_val
    else:
        min_val = 10
        max_val = options.getOption(CTX.INSTLIMIT)/2 + min_val
    if block.label in ctx[CTX.BB]:
        if CTX.INSTLIMIT in ctx[CTX.BB][block.label]:
            max_val = ctx[CTX.BB][block.label][CTX.INSTLIMIT]
    return random.randint(min_val, max_val)

def genBlockInstrs(block, fsm, ctx):
    instrs = []
    opkind = []
    ropts = {}
    limit = getBlockLimit(block, ctx)
    if fsm is FSMType.RANDOM:
        instrs = genInstrs(opkind, ropts, limit, ctx)
    elif fsm is FSMType.SAME_KIND:
        #same opkind / mul-shift / vfp_vector
        type = random.randint(0,6)
        if type is 0:
            regs = [arminfo.REGType.R_0_3, arminfo.REGType.R_4_7, None]
            if ctx[CTX.ISA]:
                #TODO enable these regs for thumb
                regs.append(arminfo.REGType.R_8_12)
                regs.append(arminfo.REGType.R_SP)
                #regs.append(arminfo.REGType.R_LR)
            reg = random.choice(regs)
            instrs = genMemInstrs(reg, ropts, limit, ctx)
        elif type is 1:
            instrs = genMulShiftInstrs(ropts, limit, ctx)
        elif type is 2:
            opkind = list(arminfo.VFP_OP)
            instrs = genInstrs(opkind, ropts, limit, ctx)
        elif type is 3:
            opkind = list(arminfo.DATA_OP)
            instrs = genInstrs(opkind, ropts, limit, ctx)
        else:
            opkind = [random.choice(arminfo.ALL_OP)]
            instrs = genInstrs(opkind, ropts, limit, ctx)
    elif fsm is FSMType.FLAG_DU:
        pass
    elif fsm is FSMType.FLAG_DU_D:
        instrs = genFlagDefInstrs(opkind, ropts, limit, ctx)
    elif fsm is FSMType.FLAG_DU_J:
        instrs = genFlagJointInstrs(opkind, ropts, limit, ctx)
    elif fsm is FSMType.FLAG_DU_U:
        instrs = genFlagUseInstrs(opkind, ropts, limit, ctx)
    elif fsm is FSMType.REG_ALLOC:
        pass
    elif fsm is FSMType.REG_ALLOC_D:
        instrs = genRegAllocDefInstrs(opkind, ropts, limit, ctx)
    elif fsm is FSMType.REG_ALLOC_J:
        instrs = genRegAllocJointInstrs(opkind, ropts, limit, ctx)
    elif fsm is FSMType.REG_ALLOC_U:
        instrs = genRegAllocUseInstrs(opkind, ropts, limit, ctx)
    else:
        pass 
    #print "%s[%s]:    %d" % (block.label, STR_FSM[fsm], len(instrs))
    block.code.append((code.CODETYPE.INST, instrs))

def genFlagDefInstrs(opkind, ropts, limit, ctx):
    ropts['sbit'] = [1]
    return genInstrs(opkind, ropts, limit, ctx)

def genFlagJointInstrs(opkind, ropts, limit, ctx):
    ropts['sbit'] = [0]
    ropts['cond'] = [14]
    return genInstrs(opkind, ropts, limit, ctx)

def genFlagUseInstrs(opkind, ropts, limit, ctx):
    ropts['cond'] = []
    n = random.randint(1, 2**14-1)
    for i in range(14):
        if n & 2**i:
            ropts['cond'].append(i)
    return genInstrs(opkind, ropts, limit, ctx)

def genRegAllocDefInstrs(opkind, ropts, limit, ctx):
    regs = list(arminfo.CANDIDATE_REGS)
    regs.remove(arminfo.REGType.R_PC)
    ctx[CTX.REGALLOC] = random.choice(regs)
    ropts['dst_reg'] = [ctx[CTX.REGALLOC]]
    return genInstrs(opkind, ropts, limit, ctx)

def genRegAllocJointInstrs(opkind, ropts, limit, ctx):
    if CTX.REGALLOC in ctx:
        reg = ctx[CTX.REGALLOC]
    else:
        reg = random.choice(arminfo.CANDIDATE_REGS)
    src_regs = list(arminfo.CANDIDATE_REGS)
    src_regs.remove(reg)
    ropts['src_reg'] = src_regs
    dst_regs = list(arminfo.CANDIDATE_REGS)
    dst_regs.remove(reg)
    if arminfo.REGType.R_PC in dst_regs:
        dst_regs.remove(arminfo.REGType.R_PC)
    ropts['dst_reg'] = dst_regs
    return genInstrs(opkind, ropts, limit, ctx)

def genRegAllocUseInstrs(opkind, ropts, limit, ctx):
    if CTX.REGALLOC in ctx:
        reg = ctx[CTX.REGALLOC]
    else:
        reg = random.choice(arminfo.CANDIDATE_REGS)
    ropts['src_reg'] = [reg]
    return genInstrs(opkind, ropts, limit, ctx)

def genITInstr(opkind, ropts, ctx):
    retry = 0
    while retry < 8:
        rinstr = genInstrs(opkind, ropts, 1, ctx)
        if rinstr:
            valid = True
            if len(rinstr) == 1:
                op, rest = rinstr[0]
                #MOV_reg_T2 not permitted in IT Block
                if op == '.inst.n':
                    hexcode = int(rest, 16)
                    if hexcode < 64:
                        #print "found MOV_reg_T2 in IT Block"
                        #print rinstr
                        valid = False
            else:
                valid = False
            if valid:
                return rinstr
            else:
                retry += 1
        retry += 1
    return []

def genThumbIT(opkind, ropts, ctx, n=None):
    if n is None or n > 4 or n < 1:
        n = random.randint(1, 4)
    itinst = []
    instr = []
    newopkind = list(opkind[:])
    for i in range(n):
        instr += genITInstr(newopkind, ropts, ctx)
    if instr:
        if len(instr) < n:
            n = len(instr)
    else:
        return None
    #firstcond = random.randint(0, 14)
    #FIXIT build fail when using cond al
    firstcond = random.randint(0, 13)
    if n == 1:
        masks = list(arminfo.IT_MASK)[:1]
    elif n == 2:
        masks = list(arminfo.IT_MASK)[1:3]
    elif n == 3:
        masks = list(arminfo.IT_MASK)[3:7]
    elif n == 4:
        masks = list(arminfo.IT_MASK)[7:]
    else:
        masks = list(arminfo.IT_MASK)
    mask = random.choice(masks)
    if firstcond == code.COND.AL:
        #if firstcond == ''
        mask = mask.replace('e', 't')
    num = len(mask) + 1
    it_cond = [code.CondCode[firstcond]]
    for i in range(len(mask)):
        if mask[i] == 'e':
            it_cond.append(code.CondCode[firstcond ^ 1])
        else:
            it_cond.append(code.CondCode[firstcond])
    itinst.append(('it%s' % mask, code.CondCode[firstcond]))
    for i in range(n):
        op, rest = instr[i]
        if op == '.inst.n' or op == '.inst.w':
            newop = op
        else:
            if '.' in op:
                idx = op.index('.')
                newop = op[:idx] + it_cond[i] + op[idx:]
            else:
                newop = op + it_cond[i]
        #itinst[i][0] = op
        itinst.append((newop, rest))
    return itinst

def genMulShiftInstrs(ropts, limit, ctx):
    rmul_opkind = list(arminfo.MUL_OPKIND)
    if ctx[options.CTX.ISA]:
        rshift_op = list(arminfo.ARM_SHIFT_OP)
    else:
        rshift_op = list(arminfo.THUMB_SHIFT_OP)
    maxretry = options.getOption('maxretry')
    isa = ctx[options.CTX.ISA]
    inst = []
    cnt = 0
    retry = 0
    while cnt < limit:
        gen_mul = random.choice([True, False])
        rcond, rsbit, rsrc_reg, rdst_reg = extOptions(ropts)
        if gen_mul:
            ropkind = random.choice(rmul_opkind)
            ret = ctx['rit'].gen_single_inst_with_opkind(isa, ropkind, rcond, rsbit, rsrc_reg, rdst_reg)
        else:
            ropcode = random.choice(rshift_op)
            ret = ctx['rit'].gen_single_inst_with_opcode(isa, ropcode, rcond, rsbit, rsrc_reg, rdst_reg)
        if ret:
            inst += ret
            cnt += 1
            retry = 0
        else:
            retry += 1
        if retry > maxretry:
            cnt += 1
            retry = 0
        #reinit regs with boundary value
        rval = random.randint(0, 100)
        if rval > 80:
            fixinst = reinitGeneralRegs(isa, 'bv_general', 512)
            inst += fixinst
            cnt += len(fixinst)
    return inst

def genMemInstrs(reg, ropts, limit, ctx):
    inst = []
    isa = ctx[options.CTX.ISA]
    allow_fix = True
    m_lo, m_hi = getMemLimit()
    if reg == arminfo.REGType.R_SP:
        mem_op = list(arminfo.STACK_OP)
        allow_fix = False
        inst += setReg(arminfo.REGType.R_SP, m_lo, m_hi, isa, True)
    elif reg == arminfo.REGType.R_0_3:
        mem_op = list(arminfo.MEM_OP)
        allow_fix = False
        for i in range(0, 4):
            inst += setReg(i, m_lo, m_hi, isa, True)
        inst += setReg(arminfo.REGType.R_SP, m_lo, m_hi, isa, True)
    elif reg == arminfo.REGType.R_4_7:
        mem_op = list(arminfo.MEM_OP)
        allow_fix = False
        for i in range(4, 8):
            inst += setReg(i, m_lo, m_hi, isa, True)
        inst += setReg(arminfo.REGType.R_SP, m_lo, m_hi, isa, True)
    else:
        mem_op = list(arminfo.MEM_OP)
    if allow_fix or isa:
        dst_regs = list(arminfo.CANDIDATE_REGS)
        dst_regs.remove(arminfo.REGType.R_PC)
    else:
        dst_regs = [arminfo.REGType.R_0_3, arminfo.REGType.R_4_7]
    if reg in dst_regs:
        dst_regs.remove(reg)
    maxretry = options.getOption('maxretry')
    cnt = 0
    retry = 0
    while cnt < limit:
        rcond, rsbit, _, _ = extOptions(ropts)
        opkind = random.choice(mem_op)
        dst_reg = random.choice(dst_regs)
        ret = ctx['rit'].gen_single_inst_with_opkind(isa, opkind, rcond, rsbit, reg, dst_reg, allow_fix)
        if ret:
            inst += ret
            cnt += len(ret)
            retry = 0
        else:
            retry += 1
        if retry > maxretry:
            cnt += 1
            retry = 0
    return inst

def genInstrs(opkind, ropts, limit, ctx):
    #print opkind, ropts, limit
    maxretry = options.getOption('maxretry')
    cnt = 0
    retry = 0
    inst = []
    isa = ctx[options.CTX.ISA]
    while cnt < limit:
        gen_it = False
        if not isa:
            if limit > 5:
                r = random.randint(0, 100)
                if r < options.getOption('per_it'):
                    gen_it = True
        if gen_it:
            it_num = random.randint(1, 4)
            ret = genThumbIT(opkind, ropts, ctx, it_num)
        else:
            if opkind:
                ropkind = random.choice(opkind)
            else:
                ropkind = None
            rcond, rsbit, rsrc_reg, rdst_reg = extOptions(ropts)
            ret = ctx['rit'].gen_single_inst_with_opkind(isa, ropkind, rcond, rsbit, rsrc_reg, rdst_reg)
        if ret:
            inst += ret
            cnt += len(ret)
            retry = 0
        else:
            retry += 1
        if retry >= maxretry:
            cnt += 1
            retry = 0
    return inst

def extOptions(ropts):
    if 'cond' in ropts:
        rcond = random.choice(ropts['cond'])
    else:
        rcond = None
    if 'sbit' in ropts:
        rsbit = random.choice(ropts['sbit'])
    else:
        rsbit = None
    if 'src_reg' in ropts:
        rsrc_reg = random.choice(ropts['src_reg'])
    else:
        rsrc_reg = None
    if 'dst_reg' in ropts:
        rdst_reg = random.choice(ropts['dst_reg'])
    else:
        rdst_reg = None
    return (rcond, rsbit, rsrc_reg, rdst_reg)

def get_instr_limit(n):
    if n < 10:
        rval = random.randint(5, 10)
    else:
        rval = random.randint(5, n)
    return rval

def setReg(reg, min, max, isARM, isAlign=False):
    """gen instructions to set reg in range(min, max)"""
    rval = random.randint(min, max)
    if isAlign:
        rval = rval / 4 * 4
    if isARM:
        instr = ('ldr', 'r%d, =0x%x' % (reg, rval))
    else:
        instr = ('ldr.w', 'r%d, =0x%x' % (reg, rval))
    return [instr]

def genBranch(isARM, ctx, target, isconditional, cond=None, forward=False,
              off_block=None):
    allow_cbz = False
    allow_adr = False
    if isconditional:
        if cond is None:
            allow_cbz = True
            cond = random.choice(arminfo.CONDCODE)
    else:
        cond = ''
    
    #check condition for cbz/cbnz
    if isARM or ctx['nest'] > 0:
        allow_cbz = False
    if not forward:
        allow_cbz = False
    
    if isARM:
        if ctx['nest'] < 2:
            allow_adr = True
        branch_op = 'b' + cond
    else:
        if ctx['nest'] < 1:
            branch_op = 'b' + cond
        else:
            branch_op = 'b' + cond + '.w'
    #binst = [('.arm' if isARM else '.thumb', ''), (branch_op, target)]
    binst = [(branch_op, target)]
    if allow_cbz:
        per_use_cbz = 20
        per = random.randint(0, 100)
        if per < per_use_cbz:
            cbz_inst = random.choice(['cbz', 'cbnz'])
            reg = random.randint(0, 7)
            binst = [(cbz_inst, 'r%d, %s' % (reg, target))]
            #print binst
        if off_block:
            ctx[CTX.BB][off_block.label][CTX.INSTLIMIT] = 32
    if allow_adr:
        per_use_adr = 20
        per = random.randint(0, 100)
        if per < per_use_adr:
            binst = [('adr'+cond, 'pc, '+target)]
    if not isconditional:
        binst.append(('.ltorg', ''))
    return binst

def genLoop(isARM, ctx, target):
    idx = ctx['nest']*4
    inc = random.randint(1, 10)
    #limit = random.randint(0, 500) * random.choice([1, 2, 4])
    limit = random.randint(0, 256)
    r1 = random.randint(0, 7)
    regs = range(0, 8)
    regs.remove(r1)
    r2 = random.choice(regs)
    loopinst = [('ldr', 'r%d, =cfgdata+32+%d' % (r2, idx)),
                ('ldr', 'r%d, [r%d]' % (r1, r2)),
                ('add', 'r%d, r%d, #%d' % (r1, r1, inc)),
                ('str', 'r%d, [r%d]' % (r1, r2))]
    if isARM:
        loopinst.append(('cmp', 'r%d, #%d' % (r1, limit)))
        loopinst.append(('ble', target))
    else:
        loopinst.append(('cmp.w', 'r%d, #%d' % (r1, limit)))
        loopinst.append(('ble.w', target))
    return loopinst

def genBX(isARM, ctx, target, isconditional, cond=None):
    use_bx = random.choice([True, False])
    if isconditional:
        if cond is None:
            cond = random.choice(arminfo.CONDCODE)
    else:
        cond = ''
    if isARM:
        if use_bx:
            tmp_reg = random.randint(0, 15)
            if tmp_reg is 15:
                bxinst = [('.inst', '0x%x12fff1f' % random.randint(0, 14)),
                          ('ldr%s' % cond, 'pc, =%s+1' % target),
                          ('ldr%s' % cond, 'pc, =%s+1' % target)]
            else:
                bxinst = [('ldr', 'r%d, =%s+1' % (tmp_reg, target)),
                          ('bx%s' % cond, 'r%d' % tmp_reg)]
        else:
            bxinst = [('ldr%s' % cond, 'pc, =%s+1' % target)]
    else:
        if use_bx:
            if isconditional:
                tmp_reg = random.randint(0, 14)
            else:
                tmp_reg = random.randint(0, 15)
            if tmp_reg is 15:
                bxinst = [('.align', ''),
                          ('bx', 'pc'),
                          ('nop', ''),
                          ('.arm', ''),
                          ('ldr', 'pc, =%s' % target),
                          ('.thumb', '')]
            else:
                bxinst = [('ldr.w', 'r%d, =%s' % (tmp_reg, target))]
                if isconditional:
                    bxinst.append(('it', cond))
                    bxinst.append(('bx%s' % cond, 'r%d' % tmp_reg))
                else:
                    bxinst.append(('bx', 'r%d' % tmp_reg))
        else:
            if isconditional:
                bxinst = [('it', cond),
                          ('ldr%s.w' % cond, 'pc, =%s' % target)]
            else:
                bxinst = [('ldr.w', 'pc, =%s' % target)]
    #print bxinst
    return bxinst

def genCall(isCallerARM, isCalleeARM, ctx, target):
    use_blx_reg = random.choice([True, False])
    callinst = []
    if use_blx_reg:
        #BLX(register)
        tmp_reg = random.randint(0, 14)
        if not isCalleeARM:
            target += '+1'
        if isCallerARM:
            callinst.append(('ldr', 'r%d, =%s' % (tmp_reg, target)))
            cond = random.choice([random.choice(arminfo.CONDCODE), ''])
        else:
            callinst.append(('ldr.w', 'r%d, =%s' % (tmp_reg, target)))
            cond = ''
        callinst.append(('blx'+cond, 'r%d' % tmp_reg))
    else:
        #BL/BLX(imm)
        if isCallerARM != isCalleeARM:
            opcode = 'blx'
        else:
            opcode = 'bl'
        if isCallerARM and isCalleeARM:
            cond = random.choice([random.choice(arminfo.CONDCODE), ''])
        else:
            cond = ''
        callinst.append((opcode+cond, target))
    return callinst

def genLDREX(isARM, ctx, isconditional, selfLabelPrefix, ExclSize=None, cond=None):
    elinst = []
    if isconditional:
        if cond is None:
            cond = random.choice(arminfo.CONDCODE)
    else:
        cond = ''

    if isARM:
        wsize_suffix = ''
    else:
        wsize_suffix = '.w'   

    m_lo, m_hi = getMemLimit()
    rval = random.randint(m_lo, m_hi)
    if ExclSize is None:
        ExclSize = random.choice(arminfo.OP_SIZE)
    if ExclSize == 'h':
        rval = rval >> 1 << 1
    elif ExclSize == 'w':
        rval = rval >> 2 << 2
    elif ExclSize == 'd':
        rval = rval >> 3 << 3

    reglist = range(15)
    rn = random.choice(reglist)
    if isARM:
        if ExclSize == 'd':
            reglist.remove(14)
            reglist = [i for i in reglist if i&1==0]
    else:
        reglist.remove(13)
    rt = random.choice(reglist)
    if 13 in reglist:
        reglist.remove(13)
    rp = random.choice(reglist)
    reglist.remove(rp)
    rq = random.choice(reglist)
    
    elinst.append(('mrs', 'r%d, APSR' % (rp)))
    elinst.append(('ldr'+wsize_suffix, 'r%d, =exclflag+0' % (rq)))
    elinst.append(('str'+wsize_suffix, 'r%d, [r%d]' % (rp, rq)))
    elinst.append((selfLabelPrefix+'_excl_back_edge', ':'))
    elinst.append(('ldr'+wsize_suffix, 'r%d, =exclflag+0' % (rq)))
    elinst.append(('ldr'+wsize_suffix, 'r%d, [r%d]' % (rp, rq)))
    elinst.append(('msr', 'APSR_nzcvqg, r%d' % (rp)))
    if isARM:
        elinst.append(('ldr'+cond, 'r%d, =0x%x' % (rn, rval)))
    else:
        if cond != '':
            elinst.append(('itttt', cond))
        imm_offset = random.choice([True, False])
        if ExclSize == 'w' and imm_offset:
            imm = ( random.randint(0x0, 0xff) << 2 )
            elinst.append(('ldr'+cond+wsize_suffix, 'r%d, =0x%x' % (rn, rval - imm)))
        else:
            elinst.append(('ldr'+cond+wsize_suffix, 'r%d, =0x%x' % (rn, rval)))

    if ExclSize == 'b':
        elinst.append(('ldrexb'+cond, 'r%d, [r%d]' % (rt, rn)))
    elif ExclSize == 'h':
        elinst.append(('ldrexh'+cond, 'r%d, [r%d]' % (rt, rn)))
    elif ExclSize == 'w':
        if isARM or not imm_offset:
            elinst.append(('ldrex'+cond, 'r%d, [r%d]' % (rt, rn)))
        else:
            elinst.append(('ldrex'+cond, 'r%d, [r%d, #%d]' % (rt, rn, imm)))
    elif ExclSize == 'd':
        if isARM:
            rt2 = rt + 1
        else:
            if rt in reglist:
                reglist.remove(rt)
            rt2 = random.choice(reglist)
        elinst.append(('ldrexd'+cond, 'r%d, r%d, [r%d]' % (rt, rt2, rn)))
    else:
        pass

    regtmp = range(13)
    rx = random.choice(regtmp)
    regtmp.remove(rx)
    ry = random.choice(regtmp)
    elinst.append(('ldr'+cond+wsize_suffix, 'r%d, =excladdr+0' % (rx)))
    elinst.append(('ldr'+cond+wsize_suffix, 'r%d, =0x%x' % (ry, rval)))

    if not isARM and cond != '':
        if ExclSize == 'd':
            elinst.append(('ittt', cond))
        else:
            elinst.append(('itt', cond))

    elinst.append(('str'+cond+wsize_suffix, 'r%d, [r%d]' % (ry, rx)))
    elinst.append(('ldr'+cond+wsize_suffix, 'r%d, =0x%x' % (rt, code.getRandRegVal(rt))))
    if ExclSize == 'd':
        elinst.append(('ldr'+cond+wsize_suffix, 'r%d, =0x%x' % (rt2, code.getRandRegVal(rt2))))

    return (elinst, ExclSize)

def genSTREX(isARM, ctx, isconditional, selfLabelPrefix, ExclSize=None, cond=None):
    esinst = []
    if ExclSize is None:
        esinst.append(('clrex', ''))
        return esinst
    if isconditional:
        if cond is None:
            cond = random.choice(arminfo.CONDCODE)
    else:
        cond = ''

    if isARM:
        wsize_suffix = ''
    else:
        wsize_suffix = '.w'

    reglist = range(15)
    if not isARM:
        reglist.remove(13)
    rs = random.choice(reglist)
    esinst.append(('mrs', 'r%d, APSR' % (rs)))
    reglist.remove(rs)
    rw = random.choice(reglist)
    reglist.remove(rw)
    rn = random.choice(reglist)
    reglist.remove(rn)
    rv = random.choice(reglist)

    esinst.append(('ldr'+wsize_suffix, 'r%d, =excladdr+0' % (rw)))
    esinst.append(('ldr'+wsize_suffix, 'r%d, [r%d]' % (rn, rw)))
    esinst.append(('cmp'+wsize_suffix, 'r%d, #0' % (rn)))
    esinst.append(('beq'+wsize_suffix, selfLabelPrefix+'_excl_stub'))
    esinst.append(('mov'+wsize_suffix, 'r%d, #0' % (rv)))
    esinst.append(('str'+wsize_suffix, 'r%d, [r%d]' % (rv, rw)))
    esinst.append(('msr', 'APSR_nzcvqg, r%d' % (rs)))


    imm_offset = random.choice([True, False])
    if not isARM and imm_offset and ExclSize == 'w':
            imm = ( random.randint(0x0, 0xff) << 2 )
            esinst.append(('sub'+wsize_suffix, 'r%d, r%d, #0x%x' % (rn, rn, imm)))

    reglist = range(15)
    rt = random.choice(reglist)
    if isARM:
        if ExclSize == 'd':
            regtmp = reglist
            regtmp.remove(14)
            regtmp = [i for i in regtmp if i&1==0]
            rt = random.choice(regtmp)
    else:
        reglist.remove(13)
        rt = random.choice(reglist)
    reglist.remove(rt)
    if isARM:
        rt2 = rt + 1
    else:
        rt2 = random.choice(reglist)
    if rt2 in reglist:
        reglist.remove(rt2)
    if rn in reglist:
        reglist.remove(rn)
    rd = random.choice(reglist)
    esinst.append(('mov'+wsize_suffix, 'r%d, #0' % (rd)))

    if cond != '':
        uncond = arminfo.INVCONDCODE[arminfo.CONDCODE.index(cond)]
        if not isARM:
            esinst.append(('ite', cond))

    if ExclSize == 'b':
        esinst.append(('strexb'+cond, 'r%d, r%d, [r%d]' % (rd, rt, rn)))
    elif ExclSize == 'h':
        esinst.append(('strexh'+cond, 'r%d, r%d, [r%d]' % (rd, rt, rn)))
    elif ExclSize == 'w':
        if isARM or not imm_offset:
            esinst.append(('strex'+cond, 'r%d, r%d, [r%d]' % (rd, rt, rn)))
        else:
            esinst.append(('strex'+cond, 'r%d, r%d, [r%d , #%d]' % (rd, rt, rn, imm)))
    elif ExclSize == 'd':
        esinst.append(('strexd'+cond, 'r%d, r%d, r%d, [r%d]' % (rd, rt, rt2, rn)))
    else:
        pass

    if cond != '':
        esinst.append(('b'+uncond, selfLabelPrefix+'_excl_fill_lock_mem'))
    esinst.append(('cmp'+wsize_suffix, 'r%d, #0' % (rd)))

    reglist = range(15)
    if rn in reglist:
        reglist.remove(rn)
    rz1 = random.choice(reglist)
    reglist.remove(rz1)
    rz2 = random.choice(reglist)
    reglist.remove(rz2)

    esinst.append(('ldr'+wsize_suffix, 'r%d, =exclcnt+0' % (rz1)))
    esinst.append(('ldr'+wsize_suffix, 'r%d, [r%d]' % (rz2, rz1)))
    esinst.append(('sub'+wsize_suffix, 'r%d, r%d, #0x%x' % (rz2, rz2, 1)))
    esinst.append(('str'+wsize_suffix, 'r%d, [r%d]' % (rz2, rz1)))

    if not isARM:
        esinst.append(('it', 'ne'))
    esinst.append(('cmp'+'ne'+wsize_suffix, 'r%d, #0' % (rz2)))

    esinst.append(('bne', selfLabelPrefix+'_excl_back_edge'))
    esinst.append((selfLabelPrefix+'_excl_fill_lock_mem', ':'))
    if not isARM and imm_offset and ExclSize == 'w':
        esinst.append(('add'+wsize_suffix, 'r%d, r%d, #0x%x' % (rn, rn, imm)))

    reg_lm = range(13)
    if rt in reg_lm:
        reg_lm.remove(rt)
    if (ExclSize == 'd') and (rt2 in reg_lm):
        reg_lm.remove(rt2)
    if not (rn in reg_lm):
        rm = random.choice(reg_lm)
        esinst.append(('mov'+wsize_suffix, 'r%d, r%d' % (rm, rn)))
        rn = rm
    esinst.append(('ldr'+wsize_suffix, 'r%d, =0x%x' % (rt, code.getRandRegVal(rt))))
    if ExclSize == 'd':
        esinst.append(('ldr'+wsize_suffix, 'r%d, =0x%x' % (rt2, code.getRandRegVal(rt2))))
        esinst.append(('strd'+wsize_suffix, 'r%d, r%d, [r%d, #0]' % (rt, rt2, rn)))
    else:
        esinst.append(('str'+wsize_suffix, 'r%d, [r%d]' % (rt, rn)))

    esinst.append((selfLabelPrefix+'_excl_stub', ':'))
    esinst.append(('ldr'+wsize_suffix, 'r%d, =0x%x' % (rs, code.getRandRegVal(rs))))
    esinst.append(('ldr'+wsize_suffix, 'r%d, =0x%x' % (rn, code.getRandRegVal(rn))))
    esinst.append(('mov'+wsize_suffix, 'r%d, #0' % (rd)))
    esinst.append(('cmp'+wsize_suffix, 'r%d, #0' % (rd)))
    esinst.append(('ldr'+wsize_suffix, 'r%d, =exclcnt+0' % (rz1)))
    esinst.append(('ldr'+wsize_suffix, 'r%d, =0x%x' % (rz2, arminfo.EXCL_CNT)))
    esinst.append(('str'+wsize_suffix, 'r%d, [r%d]' % (rz2, rz1)))
    return esinst

def genALUwritePC (ctx, label, allow_fix=True):
    isARM = ctx[options.CTX.ISA]
    cond = ''
    inst = []
    hexcode = random.randint(0, 2**32-1)
    seed = random.randint(0, 2**32-1)
    reg_list = range(16)
    dst_reg = arminfo.REGType.R_PC
    tmp_dst = arminfo.REGType.R_LR
    reg_list.remove(dst_reg)
    reg_list.remove(tmp_dst)
    reg_list.remove(arminfo.REGType.R_SP)
    rn = random.choice(reg_list)
    rm = random.choice(reg_list)
    if rn in reg_list:
        reg_list.remove(rn)
    if rm in reg_list:
        reg_list.remove(rm)
    if arminfo.REGType.R_SP in reg_list:
        reg_list.remove(arminfo.REGType.R_SP)
    ri = random.choice(reg_list)
    reg_list.remove(ri)
    rf = random.choice(reg_list)

    if random.choice([True, False]):
        cond = random.choice(arminfo.CONDCODE);
        rcond = arminfo.CONDVALUE[arminfo.CONDCODE.index(cond)]
        uncond = arminfo.INVCONDCODE[arminfo.CONDCODE.index(cond)]
        inst.append(('b'+uncond, label+'_wPC_inst_pre'))
    else:
        cond = ''
        rcond = 14

    inst.append(('mrs', 'r%d, APSR' % (rf)))
    if isARM:
        wsize_suffix = ''
        inst.append(('ldr'+wsize_suffix, 'r%d, =%s+8' % (tmp_dst, label+'_wPC_inst_pre')))
        opcode = random.choice(arminfo.ARM_ALU_W_PC_ALL)
        ret = ctx['rit'].gen_single_inst_with_opcode(isARM, opcode, rcond, 0, None, tmp_dst, allow_fix, seed, hexcode, True)
        if ret:
            inst_hex = ret.pop()[-1]
            #print inst_hex
            inst_hex = inst_hex[:-6] + arminfo.Sbit_Msk[arminfo.Sbit_Val.index(inst_hex[-6])]  + inst_hex[-5:]
            #print inst_hex
            if opcode in arminfo.ARM_ALU_W_PC_sreg_4_0:
                inst.append(('.inst', '%s%x%s%x' % (inst_hex[:-5],rn,inst_hex[-4:-1],rm)))
            elif opcode in arminfo.ARM_ALU_W_PC_sreg_4:
                inst.append(('.inst', '%s%x%s' % (inst_hex[:-5],rn,inst_hex[-4:])))
            elif opcode in arminfo.ARM_ALU_W_PC_sreg_0:
                inst.append(('.inst', '%s%x' % (inst_hex[:-1],rn)))
            else:
                inst.append(('.inst', inst_hex))
                if (opcode is 'ADR_a1') or (opcode is 'ADR_a2'):
                    inst.append((label+'_wPC_ADR_offset', ':'))
                    inst.append(('add', 'r%d, r%d, #%s-%s+4' % (tmp_dst, tmp_dst, label+'_wPC_inst_pre', label+'_wPC_ADR_offset')))
        else:
            pass
    else:
        wsize_suffix = '.w'
        if cond != '':
            inst.append(('ldr'+wsize_suffix, 'r%d, =%s+6' % (tmp_dst, label+'_wPC_inst_pre')))
        else:
            inst.append(('ldr'+wsize_suffix, 'r%d, =%s+4' % (tmp_dst, label+'_wPC_inst_pre')))
        opcode = random.choice(arminfo.THUMB_ALU_W_PC)
        if cond != '':
            inst.append(('it', cond))
        if opcode is 'ADD_reg_T2':
            inst.append(('add'+cond,'r%d, r%d' % (tmp_dst, rn)));
        elif opcode is 'MOV_reg_T1':
            inst.append(('mov'+cond,'r%d, r%d' % (tmp_dst, rn)));
        else:
            pass

    inst.append(('ldr'+wsize_suffix, 'r%d, =%s' % (ri,arminfo.ALU_W_PC_VALID_BEG_ADDR)))
    inst.append(('subs'+wsize_suffix, 'r%d, r%d, r%d' % (tmp_dst, tmp_dst, ri)))
    inst.append(('blt', label+'_wPC_inst_post'))
    inst.append(('ldr'+wsize_suffix, 'r%d, =%s-8' % (ri,arminfo.ALU_W_PC_VALID_SPACE_SIZE)))
    inst.append(('cmp'+wsize_suffix, 'r%d, r%d' % (tmp_dst, ri)))
    inst.append(('bgt', label+'_wPC_inst_post'))
    inst.append(('ldr'+wsize_suffix, 'r%d, =%s' % (ri,arminfo.ALU_W_PC_VALID_BEG_ADDR)))
    inst.append(('add'+wsize_suffix, 'r%d, r%d, r%d' % (tmp_dst, tmp_dst, ri)))

    if isARM:
        inst.append(('ands', 'r%d, r%d, #0x1' % (ri, tmp_dst)))
        inst.append(('beq', label+'_wPC_ARM_spring'))

    inst.append((label+'_wPC_THUMB_spring', ':'))
    inst.append(('ldr'+wsize_suffix, 'r%d, =0xfffffffe' % (ri)))
    inst.append(('and'+wsize_suffix, 'r%d, r%d, r%d' % (tmp_dst, tmp_dst, ri)))
    inst.append(('ldr'+wsize_suffix, 'r%d, =0x%x' % (ri, arminfo.THUMB_PC_RET)))

    if isARM:
        inst.append(('b', label+'_wPC_st_spring'))
        inst.append((label+'_wPC_ARM_spring', ':'))
        inst.append(('ands', 'r%d, r%d, #0x2' % (ri, tmp_dst)))
        inst.append(('bne', label+'_wPC_inst_post'))
        inst.append(('ldr'+wsize_suffix, 'r%d, =0xfffffffc' % (ri)))
        inst.append(('and'+wsize_suffix, 'r%d, r%d, r%d' % (tmp_dst, tmp_dst, ri)))
        inst.append(('ldr'+wsize_suffix, 'r%d, =0x%x' % (ri, arminfo.ARM_PC_RET)))

    inst.append((label+'_wPC_st_spring', ':'))
    inst.append(('str'+wsize_suffix, 'r%d, [r%d]' % (ri, tmp_dst)))

    if not isARM:
        inst.append(('ldr'+wsize_suffix, 'r%d, =%s+1' % (tmp_dst, label+'_wPC_inst_post')))
    else:
        inst.append(('ldr'+wsize_suffix, 'r%d, =%s+0' % (tmp_dst, label+'_wPC_inst_post')))
    inst.append(('msr', 'APSR_nzcvqg, r%d' % (rf)))
    inst.append(('isb', ''))

    inst.append((label+'_wPC_inst_pre', ':'))
    if isARM:
        ret = ctx['rit'].gen_single_inst_with_opcode(isARM, opcode, rcond, 0, None, dst_reg, allow_fix, seed, hexcode, True)
        if ret:
            inst_hex = ret.pop()[-1]
            inst_hex = inst_hex[:-6] + arminfo.Sbit_Msk[arminfo.Sbit_Val.index(inst_hex[-6])]  + inst_hex[-5:]
            if opcode in arminfo.ARM_ALU_W_PC_sreg_4_0:
                inst.append(('.inst','%s%x%s%x' % (inst_hex[:-5],rn,inst_hex[-4:-1],rm)))
            elif opcode in arminfo.ARM_ALU_W_PC_sreg_4:
                inst.append(('.inst','%s%x%s' % (inst_hex[:-5],rn,inst_hex[-4:])))
            elif opcode in arminfo.ARM_ALU_W_PC_sreg_0:
                inst.append(('.inst','%s%x' % (inst_hex[:-1],rn)))
            else:
                inst.append(('.inst',inst_hex))
        else:
            pass
    else:
        if cond != '':
            inst.append(('it', cond))
        if opcode is 'ADD_reg_T2':
            inst.append(('add'+cond,'PC, r%d' % (rn)));
        elif opcode is 'MOV_reg_T1':
            inst.append(('mov'+cond,'PC, r%d' % (rn)));
        else:
            pass

    inst.append(('.align', ''))
    inst.append((label+'_wPC_inst_post', ':'))
    return inst

def getPreCalleeInsts(ctx):
    tmp_reg = random.randint(0, 7)
    preinst = [('ldr', 'r%d, =cfgdata+%d' % (tmp_reg, ctx['nest']*4+4)),
               ('mov', 'sp, r%d' % tmp_reg),
               ('push', '{lr}')]
    return preinst

def getPostCalleeInsts(ctx):
    tmp_reg = random.randint(0, 7)
    postinst = [('ldr', 'r%d, =cfgdata+%d' % (tmp_reg, ctx['nest']*4)),
                ('mov', 'sp, r%d' % tmp_reg),
                ('pop', '{pc}'),
                ('.ltorg', '')]
    return postinst

def reinitGeneralRegs(isARM, label, limit):
    base_reg = random.randint(0, 12)
    if limit < 16:
        offset = 0
    else:
        offset = random.randint(0, (limit-16))*4
    insts = [('ldr', 'r%d, =%s+%d' % (base_reg, label, offset))]
    regs = range(15)
    if not isARM:
        regs.remove(13)
    n = random.randint(len(regs)/2, len(regs))
    rlist = random.sample(regs, n)
    rlist.sort()
    str_rlist = ', '.join(['r'+str(i) for i in rlist])
    if isARM:
        insts.append(('ldm', 'r%d, {%s}' % (base_reg, str_rlist)))
    else:
        insts.append(('ldm.w', 'r%d, {%s}' % (base_reg, str_rlist)))
    #print insts
    return insts

def getMemLimit():
    return (0x03fc0000, 0x03ffffff)

def test():
    options.parseOptions()
    ctx = options.getInitContext(rb.getRBlock)
    ctx[options.CTX.ISA] = False
    ropts = {}
    inst = genThumbIT(None, ropts, ctx)
    print inst


if __name__ == '__main__':
    test()
