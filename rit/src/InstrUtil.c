#include <stdio.h>
#include "InstrUtil.h"
#include "rit-common.h"

extern gen_option_t options;

//instr table, index=op
arm_instr_desc_t glb_instr_tlb[SISA_OP_MAX]= {
    {0},
#define DEF_INST(OP, VAL0, MSK0, VAL1, MSK1,    \
        OCOND,                      \
        O0, O1,                     \
        I0, I1, I2, I3,             \
        CGFLAGS, OPKIND, ...)       \
    {                                           \
        VAL0, MSK0, VAL1, MSK1, OPKIND, #OP,    \
        {O0, O1, I0, I1, I2, I3}                \
    },
#define DEF_LINK(OP,VAL0,MSK0,VAL1,MSK1) {0},
#define DEF_TAIL(OP,VAL0,MSK0,VAL1,MSK1) {0},
#define DEF_CONNECT(OP) {0},
#include "cortex-a9.def"
    { 0 },
#include "Thumb.def"
    { 0 },
#include "Thumb_32.def"
    { 0 },
};



uint32_t bit_count(uint32_t value)
{
    uint32_t cnt;
    for (cnt = 0; value != 0;) {
        cnt += value & 1;
        value = value >> 1;
    }   
    return cnt;
}

//return data[bit]
uint32_t get_bit_value(uint32_t data, uint32_t bit)
{
    uint32_t mask = 0x1 << bit;
    if ((data & mask) == mask) {
        return 1;
    } else {
        return 0;
    }   
}

//return data[end_bit:start_bit]
uint32_t get_bits_value(uint32_t data, uint32_t start_bit, uint32_t end_bit)
{
    uint32_t mask = ((1 << (end_bit + 1)) - 1) ^ ((1 << start_bit) - 1);
    return (data & mask) >> start_bit;
}   

uint32_t get_regnum(uint32_t instr, sisa_opnd_t opnd)
{
    uint32_t opnd_regnum = INVALID_GPR;
    switch (opnd.type) { 
        case SISA_OPND_TYPE_GPR:
        case SISA_OPND_TYPE_GPR_RW:
        case SISA_OPND_TYPE_GPRSET:
        case SISA_OPND_TYPE_SHIFT_REG:
        case SISA_OPND_TYPE_ROTATE_REG:
            opnd_regnum = get_bits_value(instr, opnd.start_bit, opnd.end_bit);
            break;
        case SISA_OPND_TYPE_GPR_CONST:
            opnd_regnum = opnd.bit1;
            break;
        case SISA_OPND_TYPE_GPR_MERGE:
            opnd_regnum = get_bits_value(instr, opnd.start_bit, opnd.end_bit);
            if (get_bit_value(instr, opnd.bit1)) {
                opnd_regnum += 8;
            }   
            break;
        default:
            break;
    }       
    return opnd_regnum;
}

sisa_opcode_t get_instr_op(uint32_t instr)
{
    uint32_t idx, idx_min, idx_max;
    if (options.isa == ISA_ARMv5) {
        idx_min = SISA_OP_NA;
        idx_max = SISA_OP_ARM_MAX;
    } else if (options.isa == ISA_ARMv7) {
        idx_min = SISA_OP_NA;
        idx_max = SISA_OP_ARM_MAX;
    } else if (options.isa == ISA_THUMB) {
        idx_min = SISA_OP_ARM_MAX;
        idx_max = SISA_OP_THUMB_MAX;
    } else {
        idx_min = SISA_OP_NA;
        idx_max = SISA_OP_MAX;
    }   
    for (idx = idx_min; idx < idx_max; idx++) {
        if (glb_instr_tlb[idx].op_kind != NO_OP) {
            if ((instr & glb_instr_tlb[idx].match_msk) == glb_instr_tlb[idx].match_val 
                    && (instr & glb_instr_tlb[idx].neg_msk) != glb_instr_tlb[idx].neg_val) {
                return (sisa_opcode_t)idx;

            }   
        }   
    }   
    return SISA_OP_NA;

}

uint32_t get_mem_addr(uint32_t base, uint32_t offset, bool flag_add, bool flag_index)
{
    uint32_t offset_addr;
    if (flag_add) {
        offset_addr = base + offset;
    } else {
        offset_addr = base - offset;
    }
    if (flag_index) {
        return offset_addr;
    } else {
        return base;
    }
}

uint32_t set_instr(uint32_t input_instr, uint32_t low_bit, uint32_t high_bit, uint32_t new_val)
{
    //input_instr[high_bit:low_bit] = new_val
    uint32_t mask = (((1 << (high_bit + 1)) - 1) ^ ((1 << low_bit) - 1)) ^ 0xffffffff;
    uint32_t val = (new_val & ((1 << (high_bit - low_bit + 1)) - 1)) << low_bit;
    assert(high_bit >= low_bit);
    if (options.verbose) {
        printf("[LOG] old_instr 0x%x, mask 0x%x, ", input_instr, mask);
    }   
    input_instr &= mask;
    input_instr |= val;
    if (options.verbose) {
        printf("[LOG] new_instr 0x%x\n", input_instr);
    }   
    return input_instr;
}

uint32_t set_instr_reg(uint32_t input_instr, sisa_opnd_t reg_opnd, uint32_t new_val)
{
    return set_instr(input_instr, reg_opnd.start_bit, reg_opnd.end_bit, new_val);
}

bool is_fixed_oprand(sisa_opcode_t op, uint32_t start_bit, uint32_t end_bit)
{
    uint32_t idx;
    uint32_t match_msk = glb_instr_tlb[op].match_msk;
    //uint32_t neg_msk = glb_instr_tlb[op].neg_msk;
    for (idx = start_bit; idx <= end_bit; idx++) {
        //if ((get_bit_value(match_msk, idx) == 1) || (get_bit_value(neg_msk, idx) == 1)) {
        if (get_bit_value(match_msk, idx) == 1) {
            return true;
        }
    }
    return false;
}

/*
uint32_t set_bits(uint32_t data, uint32_t start_bit, uint32_t end_bit)
{
    uint32_t mask = ((1 << (end_bit + 1)) - 1) ^ ((1 << start_bit) - 1);
    uint32_t ret_val = data | mask;
    return ret_val;
}

uint32_t clear_bits(uint32_t data, uint32_t start_bit, uint32_t end_bit)
{
    uint32_t mask = ((0xffffffff >> (end_bit + 1)) << (end_bit + 1)) | ((1 << start_bit) - 1);
    uint32_t ret_val = data & mask;
    return ret_val;
}
*/

