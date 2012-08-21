#include "GenHelper.h"
#include <stdio.h>
#include <stdlib.h>
#include "RandomInstrGen.h"

extern gen_option_t options; 

bool is_imm_encodable(uint32_t imm)
{
    bool status = false;
    if (options.isa == ISA_ARMv5 || options.isa == ISA_ARMv7) {
        uint32_t cnt = 0;
        int idx;
        for (idx = 0; idx < 32; idx++) {
            if (imm & (1 << idx)) 
                break;
            else 
                cnt++;
        }
        for (idx = 0; idx < 32; idx++) {
            if (imm & (1 << (31 - idx))) 
                break;
            else 
                cnt++;
        }
        if (cnt >= 24) {
            status = true;
        } else {
            //printf("zero_cnt %d 0x%x\n", cnt, imm);
            cnt = 0;
            for (idx = 0; idx < 16; idx++) {
                if (imm & (1 << (15 - idx))) 
                    break;
                else 
                    cnt++;
            }
            for (idx = 0; idx < 16; idx++) {
                if (imm & (1 << (16 + idx))) 
                    break;
                else 
                    cnt++;
            }
            if (cnt >=24) 
                status = true;
            //else
            //    printf("zero_cnt %d 0x%x\n", cnt, imm);
        }
    }
    return status;
}

uint32_t gen_pre_instr_for_mem_base(uint32_t reg_base)
{
    char gen_instr_buf[MAX_STR_INSTR_LEN+1];
    uint32_t base_rand;
    assert(reg_base != 15);
    uint32_t tmp_reg;
    
    if (reg_base != 0) {
        tmp_reg = 0;
    } else {
        tmp_reg = 1;
    }   
    
    
    if (options.isa == ISA_THUMB && reg_base > 7) {
        sprintf(gen_instr_buf, "[CODE] ldr r%d, =mem_data", tmp_reg);
        insert_instr(gen_instr_buf);
        sprintf(gen_instr_buf, "[CODE] mov r%d, r%d", reg_base, tmp_reg);
        insert_instr(gen_instr_buf);
    } else {
        sprintf(gen_instr_buf, "[CODE] ldr r%d, =mem_data", reg_base);
        insert_instr(gen_instr_buf);
    }   
    
    //base_rand = rand() % (options.mem_base * 4) + (options.max_mem_offset * 4);
    //TODO
    base_rand = 0x600000;
    if (!options.allow_unalign_mem) {
        //trend to choose aligned base register
        if ((rand() % 8) != 0) {
            base_rand = (base_rand / 4) * 4;
        }
    }
    sprintf(gen_instr_buf, "[CODE] ldr r%d, =0x%x", tmp_reg, base_rand);
    //printf("Gen instr [%d]: %s\n", instr_idx, instr_buf[instr_idx]);
    insert_instr(gen_instr_buf);
    sprintf(gen_instr_buf, "[CODE] add r%d, r%d, r%d", reg_base, reg_base, tmp_reg);
    //printf("Gen instr [%d]: %s\n", instr_idx, instr_buf[instr_idx]);
    insert_instr(gen_instr_buf);
    return base_rand;
}

void fix_mem_base(uint32_t reg_base, bool forceAlign)
{
    char gen_instr_buf[MAX_STR_INSTR_LEN+1];
    uint32_t unset_mask = (~options.mem_base_val) & options.mem_base_mask;
    uint32_t set_mask = options.mem_base_val & options.mem_base_mask;
    uint32_t tmp_reg;
    uint32_t reg_base_t = 0;
    if (reg_base != 0) {
        tmp_reg = 0;
    } else {
        tmp_reg = 1;
    }
    if (options.isa == ISA_THUMB && reg_base > 7) {
        reg_base_t = reg_base;
        reg_base = 2;
        sprintf(gen_instr_buf, "[INST] mov r%d, r%d", reg_base, reg_base_t);
        insert_instr(gen_instr_buf);
    }

    if (forceAlign) {
        unset_mask |= 0x3;
    }
    
    if (options.isa == ISA_THUMB) {
        sprintf(gen_instr_buf, "[CODE] ldr r%d, =0x%x", tmp_reg, unset_mask);
        insert_instr(gen_instr_buf);
        sprintf(gen_instr_buf, "[CODE] bics r%d, r%d", reg_base, tmp_reg);
        insert_instr(gen_instr_buf);
        sprintf(gen_instr_buf, "[CODE] ldr r%d, =0x%x", tmp_reg, set_mask);
        insert_instr(gen_instr_buf);
        sprintf(gen_instr_buf, "[CODE] orrs r%d, r%d", reg_base, tmp_reg);
        insert_instr(gen_instr_buf);
    } else {
        if (is_imm_encodable(unset_mask)) {
            sprintf(gen_instr_buf, "[CODE] bic r%d, r%d, #0x%x", reg_base, reg_base, unset_mask);
            insert_instr(gen_instr_buf);
        } else {
            sprintf(gen_instr_buf, "[CODE] ldr r%d, =0x%x", tmp_reg, unset_mask);
            insert_instr(gen_instr_buf);
            sprintf(gen_instr_buf, "[CODE] bic r%d, r%d", reg_base, tmp_reg);
            insert_instr(gen_instr_buf);
        }
        if (is_imm_encodable(set_mask)) {
            sprintf(gen_instr_buf, "[CODE] orr r%d, r%d, #0x%x", reg_base, reg_base, set_mask);
            insert_instr(gen_instr_buf);
        } else {
            sprintf(gen_instr_buf, "[CODE] ldr r%d, =0x%x", tmp_reg, set_mask);
            insert_instr(gen_instr_buf);
            sprintf(gen_instr_buf, "[CODE] orr r%d, r%d", reg_base, tmp_reg);
            insert_instr(gen_instr_buf);
        }
    }
    if (forceAlign) {
        sprintf(gen_instr_buf, "[INST] bic r%d, r%d, #3", reg_base, reg_base);
        insert_instr(gen_instr_buf);
    }
    if (options.isa == ISA_THUMB && reg_base_t > 7) {
        sprintf(gen_instr_buf, "[INST] mov r%d, r%d", reg_base_t, reg_base);
        insert_instr(gen_instr_buf);
    }
}

uint32_t get_rand_mem_offset(uint32_t shift_t, uint32_t shift_n, uint32_t *offset_val)
{
    uint32_t reg_val = rand();
    uint32_t shift_mask = 0xffffffff;
    uint32_t result;
    //ensure offset <= 0xfff;
    if (shift_t == 0) {
        //LSL
        if (shift_n < 12) {
            shift_mask = (0x00000fff >> shift_n) | (0x00000fff << (32 - shift_n));
        } else {
            shift_mask = 0xffffffff << (32 - shift_n);
        }
        reg_val &= shift_mask;
        result = reg_val<<shift_n;
        //printf("0x%x LSL %d => 0x%x\n", reg_val, shift_n, reg_val<<shift_n);

    } else if (shift_t == 1) {
        //LSR
        if (shift_n < 20) {
            shift_mask = 0xffffffff >> (20 - shift_n);
        }
        reg_val &= shift_mask;
        if (shift_n < 32) {
            result = reg_val>>shift_n;
        } else {
            result = 0;
        }
        //printf("0x%x LSR %d => 0x%x\n", reg_val, shift_n, reg_val>>shift_n);
    } else if (shift_t == 2) {
        //ASR
        if (shift_n < 20) {
            shift_mask = 0xffffffff >> (20 - shift_n);
        } else {
            shift_mask = 0x7fffffff;
        }
        reg_val &= shift_mask;
        if (shift_n < 32) {
            result = ((int32_t)reg_val)>>shift_n;
        } else {
            result = 0;
        }
        //printf("0x%x ASR %d => 0x%x\n", reg_val, shift_n, ((int32_t)reg_val)>>shift_n);

    } else if (shift_t == 3) {
        //ROR/RRX
        if (shift_n == 0) {
            shift_mask = 0xffffffff >> 19;
            reg_val &= shift_mask;
            //printf("0x%x RRX 1 ==> 0x%x\n", reg_val, reg_val >> 1);
            result = reg_val >> 1;
            options.cond_val = 3; // exec when C == 0
        } else {
            shift_mask = (0x00000fff << shift_n) | (0x00000fff >> (32-shift_n));
            reg_val &= shift_mask;
            //printf("0x%x ROR %d => 0x%x\n", reg_val, shift_n, (reg_val >> shift_n) | (reg_val << (32 -    shift_n)));
            result = (reg_val >> shift_n) | (reg_val << (32 - shift_n));
        }
    } else {
        printf("[ERROR] unknown shift type %x\n", shift_t);
    }
    *offset_val = reg_val;
    return result;
}

bool is_mem_align(uint32_t addr, uint32_t align_bit)
{
    uint32_t mask = (1 << align_bit) - 1;
    if (!options.allow_unalign_mem && ((addr & mask) != 0x0)) {
        return false;
    } else {
        return true;
    }
}
