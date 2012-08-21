#ifndef INSTR_UTIL_H
#define INSTR_UTIL_H

#include "RitCommon.h"

#define set_bit(data, bit)  \
    ((data) = (data) | (1<<(bit)))

#define clear_bit(data, bit)    \
    ((data) = (data) & (~(1<<(bit))))

#define set_bits(data, start_bit, end_bit)          \
    ((data) = (data) | (((1<<((end_bit)+1))-1)^((1<<(start_bit))-1)))

#define clear_bits(data, start_bit, end_bit)        \
    ((data) = (data) & (((0xffffffff>>((end_bit)+1))<<((end_bit)+1))|((1<<(start_bit))-1)))

extern arm_instr_desc_t glb_instr_tlb[SISA_OP_MAX];
extern gen_option_t options;

uint32_t bit_count(uint32_t value);
uint32_t get_bit_value(uint32_t data, uint32_t bit);
uint32_t get_bits_value(uint32_t data, uint32_t start_bit, uint32_t end_bit);
uint32_t get_regnum(uint32_t instr, sisa_opnd_t opnd);
sisa_opcode_t get_instr_op(uint32_t instr);
uint32_t get_mem_addr(uint32_t base, uint32_t offset, bool flag_add, bool flag_index);
uint32_t set_instr(uint32_t input_instr, uint32_t low_bit, uint32_t high_bit, uint32_t new_val);
uint32_t set_instr_reg(uint32_t input_instr, sisa_opnd_t reg_opnd, uint32_t new_val);
bool is_fixed_oprand(sisa_opcode_t op, uint32_t start_bit, uint32_t end_bit);
//uint32_t set_bits(uint32_t data, uint32_t start_bit, uint32_t end_bit);
//uint32_t clear_bits(uint32_t data, uint32_t start_bit, uint32_t end_bit);

#endif

