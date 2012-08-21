#ifndef GEN_ARM_INSTR_H
#define GEN_ARM_INSTR_H
#include "rit-common.h"

instr_status_t gen_ld_instr(sisa_opcode_t op);
instr_status_t gen_st_instr(sisa_opcode_t op);
instr_status_t gen_multi_mem_instr(sisa_opcode_t op);
instr_status_t gen_misc_instr(sisa_opcode_t op);
instr_status_t gen_data_instr(sisa_opcode_t op);
instr_status_t gen_mul_instr(sisa_opcode_t op);
instr_status_t gen_jmp_instr(sisa_opcode_t op);
instr_status_t gen_unsup_instr(sisa_opcode_t op);
instr_status_t gen_empty_instr(sisa_opcode_t op);
instr_status_t gen_uncategorized_instr(sisa_opcode_t op);
instr_status_t gen_ldex_instr(sisa_opcode_t op);
instr_status_t gen_stex_instr(sisa_opcode_t op);


#endif
