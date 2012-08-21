#include <stdio.h>
#include <string.h>
#include "GenARMInstr.h"
#include "InstrUtil.h"
#include "GenHelper.h"
#include "BiasUtil.h"
#include "RandomInstrGen.h"


extern gen_option_t options;
extern bool is_oprand_init;
extern uint32_t shift_imm[];
extern uint32_t shift_imm_num;
static char gen_instr_buf[MAX_STR_INSTR_LEN+1];
// used for ld

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

bool is_op_armv5_instr(sisa_opcode_t op)
{
    //TODO
    return true;
}

instr_status_t gen_arm_instr (sisa_opcode_t op) {

    instr_status_t status = VALID;
    memset(gen_instr_buf, 0 , (MAX_STR_INSTR_LEN+1)*sizeof(char));

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

    return status;
}

instr_status_t gen_ld_instr(sisa_opcode_t op)
{
    instr_status_t status = VALID;
    uint32_t n, t; //Rn and Rt
    uint32_t P, W, U;
    bool wback, index, add;
    uint32_t instr = get_raw_instr();
    t = get_regnum(instr, glb_instr_tlb[op].opnds[SISA_OUTPUT0]);
    n = get_regnum(instr, glb_instr_tlb[op].opnds[SISA_INPUT0]);

    //P = ((raw_instr & 0x01000000) == 0x01000000);
    //W = ((raw_instr & 0x00200000) == 0x00200000); 
    //U = ((raw_instr & 0x00800000) == 0x00800000);
    P = get_bit_value(instr, 24);
    W = get_bit_value(instr, 21);
    U = get_bit_value(instr, 23);
    //wback = (P == '0') || (W == '1'); P:24, W:21
    if (P == 0 || W == 1) {
        wback = true;
    } else {
        wback = false;
    }
    //index = (P == '1'); P:24
    if (P == 1) {
        index = true;
    } else {
        index = false;
    }
    if (U == 1) {
        add = true;
    } else {
        add = false;
    }

    if (op == SISA_OP_LDR_imm || op == SISA_OP_LDRB_imm || 
            op == SISA_OP_LDRH_imm || op == SISA_OP_LDRD_imm ||
            op == SISA_OP_LDRSB_imm || op == SISA_OP_LDRSH_imm) {
        //if P == '0' && W == '1' then SEE LDRT/LDRBT/LDRHT/LDRBT/LDRHT;
        if (P == 0 && W == 1) {
            status = ALIAS;
            if (op == SISA_OP_LDRD_imm) {
                status = UNPREDICTABLE;
            }
        }
        if (wback && n == t) {
            status = UNPREDICTABLE;
        }

        if (op == SISA_OP_LDR_imm) {
            //if Rn == '1101' && P == '0' && U == '1' && W == '0' && imm12 == '000000000100' then SEE POP;
            if ((instr & 0x01af0fff) == 0x009d0004) {
                status = ALIAS;
            }
        }
        if (t == R_PC) {
            if (op == SISA_OP_LDRB_imm || op == SISA_OP_LDRH_imm || 
                    op == SISA_OP_LDRSB_imm || op == SISA_OP_LDRSH_imm) {
                status = UNPREDICTABLE;
            } else {
                //LoadWritePC
                status = UNSUPPORTED;
            }
        }

        if (op == SISA_OP_LDRD_imm) {
            if ((t % 2) == 1 || t == 14) {
                status = UNPREDICTABLE;
            }
            if (wback && (n == (t + 1))) {
                status = UNPREDICTABLE; 
            }
        }

        if (n == R_PC) {
            //if Rn == '1111' then SEE 
            //  LDR (literal) / LDRB (literal) / LDRH (literal) / 
            //  LDRD (literal) / LDRSB (literal) / LDRSH (literal)
            //status = ALIAS;
            //ld literal
            status = UNSUPPORTED;
        }

        if (! GET_OPT_STAT(OPT_NOT_ALLOW_FIX_MEM)) {
            if (status != UNSUPPORTED) {
                /*
                status = FIXINST;
                if (op == SISA_OP_LDRD_imm) {
                    insert_instr("[INSTINFO] type=MemA");
                } else {
                    insert_instr("[INSTINFO] type=MemU");
                }*/
                if (op == SISA_OP_LDRD_imm) {
                    fix_mem_base(n, true);
                    if (index) {
                        set_raw_instr(set_instr(instr, 0, 1, 0));
                    }
                } else {
                    fix_mem_base(n, false);
                }
            }
        } else {
            if (status != UNSUPPORTED) {
                uint32_t base_val = gen_pre_instr_for_mem_base(n);
                uint32_t imm_l = instr & 0xf;
                uint32_t mem_addr = get_mem_addr(base_val, imm_l, add, index);

                if (op == SISA_OP_LDRH_imm || op == SISA_OP_LDRSH_imm) {
                    if ((mem_addr & 0x1) && (options.arch_version < 7)) {
                        status = INST_UNKNOWN;
                    }
                }

                if (op == SISA_OP_LDR_imm || op == SISA_OP_LDRD_imm) {
                    if (!is_mem_align(mem_addr, 2)) {
                        status = UNSUPPORTED;
                    }
                }
            }
        }
    } else if (op == SISA_OP_LDRT_a1 || op == SISA_OP_LDRHT_a1 || op == SISA_OP_LDRSHT_a1
            || op == SISA_OP_LDRBT_a1 || op == SISA_OP_LDRSBT_a1) {
        if (t == R_PC || n == R_PC || n == t) {
            status = UNPREDICTABLE;
        }
        if (t == R_PC || n == R_PC) {
            status = UNSUPPORTED;
        }

        if (! GET_OPT_STAT(OPT_NOT_ALLOW_FIX_MEM)) {
            if (status != UNSUPPORTED) {
                /*
                status = FIXINST;
                insert_instr("[INSTINFO] type=MemU_unpriv");
                */
                fix_mem_base(n, false);
            }
        } else {
            if (status != UNSUPPORTED) {
                uint32_t base_val = gen_pre_instr_for_mem_base(n);
                uint32_t imm_l = instr & 0xf;
                uint32_t mem_addr = get_mem_addr(base_val, imm_l, add, index);
                if (op == SISA_OP_LDRHT_a1 || op == SISA_OP_LDRSHT_a1) {
                    if ((mem_addr & 0x1) && (options.arch_version < 7)) {
                        status = INST_UNKNOWN;
                    }
                }

                if (op == SISA_OP_LDRT_a1) {
                    if (!is_mem_align(mem_addr, 2)) {
                        status = UNSUPPORTED;
                    }
                }
            }
        }
    } else if (op == SISA_OP_LDR_reg || op == SISA_OP_LDRB_reg ||
            op == SISA_OP_LDRH || op == SISA_OP_LDRD_reg ||
            op == SISA_OP_LDRSB || op == SISA_OP_LDRSH) {
        uint32_t m, shift_n, shift_t;
        m = get_regnum(instr, glb_instr_tlb[op].opnds[SISA_INPUT1]);
        if (op == SISA_OP_LDR_reg || op == SISA_OP_LDRB_reg) {
            shift_n = get_bits_value(instr, glb_instr_tlb[op].opnds[SISA_INPUT2].start_bit, glb_instr_tlb[op].opnds[SISA_INPUT2].end_bit);
            shift_t = get_bits_value(instr, glb_instr_tlb[op].opnds[SISA_INPUT2].bit1, glb_instr_tlb[op].opnds[SISA_INPUT2].bit2);
            if ((shift_t == 1 || shift_t == 2) && shift_n == 0) {
                //if imm5 == '00000' then 32 else UInt(imm5) LSR/ASR
                shift_n = 32;
            }
        } else {
            shift_n = 0;
            shift_t = 0;
        }

        //LDRT/LDRBT/LDRHT/LDRSBT/LDRSHT
        if (P == 0 && W == 1) {
            if (op == SISA_OP_LDRD_reg) {
                status = UNPREDICTABLE;
            } else {
                status = ALIAS;
            }
        }
        if (m == R_PC) {
            status = UNPREDICTABLE;
        }
        if (t == R_PC) {
            if (op == SISA_OP_LDRB_reg || op == SISA_OP_LDRH ||
                    op == SISA_OP_LDRSB || op == SISA_OP_LDRSH) {
                status = UNPREDICTABLE;
            }
        }
        if (wback && (n == R_PC || n == t)) {
            status = UNPREDICTABLE;
        }
        if (options.arch_version < 6 && wback && m == n) {
            status = UNPREDICTABLE;
        }

        if (op == SISA_OP_LDRD_reg) {
            uint32_t t2 = t + 1;
            if ((t % 2) == 1) {
                status = UNPREDICTABLE;
            }
            if (P == 0 && W == 1) {
                status = UNPREDICTABLE;
            }
            if (m == t || m == t2) {
                status = UNPREDICTABLE;
            }
            if (wback && n == t2) {
                status = UNPREDICTABLE;
            }
            if (t2 == R_PC) {
                status = UNSUPPORTED;
            }
        }

        if (t == R_PC) {
            status = UNSUPPORTED;
        }

        if (n == R_PC) {
            // pc as base
            status = UNSUPPORTED;
        }

        if (m == n) {
            //TODO Rn + Rn shift imm
            status = UNSUPPORTED;
        }  

        if (! GET_OPT_STAT(OPT_NOT_ALLOW_FIX_MEM)) {
            if (status != UNSUPPORTED) {
                /*
                status = FIXINST;
                if (op == SISA_OP_LDRD_reg) {
                    insert_instr("[INSTINFO] type=MemA");
                } else {
                    insert_instr("[INSTINFO] type=MemU");
                }*/
                uint32_t offset_val;
                //fix base
                if (op == SISA_OP_LDRD_reg) {
                    fix_mem_base(n, true);
                } else {
                    fix_mem_base(n, false);
                }

                //fix offset
                if (op == SISA_OP_LDR_reg || op == SISA_OP_LDRB_reg) {
                    get_rand_mem_offset(shift_t, shift_n, &offset_val);
                } else if (op == SISA_OP_LDRD_reg) {
                    offset_val = rand() & (options.mem_offset_val & (~0x3));
                } else {
                    offset_val = rand() & options.mem_offset_val;
                }
                sprintf(gen_instr_buf, "[INST] ldr r%d, =0x%x", m, offset_val);
                insert_instr(gen_instr_buf);
            }
        } else {
            if (status != UNSUPPORTED) {

                uint32_t base_val = gen_pre_instr_for_mem_base(n);
                uint32_t reg_offset_val;
                uint32_t offset_val = get_rand_mem_offset(shift_t, shift_n, &reg_offset_val);
                uint32_t mem_addr = get_mem_addr(base_val, offset_val, add, index);

                sprintf(gen_instr_buf, "[INST] ldr r%d, =0x%x", m, reg_offset_val);
                insert_instr(gen_instr_buf);
                if (op == SISA_OP_LDRH || op == SISA_OP_LDRSH) {
                    if (mem_addr & 0x1 && (options.arch_version < 7)) {
                        status = INST_UNKNOWN;
                    }
                }

                if (op == SISA_OP_LDR_reg || op == SISA_OP_LDRD_reg) {
                    if (!is_mem_align(mem_addr, 2)) {
                        status = UNSUPPORTED;
                    }
                }
            }
        }
    } else if (op == SISA_OP_LDRT_a2 || op == SISA_OP_LDRHT_a2 || op == SISA_OP_LDRSHT_a2
            || op == SISA_OP_LDRBT_a2 || op == SISA_OP_LDRSBT_a2) {
        uint32_t m, shift_t, shift_n;
        m = get_regnum(instr, glb_instr_tlb[op].opnds[SISA_INPUT1]);
        if (op == SISA_OP_LDRT_a2 || op == SISA_OP_LDRBT_a2) {
            shift_n = get_bits_value(instr, glb_instr_tlb[op].opnds[SISA_INPUT2].start_bit, glb_instr_tlb[op].opnds[SISA_INPUT2].end_bit);
            shift_t = get_bits_value(instr, glb_instr_tlb[op].opnds[SISA_INPUT2].bit1, glb_instr_tlb[op].opnds[SISA_INPUT2].bit2);
            if ((shift_t == 1 || shift_t == 2) && shift_n == 0) {
                //if imm5 == '00000' then 32 else UInt(imm5) LSR/ASR
                shift_n = 32; 
            } 
        } else {
            shift_n = 0;
            shift_t = 0;
        } 
        if (t == R_PC || n == R_PC || m == R_PC || n == t) {
            status = UNPREDICTABLE;
        }
        if (t == R_PC || n == R_PC || m == R_PC) {
            status = UNSUPPORTED;
        }

        if (m == n) {
            //TODO Rn + Rn shift imm
            status = UNSUPPORTED;
        }

        if (! GET_OPT_STAT(OPT_NOT_ALLOW_FIX_MEM)) {
            if (status != UNSUPPORTED) {
                /*status = FIXINST;
                insert_instr("[INSTINFO] type=MemU_unpriv");*/
                uint32_t offset_val;
                //fix base
                fix_mem_base(n, false);

                //fix offset
                if (op == SISA_OP_LDRT_a2 || op == SISA_OP_LDRBT_a2) {
                    get_rand_mem_offset(shift_t, shift_n, &offset_val);
                } else {
                    offset_val = rand() & options.mem_offset_val;
                }
                sprintf(gen_instr_buf, "[INST] ldr r%d, =0x%x", m, offset_val);
                insert_instr(gen_instr_buf);
            }
        } else {
            if (status != UNSUPPORTED) {
                uint32_t base_val = gen_pre_instr_for_mem_base(n);
                uint32_t reg_offset_val;
                uint32_t offset_val = get_rand_mem_offset(shift_t, shift_n, &reg_offset_val);
                uint32_t mem_addr = get_mem_addr(base_val, offset_val, add, index);
                sprintf(gen_instr_buf, "[INST] ldr r%d, =0x%x", m, reg_offset_val);
                insert_instr(gen_instr_buf);
                if (op == SISA_OP_LDRHT_a2 || op == SISA_OP_LDRSHT_a2) {
                    if ((mem_addr & 1) && (options.arch_version < 7)) {
                        status = INST_UNKNOWN;
                    }
                }

                if (op == SISA_OP_LDRT_a2) {
                    if (!is_mem_align(mem_addr, 2)) {
                        status = UNSUPPORTED;
                    }
                }
            }
        }
    } else if (op == SISA_OP_LDR_lit || op == SISA_OP_LDRB_lit ||
            op == SISA_OP_LDRH_lit || op == SISA_OP_LDRD_lit ||
            op == SISA_OP_LDRSB_lit || op == SISA_OP_LDRSH_lit) {
        uint32_t t = get_regnum(instr, glb_instr_tlb[op].opnds[SISA_OUTPUT0]);
        if (op == SISA_OP_LDR_lit) {
            if (t == R_PC) {
                status = UNSUPPORTED;
            }
        } else if (op == SISA_OP_LDRD_lit) {
            clear_bits(instr, 0, 1);
            set_raw_instr(instr);
            if ((t % 2 == 1) || t == 14) {
                status = UNPREDICTABLE;
            }
        } else {
            if (t == R_PC) {
                status = UNPREDICTABLE;
            }
        }
        /*
        if (! GET_OPT_STAT(OPT_NOT_ALLOW_FIX_MEM)) {
            status = FIXINST;
            if (op == SISA_OP_LDRD_lit) {
                insert_instr("[INSTINFO] type=MemA");
            } else {
                insert_instr("[INSTINFO] type=MemU");
            }

            if (op == SISA_OP_LDR_lit) {
                status = UNSUPPORTED;
            }
            status = UNSUPPORTED;
        } else {
            printf("Unsupported pc-rel memory instruction\n");
            status = UNSUPPORTED;
        }*/
    } else {
        printf("Unsupported LD_OP instruction %s\n", glb_instr_tlb[op].name);
        status = UNSUPPORTED;
    }
    is_oprand_init = true;
    return status;
}

// used for st
instr_status_t gen_st_instr(sisa_opcode_t op)
{
    instr_status_t status = VALID;
    uint32_t n, t; //Rn and Rt
    uint32_t P, W, U;
    bool wback, index, add;
    uint32_t instr = get_raw_instr();
    t = get_regnum(instr, glb_instr_tlb[op].opnds[SISA_INPUT0]);
    n = get_regnum(instr, glb_instr_tlb[op].opnds[SISA_INPUT1]);

    //P = ((raw_instr & 0x01000000) == 0x01000000);
    //W = ((raw_instr & 0x00200000) == 0x00200000);
    //U = ((raw_instr & 0x00800000) == 0x00800000);
    P = get_bit_value(instr, 24);
    W = get_bit_value(instr, 21);
    U = get_bit_value(instr, 23);

    //wback = (P == '0') || (W == '1'); P:24, W:21
    if (P == 0 || W == 1) {
        wback = true;
    } else {
        wback = false;
    }
    //index = (P == '1'); P:24
    if (P == 1) {
        index = true;
    } else {
        index = false;
    }
    if (U == 1) {
        add = true;
    } else {
        add = false;
    }

    if (op == SISA_OP_STR_imm || op == SISA_OP_STRB_imm ||
            op == SISA_OP_STRH_imm || op == SISA_OP_STRD_imm) {
        //if P == '0' && W == '1' then SEE STRT/STRBT/STRHT;
        if (P == 0 && W == 1) {
            status = ALIAS;
        }

        //if Rn == '1101' && P == '1' && U == '0' && W == '1' && imm12 == '000000000100' then SEE PUSH;
        if ((instr & 0x01af0fff) == 0x012d0004) {
            status = ALIAS;
        }
        if (wback && (n == R_PC || n == t)) {
            status = UNPREDICTABLE;
        }
        if (t == R_PC) {
            if (op == SISA_OP_STRB_imm || op == SISA_OP_STRH_imm) {
                status = UNPREDICTABLE;
            }
        }

        if (op == SISA_OP_STRD_imm) {
            if (P == 0 && W == 1) {
                status = UNPREDICTABLE;
            }
            if ((t % 2) == 1 || t == 14) {
                status = UNPREDICTABLE;
            }
            if (wback && (n == (t + 1))) {
                status = UNPREDICTABLE;        
            }
        }
        if (n == R_PC) {
            //pc as base
            status = UNSUPPORTED;
        }

        if (! GET_OPT_STAT(OPT_NOT_ALLOW_FIX_MEM)) {
            if (status != UNSUPPORTED) {
                /*
                status = FIXINST;
                if (op == SISA_OP_STRD_imm) {
                    insert_instr("[INSTINFO] type=MemA");
                } else {
                    insert_instr("[INSTINFO] type=MemU");
                }*/
                if (op == SISA_OP_STRD_imm) {
                    fix_mem_base(n, true);
                    if (index) {
                        set_raw_instr(set_instr(instr, 0, 1, 0));
                    }
                } else {
                    fix_mem_base(n, false);
                }
            }
        } else {
            if (status != UNSUPPORTED) {
                uint32_t base_val = gen_pre_instr_for_mem_base(n);
                uint32_t imm_l = instr & 0xf;
                uint32_t mem_addr = get_mem_addr(base_val, imm_l, add, index);
                if (op == SISA_OP_STRH_imm) {
                    if ((mem_addr & 1) && (options.arch_version < 7)) {
                        status = INST_UNKNOWN;
                    }
                }

                if (op == SISA_OP_STR_imm || op == SISA_OP_STRD_imm) {
                    if (!is_mem_align(mem_addr, 2)) {
                        status = UNSUPPORTED;
                    }
                }
            }
        }

    } else if (op == SISA_OP_STRT_a1 || op == SISA_OP_STRBT_a1 || op == SISA_OP_STRHT_a1) {
        if (n == R_PC || n == t) {
            status = UNPREDICTABLE;
        }
        if (op == SISA_OP_STRBT_a1 || op == SISA_OP_STRHT_a1) {
            if (t == R_PC) {
                status = UNPREDICTABLE;
            }
        }
        if (n == R_PC) {
            status = UNSUPPORTED;
        }

        if (! GET_OPT_STAT(OPT_NOT_ALLOW_FIX_MEM)) {
            if (status != UNSUPPORTED) {
                /*
                status = FIXINST;
                insert_instr("[INSTINFO] type=MemU_unpriv");
                */
                fix_mem_base(n, false);
            }
        } else {
            if (status != UNSUPPORTED) {
                uint32_t base_val = gen_pre_instr_for_mem_base(n);
                uint32_t imm_l = instr & 0xf;
                uint32_t mem_addr = get_mem_addr(base_val, imm_l, add, index);
                if (op == SISA_OP_STRHT_a1) {
                    if ((mem_addr & 1) && (options.arch_version < 7)) {
                        status = INST_UNKNOWN;
                    }
                }

                if (op == SISA_OP_STRT_a1) {
                    if (!is_mem_align(mem_addr, 2)) {
                        status = UNSUPPORTED;
                    }
                }
            }
        }
    } else if (op == SISA_OP_STR_reg || op == SISA_OP_STRB_reg || 
            op == SISA_OP_STRH_reg || op == SISA_OP_STRD_reg) {
        uint32_t m, shift_n, shift_t;
        m = get_regnum(instr, glb_instr_tlb[op].opnds[SISA_INPUT2]);
        if (op == SISA_OP_STR_reg || op == SISA_OP_STRB_reg) {
            shift_n = get_bits_value(instr, glb_instr_tlb[op].opnds[SISA_INPUT3].start_bit, glb_instr_tlb[op].opnds[SISA_INPUT3].end_bit);
            shift_t = get_bits_value(instr, glb_instr_tlb[op].opnds[SISA_INPUT3].bit1, glb_instr_tlb[op].opnds[SISA_INPUT3].bit2);
            if ((shift_t == 1 || shift_t == 2) && shift_n == 0) {
                //if imm5 == '00000' then 32 else UInt(imm5) LSR/ASR
                shift_n = 32; 
            }
        } else {
            shift_n = 0;
            shift_t = 0;
        }
        //STRT/STRBT/STRHT
        if (P == 0 && W == 1) {
            if (op == SISA_OP_STRD_reg) {
                status =UNPREDICTABLE;
            } else {
                status = ALIAS;
            }
        }
        if (m == R_PC) {
            status = UNPREDICTABLE;
        }
        if (t == R_PC) {
            if (op == SISA_OP_STRB_reg || op == SISA_OP_STRH_reg) {
                status = UNPREDICTABLE;
            }
        }
        if (wback && (n == R_PC || n == t)) {
            status = UNPREDICTABLE;
        }
        if (options.arch_version < 6 && wback && m == n) {
            status = UNPREDICTABLE;
        }

        if (op == SISA_OP_STRD_reg) {
            if ((t % 2) == 1 || t == 14) {
                status = UNPREDICTABLE;
            }   
            if (wback && (n == (t + 1))) {
                status = UNPREDICTABLE;    
            }
        }

        if (n == R_PC) {
            //pc as base
            status = UNSUPPORTED;
        }

        if (m == n) {
            //TODO Rn + Rn shift imm
            status = UNSUPPORTED;
        }

        if (! GET_OPT_STAT(OPT_NOT_ALLOW_FIX_MEM)) {
            if (status != UNSUPPORTED) {
                /*
                status = FIXINST;
                if (op == SISA_OP_STRD_reg) {
                    insert_instr("[INSTINFO] type=MemA");
                } else {
                    insert_instr("[INSTINFO] type=MemU");
                }
                */
                uint32_t offset_val;
                //fix base
                if (op == SISA_OP_STRD_reg) {
                    fix_mem_base(n, true);
                } else {
                    fix_mem_base(n, false);
                }

                //fix offset
                if (op == SISA_OP_STR_reg || op == SISA_OP_STRB_reg) {
                    get_rand_mem_offset(shift_t, shift_n, &offset_val);
                } else if (op == SISA_OP_STRD_reg) {
                    offset_val = rand() & (options.mem_offset_val & (~0x3)); 
                } else {
                    offset_val = rand() & options.mem_offset_val;
                }
                sprintf(gen_instr_buf, "[INST] ldr r%d, =0x%x", m, offset_val);
                insert_instr(gen_instr_buf);
            }
        } else {
            if (status != UNSUPPORTED) {
                uint32_t base_val = gen_pre_instr_for_mem_base(n);
                uint32_t reg_offset_val;
                uint32_t offset_val = get_rand_mem_offset(shift_t, shift_n, &reg_offset_val);
                uint32_t mem_addr = get_mem_addr(base_val, offset_val, add, index);
                sprintf(gen_instr_buf, "[INST] ldr r%d, =0x%x", m, reg_offset_val);
                insert_instr(gen_instr_buf);

                if (op == SISA_OP_STRH_reg) {
                    if ((mem_addr & 1) && (options.arch_version < 7)) {
                        status = INST_UNKNOWN;
                    }
                }

                if (op == SISA_OP_STR_reg || op == SISA_OP_STRD_reg) {
                    if (!is_mem_align(mem_addr, 2)) {
                        status = UNSUPPORTED;
                    }
                }
            }
        }
    } else if (op == SISA_OP_STRT_a2 || op == SISA_OP_STRBT_a2 || op == SISA_OP_STRHT_a2) {
        uint32_t m, shift_n, shift_t;
        m = get_regnum(instr, glb_instr_tlb[op].opnds[SISA_INPUT2]);
        if (op == SISA_OP_STRT_a2 || op == SISA_OP_STRBT_a2) {
            shift_n = get_bits_value(instr, glb_instr_tlb[op].opnds[SISA_INPUT3].start_bit, glb_instr_tlb[op].opnds[SISA_INPUT3].end_bit);
            shift_t = get_bits_value(instr, glb_instr_tlb[op].opnds[SISA_INPUT3].bit1, glb_instr_tlb[op].opnds[SISA_INPUT3].bit2);
            if ((shift_t == 1 || shift_t == 2) && shift_n == 0) {
                //if imm5 == '00000' then 32 else UInt(imm5) LSR/ASR
                shift_n = 32; 
            }
        } else {
            shift_n = 0;
            shift_t = 0;
        }
        
        if (n == R_PC || n == t || m == R_PC) {
            status = UNPREDICTABLE;
        }
        if (op == SISA_OP_STRBT_a2 || op == SISA_OP_STRHT_a2) {
            if (t == R_PC) {
                status = UNPREDICTABLE;
            }
        }
        if (n == R_PC || m == R_PC) {
            status = UNSUPPORTED;
        }

        if (m == n) {
            //TODO Rn + Rn shift imm
            status = UNSUPPORTED;
        }

        if (! GET_OPT_STAT(OPT_NOT_ALLOW_FIX_MEM)) {
            if (status != UNSUPPORTED) {
                /*
                status = FIXINST;
                insert_instr("[INSTINFO] type=MemU_unpriv");
                */
                uint32_t offset_val;
                //fix_base
                fix_mem_base(n, false);

                //fix_offset
                if (op == SISA_OP_STRT_a2 || op == SISA_OP_STRBT_a2) {
                    get_rand_mem_offset(shift_t, shift_n, &offset_val);
                } else {
                    offset_val = rand() & options.mem_offset_val;
                }
                sprintf(gen_instr_buf, "[INST] ldr r%d, =0x%x", m, offset_val);
                insert_instr(gen_instr_buf);
            }
        } else {
            if (status != UNSUPPORTED) {
                uint32_t base_val = gen_pre_instr_for_mem_base(n);
                uint32_t reg_offset_val;
                uint32_t offset_val = get_rand_mem_offset(shift_t, shift_n, &reg_offset_val);
                uint32_t mem_addr = get_mem_addr(base_val, offset_val, add, index);
                sprintf(gen_instr_buf, "[INST] ldr r%d, =0x%x", m, reg_offset_val);
                insert_instr(gen_instr_buf);
                if (op == SISA_OP_STRHT_a2) {
                    if ((mem_addr & 1) && (options.arch_version < 7)) {
                        status = INST_UNKNOWN;
                    }   
                }

                if (op == SISA_OP_STRT_a2) {
                    if (!is_mem_align(mem_addr, 2)) {
                        status = UNSUPPORTED;
                    }
                }
            }
        }
    } else {
        printf("Unsupported ST_OP instruction %s\n", glb_instr_tlb[op].name);
        status = UNSUPPORTED;
    }
    is_oprand_init = true;
    return status;
}

// used for ldm/stm
instr_status_t gen_multi_mem_instr(sisa_opcode_t op)
{
    instr_status_t status = VALID;
    uint32_t n; //Rn
    uint32_t W;
    bool wback;
    uint32_t register_list, reg_cnt;
    uint32_t instr = get_raw_instr();
    if (glb_instr_tlb[op].op_kind == LDM_OP) {
        assert(op == SISA_OP_LDM || 
                op == SISA_OP_LDMDA || 
                op == SISA_OP_LDMDB || 
                op == SISA_OP_LDMIB);

        n = get_regnum(instr, glb_instr_tlb[op].opnds[SISA_INPUT0]);
        W = get_bit_value(instr, 21);
        wback = W;
        register_list = instr & 0xffff;
        reg_cnt = bit_count(register_list);
        //SEE POP;
        if (op == SISA_OP_LDM) {
            if (W == 1 && n == R_SP && reg_cnt >=2) {
                status = ALIAS;
            }
        }
        if (n == R_PC || reg_cnt < 1) {
            status = UNPREDICTABLE;
        }
        if (wback && get_bit_value(register_list, n)) {
            if(options.arch_version >= 7) {
                status = UNPREDICTABLE;
            } else {
                status = INST_UNKNOWN;
            }
        }
        if (n == R_PC) {
            status = UNSUPPORTED;
        }
        if (get_bit_value(register_list, R_PC) == 1) {
            //LoadWritePC
            status = UNSUPPORTED;
        }
        if (! GET_OPT_STAT(OPT_NOT_ALLOW_FIX_MEM)) {
            if (status != UNSUPPORTED) {
                /*
                status = FIXINST;
                insert_instr("[INSTINFO] type=MemA");
                */
                fix_mem_base(n, true);
            }
        } else {
            if (status != UNSUPPORTED) {
                uint32_t base_val = gen_pre_instr_for_mem_base(n);
                if (!is_mem_align(base_val, 2)) {
                    status = UNSUPPORTED;
                }
            }
        }
    } else if (glb_instr_tlb[op].op_kind == STM_OP) {

        assert(op == SISA_OP_STM || 
                op == SISA_OP_STMDA || 
                op == SISA_OP_STMDB || 
                op == SISA_OP_STMIB);
        n = get_regnum(instr, glb_instr_tlb[op].opnds[SISA_INPUT0]);
        W = get_bit_value(instr, 21);
        wback = W;
        register_list = instr & 0xffff;
        reg_cnt = bit_count(register_list);
        //SEE PUSH
        if (op == SISA_OP_STMDB) {
            if (W == 1 && n == R_SP && reg_cnt >=2) {
                status = ALIAS;
            }
        }
        if (n == R_PC || reg_cnt < 1) {
            status = UNPREDICTABLE;
        }
        //if i == n && wback && i != LowestSetBit(reg) then UNKNOWN
        if (get_bit_value(register_list, n) == 1 && wback &&
                (register_list & (0xffff >> (16 - n)))) {
            status = INST_UNKNOWN;
        }
        if (n == R_PC) {
            status = UNSUPPORTED;
        }
        if (! GET_OPT_STAT(OPT_NOT_ALLOW_FIX_MEM)) {
            if (status != UNSUPPORTED) {
                /*
                status = FIXINST;
                insert_instr("[INSTINFO] type=MemA");
                */
                fix_mem_base(n, true);
            }
        } else {
            if (status != UNSUPPORTED) {
                uint32_t base_val = gen_pre_instr_for_mem_base(n);
                if (!is_mem_align(base_val, 2)) {
                    status = UNSUPPORTED;
                }
            }
        }
    } else if (glb_instr_tlb[op].op_kind == PUSH_OP) {
        assert(op == SISA_OP_PUSH_a1 || op == SISA_OP_PUSH_a2);
        if (op == SISA_OP_PUSH_a1) {
            register_list = instr & 0xffff;
            //SEE STMDB/STMFD
            if (bit_count(register_list) < 2) {
                status = ALIAS;
            }
            if (get_bit_value(register_list, R_SP) && (register_list & 0x1fff)) {
                status = INST_UNKNOWN;
            }
        }
        if (op == SISA_OP_PUSH_a2) {
            if ((instr & 0xf000) == 0xd000) {
                //if t == R_SP then UNPREDICTABLE
                status = UNPREDICTABLE;
            }
        }
        if (! GET_OPT_STAT(OPT_NOT_ALLOW_FIX_MEM)) {
            if (status != UNSUPPORTED) {
                /*
                status = FIXINST;
                if (op == SISA_OP_PUSH_a1) {
                    insert_instr("[INSTINFO] type=MemA");
                } else {
                    insert_instr("[INSTINFO] type=MemU");
                }
                */
                fix_mem_base(R_SP, true);
            }
        } else {
            if (status != UNSUPPORTED) {
                uint32_t base_val = gen_pre_instr_for_mem_base(R_SP);
                if (!is_mem_align(base_val, 2)) {
                    status = UNSUPPORTED;
                }
            }
        }
    } else if (glb_instr_tlb[op].op_kind == POP_OP) {
        assert(op == SISA_OP_POP_a1 || op == SISA_OP_POP_a2);
        if (op == SISA_OP_POP_a1) {
            register_list = instr & 0xffff;
            //SEE LDM/LDMIA/LDMFD
            if (bit_count(register_list) < 2) {
                status = ALIAS;
            }
            if (get_bit_value(register_list, R_SP)) {
                if (options.arch_version >=7) {
                    status = UNPREDICTABLE;
                } else {
                    status = INST_UNKNOWN;
                }
            }
            //pop pc
            if (get_bit_value(register_list, R_PC)) {
                status = UNSUPPORTED;
            }
        }
        if (op == SISA_OP_POP_a2) {
            if ((instr & 0xf000) == 0xd000) {
                //if t == R_SP then UNPREDICTABLE
                status = UNPREDICTABLE;
            }
            //pop pc
            if ((instr & 0xf000) == 0xf000) {
                status = UNSUPPORTED;
            }
        }
        if (! GET_OPT_STAT(OPT_NOT_ALLOW_FIX_MEM)) {
            if (status != UNSUPPORTED) {
                /*
                status = FIXINST;
                if (op == SISA_OP_POP_a1) {
                    insert_instr("[INSTINFO] type=MemA");
                } else {
                    insert_instr("[INSTINFO] type=MemU");
                }
                */
                fix_mem_base(R_SP, true);
            }
        } else {
        if (status != UNSUPPORTED) {
            uint32_t base_val = gen_pre_instr_for_mem_base(R_SP);
            if (!is_mem_align(base_val, 2)) {
                status = UNSUPPORTED;
            }
        }
        }

    } else {
        printf("Unsupported multi mem instruction %s\n", glb_instr_tlb[op].name);
        status = UNSUPPORTED;
    }
    is_oprand_init = true;
    return status;
}

instr_status_t gen_misc_instr(sisa_opcode_t op)
{
    instr_status_t status = VALID;
    uint32_t instr = get_raw_instr();

    if (op == SISA_OP_SVC) {
        status = UNSUPPORTED;
    } else if (op == SISA_OP_SWP || op == SISA_OP_SWPB) {
        uint32_t t, t2, n;
        t = get_regnum(instr, glb_instr_tlb[op].opnds[SISA_OUTPUT0]);
        t2 = get_regnum(instr, glb_instr_tlb[op].opnds[SISA_INPUT0]);
        n = get_regnum(instr, glb_instr_tlb[op].opnds[SISA_INPUT1]);

        if (GET_OPT_STAT(OPT_ALLOW_BIAS_OPND)) {
            random_bias_reg_2(&t, &t2);
            instr = set_instr_reg(instr, glb_instr_tlb[op].opnds[SISA_OUTPUT0], t);
            instr = set_instr_reg(instr, glb_instr_tlb[op].opnds[SISA_INPUT0], t2);
            set_raw_instr(instr);
        }

        if (t == R_PC || t2 == R_PC || n == R_PC || n == t || n == t2) {
            status = UNPREDICTABLE;
        }
        if (n == R_PC) {
            status = UNSUPPORTED;
        }
        if (status != UNSUPPORTED) {
            //uint32_t base_val = gen_pre_instr_for_mem_base(n);
            //if (!is_mem_align(base_val, 2)) {
            //    status = UNSUPPORTED;
            //}
            fix_mem_base(n, true);
        }
    } else if (op == SISA_OP_CLZ) {
        uint32_t d, m;
        d = get_regnum(instr, glb_instr_tlb[op].opnds[SISA_OUTPUT0]);
        m = get_regnum(instr, glb_instr_tlb[op].opnds[SISA_INPUT0]);
        if (GET_OPT_STAT(OPT_ALLOW_BIAS_OPND)) {
            random_bias_reg_2(&d, &m);
            instr = set_instr_reg(instr, glb_instr_tlb[op].opnds[SISA_OUTPUT0], d);
            instr = set_instr_reg(instr, glb_instr_tlb[op].opnds[SISA_INPUT0], m);
            set_raw_instr(instr);
        }

        if (d == R_PC || m == R_PC) {
            status = UNPREDICTABLE;
        }
    } else if (op == SISA_OP_ADR_a1 || op == SISA_OP_ADR_a2 || op == SISA_OP_MOVT) {
        //nothing to do for adr
        uint32_t d;
        d = get_regnum(instr, glb_instr_tlb[op].opnds[SISA_OUTPUT0]);
        if (d == R_PC) {
            status = UNSUPPORTED;
        }
    } else {
        printf("[WARNING] unknown op %s with misc_op\n", glb_instr_tlb[op].name);
        return UNSUPPORTED;
    }
    return status;
}

instr_status_t gen_data_instr(sisa_opcode_t op)
{
    instr_status_t status = VALID;
    uint32_t regnum;
    uint32_t instr = get_raw_instr();

    if (glb_instr_tlb[op].opnds[SISA_INPUT2].type == SISA_OPND_TYPE_SHIFT_REG) {
        //register shift register
        uint32_t d, m, n, s;
        if (glb_instr_tlb[op].opnds[SISA_OUTPUT0].type != SISA_OPND_TYPE_NONE) {
            d = get_regnum(instr, glb_instr_tlb[op].opnds[SISA_OUTPUT0]);
            n = get_regnum(instr, glb_instr_tlb[op].opnds[SISA_INPUT0]);
            m = get_regnum(instr, glb_instr_tlb[op].opnds[SISA_INPUT1]);
            s = get_regnum(instr, glb_instr_tlb[op].opnds[SISA_INPUT2]);
            if (d == R_PC || n == R_PC || m == R_PC || s == R_PC) {
                status = UNPREDICTABLE;
            } else {
                if (GET_OPT_STAT(OPT_ALLOW_BIAS_OPND)) {
                    random_bias_reg_4(&d, &n, &m, &s);
                    instr = set_instr_reg(instr, glb_instr_tlb[op].opnds[SISA_OUTPUT0], d);
                    instr = set_instr_reg(instr, glb_instr_tlb[op].opnds[SISA_INPUT0], n);
                    instr = set_instr_reg(instr, glb_instr_tlb[op].opnds[SISA_INPUT1], m);
                    instr = set_instr_reg(instr, glb_instr_tlb[op].opnds[SISA_INPUT2], s);
                    set_raw_instr(instr);
                }
            }
        } else {
            n = get_regnum(instr, glb_instr_tlb[op].opnds[SISA_INPUT0]);
            m = get_regnum(instr, glb_instr_tlb[op].opnds[SISA_INPUT1]);
            s = get_regnum(instr,glb_instr_tlb[op].opnds[SISA_INPUT2]);
            if (n == R_PC || m == R_PC || s == R_PC) {
                status = UNPREDICTABLE;
            } else {
                if (GET_OPT_STAT(OPT_ALLOW_BIAS_OPND)) {
                    random_bias_reg_3(&n, &m, &s);
                    instr = set_instr_reg(instr, glb_instr_tlb[op].opnds[SISA_INPUT0], n);
                    instr = set_instr_reg(instr, glb_instr_tlb[op].opnds[SISA_INPUT1], m);
                    instr = set_instr_reg(instr, glb_instr_tlb[op].opnds[SISA_INPUT2], s);
                    set_raw_instr(instr);
                }
            }
        }

    } else if (glb_instr_tlb[op].opnds[SISA_INPUT2].type == SISA_OPND_TYPE_SHIFT_IMM) {
        //reg
        if (GET_OPT_STAT(OPT_ALLOW_BIAS_OPND)) {
            uint32_t d, m, n;
            if (glb_instr_tlb[op].opnds[SISA_OUTPUT0].type != SISA_OPND_TYPE_NONE) {
                d = get_regnum(instr, glb_instr_tlb[op].opnds[SISA_OUTPUT0]);
                n = get_regnum(instr, glb_instr_tlb[op].opnds[SISA_INPUT0]);
                m = get_regnum(instr, glb_instr_tlb[op].opnds[SISA_INPUT1]);
                random_bias_reg_3(&d, &n, &m);
                instr = set_instr_reg(instr, glb_instr_tlb[op].opnds[SISA_OUTPUT0], d);
                instr = set_instr_reg(instr, glb_instr_tlb[op].opnds[SISA_INPUT0], n);
                instr = set_instr_reg(instr, glb_instr_tlb[op].opnds[SISA_INPUT1], m);
                set_raw_instr(instr);
            } else {
                n = get_regnum(instr, glb_instr_tlb[op].opnds[SISA_INPUT0]);
                m = get_regnum(instr, glb_instr_tlb[op].opnds[SISA_INPUT1]);
                random_bias_reg_2(&n, &m);
                instr = set_instr_reg(instr, glb_instr_tlb[op].opnds[SISA_INPUT0], n);
                instr = set_instr_reg(instr, glb_instr_tlb[op].opnds[SISA_INPUT1], m);
                set_raw_instr(instr);
            }
        }
    } else if (glb_instr_tlb[op].opnds[SISA_INPUT2].type == SISA_OPND_TYPE_NONE) {
        if (glb_instr_tlb[op].opnds[SISA_INPUT1].type == SISA_OPND_TYPE_SHIFT_REG) {
            //shift_reg
            uint32_t d, m, n;
            d = get_regnum(instr, glb_instr_tlb[op].opnds[SISA_OUTPUT0]);
            n = get_regnum(instr, glb_instr_tlb[op].opnds[SISA_INPUT0]);
            m = get_regnum(instr, glb_instr_tlb[op].opnds[SISA_INPUT1]);
            if (d == R_PC || n == R_PC || m == R_PC) {
                status = UNPREDICTABLE;
            } else {
                if (GET_OPT_STAT(OPT_ALLOW_BIAS_OPND)) {
                    uint32_t d, m, n;
                    d = get_regnum(instr, glb_instr_tlb[op].opnds[SISA_OUTPUT0]);
                    n = get_regnum(instr, glb_instr_tlb[op].opnds[SISA_INPUT0]);
                    m = get_regnum(instr, glb_instr_tlb[op].opnds[SISA_INPUT1]);
                    random_bias_reg_3(&d, &n, &m);
                    instr = set_instr_reg(instr, glb_instr_tlb[op].opnds[SISA_OUTPUT0], d);
                    instr = set_instr_reg(instr, glb_instr_tlb[op].opnds[SISA_INPUT0], n);
                    instr = set_instr_reg(instr, glb_instr_tlb[op].opnds[SISA_INPUT1], m);
                    set_raw_instr(instr);
                }
            }
        } else if (glb_instr_tlb[op].opnds[SISA_INPUT1].type == SISA_OPND_TYPE_SHIFT_IMM) {
            //shift_imm
            if (GET_OPT_STAT(OPT_ALLOW_BIAS_OPND)) {
                uint32_t d, m;
                d = get_regnum(instr, glb_instr_tlb[op].opnds[SISA_OUTPUT0]);
                m = get_regnum(instr, glb_instr_tlb[op].opnds[SISA_INPUT0]);
                random_bias_reg_2(&d, &m);
                instr = set_instr_reg(instr, glb_instr_tlb[op].opnds[SISA_OUTPUT0], d);
                instr = set_instr_reg(instr, glb_instr_tlb[op].opnds[SISA_INPUT0], m);
                set_raw_instr(instr);
            }
        } else if (glb_instr_tlb[op].opnds[SISA_INPUT1].type == SISA_OPND_TYPE_IMM_ARM_EXP) {
            //imm
            if (glb_instr_tlb[op].opnds[SISA_OUTPUT0].type != SISA_OPND_TYPE_NONE) {
                if (GET_OPT_STAT(OPT_ALLOW_BIAS_OPND)) {
                    uint32_t d, n;
                    d = get_regnum(instr, glb_instr_tlb[op].opnds[SISA_OUTPUT0]);
                    n = get_regnum(instr, glb_instr_tlb[op].opnds[SISA_INPUT0]);
                    random_bias_reg_2(&d, &n);
                    instr = set_instr_reg(instr, glb_instr_tlb[op].opnds[SISA_OUTPUT0], d);
                    instr = set_instr_reg(instr, glb_instr_tlb[op].opnds[SISA_INPUT0], n);
                    set_raw_instr(instr);
                }
            }
        } else {
            //LOG("Unsupported arm data instructions %s.\n", glb_instr_tlb[op].name);
            //status = UNSUPPORTED;   
        }
    } else {
        //LOG("Unsupported arm data instructions %s.\n", glb_instr_tlb[op].name);
        //status = UNSUPPORTED;
    }


    if (glb_instr_tlb[op].opnds[SISA_OUTPUT0].type != SISA_OPND_TYPE_NONE) {
        regnum = get_regnum(instr, glb_instr_tlb[op].opnds[SISA_OUTPUT0]);
        if (regnum == R_PC) {
            status = UNSUPPORTED;
        }
    }
    if (glb_instr_tlb[op].opnds[SISA_OUTPUT1].type != SISA_OPND_TYPE_NONE) {
        regnum = get_regnum(instr, glb_instr_tlb[op].opnds[SISA_OUTPUT1]);
        if (regnum == R_PC) {
            status = UNSUPPORTED;
        }
    }
    return status;
}

instr_status_t gen_mul_instr(sisa_opcode_t op)
{
    instr_status_t status = VALID;
    uint32_t instr = get_raw_instr();
    if (op == SISA_OP_MUL || op == SISA_OP_SMULBB || op == SISA_OP_SMULWB ||
            /*v7*/op == SISA_OP_SMUAD || op == SISA_OP_SMUSD || op == SISA_OP_SMMUL) {
        uint32_t d, m, n;
        d = get_regnum(instr, glb_instr_tlb[op].opnds[SISA_OUTPUT0]);
        n = get_regnum(instr, glb_instr_tlb[op].opnds[SISA_INPUT0]);
        m = get_regnum(instr, glb_instr_tlb[op].opnds[SISA_INPUT1]);

        if (GET_OPT_STAT(OPT_ALLOW_BIAS_OPND)) {
            random_bias_reg_3(&d, &m, &n);
            instr = set_instr_reg(instr, glb_instr_tlb[op].opnds[SISA_OUTPUT0], d);
            instr = set_instr_reg(instr, glb_instr_tlb[op].opnds[SISA_INPUT0], n);
            instr = set_instr_reg(instr, glb_instr_tlb[op].opnds[SISA_INPUT1], m);
            set_raw_instr(instr);
        }

        if (d == R_PC || n == R_PC || m == R_PC) {
            status = UNPREDICTABLE;
        }
        if (op == SISA_OP_MUL) {
            if (d == n && options.arch_version < 6) {
                status = UNPREDICTABLE;
            }
        }
        if (d == R_PC) {
            status = UNSUPPORTED;
        }
    } else if (op == SISA_OP_MLA || op == SISA_OP_SMLABB || op == SISA_OP_SMLAWB ||
            /*v7*/op == SISA_OP_MLS || op == SISA_OP_SMLAD || op == SISA_OP_SMLSD ||
            op == SISA_OP_SMMLA || op == SISA_OP_SMMLS) {
        uint32_t d, m, n, a;
        d = get_regnum(instr, glb_instr_tlb[op].opnds[SISA_OUTPUT0]);
        n = get_regnum(instr, glb_instr_tlb[op].opnds[SISA_INPUT0]);
        m = get_regnum(instr, glb_instr_tlb[op].opnds[SISA_INPUT1]);
        a = get_regnum(instr, glb_instr_tlb[op].opnds[SISA_INPUT2]);

        if (op == SISA_OP_SMLAD || op == SISA_OP_SMLSD || op == SISA_OP_SMMLA) {
            if (a == R_PC) {
                status = ALIAS;
            }
        }

        if (GET_OPT_STAT(OPT_ALLOW_BIAS_OPND)) {
            random_bias_reg_4(&d, &n, &m, &a);
            instr = set_instr_reg(instr, glb_instr_tlb[op].opnds[SISA_OUTPUT0], d);
            instr = set_instr_reg(instr, glb_instr_tlb[op].opnds[SISA_INPUT0], n);
            instr = set_instr_reg(instr, glb_instr_tlb[op].opnds[SISA_INPUT1], m);
            instr = set_instr_reg(instr, glb_instr_tlb[op].opnds[SISA_INPUT2], a);
            set_raw_instr(instr);
        }

        if (op == SISA_OP_SMLAD || op == SISA_OP_SMLSD || op == SISA_OP_SMMLA) {
            if (d == R_PC || n == R_PC || m == R_PC) {
                status = UNPREDICTABLE;
            }
        } else {
            if (d == R_PC || n == R_PC || m == R_PC || a == R_PC) {
                status = UNPREDICTABLE;
            }
        }
        if (op == SISA_OP_MLA) {
            if (d == n && options.arch_version < 6) {
                status = UNPREDICTABLE;
            }
        }
        if (d == R_PC) {
            status = UNSUPPORTED;
        }
    } else if (op == SISA_OP_SMLAL || op == SISA_OP_SMLALBB || op == SISA_OP_SMULL ||
            op == SISA_OP_UMLAL || op == SISA_OP_UMULL ||
            /*v7*/op == SISA_OP_UMAAL || op == SISA_OP_SMLALD || op == SISA_OP_SMLSLD) {
        uint32_t dLo, dHi, m, n;
        dHi = get_regnum(instr, glb_instr_tlb[op].opnds[SISA_OUTPUT0]);
        dLo = get_regnum(instr, glb_instr_tlb[op].opnds[SISA_OUTPUT1]);
        n = get_regnum(instr, glb_instr_tlb[op].opnds[SISA_INPUT0]);
        m = get_regnum(instr, glb_instr_tlb[op].opnds[SISA_INPUT1]);

        if (GET_OPT_STAT(OPT_ALLOW_BIAS_OPND)) {
            random_bias_reg_4(&dHi, &dLo, &n, &m);
            instr = set_instr_reg(instr, glb_instr_tlb[op].opnds[SISA_OUTPUT0], dHi);
            instr = set_instr_reg(instr, glb_instr_tlb[op].opnds[SISA_OUTPUT1], dLo);
            instr = set_instr_reg(instr, glb_instr_tlb[op].opnds[SISA_INPUT0], n);
            instr = set_instr_reg(instr, glb_instr_tlb[op].opnds[SISA_INPUT1], m);
            set_raw_instr(instr);
        }

        if (dLo == R_PC || dHi == R_PC || n == R_PC || m == R_PC) {
            status = UNPREDICTABLE;
        }
        if (dLo == dHi) {
            status = UNPREDICTABLE;
        }
        if (op == SISA_OP_SMLAL || op == SISA_OP_SMULL || op == SISA_OP_UMLAL || op == SISA_OP_UMULL) {
            if ((dHi == n || dLo == n) && options.arch_version < 6) {
                status = UNPREDICTABLE;
            }
        }
        if (dLo == R_PC || dHi == R_PC) {
            status = UNSUPPORTED;
        }
    } else {
        printf("[WARNING] unsupported op %s with mul_op\n", glb_instr_tlb[op].name);
        return UNSUPPORTED;
    }
    return status;
}

instr_status_t gen_jmp_instr(sisa_opcode_t op)
{
    instr_status_t status = VALID;
    uint32_t instr = get_raw_instr();
    if (op == SISA_OP_B_imm) {
        //BranchWritePC
        status = FIXINST;
        insert_instr("[INSTINFO] type=BranchWritePC target=ARM");
    } else if (op == SISA_OP_BL_imm_a1) {
        //targetInstrSet -> ARM
        //BranchWritePC
        status = FIXINST;
        insert_instr("[INSTINFO] type=BranchWritePC target=ARM");
    } else if (op == SISA_OP_BLX_imm_a2) {
        //targetInstrSet -> Thumb
        //BranchWritePC
        status = FIXINST;
        insert_instr("[INSTINFO] type=BranchWritePC target=Thumb");
    } else if (op == SISA_OP_BX) {
        //BXWritePC
        status = FIXINST;
        insert_instr("[INSTINFO] type=BXWritePC");
    } else if (op == SISA_OP_BLX_reg) {
        //BXWritePC
        uint32_t m;
        m = get_regnum(instr, glb_instr_tlb[op].opnds[SISA_INPUT0]);
        status = FIXINST;

        if (m == R_PC) {
            status = UNPREDICTABLE;
        }
        insert_instr("[INSTINFO] type=BXWritePC");
    } else {
        status = INVALID;
    }
    return status;
}

instr_status_t gen_parallel_add_sub_instr(sisa_opcode_t op)
{
    instr_status_t status = VALID;
    uint32_t instr = get_raw_instr();
    uint32_t n, d, m;
    d = get_regnum(instr, glb_instr_tlb[op].opnds[SISA_OUTPUT0]);
    n = get_regnum(instr, glb_instr_tlb[op].opnds[SISA_INPUT0]);
    m = get_regnum(instr, glb_instr_tlb[op].opnds[SISA_INPUT1]);

    if (GET_OPT_STAT(OPT_ALLOW_BIAS_OPND)) {
        random_bias_reg_3(&d, &n, &m); 
        instr = set_instr_reg(instr, glb_instr_tlb[op].opnds[SISA_OUTPUT0], d);
        instr = set_instr_reg(instr, glb_instr_tlb[op].opnds[SISA_INPUT0], n);
        instr = set_instr_reg(instr, glb_instr_tlb[op].opnds[SISA_INPUT1], m);
        set_raw_instr(instr);
    }    

    if (d == R_PC || n == R_PC || m == R_PC) {
        status = UNPREDICTABLE;
    }    

    if (d == R_PC) {
        status = UNSUPPORTED;
    } 

    return status;
}

instr_status_t gen_uncategorized_instr(sisa_opcode_t op)
{
    instr_status_t status = VALID;
    uint32_t instr = get_raw_instr();
    
    if (op == SISA_OP_SADD16 || op == SISA_OP_SASX || op == SISA_OP_SSAX ||
            op == SISA_OP_SSUB16 || op == SISA_OP_SADD8 || op == SISA_OP_SSUB8) {
        //signed parallel addition and subtraction
        status = gen_parallel_add_sub_instr(op);
    } else if (op == SISA_OP_QADD || op == SISA_OP_QSUB || op == SISA_OP_QDADD ||
            op == SISA_OP_QDSUB) {
        //sauturation addition and subtraction 
        status = gen_parallel_add_sub_instr(op);
    } else if (op == SISA_OP_QADD16 || op == SISA_OP_QASX || op == SISA_OP_QSAX || 
            op == SISA_OP_QSUB16 || op == SISA_OP_QADD8 || op == SISA_OP_QSUB8) {
        //signed parallel addition and subtraction (saturation)
        status = gen_parallel_add_sub_instr(op);
    } else if (op == SISA_OP_SHADD16 || op == SISA_OP_SHASX || op == SISA_OP_SHSAX ||
            op == SISA_OP_SHSUB16 || op == SISA_OP_SHADD8 || op == SISA_OP_SHSUB8) {
        //signed parallel addition and subtraction (halving) 
        status = gen_parallel_add_sub_instr(op);
    } else if (op == SISA_OP_UADD16 || op == SISA_OP_UASX || op == SISA_OP_USAX ||
            op == SISA_OP_USUB16 || op == SISA_OP_UADD8 || op == SISA_OP_USUB8) {
        //unsigned parallel addition and subtraction
        status = gen_parallel_add_sub_instr(op);
    } else if (op == SISA_OP_UQADD16 || op == SISA_OP_UQASX || op == SISA_OP_UQSAX ||
            op == SISA_OP_UQSUB16 || op == SISA_OP_UQADD8 || op == SISA_OP_UQSUB8) {
        //unsigned parallel addition and subtraction (saturation)
        status = gen_parallel_add_sub_instr(op);
    } else if (op == SISA_OP_UHADD16 || op == SISA_OP_UHASX || op == SISA_OP_UHSAX ||
            op == SISA_OP_UHSUB16 || op == SISA_OP_UHADD8 || op == SISA_OP_UHSUB8) {
        //unsigned parallel addition and subtraction (halving)
        status = gen_parallel_add_sub_instr(op);
    
    } else if (op == SISA_OP_SXTAB || op == SISA_OP_SXTAB16 || op == SISA_OP_SXTAH ||
            op == SISA_OP_UXTAB || op == SISA_OP_UXTAB16 || op == SISA_OP_UXTAH) {
        //extend and add byte
        uint32_t d, n, m;
        d = get_regnum(instr, glb_instr_tlb[op].opnds[SISA_OUTPUT0]);
        n = get_regnum(instr, glb_instr_tlb[op].opnds[SISA_INPUT0]);
        m = get_regnum(instr, glb_instr_tlb[op].opnds[SISA_INPUT1]);

        if (GET_OPT_STAT(OPT_ALLOW_BIAS_OPND)) {
            random_bias_reg_3(&d, &n, &m); 
            instr = set_instr_reg(instr, glb_instr_tlb[op].opnds[SISA_OUTPUT0], d);
            instr = set_instr_reg(instr, glb_instr_tlb[op].opnds[SISA_INPUT0], n);
            instr = set_instr_reg(instr, glb_instr_tlb[op].opnds[SISA_INPUT1], m);
            set_raw_instr(instr);
        }
        
        if (n == R_PC) {
            status = ALIAS;
        }

        if (d == R_PC || m == R_PC) {
            status = UNPREDICTABLE;
        }

        if (d == R_PC) {
            status = UNSUPPORTED;
        }

    } else if (op == SISA_OP_SXTB || op == SISA_OP_SXTB16 || op == SISA_OP_SXTH ||
            op == SISA_OP_UXTB || op == SISA_OP_UXTB16 || op == SISA_OP_UXTH) {
        //extend byte
        uint32_t d, m;
        d = get_regnum(instr, glb_instr_tlb[op].opnds[SISA_OUTPUT0]);
        m = get_regnum(instr, glb_instr_tlb[op].opnds[SISA_INPUT0]);
        if (GET_OPT_STAT(OPT_ALLOW_BIAS_OPND)) {
            random_bias_reg_2(&d, &m);
            instr = set_instr_reg(instr, glb_instr_tlb[op].opnds[SISA_OUTPUT0], d);
            instr = set_instr_reg(instr, glb_instr_tlb[op].opnds[SISA_OUTPUT0], m);
            set_raw_instr(instr);
        }

        if (d == R_PC || m == R_PC) {
            status = UNPREDICTABLE;
        }

        if (d == R_PC) {
            status = UNSUPPORTED;
        }

    } else if (op == SISA_OP_RBIT || op == SISA_OP_REV || op == SISA_OP_REV16 || op == SISA_OP_REVSH) {
        //reverse
        uint32_t d, m;
        d = get_regnum(instr, glb_instr_tlb[op].opnds[SISA_OUTPUT0]);
        m = get_regnum(instr, glb_instr_tlb[op].opnds[SISA_INPUT0]);

        if (d == R_PC || m == R_PC) {
            status = UNPREDICTABLE;
        }

        if (d == R_PC) {
            status = UNSUPPORTED;
        }
    } else if (op == SISA_OP_SSAT || op == SISA_OP_USAT || op == SISA_OP_SSAT16 || op == SISA_OP_USAT16) {
        //saturate
        uint32_t d, n;
        d = get_regnum(instr, glb_instr_tlb[op].opnds[SISA_OUTPUT0]);
        n = get_regnum(instr, glb_instr_tlb[op].opnds[SISA_INPUT1]);

        if (GET_OPT_STAT(OPT_ALLOW_BIAS_REG)) {
            random_bias_reg_2(&d, &n);
            instr = set_instr_reg(instr, glb_instr_tlb[op].opnds[SISA_OUTPUT0], d);
            instr = set_instr_reg(instr, glb_instr_tlb[op].opnds[SISA_INPUT0], n);
            set_raw_instr(instr);
        }

        if (GET_OPT_STAT(OPT_ALLOW_BIAS_OPND)) {
            if (op == SISA_OP_SSAT || op == SISA_OP_USAT) {
                uint32_t rand_val = rand();
                instr = set_instr(instr, 7, 11, shift_imm[rand_val % shift_imm_num]);
                set_raw_instr(instr);
            }
        }

        if (d == R_PC || n == R_PC) {
            status = UNPREDICTABLE;
        }
    } else if (op == SISA_OP_PKH) {
        //pack
        uint32_t d, n, m;
        d = get_regnum(instr, glb_instr_tlb[op].opnds[SISA_OUTPUT0]);
        n = get_regnum(instr, glb_instr_tlb[op].opnds[SISA_INPUT0]);
        m = get_regnum(instr, glb_instr_tlb[op].opnds[SISA_INPUT1]);

        if (GET_OPT_STAT(OPT_ALLOW_BIAS_REG)) {
            random_bias_reg_3(&d, &n, &m);
            instr = set_instr_reg(instr, glb_instr_tlb[op].opnds[SISA_OUTPUT0], d);
            instr = set_instr_reg(instr, glb_instr_tlb[op].opnds[SISA_INPUT0], n);
            instr = set_instr_reg(instr, glb_instr_tlb[op].opnds[SISA_INPUT1], m);
            set_raw_instr(instr);
        }

        if (GET_OPT_STAT(OPT_ALLOW_BIAS_OPND)) {
            uint32_t rand_val = rand();
            instr = set_instr(instr, 7, 11, shift_imm[rand_val % shift_imm_num]);
            set_raw_instr(instr);
        }

        if (d == R_PC || n == R_PC || m == R_PC) {
            status = UNPREDICTABLE;
        }

        if (d == R_PC) {
            status = UNSUPPORTED;
        }

    } else if (op == SISA_OP_SBFX || op == SISA_OP_UBFX) {
        uint32_t d, n, lsbit, widthm1;
        d = get_regnum(instr, glb_instr_tlb[op].opnds[SISA_OUTPUT0]);
        n = get_regnum(instr, glb_instr_tlb[op].opnds[SISA_INPUT0]);
        lsbit = get_bits_value(instr, 7, 11);
        widthm1 = get_bits_value(instr, 16, 20);

        if (GET_OPT_STAT(OPT_ALLOW_BIAS_REG)) {
            random_bias_reg_2(&d, &n);
            instr = set_instr_reg(instr, glb_instr_tlb[op].opnds[SISA_OUTPUT0], d);
            instr = set_instr_reg(instr, glb_instr_tlb[op].opnds[SISA_INPUT0], n);
            set_raw_instr(instr);
        }

        if (d == R_PC || n == R_PC) {
            status = UNPREDICTABLE;
        }
        if ((lsbit + widthm1) > 31) {
            status = UNPREDICTABLE;
        }

        if (d == R_PC) {
            status = UNSUPPORTED;
        }
    } else if (op == SISA_OP_BFC) {
        uint32_t d, msbit, lsbit;
        d = get_regnum(instr, glb_instr_tlb[op].opnds[SISA_OUTPUT0]);
        msbit = get_bits_value(instr, 16, 20);
        lsbit = get_bits_value(instr, 7, 11);

        if (d == R_PC) {
            status = UNPREDICTABLE;
            status = UNSUPPORTED;
        }
        if (msbit < lsbit) {
            status = UNPREDICTABLE;
        }
    } else if (op == SISA_OP_BFI) {
        uint32_t d, n, msb, lsb;
        d = get_regnum(instr, glb_instr_tlb[op].opnds[SISA_OUTPUT0]);
        n = get_regnum(instr, glb_instr_tlb[op].opnds[SISA_INPUT2]);
        msb = get_bits_value(instr, 16, 20);
        lsb = get_bits_value(instr, 7, 11);

        if (GET_OPT_STAT(OPT_ALLOW_BIAS_REG)) {
            random_bias_reg_2(&d, &n);
            instr = set_instr_reg(instr, glb_instr_tlb[op].opnds[SISA_OUTPUT0], d);
            instr = set_instr_reg(instr, glb_instr_tlb[op].opnds[SISA_INPUT2], n);
            set_raw_instr(instr);
        }

        if (n == R_PC) {
            status = ALIAS;
        }
        if (msb < lsb) {
            status = UNPREDICTABLE;
        }
        if (d == R_PC) {
            status = UNPREDICTABLE;
            status = UNSUPPORTED;
        }
    } else if (op == SISA_OP_USAD8) {
        uint32_t n, d, m;
        d = get_regnum(instr, glb_instr_tlb[op].opnds[SISA_OUTPUT0]);
        n = get_regnum(instr, glb_instr_tlb[op].opnds[SISA_INPUT0]);
        m = get_regnum(instr, glb_instr_tlb[op].opnds[SISA_INPUT1]);

        if (GET_OPT_STAT(OPT_ALLOW_BIAS_REG)) {
            random_bias_reg_3(&d, &n, &m); 
            instr = set_instr_reg(instr, glb_instr_tlb[op].opnds[SISA_OUTPUT0], d);
            instr = set_instr_reg(instr, glb_instr_tlb[op].opnds[SISA_INPUT0], n);
            instr = set_instr_reg(instr, glb_instr_tlb[op].opnds[SISA_INPUT1], m);
            set_raw_instr(instr);
        }    

        if (d == R_PC || n == R_PC || m == R_PC) {
            status = UNPREDICTABLE;
        }    

        if (d == R_PC) {
            status = UNSUPPORTED;
        }    
    } else if (op == SISA_OP_USADA8) {
        uint32_t n, d, m, a;
        d = get_regnum(instr, glb_instr_tlb[op].opnds[SISA_OUTPUT0]);
        n = get_regnum(instr, glb_instr_tlb[op].opnds[SISA_INPUT0]);
        m = get_regnum(instr, glb_instr_tlb[op].opnds[SISA_INPUT1]);
        a = get_regnum(instr, glb_instr_tlb[op].opnds[SISA_INPUT2]);

        if (GET_OPT_STAT(OPT_ALLOW_BIAS_REG)) {
            random_bias_reg_4(&d, &n, &m, &a); 
            instr = set_instr_reg(instr, glb_instr_tlb[op].opnds[SISA_OUTPUT0], d);
            instr = set_instr_reg(instr, glb_instr_tlb[op].opnds[SISA_INPUT0], n);
            instr = set_instr_reg(instr, glb_instr_tlb[op].opnds[SISA_INPUT1], m);
            instr = set_instr_reg(instr, glb_instr_tlb[op].opnds[SISA_INPUT2], a);
            set_raw_instr(instr);
        }    

        if (a == R_PC) {
            status = ALIAS;
        }
        if (d == R_PC || n == R_PC || m == R_PC) {
            status = UNPREDICTABLE;
        }    

        if (d == R_PC) {
            status = UNSUPPORTED;
        }    
    } else if (op == SISA_OP_SEL) {
        uint32_t n, d, m;
        d = get_regnum(instr, glb_instr_tlb[op].opnds[SISA_OUTPUT0]);
        n = get_regnum(instr, glb_instr_tlb[op].opnds[SISA_INPUT0]);
        m = get_regnum(instr, glb_instr_tlb[op].opnds[SISA_INPUT1]);
        if (d == R_PC || n == R_PC || m == R_PC) {
            status = UNPREDICTABLE;
        }
    } else if (op == SISA_OP_MRS_a) {
        uint32_t d;
        d = get_regnum(instr, glb_instr_tlb[op].opnds[SISA_OUTPUT0]);

        if (d == R_PC) {
            status = UNPREDICTABLE;
        }
    } else if (op == SISA_OP_MSR_imm_a) {
        uint32_t mask;
        mask = get_bits_value(instr, 18, 19);
        if (mask == 0) {
            //if mask == '00' then SEE "Related encodings"
            status = UNSUPPORTED;
        }
    } else if (op == SISA_OP_MSR_reg_a) {
        uint32_t n, mask;
        n = get_regnum(instr, glb_instr_tlb[op].opnds[SISA_INPUT0]);
        mask = get_bits_value(instr, 18, 19);

        if (mask == 0) {
            status = UNPREDICTABLE;
        }
        if (n == R_PC) {
            status = UNPREDICTABLE;
        }

    } else if (op == SISA_OP_PLI || op == SISA_OP_PLD || op == SISA_OP_PLI_imm || 
            op == SISA_OP_PLD_imm || op == SISA_OP_PLD_lit) {
        if (op == SISA_OP_PLD_imm) {
            uint32_t n = get_bits_value(instr, 16, 19);
            if (n == R_PC) {
                status = ALIAS;
            }
        } else if (op == SISA_OP_PLI) {
            uint32_t m = get_bits_value(instr, 0, 3);
            if (m == R_PC) {
                status = UNPREDICTABLE;
            }
        } else if (op == SISA_OP_PLD) {
            uint32_t n = get_bits_value(instr, 16, 19);
            uint32_t m = get_bits_value(instr, 0, 3);
            bool r = get_bit_value(instr, 22);
            if (m == R_PC || (r == 0 && n == R_PC)) {
                status = UNPREDICTABLE;
            }
        }
        //PLD_lit / PLI_imm
    } else {
        status = UNSUPPORTED;
    }
    return status;
}

instr_status_t gen_unsup_instr(sisa_opcode_t op)
{
    return UNSUPPORTED;
}

instr_status_t gen_ldex_instr(sisa_opcode_t op)
{
    instr_status_t status = VALID;
    uint32_t instr = get_raw_instr();
    uint32_t t = get_regnum(instr, glb_instr_tlb[op].opnds[SISA_OUTPUT0]);
    uint32_t n = get_regnum(instr, glb_instr_tlb[op].opnds[SISA_INPUT0]);
    if (t == R_PC || n == R_PC) {
        status = UNPREDICTABLE;
    }
    if (op == SISA_OP_LDREXD) {
        if (t == 14 || (t % 2) == 1) {
            status = UNPREDICTABLE;
        }
    }
    fix_mem_base(n, true);
    return status;
}

instr_status_t gen_empty_instr(sisa_opcode_t op)
{
    if (op == SISA_OP_WFI) {
        return UNSUPPORTED; 
    } else if (op == SISA_OP_CLREX) {
        //TODO clrex support
        return UNSUPPORTED;
    } else {
        return VALID;
    }
 }

instr_status_t gen_stex_instr(sisa_opcode_t op)
{
    instr_status_t status = VALID;
    uint32_t instr = get_raw_instr();
    uint32_t d = get_regnum(instr, glb_instr_tlb[op].opnds[SISA_OUTPUT0]);
    uint32_t t = get_regnum(instr, glb_instr_tlb[op].opnds[SISA_INPUT0]);
    uint32_t n = get_regnum(instr, glb_instr_tlb[op].opnds[SISA_INPUT1]);
    if (d == R_PC || t == R_PC || n == R_PC) {
        status = UNPREDICTABLE;
    }
    if (d == n || d == t) {
        status = UNPREDICTABLE;
    }
    if (op == SISA_OP_STREXD) {
        uint32_t t2 = t + 1;
        if (t2 == R_PC || d == t2 || (t % 2) == 1) {
            status = UNPREDICTABLE;
        }
    }
    fix_mem_base(n, true);
    return status;
}
