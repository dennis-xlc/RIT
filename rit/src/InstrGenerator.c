#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "rit-common.h"
#include "RandomInstrGen.h"

gen_option_t options;
static bool init_option = false;
char print_buff[BUF_SIZE];

void set_default_option()
{
    options.bias_register       = false;
    options.bias_oprand_value   = false;
    options.fix_mem             = true;
    options.mem_base_val        = 0x03fc0000;
    options.mem_base_mask       = 0xfffc0000;
    options.mem_offset_val      = 0x00000fff;
    options.allow_unalign_mem   = false;
    options.allow_all_instr     = false;
    options.arch_version        = 7;
    options.verbose             = 0;
    options.isa                 = ISA_ARMv5;
    options.is_sbit_init        = false;
    options.is_cond_init        = false;
    options.is_shift_imm_init   = false;
    options.is_shift_reg_init   = false;
    options.src_regs            = R_DEFAULT;
    options.dst_regs            = R_DEFAULT;
    //without
    //NO_OP/JMP_OP
    //LDREX_OP/STREX_OP
    update_opkind_str("EMPTY_OP,ALU_OP,LOG_OP,MOV_OP,CMP_OP,TEST_OP,EXT_OP,LD_OP,ST_OP,LDM_OP,STM_OP,PUSH_OP,POP_OP,UMUL_OP,MUL32_OP,SMUL_OP,SMULD_OP,BYTE_ALU_OP,S_BYTE_ALU_OP,BF_OP,REVERSE_OP,SSAT_OP,SR_OP,MISC_OP", true);
}

static void print_help()
{
    printf("Usage: ./random-instr-gen [OPTIONS] opcode\n");
    printf("Generate one random instruction.\n");
    printf("    -h, --help                  print help message\n");
    printf("    --verbose                   print verbose info\n");
    printf("    -i, --isa [armv5|armv7|thumb] instruction set(default arm)\n");
    printf("    --arch-version [5|7]        arch version of instruction(default 7)\n");
    printf("    --seed <num>                seed used to init srand\n");
    printf("    --hex hexcode               generate instruction with given hex code\n");
    
    printf("    --cond-val <num>            exec condition of instruction(default 14)\n");
    printf("    --with-opkind opkinds       choose opcode with given opkinds\n");
    printf("    --without-opkind opkinds    choose opcode without given opkinds\n");
    printf("    --set-sbit                  set s bit in instruction\n");
    printf("    --unset-sbit                unset s bit in instruction\n");
    printf("    --src-regs <regtype>        src_regs\n");
    printf("    --dst-regs <regtype>        dst_regs\n");
    printf("    --shift-imm <num>           shift_imm_range\n");
    printf("    --shift-reg <num>           shift_reg_range\n");
    printf("    --mem-base <num>            base limit for memory instruction\n");
    printf("    --mem-base-mask <num>       mask for memory instruction base register\n");
    printf("    --mem-offset <num>          offset limit for memory instruction\n");

    //legacy options
    //printf("    --allow-unalign-mem         generate unalign mem access instruction\n");
    //printf("    --bias-register             generate instruction with bias register combianation\n");
    //printf("    --bias-oprand-value         generate instruction with bias oprand value\n");
}

int main(int argc, char *argv[])
{
    int idx;
    bool rand_init = false;
    char *input_opcode = NULL;
    if (!init_option) {
        set_default_option();
        init_option = true;
    }
    for (idx = 1; idx < argc; idx++) {
        if (strcmp(argv[idx], "--help") == 0 || strcmp(argv[idx], "-h") == 0) {
            print_help();
            return 0;
        } else if (strcmp(argv[idx], "--verbose") == 0) {
            options.verbose = 1;
        } else if (strcmp(argv[idx], "--isa") == 0 || strcmp(argv[idx], "-i") == 0) {
            idx++;
            if (strcmp(argv[idx], "armv5") == 0) {
                options.isa = ISA_ARMv5;
            } else if (strcmp(argv[idx], "armv7") == 0) {
                options.isa = ISA_ARMv7;
            } else if (strcmp(argv[idx], "thumb") == 0) {
                options.isa = ISA_THUMB;
            } else if (strcmp(argv[idx], "arm") == 0) {
                options.isa = ISA_ARMv7;
            } else {
                printf("[WARNING] unknown isa option\n.");
            }
        } else if (strcmp(argv[idx], "--arch_version") == 0) {
            idx++;
            if (strcmp(argv[idx], "5") == 0) {
                options.arch_version = 5;
            } else if (strcmp(argv[idx], "7") == 0) {
                options.arch_version = 7;
            } else {
                printf("[WARNING] arch_version %s\n", argv[idx]);
            }
        } else if (strcmp(argv[idx], "--seed") == 0) {
            uint32_t seed_num;
            idx++;
            if (sscanf(argv[idx], "%d", &seed_num)) {
                printf("seed_num %x\n", seed_num);
                srand(seed_num);
                rand_init = true;
            }
        } else if (strcmp(argv[idx], "--hex") == 0) {
            uint32_t instr;
            idx++;
            if (strncmp(argv[idx], "0x", 2) == 0 || strncmp(argv[idx], "0X", 2) == 0) {
                if (sscanf(argv[idx]+2, "%x", &instr)) {
                    set_raw_instr(instr);
                } else {
                    printf("[WARNING] unknown hex instr %s\n", argv[idx]);
                }
            }
        } else if (strcmp(argv[idx], "--cond-val") == 0) {
            uint32_t new_cond;
            idx++;
            if (sscanf(argv[idx], "%d", &new_cond)) {
                if (new_cond <= 14) {
                    options.cond_val = new_cond;
                } else {
                    options.cond_val = 14; //AL
                }
                options.is_cond_init = true;
            } else {
                printf("[WARNING] unknown instr cond code %s\n", argv[idx]);
            }
        } else if (strcmp(argv[idx], "--with-opkind") == 0) {
            idx++;
            update_opkind_str(argv[idx], true);
        } else if (strcmp(argv[idx], "--without-opkind") == 0) {
            idx++;
            update_opkind_str(argv[idx], false);
        } else if (strcmp(argv[idx], "--set-sbit") == 0) {
            options.is_sbit_init = true;
            options.sbit_val = 1;
        } else if (strcmp(argv[idx], "--unset-sbit") == 0) {
            options.is_sbit_init = true;
            options.sbit_val = 0;
        } else if (strcmp(argv[idx], "--src-regs") == 0) {
            idx++;
            if (strcmp(argv[idx], "0-3") == 0) {
                options.src_regs = R_0_3;
            } else if (strcmp(argv[idx], "4-7") == 0) {
                options.src_regs = R_4_7;
            } else if (strcmp(argv[idx], "8-12") == 0) {
                options.src_regs = R_8_12;
            } else if (strcmp(argv[idx], "13") == 0 || strcmp(argv[idx], "sp") == 0) {
                options.src_regs = R_SP;
            } else if (strcmp(argv[idx], "14") == 0 || strcmp(argv[idx], "lr") == 0) {
                options.src_regs = R_LR;
            } else if (strcmp(argv[idx], "15") == 0 || strcmp(argv[idx], "pc") == 0) {
                options.src_regs = R_PC;
            } else {
                printf("[WARNING] unknown src-regs type\n");
                options.src_regs = R_DEFAULT;
            }
        } else if (strcmp(argv[idx], "--dst-regs") == 0) {
            idx++;
            if (strcmp(argv[idx], "0-3") == 0) {
                options.dst_regs = R_0_3;
            } else if (strcmp(argv[idx], "4-7") == 0) {
                options.dst_regs = R_4_7;
            } else if (strcmp(argv[idx], "8-12") == 0) {
                options.dst_regs = R_8_12;
            } else if (strcmp(argv[idx], "13") == 0 || strcmp(argv[idx], "sp") == 0) {
                options.dst_regs = R_SP;
            } else if (strcmp(argv[idx], "14") == 0 || strcmp(argv[idx], "lr") == 0) {
                options.dst_regs = R_LR;
            } else if (strcmp(argv[idx], "15") == 0 || strcmp(argv[idx], "pc") == 0) {
                options.dst_regs = R_PC;
            } else {
                printf("[WARNING] unknown dst-regs type\n");
                options.dst_regs = R_DEFAULT;
            }
        } else if (strcmp(argv[idx], "--allow_unalign_mem") == 0) {
            options.allow_unalign_mem = true;
        } else if (strcmp(argv[idx], "--shift-imm") == 0) {
            uint32_t tmp_imm;
            idx++;
            if (strncmp(argv[idx], "0x", 2) == 0 || strncmp(argv[idx], "0X", 2) == 0) {
                //hex
                if (sscanf(argv[idx]+2, "%x", &tmp_imm)) {
                    options.is_shift_imm_init = true;
                    options.shift_imm_val = tmp_imm;
                }
            } else {
                //dec
                if (sscanf(argv[idx], "%d", &tmp_imm)) {
                    options.is_shift_imm_init = true;
                    options.shift_imm_val = tmp_imm;
                }
            }
            idx++;
        } else if (strcmp(argv[idx], "--shift-reg") == 0) {
            uint32_t tmp_imm;
            idx++;
            if (strncmp(argv[idx], "0x", 2) == 0 || strncmp(argv[idx], "0X", 2) == 0) {
                //hex
                if (sscanf(argv[idx]+2, "%x", &tmp_imm)) {
                    options.is_shift_reg_init = true;
                    options.shift_reg_val = tmp_imm;
                }
            } else {
                //dec
                if (sscanf(argv[idx], "%d", &tmp_imm)) {
                    options.is_shift_reg_init = true;
                    options.shift_reg_val = tmp_imm;
                }
            }
        } else if (strcmp(argv[idx], "--mem-base") == 0) {
            uint32_t tmp_imm;
            idx++;
            if (strncmp(argv[idx], "0x", 2) == 0 || strncmp(argv[idx], "0X", 2) == 0) {
                if (sscanf(argv[idx]+2, "%x", &tmp_imm)) {
                    options.mem_base_val = tmp_imm;
                }
            } else {
                printf("[WARNING] unknown mem-base val\n");
            }
        } else if (strcmp(argv[idx], "--mem-base-mask") == 0) {
            uint32_t tmp_imm;
            idx++;
            if (strncmp(argv[idx], "0x", 2) == 0 || strncmp(argv[idx], "0X", 2) == 0) {
                if (sscanf(argv[idx]+2, "%x", &tmp_imm)) {
                    options.mem_base_mask = tmp_imm;
                }
            } else {
                printf("[WARNING] unknwon mem-base-make\n");
            }
        } else if (strcmp(argv[idx], "--mem-offset") == 0) {
            uint32_t new_mem_offset;
            idx++;
            if (sscanf(argv[idx], "%d", &new_mem_offset)) {
                options.mem_offset_val = new_mem_offset;
            } else {
                printf("[WARNING] unknown max-mem-offset %s\n", argv[idx]);
            }
        } else if (strcmp(argv[idx], "--") == 0 || strcmp(argv[idx], "-") == 0){
            printf("[WARNING] unknown option %s\n", argv[idx]);
            print_help();
            return 1;
        /*
        } else if (strcmp(argv[idx], "--bias_register") == 0) {
            options.bias_register = true;
        } else if (strcmp(argv[idx], "--bias_oprand_value") == 0) {
            options.bias_oprand_value = true;
            }*/
        } else {
            input_opcode = argv[idx];
            options.allow_all_instr = true;
        }
    }
    if (!rand_init) {
        srand(time(NULL));
    }
    if (!options.is_cond_init) {
        options.cond_val = rand() % 28;
        if (options.cond_val > 13) {
            //AL or NOT_AL
            options.cond_val = 14;
        }
    }
    
    if (input_opcode != NULL) {
        gen_random_instr(input_opcode);
    } else {
        //choose an opcode
        //gen_instr_with_op(get_random_op());
        gen_random_instr_without_op();
    }
    printf("%s", print_buff);
    return 0;
}

static inline arm_regs_t get_reg_type(int reg)
{
    arm_regs_t reg_type;
    switch (reg) {
        case 0:
        case 1:
        case 2:
        case 3:
            reg_type = R_0_3;
            break;
        case 4:
        case 5:
        case 6:
        case 7:
            reg_type = R_4_7;
            break;
        case 8:
        case 9:
        case 10:
        case 11:
        case 12:
            reg_type = R_8_12;
            break;
        case 13:
            reg_type = R_SP;
            break;
        case 14:
            reg_type = R_LR;
            break;
        case 15:
            reg_type = R_PC;
            break;
        default:
            reg_type = R_DEFAULT;
            break;
    }
    return reg_type;
}

/**
 * int gen_single_instr()
 *      This function can be called from python with ctypes library
 *
 * Parameter:
 *  int isa 
 *      0: ARM; not 0: Thumb
 *  char* opkind:
 *  uint32_t hexcode
 *  int cond
 *  int sbit
 *  int src_reg
 *  int dst_reg
 *  int seed
 */
int py_gen_instr(char* output, int isa, const char* opkind, uint32_t hexcode, 
        int cond, int sbit, int src_reg, int dst_reg, int seed)
{
    if (!init_option) {
        set_default_option();
        init_option = true;
    }
    srand(seed);
    if (isa) {
        options.isa = ISA_THUMB;
    } else {
        options.isa = ISA_ARMv7;
    }
    update_opkind_str(opkind, true);
    set_raw_instr(hexcode);
    options.cond_val = cond;
    options.is_cond_init = true;
    options.sbit_val = sbit;
    options.is_sbit_init = true;
    options.src_regs = get_reg_type(src_reg);
    options.dst_regs = get_reg_type(dst_reg);
    gen_random_instr_without_op();
    strncpy(output, print_buff, BUF_SIZE);
    return 0;
}

int py_gen_instr_with_op(char* output, const char* op, uint32_t hexcode, 
        int cond, int sbit, int src_reg, int dst_reg, int seed)
{
    if (!init_option) {
        set_default_option();
        init_option = true;
    }
    srand(seed);
    set_raw_instr(hexcode);
    options.cond_val = cond;
    options.is_cond_init = true;
    options.sbit_val = sbit;
    options.is_sbit_init = true;
    options.src_regs = get_reg_type(src_reg);
    options.dst_regs = get_reg_type(dst_reg);
    gen_random_instr(op);
    strncpy(output, print_buff, BUF_SIZE);
    return 0;
}

int py_test(int x)
{
    return x*x;
}
