#!/usr/bin/python

import sys, random

import options
import processtest as pt
import randomblock as rb
import instrgen as ig
import code

def genTest(ctx, name=None, initblock=None, exitblock=None):
    test = code.Block()
    if not initblock:
        initblock = code.genInitBlock()
    test.code.append((code.CODETYPE.BLOCK, initblock))
    rtest = ctx['getRBlock']('rit', ctx['nest'])
    blocks = rtest.makeRandomBlock(ctx)
    #ig.genCodeBlocks(blocks, ctx)
    ig.genCodeBlocksWithLoadInsts(blocks, ctx)
    test.code.append((code.CODETYPE.BLOCK, rtest))
    if not exitblock:
        exitblock = code.genExitBlock()
    test.code.append((code.CODETYPE.BLOCK, exitblock))
    if name is None:
        name = "tmp.s"
    f = open(name, 'w')
    f.write(str(test))
    f.close() 

def buildTest(test):
    try:
        pt.processTest(test)
    except Exception, e:
        sys.exit("Build seed fail")

def main():
    options.parseOptions()
    #init random seed
    opt_seed = options.getOption('seed')
    if opt_seed:
        random.seed(opt_seed)
    else:
        random.seed()

    ctx = options.getInitContext(rb.getRBlock)
    tname = options.getOption('output')
    genTest(ctx, tname)
    buildTest(tname)
    

if __name__ == '__main__':
    main()
