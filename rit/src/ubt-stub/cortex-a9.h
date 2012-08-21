#ifndef CORTEX_A9_H
#define CORTEX_A9_H

#include "sl/bt/sl_def.h"

#define SISA_UNKNOWN_VAL 0xffffffff

#define SISA_ARM_BR_H_BIT  (1<<0x18)    // H bit is 24 bit in BL
#define SISA_ARM_MEM_P_BIT (1<<0x18)    // ARM P bit is 24 bit
#define SISA_ARM_MEM_U_BIT (1<<0x17)    // ARM U bit is 23 bit
#define SISA_ARM_MEM_W_BIT (1<<0x15)    // ARM W bit is 21 bit
#define SISA_THUMB_MEM_P_BIT (1<<0xa)    // Thumb P bit is 10 bit
#define SISA_THUMB_MEM_U_BIT (1<<0x9)    // Thumb U bit is 9 bit
#define SISA_THUMB_MEM_W_BIT (1<<0x8)    // Thumb W bit is 8 bit
#define SISA_ARM_SWP_B_BIT (1<<0x16)   // B bit, for Swap instruction, determine the size
#define SISA_ARM_MUL_M_BIT (1<<0x6)    // for 16-bit multiple, decide rm should use top 16 bit or bottom 16 bit
#define SISA_ARM_MUL_N_BIT (1<<0x5)     // for 16-bit multiple, decide rn should use top 16 bit or bottom 16 bit
#define SISA_THUMB_MUL_M_BIT (1<<0x4)    // for 16-bit multiple, decide rm should use top 16 bit or bottom 16 bit
#define SISA_THUMB_MUL_N_BIT (1<<0x5)     // for 16-bit multiple, decide rn should use top 16 bit or bottom 16 bit
#define SISA_ARM_SMULL_R_BIT (1<<0x5)     // round bit for SMMLA, decide whether round the result
#define SISA_THUMB_SMULL_R_BIT (1<<0x4)
#define SISA_ARM_SMUAD_M_BIT (1<<0x5) // for SMUAD, decide whether swap input operand rm
#define SISA_THUMB_SMUAD_M_BIT (1<<0x4)
#define SISA_ARM_MSR_NZCVQ_BIT (1<<0x13)
#define SISA_ARM_MSR_G_BIT (1<<0x12)
#define SISA_THUMB_MSR_NZCVQ_BIT (1<<0xb)
#define SISA_THUMB_MSR_G_BIT (1<<0xa)
#define SISA_ARM_SIMD_U_BIT (1<<0x18) //ARM U bit is 24 bit
#define SISA_THUMB_SIMD_U_BIT (1<<0x1c) //THUMB U bit is 28 bit
#define SISA_ARM_SIMD_F10_BIT (1<<0xa ) // ARM/THUMB F bit is 10 bit
#define SISA_ARM_SIMD_F8_BIT (1<<0x8) //// ARM/THUMB F bit is 8 bit

typedef enum {
    SISA_OP_NA = 0,	/* NA */
#define DEF_INST(op, ...) SISA_OP_##op,
#define DEF_LINK(op, ...) SISA_OP_##op,
#define DEF_TAIL(op, ...) SISA_OP_##op,
#define DEF_CONNECT(op)   SISA_OP_##op##_CONNECT,

#include "cortex-a9.def"
    SISA_OP_ARM_MAX,
#include "Thumb.def"
    SISA_OP_THUMB_MAX,
#include "Thumb_32.def"
    SISA_OP_THUMB_32_MAX,
    SISA_OP_MAX
} sisa_opcode_t;


typedef enum {
    SISA_OPND_TYPE_NONE=0,
    SISA_OPND_TYPE_GPR,
    SISA_OPND_TYPE_GPR_RW,
    SISA_OPND_TYPE_SHIFT_REG,
    SISA_OPND_TYPE_GPRLIST,
    SISA_OPND_TYPE_GPRLIST_T,
    SISA_OPND_TYPE_GPRSET,
    SISA_OPND_TYPE_GPR_MERGE,
    SISA_OPND_TYPE_GPR_CONST,
    SISA_OPND_TYPE_IMM,
    SISA_OPND_TYPE_IMM_MERGE,
    SISA_OPND_TYPE_SHIFT_IMM,
    SISA_OPND_TYPE_SHIFT_IMM_T,
    SISA_OPND_TYPE_IMM_ARM_EXP,
    SISA_OPND_TYPE_IMM_THUMB_EXP,
    SISA_OPND_TYPE_IMM_ZERO_EXTEND,
    SISA_OPND_TYPE_IMM_SIGN_EXTEND,
    SISA_OPND_TYPE_IMM_VFP_EXPAND,
    SISA_OPND_TYPE_FLAGS,
    SISA_OPND_TYPE_VEC,
    SISA_OPND_TYPE_ROTATE_REG,
    SISA_OPND_TYPE_COND,

    //Support VFP
    SISA_OPND_TYPE_FPSR,
    SISA_OPND_TYPE_FPDR,
    SISA_OPND_TYPE_FPDSR,

    ///TODO: support SIMD
    SISA_OPND_TYPE_FPQR,
    SISA_OPND_TYPE_FPQDR,
    SISA_OPND_TYPE_IMM_SIMD_EXP,

    SISA_OPND_TYPE_MAX_NUM
} sisa_opnd_type_t;

typedef enum {

    SISA_FIRST_OUTPUT=0,
    SISA_OUTPUT0 = SISA_FIRST_OUTPUT,
    SISA_OUTPUT1,
    SISA_LAST_OUTPUT = SISA_OUTPUT1,

    SISA_FIRST_INPUT,
    SISA_INPUT0 = SISA_FIRST_INPUT,
    SISA_INPUT1,
    SISA_INPUT2,
    SISA_INPUT3,
    SISA_LAST_INPUT = SISA_INPUT3,

    SISA_OPND_NUM,
}sisa_opnd_pos_t;

typedef struct sisa_opnd_str{
    uint8_t start_bit;   //0-31
    uint8_t end_bit;
    uint8_t bit1; //shift type for shift imm/reg, d bit for simd
    uint8_t bit2; //q bit for simd bit tells whether it is quadword op, otherwise, word op
    sisa_opnd_type_t type;
}sisa_opnd_t;

typedef struct sisa_instr_str {
    uint16_t       cond;
    const uint8_t opkind;
    const uint32_t cgflags;
    const char*    name;
    const          sisa_opnd_t opnds[SISA_OPND_NUM];
} sisa_instr_t;

typedef enum{
    NO_OP,
    EMPTY_OP,
    ALU_OP,
    LOG_OP,
    MOV_OP,
    CMP_OP,
    TEST_OP,
    EXT_OP,
    LD_OP,
    ST_OP,
    LDM_OP,
    STM_OP,
    PUSH_OP,
    POP_OP,
    JMP_OP,
    UMUL_OP,
    MUL32_OP,
    SMUL_OP,
    SMULD_OP,
    BYTE_ALU_OP,
    S_BYTE_ALU_OP,
    BF_OP,
    REVERSE_OP,
    SSAT_OP,
    SR_OP,
    LDREX_OP,
    STREX_OP,
    MISC_OP,
    COPR_OP,
    VFP_OP,
    SYS_OP,
    SIMD_OP,
    /* not finish yet*/
    LAST_OP,

    PSEUDO_OP,
    AEABI_OP,
    KERNEL_HELPER_OP,

    /* DOWN Below is the pseudo op we used for LINK, CONNECT, TAIL Node */
    PSEUDO_LINK_NODE,
    PSEUDO_CONNECT_NODE,
    PSEUDO_TAIL_NODE,

} sisa_opkind_t;

/* Main Decoder interface */
static inline void sisa_init(void);
//static inline sisa_opcode_t sisa_decode_get_opcode(uint32_t code);
//static inline sisa_opcode_t sisa_decode_get_opcode_for_thumb(uint32_t *code, uint32_t* instr_size);

/* opnd interface */
static inline bool sisa_input_opnd_pos_p (int32_t opnd_pos);
static inline uint32_t sisa_decode_get_bits(uint32_t code, uint8_t bit_pos_start, uint8_t bit_pos_end);
static inline uint32_t sisa_decode_get_opnd(uint32_t code, const sisa_opnd_t* opnd);
static inline uint32_t sisa_decode_get_opnd_ext(uint32_t code, const sisa_opnd_t* opnd);
static inline uint32_t sisa_decode_get_imm_merge(uint32_t code, const sisa_opnd_t* opnd);

/* op interface */
static inline bool sisa_op_has_reg_form(sisa_opcode_t op, sisa_opkind_t opkind);
static inline bool sisa_op_has_imm_form(sisa_opcode_t op, sisa_opkind_t opkind);
static inline bool sisa_op_has_lit_form(sisa_opcode_t op, sisa_opkind_t opkind);
static inline bool sisa_op_has_sft_reg_form(sisa_opcode_t op, sisa_opkind_t opkind);
static inline bool sisa_op_has_sxt(sisa_opcode_t op, sisa_opkind_t opkind);
static inline bool sisa_op_has_uxt(sisa_opcode_t op, sisa_opkind_t opkind);
static inline bool sisa_op_has_size_dw(sisa_opcode_t op, sisa_opkind_t opkind);
static inline bool sisa_op_has_size_b(sisa_opcode_t op, sisa_opkind_t opkind);
static inline bool sisa_op_has_size_w(sisa_opcode_t op, sisa_opkind_t opkind);
static inline bool sisa_op_has_size_hw(sisa_opcode_t op, sisa_opkind_t opkind);
static inline bool sisa_op_has_double_dest(sisa_opcode_t op, sisa_opkind_t opkind);
static inline bool sisa_op_is_prefetch(sisa_opcode_t op);
static inline bool sisa_op_is_ldr(sisa_opcode_t op);
static inline bool sisa_op_is_str(sisa_opcode_t op);


/**************************************************************************************
*                                                                                     *
*               BELOW ARE INTERNAL IMPLEMENTATION                                     *
*                                                                                     *
***************************************************************************************/

#define CHECK_FLAG(value, flag)  (((value) & (flag)) == (flag))
#define CHECK_BRANCH(value)      (CHECK_FLAG(value, BR_DIR_FLAG) || CHECK_FLAG(value, BR_INDIR_FLAG))
#define CHECK_DIRECT_BRANCH(value)      (CHECK_FLAG(value, BR_DIR_FLAG) )
#define CHECK_INDIR_BRANCH(value)      (CHECK_FLAG(value, BR_INDIR_FLAG) )
#define CHECK_CALL(value)      (CHECK_FLAG(value, BLR_FLAG) )

#define CHECK_PUW_FLAG(value)    (CHECK_FLAG(value, P_FLAG) && CHECK_FLAG(value, U_FLAG) && CHECK_FLAG(value, W_FLAG))
#define CHECK_PU_FLAG(value)     (CHECK_FLAG(value, P_FLAG) && CHECK_FLAG(value, U_FLAG))
#define CHECK_W_FLAG(value)      (CHECK_FLAG(value, W_FLAG))
#define CHECK_P_FLAG(value)     (CHECK_FLAG(value, P_FLAG))
#define CHECK_U_FLAG(value)     (CHECK_FLAG(value, U_FLAG))

#define CHECK_SIMD_U_FLAG(value)     (CHECK_FLAG(value, SIMD_U_FLAG))
#define CHECK_SIMD_F_FLAG(value)     (CHECK_FLAG(value, SIMD_F_FLAG))

#define CHECK_THUMB2_PUW_LOW(value) (CHECK_FLAG(value, THUMB2_PUW_LOW))
#define CHECK_THUMB2_PUW_HIGH(value) (CHECK_FLAG(value, THUMB2_PUW_HIGH))
#define CHECK_THUMB2_PUW_CONST(value) (CHECK_FLAG(value, THUMB2_PUW_CONST))
#define CHECK_THUMB2_PUW_U(value) (CHECK_FLAG(value, THUMB2_PUW_U))

#define CHECK_IMM_FLAG(value)      (CHECK_FLAG(value, SECOND_IMM_FLAG))
#define CHECK_SFT_REG_FLAG(value)      (CHECK_FLAG(value, SFT_REG_FLAG))
#define CHECK_REG_FLAG(value)      (CHECK_FLAG(value, REG_FLAG))
#define CHECK_LIT_FLAG(value)      (CHECK_FLAG(value, OP_LIT_FLAG))

#define CHECK_SIZE_WORD_FLAG(value)      (((value) & (OP_SIZE_MASK)) == (OP_SIZE_WORD_FLAG))
#define CHECK_SIZE_BYTE_FLAG(value)      (((value) & (OP_SIZE_MASK)) == (OP_SIZE_BYTE_FLAG))
#define CHECK_SIZE_HWORD_FLAG(value)      (((value) & (OP_SIZE_MASK)) == (OP_SIZE_HWORD_FLAG))
#define CHECK_SIZE_DWORD_FLAG(value)      (((value) & (OP_SIZE_MASK)) == (OP_SIZE_DWORD_FLAG))

#define CHECK_SIGNED_FLAG(value)      (CHECK_FLAG(value, SIGNED_FLAG))
#define CHECK_UNSIGNED_FLAG(value)      (CHECK_FLAG(value, UNSIGNED_FLAG))


#define DGPRM		0x0100000000000000ULL
#define FPSRM		0x0200000000000000ULL
#define FPDRM		0x0400000000000000ULL
#define FPQRM		0x0800000000000000ULL

#define OPNDNA 		        {0}

#define DFLAGS(S,E)              {S,E,0,0,SISA_OPND_TYPE_FLAGS} //flags for the instruction

#define DIMM(S, E)                {S,E,0, 0, SISA_OPND_TYPE_IMM}
#define DIMM_MERGE(HS, HE, LS, LE)  {HS,HE,LS,LE,SISA_OPND_TYPE_IMM_MERGE}
#define DIMM_ARMEXP(S, E, L)  {S,E,L,0,SISA_OPND_TYPE_IMM_ARM_EXP} //T is the low bit of shift part [shift part] [base part] imm=base >> (shift) | base << (32 - shfit)
#define DIMM_THUMBEXP(S, E, L)  {S,E,L,0,SISA_OPND_TYPE_IMM_THUMB_EXP}
#define DIMM_SIMDEXP(S, E) {S,E,0,0, SISA_OPND_TYPE_IMM_SIMD_EXP}
#define DIMM_ZERO_EXTEND(S, E, N)   {S, E, N, 0, SISA_OPND_TYPE_IMM_ZERO_EXTEND} //append M '0' in the end before extend
#define DIMM_ZERO_EXTEND_I(S, E, N, I)   {S, E, N, I, SISA_OPND_TYPE_IMM_ZERO_EXTEND} //merge I before imm; append M '0' in the end before extend
#define DIMM_SIGN_EXTEND(S, E, N)   {S, E, N, 0, SISA_OPND_TYPE_IMM_SIGN_EXTEND} //append M '0' in the end before extend
#define DSHFTIMM(S, E, L,H)  {S,E,L,H,SISA_OPND_TYPE_SHIFT_IMM} //T is the low bit of shift part [shift part] [base part] imm=base >> (shift) | base << (32 - shfit)
#define DSHFTIMM_T(S, E, L,H)  {S, E, L, H, SISA_OPND_TYPE_SHIFT_IMM_T} //thumb shift gpr (type=L-H) imm=14-12:7-6
#define DIMM_VFP_EXPAND(HS, HE, LS, LE)  {HS, HE, LS, LE, SISA_OPND_TYPE_IMM_VFP_EXPAND} //thumb shift gpr (type=L-H) imm=14-12:7-6

#define DROTATEGPR(S,E,C,D)    {S,E,C,D,SISA_OPND_TYPE_ROTATE_REG}

#define DNA 		OPNDNA
#define DGPR(S,E)               {S,E, 0, 0, SISA_OPND_TYPE_GPR}
#define DGPR_RW(S,E)               {S,E, 0, 0, SISA_OPND_TYPE_GPR_RW}
#define DSHFTGPR(S,E, C,D)               {S,E, C,D, SISA_OPND_TYPE_SHIFT_REG}
#define DGPR_LIST(S,E)	 {S,E, 0, 0, SISA_OPND_TYPE_GPRLIST}
#define DGPR_SET(S,E)       {S,E, 0, 0, SISA_OPND_TYPE_GPRSET}
#define DGPR_MERGE(S, E, DN)     {S, E, DN, 0, SISA_OPND_TYPE_GPR_MERGE}
#define DGPR_CONST(V)            {0, 0, V, 0, SISA_OPND_TYPE_GPR_CONST}
#define DGPR_LIST_T(S, E, V, P)  {S, E, V, P, SISA_OPND_TYPE_GPRLIST_T} //thumb register_list (register[P]=V)

#define DCOND(S, E)              {S, E, 0, 0, SISA_OPND_TYPE_COND}

///TODO: support SIMD and VFP
#define FPNA		DNA
#define FPSR(S,E,C)	{S,E, C, 0, SISA_OPND_TYPE_FPSR}
#define FPDR(S,E,C)	{S,E, C, 0, SISA_OPND_TYPE_FPDR}

#define FPQR(S,E,C)	{S,E, C, 0, SISA_OPND_TYPE_FPQR}
#define FPDSR(S,E,C,D)	{S,E, C, D, SISA_OPND_TYPE_FPDSR}
#define FPQDR(S,E,D,Q)      {S,E,D,Q, SISA_OPND_TYPE_FPQDR}
#define VFP_SIZE_BIT           8

#define ARGIN0()                OPNDNA, OPNDNA,
#define ARGIN1(in0)           (in0), OPNDNA
#define ARGIN2(in0, in1)    (in0),(in1)
#define ARGOUT0()                  OPNDNA, OPNDNA,OPNDNA
#define ARGOUT1(out0)           (out0),OPNDNA
#define ARGOUT2(out0, out1) (out0),(out1),OPNDNA
#define ARGOUT3(out0, out1, out2)   (out0),(out1),(out2)

#define OPND_PATTERN_REG_SHIFT  	DGPR(12,15), DNA, DGPR(16,19), DGPR(0,3), DSHFTGPR(8,11,5,6)


#define DGOPR(A,INST)	((INST>>(A&0xff))&0xf)
#define OPR(A,INST)	((A&DGPRM)?(DGOPR(A,INST)+1):0)	/* FIXME: fp opr when needed for schedule and issue */

#define cond_mask_bit 28
#define cond_mask   (0xf0000000)
#define thumb_B_T1_cond_mask 0xf00
#define thumb_B_T1_cond_mask_bit 8
#define thumb_B_T3_cond_mask 0x3c00000
#define thumb_B_T3_cond_mask_bit 22

/* condtion bits */
#define CONDZ		0x1
#define CONDC		0x2
#define	CONDN		0x4
#define	CONDV		0x8
#define	NCOND		0x0
#define COND_ZCNV (CONDN | CONDZ | CONDC | CONDV)

#define CONDS(A)	(A<<4)
#define CHECK_DST_COND( INST, COND )	(( COND == (uint32_t)NCOND) ? (uint32_t)0 : \
                                                                                 (( COND >> 4) == (uint32_t)0 ? (uint32_t)1 : \
                                                                                 ((uint32_t)(( ((uint32_t)1) << ( COND >> 4 )) & INST) )))

#define FCONDQC        	0x10

/****************************************/
//
//  ARM Decode Flags:
//
/****************************************/
#define NONE_FLAG                       0x00000000
// flags for memory operations
#define OP_SIZE_MASK                    0x30000000
#define OP_SIZE_WORD_FLAG               0x0
#define OP_SIZE_BYTE_FLAG               0x10000000
#define OP_SIZE_HWORD_FLAG              0x20000000
#define OP_SIZE_DWORD_FLAG              0x30000000

#define SIGNED_FLAG                     0x40000000   /*operate for signed instructions*/
#define UNSIGNED_FLAG                   0x01000000

#define INPUT_CARRY_FLAG                0x02000000   /*TODO: CG_ARM_FLAG_CARRY */
#define SHIFT_OUTPUT_CARRY_FLAG         0x04000000   /*gen c flags alone*/

#define ADDR_FORMAT_FLAG                0x08000000   /* TODO:CG_ARM_FLAG_MEM_REG */

#define BLR_FLAG                        0x00100000   /* TODO: CG_ARM_FLAG_B_WITH_LR*/
#define BR_DIR_FLAG                     0x00200000   /*branch to imm */
#define BR_INDIR_FLAG                   0x00400000   /* branch to register*/

#define P_FLAG                          0x00010000   /* TODO: Memory PUW flag */
#define U_FLAG                          0x00020000   /* TODO: Memory index flag */
#define W_FLAG                          0x00040000   /* TODO: Memory addr flag */

#define THUMB2_PUW_LOW                  P_FLAG|U_FLAG|W_FLAG /* P(bit 10), U(bit 9), W(bit 8)*/
#define THUMB2_PUW_HIGH                 P_FLAG        /* P(bit 24), U(bit 23), W(bit 21)*/
#define THUMB2_PUW_CONST                W_FLAG               /* P == 1, U == 1, W == 0*/
#define THUMB2_PUW_U                    U_FLAG               /* P == 1, U(bit 23), W == 0*/

#define SIMD_U_FLAG                     0x00080000
#define SIMD_F_FLAG                     0x00800000

#define J1_BIT  0x00002000
#define J2_BIT  0x00000800
#define S_BIT   0x04000000

// flags for 2nd operand
#define SECOND_OPND_MASK                0x00000f00
#define SECOND_IMM_FLAG                 0x00000100
#define SFT_REG_FLAG                    0x00000400
#define REG_FLAG                        0x00000800

#define OP_LIT_FLAG                     0x00000080

#define GE_FLAG                         0x00001000
#define USE_GE_FLAG                     0x00002000
#define VFP_VEC_INS_FLAG                0x00004000


// flag for signed instructions
#define INS_CLA_SIGNED                  0x40000000

#define DEST_1   SISA_OUTPUT0
#define DEST_2   SISA_OUTPUT1
#define SRC_1    SISA_INPUT0
#define SRC_2    SISA_INPUT1
#define SRC_EXT  SISA_INPUT2    // could be shift register, addand of sma
#define SRC_EXT_2 SISA_INPUT3

typedef struct sisa_dec_node_str{
    uint32_t match_val;
    uint32_t match_msk;
    uint32_t neg_val;
    uint32_t neg_msk;
}sisa_dec_node_t;


#define SISA_DEC_NOD_MATMASK(node_idx)  (sisa_dec_node_tbl[node_idx].match_msk)
#define SISA_DEC_NOD_MATVAL(node_idx)   (sisa_dec_node_tbl[node_idx].match_val)
#define SISA_DEC_NOD_NEGMASK(node_idx)  (sisa_dec_node_tbl[node_idx].neg_msk)
#define SISA_DEC_NOD_NEGVAL(node_idx)   (sisa_dec_node_tbl[node_idx].neg_val)


#define SISA_DEC_NODE_LEAF_END(node_idx) (SISA_DEC_NOD_MATMASK(node_idx) == 0)
#define SISA_DEC_NODE_MATCH(instr, node_idx) \
    ((((instr) & SISA_DEC_NOD_MATMASK(node_idx)) == SISA_DEC_NOD_MATVAL(node_idx)) && \
     (((instr) & SISA_DEC_NOD_NEGMASK(node_idx)) != SISA_DEC_NOD_NEGVAL(node_idx)) )




extern const sisa_instr_t    sisa_instr_tbl[SISA_OP_MAX];
extern const sisa_dec_node_t sisa_dec_node_tbl[SISA_OP_MAX];
extern void sisa_decode_assert(uint32_t code, sisa_opcode_t op,  const sisa_instr_t* desc_entry, arm_state_t *context);

//
// SRC ISA interfaces, mainly inlcuding functions to decode a raw ARM instrs to get opcode or opnd value


static inline bool sisa_input_opnd_pos_p (int32_t opnd_pos)
{
    return ((opnd_pos >= SISA_FIRST_INPUT) &&  (opnd_pos <=  SISA_LAST_INPUT) );
}


static inline void sisa_init(void) {
    return;
}

#define OPCODE_CACHE_BITS (14)
#define OPCODE_CACHE_SIZE (1UL<<OPCODE_CACHE_BITS)
#define OPCODE_CACHE_MASK (OPCODE_CACHE_SIZE-1)
#define OPCODE_CACHE_COUNT_MAX 0x0u
#if OPCODE_CACHE_COUNT_MAX != 0x0
#define OPCODE_CACHE_COUNT_THRESHOLD 0
#define OPCODE_CACHE_COUNT_INIT 0
#endif

//extern lock_t opcode_cache_lock;
//typedef struct opcode_cache_str {
//    uint32_t code;
//    sisa_opcode_t opcode;
//#if OPCODE_CACHE_COUNT_MAX != 0x0
//    uint8_t count;
//#endif
//    bool thumb_p;
//} opcode_cache_t;
//
//extern opcode_cache_t opcode_cache[OPCODE_CACHE_SIZE];
//extern void opcode_cache_lock_init(void);
//
//static inline uint32_t opcode_cache_hash(uint32_t code)
//{
//    uint32_t hash;
//    hash = (code & OPCODE_CACHE_MASK)
//            ^ ((code>>OPCODE_CACHE_BITS) & OPCODE_CACHE_MASK)
//            ^ ((code>>(OPCODE_CACHE_BITS*2)) & OPCODE_CACHE_MASK);
//    return hash;
//}

//static inline void opcode_cache_set(uint32_t code, sisa_opcode_t opcode)
//{
//    bool thumb_p = (arm_home.instr_set == SL_INSTR_SET_THUMB);
//
//    if (code == 0) { // in some case real code will be 0, ignore it
//        return;
//    }
//    if (bt_try_lock(&opcode_cache_lock)) {
//        uint32_t hash = opcode_cache_hash(code);
//        if (opcode_cache[hash].code != code || opcode_cache[hash].thumb_p != thumb_p) {
//            assert (opcode_cache[hash].code== 0 || opcode_cache_hash(opcode_cache[hash].code) == hash);
//#if OPCODE_CACHE_COUNT_MAX != 0x0
//            // if count > OPCODE_CACHE_COUNT_THRESHOLD, it will not be replaced
//            // otherwise it will be replaced and set to an initial number to avoid thrashing
//            if (opcode_cache[hash].count > OPCODE_CACHE_COUNT_THRESHOLD) { // do not replace
//                opcode_cache[hash].count --;
//                STAT_CODE(perf_stat_increase(STAT_TR_OPCODE_CACHE_NO_REPLACE));
//            } else { // replace
//#endif
//                STAT_CODE(
//                    {
//                        if (opcode_cache[hash].code == 0) {
//                            perf_stat_increase(STAT_TR_OPCODE_CACHE_FILL);
//                        } else {
//                            perf_stat_increase(STAT_TR_OPCODE_CACHE_REPLACE);
//                        }
//                    });
//                opcode_cache[hash].opcode = opcode;
//                opcode_cache[hash].code = code;
//                opcode_cache[hash].thumb_p = thumb_p;
//#if OPCODE_CACHE_COUNT_MAX != 0x0
//                opcode_cache[hash].count = OPCODE_CACHE_COUNT_INIT;
//            }
//#endif
//        } else {
//            DBCODE(true, {
//                            STAT_CODE(perf_stat_increase(STAT_TR_OPCODE_CACHE_SET_SAME));
//                        });
//        }
//        bt_unlock(&opcode_cache_lock);
//    }
//}
//
//static inline bool opcode_cache_get(uint32_t code, sisa_opcode_t *opcode)
//{
//    bool thumb_p = (arm_home.instr_set == SL_INSTR_SET_THUMB);
//
//    if (code == 0) { // in some case real code will be 0;
//        return false;
//    }
//    if (bt_try_lock(&opcode_cache_lock)) {
//        uint32_t hash = opcode_cache_hash(code);
//        if (opcode_cache[hash].code == code && opcode_cache[hash].thumb_p == thumb_p) { // hit
//#if OPCODE_CACHE_COUNT_MAX != 0x0
//            if (opcode_cache[hash].count < OPCODE_CACHE_COUNT_MAX) {
//                opcode_cache[hash].count ++;
//            }
//#endif
//            *opcode = opcode_cache[hash].opcode;
//            STAT_CODE(perf_stat_increase(STAT_TR_OPCODE_CACHE_HIT));
//            bt_unlock(&opcode_cache_lock);
//            return true;
//        } else {
//            STAT_CODE(perf_stat_increase(STAT_TR_OPCODE_CACHE_MISS));
//            bt_unlock(&opcode_cache_lock);
//            return false;
//        }
//    } else {
//        return false;
//    }
//}


//static inline sisa_opcode_t sisa_decode_get_opcode_internal(uint32_t code);
//static inline sisa_opcode_t sisa_decode_get_opcode(uint32_t code)
//{
//    sisa_opcode_t opcode;
//    if (!UBT_OPT(intrp, disable_opcode_cache)) {
//        bool hit;
//        hit = opcode_cache_get(code, &opcode);
//        if (hit) {
//            return opcode;
//        }
//    }
//    opcode = sisa_decode_get_opcode_internal(code);
//    if (!UBT_OPT(intrp, disable_opcode_cache)) {
//        opcode_cache_set(code, opcode);
//    }
//
//    return opcode;
//}
//
//static inline sisa_opcode_t sisa_decode_get_opcode_internal(uint32_t code)
//{
//    uint32_t i=0;
//
//    STAT_CODE(perf_stat_increase(STAT_INTRP_ARM_GET_OPCODE_CNT));
//    for (i=1; i<SISA_OP_ARM_MAX; i++) {
//        STAT_CODE(perf_stat_increase(STAT_INTRP_ARM_GET_OPCODE_LOOP_CNT));
//        assert(sisa_instr_tbl[i].opkind != PSEUDO_CONNECT_NODE);
//        /* leaf end, reach TAIL node */
//        if (SISA_DEC_NODE_LEAF_END(i)) {
//            assert(sisa_instr_tbl[i].opkind == PSEUDO_TAIL_NODE);
//            return SISA_OP_NA;
//        }
//
//        if (SISA_DEC_NODE_MATCH(code, i)) {
//            if (sisa_instr_tbl[i].opkind == PSEUDO_LINK_NODE) {
//                /* branch knot, reach LINK node */
//                i = sisa_instr_tbl[i].cond;
//                // i++ will help adjust to the LINK NODE
//                assert(sisa_instr_tbl[i].opkind == PSEUDO_CONNECT_NODE);
//            } else {
//                /* found, reach regular node */
//                assert(sisa_instr_tbl[i].opkind < LAST_OP);
//                return (sisa_opcode_t)i;
//            }
//        }
//    }
//    return SISA_OP_NA;
//}

//static inline sisa_opcode_t sisa_decode_get_opcode_for_16bit_thumb_internal(uint32_t thumb_code);
//static inline sisa_opcode_t sisa_decode_get_opcode_for_16bit_thumb(uint32_t thumb_code)
//{
//    sisa_opcode_t opcode;
//    thumb_code=thumb_code & 0xFFFF;
//    if (!UBT_OPT(intrp, disable_opcode_cache)) {
//        bool hit;
//        hit = opcode_cache_get(thumb_code, &opcode);
//        if (hit) {
//            return opcode;
//        }
//    }
//    opcode = sisa_decode_get_opcode_for_16bit_thumb_internal(thumb_code);
//    if (!UBT_OPT(intrp, disable_opcode_cache)) {
//        opcode_cache_set(thumb_code, opcode);
//    }
//
//    return opcode;
//}
//
//static inline sisa_opcode_t sisa_decode_get_opcode_for_16bit_thumb_internal(uint32_t thumb_code)
//{
//    uint32_t i=0;
//
//    for (i=SISA_OP_ARM_MAX+1; i<SISA_OP_THUMB_MAX; i++) {
//
//        assert(sisa_instr_tbl[i].opkind != PSEUDO_CONNECT_NODE);
//
//        /* leaf end, reach TAIL node */
//        if (SISA_DEC_NODE_LEAF_END(i)) {
//            assert(sisa_instr_tbl[i].opkind == PSEUDO_TAIL_NODE);
//            return SISA_OP_NA;
//        }
//        if (SISA_DEC_NODE_MATCH(thumb_code, i)) {
//            if (sisa_instr_tbl[i].opkind == PSEUDO_LINK_NODE) {
//                /* branch knot, reach LINK node */
//                i = sisa_instr_tbl[i].cond;
//                // i++ will help adjust to the LINK NODE
//                assert(sisa_instr_tbl[i].opkind == PSEUDO_CONNECT_NODE);
//            } else {
//                /* found, reach regular node */
//                assert(sisa_instr_tbl[i].opkind < LAST_OP);
//                return (sisa_opcode_t)i;
//            }
//        }
//    }
//    return SISA_OP_NA;
//}
//
//static inline sisa_opcode_t sisa_decode_get_opcode_for_32bit_thumb_internal(uint32_t thumb_code);
//static inline sisa_opcode_t sisa_decode_get_opcode_for_32bit_thumb(uint32_t thumb_code)
//{
//    sisa_opcode_t opcode;
//    if (!UBT_OPT(intrp, disable_opcode_cache)) {
//        bool hit;
//        hit = opcode_cache_get(thumb_code, &opcode);
//        if (hit) {
//            return opcode;
//        }
//    }
//    opcode = sisa_decode_get_opcode_for_32bit_thumb_internal(thumb_code);
//    if (!UBT_OPT(intrp, disable_opcode_cache)) {
//        opcode_cache_set(thumb_code, opcode);
//    }
//
//    return opcode;
//}
//
//static inline sisa_opcode_t sisa_decode_get_opcode_for_32bit_thumb_internal(uint32_t thumb_code)
//{
//    uint32_t i=0;
//
//    for (i=SISA_OP_THUMB_MAX+1; i<SISA_OP_THUMB_32_MAX; i++) {
//
//        assert(sisa_instr_tbl[i].opkind != PSEUDO_CONNECT_NODE);
//
//        /* leaf end, reach TAIL node */
//        if (SISA_DEC_NODE_LEAF_END(i)) {
//            assert(sisa_instr_tbl[i].opkind == PSEUDO_TAIL_NODE);
//            return SISA_OP_NA;
//        }
//        if (SISA_DEC_NODE_MATCH(thumb_code, i)) {
//            if (sisa_instr_tbl[i].opkind == PSEUDO_LINK_NODE) {
//                /* branch knot, reach LINK node */
//                i = sisa_instr_tbl[i].cond;
//                // i++ will help adjust to the LINK NODE
//                assert(sisa_instr_tbl[i].opkind == PSEUDO_CONNECT_NODE);
//            } else {
//                /* found, reach regular node */
//                assert(sisa_instr_tbl[i].opkind < LAST_OP);
//                return (sisa_opcode_t)i;
//            }
//        }
//    }
//    return SISA_OP_NA;
//}
//
//static inline sisa_opcode_t sisa_decode_get_opcode_for_thumb(uint32_t *code, uint32_t* instr_size)
//{
//    *instr_size = 2;
//    uint32_t thumb_code = *code & 0xffff;
//    if((thumb_code >> 11) <= 0x1c) {
//        *instr_size = 2;
//        *code = thumb_code;
//        return sisa_decode_get_opcode_for_16bit_thumb(thumb_code);
//    } else {
//        *instr_size = 4;
//        thumb_code = (*code >> 16) + (*code << 16);
//        *code = thumb_code;
//        return sisa_decode_get_opcode_for_32bit_thumb(thumb_code);
//    }
//
//    return SISA_OP_NA;
//}


static inline uint32_t sisa_decode_get_bits(uint32_t code, uint8_t bit_pos_start, uint8_t bit_pos_end)
{
    uint32_t mask = ((2u << (bit_pos_end)) - 1);
    return (code & mask) >> bit_pos_start;
}


static inline uint32_t sisa_decode_get_opnd(uint32_t code, const sisa_opnd_t* opnd)
{
    uint8_t start_bit = opnd->start_bit;
    uint8_t end_bit = opnd->end_bit;
    return sisa_decode_get_bits(code, start_bit, end_bit);
}


static inline uint32_t sisa_decode_get_opnd_ext(uint32_t code, const sisa_opnd_t* opnd)
{
    uint8_t start_bit = opnd->bit1;
    uint8_t end_bit = opnd->bit2;
    return sisa_decode_get_bits(code, start_bit, end_bit);
}

static inline uint32_t sisa_decode_get_imm_merge(uint32_t code, const sisa_opnd_t* opnd)
{
    uint32_t immH, immL;

    immH = sisa_decode_get_opnd(code, opnd);
    immL = sisa_decode_get_opnd_ext(code, opnd);

    return ( (immH << (opnd->bit2 - opnd->bit1 + 1)) + immL );
}

static inline uint32_t sisa_decode_opnd_number(const sisa_instr_t* desc_entry, uint32_t opnd_idx, uint32_t code)
{

    uint32_t arg;
    const sisa_opnd_t * opnd;
    opnd = &(desc_entry->opnds[opnd_idx]);

    arg = sisa_decode_get_opnd(code, opnd);

    if (opnd->type== SISA_OPND_TYPE_GPR_MERGE) {
            assert (arg <= 7u);
            if ((code & (1u << opnd->bit1)) != 0u) {
                arg = arg+8u;
            }
    } else if (opnd->type== SISA_OPND_TYPE_GPR_CONST) {
            arg = opnd->bit1;
    } else if ((opnd->type == SISA_OPND_TYPE_FPSR)
        || ((opnd->type == SISA_OPND_TYPE_FPDSR) && (SISA_BIT(code, opnd->bit2) == 0u))) {
        arg = (arg << 1u) + SISA_BIT(code,opnd->bit1);
    } else if ((opnd->type == SISA_OPND_TYPE_FPDR)
        || ((opnd->type == SISA_OPND_TYPE_FPDSR) && (SISA_BIT(code, opnd->bit2) == 1u))) {
        arg = ((SISA_BIT(code,opnd->bit1)) << 4u) + arg;
    }


    return arg;
}

static inline uint32_t sisa_get_branch_imm_merge(uint32_t code, sisa_opcode_t op)
{
    uint32_t j1, j2, s, i1, i2;
    uint32_t imm10H, imm12L;
    uint32_t imm32;
    /*I1 = NOT(J1 EOR S); I2 = NOT(J2 EOR S);
    imm32 = SignExtend(S:I1:I2:imm10H:imm10L:???.00???., 32); */

    j1 = 0;
    if ((code & 0x20000000) == 0x20000000) j1 = 1;
    j2 = 0;
    if ((code & 0x8000000) == 0x8000000) j2 = 1;
    s = 0;
    if ((code & 0x400) == 0x400) s = 1;

    i1 = (j1 ^ s);
    i1 = ~i1;
    i1 = i1 & 0x1;

    i2 = (j2 ^ s);
    i2 = ~i2;
    i2 = i2 & 0x1;

    assert(i1==0 || i1==1);
    assert(i2==0 || i2==1);
    assert(s==0 || s==1);

    imm10H = (code) & 0x3ff;
    if(op == SISA_OP_BLX_imm_T2) {
        imm12L = (code >> 17) & 0x3ff;
        imm12L = imm12L << 2;
    } else {
        imm12L = (code >> 16) & 0x7ff;
        imm12L = imm12L << 1;
    }

    imm10H = imm10H << 12;
    imm32 = imm10H | imm12L;

    imm32 = imm32 | (i2<<22);
    imm32 = imm32 | (i1<<23);
    imm32 = imm32 | (s<<24);


    if((imm32 &  0x1000000) == 0x1000000) {
        imm32|=0xfe000000;
    }
    return imm32;
}

// TODO merge the code into imm merge
static inline uint32_t sisa_get_imm_thumb_merge(uint32_t code, sisa_opcode_t op)
{
    uint32_t j1, j2, s, imm11L, imm32 = 0;
    if (op == SISA_OP_B_T3) {
        /*imm32 = SignExtend(S:J2:J1:imm6:imm11:¡¯0¡¯, 32);*/
        j1 = 0;
        if ((code & J1_BIT) == J1_BIT) j1 = 1;
        j2 = 0;
        if ((code & J2_BIT) == J2_BIT) j2 = 1;
        s = 0;
        if ((code & S_BIT) == S_BIT) s = 1;

        uint32_t imm6H = (code >> 16) & 0x3f;
        imm11L = code & 0x7ff;

        imm6H = imm6H << 11;
        imm32 = imm6H | imm11L;

        imm32 = imm32 | (j1<<17);
        imm32 = imm32 | (j2<<18);
        imm32 = imm32 | (s<<19);

        if((imm32 &  0x80000) == 0x80000) {
            imm32|=0xfff00000;
        }
        imm32 = imm32 << 1;
    } else if(op == SISA_OP_MOV_imm_T3) {
        imm32 = (code&0xff) + ((code>>4)&0xf00) + ((code >>15)&0x800) + ((code>>4)&0xf000);
    } else if(op == SISA_OP_B_T4) {
        imm32 = sisa_get_branch_imm_merge(code, op);
    }
    return imm32;
}


static inline bool sisa_decode_code_bit_set(uint32_t code, uint32_t bit_map)
{
    if ((code & bit_map) == bit_map) {
        return true;
    } else {
        return false;
    }
}

static inline bool sisa_op_has_reg_form(sisa_opcode_t op, sisa_opkind_t opkind)
{
    if(opkind == LD_OP) {
                if (CHECK_REG_FLAG(sisa_instr_tbl[op].cgflags)) {
            assert( op == SISA_OP_LDRD_reg
                || op == SISA_OP_LDR_reg
                || op == SISA_OP_LDRB_reg
                || op == SISA_OP_LDRH
                || op == SISA_OP_LDRSB
                || op == SISA_OP_LDRSH
                || op == SISA_OP_LDRSB_reg_T1
                || op == SISA_OP_LDRSB_reg_T2
                || op == SISA_OP_LDR_reg_T1
                || op == SISA_OP_LDR_reg_T2
                || op == SISA_OP_LDRH_reg_T1
                || op == SISA_OP_LDRH_reg_T2
                || op == SISA_OP_LDRB_reg_T1
                || op == SISA_OP_LDRB_reg_T2
                || op == SISA_OP_LDRSH_reg_T1
                || op == SISA_OP_LDRSH_reg_T2
                );
                    return TRUE;
                }
    }

    if(opkind == ST_OP) {
                if (CHECK_REG_FLAG(sisa_instr_tbl[op].cgflags)) {
            assert( op == SISA_OP_STRD_reg
                || op == SISA_OP_STR_reg
                || op == SISA_OP_STR_reg_T1
                || op == SISA_OP_STR_reg_T2
                || op == SISA_OP_STRB_reg
                || op == SISA_OP_STRH_reg
                || op == SISA_OP_STRH_reg_T1
                || op == SISA_OP_STRH_reg_T2
                || op == SISA_OP_STRB_reg_T1
                || op == SISA_OP_STRB_reg_T2
                );
            return TRUE;
        }
    }
    return FALSE;
}


static inline bool sisa_op_has_imm_form(sisa_opcode_t op, sisa_opkind_t opkind)
{
    if(opkind == LD_OP) {
                if (CHECK_IMM_FLAG(sisa_instr_tbl[op].cgflags)) {
            assert( op == SISA_OP_LDRD_imm
                || op == SISA_OP_LDR_imm
                || op == SISA_OP_LDRB_imm
                || op == SISA_OP_LDRH_imm
                || op == SISA_OP_LDRSB_imm
                || op == SISA_OP_LDRSB_imm_T1
                || op == SISA_OP_LDRSB_imm_T2
                || op == SISA_OP_LDRSH_imm
                || op == SISA_OP_LDRSH_imm_T1
                || op == SISA_OP_LDRSH_imm_T2
                || op == SISA_OP_STRH_imm
                || op == SISA_OP_LDR_imm_T1 /*Thumb*/
                || op == SISA_OP_LDRB_imm_T1
                || op == SISA_OP_LDRH_imm_T1
                || op == SISA_OP_LDRH_imm_T2
                || op == SISA_OP_LDRH_imm_T3
                || op == SISA_OP_LDR_imm_T2
                || op == SISA_OP_LDRHT_T1);
            return TRUE;
        }
    }
    if(opkind == ST_OP) {
                if (CHECK_IMM_FLAG(sisa_instr_tbl[op].cgflags)) {
#if 0
            assert( op == SISA_OP_STRD_imm
                || op == SISA_OP_STR_imm
                || op == SISA_OP_STRB_imm
                || op == SISA_OP_STRH_imm
                || op == SISA_OP_STR_imm_T1 /* Thumb */
                || op == SISA_OP_STRB_imm_T1
                || op == SISA_OP_STRH_imm_T1
                || op == SISA_OP_STR_imm_T2 );
#endif
            return TRUE;
        }
    }
    return FALSE;
}

static inline bool sisa_op_has_lit_form(sisa_opcode_t op, sisa_opkind_t opkind)
{
    if(opkind == LD_OP) {
        if (CHECK_LIT_FLAG(sisa_instr_tbl[op].cgflags)) {
#if 0
            assert( op == SISA_OP_LDRD_lit
                || op == SISA_OP_LDRB_lit
                || op== SISA_OP_LDRH_lit
                || op == SISA_OP_LDRSB_lit
                || op== SISA_OP_LDRSH_lit
                || op == SISA_OP_LDR_lit /*THUMB*/
                || op == SISA_OP_LDR_lit_T1 );
#endif
            return TRUE;
        }
    }
    return FALSE;
}



static inline bool sisa_op_has_sft_reg_form(sisa_opcode_t op, sisa_opkind_t opkind)
{
    return FALSE;
}

static inline bool sisa_op_has_sxt(sisa_opcode_t op, sisa_opkind_t opkind) {
    if(opkind == LD_OP) {
        if (CHECK_SIGNED_FLAG(sisa_instr_tbl[op].cgflags)) {
            assert(op == SISA_OP_LDRSB
                ||op == SISA_OP_LDRSBT_a1
                ||op == SISA_OP_LDRSBT_a2
                ||op == SISA_OP_LDRSB_imm
                || op == SISA_OP_LDRSB_imm_T1
                || op == SISA_OP_LDRSB_imm_T2
                ||op == SISA_OP_LDRSB_lit
                ||op == SISA_OP_LDRSB_lit_T1
                ||op == SISA_OP_LDRSH
                ||op == SISA_OP_LDRSH_imm
                ||op == SISA_OP_LDRSH_imm_T1
                || op == SISA_OP_LDRSH_imm_T2
                ||op == SISA_OP_LDRSH_lit
                || op == SISA_OP_LDRSH_lit_T1
                || op == SISA_OP_LDRSB_reg_T1 /*Thumb*/
                || op == SISA_OP_LDRSB_reg_T2
                || op == SISA_OP_LDRSH_reg_T1
                || op == SISA_OP_LDRSH_reg_T2
                );
            return TRUE;
        }
    }
    if(opkind == EXT_OP) {
        if (CHECK_SIGNED_FLAG(sisa_instr_tbl[op].cgflags)) {
            assert( op == SISA_OP_SXTB16
                || op == SISA_OP_SXTB16_T1
                || op == SISA_OP_SXTAB16
                || op == SISA_OP_SXTAB16_T1
                || op == SISA_OP_SXTB
                || op == SISA_OP_SXTB_T1
                || op == SISA_OP_SXTB_T2
                || op == SISA_OP_SXTAB
                || op == SISA_OP_SXTAB_T1
                || op == SISA_OP_SXTH
                || op == SISA_OP_SXTH_T1
                || op == SISA_OP_SXTH_T2
                || op == SISA_OP_SXTAH
                || op == SISA_OP_SXTAH_T1
                );
            return TRUE;
        }
    }

    return FALSE;

}

static inline bool sisa_op_has_uxt(sisa_opcode_t op, sisa_opkind_t opkind) {
    if(opkind == LD_OP) {
        if (CHECK_UNSIGNED_FLAG(sisa_instr_tbl[op].cgflags)) {
            assert(op == SISA_OP_LDRB_reg
                ||op == SISA_OP_LDRB_imm
                ||op == SISA_OP_LDRB_lit
                ||op == SISA_OP_LDRB_lit_T1
                ||op == SISA_OP_LDRBT_a1
                ||op == SISA_OP_LDRBT_a2
                ||op == SISA_OP_LDRH
                ||op == SISA_OP_LDRH_imm
                ||op == SISA_OP_LDRH_imm_T2
                || op == SISA_OP_LDRH_imm_T3
                ||op == SISA_OP_LDRH_lit
                || op == SISA_OP_LDRH_lit_T1
                || op == SISA_OP_LDRB_reg_T1
                || op == SISA_OP_LDRB_reg_T2
                || op == SISA_OP_LDRH_reg_T1
                || op == SISA_OP_LDRH_reg_T2
                || op == SISA_OP_LDRB_imm_T1
                || op == SISA_OP_LDRB_imm_T2
                || op == SISA_OP_LDRB_imm_T3
                || op == SISA_OP_LDRH_imm_T1
                || op == SISA_OP_LDRHT_T1);
            return TRUE;
        }
    }
    if(opkind == EXT_OP) {
        if (CHECK_UNSIGNED_FLAG(sisa_instr_tbl[op].cgflags)) {
            assert(op == SISA_OP_UXTB16||
               op == SISA_OP_UXTB16_T1 ||
               op == SISA_OP_UXTAB16||
               op == SISA_OP_UXTAB16_T1 ||
               op == SISA_OP_UXTB||
               op == SISA_OP_UXTB_T1||
               op == SISA_OP_UXTB_T2||
               op == SISA_OP_UXTAB||
               op == SISA_OP_UXTAB_T1||
               op == SISA_OP_UXTH||
               op == SISA_OP_UXTH_T1||
               op == SISA_OP_UXTH_T2||
               op == SISA_OP_UXTAH||
               op == SISA_OP_UXTAH_T1 ||
               op == SISA_OP_USAT||
               op == SISA_OP_USAT_T1||
               op == SISA_OP_USAT16 ||
               op == SISA_OP_USAT16_T1
               );
           return TRUE;
        }
    }
    return FALSE;

}


static inline bool sisa_op_has_size_dw(sisa_opcode_t op, sisa_opkind_t opkind) {
    if(opkind == LD_OP) {
        if (CHECK_SIZE_DWORD_FLAG(sisa_instr_tbl[op].cgflags)) {
            assert(op == SISA_OP_LDRD_reg
                ||op == SISA_OP_LDRD_imm
                ||op == SISA_OP_LDRD_imm_T1
                ||op == SISA_OP_LDRD_lit
                ||op ==SISA_OP_LDRD_lit_T1);
            return TRUE;
        }
    }

    if(opkind == ST_OP) {
        if (CHECK_SIZE_DWORD_FLAG(sisa_instr_tbl[op].cgflags)) {
            assert(op == SISA_OP_STRD_reg
                ||op == SISA_OP_STRD_imm
                ||op == SISA_OP_STRD_imm_T1);
            return TRUE;
        }
    }
    return FALSE;
}

static inline bool sisa_op_has_size_b(sisa_opcode_t op, sisa_opkind_t opkind) {
    if(opkind == LD_OP) {
        if (CHECK_SIZE_BYTE_FLAG(sisa_instr_tbl[op].cgflags)) {
            assert(op == SISA_OP_LDRB_reg
                ||op == SISA_OP_LDRB_imm
                ||op == SISA_OP_LDRB_imm_T1 /*THUMB*/
                || op == SISA_OP_LDRB_imm_T2
                || op == SISA_OP_LDRB_imm_T3
                ||op == SISA_OP_LDRB_lit
                ||op == SISA_OP_LDRB_lit_T1
                ||op == SISA_OP_LDRSB
                ||op == SISA_OP_LDRBT_a1
                ||op == SISA_OP_LDRBT_a2
                ||op == SISA_OP_LDRSBT_a1
                ||op == SISA_OP_LDRSBT_a2
                ||op == SISA_OP_LDRSB_imm
                ||op == SISA_OP_LDRSB_imm_T1
                ||op == SISA_OP_LDRSB_imm_T2
                ||op == SISA_OP_LDRSB_lit
                ||op == SISA_OP_LDRSB_lit_T1
                || op == SISA_OP_LDRSB_reg_T1 /*Thumb*/
                || op == SISA_OP_LDRSB_reg_T2
                || op == SISA_OP_LDRB_reg_T1
                || op == SISA_OP_LDRB_reg_T2
                );
            return TRUE;
        }
    }

    if(opkind == ST_OP) {
        if (CHECK_SIZE_BYTE_FLAG(sisa_instr_tbl[op].cgflags)) {
            assert(op == SISA_OP_STRB_reg
                ||op == SISA_OP_STRB_imm
                ||op == SISA_OP_STRBT_a1
                ||op == SISA_OP_STRBT_a2
                ||op == SISA_OP_STRB_reg_T1 /*THUMB*/
                ||op == SISA_OP_STRB_reg_T2 /*THUMB*/
                ||op == SISA_OP_STRB_imm_T1
                ||op == SISA_OP_STRB_imm_T2
                ||op == SISA_OP_STRB_imm_T3
                );
            return TRUE;
        }
    }

    return FALSE;
}

static inline bool sisa_op_has_size_w(sisa_opcode_t op, sisa_opkind_t opkind) {
    if(opkind == LD_OP) {
        return FALSE;
    }
    return FALSE;
}

static inline bool sisa_op_has_size_hw(sisa_opcode_t op, sisa_opkind_t opkind) {
    if(opkind == LD_OP) {
        if (CHECK_SIZE_HWORD_FLAG(sisa_instr_tbl[op].cgflags)) {
            assert(op == SISA_OP_LDRH
                ||op == SISA_OP_LDRH_imm
                ||op == SISA_OP_LDRH_imm_T1 /*THUMB*/
                ||op == SISA_OP_LDRH_imm_T2
                || op == SISA_OP_LDRH_imm_T3
                ||op == SISA_OP_LDRH_lit
                || op == SISA_OP_LDRH_lit_T1
                ||op == SISA_OP_LDRSH
                ||op == SISA_OP_LDRSH_imm
                ||op == SISA_OP_LDRSH_imm_T1
                || op == SISA_OP_LDRSH_imm_T2
                ||op == SISA_OP_LDRSH_lit
                || op == SISA_OP_LDRSH_lit_T1
                || op == SISA_OP_LDRH_reg_T1 /*Thumb*/
                || op == SISA_OP_LDRH_reg_T2
                || op == SISA_OP_LDRSH_reg_T1
                || op == SISA_OP_LDRSH_reg_T2
                || op == SISA_OP_LDRHT_T1
                );
            return TRUE;
        }
    }

    if(opkind == ST_OP) {
        if (CHECK_SIZE_HWORD_FLAG(sisa_instr_tbl[op].cgflags)) {
                    assert(op == SISA_OP_STRH_reg
                ||op == SISA_OP_STRH_imm
                ||op == SISA_OP_STRHT_a1
                ||op == SISA_OP_STRHT_a2
                || op == SISA_OP_STRH_reg_T1 /*Thumb*/
                || op == SISA_OP_STRH_reg_T2
                || op == SISA_OP_STRH_imm_T1
                || op == SISA_OP_STRH_imm_T2
                || op == SISA_OP_STRH_imm_T3 );
            return TRUE;
        }
    }

    return FALSE;
}


/* interface like this is specific for certain functionality and should be limited */
static inline bool sisa_op_has_double_dest(sisa_opcode_t op, sisa_opkind_t opkind)
{
    if(opkind == UMUL_OP) {
        return TRUE;
    }

    if(opkind == SMUL_OP) {
        if ((op == SISA_OP_SMULL) || (op == SISA_OP_SMLAL) || (op == SISA_OP_SMLALBB)
            ||(op == SISA_OP_SMULL_T1) || (op == SISA_OP_SMLAL_T1) ||(op==SISA_OP_SMLALBB_T1)) {
            return TRUE;
        }
    }

    if(opkind == SMULD_OP) {
        if ((op == SISA_OP_SMLSLD) || (op == SISA_OP_SMLALD)
            ||(op == SISA_OP_SMLSLD_T1) || (op == SISA_OP_SMLALD_T1) ) {
            return TRUE;
        }
    }

    return FALSE;
}

static inline bool sisa_op_is_prefetch(sisa_opcode_t op)
{

/// ToDO Need to add THUMB prefetch !!!

    if((op == SISA_OP_PLD_imm)
       || (op == SISA_OP_PLD_lit)
       || (op == SISA_OP_PLD)
       || (op == SISA_OP_PLI_imm)
       || (op == SISA_OP_PLI)) {
       return TRUE;
    }

    return FALSE;
}

static inline bool sisa_op_is_ldr(sisa_opcode_t op)
{
    if(op == SISA_OP_LDR_reg
        ||op == SISA_OP_LDR_imm
        ||op == SISA_OP_LDR_lit
        || /*THUMB*/ op == SISA_OP_LDR_imm_T1
        || /*THUMB*/ op == SISA_OP_LDR_imm_T2
        || /*THUMB*/ op == SISA_OP_LDR_imm_T3
        || /*THUMB*/ op == SISA_OP_LDR_imm_T4
        || /*THUMB*/ op == SISA_OP_LDR_reg_T1
        || /*THUMB*/ op == SISA_OP_LDR_reg_T2
        || /*THUMB*/ op == SISA_OP_LDR_lit_T1
        || /*THUMB*/ op == SISA_OP_LDR_lit_T2) {
       return TRUE;
    }

    return FALSE;
}

static inline bool sisa_op_is_str(sisa_opcode_t op)
{

    if(op == SISA_OP_STR_reg
        ||op == SISA_OP_STR_imm
        ||/*THUMB*/ op == SISA_OP_STR_reg_T1
        ||/*THUMB*/ op == SISA_OP_STR_reg_T2
        ||/*THUMB*/ op == SISA_OP_STR_imm_T1
        ||/*THUMB*/ op == SISA_OP_STR_imm_T2
        ||/*THUMB*/ op == SISA_OP_STR_imm_T3
        ||/*THUMB*/ op == SISA_OP_STR_imm_T4 ) {
       return TRUE;
    }

    return FALSE;
}

static inline bool sisa_op_is_adr(sisa_opcode_t op)
{

    if(op == SISA_OP_ADR_a1
        ||op == SISA_OP_ADR_a2
        ||/*THUMB*/ op == SISA_OP_ADR_T1
        ||/*THUMB*/ op == SISA_OP_ADR_T2
        ||/*THUMB*/ op == SISA_OP_ADR_T3
        ) {
       return TRUE;
    }

    return FALSE;
}


static inline bool sisa_op_set_flag_by_InITBLOCK(sisa_opcode_t op)
{

    if(op ==  SISA_OP_ADC_reg_T1
       ||op == SISA_OP_ADD_reg_T1
       ||op == SISA_OP_ADD_imm_T1
       ||op == SISA_OP_ADD_imm_T2
       ||op == SISA_OP_ADD_reg_T1
       ||op == SISA_OP_AND_reg_T1
       ||op == SISA_OP_ASR_imm_T1
       ||op == SISA_OP_ASR_reg_T1
       ||op == SISA_OP_BIC_reg_T1
       ||op == SISA_OP_EOR_reg_T1
       ||op == SISA_OP_LSL_imm_T1
       ||op == SISA_OP_LSL_reg_T1
       ||op == SISA_OP_MOV_imm_T1
       ||op == SISA_OP_LSR_reg_T1
       ||op == SISA_OP_LSR_imm_T1
       ||op == SISA_OP_MVN_reg_T1
       ||op == SISA_OP_MUL_reg_T1
       ||op == SISA_OP_ORR_reg_T1
       ||op == SISA_OP_ROR_reg_T1
       ||op == SISA_OP_RSB_imm_T1
       ||op == SISA_OP_SBC_reg_T1
       ||op == SISA_OP_SUB_reg_T1
       ||op == SISA_OP_SUB_imm_T1
       ||op == SISA_OP_SUB_imm_T2
       ||op == SISA_OP_LSR_imm_T1 ) {
       return TRUE;
    }

    return FALSE;
}

#endif




