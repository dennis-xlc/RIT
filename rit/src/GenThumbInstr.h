#ifndef GEN_THUMB_INSTR_H
#define GEN_THUMB_INSTR_H
#include "rit-common.h"

instr_status_t gen_thumb_mem_instr(sisa_opcode_t op);
instr_status_t gen_thumb_multi_mem_instr(sisa_opcode_t op);
instr_status_t gen_thumb_misc_instr(sisa_opcode_t op);
instr_status_t gen_thumb_data_instr(sisa_opcode_t op);
instr_status_t gen_thumb_mul_instr(sisa_opcode_t op);
instr_status_t gen_thumb_jmp_instr(sisa_opcode_t op);
instr_status_t gen_thumb_unsup_instr(sisa_opcode_t op);
instr_status_t gen_thumb_empty_instr(sisa_opcode_t op);
instr_status_t gen_thumb_ext_instr(sisa_opcode_t op);
instr_status_t gen_thumb_reverse_instr(sisa_opcode_t op);

#endif
