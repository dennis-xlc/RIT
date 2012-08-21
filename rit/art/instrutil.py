#!/usr/bin/python
import random

def getRandNum(nums):
    if not nums:
        return 0
    if len(nums) < 2:
        return nums[0]
    input = list(nums)
    input.sort()
    ridx = random.randint(0, len(input)-2)
    min = input[ridx]
    max = input[ridx+1]
    if min is max:
        return min
    rnums = [min, min+1, max-1, max]
    rnums.append(random.randint(min, max))
    return random.choice(rnums)

def getRandVFPNum(is_sp=True):
    h_num = 0
    l_num = 0
    rval = random.randint(0, 17)
    if is_sp:
        h_num = 0
        if rval is 0 or rval is 10 or rval is 11:
            l_num = 0
        elif rval is 1:
            l_num = random.randint(1, 0x7fffff)
        elif rval is 2:
            l_num = random.randint(0x800000, 0x7f7fffff)
        elif rval is 3 or rval is 12 or rval is 13:
            l_num = 0x7f800000
        elif rval is 4:
            l_num = random.randint(0x7f800001, 0x7fffffff)
        elif rval is 5 or rval is 14 or rval is 15:
            l_num = 0x80000000
        elif rval is 6:
            l_num = random.randint(0x80000001, 0x807fffff)
        elif rval is 7:
            l_num = random.randint(0x80800000, 0xff7fffff)
        elif rval is 8 or rval is 16 or rval is 17:
            l_num = 0xff800000
        elif rval is 9:
            l_num = random.randint(0xff800001, 0xffffffff)
        else:
            l_num = random.randint(0, 0xffffffff)
    else:
        if rval is 0 or rval is 10 or rval is 11:
            h_num = 0
            l_num = 0
        elif rval is 1:
            h_num = random.randint(0, 0xfffff)
            l_num = random.randint(0, 0xffffffff)
        elif rval is 2:
            h_num = random.randint(0x100000, 0x7fefffff)
            l_num = random.randint(0, 0xffffffff)
        elif rval is 3 or rval is 12 or rval is 13:
            h_num = 0x7ff00000
            l_num = 0
        elif rval is 4:
            h_num = random.randint(0x7ff00000, 0x7fffffff)
            l_num = random.randint(0, 0xffffffff)
        elif rval is 5 or rval is 14 or rval is 15:
            h_num = 0x80000000
            l_num = 0
        elif rval is 6:
            h_num = random.randint(0x80000000, 0x800fffff)
            l_num = random.randint(0, 0xffffffff)
        elif rval is 7:
            h_num = random.randint(0x80100000, 0xffefffff)
            l_num = random.randint(0, 0xffffffff)
        elif rval is 8 or rval is 16 or rval is 17:
            h_num = 0xfff00000
            l_num = 0
        elif rval is 9:
            h_num = random.randint(0xfff00000, 0xffffffff)
            l_num = random.randint(0, 0xffffffff)
        else:
            h_num = random.randint(0, 0xffffffff)
            l_num = random.randint(0, 0xffffffff)
    return (l_num, h_num)

            


def test():
    nums = (0, 1, 31, 0xff, 0xffffffff)
    print "Call getRandNum..."
    for i in range(10):
        print getRandNum(nums),


if __name__ == '__main__':
    test()
