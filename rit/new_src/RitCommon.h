/*
 * =====================================================================================
 *
 *       Filename:  RitCommon.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  06/11/2012 12:55:34 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *   Organization:  
 *
 * =====================================================================================
 */

#ifndef RIT_COMMON_H
#define RIT_COMMON_H

#include <stdint.h>
#include "ubt-stubs.h"

#define MAX_REG_NUM 4
#define DEFAULT_ARCH_VER 5
#define REGS_LIST_MASK 0xFFFF
#define MAX_UINT16 0xFFFF
#define RETRY_RELAX_GEAR_1 4
#define RETRY_RELAX_GEAR_2 8
#define RETRY_RELAX_GEAR_3 12
#define RETRY_RELAX_GEAR_4 14
#define RETRY_MAX_TIMES 16

#define GET_OPT_STAT(x) (options.opt_status & (1 << (x)))
#define SET_OPT_STAT(x) (options.opt_status |= (1 << (x)))

typedef uint32_t bitmap32_t;

typedef enum {
    OPT_TOP = 0,
    OPT_ISA = OPT_TOP,
    OPT_ARCH_VERSION = 1,
    OPT_OPKIND = 2,
    OPT_OPCODE = 3,
    OPT_SBIT = 4,
    OPT_COND = 5,
    OPT_SREG = 6,
    OPT_DREG = 7,
    OPT_REGS_LIST = 8,
    OPT_IMM = 9,
    OPT_SHRTREG = 10,
    OPT_SHIMM = 11,
    OPT_SHRTTYPE = 12,
    OPT_FP_SREG_EXT = 13,
    OPT_FP_DREG_EXT = 14,
    OPT_FP_SZ_Q_EXT = 15,
    OPT_MEM_BASE_VAL = 16,
    OPT_MEM_BASE_MSK = 17,
    OPT_MEM_OFFSET_VAL = 18,
    OPT_ALLOW_UNALIGN_MEM = 19,
    OPT_ALLOW_ALL_INSTR = 20,
    OPT_ALLOW_BIAS_OPND = 21,
    OPT_ALLOW_BIAS_REG = 22,
    OPT_NOT_ALLOW_FIX_MEM = 23,
    OPT_NOT_ALLOW_RELAX = 24,
    OPT_INSTR_BIN_INIT = 25,
    OPT_RAND_SEED = 26,
    OPT_VERBOSE = 27,
    OPT_BTM
} option_kind_t;

typedef enum {
    BYTE    =   8,
    HALF    =   16,
    WORD    =   32,
    DOUBLE  =   64
} arm_datatype_t;

typedef enum {
    VALID = 0,
    UNPREDICTABLE,  //UNPREDICTABLE behavior
    INST_UNKNOWN,   //UNKNOWN behavior
    UNDEFINED,      //UNDEFINED
    UNSUPPORTED,    //not supported by rit
    ALIAS,          //see in other opcode
    INST_DIFF,      //different result comparing with qemu
    INVALID,        //INVALID opcode
    FIXINST,        //
} instr_status_t;

typedef enum {
    ISA_TOP      = 0,
    ISA_ARMv5    = ISA_TOP,
    ISA_ARMv7    = 1,
    ISA_THUMB    = 2,
    ISA_THUMB_32 = 3,
    ISA_RAND
} instr_set_t;

typedef enum {
    R_TOP = 0,   R_DEFAULT = R_TOP,
    R_0 = R_TOP, R_A1 = R_0,               R_S0 = R_0,   R_S1 = R_0,   R_D0 = R_0,   R_D16 = R_0,  R_Q0 = R_0,  R_Q8 = R_0,
    R_1,         R_A2 = R_1,               R_S2 = R_1,   R_S3 = R_1,   R_D1 = R_1,   R_D17 = R_1,
    R_2,         R_A3 = R_2,               R_S4 = R_2,   R_S5 = R_2,   R_D2 = R_2,   R_D18 = R_2,  R_Q1 = R_2,  R_Q9 = R_2,
    R_3,         R_A4 = R_3,               R_S6 = R_3,   R_S7 = R_3,   R_D3 = R_3,   R_D19 = R_3,
    R_4,         R_V1 = R_4,               R_S8 = R_4,   R_S9 = R_4,   R_D4 = R_4,   R_D20 = R_4,  R_Q2 = R_4,  R_Q10 = R_4,
    R_5,         R_V2 = R_5,               R_S10 = R_5,  R_S11 = R_5,  R_D5 = R_5,   R_D21 = R_5,
    R_6,         R_V3 = R_6,               R_S12 = R_6,  R_S13 = R_6,  R_D6 = R_6,   R_D22 = R_6,  R_Q3 = R_6,  R_Q11 = R_6,
    R_7,         R_V4 = R_7,               R_S14 = R_7,  R_S15 = R_7,  R_D7 = R_7,   R_D23 = R_7,
    R_8,         R_V5 = R_8,               R_S16 = R_8,  R_S17 = R_8,  R_D8 = R_8,   R_D24 = R_8,  R_Q4 = R_8,  R_Q12 = R_8,
    R_9,         R_V6 = R_9,  R_SB = R_9,  R_S18 = R_9,  R_S19 = R_9,  R_D9 = R_9,   R_D25 = R_9,
    R_10,        R_V7 = R_10, R_SL = R_10, R_S20 = R_10, R_S21 = R_10, R_D10 = R_10, R_D26 = R_10, R_Q5 = R_10, R_Q13 = R_10,
    R_11,        R_V8 = R_11, R_FP = R_11, R_S22 = R_11, R_S23 = R_11, R_D11 = R_11, R_D27 = R_11,
    R_12,        R_IP = R_12,              R_S24 = R_12, R_S25 = R_12, R_D12 = R_12, R_D28 = R_12, R_Q6 = R_12, R_Q14 = R_12,
    R_13,        R_SP = R_13,              R_S26 = R_13, R_S27 = R_13, R_D13 = R_13, R_D29 = R_13,
    R_14,        R_LR = R_14,              R_S28 = R_14, R_S29 = R_14, R_D14 = R_14, R_D30 = R_14, R_Q7 = R_14, R_Q15 = R_14,
    R_15,        R_PC = R_15,              R_S30 = R_15, R_S31 = R_15, R_D15 = R_15, R_D31 = R_15,
//    R_MAX, 
//    R_0_3 = R_MAX,
//    R_4_7,
//    R_8_12,
    R_RAND
} arm_reg_t;

typedef enum {
    FP_REG_TOP = 0,     FP_SZ_Q_TOP = FP_REG_TOP,

    FP_REG_UNSET = FP_REG_TOP, FP_SZ_Q_UNSET = FP_REG_TOP,
    FP_S0 = FP_REG_UNSET,      FP_S2 = FP_REG_UNSET,  FP_S4 = FP_REG_UNSET,  FP_S6 = FP_REG_UNSET,
    FP_S8 = FP_REG_UNSET,      FP_S10 = FP_REG_UNSET, FP_S12 = FP_REG_UNSET, FP_S14 = FP_REG_UNSET,
    FP_S16 = FP_REG_UNSET,     FP_S18 = FP_REG_UNSET, FP_S20 = FP_REG_UNSET, FP_S22 = FP_REG_UNSET,
    FP_S24 = FP_REG_UNSET,     FP_S26 = FP_REG_UNSET, FP_S28 = FP_REG_UNSET, FP_S30 = FP_REG_UNSET, 
    FP_D0 = FP_REG_UNSET,      FP_D1 = FP_REG_UNSET,  FP_D2 = FP_REG_UNSET,  FP_D3 = FP_REG_UNSET,
    FP_D4 = FP_REG_UNSET,      FP_D5 = FP_REG_UNSET,  FP_D6 = FP_REG_UNSET,  FP_D7 = FP_REG_UNSET,
    FP_D8 = FP_REG_UNSET,      FP_D9 = FP_REG_UNSET,  FP_D10 = FP_REG_UNSET, FP_D11 = FP_REG_UNSET,
    FP_D12 = FP_REG_UNSET,     FP_D13 = FP_REG_UNSET, FP_D14 = FP_REG_UNSET, FP_D15 = FP_REG_UNSET,
    FP_Q0 = FP_REG_UNSET,      FP_Q1 = FP_REG_UNSET,  FP_Q2 = FP_REG_UNSET,  FP_Q3 = FP_REG_UNSET,
    FP_Q4 = FP_REG_UNSET,      FP_Q5 = FP_REG_UNSET,  FP_Q6 = FP_REG_UNSET,  FP_Q7 = FP_REG_UNSET,

    FP_REG_SET = 1,      FP_SZ_Q_SET = FP_REG_SET,
    FP_S1 = FP_REG_SET,  FP_S3 = FP_REG_SET,  FP_S5 = FP_REG_SET,  FP_S7 = FP_REG_SET,
    FP_S9 = FP_REG_SET,  FP_S11 = FP_REG_SET, FP_S13 = FP_REG_SET, FP_S15 = FP_REG_SET,
    FP_S17 = FP_REG_SET, FP_S19 = FP_REG_SET, FP_S21 = FP_REG_SET, FP_S23 = FP_REG_SET,
    FP_S25 = FP_REG_SET, FP_S27 = FP_REG_SET, FP_S29 = FP_REG_SET, FP_S31 = FP_REG_SET, 
    FP_D16 = FP_REG_SET, FP_D17 = FP_REG_SET, FP_D18 = FP_REG_SET, FP_D19 = FP_REG_SET,
    FP_D20 = FP_REG_SET, FP_D21 = FP_REG_SET, FP_D22 = FP_REG_SET, FP_D23 = FP_REG_SET,
    FP_D24 = FP_REG_SET, FP_D25 = FP_REG_SET, FP_D26 = FP_REG_SET, FP_D27 = FP_REG_SET,
    FP_D28 = FP_REG_SET, FP_D29 = FP_REG_SET, FP_D30 = FP_REG_SET, FP_D31 = FP_REG_SET,
    FP_Q8 = FP_REG_SET,  FP_Q9 = FP_REG_SET,  FP_Q10 = FP_REG_SET, FP_Q11 = FP_REG_SET,
    FP_Q12 = FP_REG_SET, FP_Q13 = FP_REG_SET, FP_Q14 = FP_REG_SET, FP_Q15 = FP_REG_SET,

    FP_REG_RAND, FP_SZ_Q_RAND = FP_REG_RAND
} arm_fp_reg_ext_t;

typedef enum {
    SH_TYPE_TOP = 0,
    SH_TYPE_LSL = SH_TYPE_TOP,  RT_TYPE_0 = SH_TYPE_LSL,
    SH_TYPE_LSR = 1,            RT_TYPE_8 = SH_TYPE_LSR,
    SH_TYPE_ASR = 2,            RT_TYPE_16 = SH_TYPE_ASR,
    SH_TYPE_ROR = 3,            RT_TYPE_24 = SH_TYPE_ROR,
    SH_TYPE_RRX = SH_TYPE_ROR,
    SH_TYPE_RAND
} sh_rt_type_t;

typedef enum {
    COND_TOP = 0,
    COND_EQ = COND_TOP,
    COND_NE,
    COND_CS,
    COND_CC,
    COND_MI,
    COND_PL,
    COND_VS,
    COND_VC,
    COND_HI,
    COND_LS,
    COND_GE,
    COND_LT,
    COND_GT,
    COND_LE,
    COND_AL,
    COND_RAND
} cond_val_t;

typedef enum {
    SBIT_TOP = 0,
    SBIT_UNSET = SBIT_TOP,
    SBIT_SET,
    SBIT_RAND
} sbit_val_t;

typedef struct {
    instr_set_t isa;
    uint32_t arch_version;

    const char* opcode;

    sbit_val_t sbit_val;        //sbit
    cond_val_t cond_val;        //cond_field

    arm_reg_t src_regs[MAX_REG_NUM];        //src_regs
    arm_reg_t dst_regs[MAX_REG_NUM];        //dst_regs
    arm_fp_reg_ext_t src_fp_regs_ext[MAX_REG_NUM];
    arm_fp_reg_ext_t dst_fp_regs_ext[MAX_REG_NUM];
    arm_fp_reg_ext_t fp_sz_q_bit;
    uint32_t  regs_list;        //dst_regs
    uint32_t imm_val;

    arm_reg_t shift_rotate_reg;       //shift_reg_range
    uint32_t shift_imm_val;         //shift_imm_range
    sh_rt_type_t shift_rotate_type;

    uint32_t mem_base_val;
    uint32_t mem_base_mask;
    uint32_t mem_offset_val;

    bitmap32_t opt_status;

} gen_option_t;

typedef enum {
    IP_TOP = 0,
    IP_ZERO = IP_TOP,
    IP_ONE,
    IP_VAL,
    IP_CML_ONE,
    IP_FULL,
    IP_RAND
} imm_part;

typedef enum {
    GPR_TOP = 0,
    GPR_0_3 = GPR_TOP,
    GPR_4_7,
    GPR_8_12,
    GPR_13,
    GPR_14,
    GPR_15,
    GPR_RAND
} gpreg_part;

typedef enum {
    CP_TOP = 0,
    CP_COND,
    CP_AL,
    CP_RAND
} cond_part;

typedef struct {
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
