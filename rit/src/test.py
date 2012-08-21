from ctypes import *
import random

#cdll.LoadLibrary('librit.so')
cdll.LoadLibrary('/home/gchen22/workspace/Houdini/trunk/dev-tools/rit/src/librit.so')
librit = CDLL('/home/gchen22/workspace/Houdini/trunk/dev-tools/rit/src/librit.so')
#librit.py_test(5)

opkind = 'LD_OP'
c_str_opkind = c_char_p(opkind)
output = create_string_buffer(1024)
librit.py_gen_instr(output, 1, c_str_opkind, 1139273350, 14, 0, 4, 0, 2769717578)
print repr(output.value)
#for i in range(10):
#    hexcode = random.randint(0, 2**32-1)
#    seed = random.randint(0, 2**32-1)
#    librit.py_gen_instr(output, 1, c_str_opkind, hexcode, 14, 1, 3, 3, seed)
#    print repr(output.value)

#op = 'ADD_reg_T3'
#c_str_op = c_char_p(opkind)
#librit.py_gen_instr_with_op(c_str_op, 0x23456789, 2, 1, 2, 14)
#c_str_op = c_char_p('CMP_reg_T3')
#librit.py_gen_instr_with_op(c_str_op, 0x23456789, 2, 1, 2, 14)

