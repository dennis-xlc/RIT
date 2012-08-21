#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <string.h>
#include <sys/time.h>

#include "RitCommon.h"
#include "RandomInstrGen.h"
#include "InstrUtil.h"
#include "GenARMInstr.h"
#include "GenThumbInstr.h"
#include "GenThumb32Instr.h"

//bias instruction oprand values
const uint32_t reg_val[] = {0, 0xffffffff, 0x80000000, 0x7fffffff};
const uint32_t reg_val_num = sizeof(reg_val) / sizeof(reg_val[0]);
const uint32_t shift_imm[] = {0, 1, 30, 31};
const uint32_t shift_imm_num = sizeof(shift_imm) / sizeof(shift_imm[0]);
const uint32_t shift_reg_val[] = {0, 31, 32, 0x80, 0xc0, 0xff, 0x80808000, 0xffffff00, 0xffffffff};
const uint32_t shift_reg_val_num = sizeof(shift_reg_val) / sizeof(shift_reg_val[0]);
const uint32_t armexpand_imm[]= {0, 0x4ff};
const uint32_t armexpand_imm_num = sizeof(armexpand_imm) / sizeof(armexpand_imm[0]);
const uint32_t mul_reg_val[] = {0, 0xffff, 0xffff0000, 0xffffffff};
const uint32_t mul_reg_val_num = sizeof(mul_reg_val) / sizeof(mul_reg_val[0]);

gen_option_t shadow_options;
bool is_reg_init[16];
uint32_t reg_init_vals[16];
char instr_buf[MAX_INSTR][MAX_STR_INSTR_LEN+1];
char init_instr_buf[MAX_INSTR][MAX_STR_INSTR_LEN+1];
uint32_t instr_idx;
uint32_t init_instr_idx;
uint32_t instr_fixed_bit;
uint32_t cur_fixed_bit;
bool is_set_fixed_bit;
bool is_oprand_init;
bool is_reg_bit_cut;

gen_option_t options;
uint32_t raw_instr;
char print_buf[BUF_SIZE];

uint32_t sreg_idx;
uint32_t dreg_idx;
uint32_t fp_sreg_ext_idx;
uint32_t fp_dreg_ext_idx;

const char* instr_status_name[] = {
     "VALID", "UNPREDICTABLE", "INST_UNKNOWN", "UNDEFINED", 
     "UNSUPPORTED", "ALIAS", "INST_DIFF", "INVALID", "FIXINST"
};

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

void init_env_val ()
{
    raw_instr = 0;
    instr_fixed_bit = 0;
    cur_fixed_bit = 0;
    is_set_fixed_bit = false;
    is_reg_bit_cut = false;

    sreg_idx = 0;
    dreg_idx = 0;
    fp_sreg_ext_idx = 0;
    fp_dreg_ext_idx = 0;

    is_oprand_init = false;
    instr_idx = 0;
    init_instr_idx = 0;
    uint32_t idx;
    for (idx = 0; idx < 16; idx++) {
        is_reg_init[idx] = false;
        reg_init_vals[idx] = 0;
    }
    memset(print_buf, 0, BUF_SIZE*sizeof(char));
    memset(instr_buf, 0, MAX_INSTR*(MAX_STR_INSTR_LEN+1)*sizeof(char));
    memset(init_instr_buf, 0, MAX_INSTR*(MAX_STR_INSTR_LEN+1)*sizeof(char));
    memset(&options, 0, sizeof(gen_option_t));
    memset(&shadow_options, 0, sizeof(gen_option_t));
    for (idx=0; idx < MAX_REG_NUM; idx++) {
        options.src_regs[idx] = R_RAND;
        options.dst_regs[idx] = R_RAND;
        options.src_fp_regs_ext[idx] = FP_REG_RAND;
        options.dst_fp_regs_ext[idx] = FP_REG_RAND;
    }
    options.isa                 = ISA_RAND;
    options.arch_version        = 7;
    options.opcode              = NULL;
    options.sbit_val            = SBIT_RAND;
    options.cond_val            = COND_RAND;
    options.regs_list           = 0;
    options.imm_val             = 0;
    options.shift_rotate_reg    = R_RAND;
    options.shift_imm_val       = 0;
    options.shift_rotate_type   = SH_TYPE_RAND;
    options.fp_sz_q_bit         = FP_REG_RAND;
    options.mem_base_val        = 0x03fc0000;
    options.mem_base_mask       = 0xfffc0000;
    options.mem_offset_val      = 0x00000fff;
    options.opt_status          = 0;

    //without
    //NO_OP/JMP_OP
    //LDREX_OP/STREX_OP
    update_opkind_str("EMPTY_OP,ALU_OP,LOG_OP,MOV_OP,CMP_OP,TEST_OP,EXT_OP,LD_OP,ST_OP,LDM_OP,STM_OP,PUSH_OP,POP_OP,UMUL_OP,MUL32_OP,SMUL_OP,SMULD_OP,BYTE_ALU_OP,S_BYTE_ALU_OP,BF_OP,REVERSE_OP,SSAT_OP,SR_OP,MISC_OP");
}

void insert_instr(const char* str_instr) {
    strncpy(instr_buf[instr_idx], str_instr, MAX_STR_INSTR_LEN);
    instr_idx++;
}

void chk_3bit_reg (uint8_t reg_bits, uint32_t reg_val) {
    if (is_reg_bit_cut) {
    // Already encontered 3bit reg
        return;
    }

    if ((reg_bits < 4) && (reg_val | 0x8)) {
    // The 4th bit of reg No. is 1, will be ignored in 3bit reg
        is_reg_bit_cut = true;
    }
}

uint32_t get_raw_instr() {
    return raw_instr;
}

void set_raw_instr(uint32_t new_instr) {
    raw_instr = new_instr;
}

void set_bias_oprand_value(sisa_opcode_t op) {
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

void set_instr_fixed_bit (uint32_t start_bit, uint32_t end_bit) {
    if (is_set_fixed_bit) {
        return;
    }

    set_bits (instr_fixed_bit, start_bit, end_bit);
}

void set_sbit (sisa_opcode_t op) {
    if (GET_OPT_STAT(OPT_SBIT) && ! is_fixed_oprand(op, 20, 20)) {
        if (SBIT_RAND != shadow_options.sbit_val) {
            set_instr_fixed_bit (20, 20);
        }
        if (SBIT_SET == options.sbit_val) {
            set_bit(raw_instr, 20);
        } else {
            clear_bit(raw_instr, 20);
        }
    }
}

void set_arm_cond_bits (sisa_opcode_t op) {
    // Only set cond bits for ARM. cond of B in thumb will be set later.
    if ((options.isa == ISA_ARMv5 || options.isa == ISA_ARMv7) && op < SISA_OP_ARM_MAX) {
        if (GET_OPT_STAT(OPT_COND) ) {
            if (COND_RAND != shadow_options.cond_val) {
                set_instr_fixed_bit (cond_mask_bit, 31);
            }
            raw_instr |= cond_mask;
            raw_instr &= ((~cond_mask) | (options.cond_val << cond_mask_bit));
        }

        if (cond_mask == (raw_instr & cond_mask)) {
            raw_instr &= ~(1 << cond_mask_bit);
        }
    }
}

void gen_oprand_init_instrs()
{
    int32_t reg_idx;
    uint32_t thumb_tmp_reg = 0;
    init_instr_idx = 0;
    memset(init_instr_buf, 0, MAX_INSTR*(MAX_STR_INSTR_LEN+1)*sizeof(char));
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
    sprintf(print_buf, "[INFO] %s %s 0x%08x\n", instr_status_name[status], glb_instr_tlb[op].name, instr);
    if (GET_OPT_STAT(OPT_ALLOW_BIAS_OPND) && !is_oprand_init) {
        set_bias_oprand_value(op);
        gen_oprand_init_instrs();
    }
    for (idx = 0; idx < init_instr_idx; idx++) {
        strncat(print_buf, init_instr_buf[idx], MAX_STR_INSTR_LEN);
        strcat(print_buf, "\n");
    }
    for (idx = 0; idx < instr_idx; idx++) {
        strncat(print_buf, instr_buf[idx], MAX_STR_INSTR_LEN);
        strcat(print_buf, "\n");
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
            clear_bit(raw_instr, 21);
            set_bit(raw_instr, 24);
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
            set_bit(raw_instr, 22);
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
            clear_bit(raw_instr, 17);
            break;
        case SISA_OP_RFE_T1:
        case SISA_OP_RFE_T2:
            clear_bits(raw_instr, 0, 13);
            set_bits(raw_instr, 14, 15);
            break;
        case SISA_OP_STM_T2:
        case SISA_OP_STMDB_T1:
        case SISA_OP_PUSH_T2:
            clear_bit(raw_instr, 13);
            clear_bit(raw_instr, 15);
            break;
        case SISA_OP_LDM_T2:
        case SISA_OP_LDMDB_T1:
        case SISA_OP_POP_T2:
            clear_bit(raw_instr, 13);
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
            clear_bit(raw_instr, 15);
            break;
        case SISA_OP_SSAT_T1:
        case SISA_OP_USAT_T1:
        case SISA_OP_SBFX_T1:
        case SISA_OP_UBFX_T1:
        case SISA_OP_BFI_T1:
        case SISA_OP_BFC_T1:
            clear_bit(raw_instr, 5);
            clear_bit(raw_instr, 26);
            break;
        case SISA_OP_SSAT16_T1:
        case SISA_OP_USAT16_T1:
            clear_bits(raw_instr, 4, 5);
            clear_bit(raw_instr, 26);
            break;
        case SISA_OP_MSR_a_T1:
            clear_bit(raw_instr, 13);
            clear_bits(raw_instr, 0, 7);
            break;
        case SISA_OP_MSR_s_T1:
            clear_bits(raw_instr, 0, 7);
            clear_bit(raw_instr, 13);
            break;
        case SISA_OP_CPS_T2:
        case SISA_OP_NOP_T2:
        case SISA_OP_YIELD_T2:
        case SISA_OP_WFE_T2:
        case SISA_OP_WFI_T2:
        case SISA_OP_SEV_T2:
        case SISA_OP_DBG_T1:
            clear_bit(raw_instr, 11);
            clear_bit(raw_instr, 13);
            set_bits(raw_instr, 16, 19);
            break;
        case SISA_OP_CLREX_T1:
            set_bits(raw_instr, 0, 3);
            set_bits(raw_instr, 8, 11);
            clear_bit(raw_instr, 13);
            set_bits(raw_instr, 16, 19);
            break;
        case SISA_OP_MRS_a_T1:
        case SISA_OP_MRS_s_T1:
            clear_bits(raw_instr, 0, 7);
            clear_bit(raw_instr, 13);
            set_bits(raw_instr, 16, 19);
            break;
        case SISA_OP_PLD_lit_T1:
            clear_bit(raw_instr, 21);
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
            clear_bit(raw_instr, 6);
            break;
        case SISA_OP_SDIV_T1:
        case SISA_OP_UDIV_T1:
            set_bits(raw_instr, 12, 15);
            break;
        case SISA_OP_RRX_T1:
            clear_bit(raw_instr, 15);
            break;
        default:
            break;
    }
}

void set_opnd_bits (sisa_opcode_t op, bool opnd_kind) {

    int idx = 0;
    int remainder_bits = 0;
    int reg_idx = 0;
    int i_bit_pos = 0;
    int imm_startbit = 0;
    int imm_endbit = 0;
    arm_reg_t* regs_arr = NULL;
    arm_reg_t* shadow_regs_arr = NULL;
    arm_fp_reg_ext_t* fp_regs_ext_arr = NULL;
    arm_fp_reg_ext_t* shadow_fp_regs_ext_arr = NULL;
    option_kind_t opt_reg_kind = OPT_BTM;
    option_kind_t opt_fp_reg_kind = OPT_BTM;
    sisa_opnd_t opnd;
    sisa_opnd_type_t opnd_type = SISA_OPND_TYPE_MAX_NUM;
    sisa_opnd_pos_t opnd_pos_start;
    sisa_opnd_pos_t opnd_pos_end;

    if (opnd_kind) {
        regs_arr = options.src_regs;
        shadow_regs_arr = shadow_options.src_regs;
        fp_regs_ext_arr = options.src_fp_regs_ext;
        shadow_fp_regs_ext_arr = shadow_options.src_fp_regs_ext;
        opt_reg_kind = OPT_SREG;
        opt_fp_reg_kind = OPT_FP_SREG_EXT;
        opnd_pos_start = SISA_FIRST_INPUT;
        opnd_pos_end = SISA_LAST_INPUT;
    } else {
        regs_arr = options.dst_regs;
        shadow_regs_arr = shadow_options.dst_regs;
        fp_regs_ext_arr = options.dst_fp_regs_ext;
        shadow_fp_regs_ext_arr = shadow_options.dst_fp_regs_ext;
        opt_reg_kind = OPT_DREG;
        opt_fp_reg_kind = OPT_FP_DREG_EXT;
        opnd_pos_start = SISA_FIRST_OUTPUT;
        opnd_pos_end = SISA_LAST_OUTPUT;
    }

    for (idx=opnd_pos_start; idx <= opnd_pos_end; idx++) {
        opnd = glb_instr_tlb[op].opnds[idx];
        opnd_type = opnd.type;
        if (SISA_OPND_TYPE_IMM_ZERO_EXTEND == opnd_type) {
            if (opnd.bit2) {
                opnd_type = SISA_OPND_TYPE_IMM_THUMB_EXP;
            } else {
                opnd_type = SISA_OPND_TYPE_IMM;
            }
        }
        switch (opnd_type) {
            case SISA_OPND_TYPE_GPR:
            case SISA_OPND_TYPE_GPRSET:
            case SISA_OPND_TYPE_FPSR:
            case SISA_OPND_TYPE_FPDR:
            case SISA_OPND_TYPE_FPQR:
            case SISA_OPND_TYPE_FPDSR:
            case SISA_OPND_TYPE_FPQDR:
            case SISA_OPND_TYPE_GPR_RW:
                if (reg_idx < MAX_REG_NUM
                    && ! is_fixed_oprand(op, opnd.start_bit, opnd.end_bit)) {
                    if (GET_OPT_STAT(opt_reg_kind)) {
                        if (R_RAND != shadow_regs_arr[reg_idx]) {
                            set_instr_fixed_bit (opnd.start_bit, opnd.end_bit);
                        }
                        chk_3bit_reg (opnd.end_bit - opnd.start_bit + 1, regs_arr[reg_idx]);
                    }
                    raw_instr = set_instr(raw_instr, opnd.start_bit, opnd.end_bit, regs_arr[reg_idx]);
                    if ((SISA_OPND_TYPE_FPSR == opnd_type
                            || SISA_OPND_TYPE_FPDR == opnd_type
                            || SISA_OPND_TYPE_FPQR == opnd_type
                            || SISA_OPND_TYPE_FPDSR == opnd_type
                            || SISA_OPND_TYPE_FPQDR == opnd_type)
                        && ! is_fixed_oprand(op, opnd.bit1, opnd.bit1)) {
                        if (GET_OPT_STAT(opt_fp_reg_kind) && FP_REG_RAND != shadow_fp_regs_ext_arr[reg_idx]) {
                            set_instr_fixed_bit (opnd.bit1, opnd.bit1);
                        }
                        raw_instr = set_instr(raw_instr, opnd.bit1, opnd.bit1, fp_regs_ext_arr[reg_idx]);

                    }
                    if ((SISA_OPND_TYPE_FPDSR == opnd_type
                            || SISA_OPND_TYPE_FPQDR == opnd_type)
                        && ! is_fixed_oprand(op, opnd.bit2, opnd.bit2)) {
                        if (GET_OPT_STAT(OPT_FP_SZ_Q_EXT) && FP_REG_RAND != shadow_options.fp_sz_q_bit) {
                            set_instr_fixed_bit (opnd.bit2, opnd.bit2);
                        }
                        raw_instr = set_instr(raw_instr, opnd.bit2, opnd.bit2, options.fp_sz_q_bit);
                    }
                    reg_idx ++;
                }
                break;
            case SISA_OPND_TYPE_SHIFT_REG:
            case SISA_OPND_TYPE_ROTATE_REG:
                if (! is_fixed_oprand(op, opnd.start_bit, opnd.end_bit)) {
                    if (GET_OPT_STAT(OPT_SHRTREG)) {
                        if (R_RAND != shadow_options.shift_rotate_reg) {
                            set_instr_fixed_bit (opnd.start_bit, opnd.end_bit);
                        }
                        chk_3bit_reg (opnd.end_bit - opnd.start_bit + 1, options.shift_rotate_reg);
                    }
                    raw_instr = set_instr(raw_instr, opnd.start_bit, opnd.end_bit, options.shift_rotate_reg);
                }
                if (! is_fixed_oprand(op, opnd.bit1, opnd.bit2)) {
                    if (GET_OPT_STAT(OPT_SHRTTYPE) && SH_TYPE_RAND != shadow_options.shift_rotate_type) {
                        set_instr_fixed_bit (opnd.bit1, opnd.bit2);
                    }
                    raw_instr = set_instr(raw_instr, opnd.bit1, opnd.bit2, options.shift_rotate_type);
                }
                break;
            case SISA_OPND_TYPE_GPRLIST:
            case SISA_OPND_TYPE_GPRLIST_T:
                if (! is_fixed_oprand(op, opnd.start_bit, opnd.end_bit)) {
                    if (GET_OPT_STAT(OPT_REGS_LIST)) {
                        set_instr_fixed_bit (opnd.start_bit, opnd.end_bit);
                    }
                    raw_instr = set_instr(raw_instr, opnd.start_bit, opnd.end_bit, options.regs_list);
                }
                break;
            case SISA_OPND_TYPE_GPR_MERGE:
                if (reg_idx < MAX_REG_NUM
                    && ! is_fixed_oprand(op, opnd.bit1, opnd.bit1)
                    && ! is_fixed_oprand(op, opnd.start_bit, opnd.end_bit)) {
                    if (GET_OPT_STAT(opt_reg_kind)) {
                        if (R_RAND != shadow_regs_arr[reg_idx]) {
                        set_instr_fixed_bit (opnd.start_bit, opnd.end_bit);
                        set_instr_fixed_bit (opnd.bit1, opnd.bit1);
                        }
                        chk_3bit_reg (opnd.end_bit - opnd.start_bit + 2, regs_arr[reg_idx]);
                    }
                    raw_instr = set_instr(raw_instr, opnd.start_bit, opnd.end_bit, regs_arr[reg_idx]);
                    remainder_bits = regs_arr[reg_idx] >> (opnd.end_bit - opnd.start_bit + 1);
                    raw_instr = set_instr(raw_instr, opnd.bit1, opnd.bit1, remainder_bits);
                    reg_idx ++;
                }
                break;
            case SISA_OPND_TYPE_IMM:
            case SISA_OPND_TYPE_IMM_ARM_EXP:
            case SISA_OPND_TYPE_IMM_SIGN_EXTEND:
                if (! is_fixed_oprand(op, opnd.start_bit, opnd.end_bit)) {
                    if (GET_OPT_STAT(OPT_IMM)) {
                        set_instr_fixed_bit (opnd.start_bit, opnd.end_bit);
                    }
                    raw_instr = set_instr(raw_instr, opnd.start_bit, opnd.end_bit, options.imm_val);
                }
                break;
            case SISA_OPND_TYPE_IMM_MERGE:
            case SISA_OPND_TYPE_IMM_VFP_EXPAND:
                if (! is_fixed_oprand(op, opnd.start_bit, opnd.end_bit)
                    && ! is_fixed_oprand(op, opnd.bit1, opnd.bit2)) {
                    if (GET_OPT_STAT(OPT_IMM)) {
                        set_instr_fixed_bit (opnd.bit1, opnd.bit2);
                        set_instr_fixed_bit (opnd.start_bit, opnd.end_bit);
                    }
                    raw_instr = set_instr(raw_instr, opnd.bit1, opnd.bit2, options.imm_val);
                    remainder_bits = options.imm_val >> (opnd.bit2 - opnd.bit1 + 1);
                    raw_instr = set_instr(raw_instr, opnd.start_bit, opnd.end_bit, remainder_bits);
                }
                break;
            case SISA_OPND_TYPE_SHIFT_IMM:
                if (! is_fixed_oprand(op, opnd.start_bit, opnd.end_bit)) {
                    if (GET_OPT_STAT(OPT_SHIMM)) {
                        set_instr_fixed_bit (opnd.start_bit, opnd.end_bit);
                    }
                    raw_instr = set_instr(raw_instr, opnd.start_bit, opnd.end_bit, options.shift_imm_val);
                }
                if (! is_fixed_oprand(op, opnd.bit1, opnd.bit2)) {
                    if (GET_OPT_STAT(OPT_SHRTTYPE) && SH_TYPE_RAND != shadow_options.shift_rotate_type) {
                        set_instr_fixed_bit (opnd.bit1, opnd.bit2);
                    }
                    raw_instr = set_instr(raw_instr, opnd.bit1, opnd.bit2, options.shift_rotate_type);
                }
                break;
            case SISA_OPND_TYPE_SHIFT_IMM_T:
                if (! is_fixed_oprand(op, opnd.start_bit, opnd.end_bit)
                    && ! is_fixed_oprand(op, 2*opnd.start_bit, 2*opnd.end_bit)) {
                    if (GET_OPT_STAT(OPT_SHIMM)) {
                        set_instr_fixed_bit (opnd.start_bit, opnd.end_bit);
                        set_instr_fixed_bit (2*opnd.start_bit, 2*opnd.end_bit);
                    }
                    raw_instr = set_instr(raw_instr, opnd.start_bit, opnd.end_bit, options.shift_imm_val);
                    remainder_bits = options.shift_imm_val >> (opnd.end_bit - opnd.start_bit + 1);
                    raw_instr = set_instr(raw_instr, 2*opnd.start_bit, 2*opnd.end_bit, remainder_bits);
                }
                if (! is_fixed_oprand(op, opnd.bit1, opnd.bit1)) {
                    if (GET_OPT_STAT(OPT_SHRTTYPE) && SH_TYPE_RAND != shadow_options.shift_rotate_type) {
                        set_instr_fixed_bit (opnd.bit1, opnd.bit1);
                    }
                    raw_instr = set_instr(raw_instr, opnd.bit1, opnd.bit1, options.shift_rotate_type);
                }
                if (! is_fixed_oprand(op, opnd.bit2, opnd.bit2)) {
                    if (GET_OPT_STAT(OPT_SHRTTYPE) && SH_TYPE_RAND != shadow_options.shift_rotate_type) {
                        set_instr_fixed_bit (opnd.bit2, opnd.bit2);
                    }
                    remainder_bits = options.shift_rotate_type >> (opnd.bit2 - opnd.bit2 + 1);
                    raw_instr = set_instr(raw_instr, opnd.bit2, opnd.bit2, remainder_bits);
                }
                break;
            case SISA_OPND_TYPE_IMM_THUMB_EXP:
            case SISA_OPND_TYPE_IMM_SIMD_EXP:
                if (SISA_OPND_TYPE_IMM_THUMB_EXP == opnd_type) {
                    i_bit_pos = 26;
                    imm_startbit = 12;
                    imm_endbit = 14;
                } else if (SISA_OPND_TYPE_IMM_SIMD_EXP == opnd_type) {
                    imm_startbit = 16;
                    imm_endbit = 18;
                    if (op >= SISA_OP_ARM_MAX) {
                        i_bit_pos = 28;
                    } else {
                        i_bit_pos = 24;
                    }
                }
                if (! is_fixed_oprand(op, i_bit_pos, i_bit_pos)
                    && ! is_fixed_oprand(op, imm_startbit, imm_endbit)
                    && ! is_fixed_oprand(op, opnd.start_bit, opnd.end_bit)) {
                    if (GET_OPT_STAT(OPT_IMM)) {
                        set_instr_fixed_bit (opnd.start_bit, opnd.end_bit);
                        set_instr_fixed_bit (imm_startbit, imm_endbit);
                        set_instr_fixed_bit (i_bit_pos, i_bit_pos);
                    }
                    raw_instr = set_instr(raw_instr, opnd.start_bit, opnd.end_bit, options.imm_val);
                    remainder_bits = options.imm_val >> (opnd.end_bit - opnd.start_bit + 1);
                    raw_instr = set_instr(raw_instr, imm_startbit, imm_endbit, remainder_bits);
                    remainder_bits = remainder_bits >> (imm_endbit - imm_startbit + 1);
                    raw_instr = set_instr(raw_instr, i_bit_pos, i_bit_pos, remainder_bits);
                }
                break;
            case SISA_OPND_TYPE_COND:
                if (! is_fixed_oprand(op, opnd.start_bit, opnd.end_bit)) {
                    if (GET_OPT_STAT(OPT_COND)) {
                        set_instr_fixed_bit (opnd.start_bit, opnd.end_bit);
                    }
                    raw_instr = set_instr(raw_instr, opnd.start_bit, opnd.end_bit, options.cond_val);
                }
                break;
            //SISA_OPND_TYPE_GPR_CONST,
            //SISA_OPND_TYPE_FLAGS,
            //SISA_OPND_TYPE_VEC,
            default:
                break;
        }
    }
}

void set_src_opnd_bits (sisa_opcode_t op) {
    set_opnd_bits (op, true);
}

void set_dst_opnd_bits (sisa_opcode_t op) {
    set_opnd_bits (op, false);
}

instr_status_t gen_instr(sisa_opcode_t op) {
    uint32_t orig_instr;
    instr_status_t status = VALID;

    instr_idx = 0;
    is_oprand_init = false;
    is_reg_bit_cut = false;
    memset(instr_buf, 0, MAX_INSTR*(MAX_STR_INSTR_LEN+1)*sizeof(char));

    //filt unsupported op
    if (glb_instr_tlb[op].op_kind == PSEUDO_OP) {
        printf("[WARNING] Unsupported instruction %s\n", glb_instr_tlb[op].name);
        return UNSUPPORTED;
    }  

    set_sbit (op);
    set_arm_cond_bits (op);
    set_src_opnd_bits (op);
    set_dst_opnd_bits (op);

    if (! is_set_fixed_bit) {
        orig_instr = raw_instr;
        is_set_fixed_bit = true;
    }

    fix_unpredictable_bit(op);

    if (op < SISA_OP_ARM_MAX) {
        status = gen_arm_instr (op);
        sprintf(instr_buf[instr_idx], "[INST] .inst 0x%08x", raw_instr);
        instr_idx++;
    } else if (op < SISA_OP_THUMB_MAX) {
        options.isa = ISA_THUMB;
        status = gen_thumb_instr (op);
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

    cur_fixed_bit = (orig_instr & instr_fixed_bit) ^ (raw_instr & instr_fixed_bit);
    if (is_reg_bit_cut) {
        // Special fixed_bit value to indicate 3bit reg with truncated reg No.
        cur_fixed_bit |= 0xFFFF0000;
    }
    return status;
}

sisa_opcode_t gen_random_op() {
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
        idx_max = SISA_OP_THUMB_MAX;
    } else if (options.isa == ISA_THUMB_32){
        idx_min = SISA_OP_THUMB_MAX;
        idx_max = SISA_OP_THUMB_32_MAX;
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

arm_reg_t gen_rand_reg () {
    arm_reg_t rand_reg = R_TOP;
    gpreg_part gp_reg = rand() % (GPR_RAND - GPR_TOP) + GPR_TOP;
    switch (gp_reg) {
        case GPR_0_3:
            rand_reg = rand() % (R_4 - R_0) + R_0;
            break;
        case GPR_4_7:
            rand_reg = rand() % (R_8 - R_4) + R_4;
            break;
        case GPR_8_12:
            rand_reg = rand() % (R_13 - R_8) + R_8;
            break;
        case GPR_13:
            rand_reg = R_SP;
            break;
        case GPR_14:
            rand_reg = R_LR;
            break;
        case GPR_15:
            rand_reg = R_PC;
            break;
        default:
            break;
    }
    return rand_reg;
}

cond_val_t gen_rand_cond () {
    cond_val_t rand_cond = COND_AL;
    cond_part cp_cond = rand() % (CP_RAND - CP_TOP) + CP_TOP;
    switch (cp_cond) {
        case CP_COND:
            rand_cond = rand() % (COND_AL - COND_TOP) + COND_TOP;
            break;
        case CP_AL:
            rand_cond = COND_AL;
            break;
        default:
            break;
    }
    return rand_cond;
}

uint32_t gen_part_imm (int bits) {
    uint32_t part_imm = 0;
    imm_part imm_part_slot = rand() % (IP_RAND - IP_TOP) + IP_TOP;
    switch (imm_part_slot) {
        case IP_ZERO:
            part_imm = 0;
            break;
        case IP_ONE:
            part_imm = 1;
            break;
        case IP_VAL:
            part_imm = rand() % (1 << bits);
            break;
        case IP_CML_ONE:
            part_imm = (~1) & ((1<< bits)-1);
            break;
        case IP_FULL:
            part_imm = (~0) & ((1<< bits)-1);
            break;
        default:
            break;
    }
    return part_imm;
}

uint32_t gen_rand_imm () {
    uint32_t rand_imm = 0;
    imm_part ip_imm = rand() % (IP_RAND - IP_TOP) + IP_TOP;
    switch (ip_imm) {
        case IP_ZERO:
            rand_imm = 0;
            break;
        case IP_ONE:
            rand_imm = 1;
            break;
        case IP_VAL:
            rand_imm = gen_part_imm(8) | (gen_part_imm(4) << 8) | (gen_part_imm(20)<< 12);
            break;
        case IP_CML_ONE:
            rand_imm = ~1;
            break;
        case IP_FULL:
            rand_imm = ~0;
            break;
        default:
            break;
    }

    return rand_imm;
}

void gen_rand_opt_val () {
    int idx = 0;

    if (! GET_OPT_STAT(OPT_ISA) || ISA_RAND == options.isa) {
        options.isa = rand() % (ISA_RAND - ISA_TOP) + ISA_TOP;
    }

    if (! GET_OPT_STAT(OPT_SBIT) || SBIT_RAND == options.sbit_val) {
        options.sbit_val = rand() % (SBIT_RAND - SBIT_TOP) + SBIT_TOP;
    }

    if (! GET_OPT_STAT(OPT_COND) || COND_RAND == options.cond_val) {
        options.cond_val = gen_rand_cond();
    }

    for (idx = 0; idx < MAX_REG_NUM; idx++) {
        if (! GET_OPT_STAT(OPT_SREG) || R_RAND == options.src_regs[idx]) {
            options.src_regs[idx] = gen_rand_reg();
        }

        if (! GET_OPT_STAT(OPT_DREG) || R_RAND == options.dst_regs[idx]) {
            options.dst_regs[idx] = gen_rand_reg ();
        }

        if (! GET_OPT_STAT(OPT_FP_SREG_EXT) || FP_REG_RAND == options.src_fp_regs_ext[idx]) {
            options.src_fp_regs_ext[idx] = rand() % (FP_REG_RAND - FP_REG_TOP) + FP_REG_TOP;
        }

        if (! GET_OPT_STAT(OPT_FP_DREG_EXT) || FP_REG_RAND == options.dst_fp_regs_ext[idx]) {
            options.dst_fp_regs_ext[idx] = rand() % (FP_REG_RAND - FP_REG_TOP) + FP_REG_TOP;
        }
    }

    if (! GET_OPT_STAT(OPT_FP_SZ_Q_EXT) || FP_SZ_Q_RAND== options.fp_sz_q_bit) {
            options.fp_sz_q_bit = rand() % (FP_SZ_Q_RAND - FP_SZ_Q_TOP) + FP_SZ_Q_TOP;
    }

    if (! GET_OPT_STAT(OPT_SHRTREG) || R_RAND == options.shift_rotate_reg) {
        options.shift_rotate_reg = gen_rand_reg ();
    }

    if (! GET_OPT_STAT(OPT_SHRTTYPE) || SH_TYPE_RAND == options.shift_rotate_type) {
        options.shift_rotate_type = rand() % (SH_TYPE_RAND - SH_TYPE_TOP) + SH_TYPE_TOP;
    }

    if (! GET_OPT_STAT(OPT_REGS_LIST)) {
        options.regs_list = rand() & REGS_LIST_MASK;
    }

    if (! GET_OPT_STAT(OPT_IMM)) {
        options.imm_val = gen_rand_imm();
    }

    if (! GET_OPT_STAT(OPT_SHIMM)) {
        options.shift_imm_val = gen_rand_imm();
    }
}

void init_raw_instr_bin (sisa_opcode_t op) {

    if (! GET_OPT_STAT(OPT_INSTR_BIN_INIT)) {
        raw_instr = rand();
    }
    if (op > SISA_OP_ARM_MAX && op < SISA_OP_THUMB_MAX) {
        raw_instr &= MAX_UINT16;
    }
    raw_instr = (raw_instr & (~glb_instr_tlb[op].match_msk)) | glb_instr_tlb[op].match_val;
}

void relax_restrict (int cur_retry) {
    if (GET_OPT_STAT(OPT_NOT_ALLOW_RELAX)) {
        return;
    }
    int idx = 0;

    if (cur_retry > RETRY_RELAX_GEAR_1) {
        SET_OPT_STAT(OPT_COND);
        options.cond_val = COND_RAND;

        SET_OPT_STAT(OPT_SHRTREG);
        options.shift_rotate_reg = R_RAND;
    }

    if (cur_retry > RETRY_RELAX_GEAR_2) {
        SET_OPT_STAT(OPT_SBIT);
        options.sbit_val = SBIT_RAND;

        SET_OPT_STAT(OPT_SHRTTYPE);
        options.shift_rotate_type = SH_TYPE_RAND;

        SET_OPT_STAT(OPT_FP_SREG_EXT);
        for (idx = 0; idx < MAX_REG_NUM; idx++) {
            options.src_fp_regs_ext[idx] = FP_REG_RAND;
        }

        SET_OPT_STAT(OPT_SHIMM);
        options.shift_imm_val = rand();
    }

    if (cur_retry > RETRY_RELAX_GEAR_3) {
        SET_OPT_STAT(OPT_SREG);
        for (idx = 0; idx < MAX_REG_NUM; idx++) {
            options.src_regs[idx] = R_RAND;
        }

        SET_OPT_STAT(OPT_FP_DREG_EXT);
        for (idx = 0; idx < MAX_REG_NUM; idx++) {
            options.dst_fp_regs_ext[idx] = FP_REG_RAND;
        }

        SET_OPT_STAT(OPT_IMM);
        options.imm_val = rand();
    }   

    if (cur_retry > RETRY_RELAX_GEAR_4) {
        SET_OPT_STAT(OPT_DREG);
        for (idx = 0; idx < MAX_REG_NUM; idx++) {
            options.dst_regs[idx] = R_RAND;
        }

        SET_OPT_STAT(OPT_FP_SZ_Q_EXT);
        options.fp_sz_q_bit = FP_REG_RAND;

        SET_OPT_STAT(OPT_REGS_LIST);
        options.regs_list = rand() % 0x10000;
    }
}

instr_status_t gen_random_instr() {
    uint32_t idx;
    int retry = 0;
    int retry_max = RETRY_MAX_TIMES;
    instr_status_t status = UNSUPPORTED;
    sisa_opcode_t op = SISA_OP_MAX;
    shadow_options = options;
    instr_fixed_bit = 0;
    is_set_fixed_bit = false;

    if (GET_OPT_STAT(OPT_NOT_ALLOW_RELAX)) {
        retry_max = retry_max / 2;
    }
    if (! GET_OPT_STAT(OPT_RAND_SEED)) {
        struct timeval tv;
        gettimeofday(&tv, NULL);
        srand (tv.tv_sec + tv.tv_usec);
    }

    if (GET_OPT_STAT(OPT_OPCODE)) {
        for(idx = 1; idx < SISA_OP_MAX; idx++) {
            if (glb_instr_tlb[idx].name && strcmp(glb_instr_tlb[idx].name, options.opcode) == 0) {
                if (idx > SISA_OP_ARM_MAX) {
                    options.isa = ISA_THUMB;
                }
                op = idx;
                break;
            }
        }
        if (SISA_OP_MAX <= op) {
            printf("[WARNING] Fail to find opcode for %s\n", options.opcode);
            return status;
        }
    }

    while (status != VALID && retry <= retry_max) {
        options = shadow_options;
        relax_restrict (retry);
        gen_rand_opt_val ();
        if (!GET_OPT_STAT(OPT_OPCODE)) {
            op = gen_random_op();
        }
        init_raw_instr_bin(op);
        status = gen_instr(op);
        retry++;
    }

    output_instr(op, raw_instr, status);
    return status;
}

void update_opkind_str(const char *str_opkind) {
    uint32_t i;
    const char *p_s = str_opkind;
    const char *p_e = ",";
    for (i = 0; i < opkind_num; i++) {
        opkind_list[i] = false;
    }
    
    while ((*p_e == ',') && (*p_s != '\0')) {
        p_e = strchr(p_s, ',');
        if (!p_e) {
            p_e = p_s + strlen(p_s);
        }
        for (i = 0; i < opkind_num; i++) {
            if (strncmp(p_s, opkind_names[i], p_e - p_s) == 0) {
                opkind_list[i] = true;
            }
        }
        p_s = p_e + 1;
    }
}
