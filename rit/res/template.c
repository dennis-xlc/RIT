#include <stdio.h>

#define MEM_LEN 16
#define MEM_OFFSET 16

@init_regs
unsigned int reg_apsr;

//unsigned int bak_regs[4]; //fp, ip, sp and lr
unsigned int fp_bak;
unsigned int lr_bak;
unsigned int sp_bak;

unsigned int mem_data[MEM_LEN+MEM_OFFSET*2] = {@init_mem};

inline void set_regs() __attribute__ ((always_inline));
inline void get_regs() __attribute__ ((always_inline));
inline void print_regs() __attribute__ ((always_inline));

void init()
{
    //init variable
    reg_13 = (unsigned int)&mem_data[MEM_OFFSET];
    asm volatile(
        "ADDS r0, r0, #0\n\t" //set APSR
        "MRS r1, apsr\n\t"
        "LDR r0, =reg_apsr\n\t"
        "STR r1, [r0]"
    );
}

void run_test()
{
    //backup fp, sp and lr
    asm volatile(
        "LDR r0, =fp_bak\n\t"
        "STR fp, [r0]\n\t"
        "LDR r0, =sp_bak\n\t"
        "STR sp, [r0]\n\t"
        "LDR r0, =lr_bak\n\t"
        "STR lr, [r0]"
    );
    
    set_regs();
    //exec randome instr
    asm volatile(
            @rit_inst
    );
    get_regs();

    //restore fp, sp and lr
    asm volatile(
        "LDR r0, =fp_bak\n\t"
        "LDR fp, [r0]\n\t"
        "LDR r0, =sp_bak\n\t"
        "LDR sp, [r0]\n\t"
        "LDR r0, =lr_bak\n\t"
        "LDR lr, [r0]"
    );
}

int main()
{
    init();
    print_regs();
    run_test();
    print_regs();
    return 0;
}

inline void set_regs()
{
    asm volatile(
        "LDR r0, =reg_0\n\t"
        "LDR r0, [r0]\n\t"
        "LDR r1, =reg_1\n\t"
        "LDR r1, [r1]\n\t"
        "LDR r2, =reg_2\n\t"
        "LDR r2, [r2]\n\t"
        "LDR r3, =reg_3\n\t"
        "LDR r3, [r3]\n\t"
        "LDR r4, =reg_4\n\t"
        "LDR r4, [r4]\n\t"
        "LDR r5, =reg_5\n\t"
        "LDR r5, [r5]\n\t"
        "LDR r6, =reg_6\n\t"
        "LDR r6, [r6]\n\t"
        "LDR r7, =reg_7\n\t"
        "LDR r7, [r7]\n\t"
        "LDR r8, =reg_8\n\t"
        "LDR r8, [r8]\n\t"
        "LDR r9, =reg_9\n\t"
        "LDR r9, [r9]\n\t"
        "LDR r10, =reg_10\n\t"
        "LDR r10, [r10]\n\t"
        "LDR r11, =reg_11\n\t"
        "LDR r11, [r11]\n\t"
        "LDR r12, =reg_12\n\t"
        "LDR r12, [r12]\n\t"
        "LDR sp, =reg_13\n\t"
        "LDR sp, [sp]\n\t"
        "LDR lr, =reg_14\n\t"
        "LDR lr, [lr]"
    );
}

inline void get_regs()
{
    asm volatile(
        "LDR sp, =reg_13\n\t"
        "LDR sp, [sp]"
    );
    //TODO 
    asm volatile(
        "PUSH {r0}\n\t"
        "LDR r0, =reg_1\n\t"
        "STR r1, [r0]\n\t"
        "LDR r0, =reg_2\n\t"
        "STR r2, [r0]\n\t"
        "LDR r0, =reg_3\n\t"
        "STR r3, [r0]\n\t"
        "LDR r0, =reg_4\n\t"
        "STR r4, [r0]\n\t"
        "LDR r0, =reg_5\n\t"
        "STR r5, [r0]\n\t"
        "LDR r0, =reg_6\n\t"
        "STR r6, [r0]\n\t"
        "LDR r0, =reg_7\n\t"
        "STR r7, [r0]\n\t"
        "LDR r0, =reg_8\n\t"
        "STR r8, [r0]\n\t"
        "LDR r0, =reg_9\n\t"
        "STR r9, [r0]\n\t"
        "LDR r0, =reg_10\n\t"
        "STR r10, [r0]\n\t"
        "LDR r0, =reg_11\n\t"
        "STR r11, [r0]\n\t"
        "LDR r0, =reg_12\n\t"
        "STR r12, [r0]\n\t"
        "LDR r0, =reg_13\n\t"
        "STR sp, [r0]\n\t"
        "LDR r0, =reg_14\n\t"
        "STR lr, [r0]\n\t"
        "POP {r0}\n\t"
        "LDR r1, =reg_0\n\t"
        "STR r0, [r1]\n\t"

        "MRS r1, apsr\n\t"
        "LDR r0, =reg_apsr\n\t"
        "STR r1, [r0]"

    );
}

inline void print_regs()
{
    printf("r0 = 0x%x\n", reg_0);
    printf("r1 = 0x%x\n", reg_1);
    printf("r2 = 0x%x\n", reg_2);
    printf("r3 = 0x%x\n", reg_3);
    printf("r4 = 0x%x\n", reg_4);
    printf("r5 = 0x%x\n", reg_5);
    printf("r6 = 0x%x\n", reg_6);
    printf("r7 = 0x%x\n", reg_7);
    printf("r8 = 0x%x\n", reg_8);
    printf("r9 = 0x%x\n", reg_9);
    printf("r10 = 0x%x\n", reg_10);
    printf("r11 = 0x%x\n", reg_11);
    printf("r12 = 0x%x\n", reg_12);
    printf("sp = 0x%x\n", reg_13);
    printf("lr = 0x%x\n", reg_14);

    printf("APSR = %08x\n", reg_apsr);
}
