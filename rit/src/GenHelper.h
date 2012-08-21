#ifndef GEN_HELPER_H
#define GEN_HELPER_H
#include "rit-common.h"

uint32_t gen_pre_instr_for_mem_base(uint32_t reg_base);
uint32_t get_rand_mem_offset(uint32_t shift_t, uint32_t shift_n, uint32_t *offset_val);
bool     is_mem_align(uint32_t addr, uint32_t align_bit);
void     fix_mem_base(uint32_t reg_base, bool forceAlign);

#endif
