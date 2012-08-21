#include <stdio.h>
#include "GenThumbInstr.h"
#include "InstrUtil.h"
#include "GenHelper.h"
#include "BiasUtil.h"
#include "RandomInstrGen.h"

extern gen_option_t options;
extern bool is_oprand_init;
static char gen_instr_buf[MAX_STR_INSTR_LEN+1];

instr_status_t gen_thumb_mem_instr(sisa_opcode_t op)
{
    instr_status_t status = VALID;
    uint32_t instr = get_raw_instr();
    if (op == SISA_OP_LDR_imm_T1 || op == SISA_OP_LDR_imm_T2 ||
            op == SISA_OP_LDRB_imm_T1 || op == SISA_OP_LDRH_imm_T1) {
        uint32_t n;
        n = get_regnum(instr, glb_instr_tlb[op].opnds[SISA_INPUT0]);
        if (options.fix_mem) {
            fix_mem_base(n, false);
        } else {
            uint32_t base_addr = gen_pre_instr_for_mem_base(n);
            printf("base %x\n", base_addr);
            if (op == SISA_OP_LDR_imm_T1 || op == SISA_OP_LDR_imm_T2) {
                if ((base_addr & 3) && (options.arch_version < 7)) {
                    status = INST_UNKNOWN;
                }
            }
            if (op == SISA_OP_LDRH_imm_T1) {
                if ((base_addr & 1) && (options.arch_version < 7)) {
                    status = INST_UNKNOWN;
                }
            }
        }

    } else if (op == SISA_OP_STR_imm_T1 || op == SISA_OP_STR_imm_T2 ||
            op == SISA_OP_STRB_imm_T1 || op == SISA_OP_STRH_imm_T1) {
        uint32_t n;
        n = get_regnum(instr, glb_instr_tlb[op].opnds[SISA_INPUT1]);
        if (options.fix_mem) {
            fix_mem_base(n, false);
        } else {

            uint32_t base_addr = gen_pre_instr_for_mem_base(n);
            if (op == SISA_OP_STR_imm_T1 || op == SISA_OP_STR_imm_T2) {
                if ((base_addr & 3) && (options.arch_version < 7)) {
                    status = INST_UNKNOWN;
                }
            }
            if (op == SISA_OP_STRH_imm_T1) {
                if ((base_addr & 1) && (options.arch_version < 7)) {
                    status = INST_UNKNOWN;
                }
            }
        }

    } else if (op == SISA_OP_LDR_reg_T1 || op == SISA_OP_LDRB_reg_T1 ||
            op == SISA_OP_LDRH_reg_T1 || op == SISA_OP_LDRSH_reg_T1 ||
            op == SISA_OP_LDRSB_reg_T1) {
        uint32_t n, m;
        //t = get_regnum(instr, glb_instr_tlb[op].opnds[SISA_OUTPUT0]);
        n = get_regnum(instr, glb_instr_tlb[op].opnds[SISA_INPUT0]);
        m = get_regnum(instr, glb_instr_tlb[op].opnds[SISA_INPUT1]);
        uint32_t reg_offset;
        if (m == n) {
            //TODO base_reg == offset_reg
            status = UNSUPPORTED;
        }
        if (status != UNSUPPORTED) {
            if (options.fix_mem) {
                fix_mem_base(n, false);
                sprintf(gen_instr_buf, "[INST] ldr r%d, =0x%x", m, rand() & options.mem_offset_val);
                insert_instr(gen_instr_buf);
            } else {
                uint32_t base_val = gen_pre_instr_for_mem_base(n);
                uint32_t offset_val = get_rand_mem_offset(0, 0, &reg_offset);
                sprintf(gen_instr_buf, "[INST] ldr r%d, =0x%x", m, offset_val);
                insert_instr(gen_instr_buf);
                uint32_t offset_addr = get_mem_addr(base_val, offset_val, true, true);
                if (op == SISA_OP_LDR_reg_T1) {
                    if ((offset_addr & 3) && (options.arch_version < 7)) {
                        status = INST_UNKNOWN;
                    }
                }
                if (op == SISA_OP_LDRH_reg_T1 || op == SISA_OP_LDRSH_reg_T1) {
                    if ((offset_addr & 1) && (options.arch_version < 7)) {
                        status = INST_UNKNOWN;
                    }
                }
            }
        }
    } else if (op == SISA_OP_STR_reg_T1 || op == SISA_OP_STRH_reg_T1 ||
            op == SISA_OP_STRB_reg_T1) {
        uint32_t n, m;
        //t = get_regnum(instr, glb_instr_tlb[op].opnds[SISA_INPUT0]);
        n = get_regnum(instr, glb_instr_tlb[op].opnds[SISA_INPUT1]);
        m = get_regnum(instr, glb_instr_tlb[op].opnds[SISA_INPUT2]);
        uint32_t reg_offset;
        if (m == n) {
            //TODO base_reg == offset_reg
            status = UNSUPPORTED;
        }
        if (status != UNSUPPORTED) {
            if (options.fix_mem) {
                fix_mem_base(n, false);
                sprintf(gen_instr_buf, "[INST] ldr r%d, =0x%x", m, rand() & options.mem_offset_val);
                insert_instr(gen_instr_buf);
            } else {
                uint32_t base_val = gen_pre_instr_for_mem_base(n);
                uint32_t offset_val = get_rand_mem_offset(0, 0, &reg_offset);
                sprintf(gen_instr_buf, "[INST] ldr r%d, =0x%x", m, offset_val);
                insert_instr(gen_instr_buf);
                uint32_t offset_addr = get_mem_addr(base_val, offset_val, true, true);
                if (op == SISA_OP_STR_reg_T1) {
                    if ((offset_addr & 3) && (options.arch_version < 7)) {
                        status = INST_UNKNOWN;
                    }
                }
                if (op == SISA_OP_STRH_reg_T1) {
                    if ((offset_addr & 1) && (options.arch_version < 7)) {
                        status = INST_UNKNOWN;
                    }
                }
            }
        }
    } else if (op == SISA_OP_LDR_lit_T1) {
        status = VALID;
    } else {
        printf("[TODO] thumb mem instruction %s\n", glb_instr_tlb[op].name);
        status = UNSUPPORTED;
    }
    is_oprand_init = true;
    return status;
}

instr_status_t gen_thumb_multi_mem_instr(sisa_opcode_t op)
{
    instr_status_t status = VALID;
    uint32_t n;
    uint32_t instr = get_raw_instr();
    uint32_t mem_addr = 0;
    if (op == SISA_OP_LDM_T1) {
        n = get_regnum(instr, glb_instr_tlb[op].opnds[SISA_INPUT0]);
        //BitCount(registers) < 1
        if ((instr & 0xff) == 0) {
            status = UNPREDICTABLE;
        }
        if (options.fix_mem) {
            fix_mem_base(n, true);
        } else {
            mem_addr = gen_pre_instr_for_mem_base(n);
        }
    } else if (op == SISA_OP_STM_T1) {
        n = get_regnum(instr, glb_instr_tlb[op].opnds[SISA_INPUT0]);
        //BitCount(registers) < 1
        if ((instr & 0xff) == 0) {
            status = UNPREDICTABLE;
        }
        if ((get_bit_value(instr, n) != 0) && ((instr & (0xff >> (8 - n))) != 0)) {
            status = INST_UNKNOWN;
        }

        //due to qemu bug, skip this kind of instruction
        if ((get_bit_value(instr, n) != 0)) {
            status = UNSUPPORTED;
        }
        if (options.fix_mem) {
            fix_mem_base(n, true);
        } else {
            mem_addr = gen_pre_instr_for_mem_base(n);
        }
    } else if (op == SISA_OP_POP_T) {
        if ((instr & 0xff) == 0) {
            status = UNPREDICTABLE;
        }
        if (get_bit_value(instr, 8)) {
            //TODO if registers<15> == '1' && InITBlock() && !LastInITBlock() then UNPREDICTABLE
            //LoadWritePC
            status = UNSUPPORTED;
        }
        if (options.fix_mem) {
            fix_mem_base(13, true);
        } else {
            mem_addr = gen_pre_instr_for_mem_base(13);
        }
    } else if (op == SISA_OP_PUSH_T) {
        if ((instr & 0xff) == 0) {
            status = UNPREDICTABLE;
        }
        if (options.fix_mem) {
            fix_mem_base(13, true);
        } else {
            mem_addr = gen_pre_instr_for_mem_base(13);
        }
    } else {
        status = INVALID;
    }   

    if (!is_mem_align(mem_addr, 2) && !options.fix_mem) {
        status = UNSUPPORTED;
    }

    is_oprand_init = true;
    return status;
}

instr_status_t gen_thumb_misc_instr(sisa_opcode_t op) 
{
    if (op == SISA_OP_SVC_T1) {
        return UNSUPPORTED;
    } else if (op == SISA_OP_ADR_T1) {
        return VALID;
    } else {
        return INVALID;
    }   
}

instr_status_t gen_thumb_data_instr(sisa_opcode_t op) 
{
    instr_status_t status = VALID;    
    uint32_t instr = get_raw_instr();
    uint32_t regnum;

    if (op == SISA_OP_CMP_reg_T2) {
        uint32_t m,n;
        n = get_regnum(instr, glb_instr_tlb[op].opnds[SISA_INPUT0]);
        m = get_regnum(instr, glb_instr_tlb[op].opnds[SISA_INPUT1]);
        if (n < 8 && m < 8) {
            status = UNPREDICTABLE;
        }
        if (n == 15 || m == 15) {
            status = UNPREDICTABLE;
        }
    } else {
        if (glb_instr_tlb[op].opnds[SISA_OUTPUT0].type != SISA_OPND_TYPE_NONE) {
            regnum = get_regnum(instr, glb_instr_tlb[op].opnds[SISA_OUTPUT0]);
            if (regnum == 15) {
                status = UNSUPPORTED;
            }    
        }    
        if (glb_instr_tlb[op].opnds[SISA_OUTPUT1].type != SISA_OPND_TYPE_NONE) {
            regnum = get_regnum(instr, glb_instr_tlb[op].opnds[SISA_OUTPUT1]);
            if (regnum == 15) {
                status = UNSUPPORTED;
            }    
        }
    }
    return status;
}

instr_status_t gen_thumb_mul_instr(sisa_opcode_t op)
{
    instr_status_t status = VALID;
    uint32_t instr = get_raw_instr();
    if (op == SISA_OP_MUL_reg_T1) {
        uint32_t dm, n;
        dm = get_regnum(instr, glb_instr_tlb[op].opnds[SISA_OUTPUT0]);
        n = get_regnum(instr, glb_instr_tlb[op].opnds[SISA_INPUT0]);
        if ((options.arch_version < 6) && (dm == n)) {
            status = UNPREDICTABLE; 
        }   
    } else {
        status = INVALID;
    }   
    return status;
}   

instr_status_t gen_thumb_jmp_instr(sisa_opcode_t op)
{
    instr_status_t status = VALID;
    uint32_t instr = get_raw_instr();
    if (op == SISA_OP_B_T1 || op == SISA_OP_B_T2 || op == SISA_OP_CBZ_T1 || op == SISA_OP_CBNZ_T1) {
        //BranchWritePC
        status = FIXINST;
        if (op == SISA_OP_B_T1) {
            uint32_t cond = get_bits_value(instr, 8, 11);
            if (cond == 14) {
                //if cond == '1110' then UNDEFINED;
                status = UNDEFINED;
            }
            if (cond == 15) {
                //if cond == '1111' then SEE SVC;
                status = ALIAS;
            }
        }
        insert_instr("[INSTINFO] type=BranchWritePC target=Thumb");
    } else if (op == SISA_OP_BL_imm_T1) {
        //TODO 32-bit thumb
        //targetInstrSet -> Thumb
        //BranchWritePC
        status = FIXINST;
        insert_instr("[INSTINFO] type=BranchWritePC target=Thumb");
    } else if (op == SISA_OP_BLX_imm_T2) {
        //TODO 32-bit thumb
        //targetInstrSet -> ARM
        //BranchWritePC
        status = FIXINST;
        insert_instr("[INSTINFO] type=BranchWritePC target=ARM");
    } else if (op == SISA_OP_BX_T1) {
        //BXWritePC
        status = FIXINST;
        insert_instr("[INSTINFO] type=BXWritePC");
    } else if (op == SISA_OP_BLX_reg_T1) {
        //BXWritePC
        uint32_t m;
        m = get_regnum(instr, glb_instr_tlb[op].opnds[SISA_INPUT0]);
        status = FIXINST;

        if (m == 15) {
            status = UNPREDICTABLE;
        }
        insert_instr("[INSTINFO] type=BXWritePC");
    } else {
        status = INVALID;
    }
    return status;
}

instr_status_t gen_thumb_unsup_instr(sisa_opcode_t op)
{
    return INVALID;
}
instr_status_t gen_thumb_empty_instr(sisa_opcode_t op)
{
    if (op == SISA_OP_WFI_T1) {
        //different behavior for QEMU
        return UNSUPPORTED;
    } else {
        return VALID;
    }
}

instr_status_t gen_thumb_ext_instr(sisa_opcode_t op)
{
    return VALID;
}

instr_status_t gen_thumb_reverse_instr(sisa_opcode_t op)
{
    return VALID;
}
