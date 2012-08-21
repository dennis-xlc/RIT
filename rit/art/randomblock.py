#!/usr/bin/python

import random
import code
from code import CODETYPE as CT
import instrgen as ig
import options
from options import CTX

class RBType():
    Basic       = 0
    ByPass      = 1
    Hammock     = 2
    Loop        = 3
    IrLoop      = 4
    Interwork   = 5
    CallRet     = 6
    Exclusive   = 7
    ALUwritePC  = 8


class BasicRBlock(code.RBlock):

    def makeRandomBlock(self, ctx):
        super(BasicRBlock, self).makeRandomBlock(ctx)
        self.isARM = ctx[CTX.ISA]
        ctx[CTX.BB][self.label] = {}
        return [self]
        #instrs = ig.genRandomInstr(ctx)
        #self.code.append((CT.INST, instrs))
        #r = random.randint(0, 100)
        #if r < 30:
        #    self.code.append((code.CODETYPE.INST, ig.genALUwritePC(ctx, self.label)))
        #if not ctx[CTX.ISA]:
        #    rval = random.randint(0, 100)
        #    if rval < options.getOption('per_tb'):
        #        if not ctx['tb']:
        #            ctx['tb'] = True
        #            tb = ThumbTBRBlock(self.label+'_tb')
        #            tb.makeRandomBlock(ctx)
        #            self.code.append((CT.BLOCK, tb))
        #            ctx['tb'] = False


class ByPassRBlock(code.RBlock):
    """ByPass Block
    b1: xxx
        xxx
        b<c> b3
    b2: xxx
        xxx
    b3: xxx
        xxx
    """

    def makeRandomBlock(self, ctx):
        super(ByPassRBlock, self).makeRandomBlock(ctx)
        nest = ctx['nest'] - 1
        b1 = ctx['getRBlock'](self.label+'_b1', nest)
        b2 = ctx['getRBlock'](self.label+'_b2', nest)
        b3 = ctx['getRBlock'](self.label+'_b3', nest)
        ctx['nest'] = nest
        sub1 = b1.makeRandomBlock(ctx)
        sub2 = b2.makeRandomBlock(ctx)
        sub3 = b3.makeRandomBlock(ctx)
        b_b3 = ig.genBranch(ctx[CTX.ISA], ctx, b3.label, True, forward=True, off_block=b2)

        self.code.append((CT.BLOCK, b1))
        self.code.append((CT.INST, b_b3))
        self.code.append((CT.BLOCK, b2))
        self.code.append((CT.BLOCK, b3))
        ctx['nest'] = nest + 1
        return sub1+sub2+sub3 


class HammockRBlock(code.RBlock):
    """Hammock Block
    h1: xxx
        xxx
        b<c> h3
    h2: xxx
        xxx
        b h4
    h3: xxx
        xxx
    h4: xxx
        xxx
    """
    def makeRandomBlock(self, ctx):
        blocks = []
        super(HammockRBlock, self).makeRandomBlock(ctx)
        nest = ctx['nest'] - 1
        h1 = ctx['getRBlock'](self.label+'_h1', nest)
        h2 = ctx['getRBlock'](self.label+'_h2', nest)
        h3 = ctx['getRBlock'](self.label+'_h3', nest)
        h4 = ctx['getRBlock'](self.label+'_h4', nest)
        ctx['nest'] = nest
        sub1 = h1.makeRandomBlock(ctx)
        blocks += sub1
        sub2 = h2.makeRandomBlock(ctx)
        sub3 = h3.makeRandomBlock(ctx)
        sub4 = h4.makeRandomBlock(ctx)
        b_h3 = ig.genBranch(ctx[CTX.ISA], ctx, h3.label, True, forward=True, off_block=h2)
        b_h4 = ig.genBranch(ctx[CTX.ISA], ctx, h4.label, False, forward=True, off_block=h3)
        blocks.append((sub2, sub3))
        blocks += sub4

        self.code.append((CT.BLOCK, h1))
        self.code.append((CT.INST, b_h3))
        self.code.append((CT.BLOCK, h2))
        self.code.append((CT.INST, b_h4))
        self.code.append((CT.BLOCK, h3))
        self.code.append((CT.BLOCK, h4))
        ctx['nest'] = nest + 1
        return blocks


class LoopRBlock(code.RBlock):
    """Loop
    @loop body
    l1: xxx
        xxx
    @loop condition
        b<c> l1
    """
    def makeRandomBlock(self, ctx):
        super(LoopRBlock, self).makeRandomBlock(ctx)
        nest = ctx['nest'] - 1
        l1 = ctx['getRBlock'](self.label+'_l1', nest)
        ctx['nest'] = nest
        sub1 = l1.makeRandomBlock(ctx)
        b_l1 = ig.genLoop(ctx[CTX.ISA], ctx, l1.label)

        self.code.append((CT.BLOCK, l1))
        self.code.append((CT.INST, b_l1))
        ctx['nest'] = nest + 1
        return sub1


class IrreducibleLoopRBlock(code.RBlock):
    """Irreducible Loop
    ir1: xxx
         xxx
         b<c> ir3
    @loop body (ir2 + ir3)
    ir2: xxx
         xxx
    ir3: xxx
         xxx
    @loop condition
         b<c> ir2
    """
    def makeRandomBlock(self, ctx):
        super(IrreducibleLoopRBlock, self).makeRandomBlock(ctx)
        nest = ctx['nest'] - 1
        ir1 = ctx['getRBlock'](self.label+'_ir1', nest)
        ir2 = ctx['getRBlock'](self.label+'_ir2', nest)
        ir3 = ctx['getRBlock'](self.label+'_ir3', nest)
        ctx['nest'] = nest
        sub1 = ir1.makeRandomBlock(ctx)
        sub2 = ir2.makeRandomBlock(ctx)
        sub3 = ir3.makeRandomBlock(ctx)
        b_ir2 = ig.genLoop(ctx[CTX.ISA], ctx, ir2.label)
        b_ir3 = ig.genBranch(ctx[CTX.ISA], ctx, ir3.label, True, off_block=ir2)

        self.code.append((CT.BLOCK, ir1))
        self.code.append((CT.INST, b_ir3))
        self.code.append((CT.BLOCK, ir2))
        self.code.append((CT.BLOCK, ir3))
        self.code.append((CT.INST, b_ir2))
        ctx['nest'] = nest + 1
        return sub1 + sub2 + sub3


class InterworkRBlock(code.RBlock):
    """
    iw1: xxx
         xxx
    iw2: xxx
         xxx
    iw3: xxx
         xxx
    """
    def makeRandomBlock(self, ctx):
        super(InterworkRBlock, self).makeRandomBlock(ctx)
        nest = ctx['nest'] - 1
        iw1 = ctx['getRBlock'](self.label+'_iw1', nest)
        iw2 = ctx['getRBlock'](self.label+'_iw2', nest)
        iw3 = ctx['getRBlock'](self.label+'_iw3', nest)
        ctx['nest'] = nest
        sub1 = iw1.makeRandomBlock(ctx)
        ctx[CTX.ISA] = not ctx[CTX.ISA]
        sub2 = iw2.makeRandomBlock(ctx)
        ctx[CTX.ISA] = not ctx[CTX.ISA]
        sub3 = iw3.makeRandomBlock(ctx)
        
        cond_bx = random.choice([True, False])
        iw_followed = random.choice([True, False])
        in_isa = ctx[CTX.ISA]
        bx_1 = ig.genBX(in_isa, ctx, iw2.label, cond_bx)
        bx_2 = ig.genBX(not in_isa, ctx, iw1.label, cond_bx)
        bx_3 = ig.genBX(not in_isa, ctx, iw3.label, False)
        b = ig.genBranch(in_isa, ctx, iw3.label, False)
        ctx['nest'] = nest + 1
        self.code.append((CT.ATTR, code.ISA_A if in_isa else code.ISA_T))
        if iw_followed:
            self.code.append((CT.INST, bx_1))
            self.code.append((CT.INST, b))
            self.code.append((CT.BLOCK, iw2))
            self.code.append((CT.INST, bx_2))
            self.code.append((CT.INST, bx_3))
            self.code.append((CT.BLOCK, iw1))
            self.code.append((CT.BLOCK, iw3))
            return sub2 + sub1 + sub3
        else:
            self.code.append((CT.INST, bx_1))
            self.code.append((CT.BLOCK, iw1))
            self.code.append((CT.INST, b))
            self.code.append((CT.BLOCK, iw2))
            self.code.append((CT.INST, bx_2))
            self.code.append((CT.INST, bx_3))
            self.code.append((CT.BLOCK, iw3))
            return sub1 + sub2 + sub3


class CallRetRBlock(code.RBlock):
    """
    c1: xxx
        xxx
    c2: xxx
        xxx
    c3: xxx
        xxx
    """
    def makeRandomBlock(self, ctx):
        super(CallRetRBlock, self).makeRandomBlock(ctx)
        nest = ctx['nest'] - 1
        c1 = ctx['getRBlock'](self.label+'_c1', nest)
        c2 = ctx['getRBlock'](self.label+'_c2', nest)
        c3 = ctx['getRBlock'](self.label+'_c3', nest)
        ctx['nest'] = nest
        iwcall = random.choice([True, False])
        callee_followed = random.choice([True, False])
        begin_with_call = random.choice([True, False])
        caller_isa = ctx[CTX.ISA]
        if iwcall:
            callee_isa = not caller_isa
        else:
            callee_isa = caller_isa
        call = ig.genCall(caller_isa, callee_isa, ctx, c2.label)
        sub1 = c1.makeRandomBlock(ctx)
        if iwcall:
            ctx[CTX.ISA] = not ctx[CTX.ISA]
        pre_call = ig.getPreCalleeInsts(ctx)
        post_call = ig.getPostCalleeInsts(ctx)
        c2.code.append((CT.INST, pre_call))
        sub2 = c2.makeRandomBlock(ctx)
        if iwcall:
            ctx[CTX.ISA] = not ctx[CTX.ISA]
        sub3 = c3.makeRandomBlock(ctx)
        ctx['nest'] = nest + 1

        self.code.append((CT.ATTR, code.ISA_A if caller_isa else code.ISA_T))
        if callee_followed:
            b = ig.genBranch(caller_isa, ctx, c1.label, False)
            if begin_with_call:
                self.code.append((CT.INST, call))
            self.code.append((CT.INST, b))
            self.code.append((CT.BLOCK, c2))
            self.code.append((CT.INST, post_call))
            self.code.append((CT.BLOCK, c1))
            if not begin_with_call:
                self.code.append((CT.INST, call))
            self.code.append((CT.BLOCK, c3))
            return sub2 + sub1 + sub3
        else:
            b = ig.genBranch(caller_isa, ctx, c3.label, False)
            if begin_with_call:
                self.code.append((CT.INST, call))
            self.code.append((CT.BLOCK, c1))
            if not begin_with_call:
                self.code.append((CT.INST, call))
            self.code.append((CT.INST, b))
            self.code.append((CT.BLOCK, c2))
            self.code.append((CT.INST, post_call))
            self.code.append((CT.BLOCK, c3))
            return sub1 + sub2 + sub3


class ThumbTBRBlock(code.RBlock):

    def makeRandomBlock(self, ctx):
        super(ThumbTBRBlock, self).makeRandomBlock(ctx)
        if ctx[CTX.ISA]:
            return
        
        blocks = []
        tb_max = options.getOption('tb_max')
        tb_min = options.getOption('tb_min')
        items = random.randint(tb_min, tb_max)
        table_label = self.label + '_table'
        tb_inst = []     
        is_tbh = random.choice([True, False])

        #init Rn and Rm
        regs = range(16 + 16) #increase freq of Rn == 15
        regs.remove(13)
        Rn = random.choice(regs)
        if Rn > 15: 
            Rn = 15
        regs = range(14)
        regs.remove(13)
        if Rn in regs:
            regs.remove(Rn)
        Rm = random.choice(regs)

        tb_inst += ig.setReg(Rm, 0, items-1, False)
        if not Rn == 15:
            tb_inst.append(('ldr.w', 'r%d, =%s' % (Rn, table_label)))
        if is_tbh:
            tb_inst.append(('tbh', '[r%d, r%d, lsl #1]' % (Rn, Rm)))
            index_type = '.hword'
        else:
            tb_inst.append(('tbb', '[r%d, r%d]' % (Rn, Rm)))
            index_type = '.byte'
        self.code.append((CT.INST, tb_inst))

        case_code = []
        nest = ctx['nest'] - 3
        ctx['nest'] = nest
        for i in range(items):
            case_block = ctx['getRBlock']("%s_case%d" % (self.label, i), nest)
            sub_block = case_block.makeRandomBlock(ctx)
            blocks += sub_block
            b_end = ig.genBranch(False, ctx, self.label_end, False)
            case_code.append((CT.BLOCK, case_block))
            case_code.append((CT.INST, b_end))

        if Rn == 15:
            self.code.append((CT.LABEL, table_label))
            for i in range(items):
                self.code.append((CT.INST,
                    [(index_type, "((%s_case%d - %s)/2)" % (self.label, i, table_label))]))
            self.code += case_code
        else:
            self.code += case_code
            self.code.append((CT.LABEL, table_label))
            for i in range(items):
                self.code.append((CT.INST,
                    [(index_type, "((%s_case%d - %s_case0)/2)" % (self.label, i,
                                                                  self.label))]))
        ctx['nest'] = nest + 2
        return blocks

class ExclsiveRBlock(code.RBlock):

    def makeRandomBlock(self, ctx):
        super(ExclsiveRBlock, self).makeRandomBlock(ctx)
        condcode = random.choice([True, False])
        nest = ctx['nest'] - 1
        e1 = ctx['getRBlock'](self.label+'_e1', nest)
        e2 = ctx['getRBlock'](self.label+'_e2', nest)
        ctx['nest'] = nest
        sub1 = e1.makeRandomBlock(ctx)
        e_l, ExclSize = ig.genLDREX(ctx[CTX.ISA], ctx, condcode, self.label)
        e_s = ig.genSTREX(ctx[CTX.ISA], ctx, condcode, self.label, ExclSize)
        sub2 = e2.makeRandomBlock(ctx)

        self.code.append((CT.BLOCK, e1))
        self.code.append((CT.INST, e_l))
        self.code.append((CT.INST, e_s))
        self.code.append((CT.BLOCK, e2))
        ctx['nest'] = nest + 1
        return sub1 + sub2

class ALUwritePCRBlock(code.RBlock):
    def makeRandomBlock(self, ctx):
        super(ALUwritePCRBlock, self).makeRandomBlock(ctx)
        nest = ctx['nest'] - 1
        preB = ctx['getRBlock'](self.label+'_aluwpc1', nest)
        postB = ctx['getRBlock'](self.label+'_aluwpc2', nest)
        ctx['nest'] = nest
        sub1 = preB.makeRandomBlock(ctx)
        aluwpcB = ig.genALUwritePC(ctx, self.label)
        sub2 = postB.makeRandomBlock(ctx)

        self.code.append((CT.BLOCK, preB))
        self.code.append((CT.INST, aluwpcB))
        self.code.append((CT.BLOCK, postB))
        ctx['nest'] = nest + 1
        return sub1 + sub2

def getRBlock(label, nest):
    if nest > 0:
        rb = [RBType.Basic, RBType.ByPass, RBType.Hammock, RBType.Loop, 
              RBType.IrLoop, RBType.Interwork, RBType.CallRet]
        if options.getOption('excl_block'):
            rb.append(RBType.Exclusive)
        if options.getOption('aluwpc_block'):
            rb.append(RBType.ALUwritePC)
        if nest > 3:
            rb.remove(RBType.Basic)
            if RBType.Exclusive in rb:
                rb.remove(RBType.Exclusive)
        rval = random.choice(rb)
        if rval is RBType.ByPass:
            return ByPassRBlock(label)
        elif rval is RBType.Hammock:
            return HammockRBlock(label)
        elif rval is RBType.Loop:
            return LoopRBlock(label)
        elif rval is RBType.IrLoop:
            return IrreducibleLoopRBlock(label)
        elif rval is RBType.Interwork:
            return InterworkRBlock(label)
        elif rval is RBType.CallRet:
            return CallRetRBlock(label)
        elif rval is RBType.Exclusive:
            r = random.randint(0, 100)
            if r < 30:
                return ExclsiveRBlock(label)
            else:
                return BasicRBlock(label)
        elif rval is RBType.ALUwritePC:
            r = random.randint(0, 100)
            if r < 30:
                return ALUwritePCRBlock(label)
            else:
                return BasicRBlock(label)
        else:
            return BasicRBlock(label)
    else:
        return BasicRBlock(label)

def test():
    options.parseOptions()
    ctx = options.getInitContext(getRBlock)
    test = getRBlock('rit', 5)
    blocks = test.makeRandomBlock(ctx)
    ig.genBlockInstrs(blocks, ctx)
    print test
    print blocks, len(blocks)
    #print test


if __name__ == '__main__':
    test()
