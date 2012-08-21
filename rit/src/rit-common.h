#ifndef RIT_COMMON_H
#define RIT_COMMON_H

#include <stdint.h>
#include "ubt-stubs.h"

typedef enum{
    VALID,
    UNPREDICTABLE,  //UNPREDICTABLE behavior
    INST_UNKNOWN,   //UNKNOWN behavior
    UNDEFINED,      //UNDEFINED
    UNSUPPORTED,    //not supported by rit
    ALIAS,          //see in other opcode
    INST_DIFF,      //different result comparing with qemu
    INVALID,        //INVALID opcode
    FIXINST,        //
} instr_status_t;

typedef enum{
    ISA_ARMv5,
    ISA_ARMv7,
    ISA_THUMB,
} instr_set_t;

typedef enum {
    R_DEFAULT,
    R_0_3,
    R_4_7,
    R_8_12,
    R_SP,
    R_LR,
    R_PC,
} arm_regs_t;

struct gen_option_str {
    //bool allow_unpredictable;
    //bool allow_unknwon;
    //bool force_invalid;
    //bool gen_bias_instr;
    bool bias_register;
    bool bias_oprand_value;
    bool allow_unalign_mem;
    bool allow_all_instr;
    bool fix_mem;

    uint32_t mem_base_val;
    uint32_t mem_base_mask;
    uint32_t mem_offset_val;

    bool is_cond_init;
    int cond_val;               //cond_field
    arm_regs_t src_regs;        //src_regs
    arm_regs_t dst_regs;        //dst_regs
    bool is_sbit_init;
    int sbit_val;               //sbit

    bool is_shift_imm_init;
    uint32_t shift_imm_val;     //shift_imm_range
    bool is_shift_reg_init;
    uint32_t shift_reg_val;     //shift_reg_range

    int arch_version;
    int verbose;
    instr_set_t isa;
};
typedef struct gen_option_str gen_option_t;

typedef struct{
    uint32_t match_val;
    uint32_t match_msk;
    uint32_t neg_val;
    uint32_t neg_msk;
    uint16_t op_kind;
    char* name;
    sisa_opnd_t opnds[SISA_OPND_NUM];
} arm_instr_desc_t;
//SISA_FIRST_OUTPUT
//SISA_LAST_OUTPUT
//SISA_OUTPUT0/1

//variables
#define INVALID_GPR 0xff
#define MAX_INSTR 8
#define MAX_STR_INSTR_LEN 100
#define BUF_SIZE 1024


#ifdef DEBUG
#define LOG(format, args ...)           \
    do {                                \
        printf("[LOG] ");               \
        printf(format, ##args);         \
    } while(0)
#else
#define LOG(format, args ...)           \
    do {                                \
        if (options.verbose) {          \
            printf("[LOG] ");           \
            printf(format, ##args);     \
        }                               \
    } while(0)
#endif

#endif
