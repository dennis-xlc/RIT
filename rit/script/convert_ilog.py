#!/usr/bin/python

import argparse
import os

def parse_imm(str_bit, str_val):
    bit = int(str_bit)
    val = int(str_val)
    if bit == 3:
        if val >=2 and val <= 5:
            return "2,5"
    elif bit == 4:
        if val >=2 and val <= 0xd:
            return "2,13"
    elif bit == 5:
        if val >= 2 and val <= 0x1d:
            return "2,29"
    elif bit == 6:
        if val >= 2 and val <= 0x3d:
            return "2,61"
    elif bit == 7:
        if val >= 2 and val <= 0x7d:
            return "2,125"
    elif bit == 8:
        if val >= 2 and val <= 0xfd:
            return "2,253"
    elif bit == 10:
        if val >= 2 and val <= 0x3fd:
            return "2,1021"
    elif bit == 11:
        if val >= 2 and val <= 0x7fd:
            return "2,2045"
    elif bit == 12:
        if val >= 2 and val <= 0xffd:
            return "2,4093"
    elif bit == 24:
        if val >= 2 and val <= 0xfffffd:
            return "2,16777213"
    elif bit == 16:
        if val >= 2 and val <= 0xfffd:
            return "2,65533"
    elif bit == 17:
        if val >= 2 and val <= 0x1fffd:
            return "2,131069"
    elif bit == 21:
        if val >= 2 and val <= 0x1ffffd:
            return "2,2097149"
    return str_val

            
def parseCovLog(dst, ilog):

    fdst = open(dst, 'w+')
    cl_lines = open(ilog, 'r').readlines()
    for cl in cl_lines:
        print "processing line: %s..." %cl
        cl_subs = cl.split('\', \'')
        bit = cl_subs[14]
        val = cl_subs[15]
        print "bit=%s, val=%s" %(bit,val)
        new_val = 'NA'
        if bit != 'NA':
            cl_subs[15] = parse_imm(bit,val)
        newstr = '\', \''.join(cl_subs)
        newstr = newstr[:-1] + '       \n'
        cl_subs = newstr.split( ":")
        cl_var = cl_subs[0]
        cl_cnt = cl_subs[1]
        found = 0
        fdst.seek(0)
        last_pos = fdst.tell()
        while True:
           cd = fdst.readline()
           if not cd:
               break
           cd_subs = cd.split(':')
           cd_var = cd_subs[0]
           cd_cnt = cd_subs[1]

           if cmp(cl_var, cd_var) == 0:
                cd_subs[1] = str(int(cd_cnt) + int(cl_cnt))
                found = 1
                break
           last_pos = fdst.tell()
        if found == 1:
            newstr = ':'.join(cd_subs)
            fdst.seek(last_pos)

        print "\t%s" % newstr
        fdst.write(newstr)
        fdst.flush()
    fdst.close()
    return 0

def main():
    parser = argparse.ArgumentParser(description="process ISA coverage info with RIT coverage log")
    parser.add_argument('-i', '--ilog', dest='ilog', 
            help='file for instruction coverage log')
    parser.add_argument('-d', '--dest', dest='dst', default='./new.ilog',
            help='files being converted')
    args = parser.parse_args()
    parseCovLog(args.dst, args.ilog)

if __name__ == '__main__':
    main()
