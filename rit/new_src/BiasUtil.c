#include "BiasUtil.h"

void random_bias_reg_2(uint32_t *reg_1, uint32_t *reg_2)
{
    //50% reg1 = reg_2
    uint32_t rand_val = rand();
    if ((rand_val % 2) == 0) {
        *reg_1 = *reg_2;
    }   
}

void random_bias_reg_3(uint32_t *reg_1, uint32_t *reg_2, uint32_t *reg_3)
{
    uint32_t rand_val = rand();
    switch(rand_val % 8) {
        case 0:
        case 1:
        case 2:
            *reg_1 = *reg_3;
            *reg_2 = *reg_3;
            break;
        case 3:
            *reg_3 = *reg_2;
            break;
        case 4:
            *reg_2 = *reg_1;
            break;
        case 5:
            *reg_1 = *reg_3;
            break;
        default:
            break;
    }   
}

void random_bias_reg_4(uint32_t *reg_1, uint32_t *reg_2, uint32_t *reg_3, uint32_t *reg_4)
{
    uint32_t rand_val = rand();
    switch(rand_val % 16) {
        case 0:
        case 1:
        case 2:
        case 3:
            *reg_1 = *reg_4;
            *reg_2 = *reg_4;
            *reg_3 = *reg_4;
            break;
        case 4:
            *reg_1 = *reg_3;
            *reg_2 = *reg_3;
            break;
        case 5:
            *reg_2 = *reg_4;
            *reg_3 = *reg_4;
            break;
        case 6:
            *reg_3 = *reg_1;
            *reg_4 = *reg_1;
            break;
        case 7:
            *reg_4 = *reg_2;
            *reg_1 = *reg_2;
            break;
        case 8:
            *reg_1 = *reg_2;
            break;
        case 9:
            *reg_2 = *reg_3;
            break;
        case 10:
            *reg_3 = *reg_4;
            break;
        case 11:
            *reg_4 = *reg_1;
            break;
        case 12:
            *reg_1 = *reg_3;
            break;
        case 13:
            *reg_2 = *reg_4;
            break;
        default:
            break;
    }
}

