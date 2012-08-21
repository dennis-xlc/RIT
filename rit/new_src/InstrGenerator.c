/*
 * =====================================================================================
 *
 *       Filename:  InstrGenerator.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  06/11/2012 12:54:10 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *   Organization:  
 *
 * =====================================================================================
 */
#include <Python.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
//#include <unistd.h>
#include "RitCommon.h"
#include "RandomInstrGen.h"
#include "InstrUtil.h"

extern gen_option_t options;
extern uint32_t raw_instr;
extern char print_buf[BUF_SIZE];
extern uint32_t cur_fixed_bit;
extern uint32_t sreg_idx;
extern uint32_t dreg_idx;
extern uint32_t fp_sreg_ext_idx;
extern uint32_t fp_dreg_ext_idx;

bool parse_long_para (option_kind_t para_type, long para_long);
bool parse_string_para (option_kind_t para_type, char* para_str);

//--------------------------- Python interface --------------------------------------

PyObject* single_instr_gen_py (PyObject *self, PyObject *args) 
{
    static int item_idx = 0;
    long para_long = -1;
    char* para_str = NULL;

    option_kind_t para_type = OPT_BTM;
    instr_status_t status = UNSUPPORTED;
    
    PyObject* para_pairs_l = NULL;
    PyObject* para_pair_t = NULL;
    PyObject* para_long_o = NULL;

    init_env_val ();

    // Parse the parameters in the list passed from Python
    if (! PyArg_ParseTuple(args, "O", &para_pairs_l)) {
        return NULL;
    }
    if (! PyList_Check(para_pairs_l)) {
        return NULL;
    }

    for ( item_idx=0; item_idx < PyList_Size(para_pairs_l); item_idx++ ) {
        para_long = -1;
        para_str = NULL;
        para_type = OPT_BTM;

        // Get every valid parameter
        para_pair_t = PyList_GetItem(para_pairs_l, item_idx);
        if (! PyTuple_Check(para_pair_t)) {
            return NULL;
        }
        if (! PyArg_ParseTuple(para_pair_t, "lO", &para_type, &para_long_o)) {
            return NULL;
        }

        if (PyInt_Check(para_long_o)) {
            // Get long value parameter
            para_long = PyInt_AsLong(para_long_o);
            if (-1 == para_long && PyErr_Occurred()) {
                return NULL;
            }
            if (! parse_long_para (para_type, para_long)) {
                return NULL;
            }
        } else if (PyString_Check(para_long_o)) {
            // Get string value parameter
            para_str = PyString_AsString(para_long_o);
            if (! para_str) {
                return NULL;
            }
            if (! parse_string_para (para_type, para_str)) {
                return NULL;
            }
        } else {
            return NULL;
        }
    }
    
    // Generate the instruction with options
    status = gen_random_instr();

    return Py_BuildValue("llls", status, cur_fixed_bit, raw_instr, print_buf);
}

static PyMethodDef libritMethods[] = {
    { "single_instr_gen", single_instr_gen_py, METH_VARARGS, "Randomly generate single instruction with valid conditions!" },
    {NULL, NULL}
};

void initlibrit() 
{
    PyObject* m;
    m = Py_InitModule("librit", libritMethods);
    m = m;
}

//---------------------------------------------------------------------------------------

bool parse_long_para (option_kind_t para_type, long para_long) {
    bool set_stat = true;

    if ( para_type >= OPT_BTM || para_type < OPT_TOP ) {
        return false;
    }

    switch (para_type) {
        case OPT_ISA:
            if ( para_long < ISA_TOP || para_long > ISA_RAND ) {
                options.isa = ISA_RAND;
            } else {
                options.isa = para_long;
            }
            break;
        case OPT_ARCH_VERSION:
            if ( para_long <= 0 ) {
                options.arch_version = DEFAULT_ARCH_VER;
            } else {
                options.arch_version = para_long;
            }
            break;
        case OPT_SBIT:
            if ( para_long < SBIT_TOP || para_long > SBIT_RAND ) {
                options.sbit_val = SBIT_RAND;
            } else {
                options.sbit_val = para_long;
            }
            break;
        case OPT_COND:
            if ( para_long < COND_TOP || para_long > COND_RAND ) {
                options.cond_val = COND_RAND;
            } else {
                options.cond_val = para_long;
            }
            break;
        case OPT_SREG:
            if (sreg_idx >= MAX_REG_NUM) {
                set_stat = false;
                break;
            }
            if ( para_long < R_TOP || para_long > R_RAND ) {
                options.src_regs[sreg_idx] = R_RAND;
            } else {
                options.src_regs[sreg_idx] = para_long;
            }
            sreg_idx ++;
            break;
        case OPT_DREG:
            if (dreg_idx >= MAX_REG_NUM) {
                set_stat = false;
                break;
            }
            if ( para_long < R_TOP || para_long > R_RAND ) {
                options.dst_regs[dreg_idx] = R_RAND;
            } else {
                options.dst_regs[dreg_idx] = para_long;
            }
            dreg_idx ++;
            break;
        case OPT_FP_SZ_Q_EXT:
            if ( para_long < FP_SZ_Q_TOP || para_long > FP_SZ_Q_RAND ) {
                options.fp_sz_q_bit = FP_SZ_Q_RAND;
            } else {
                options.fp_sz_q_bit = para_long;
            }
            break;
        case OPT_REGS_LIST:
            options.regs_list = para_long & REGS_LIST_MASK;
            break;
        case OPT_FP_SREG_EXT:
            if (fp_sreg_ext_idx >= MAX_REG_NUM) {
                set_stat = false;
                break;
            }
            if ( para_long < FP_REG_TOP || para_long > FP_REG_RAND ) {
                options.src_fp_regs_ext[fp_sreg_ext_idx] = FP_REG_RAND;
            } else {
                options.src_fp_regs_ext[fp_sreg_ext_idx] = para_long;
            }
            fp_sreg_ext_idx ++;
            break;
        case OPT_FP_DREG_EXT:
            if (fp_dreg_ext_idx >= MAX_REG_NUM) {
                set_stat = false;
                break;
            }
            if ( para_long < FP_REG_TOP || para_long > FP_REG_RAND ) {
                options.dst_fp_regs_ext[fp_dreg_ext_idx] = FP_REG_RAND;
            } else {
                options.dst_fp_regs_ext[fp_dreg_ext_idx] = para_long;
            }
            fp_dreg_ext_idx ++;
            break;
        case OPT_IMM:
            options.imm_val = para_long;
            break;
        case OPT_SHRTREG:
            if ( para_long < R_TOP || para_long > R_RAND ) {
                options.shift_rotate_reg = R_RAND;
            } else {
                options.shift_rotate_reg = para_long;
            }
            break;
        case OPT_SHIMM:
            options.shift_imm_val = para_long;
            break;
        case OPT_SHRTTYPE:
            if ( para_long < SH_TYPE_TOP || para_long > SH_TYPE_RAND ) {
                options.shift_rotate_type = SH_TYPE_RAND;
            } else {
                options.shift_rotate_type = para_long;
            }
            break;
        case OPT_MEM_BASE_VAL:
            options.mem_base_val = para_long;
            break;
        case OPT_MEM_BASE_MSK:
            options.mem_base_mask = para_long;
            break;
        case OPT_MEM_OFFSET_VAL:
            options.mem_offset_val = para_long;
            break;
        case OPT_ALLOW_UNALIGN_MEM:
        case OPT_ALLOW_ALL_INSTR:
        case OPT_ALLOW_BIAS_OPND:
        case OPT_ALLOW_BIAS_REG:
        case OPT_NOT_ALLOW_FIX_MEM:
        case OPT_NOT_ALLOW_RELAX:
        case OPT_VERBOSE:
            if (! para_long)
                set_stat = false;
            break;
        case OPT_INSTR_BIN_INIT:
            set_raw_instr (para_long);
            break;
        case OPT_RAND_SEED:
            srand (para_long);
            break;
        default:
            printf ("Unkonwn long para\n");
            return false;
    }


    if (set_stat) {
        SET_OPT_STAT(para_type);
    }

    return true;
}

bool parse_string_para (option_kind_t para_type, char* para_str) {
    if ( ! para_str || para_type >= OPT_BTM || para_type < OPT_TOP ) {
        return false;
    }

    switch (para_type) {
        case OPT_OPKIND:
            update_opkind_str (para_str);
            break;
        case OPT_OPCODE:
            options.opcode = para_str;
            break;
        default:
            printf ("Unkonwn string para\n");
            return false;
    }

    SET_OPT_STAT(para_type);

    return true;
}
