#ifndef RANDOM_INSTR_GEN_H
#define RANDOM_INSTR_GEN_H
#include <stdint.h>

sisa_opcode_t get_random_op();
void init_env_val();
instr_status_t gen_random_instr();
void insert_instr(const char *str_instr);
void insert_init_instr(const char *str_instr);
uint32_t get_raw_instr();
void set_raw_instr(uint32_t new_instr);
void update_opkind_str(const char *str_opkind);

#endif
