#!/usr/bin/python

import neon_inst
import random

NaN_OP = ('VABD', 'VADD', 'VMAX', 'VMLA', 'VMUL', 'VPADD', 
          'VPMAX', 'VRECPE', 'VRECPS', 'VRSQRTE', 'VRSQRTS', 'VSUB')

def gen_data_inst(ridx, isARM=True):
    #modified imm
    #scalar
    inst = neon_inst.NEON_INST[ridx%len(neon_inst.NEON_INST)]
    dict = {}
    str_inst = inst['fmt']
    if 'dt' in inst['opt']:
        dt = random.choice(inst['opt']['dt'])
        dict['<dt>'] = dt
    else:
        dt = ''
    if 'size' in inst['opt']:
        dict['<size>'] = str(random.choice(inst['opt']['size']))
    dict['<Qd>'] = 'Q%d' % random.randint(0, 15)
    dict['<Qm>'] = 'Q%d' % random.randint(0, 15)
    dict['<Qn>'] = 'Q%d' % random.randint(0, 15)
    dict['<Dd>'] = 'D%d' % random.randint(0, 31)
    if str_inst.startswith('VSHLL'):
        dict['<Dm>'] = 'D%d' % (random.randint(0, 15)*2)
    else:
        dict['<Dm>'] = 'D%d' % random.randint(0, 31)
    dict['<Dn>'] = 'D%d' % random.randint(0, 31)
    dict['<Sd>'] = 'S%d' % random.randint(0, 31)
    dict['<Sn>'] = 'S%d' % random.randint(0, 31)
    if '<Sm1>' in str_inst:
        rval = random.randint(0, 30)
        dict['<Sm>'] = 'S%d' % rval
        dict['<Sm1>'] = 'S%d' % (rval + 1)
    else:
        dict['<Sm>'] = 'S%d' % random.randint(0, 31)
    #ARM core register
    regs = range(0, 15)
    if not isARM:
        regs.remove(13)
    rreg = random.choice(regs)
    dict['<Rt>'] = 'R%d' % rreg
    if '<Rt2>' in str_inst:
        dict['<Rt2>'] = 'R%d' % random.choice(regs)
    if '<Rt2@D>' in str_inst:
        regs.remove(rreg)
        dict['<Rt2@D>'] = 'R%d' % random.choice(regs)
       
    #<imm>
    for i in range(1, 7):
        if str_inst.startswith('VSHLL'):
            rimm = random.randint(1, 2**i-1)
        else:
            rimm = random.randint(0, 2**i-1)
        dict['<imm%d>' % i] = str(rimm)
    #VBIC/VMOV/VMVN/VORR
    if '<mimm>' in str_inst:
        mimm = random.randint(0, 0xff)
        if dt == 'I32':
            #cmode  constant
            #000x   00000000 00000000 00000000 abcdefgh
            #001x   00000000 00000000 abcdefgh 00000000
            #010x   00000000 abcdefgh 00000000 00000000
            #011x   abcdefgh 00000000 00000000 00000000
            cmode = random.randint(0, 3)
            if cmode is 0:
                str_imm = '0x%x' % mimm
            elif cmode is 1:
                str_imm = '0x%x00' % mimm
            elif cmode is 2:
                str_imm = '0x%x0000' % mimm
            else:
                str_imm = '0x%x000000' % mimm
            if str_inst.startswith('VMOV') or str_inst.startswith('VMVN'):
                #cmode  constant
                #1100   00000000 00000000 abcdefgh 11111111
                #1101   00000000 abcdefgh 11111111 11111111
                if not mimm is 0:
                    cmode = random.randint(0, 5)
                    if cmode is 4:
                        str_imm = '0x%xff' % mimm
                    elif cmode is 5:
                        str_imm = '0x%xffff' % mimm
        elif dt == 'I16':
            #cmode  constant
            #100x   00000000 abcdefgh 00000000 abcdefgh
            #101x   abcdefgh 00000000 abcdefgh 00000000
            cmode_100 = random.choice([True, False])
            if cmode_100:
                str_imm = '0x%x' % mimm
            else:
                str_imm = '0x%x00' % mimm
        elif dt == 'I64':
            str_imm = '0x'
            for i in range(8):
                str_imm += random.choice(['00', 'ff'])
        elif dt == 'F32':
            exp = random.randint(-3, 4)
            efgh = random.randint(0, 15)
            a = random.choice([1.0, -1.0])
            str_imm = str(a*(2**exp)*(16+efgh)/16)
        else:
            str_imm = '0x%x' % mimm
        dict['<mimm>'] = str_imm
    #<Dlist>
    if '<Dlist>' in str_inst:
        length = random.randint(1, 4)
        d = random.randint(0, 32-length)
        list = []
        for i in range(length):
            list.append('D%d' % (d+i))
        dict['<Dlist>'] = '{%s}' % ','.join(list)

    for k, v in dict.iteritems():
        str_inst = str_inst.replace(k, v)
    #str_inst = str_inst.replace('<dt>', dt)
    #str_inst = str_inst.replace('<size>', str(size))
    #print str_inst.replace('<c>', '')
    #print r_inst
    idx = str_inst.find(' ')
    if idx > 0:
        inst = (str_inst[:idx], str_inst[idx+1:])
    else:
        inst = None
    return (inst, None, None)

def gen_mem_inst(isARM=True):
    isLoad = random.choice([True, False])
    if isLoad:
        op = 'VLD'
        type = random.randint(1, 3)
    else:
        op = 'VST'
        type = random.randint(1, 2)
    e = random.randint(1, 4)
    size = random.choice([8, 16, 32])
    align = 0
    list = []
    if type is 1:
        #multiple elements
        aligns = [0, 64, 128, 256]
        if e is 1:
            regs = random.randint(1, 4)
            d = random.randint(0, 32-regs)
            for i in range(regs):
                list.append('D%d' % (d+i))
            if regs is 1 or regs is 3:
                aligns.remove(128)
                aligns.remove(256)
            elif regs is 2:
                aligns.remove(256)
            else:
                pass
            align = random.choice(aligns)
        elif e is 2:
            regs = random.randint(2, 4)
            d = random.randint(0, 32-regs)
            if regs is 2:
                aligns.remove(256)
                list.append('D%d' % d)
                list.append('D%d' % (d+1))
            elif regs is 3:
                aligns.remove(256)
                list.append('D%d' % d)
                list.append('D%d' % (d+2))
            elif regs is 4:
                for i in range(4):
                    list.append('D%d' % (d+i))
            else:
                #Error
                pass
            align = random.choice(aligns)
        elif e is 3:
            step = random.choice([1, 2])
            d = random.randint(0, 31-2*step)
            for i in range(3):
                list.append('D%d' % (d+step*i))
            align = random.choice([0, 64])
        elif e is 4:
            step = random.choice([1, 2])
            d = random.randint(0, 31-3*step)
            for i in range(4):
                list.append('D%d' % (d+step*i))
            align = random.choice(aligns)
        else:
            #Error
            pass
    elif type is 2:
        #to one lane
        if e is 1:
            d = random.randint(0, 31)
            if size is 8:
                align = 0
                index = random.randint(0, 7)
            elif size is 16:
                align = random.choice([0, 16])
                index = random.randint(0, 3)
            elif size is 32:
                align = random.choice([0, 32])
                index = random.randint(0, 1)
            else:
                #Error
                pass
            list.append('D%d[%d]' % (d, index))
        elif e is 2:
            if size is 8:
                align = random.choice([0, 16])
                index = random.randint(0, 7)
                step = 1
            elif size is 16:
                align = random.choice([0, 32])
                index = random.randint(0, 3)
                step = random.choice([1, 2])
            elif size is 32:
                align = random.choice([0, 64])
                index = random.randint(0, 1)
                step = random.choice([1, 2])
            else:
                #Error
                pass
            d = random.randint(0, 31-step)
            list.append('D%d[%d]' % (d, index))
            list.append('D%d[%d]' % (d+step, index))
        elif e is 3:
            align = 0
            if size is 8:
                index = random.randint(0, 7)
                step = 1
            elif size is 16:
                index = random.randint(0, 3)
                step = random.choice([1, 2])
            elif size is 32:
                index = random.randint(0, 1)
                step = random.choice([1, 2])
            else:
                #Error
                pass
            d = random.randint(0, 31-step*2)
            for i in range(3):
                list.append('D%d[%d]' % (d+step*i, index))
        elif e is 4:
            if size is 8:
                index = random.randint(0, 7)   
                step = 1
                align = random.choice([0, 32])
            elif size is 16:
                index = random.randint(0, 3)
                step = random.choice([1, 2])
                align = random.choice([0, 64])
            elif size is 32:
                index = random.randint(0, 1)
                step = random.choice([1, 2])
                align = random.choice([0, 64, 128])
            else:
                #Error
                pass
            d = random.randint(0, 31-step*3)
            for i in range(4):
                list.append('D%d[%d]' % (d+step*i, index))
        else:
            #Error
            pass
    elif type is 3:
        #to all lanes
        if e is 1:
            regs = random.choice([1, 2])
            if size is 8:
                align = 0
            elif size is 16:
                align = random.choice([0, 16])
            elif size is 32:
                align = random.choice([0, 32])
            else:
                #Error
                pass
            d = random.randint(0, 32-regs)
            for i in range(regs):
                list.append('D%d[]' % (d+i))
        elif e is 2:
            step = random.choice([1, 2])
            align = random.choice([0, size*2])
            d = random.randint(0, 31-step)
            for i in range(2):
                list.append('D%d[]' % (d+step*i))
        elif e is 3:
            step = random.choice([1, 2])
            align = 0
            d = random.randint(0, 31-step*2)
            for i in range(3):
                list.append('D%d[]' % (d+step*i))
        elif e is 4:
            step = random.choice([1, 2])
            if size is 8:
                align = random.choice([0, 32])
            elif size is 16:
                align = random.choice([0, 64])
            elif size is 32:
                align = random.choice([0, 64, 128])
            else:
                #Error
                pass
            d = random.randint(0, 31-step*3)
            for i in range(4):
                list.append('D%d[]' % (d+step*i))
        else:
            #Error
            pass
    else:
        #Error
        pass
    n = random.randint(0, 14)
    m = random.randint(0, 15)
    str_list = '{%s}' % ','.join(list)
    str_op = "%s%d.%d" % (op, e, size)
    if align:
        str_base = "[R%d :%d]" % (n, align)
    else:
        str_base = "[R%d]" % n
    addr_mode = random.randint(0, 2)
    if addr_mode is 0:
        m = 13
    elif addr_mode is 1:
        m = 15
    else:
        pass
    if m == 13:
        str_base += "!"
    elif m == 15:
        pass
    else:
        str_base += ", R%d" % m
    #print "%s %s, %s" % (str_op, str_list, str_base)
    inst = (str_op, "%s, %s" % (str_list, str_base))
    if isARM:
        preinst = ["ldr r%d, =mem_data+%d" % (n, align*random.randint(0, 2))]
    else:
        preinst = ["ldr.w r%d, =mem_data+%d" % (n, align*random.randint(0, 2))]
    return (inst, preinst, None)

def gen_neon_inst(isARM):
    idx = random.randint(0, int(len(neon_inst.NEON_INST)*1.2))
    if idx < len(neon_inst.NEON_INST):
        return gen_data_inst(idx, isARM)
    else:
        return gen_mem_inst(isARM)

def test():
    for i in range(0, len(neon_inst.NEON_INST)):
        print gen_data_inst(i)
    for i in range(512):
        print gen_mem_inst()

def main():
    #gen_data_inst(random.randint(0, 0xffff))
    test()


if __name__ == '__main__':
    main()
