#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <string.h>

#include "rit-common.h"
#include "RandomInstrGen.h"
#include "InstrUtil.h"
#include "GenARMInstr.h"
#include "GenThumbInstr.h"
#include "GenThumb32Instr.h"

//bias instruction oprand values
const uint32_t armexpand_imm[]= {0, 0x4ff};
const uint32_t shift_imm[] = {0, 1, 30, 31};
const uint32_t reg_val[] = {0, 0xffffffff, 0x80000000, 0x7fffffff};
const uint32_t mul_reg_val[] = {0, 0xffff, 0xffff0000, 0xffffffff};
const uint32_t shift_reg_val[] = {0, 31, 32, 0x80, 0xc0, 0xff, 0x80808000, 0xffffff00, 0xffffffff};
const uint32_t armexpand_imm_num = sizeof(armexpand_imm) / sizeof(armexpand_imm[0]);
const uint32_t shift_imm_num = sizeof(shift_imm) / sizeof(shift_imm[0]);
const uint32_t reg_val_num = sizeof(reg_val) / sizeof(reg_val[0]);
const uint32_t mul_reg_val_num = sizeof(mul_reg_val) / sizeof(mul_reg_val[0]);
const uint32_t shift_reg_val_num = sizeof(shift_reg_val) / sizeof(shift_reg_val[0]);

static char instr_buf[MAX_INSTR][MAX_STR_INSTR_LEN+1];
static char init_instr_buf[MAX_INSTR][MAX_STR_INSTR_LEN+1];
static uint32_t instr_idx;
static uint32_t init_instr_idx;
static uint32_t raw_instr = 0;
//static uint32_t input_regs;
//static uint32_t output_regs;
bool is_oprand_init;
static bool is_reg_init[16];
static uint32_t reg_init_vals[16];

const char* instr_status_name[] = {
     "VALID", "UNPREDICTABLE", "INST_UNKNOWN", "UNDEFINED", 
     "UNSUPPORTED", "ALIAS", "INST_DIFF", "INVALID", "FIXINST"
};

const sisa_opcode_t arm_uncategoried_op[] = {
    SISA_OP_SBFX,       SISA_OP_BFC,        SISA_OP_BFI,        SISA_OP_UBFX,
    SISA_OP_SHADD16,    SISA_OP_SHADD8,     SISA_OP_SHASX,      SISA_OP_SHSAX,
    SISA_OP_SHSUB16,    SISA_OP_SHSUB8,     SISA_OP_SADD16,     SISA_OP_SADD8,
    SISA_OP_SSUB16,     SISA_OP_SSUB8,      SISA_OP_SASX,       SISA_OP_SSAX,
    SISA_OP_UHADD16,    SISA_OP_UHASX,      SISA_OP_UHSAX,      SISA_OP_UHSUB16,
    SISA_OP_UHADD8,     SISA_OP_UHSUB8,     SISA_OP_UADD16,     SISA_OP_UADD8,
    SISA_OP_USAD8,      SISA_OP_USADA8,     SISA_OP_USUB16,     SISA_OP_USUB8,
    SISA_OP_UASX,       SISA_OP_USAX,       SISA_OP_UQADD16,    SISA_OP_UQASX,
    SISA_OP_UQSAX,      SISA_OP_UQSUB16,    SISA_OP_UQADD8,     SISA_OP_UQSUB8,
    SISA_OP_SXTB16,     SISA_OP_SXTAB16,    SISA_OP_PKH,        SISA_OP_SSAT,
    SISA_OP_SSAT16,     SISA_OP_USAT,       SISA_OP_USAT16,     SISA_OP_SXTB,
    SISA_OP_SXTAB,      SISA_OP_SXTH,       SISA_OP_SXTAH,      SISA_OP_SEL,
    SISA_OP_REV,        SISA_OP_REV16,      SISA_OP_UXTB16,     SISA_OP_UXTAB16,
    SISA_OP_UXTB,       SISA_OP_UXTAB,      SISA_OP_RBIT,       SISA_OP_UXTH,
    SISA_OP_UXTAH,      SISA_OP_REVSH,      SISA_OP_QASX,       SISA_OP_QSAX,
    SISA_OP_QADD8,      SISA_OP_QADD16,     SISA_OP_QSUB8,      SISA_OP_QSUB16,
    SISA_OP_QADD,       SISA_OP_QSUB,       SISA_OP_QDADD,      SISA_OP_QDSUB,
    SISA_OP_MRS_a,      SISA_OP_MSR_imm_a,  SISA_OP_MSR_reg_a,  SISA_OP_PLI_imm,
    SISA_OP_PLD_imm,    SISA_OP_PLD_lit,    SISA_OP_PLI,        SISA_OP_PLD
};
const uint32_t arm_uncategoried_op_num = sizeof(arm_uncategoried_op) / sizeof(arm_uncategoried_op[0]);

const char* opkind_names[] = {
    "NO_OP",
    "EMPTY_OP",
    "ALU_OP",
    "LOG_OP",
    "MOV_OP",
    "CMP_OP",
    "TEST_OP",
    "EXT_OP",
    "LD_OP",
    "ST_OP",
    "LDM_OP",
    "STM_OP",
    "PUSH_OP",
    "POP_OP",
    "JMP_OP",
    "UMUL_OP",
    "MUL32_OP",
    "SMUL_OP",
    "SMULD_OP",
    "BYTE_ALU_OP",
    "S_BYTE_ALU_OP",
    "BF_OP",
    "REVERSE_OP",
    "SSAT_OP",
    "SR_OP",
    "LDREX_OP",
    "STREX_OP",
    "MISC_OP",
    "COPR_OP",
    "VFP_OP",
    "SYS_OP",
    "SIMD_OP",
    "LAST_OP",
    "PSEUDO_OP"
};

#define opkind_num (sizeof(opkind_names) / sizeof(opkind_names[0]))
bool opkind_list[opkind_num]; 

extern gen_option_t options;
//extern char print_buff[1024];
extern char print_buff[BUF_SIZE];

//functions
bool is_op_armv5_instr(sisa_opcode_t op)
{
    //TODO
    return true;
}

void insert_instr(const char* str_instr)
{
    strncpy(instr_buf[instr_idx], str_instr, MAX_STR_INSTR_LEN);
    instr_idx++;
}

void insert_init_instr(const char* str_instr)
{
    strncpy(init_instr_buf[instr_idx], str_instr, MAX_STR_INSTR_LEN);
    init_instr_idx++;
}

uint32_t get_raw_instr()
{
    return raw_instr;
}

void set_raw_instr(uint32_t new_instr)
{
    raw_instr = new_instr;
}

void set_bias_oprand_value(sisa_opcode_t op)
{
    //set bias init value to input regs
    uint32_t opnd_idx, reg, rand_val;
    for (opnd_idx = SISA_LAST_INPUT; opnd_idx >= SISA_FIRST_INPUT; opnd_idx--) {
        sisa_opnd_t opnd = glb_instr_tlb[op].opnds[opnd_idx];
        switch(opnd.type) {
            case SISA_OPND_TYPE_GPR:
                reg = get_regnum(raw_instr, opnd);
                rand_val = rand();
                if (reg != INVALID_GPR && !is_reg_init[reg]) {
                    reg_init_vals[reg] = reg_val[rand_val % reg_val_num];
                    is_reg_init[reg] = true;
                }
                break;
            case SISA_OPND_TYPE_SHIFT_REG:
                reg = get_regnum(raw_instr, opnd);
                rand_val = rand();
                if (reg != INVALID_GPR && !is_reg_init[reg]) {
                    reg_init_vals[reg] = shift_reg_val[rand_val % shift_reg_val_num];
                    is_reg_init[reg] = true;
                }
                break;
            default:
                break;
        }
    }
}

bool is_opnd_revisable_reg(sisa_opnd_t opnd, sisa_opcode_t op)
{
    bool ret_status = false;
    switch (opnd.type) {
        case SISA_OPND_TYPE_GPR:
        case SISA_OPND_TYPE_GPR_RW:
        case SISA_OPND_TYPE_GPRSET:
        case SISA_OPND_TYPE_SHIFT_REG:
        case SISA_OPND_TYPE_ROTATE_REG:
            if (!is_fixed_oprand(op, opnd.start_bit, opnd.end_bit)) {
                ret_status = true;
            }
            break;
        case SISA_OPND_TYPE_GPR_MERGE:
            if (!is_fixed_oprand(op, opnd.start_bit, opnd.end_bit) && 
                    !is_fixed_oprand(op, opnd.bit1, opnd.bit1))
            ret_status = true;
            break;
        case SISA_OPND_TYPE_GPRLIST:
        case SISA_OPND_TYPE_GPRLIST_T:
            ret_status = true;
            break;
        default:
            break;
    }
    return ret_status;
}

/*
bool is_opnd_revisable_imm(sisa_opnd_t opnd)
{
    bool ret_status = false;
    switch (opnd.type) {
        case SISA_OPND_TYPE_IMM:
        case SISA_OPND_TYPE_IMM_MERGE:
        case SISA_OPND_TYPE_SHIFT_IMM:
        case SISA_OPND_TYPE_IMM_ARM_EXP:
        case SISA_OPND_TYPE_IMM_ZERO_EXTEND:
        case SISA_OPND_TYPE_IMM_SIGN_EXTEND:
            ret_status = true;
            break;
        case SISA_OPND_TYPE_SHIFT_IMM_T:
        case SISA_OPND_TYPE_IMM_THUMB_EXP:
            //used in thumb-2
            ret_status = true;
            break;
        default:
            break;
    }
    return ret_status;
}
*/

void update_instr_reg_opnd(sisa_opcode_t op, sisa_opnd_pos_t pos, arm_regs_t reg)
{
    sisa_opnd_t opnd = glb_instr_tlb[op].opnds[pos];
    uint32_t reg_val = rand() % 16;
    switch (reg) {
        case R_0_3:
            reg_val = reg_val % 4;
            break;
        case R_4_7:
            reg_val = reg_val % 4 + 4;
            break;
        case R_8_12:
            reg_val = rand() % 5 + 8;
            break;
        case R_SP:
            reg_val = 13;
            break;
        case R_LR:
            reg_val = 14;
            break;
        case R_PC:
            reg_val = 15;
            break;
        default:
            break;
    }

    switch (opnd.type) {
        case SISA_OPND_TYPE_GPR:
        case SISA_OPND_TYPE_GPR_RW:
        case SISA_OPND_TYPE_GPRSET:
        case SISA_OPND_TYPE_SHIFT_REG:
        case SISA_OPND_TYPE_ROTATE_REG:
            raw_instr = set_instr(raw_instr, opnd.start_bit, opnd.end_bit, reg_val);
            break;
        case SISA_OPND_TYPE_GPRLIST:
            reg_val = reg_val % (opnd.end_bit - opnd.start_bit + 1) + opnd.start_bit;
            if (!is_fixed_oprand(op, reg_val, reg_val))
                raw_instr = set_instr(raw_instr, reg_val, reg_val, 1);
            break;
        case SISA_OPND_TYPE_GPR_MERGE:
            raw_instr = set_instr(raw_instr, opnd.start_bit, opnd.end_bit, reg_val & 0x7);
            if (reg_val > 7) {
                raw_instr = set_instr(raw_instr, opnd.bit1, opnd.bit1, 1);
            } else {
                raw_instr = set_instr(raw_instr, opnd.bit1, opnd.bit1, 0);
            }
            break;
        case SISA_OPND_TYPE_GPRLIST_T:
            if (reg_val == opnd.bit2) {
                reg_val = opnd.bit1;
            } else {
                reg_val = reg_val % (opnd.end_bit - opnd.start_bit + 1) + opnd.start_bit;
            }
            raw_instr = set_instr(raw_instr, reg_val, reg_val, 1);
            break;
        default:
            break;
    }
}

void gen_oprand_init_instrs()
{
    int32_t reg_idx;
    uint32_t thumb_tmp_reg = 0;
    init_instr_idx = 0;
    for (reg_idx = 14; reg_idx >= 0; reg_idx--) {
        if (is_reg_init[reg_idx]) {
            if (options.isa == ISA_THUMB && reg_idx > 7) {
                sprintf(init_instr_buf[init_instr_idx], "[CODE] ldr r%d, =0x%x", thumb_tmp_reg,
                        reg_init_vals[reg_idx]);
                init_instr_idx++;
                sprintf(init_instr_buf[init_instr_idx], "[CODE] mov r%d, r%d", reg_idx, thumb_tmp_reg);
                init_instr_idx++;
            } else {
                sprintf(init_instr_buf[init_instr_idx], "[CODE] ldr r%d, =0x%x", reg_idx, reg_init_vals[reg_idx]);
                init_instr_idx++;
            }
        }
    }
}

void output_instr(sisa_opcode_t op, uint32_t instr, instr_status_t status)
{
    uint32_t idx;
    assert(instr_idx >= 0 && instr_idx <= MAX_INSTR);
    sprintf(print_buff, "[INFO] %s %s %08x\n", instr_status_name[status], glb_instr_tlb[op].name, instr);
    if (options.bias_oprand_value && !is_oprand_init) {
        set_bias_oprand_value(op);
        gen_oprand_init_instrs();
    }
    for (idx = 0; idx < init_instr_idx; idx++) {
        //printf("%s\n", init_instr_buf[idx]);
        strncat(print_buff, init_instr_buf[idx], MAX_STR_INSTR_LEN);
        strcat(print_buff, "\n");
    }
    for (idx = 0; idx < instr_idx; idx++) {
        //printf("%s\n", instr_buf[idx]);
        strncat(print_buff, instr_buf[idx], MAX_STR_INSTR_LEN);
        strcat(print_buff, "\n");
    }
}

void fix_unpredictable_bit(sisa_opcode_t op)
{
    switch (op) {
        case SISA_OP_ASR_imm:
        case SISA_OP_ASR_reg:
        case SISA_OP_LSL_imm:
        case SISA_OP_LSL_reg:
        case SISA_OP_LSR_imm:
        case SISA_OP_LSR_reg:
        case SISA_OP_ROR_imm:
        case SISA_OP_ROR_reg:
        case SISA_OP_RRX_imm:
        case SISA_OP_MOV_imm_a1:
        case SISA_OP_MOV_reg:
        case SISA_OP_MVN_imm:
        case SISA_OP_MVN_reg:
        case SISA_OP_MVN_rsh:
            clear_bits(raw_instr, 16, 19);
            break;
        case SISA_OP_BLX_reg:
        case SISA_OP_BX:
        case SISA_OP_BXJ_reg:
            set_bits(raw_instr, 8, 19);
            break;
        case SISA_OP_CLREX:
            set_bits(raw_instr, 0, 3);
            clear_bits(raw_instr, 8, 11);
            set_bits(raw_instr, 12, 19);
            break;
        case SISA_OP_CLZ:
        case SISA_OP_RBIT:
        case SISA_OP_REV:
        case SISA_OP_REV16:
        case SISA_OP_REVSH:
            set_bits(raw_instr, 8, 11);
            set_bits(raw_instr, 16, 19);
            break;
        case SISA_OP_CMN_imm:
        case SISA_OP_CMN_reg:
        case SISA_OP_CMN_rsh:
        case SISA_OP_CMP_imm:
        case SISA_OP_CMP_reg:
        case SISA_OP_CMP_rsh:
        case SISA_OP_TEQ_imm:
        case SISA_OP_TEQ_reg:
        case SISA_OP_TEQ_rsh:
        case SISA_OP_TST_imm:
        case SISA_OP_TST_reg:
        case SISA_OP_TST_rsh:
        case SISA_OP_MUL:
        case SISA_OP_SMULBB:
        case SISA_OP_SMULWB:
            clear_bits(raw_instr, 12, 15);
            break;
        case SISA_OP_DBG:
        case SISA_OP_NOP:
        case SISA_OP_WFE:
        case SISA_OP_WFI:
        case SISA_OP_YIELD:
            clear_bits(raw_instr, 8, 11);
            set_bits(raw_instr, 12, 15);
            break;
        case SISA_OP_DMB:
        case SISA_OP_DSB:
        case SISA_OP_ISB:
            clear_bits(raw_instr, 8, 11);
            set_bits(raw_instr, 12, 19);
            break;
        case SISA_OP_LDR_lit:
        case SISA_OP_LDRB_lit:
        case SISA_OP_LDRD_lit:
        case SISA_OP_LDRH_lit:
        case SISA_OP_LDRSB_lit:
        case SISA_OP_LDRSH_lit:
            clear_bits(raw_instr, 21, 21);
            set_bits(raw_instr, 24, 24);
            break;
        case SISA_OP_LDREX:
        case SISA_OP_LDREXB:
        case SISA_OP_LDREXD:
        case SISA_OP_LDREXH:
            set_bits(raw_instr, 0, 3);
            set_bits(raw_instr, 8, 11);
            break;
        case SISA_OP_LDRH:
        case SISA_OP_LDRSB:
        case SISA_OP_LDRSH:
        case SISA_OP_STRD_reg:
        case SISA_OP_STREX:
        case SISA_OP_STREXB:
        case SISA_OP_STREXD:
        case SISA_OP_STREXH:
        case SISA_OP_STRH_reg:
        case SISA_OP_STRHT_a2:
        case SISA_OP_SWP:
        case SISA_OP_SWPB:
        case SISA_OP_LDRHT_a2:
        case SISA_OP_LDRSBT_a2:
        case SISA_OP_LDRSHT_a2:
            clear_bits(raw_instr, 8, 11);
            break;
        case SISA_OP_QADD:
        case SISA_OP_QSUB:
        case SISA_OP_QDADD:
        case SISA_OP_QDSUB:
            clear_bits(raw_instr, 8, 11);
            break;
        case SISA_OP_QADD16:
        case SISA_OP_QADD8:
        case SISA_OP_QASX:
        case SISA_OP_QSAX:
        case SISA_OP_QSUB16:
        case SISA_OP_QSUB8:
        case SISA_OP_SADD16:
        case SISA_OP_SADD8:
        case SISA_OP_SASX:
        case SISA_OP_SEL:
        case SISA_OP_SHADD16:
        case SISA_OP_SHADD8:
        case SISA_OP_SHASX:
        case SISA_OP_SHSAX:
        case SISA_OP_SHSUB16:
        case SISA_OP_SHSUB8:
        case SISA_OP_SSAT16:
        case SISA_OP_SSAX:
        case SISA_OP_SSUB16:
        case SISA_OP_SSUB8:
        case SISA_OP_UADD16:
        case SISA_OP_UADD8:
        case SISA_OP_UASX:
        case SISA_OP_UHADD16:
        case SISA_OP_UHADD8:
        case SISA_OP_UHASX:
        case SISA_OP_UHSAX:
        case SISA_OP_UHSUB16:
        case SISA_OP_UHSUB8:
        case SISA_OP_UQADD16:
        case SISA_OP_UQADD8:
        case SISA_OP_UQASX:
        case SISA_OP_UQSAX:
        case SISA_OP_UQSUB16:
        case SISA_OP_UQSUB8:
        case SISA_OP_USAT16:
        case SISA_OP_USAX:
        case SISA_OP_USUB16:
        case SISA_OP_USUB8:
            set_bits(raw_instr, 8, 11);
            break;
        case SISA_OP_MRS_a:
            clear_bits(raw_instr, 0, 3);
            clear_bits(raw_instr, 8, 11);
            set_bits(raw_instr, 16, 19);
            break;
        case SISA_OP_MSR_imm_a:
        case SISA_OP_PLD_imm:
        case SISA_OP_PLD:
        case SISA_OP_PLI_imm:
        case SISA_OP_PLI:
            set_bits(raw_instr, 12, 15);
            break;
        case SISA_OP_PLD_lit:
            set_bits(raw_instr, 12, 15);
            set_bits(raw_instr, 22, 22);
            break;
        case SISA_OP_MSR_reg_a:
            clear_bits(raw_instr, 8, 11);
            set_bits(raw_instr, 12, 15);
            break;
        case SISA_OP_SXTAB:
        case SISA_OP_SXTAB16:
        case SISA_OP_SXTAH:
        case SISA_OP_SXTB:
        case SISA_OP_SXTB16:
        case SISA_OP_SXTH:
        case SISA_OP_UXTAB:
        case SISA_OP_UXTAB16:
        case SISA_OP_UXTAH:
        case SISA_OP_UXTB:
        case SISA_OP_UXTB16:
        case SISA_OP_UXTH:
            clear_bits(raw_instr, 8, 9);
            break;
        case SISA_OP_BLX_reg_T1:
        case SISA_OP_BX_T1:
            clear_bits(raw_instr, 0, 2);
            break;
        case SISA_OP_SRS_T1:
        case SISA_OP_SRS_T2:
            clear_bits(raw_instr, 5, 13);
            set_bits(raw_instr, 14, 19);
            clear_bits(raw_instr, 17, 17);
            break;
        case SISA_OP_RFE_T1:
        case SISA_OP_RFE_T2:
            clear_bits(raw_instr, 0, 13);
            set_bits(raw_instr, 14, 15);
            break;
        case SISA_OP_STM_T2:
        case SISA_OP_STMDB_T1:
        case SISA_OP_PUSH_T2:
            clear_bits(raw_instr, 13, 13);
            clear_bits(raw_instr, 15, 15);
            break;
        case SISA_OP_LDM_T2:
        case SISA_OP_LDMDB_T1:
        case SISA_OP_POP_T2:
            clear_bits(raw_instr, 13, 13);
            break;
        case SISA_OP_LDREX_T1:
        case SISA_OP_STREXB_T1:
        case SISA_OP_STREXH_T1:
            set_bits(raw_instr, 8, 11);
            break;
        case SISA_OP_TBB_T1:
            clear_bits(raw_instr, 8, 11);
            set_bits(raw_instr, 12, 15);
            break;
        case SISA_OP_LDREXB_T1:
        case SISA_OP_LDREXH_T1:
            set_bits(raw_instr, 0, 3);
            set_bits(raw_instr, 8, 11);
            break;
        case SISA_OP_LDREXD_T1:
            set_bits(raw_instr, 0, 3);
            break;
        case SISA_OP_AND_reg_T2:
        case SISA_OP_TST_reg_T2:
        case SISA_OP_BIC_reg_T2:
        case SISA_OP_ORR_reg_T2:
        case SISA_OP_MOV_reg_T3:
        case SISA_OP_ORN_reg_T1:
        case SISA_OP_MVN_reg_T2:
        case SISA_OP_EOR_reg_T2:
        case SISA_OP_TEQ_reg_T1:
        case SISA_OP_PKH_reg_T1:
        case SISA_OP_ADD_reg_T3:
        case SISA_OP_CMN_reg_T2:
        case SISA_OP_ADC_reg_T2:
        case SISA_OP_SBC_reg_T2:
            clear_bits(raw_instr, 15, 15);
            break;
        case SISA_OP_SSAT_T1:
        case SISA_OP_USAT_T1:
        case SISA_OP_SBFX_T1:
        case SISA_OP_UBFX_T1:
        case SISA_OP_BFI_T1:
        case SISA_OP_BFC_T1:
            clear_bits(raw_instr, 5, 5);
            clear_bits(raw_instr, 26, 26);
            break;
        case SISA_OP_SSAT16_T1:
        case SISA_OP_USAT16_T1:
            clear_bits(raw_instr, 4, 5);
            clear_bits(raw_instr, 26, 26);
            break;
        case SISA_OP_MSR_a_T1:
            clear_bits(raw_instr, 13, 13);
            clear_bits(raw_instr, 0, 7);
            break;
        case SISA_OP_MSR_s_T1:
            clear_bits(raw_instr, 0, 7);
            clear_bits(raw_instr, 13, 13);
            break;
        case SISA_OP_CPS_T2:
        case SISA_OP_NOP_T2:
        case SISA_OP_YIELD_T2:
        case SISA_OP_WFE_T2:
        case SISA_OP_WFI_T2:
        case SISA_OP_SEV_T2:
        case SISA_OP_DBG_T1:
            clear_bits(raw_instr, 11, 11);
            clear_bits(raw_instr, 13, 13);
            set_bits(raw_instr, 16, 19);
            break;
        case SISA_OP_CLREX_T1:
            set_bits(raw_instr, 0, 3);
            set_bits(raw_instr, 8, 11);
            clear_bits(raw_instr, 13, 13);
            set_bits(raw_instr, 16, 19);
            break;
        case SISA_OP_MRS_a_T1:
        case SISA_OP_MRS_s_T1:
            clear_bits(raw_instr, 0, 7);
            clear_bits(raw_instr, 13, 13);
            set_bits(raw_instr, 16, 19);
            break;
        case SISA_OP_PLD_lit_T1:
            clear_bits(raw_instr, 21, 21);
            break;
        case SISA_OP_SXTAB_T1:
        case SISA_OP_SXTAH_T1:
        case SISA_OP_SXTAB16_T1:
        case SISA_OP_SXTB16_T1:
        case SISA_OP_UXTAB_T1:
        case SISA_OP_UXTAH_T1:
        case SISA_OP_UXTAB16_T1:
        case SISA_OP_UXTB16_T1:
        case SISA_OP_SXTB_T2:
        case SISA_OP_SXTH_T2:
        case SISA_OP_UXTB_T2:
        case SISA_OP_UXTH_T2:
            clear_bits(raw_instr, 6, 6);
            break;
        case SISA_OP_SDIV_T1:
        case SISA_OP_UDIV_T1:
            set_bits(raw_instr, 12, 15);
            break;
        case SISA_OP_RRX_T1:
            clear_bits(raw_instr, 15, 15);
            break;
        default:
            break;
    }
}

void set_bias_imm_oprands(sisa_opcode_t op)
{
    uint32_t idx, rand_val;
    sisa_opnd_t opnd;
    if (options.bias_oprand_value) {
        for(idx = SISA_FIRST_INPUT; idx <= SISA_LAST_INPUT; idx++) {
            opnd = glb_instr_tlb[op].opnds[idx];
            switch (opnd.type) {
                case SISA_OPND_TYPE_IMM_ARM_EXP:
                    if (!is_fixed_oprand(op, opnd.start_bit, opnd.end_bit)) {
                        rand_val = rand();
                        raw_instr = set_instr(raw_instr, opnd.start_bit, opnd.end_bit, 
                                armexpand_imm[rand_val % armexpand_imm_num]);
                    }
                    break;
                case SISA_OPND_TYPE_SHIFT_IMM:
                    if (!is_fixed_oprand(op, opnd.start_bit, opnd.end_bit)) {
                        rand_val = rand();
                        raw_instr = set_instr(raw_instr, opnd.start_bit, opnd.end_bit, 
                                shift_imm[rand_val % shift_imm_num]);
                    }
                    break;
                default:
                    break;
            }
        }
    }
}

void set_mem_src_reg(sisa_opcode_t op)
{
    arm_regs_t r_offset;
    sisa_opkind_t opkind = glb_instr_tlb[op].op_kind;
    if (options.src_regs == R_0_3) {
        r_offset = R_4_7;
    } else {
        r_offset = R_0_3;
    }
    if (opkind == LD_OP || opkind == LDREX_OP) {
        if (is_opnd_revisable_reg(glb_instr_tlb[op].opnds[SISA_FIRST_INPUT], op)) {
            update_instr_reg_opnd(op, SISA_FIRST_INPUT, options.src_regs);
        }
        if (op == SISA_OP_LDR_reg || op == SISA_OP_LDRB_reg || op == SISA_OP_LDRH || 
                op == SISA_OP_LDRD_reg || op == SISA_OP_LDRSB || op == SISA_OP_LDRSH) {
            update_instr_reg_opnd(op, SISA_FIRST_INPUT+1, r_offset);
        }
        if (op == SISA_OP_LDRT_a2 || op == SISA_OP_LDRHT_a2 || op == SISA_OP_LDRSHT_a2 || 
                op == SISA_OP_LDRBT_a2 || op == SISA_OP_LDRSBT_a2) {
            update_instr_reg_opnd(op, SISA_FIRST_INPUT+1, r_offset);
        }
        if (op == SISA_OP_LDR_reg_T1 || op == SISA_OP_LDRB_reg_T1 || op == SISA_OP_LDRH_reg_T1 || 
                op == SISA_OP_LDRSH_reg_T1 || op == SISA_OP_LDRSB_reg_T1) {
            update_instr_reg_opnd(op, SISA_FIRST_INPUT+1, r_offset);
        }
        if (op == SISA_OP_LDR_reg_T2 || op == SISA_OP_LDRB_reg_T2 || op == SISA_OP_LDRH_reg_T2 ||
                op == SISA_OP_LDRSB_reg_T2 || op == SISA_OP_LDRSH_reg_T2) {
            update_instr_reg_opnd(op, SISA_FIRST_INPUT+1, r_offset);
        }

    } else if (opkind == LDM_OP) {
        if (is_opnd_revisable_reg(glb_instr_tlb[op].opnds[SISA_FIRST_INPUT], op)) {
            update_instr_reg_opnd(op, SISA_FIRST_INPUT, options.src_regs);
        }
        if (options.src_regs == R_0_3) {
            clear_bits(raw_instr, 0, 3);   
        } else if (options.src_regs == R_4_7) {
            clear_bits(raw_instr, 4, 7);
        } else if (options.src_regs == R_8_12) {
            if (op == SISA_OP_LDMDB_T1 || op == SISA_OP_LDM_T2 || 
                    op == SISA_OP_LDMDA || op == SISA_OP_LDM || 
                    op == SISA_OP_LDMDB || op == SISA_OP_LDMIB) {
                clear_bits(raw_instr, 8, 12);
            }
        } else if (options.src_regs == R_LR) {
            if (op == SISA_OP_LDMDB_T1 || op == SISA_OP_LDM_T2 || 
                    op == SISA_OP_LDMDA || op == SISA_OP_LDM || 
                    op == SISA_OP_LDMDB || op == SISA_OP_LDMIB) {
                clear_bits(raw_instr, 14, 14);
            }
        }
    } else if (opkind == STM_OP) {
        if (is_opnd_revisable_reg(glb_instr_tlb[op].opnds[SISA_FIRST_INPUT], op)) {
            update_instr_reg_opnd(op, SISA_FIRST_INPUT, options.src_regs);
        }
    } else if (opkind == ST_OP || opkind == STREX_OP) {
        if (op == SISA_OP_STREXD_T1 || op == SISA_OP_STRD_imm_T1) {
            if (is_opnd_revisable_reg(glb_instr_tlb[op].opnds[SISA_FIRST_INPUT+2], op)) {
                update_instr_reg_opnd(op, SISA_FIRST_INPUT+2, options.src_regs);
            }
        } else {
            if (is_opnd_revisable_reg(glb_instr_tlb[op].opnds[SISA_FIRST_INPUT+1], op)) {
                update_instr_reg_opnd(op, SISA_FIRST_INPUT+1, options.src_regs);
            }
        }
        if (op == SISA_OP_STR_reg || op == SISA_OP_STRB_reg || op == SISA_OP_STRH_reg || op == SISA_OP_STRD_reg) {
            update_instr_reg_opnd(op, SISA_FIRST_INPUT+2, r_offset);
        }
        if (op == SISA_OP_STRT_a2 || op == SISA_OP_STRHT_a2 || op == SISA_OP_STRBT_a2) {
            update_instr_reg_opnd(op, SISA_FIRST_INPUT+2, r_offset);
        }
        if (op == SISA_OP_STR_reg_T1 || op == SISA_OP_STRH_reg_T1 || op == SISA_OP_STRB_reg_T1) {
            update_instr_reg_opnd(op, SISA_FIRST_INPUT+2, r_offset);
        }
        if (op == SISA_OP_STR_reg_T2 || op == SISA_OP_STRB_reg_T2 || op == SISA_OP_STRH_reg_T2) {
            update_instr_reg_opnd(op, SISA_FIRST_INPUT+2, r_offset);
        }
    }
}

void preprocess_instr(sisa_opcode_t op)
{
    uint32_t idx;
    for (idx = 0; idx < 16; idx++) {
        is_reg_init[idx] = false;
        reg_init_vals[idx] = 0;
    }
    fix_unpredictable_bit(op);
    
    /*
    //set src_reg and dst_reg
    if (options.src_regs != R_DEFAULT) {
        uint32_t opnd_idxs[SISA_LAST_INPUT-SISA_FIRST_INPUT+1];
        uint32_t opnd_num = 0;
        for (idx = 0; idx <= SISA_LAST_INPUT-SISA_FIRST_INPUT; idx++) {
            opnd_idxs[idx] = SISA_OPND_NUM;
        }
        for (idx = SISA_FIRST_INPUT; idx <= SISA_LAST_INPUT; idx++) {
            if (is_opnd_revisable_reg(glb_instr_tlb[op].opnds[idx], op)) {
                opnd_idxs[opnd_num] = idx;
                opnd_num++;
            }
        }
        if (opnd_num) {
            update_instr_reg_opnd(op, opnd_idxs[rand()%opnd_num], options.src_regs);
        }
    }*/
    if (options.src_regs != R_DEFAULT) {
        sisa_opkind_t opkind = glb_instr_tlb[op].op_kind;
        if (opkind == LD_OP || opkind == LDREX_OP || opkind == LDM_OP || 
                opkind == ST_OP || opkind == STREX_OP || opkind == STM_OP) {
            set_mem_src_reg(op);
        } else {
            for (idx=SISA_FIRST_INPUT; idx<=SISA_LAST_INPUT;idx++) {
                if (is_opnd_revisable_reg(glb_instr_tlb[op].opnds[idx], op)) {
                    update_instr_reg_opnd(op, idx, options.src_regs);
                }
            }
        }
    }

    /*
    if (options.dst_regs != R_DEFAULT) {
        uint32_t opnd_idxs[SISA_LAST_OUTPUT-SISA_FIRST_OUTPUT+1];
        uint32_t opnd_num = 0;
        for (idx = 0; idx <= SISA_LAST_OUTPUT-SISA_FIRST_OUTPUT; idx++) {
            opnd_idxs[idx] = SISA_OPND_NUM;
        }
        for (idx = SISA_FIRST_OUTPUT; idx <= SISA_LAST_OUTPUT; idx++) {
            if (is_opnd_revisable_reg(glb_instr_tlb[op].opnds[idx], op)) {
                opnd_idxs[opnd_num] = idx;
                opnd_num++;
            }
        }
        if (opnd_num) {
            update_instr_reg_opnd(op, opnd_idxs[rand()%opnd_num], options.dst_regs);
        }
    }*/
    if (options.dst_regs != R_DEFAULT) {
        for (idx=SISA_FIRST_OUTPUT; idx<=SISA_LAST_OUTPUT;idx++) {
            if (is_opnd_revisable_reg(glb_instr_tlb[op].opnds[idx], op)) {
                update_instr_reg_opnd(op, idx, options.dst_regs);
            }
        }
        if (options.dst_regs == R_8_12) {
            if (op == SISA_OP_LDRD_reg || op == SISA_OP_LDRD_imm || 
                    op == SISA_OP_LDRD_lit || op == SISA_OP_LDREXD) {
                raw_instr = set_instr(raw_instr, 14, 14, 0);
                
            }
        }
    }
    // set shift_reg
    // DSHFTGPR
    if (options.is_shift_reg_init) {
        for (idx = SISA_FIRST_INPUT; idx <= SISA_LAST_INPUT; idx++) {
            sisa_opnd_t opnd = glb_instr_tlb[op].opnds[idx];
            if (opnd.type == SISA_OPND_TYPE_SHIFT_REG) {
                reg_init_vals[get_regnum(raw_instr, opnd)] = options.shift_reg_val;
            }
        }
    }

    // set shift_imm
    // DSHFTIMM
    if (options.is_shift_imm_init) {
        for (idx = SISA_FIRST_INPUT; idx <= SISA_LAST_INPUT; idx++) {
            sisa_opnd_t opnd = glb_instr_tlb[op].opnds[idx];
            if (opnd.type == SISA_OPND_TYPE_SHIFT_IMM) {
                if (!is_fixed_oprand(op, opnd.start_bit, opnd.end_bit)) {
                    raw_instr = set_instr(raw_instr, opnd.start_bit, opnd.end_bit, options.shift_imm_val);
                }
            }
        }
    }

    //set_bias_imm_oprands(op);
}

bool is_arm_uncategoried_op(sisa_opcode_t op)
{
    uint32_t idx;
    for (idx = 0; idx < arm_uncategoried_op_num; idx++) {
        if (arm_uncategoried_op[idx] == op) {
            return true;
        }
    }
    return false;
}

void update_opkind_str(const char *str_opkind, bool is_set_opkind)
{
    uint32_t i;
    const char *p_s = str_opkind;
    const char *p_e = ",";
    for (i = 0; i < opkind_num; i++) {
        if (is_set_opkind) {
            opkind_list[i] = false;
        } 
    }
    
    while ((*p_e == ',') && (*p_s != '\0')) {
        p_e = strchr(p_s, ',');
        if (!p_e) {
            p_e = p_s + strlen(p_s);
        }
        for (i = 0; i < opkind_num; i++) {
            if (strncmp(p_s, opkind_names[i], p_e - p_s) == 0) {
                if (is_set_opkind) {
                    opkind_list[i] = true;
                } else {
                    opkind_list[i] = false;
                }
            }
        }
        p_s = p_e + 1;
    }
}

uint32_t get_random_op()
{
    uint32_t idx, idx_min, idx_max;
    uint32_t op_cnt = 0;
    uint32_t tmp_op[SISA_OP_MAX];
    if (options.isa == ISA_ARMv5) {
        idx_min = SISA_OP_NA;
        idx_max = SISA_OP_ARM_MAX;
    } else if (options.isa == ISA_ARMv7) {
        idx_min = SISA_OP_NA;
        idx_max = SISA_OP_ARM_MAX;
    } else if (options.isa == ISA_THUMB){
        idx_min = SISA_OP_ARM_MAX;
        //idx_max = SISA_OP_THUMB_MAX;
        /*if (options.allow_thumb2) {
            idx_max = SISA_OP_MAX;
        } else {
            idx_max = SISA_OP_THUMB_MAX;
        }*/
        idx_max = SISA_OP_MAX;
    } else {
        idx_min = SISA_OP_NA;
        idx_max = SISA_OP_MAX;
    }

    for (idx = idx_min; idx < idx_max; idx++) {
        if (opkind_list[glb_instr_tlb[idx].op_kind]) {
            tmp_op[op_cnt] = idx;
            op_cnt++;
        }
    }

    if (op_cnt) {
        idx = rand() % op_cnt;
        return tmp_op[idx];
    } else {
        return SISA_OP_NA;
    }
}

instr_status_t gen_instr(sisa_opcode_t op)
{
    instr_status_t status = VALID;
    instr_idx = 0;
    is_oprand_init = false;
    preprocess_instr(op);

    //filt unsupported op
    if (glb_instr_tlb[op].op_kind == PSEUDO_OP) {
        printf("[WARNING] Unsupported instruction %s\n", glb_instr_tlb[op].name);
        return UNSUPPORTED;
    }   

    if (op < SISA_OP_ARM_MAX) {
        if (options.isa == ISA_ARMv5 && !is_op_armv5_instr(op)) {
            status = UNSUPPORTED;
        } else {
            if (is_arm_uncategoried_op(op)) {
                status = gen_uncategorized_instr(op);
            } else {
                switch (glb_instr_tlb[op].op_kind) {
                    case ST_OP:
                        status = gen_st_instr(op);
                        break;
                    case LD_OP:
                        status = gen_ld_instr(op);
                        break;
                    case LDREX_OP:
                        status = gen_ldex_instr(op);
                        break;
                    case STREX_OP:
                        status = gen_stex_instr(op);
                        break;
                    case STM_OP:
                    case LDM_OP:
                    case PUSH_OP:
                    case POP_OP:
                        status = gen_multi_mem_instr(op);
                        break;
                    case MISC_OP:
                        status = gen_misc_instr(op);
                        break;
                    case UMUL_OP:
                    case MUL32_OP:
                    case SMUL_OP:
                    case SMULD_OP:
                        status = gen_mul_instr(op);
                        break;
                    case JMP_OP:
                        status = gen_jmp_instr(op);
                        break;
                    case EMPTY_OP:
                        status = gen_empty_instr(op);
                        break;
                    case NO_OP:
                        status = gen_unsup_instr(op);
                        break;
                    case ALU_OP:
                    case LOG_OP:
                    case MOV_OP:
                    case CMP_OP:
                    case TEST_OP:
                        status = gen_data_instr(op);
                        break;
                    default:
                        status = UNSUPPORTED;
                        break;
                }
            }
        }
        //update cond code
        //if ((raw_instr & 0xf0000000) != 0xf0000000) {
        if ((glb_instr_tlb[op].match_msk & 0xf0000000) == 0) {
            raw_instr |= 0xf0000000;
            raw_instr &= (0x0fffffff | (options.cond_val << 28));
        }
        sprintf(instr_buf[instr_idx], "[INST] .inst 0x%08x", raw_instr);
        instr_idx++;
    } else if (op < SISA_OP_THUMB_MAX) {
        options.isa = ISA_THUMB;
        switch (glb_instr_tlb[op].op_kind) {
            case ST_OP:
            case LD_OP:
                status = gen_thumb_mem_instr(op);
                break;
            case STM_OP:
            case LDM_OP:
            case PUSH_OP:
            case POP_OP:
                status = gen_thumb_multi_mem_instr(op);
                break;
            case MISC_OP:
                status = gen_thumb_misc_instr(op);
                break;
            case MUL32_OP:
                status =gen_thumb_mul_instr(op);
                break;
            case JMP_OP:
                status = gen_thumb_jmp_instr(op);
                break;
            case EMPTY_OP:
                status = gen_thumb_empty_instr(op);
                break;
            case NO_OP:
                status = gen_thumb_unsup_instr(op);
                break;
            case ALU_OP:
            case LOG_OP:
            case MOV_OP:
            case CMP_OP:
            case TEST_OP:
                status = gen_thumb_data_instr(op);
                break;
            case EXT_OP:
                status = gen_thumb_ext_instr(op);
                break;
            case REVERSE_OP:
                status = gen_thumb_reverse_instr(op);
                break;
            default:
                status = UNSUPPORTED;
                break;
        }
        sprintf(instr_buf[instr_idx], "[INST] .inst.n 0x%04x", raw_instr & 0xffff);
        instr_idx++;
    } else if (op < SISA_OP_THUMB_32_MAX) {
        options.isa = ISA_THUMB;
        status = gen_thumb32_instr(op);
        sprintf(instr_buf[instr_idx], "[INST] .inst.w 0x%08x", raw_instr);
        instr_idx++;
    } else {
        status = INVALID;
    }
    return status;
}

void gen_instr_with_op(sisa_opcode_t op)
{
    int retry = 8;
    bool is_instr_init = false;
    if (raw_instr != 0) {
        is_instr_init = true;
    }
    instr_status_t status = UNSUPPORTED;

    while (status == UNSUPPORTED && retry > 0) {
        if (!is_instr_init) {
            raw_instr = rand();
        }
        if (options.isa == ISA_ARMv5 || options.isa == ISA_ARMv7) {
            //clear condition field
            raw_instr &= 0x0fffffff;

            //set or clear sbit in arm instruction
            if (options.is_sbit_init && !is_fixed_oprand(op, 20, 20)) {
                if (options.sbit_val) {
                    set_bits(raw_instr, 20, 20);
                } else {
                    clear_bits(raw_instr, 20, 20);
                }
            }
        }
        raw_instr = (raw_instr & (~glb_instr_tlb[op].match_msk)) | glb_instr_tlb[op].match_val;
        status = gen_instr(op);
        retry--;
    }
    output_instr(op, raw_instr, status);
}

void gen_random_instr(const char *str_op)
{
    uint32_t idx;
    for(idx = 1; idx < SISA_OP_MAX; idx++) {
        if (glb_instr_tlb[idx].name && strcmp(glb_instr_tlb[idx].name, str_op) == 0) {
            if (idx > SISA_OP_ARM_MAX) {
                options.isa = ISA_THUMB;
            }
            gen_instr_with_op(idx);
            return;
        }
    }

    printf("[WARNING] Fail to find opcode for %s\n", str_op);
}

void gen_random_instr_without_op()
{
    /*
    sisa_opcode_t op;
    int retry = 8;
    instr_status_t status = INVALID;
    if (raw_instr == 0) {
        raw_instr = rand();
    }
    op = get_instr_op(raw_instr);
    if (op != SISA_OP_NA) {
        while (status != VALID && retry > 0) {
            status = gen_instr(op);
            retry--;
        }
    }
    output_instr(op, raw_instr, status);
    */
    gen_instr_with_op(get_random_op());
}

