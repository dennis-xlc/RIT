#!/usr/bin/python

import argparse
import sys, os

import rit
import vfp

global options

ROOT=os.path.realpath(os.path.dirname(__file__))

class CTX():
    NEST = 'nest'
    ISA = 'InstrSet_ARM'
    NUM = 'num'
    REGALLOC = 'reg_alloc_opt'
    BB = 'blocks'
    INSTLIMIT = 'instr_limit'

def getInitContext(f_block):
    """docstring for getInitContext"""
    ctx = {}
    ctx[CTX.ISA] = True
    ctx[CTX.BB] = {}
    ctx[CTX.NEST] = getOption('nest')
    ctx['rit'] = rit.RIT(getOption('ritpath'), vfp.gen_instr)
    ctx[CTX.NUM] = getOption('num')
    ctx['tb'] = False
    if f_block:
        ctx['getRBlock'] = f_block
    return ctx

def getOption(option):
    global options
    if option in options:
        return options[option]
    else:
        print "[WARNING] unknown option %s" % option
        return None

def parseOptions():
    global options
    parser = argparse.ArgumentParser('ARM Random Test generator')
    parser.add_argument('-o', '--output', dest='output', default='tmp.s', 
                        help='output file name')
    parser.add_argument('-s', '--seed', default=None, dest='seed', type=int,
                        help='random init seed')
    parser.add_argument('--with-Thumb-2', action='store_true', default=True,
                        dest='thumb2', help='with Thumb-2 instructions')
    parser.add_argument('--with-ARMv7', action='store_true', default=True,
                        dest='armv7', help='with ARMv7 instructions')
    parser.add_argument('--with-VFP', action='store_true', default=True,
                        dest='vfp', help='with VFP instructions')
    parser.add_argument('--without-ARMv7', action='store_false', default=True,
                        dest='armv7', help='without ARMv7 instructions')
    parser.add_argument('--without-Thumb-2', action='store_false', default=True, 
                        dest='thumb2', help='without Thumb-2 instructions')
    parser.add_argument('--without-VFP', action='store_false', default=True,
                        dest='vfp', help='without VFP instructions')
    parser.add_argument('--enable-VFP-vector', action='store_true', default=False,
                        dest='vfp_vector', help='enable VFP Vector mode')
    parser.add_argument('--table-branch-max-index', default=4,
                        dest='tb_max', type=int, help='max table index for TBB/TBH')
    parser.add_argument('--table-branch-min-index', default=2,
                        dest='tb_min', type=int, help='min table index for TBB/TBH')
    parser.add_argument('-n', '--nest', default=4, dest='nest', type=int,
                        help='test case control flow nest level')
    parser.add_argument('-p', '--path', dest='ritpath', help='path to librit.so (real path)',
                        default=os.path.join(ROOT, '../bin/librit.so'))
    parser.add_argument('-m', '--min', type=int, dest='num', default=100,
                        help='minimal random instruction generated')
    parser.add_argument('--max-retry', type=int, dest='maxretry', default=8,
                        help='retry times when generate one instruction')
    parser.add_argument('--block-instr-limit', type=int, dest='instr_limit',
                        default=100, help='internal block instruction limit')
    parser.add_argument('--it-percent', type=int, dest='per_it', default=20,
                        help='percentage of it instruction in thumb block')
    parser.add_argument('--tb-percent', type=int, dest='per_tb', default=20,
                        help='percentage of tbb/tbh in thumb block')
    parser.add_argument('--with-excl-block', action='store_true', default=True, 
                        dest='excl_block', help='with exclusive instruction block')
    parser.add_argument('--without-excl-block', action='store_false', default=True, 
                        dest='excl_block', help='without exclusive instruction block')
    parser.add_argument('--with-aluwpc-block', action='store_true', default=True, 
                        dest='aluwpc_block', help='with ALUwritePC instruction block')
    parser.add_argument('--without-aluwpc-block', action='store_false', default=True, 
                        dest='aluwpc_block', help='without ALUwritePC instruction block')
    options = vars(parser.parse_args())

def test():
    pass


if __name__ == '__main__':
    test()
