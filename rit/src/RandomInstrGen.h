#ifndef RANDOM_INSTR_GEN_H
#define RANDOM_INSTR_GEN_H
#include <stdint.h>

void gen_random_instr(const char *str_op);
void gen_random_instr_without_op();
void insert_instr(const char *str_instr);
void insert_init_instr(const char *str_instr);
uint32_t get_raw_instr();
void set_raw_instr(uint32_t new_instr);
void update_opkind_str(const char *str_opkind, bool is_set_opkind);
#endif
