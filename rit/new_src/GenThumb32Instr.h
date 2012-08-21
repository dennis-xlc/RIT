#ifndef GEN_THUMB_32_INSTR_H
#define GEN_THUMB_32_INSTR_H

#include "RitCommon.h"

instr_status_t gen_thumb32_data_shifted_reg(sisa_opcode_t op);
instr_status_t gen_thumb32_data_mimm(sisa_opcode_t op);
instr_status_t gen_thumb32_data_pimm(sisa_opcode_t op);
instr_status_t gen_thumb32_data_reg(sisa_opcode_t op);
instr_status_t gen_thumb32_data_parallel(sisa_opcode_t op);
instr_status_t gen_thumb32_data_misc(sisa_opcode_t op);
instr_status_t gen_thumb32_data_mul(sisa_opcode_t op);
instr_status_t gen_thumb32_multi_mem(sisa_opcode_t op);
instr_status_t gen_thumb32_ld_imm(sisa_opcode_t op);
instr_status_t gen_thumb32_mem_reg(sisa_opcode_t op);
instr_status_t gen_thumb32_mem_lit(sisa_opcode_t op);
instr_status_t gen_thumb32_mem_ex(sisa_opcode_t op);
instr_status_t gen_thumb32_mem_preload(sisa_opcode_t op);
instr_status_t gen_thumb32_st_imm(sisa_opcode_t op);
instr_status_t gen_thumb32_sr(sisa_opcode_t op);
instr_status_t gen_thumb32_hint(sisa_opcode_t op);
instr_status_t gen_thumb32_instr(sisa_opcode_t op);

#endif
