#include <stdio.h>
#include <string.h>
#include "GenThumb32Instr.h"
#include "InstrUtil.h"
#include "GenHelper.h"
#include "RandomInstrGen.h"

extern gen_option_t options;
static char gen_instr_buf[MAX_STR_INSTR_LEN+1];

instr_status_t gen_thumb32_instr(sisa_opcode_t op)
{
    instr_status_t status = VALID;
    memset(gen_instr_buf, 0 , (MAX_STR_INSTR_LEN+1)*sizeof(char));
    switch (op) {
        //multi-mem
        case SISA_OP_LDM_T2:
        case SISA_OP_LDMDB_T1:
        case SISA_OP_POP_T2:
        case SISA_OP_STM_T2:
        case SISA_OP_STMDB_T1:
        case SISA_OP_PUSH_T2:
            status = gen_thumb32_multi_mem(op);
            break;
        //single-mem
        case SISA_OP_LDR_imm_T3:
        case SISA_OP_LDR_imm_T4:
        case SISA_OP_LDRD_imm_T1:
        case SISA_OP_LDRB_imm_T2:
        case SISA_OP_LDRB_imm_T3:
        case SISA_OP_LDRH_imm_T2:
        case SISA_OP_LDRH_imm_T3:
        case SISA_OP_LDRSB_imm_T1:
        case SISA_OP_LDRSB_imm_T2:
        case SISA_OP_LDRSH_imm_T1:
        case SISA_OP_LDRSH_imm_T2:
        case SISA_OP_POP_T3/*single*/:
        case SISA_OP_LDRT_T1:
        case SISA_OP_LDRBT_T1:
        case SISA_OP_LDRHT_T1:
        case SISA_OP_LDRSBT_T1:
        case SISA_OP_LDRSHT_T1:
            status = gen_thumb32_ld_imm(op);
            break;
        case SISA_OP_LDR_reg_T2:
        case SISA_OP_LDRB_reg_T2:
        case SISA_OP_LDRH_reg_T2:
        case SISA_OP_LDRSB_reg_T2:
        case SISA_OP_LDRSH_reg_T2:
            status = gen_thumb32_mem_reg(op);
            break;
        case SISA_OP_STR_imm_T3:
        case SISA_OP_STR_imm_T4:
        case SISA_OP_STRD_imm_T1:
        case SISA_OP_STRB_imm_T2:
        case SISA_OP_STRB_imm_T3:
        case SISA_OP_STRH_imm_T2:
        case SISA_OP_STRH_imm_T3:
        case SISA_OP_STRT_T1:
        case SISA_OP_STRBT_T1:
        case SISA_OP_STRHT_T1:
        case SISA_OP_PUSH_T3/*single*/:
            status = gen_thumb32_st_imm(op);
            break;
        case SISA_OP_STR_reg_T2:
        case SISA_OP_STRB_reg_T2:
        case SISA_OP_STRH_reg_T2:
            status = gen_thumb32_mem_reg(op);
            break;
        //mem-lit
        case SISA_OP_LDR_lit_T2:
        case SISA_OP_LDRD_lit_T1:
        case SISA_OP_LDRB_lit_T1:
        case SISA_OP_LDRH_lit_T1:
        case SISA_OP_LDRSB_lit_T1:
        case SISA_OP_LDRSH_lit_T1:
            status = gen_thumb32_mem_lit(op);
            break;
        //mem-ex
        case SISA_OP_STREX_T1:
        case SISA_OP_STREXB_T1:
        case SISA_OP_STREXH_T1:
        case SISA_OP_STREXD_T1:
        case SISA_OP_LDREX_T1:
        case SISA_OP_LDREXB_T1:
        case SISA_OP_LDREXH_T1:
        case SISA_OP_LDREXD_T1:
            status= gen_thumb32_mem_ex(op);
            break;
        //preload
        case SISA_OP_PLD_lit_T1:
        case SISA_OP_PLD_imm_T1:
        case SISA_OP_PLD_imm_T2:
        case SISA_OP_PLD_reg_T1:
        case SISA_OP_PLI_lit_T3:
        case SISA_OP_PLI_imm_T1:
        case SISA_OP_PLI_imm_T2:
        case SISA_OP_PLI_reg_T1:
        case SISA_OP_PLDW_imm_T1:
        case SISA_OP_PLDW_imm_T2:
        case SISA_OP_PLDW_reg_T1:
            status = gen_thumb32_mem_preload(op);
            break;
        //data-processing (shifted register)
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
        case SISA_OP_SUB_reg_T2:
        case SISA_OP_CMP_reg_T3:
        case SISA_OP_RSB_reg_T1:
        case SISA_OP_LSL_imm_T2:
        case SISA_OP_LSR_imm_T2:
        case SISA_OP_ASR_imm_T2:
        case SISA_OP_ROR_imm_T2:
        case SISA_OP_RRX_T1:
            status = gen_thumb32_data_shifted_reg(op);
            break;
        //data-processing (modified imm)
        case SISA_OP_AND_imm_T1:
        case SISA_OP_TST_imm_T1:
        case SISA_OP_BIC_imm_T1:
        case SISA_OP_ORR_imm_T1:
        case SISA_OP_MOV_imm_T2:
        case SISA_OP_ORN_imm_T1:
        case SISA_OP_MVN_imm_T1:
        case SISA_OP_EOR_imm_T1:
        case SISA_OP_TEQ_imm_T1:
        case SISA_OP_ADD_imm_T3:
        case SISA_OP_ADD_sp_imm_T3:
        case SISA_OP_CMN_imm_T1:
        case SISA_OP_ADC_imm_T1:
        case SISA_OP_SBC_imm_T1:
        case SISA_OP_SUB_imm_T3:
        case SISA_OP_SUB_sp_imm_T2:
        case SISA_OP_CMP_imm_T2:
        case SISA_OP_RSB_imm_T2:
            status = gen_thumb32_data_mimm(op);
            break;
        //data-procsssing (plain binary imm)
        case SISA_OP_ADD_imm_T4:
        case SISA_OP_ADD_sp_imm_T4:
        case SISA_OP_ADR_T3:
        case SISA_OP_MOV_imm_T3:
        case SISA_OP_SUB_imm_T4:
        case SISA_OP_SUB_sp_imm_T3:
        case SISA_OP_ADR_T2:
        case SISA_OP_MOVT_T1:
        case SISA_OP_SSAT_T1:
        case SISA_OP_SSAT16_T1:
        case SISA_OP_SBFX_T1:
        case SISA_OP_BFI_T1:
        case SISA_OP_BFC_T1:
        case SISA_OP_USAT_T1:
        case SISA_OP_USAT16_T1:
        case SISA_OP_UBFX_T1:
            status = gen_thumb32_data_pimm(op);
            break;
        //data-processing (register)
        case SISA_OP_LSL_reg_T2:
        case SISA_OP_LSR_reg_T2:
        case SISA_OP_ASR_reg_T2:
        case SISA_OP_ROR_reg_T2:
        case SISA_OP_SXTAH_T1:
        case SISA_OP_SXTH_T2:
        case SISA_OP_UXTAH_T1:
        case SISA_OP_UXTH_T2:
        case SISA_OP_SXTAB16_T1:
        case SISA_OP_SXTB16_T1:
        case SISA_OP_UXTAB16_T1:
        case SISA_OP_UXTB16_T1:
        case SISA_OP_SXTAB_T1:
        case SISA_OP_SXTB_T2:
        case SISA_OP_UXTAB_T1:
        case SISA_OP_UXTB_T2:
            status = gen_thumb32_data_reg(op);
            break;
        //parallel add/sub
        case SISA_OP_SADD16_T1:
        case SISA_OP_SASX_T1:
        case SISA_OP_SSAX_T1:
        case SISA_OP_SSUB16_T1:
        case SISA_OP_SADD8_T1:
        case SISA_OP_SSUB8_T1:
        case SISA_OP_QADD16_T1:
        case SISA_OP_QASX_T1:
        case SISA_OP_QSAX_T1:
        case SISA_OP_QSUB16_T1:
        case SISA_OP_QADD8_T1:
        case SISA_OP_QSUB8_T1:
        case SISA_OP_SHADD16_T1:
        case SISA_OP_SHASX_T1:
        case SISA_OP_SHSAX_T1:
        case SISA_OP_SHSUB16_T1:
        case SISA_OP_SHADD8_T1:
        case SISA_OP_SHSUB8_T1:
        case SISA_OP_UADD16_T1:
        case SISA_OP_UASX_T1:
        case SISA_OP_USAX_T1:
        case SISA_OP_USUB16_T1:
        case SISA_OP_UADD8_T1:
        case SISA_OP_USUB8_T1:
        case SISA_OP_UQADD16_T1:
        case SISA_OP_UQASX_T1:
        case SISA_OP_UQSAX_T1:
        case SISA_OP_UQSUB16_T1:
        case SISA_OP_UQADD8_T1:
        case SISA_OP_UQSUB8_T1:
        case SISA_OP_UHADD16_T1:
        case SISA_OP_UHASX_T1:
        case SISA_OP_UHSAX_T1:
        case SISA_OP_UHSUB16_T1:
        case SISA_OP_UHADD8_T1:
        case SISA_OP_UHSUB8_T1:
        case SISA_OP_QADD_T1:
        case SISA_OP_QDADD_T1:
        case SISA_OP_QSUB_T1:
        case SISA_OP_QDSUB_T1:
        case SISA_OP_SEL_T1:
            status = gen_thumb32_data_parallel(op);
            break;
        //miscellaneous
        case SISA_OP_REV_T2:
        case SISA_OP_REV16_T2:
        case SISA_OP_RBIT_T1:
        case SISA_OP_REVSH_T2:
        case SISA_OP_CLZ_T1:
            status = gen_thumb32_data_misc(op);
            break;
        //mul
        case SISA_OP_MLA_T1:
        case SISA_OP_MUL_T2:
        case SISA_OP_MLS_T1:
        case SISA_OP_SMLABB_T1:
        case SISA_OP_SMULBB_T1:
        case SISA_OP_SMLAD_T1:
        case SISA_OP_SMUAD_T1:
        case SISA_OP_SMLAWB_T1:
        case SISA_OP_SMULWB_T1:
        case SISA_OP_SMLSD_T1:
        case SISA_OP_SMUSD_T1:
        case SISA_OP_SMMLA_T1:
        case SISA_OP_SMMUL_T1:
        case SISA_OP_SMMLS_T1:
        case SISA_OP_USAD8_T1:
        case SISA_OP_USADA8_T1:
        //long mul / div
        case SISA_OP_SMULL_T1:
        case SISA_OP_SDIV_T1:
        case SISA_OP_UMULL_T1:
        case SISA_OP_UDIV_T1:
        case SISA_OP_SMLAL_T1:
        case SISA_OP_SMLALBB_T1:
        case SISA_OP_SMLALD_T1:
        case SISA_OP_SMLSLD_T1:
        case SISA_OP_UMLAL_T1:
        case SISA_OP_UMAAL_T1:
            status = gen_thumb32_data_mul(op);
            break;
        //sys-level
        case SISA_OP_SRS_T1:
        case SISA_OP_SRS_T2:
        case SISA_OP_RFE_T1:
        case SISA_OP_RFE_T2:
        case SISA_OP_MSR_s_T1:
        case SISA_OP_CPS_T2:
        case SISA_OP_SUBS_T1:
        case SISA_OP_MRS_s_T1:
        //case SISA_OP_SMC_T1:
            status = UNSUPPORTED;
            break;
        //TODO
        //coprocessor
        case SISA_OP_STC_T1:
        case SISA_OP_STC2_T2:
        case SISA_OP_LDC_imm_T1:
        case SISA_OP_LDC2_imm_T2:
        case SISA_OP_LDC_lit_T1:
        case SISA_OP_LDC2_lit_T2:
        case SISA_OP_MCR_reg_T1:
        case SISA_OP_MCR2_T2:
        case SISA_OP_MCRR_T1:
        case SISA_OP_MCRR2_T2:
        case SISA_OP_MRC_reg_T1:
        case SISA_OP_MRC2_T2:
        case SISA_OP_MRRC_T1:
        case SISA_OP_MRRC2_T2:
        case SISA_OP_CDP_T1:
        case SISA_OP_CDP2_T2:
            status = UNSUPPORTED;
            break;
        //branch
        case SISA_OP_B_T3:
        case SISA_OP_B_T4:
        case SISA_OP_BL_imm_T1:
        case SISA_OP_BLX_imm_T2:
        case SISA_OP_TBB_T1:
        case SISA_OP_TBH_T1:
            status = UNSUPPORTED;
            break;
        //msr / mrs
        case SISA_OP_MSR_a_T1:
        case SISA_OP_MRS_a_T1:
            status = gen_thumb32_sr(op);
            break;
        //hint
        case SISA_OP_NOP_T2:
        case SISA_OP_YIELD_T2:
        case SISA_OP_WFE_T2:
        case SISA_OP_WFI_T2:
        case SISA_OP_SEV_T2:
        case SISA_OP_DBG_T1:
        case SISA_OP_CLREX_T1:
        case SISA_OP_DSB_T1:
        case SISA_OP_DMB_T1:
        case SISA_OP_ISB_T1:
            status = gen_thumb32_hint(op);
            break;
        //others
        //case SISA_OP_ENTERX_T1:
        //case SISA_OP_LEAVEX_T1:
        case SISA_OP_BXJ_reg_T1:
            status = UNSUPPORTED;
            break;
        default:
            status = UNSUPPORTED;
            break;
    }
    return status;
}

static inline bool BadReg(uint32_t n)
{
    return n == R_SP || n == R_PC;
}

instr_status_t gen_thumb32_data_shifted_reg(sisa_opcode_t op)
{
    instr_status_t status = VALID;
    uint32_t instr = get_raw_instr();
    bool s = get_bit_value(instr, 20);
    bool t = get_bit_value(instr, 4);
    uint32_t n = get_bits_value(instr, 16, 19);
    uint32_t d = get_bits_value(instr, 8, 11);
    uint32_t m = get_bits_value(instr, 0, 3);

    switch (op) {
        case SISA_OP_AND_reg_T2:
        case SISA_OP_EOR_reg_T2:
            if (s && d == R_PC) {
                status = ALIAS;
            }
            if (d == R_SP || (d == R_PC && s == 0) || BadReg(n) || BadReg(m)) {
                status = UNPREDICTABLE;
            }
            break;
        case SISA_OP_TST_reg_T2:
        case SISA_OP_TEQ_reg_T1:
            if (BadReg(n) || BadReg(m)) {
                status = UNPREDICTABLE;
            }
            break;
        case SISA_OP_MVN_reg_T2:
            if (BadReg(d) || BadReg(m)) {
                status = UNPREDICTABLE;
            }
            break;
        case SISA_OP_BIC_reg_T2:
        case SISA_OP_ADC_reg_T2:
        case SISA_OP_SBC_reg_T2:
        case SISA_OP_RSB_reg_T1:
            if (BadReg(d) || BadReg(n) || BadReg(m)) {
                status = UNPREDICTABLE;
            }
            break;
        case SISA_OP_ORR_reg_T2:
        case SISA_OP_ORN_reg_T1:
            if (n == R_PC) {
                status = ALIAS;
            }
            if (BadReg(d) || n == R_SP || BadReg(m)) {
                status = UNPREDICTABLE;
            }
            break;
        case SISA_OP_PKH_reg_T1:
            if (s == 1 || t == 1) {
                status = UNDEFINED;
            }
            if (BadReg(d) || BadReg(n) || BadReg(m)) {
                status = UNPREDICTABLE;
            }
            break;
        case SISA_OP_ADD_reg_T3:
        case SISA_OP_SUB_reg_T2:
            if ((s && d == R_PC) || n == R_SP) {
                status = ALIAS;
            }
            if (d == R_SP || (d == R_PC && s == 0) || n == R_PC || BadReg(m)) {
                status = UNPREDICTABLE;
            }
            break;
        case SISA_OP_CMN_reg_T2:
        case SISA_OP_CMP_reg_T3:
            if (n == R_PC || BadReg(m)) {
                status = UNPREDICTABLE;
            }
            break;
        case SISA_OP_MOV_reg_T3:
            if (s && (BadReg(d) || BadReg(m))) {
                status = UNPREDICTABLE;
            }
            if (s == 0 && (d == R_PC || m == R_PC || (d == R_SP && m == R_SP))) {
                status = UNPREDICTABLE;
            }
            break;
        case SISA_OP_LSL_imm_T2:
        case SISA_OP_LSR_imm_T2:
        case SISA_OP_ASR_imm_T2:
        case SISA_OP_ROR_imm_T2:
        case SISA_OP_RRX_T1:
            if (BadReg(d) || BadReg(m)) {
                status = UNPREDICTABLE;
            }
            break;
        default:
            status = UNSUPPORTED;
            break;
    }
    return status;
}


instr_status_t gen_thumb32_data_mimm(sisa_opcode_t op)
{
    instr_status_t status = VALID;
    uint32_t instr = get_raw_instr();
    uint32_t s = get_bit_value(instr, 20);
    uint32_t n = get_bits_value(instr, 16, 19);
    uint32_t d = get_bits_value(instr, 8, 11);
    uint32_t i = get_bit_value(instr, 26);
    uint32_t imm3 = get_bits_value(instr, 12, 14);
    uint32_t imm8 = get_bits_value(instr, 0, 7);
    if ((i == 0) && (imm8 == 0)) {
        if (imm3 > 0 && imm3 < 4) {
            status = UNPREDICTABLE;
        }
    }
    switch (op) {
        case SISA_OP_AND_imm_T1:
        case SISA_OP_EOR_imm_T1:
            if (s && d == R_PC) {
                status = ALIAS;
            }
            if (d == R_SP || (d == R_PC && s == 0) || BadReg(n)) {
                status = UNPREDICTABLE;
            }
            break;
        case SISA_OP_ADD_imm_T3:
        case SISA_OP_SUB_imm_T3:
            if (n == R_SP || (s && d == R_PC)) {
                status = ALIAS;
            }
            if (d == R_SP || (d == R_PC && s == 0) || n == R_PC) {
                status = UNPREDICTABLE;
            }
            break;
        case SISA_OP_ADD_sp_imm_T3:
        case SISA_OP_SUB_sp_imm_T2:
            if (s && d == R_PC) {
                status = ALIAS;
            }
            if (d == R_PC && s == 0) {
                status = UNPREDICTABLE;
            }
            break;
        case SISA_OP_TST_imm_T1:
        case SISA_OP_TEQ_imm_T1:
            if (BadReg(n)) {
                status = UNPREDICTABLE;
            }
            break;
        case SISA_OP_BIC_imm_T1:
        case SISA_OP_ADC_imm_T1:
        case SISA_OP_SBC_imm_T1:
        case SISA_OP_RSB_imm_T2:
            if (BadReg(d) || BadReg(n)) {
                status = UNPREDICTABLE;
            }
            break;
        case SISA_OP_ORR_imm_T1:
        case SISA_OP_ORN_imm_T1:
            if (BadReg(d) || n == R_SP) {
                status = UNPREDICTABLE;
            }
            break;
        case SISA_OP_MOV_imm_T2:
        case SISA_OP_MVN_imm_T1:
            if (BadReg(d)) {
                status = UNPREDICTABLE;
            }
            break;
        case SISA_OP_CMN_imm_T1:
        case SISA_OP_CMP_imm_T2:
            if (n == R_PC) {
                status = UNPREDICTABLE;
            }
            break;
        default:
            status = UNSUPPORTED;
            break;
    }
    return status;
}

instr_status_t gen_thumb32_data_pimm(sisa_opcode_t op)
{
    instr_status_t status = VALID;
    uint32_t instr = get_raw_instr();
    uint32_t n = get_bits_value(instr, 16, 19);
    uint32_t d = get_bits_value(instr, 8, 11);
    uint32_t imm1 = get_bits_value(instr, 0, 4);
    uint32_t imm2 = get_bits_value(instr, 6, 7) + (get_bits_value(instr, 12, 14) << 2);

    switch (op) {
        case SISA_OP_ADR_T3:
        case SISA_OP_ADR_T2:
        case SISA_OP_MOV_imm_T3:
        case SISA_OP_MOVT_T1:
            if (BadReg(d)) {
                status = UNPREDICTABLE;
            }
            break;
        case SISA_OP_ADD_imm_T4:
        case SISA_OP_SUB_imm_T4:
            if (n == R_SP || n == R_PC) {
                status = ALIAS;
            }
            if (BadReg(d)) {
                status = UNPREDICTABLE;
            }
            break;
        case SISA_OP_ADD_sp_imm_T4:
        case SISA_OP_SUB_sp_imm_T3:
            if (d == R_PC) {
                status = UNPREDICTABLE;
            }
            break;
        case SISA_OP_SSAT_T1:
        case SISA_OP_USAT_T1:
            imm1 = get_bits_value(instr, 6, 7);
            imm2 = get_bits_value(instr, 12, 14);
            if (get_bit_value(instr, 21) && (imm1 == 0) && (imm2 == 0)) {
                status = ALIAS;
            }
            if (BadReg(d) || BadReg(n)) {
                status = UNPREDICTABLE;
            }
            break;
        case SISA_OP_SSAT16_T1:
        case SISA_OP_USAT16_T1:
            if (BadReg(d) || BadReg(n)) {
                status = UNPREDICTABLE;
            }
            break;
        case SISA_OP_BFI_T1:
        case SISA_OP_BFC_T1:
            if (BadReg(d) || n == R_SP) {
                status = UNPREDICTABLE;
            }
            if (imm1 < imm2) {
                status = UNPREDICTABLE;
            }
            break;
        case SISA_OP_SBFX_T1:
        case SISA_OP_UBFX_T1:
            if (BadReg(d) || BadReg(n)) {
                status = UNPREDICTABLE;
            }
            if (imm1 + imm2 > 31) {
                status = UNPREDICTABLE;
            }
            break;
        default:
            status = UNSUPPORTED;
            break;
    }
    return status;
}

instr_status_t gen_thumb32_data_reg(sisa_opcode_t op)
{
    instr_status_t status = VALID;
    uint32_t instr = get_raw_instr();
    uint32_t d = get_bits_value(instr, 8, 11);
    uint32_t m = get_bits_value(instr, 0, 3);
    uint32_t n = get_bits_value(instr, 16, 19);

    if (op == SISA_OP_LSL_reg_T2 || op == SISA_OP_LSR_reg_T2 ||
            op == SISA_OP_ASR_reg_T2 || op == SISA_OP_ROR_reg_T2) {
        if (BadReg(d) || BadReg(n) || BadReg(m)) {
            status = UNPREDICTABLE;
        }
    } else if (op == SISA_OP_SXTAH_T1 || op == SISA_OP_UXTAH_T1 || op == SISA_OP_SXTAB16_T1 ||
            op == SISA_OP_UXTAB16_T1 || op == SISA_OP_SXTAB_T1 || op == SISA_OP_UXTAB_T1) {
        if (n == R_PC) {
            status = ALIAS;
        }
        if (BadReg(d) || n == R_SP || BadReg(m)) {
            status = UNPREDICTABLE;
        }
    } else {
        if (BadReg(d) || BadReg(m)) {
            status = UNPREDICTABLE;
        }
    }
    return status;
}


instr_status_t gen_thumb32_data_parallel(sisa_opcode_t op)
{
    instr_status_t status = VALID;
    uint32_t instr = get_raw_instr();
    uint32_t n = get_bits_value(instr, 16, 19);
    uint32_t d = get_bits_value(instr, 8, 11);
    uint32_t m = get_bits_value(instr, 0, 3);
    if (BadReg(d) || BadReg(n) || BadReg(m)) {
        status = UNPREDICTABLE;
    }
    return status;
}

instr_status_t gen_thumb32_data_misc(sisa_opcode_t op)
{
    instr_status_t status = VALID;
    uint32_t instr = get_raw_instr();
    uint32_t d = get_bits_value(instr, 8, 11);
    uint32_t m = get_bits_value(instr, 0, 3);

    //if !Consistent(Rm) then UNPREDICTABLE
    instr = set_instr(instr, 16, 19, m);
    set_raw_instr(instr);

    if (BadReg(d) || BadReg(m)) {
        status = UNPREDICTABLE;
    }
    return status;
}

instr_status_t gen_thumb32_data_mul(sisa_opcode_t op)
{
    instr_status_t status = VALID;
    uint32_t instr = get_raw_instr();
    uint32_t m = get_bits_value(instr, 0, 3);
    uint32_t d = get_bits_value(instr, 8, 11);
    uint32_t a = get_bits_value(instr, 12, 15);
    uint32_t n = get_bits_value(instr, 16, 19);

    switch (op) {
        case SISA_OP_MLA_T1:
        case SISA_OP_SMLABB_T1:
        case SISA_OP_SMLAD_T1:
        case SISA_OP_SMLAWB_T1:
        case SISA_OP_SMLSD_T1:
        case SISA_OP_SMMLA_T1:
        case SISA_OP_USADA8_T1:
            if (a == R_PC) {
                status = ALIAS;
            }
            if (BadReg(d) || BadReg(n) || BadReg(m) || a == R_SP) {
                status = UNPREDICTABLE;
            }
            break;
        case SISA_OP_MUL_T2:
        case SISA_OP_SMULBB_T1:
        case SISA_OP_SMUAD_T1:
        case SISA_OP_SMULWB_T1:
        case SISA_OP_SMUSD_T1:
        case SISA_OP_SMMUL_T1:
        case SISA_OP_USAD8_T1:
            if (BadReg(d) || BadReg(n) || BadReg(m)) {
                status = UNPREDICTABLE;
            }
            break;
        case SISA_OP_MLS_T1:
        case SISA_OP_SMMLS_T1:
            if (BadReg(d) || BadReg(n) || BadReg(m) || BadReg(a)) {
                status = UNPREDICTABLE;
            }
            break;
        case SISA_OP_SMULL_T1:
        case SISA_OP_UMULL_T1:
        case SISA_OP_SMLAL_T1:
        case SISA_OP_SMLALBB_T1:
        case SISA_OP_SMLALD_T1:
        case SISA_OP_SMLSLD_T1:
        case SISA_OP_UMLAL_T1:
        case SISA_OP_UMAAL_T1:
            if (BadReg(d) || BadReg(n) || BadReg(m) || BadReg(a)) {
                status = UNPREDICTABLE;
            }
            if (d == a) {
                status = UNPREDICTABLE;
            }
            break;
        case SISA_OP_SDIV_T1:
        case SISA_OP_UDIV_T1:
            //ARMv7-R, not supported yet
            /*
            if (BadReg(m) || BadReg(d) || BadReg(n)) {
                status = UNPREDICTABLE;
            }*/
            status = UNSUPPORTED;
            break;
        default:
            status = UNSUPPORTED;
            break;
    }

    return status;
}

instr_status_t gen_thumb32_multi_mem(sisa_opcode_t op)
{
    instr_status_t status = VALID;
    uint32_t instr = get_raw_instr();
    uint32_t n = get_bits_value(instr, 16, 19);
    bool wback = get_bit_value(instr, 21);
    if (n == R_PC || bit_count(instr & 0xffff) < 2) {
        status = UNPREDICTABLE;
    }
    if (wback && get_bit_value(instr, n)) {
        status = UNPREDICTABLE;
    }

    if (op == SISA_OP_LDM_T2 || op == SISA_OP_LDMDB_T1 || op == SISA_OP_POP_T2) {
        bool p = get_bit_value(instr, 15);
        bool m = get_bit_value(instr, 14);
        if (p && m) {
            status = UNPREDICTABLE;
        }

        //LoadWritePC
        if (p) {
            status = UNSUPPORTED;
        }
    }
    if (! GET_OPT_STAT(OPT_NOT_ALLOW_FIX_MEM)) {
        fix_mem_base(n, true);
    } else {
        gen_pre_instr_for_mem_base(n);
    }

    return status;
}

instr_status_t gen_thumb32_ld_imm(sisa_opcode_t op)
{
    instr_status_t status = VALID;
    uint32_t instr = get_raw_instr();
    uint32_t n = get_bits_value(instr, 16, 19);
    uint32_t t = get_bits_value(instr, 12, 15);
    bool p, u, w;
    uint32_t t2;

    //literal or LoadWritePC
    if (n == R_PC || t == R_PC) {
        status = UNSUPPORTED;
    }

    switch(op) {
        case SISA_OP_LDR_imm_T3:
            break;
        case SISA_OP_LDR_imm_T4:
            p = get_bit_value(instr, 10);
            u = get_bit_value(instr, 9);
            w = get_bit_value(instr, 8);
            if (p && u && (w == 0)) {
                status = ALIAS;
            }
            if (p == 0 && w == 0) {
                status = UNDEFINED;
            }
            if (w && n == t) {
                status = UNPREDICTABLE;
            }
            break;
        case SISA_OP_LDRB_imm_T2:
        case SISA_OP_LDRH_imm_T2:
        case SISA_OP_LDRSB_imm_T1:
        case SISA_OP_LDRSH_imm_T1:
            if (t == R_PC) {
                status = ALIAS;
            }
            if (t == R_SP) {
                status = UNPREDICTABLE;
            }
            break;
        case SISA_OP_LDRB_imm_T3:
        case SISA_OP_LDRH_imm_T3:
        case SISA_OP_LDRSB_imm_T2:
        case SISA_OP_LDRSH_imm_T2:
            p = get_bit_value(instr, 10);
            u = get_bit_value(instr, 9);
            w = get_bit_value(instr, 8);
            if (p && u && (w == 0)) {
                status = ALIAS;
            }
            if (p == 0 && w == 0) {
                status = UNDEFINED;
            }
            if (w && n == t) {
                status = UNPREDICTABLE;
            }
            if (t == R_SP) {
                status = UNPREDICTABLE;
            }
            break;
        case SISA_OP_POP_T3/*single*/:
            if (t == R_SP) {
                status = UNPREDICTABLE;
            }
            break;
        case SISA_OP_LDRT_T1:
        case SISA_OP_LDRBT_T1:
        case SISA_OP_LDRHT_T1:
        case SISA_OP_LDRSBT_T1:
        case SISA_OP_LDRSHT_T1:
            if (BadReg(t)) {
                status = UNPREDICTABLE;
            }
            break;
        case SISA_OP_LDRD_imm_T1:
            p = get_bit_value(instr, 24);
            u = get_bit_value(instr, 23);
            w = get_bit_value(instr, 21);
            t2 = get_bits_value(instr, 8, 11);
            if (p == 0 && w == 0) {
                status = ALIAS;
            }
            if (w && (n == t || n == t2)) {
                status = UNPREDICTABLE;
            }
            if (BadReg(t) || BadReg(t2) || t == t2) {
                status = UNPREDICTABLE;
            }
            break;
        default:
            break;
    }

    if (! GET_OPT_STAT(OPT_NOT_ALLOW_FIX_MEM)) {
        if (op == SISA_OP_LDRD_imm_T1) {
            fix_mem_base(n, true);
        } else {
            fix_mem_base(n, false);
        }
    } else {
        gen_pre_instr_for_mem_base(n);
    }
    return status;
}

instr_status_t gen_thumb32_mem_reg(sisa_opcode_t op)
{
    instr_status_t status = VALID;
    uint32_t instr = get_raw_instr();
    uint32_t n = get_bits_value(instr, 16, 19);
    uint32_t t = get_bits_value(instr, 12, 15);
    uint32_t m = get_bits_value(instr, 0, 3);
    uint32_t imm = get_bits_value(instr, 4, 5);
    uint32_t reg_offset_val;
    if (n == R_PC) {
        status = ALIAS;
    }
    //pc as dst or base == offset
    if (t == R_PC || n == m) {
        status = UNSUPPORTED;
    }

    if (BadReg(m)) {
        status = UNPREDICTABLE;
    }
    if (op != SISA_OP_LDR_reg_T2 && op != SISA_OP_STR_reg_T2) {
        if (t == R_SP) {
            status = UNPREDICTABLE;
        }
    }
    if (! GET_OPT_STAT(OPT_NOT_ALLOW_FIX_MEM)) {
        fix_mem_base(n, false);
    } else {
        gen_pre_instr_for_mem_base(n);
    }
    get_rand_mem_offset(0, imm, &reg_offset_val);
    sprintf(gen_instr_buf, "[INST] ldr.w r%d, =0x%x", m, reg_offset_val);
    insert_instr(gen_instr_buf);
    return status;
}

instr_status_t gen_thumb32_mem_lit(sisa_opcode_t op)
{
    instr_status_t status = VALID;
    uint32_t instr = get_raw_instr();
    uint32_t t = get_bits_value(instr, 12, 15);
    uint32_t t2 = get_bits_value(instr, 8, 11);
    if (op == SISA_OP_LDR_lit_T2) {
        if (t == R_PC) {
            status = UNSUPPORTED;
        }
    } else if (op == SISA_OP_LDRD_lit_T1) {
        if ((instr & 0x01200000) == 0) {
            status = ALIAS;
        }
        if (BadReg(t) || BadReg(t2) || t == t2) {
            status = UNPREDICTABLE;
        }
    } else {
        if (BadReg(t)) {
            status = UNPREDICTABLE;
        }
    }
    return status;
}

instr_status_t gen_thumb32_st_imm(sisa_opcode_t op)
{
    instr_status_t status = VALID;
    uint32_t instr = get_raw_instr();
    uint32_t n = get_bits_value(instr, 16, 19);
    uint32_t t = get_bits_value(instr, 12, 15);
    uint32_t t2;
    bool p, u, w;
    if (n == R_PC) {
        status = UNDEFINED;
    }
    if (t == R_PC) {
        status = UNPREDICTABLE;
    }

    switch (op) {
        case SISA_OP_STR_imm_T3:
            break;
        case SISA_OP_STR_imm_T4:
            p = get_bit_value(instr, 10);
            u = get_bit_value(instr, 9);
            w = get_bit_value(instr, 8);
            if (p && u && (w == 0)) {
                status = ALIAS;
            }
            if (p == 0 && w == 0) {
                status = UNDEFINED;
            }
            if (w && n == t) {
                status = UNPREDICTABLE;
            }
            break;
        case SISA_OP_STRB_imm_T3:
        case SISA_OP_STRH_imm_T3:
            p = get_bit_value(instr, 10);
            u = get_bit_value(instr, 9);
            w = get_bit_value(instr, 8);
            if (p && u && (w == 0)) {
                status = ALIAS;
            }
            if (p == 0 && w == 0) {
                status = UNDEFINED;
            }
            if (BadReg(t) || (w && n == t)) {
                status = UNPREDICTABLE;
            }
            break;
        case SISA_OP_STRB_imm_T2:
        case SISA_OP_STRH_imm_T2:
        case SISA_OP_STRT_T1:
        case SISA_OP_STRBT_T1:
        case SISA_OP_STRHT_T1:
        case SISA_OP_PUSH_T3/*single*/:
            if (BadReg(t)) {
                status = UNPREDICTABLE;
            }
            break;
        case SISA_OP_STRD_imm_T1:
            p = get_bit_value(instr, 24);
            u = get_bit_value(instr, 23);
            w = get_bit_value(instr, 21);
            t2 = get_bits_value(instr, 8, 11);
            if (p == 0 && w == 0) {
                status = ALIAS;
            }
            if (w && (n == t || n == t2)) {
                status = UNPREDICTABLE;
            }
            if (n == R_PC || BadReg(t) || BadReg(t2)) {
                status = UNPREDICTABLE;
            }
            break;
        default:
            break;
    }

    if (! GET_OPT_STAT(OPT_NOT_ALLOW_FIX_MEM)) {
        if (op == SISA_OP_STRD_imm_T1) {
            fix_mem_base(n, true);
        } else {
            fix_mem_base(n, false);
        }
    } else {
        gen_pre_instr_for_mem_base(n);
    }
    return status;
}

instr_status_t gen_thumb32_mem_ex(sisa_opcode_t op)
{
    instr_status_t status = VALID;
    uint32_t instr = get_raw_instr();
    uint32_t t = get_bits_value(instr, 12, 15);
    uint32_t n = get_bits_value(instr, 16, 19);
    uint32_t t2 = get_bits_value(instr, 8, 11);
    uint32_t d = get_bits_value(instr, 0, 3);
    switch (op) {
        case SISA_OP_LDREXD_T1:
            if (BadReg(t) || BadReg(t2) || t == t2 || n == R_PC) {
                status = UNPREDICTABLE;
            }
            break;
        case SISA_OP_LDREX_T1:
        case SISA_OP_LDREXB_T1:
        case SISA_OP_LDREXH_T1:
            if (BadReg(t) || n == R_PC) {
                status = UNPREDICTABLE;
            }
            break;
        case SISA_OP_STREX_T1:
            d = get_bits_value(instr, 8, 11);
            if (BadReg(d) || BadReg(t) || n == R_PC) {
                status = UNPREDICTABLE;
            }
            if (d == n || d == t) {
                status = UNPREDICTABLE;
            }
            break;
        case SISA_OP_STREXB_T1:
        case SISA_OP_STREXH_T1:
            if (BadReg(d) || BadReg(t) || n == R_PC) {
                status = UNPREDICTABLE;
            }
            if (d == n || d == t) {
                status = UNPREDICTABLE;
            }
            break;
        case SISA_OP_STREXD_T1:
            if (BadReg(d) || BadReg(t) || BadReg(t2) || n == R_PC) {
                status = UNPREDICTABLE;
            }
            if (d == n || d == t || d == t2) {
                status = UNPREDICTABLE;
            }
            break;
        default:
            status = UNSUPPORTED;
            break;
    }
    if (! GET_OPT_STAT(OPT_NOT_ALLOW_FIX_MEM)) {
        fix_mem_base(n, true);
    } else {
        gen_pre_instr_for_mem_base(n);
    }

    return status;
}

instr_status_t gen_thumb32_mem_preload(sisa_opcode_t op)
{
    //any address for preload?
    instr_status_t status = VALID;
    uint32_t instr = get_raw_instr();
    if (op == SISA_OP_PLD_imm_T1 || op == SISA_OP_PLD_imm_T2 || 
            op == SISA_OP_PLI_imm_T1 || op == SISA_OP_PLI_imm_T2 ||
            op == SISA_OP_PLDW_imm_T1 || op == SISA_OP_PLDW_imm_T2) {
        uint32_t n = get_bits_value(instr, 16, 19);
        if (n == R_PC) {
            status = ALIAS;
        }
    } else if (op == SISA_OP_PLD_reg_T1 || op == SISA_OP_PLDW_reg_T1 || op == SISA_OP_PLI_reg_T1) {
        uint32_t n = get_bits_value(instr, 16, 19);
        uint32_t m = get_bits_value(instr, 0, 3);
        if (n == R_PC) {
            status = ALIAS;
        }
        if (BadReg(m)) {
            status = UNPREDICTABLE;
        }
    } else {
        status = VALID;
    }
    return status;
}

instr_status_t gen_thumb32_sr(sisa_opcode_t op)
{
    instr_status_t status = VALID;
    uint32_t instr = get_raw_instr();
    if (op == SISA_OP_MSR_a_T1) {
        uint32_t n = get_bits_value(instr, 16, 19);
        uint32_t mask = get_bits_value(instr, 10, 11);
        if (mask == 0 || BadReg(n)) {
            status = UNPREDICTABLE;
        }
    } else if (op == SISA_OP_MRS_a_T1) {
        uint32_t d= get_bits_value(instr, 8, 11);
        if (BadReg(d)) {
            status = UNPREDICTABLE;
        }
    } else {
        status = UNSUPPORTED;
    }
    return status;
}

instr_status_t gen_thumb32_hint(sisa_opcode_t op)
{

    if (op == SISA_OP_WFI_T2) {
        //diff behavior with qemu
         return UNSUPPORTED;
    } else if (op == SISA_OP_CLREX_T1) {
        //TODO clrex support
        return UNSUPPORTED;
    } else {
        return VALID;
    }
}

