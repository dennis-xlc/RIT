import random

class OPKINDType():
    ALL         = 0
    MEM         = 1
    STACK       = 2
    MUL         = 3
    DATA        = 4


class REGType():
    R_0_3       = 0
    R_4_7       = 4
    R_8_12      = 8
    R_SP        = 13
    R_LR        = 14
    R_PC        = 15

IT_MASK     = ('', 't', 'e', 'tt', 'et', 'te', 'ee', 
                'ttt', 'ett', 'tet', 'eet', 'tte', 'ete', 'tee', 'eee')

STACK_OP    = ('PUSH_OP', 'POP_OP')
#MEM_OP      = ('LD_OP', 'ST_OP', 'LDM_OP', 'STM_OP', 'LDREX_OP', 'STREX_OP')
#MEM_OP      = ('LD_OP', 'ST_OP', 'LDM_OP', 'STM_OP')
MEM_OP       = tuple(['LD_OP'])
MUL_OP      = ('MUL32_OP', 'UMUL_OP', 'SMUL_OP', 'SMULD_OP')
DATA_OP     = ('ALU_OP', 'LOG_OP', 'MOV_OP', 'BYTE_ALU_OP', 'S_BYTE_ALU_OP')
MISC_OP     = ('CMP_OP', 'TEST_OP', 'EXT_OP', 'BF_OP', 'REVERSE_OP', 'SR_OP', 'MISC_OP')
REST_OP     = ('NO_OP', 'EMPTY_OP')
VFP_OP      = tuple(['VFP_OP'])
ALL_OP      = STACK_OP + MEM_OP + MUL_OP + DATA_OP + MISC_OP + VFP_OP
#+ REST_OP
#('COPR_OP', 'JMP_OP', 'VFP_OP')

MUL_OPKIND = ('MUL_OP', 'SMUL_OP', 'SMULD_OP')
ARM_SHIFT_OP = ('AND_rsh', 'EOR_rsh', 'SUB_rsh', 'RSB_rsh', 'ADD_rsh',
                'ADC_rsh', 'SBC_rsh', 'RSC_rsh', 'TST_rsh', 'TEQ_rsh', 
                'CMP_rsh', 'CMN_rsh', 'ORR_rsh', 'BIC_rsh', 'MVN_rsh', 
                'LSL_reg', 'LSR_reg', 'ASR_reg', 'ROR_reg')

THUMB_SHIFT_OP = ('LSL_reg_T2', 'LSR_reg_T2', 'ASR_reg_T2', 'ROR_reg_T2')

ARM_ALU_W_PC_sreg_4_0 = ('ADC_reg','AND_reg','BIC_reg','EOR_reg','ORR_reg',
                         'RSB_reg','RSC_reg','SBC_reg','ADD_reg','SUB_reg')

ARM_ALU_W_PC_sreg_4 = ('ADC_imm','AND_imm','ADD_imm','RSC_imm','SBC_imm',
                        'BIC_imm','EOR_imm','ORR_imm','RSB_imm','SUB_imm')

ARM_ALU_W_PC_sreg_0 = ('MOV_reg','MVN_reg','LSL_imm','LSR_imm','ASR_imm',
                       'ROR_imm','RRX_imm')

ARM_ALU_W_PC_sreg_nul = ('ADR_a1','ADR_a2','MOV_imm_a1','MVN_imm')

ARM_ALU_W_PC_ALL = ARM_ALU_W_PC_sreg_4_0 + ARM_ALU_W_PC_sreg_4 + ARM_ALU_W_PC_sreg_0 + ARM_ALU_W_PC_sreg_nul

THUMB_ALU_W_PC = ('ADD_reg_T2','MOV_reg_T1')

Sbit_Val = ['0','1','2','3','4','5','6','7','8','9','a','b','c','d','e','f','A','B','C','D','E','F']

Sbit_Msk = ['0','0','2','2','4','4','6','6','8','8','a','a','c','c','e','e','A','A','C','C','E','E']

ARM_PC_RET = 0xe12fff1e

THUMB_PC_RET = 0x4770

ALU_W_PC_VALID_BEG_ADDR = '0x10000000'

ALU_W_PC_VALID_SPACE_SIZE = '0x20000000'

CANDIDATE_REGS = (REGType.R_0_3, REGType.R_4_7, REGType.R_8_12, 
                  REGType.R_SP,  REGType.R_LR,  REGType.R_PC)

CONDCODE    = ['eq', 'ne', 'cs', 'cc', 'mi', 'pl', 'vs', 'vc',
               'hi', 'ls', 'ge', 'lt', 'gt', 'le']

CONDVALUE = [0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13]

INVCONDCODE    = ['ne', 'eq', 'cc', 'cs', 'pl', 'mi', 'vc', 'vs',
               'ls', 'hi', 'lt', 'ge', 'le', 'gt']

ARM_VAL     = (0x0, 0x1, 0x1e, 0x1f, 0x20, 0xfe, 0xff)
R_MUL_VAL   = (0x0, 0xffff, 0x80000000, 0x80008000, 0x8000ffff, 0xffff0000, 0xffff8000, 0xffffffff)
R_SHIFT_VAL = (0x0, 0x1f, 0xff, 0xffffffff)

OP_SIZE = ['b', 'h', 'w', 'd']

EXCL_CNT = 0xFF

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

def getOpkind(type):
    if type == OPKINDType.STACK:
        return STACK_OP
    elif type == OPKINDType.MEM:
        return MEM_OP
    elif type == OPKINDType.MUL:
        return MUL_OP
    elif type == OPKINDType.DATA:
        return DATA_OP
    elif type == OPKINDType.ALL:
        return ALL_OP
    else:
        return None

