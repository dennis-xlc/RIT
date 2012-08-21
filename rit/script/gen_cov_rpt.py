#!/usr/bin/python

import argparse
import os
import mmap

FNAME_ARM   = "arm_cross_item.def"
FNAME_THUMB = "thumb_cross_item.def"

FNAME_COV = "cross_item_cov.rpt"
FNAME_MISMATCH = "mismatch.log"
FNAME_MATCH = "match.log"


def genCovReport(covdef):
    item_covered = 0
    item_uncovered = 0
    group_arch = 0
    
    fcov = open(FNAME_COV, "w")

    for fname in FNAME_ARM, FNAME_THUMB:
        if fname == FNAME_ARM:
            group_arch = "ARM"
        else:
            group_arch = "THUMB"

        opkind_cnt = 0
        instr_cnt = 0
        group_opkind = {}
        group_instr = {}
        cov_opkind = {}
        cov_instr = {}
        cov_arch = 0
        def_instr = {}
        def_opkind = {}

        fcd = open(os.path.join(covdef, fname), 'r')
        cd_lines = fcd.readlines()
        for cd in cd_lines:
            str = cd.strip()
            if str[0].isdigit():
                cd_subs = str.split(':')
                count = int(cd_subs[2])
                if count == 0:
                    item_uncovered += 1
                else:
                    item_covered += 1
                    cov_opkind[opkind_cnt] += 1
                    cov_instr[instr_cnt-1] += 1
                    cov_arch += 1
            else:
                if str.endswith("{"):
                    subs = str.split(' ')
                    group_opkind[opkind_cnt] = subs[0]
                    cov_opkind[opkind_cnt] = 0
                    def_opkind[opkind_cnt] = 0
                    start_cnt = instr_cnt
                elif str.endswith("}") == True:
                    # finish collecting the percentage and coverage data for this opkind
                    
                    # print "\topkind %s: %d\n" %(group_opkind[opkind_cnt], cov_opkind[opkind_cnt])
                    for i in range(start_cnt, instr_cnt):
                        def_opkind[opkind_cnt] += def_instr[i]
                    opkind_cnt += 1
                else:
                    # finish collecting the data for the instruction
                    subs = str.split('-')
                    group_instr[instr_cnt] = subs[0]
                    subs = str.split(':')
                    def_instr[instr_cnt] = int(subs[1])
                    cov_instr[instr_cnt] = 0
                    instr_cnt += 1;
        fcd.close()

        fcov.write("group %s, opkind count: %d, instr count: %d\nOpkind coverage:\n" %(group_arch, opkind_cnt, instr_cnt))
        for i in range(opkind_cnt):
            if def_opkind[i] == 0:
                precent = 0
            else:
                precent = float(cov_opkind[i])/float(def_opkind[i])
            fcov.write("\topkind %s: %f%%(%d/%d)\n" %(group_opkind[i], precent, cov_opkind[i], def_opkind[i]))
            if cov_opkind[i] > def_opkind[i]:
                exit(-1)
        fcov.write("\nInstruction coverage:\n")
        for i in range(instr_cnt):
            if def_instr[i] == 0:
                precent = 0
            else:
                precent = float(cov_instr[i])/float(def_instr[i])
            fcov.write("\tinstr %s: %f%%(%d/%d)\n" %(group_instr[i], precent, cov_instr[i], def_instr[i]))
            if cov_instr[i] > def_instr[i]:
                exit(-1)

    fcov.write("total items: %d, uncovered: %d, covered: %d\n" %(item_uncovered+item_covered, item_uncovered, item_covered))
    fcov.close()


def parseCovLog(covdef, covlog):
    fmatch = open(FNAME_MATCH, "w")
    fmismatch = open(FNAME_MISMATCH, "w")

    with open(os.path.join(covdef, FNAME_ARM), 'r+') as f:
        map_arm = mmap.mmap(f.fileno(), 0)
    with open(os.path.join(covdef, FNAME_THUMB), 'r+') as f:
        map_thumb = mmap.mmap(f.fileno(), 0)

    cl_lines = open(covlog, 'r').readlines()
    for cl in cl_lines:
        print "processing line: %s..." %cl
        cl_subs = cl.split(':')
        cl_variant = cl_subs[0]
        cl_count = cl_subs[1]
        found = 0

        for fcd in map_arm, map_thumb:
        #fname in FNAME_ARM, FNAME_THUMB:
         #   fcd = open(os.path.join(covdef, fname), 'r+')
            fcd.seek(0)
            last_pos = fcd.tell()
            while True:
                cd = fcd.readline()
                if not cd:
                    break
                cd_str = cd.strip()
                if cd_str[0].isdigit():
                    cd_subs = cd.split(':')
                    sn = cd_subs[0]
                    variant = cd_subs[1]
                    count = cd_subs[2]

                    # if cl is matched with cd.subs[1]
                    # then increase the count
                    if variant.find(cl_variant) == 0:
                        updated_count = int(count)+int(cl_count)
                        fmatch.write("[MATCH] %s (current:%s, old:%s, new:%d)\n" %(cl, cl_count, count, updated_count))
                        print "[MATCH]!"
                        found = 1
                        fcd.seek(last_pos)
                        cd_subs[2] = str(updated_count)
                        newstr = ':'.join(cd_subs)
                        fcd.write(newstr)
                   #     fcd.flush()
                        break
                last_pos = fcd.tell()
            if found == 1:
                break
        # if not found the matched line in the file
        # report the failure and exit
        if found == 0:
            fmismatch.write("%s\n" %cl)
            print "[MISMATCH]!!!"

    map_arm.close()
    map_thumb.close()
    fmatch.close()
    fmismatch.close()
    return 0

def main():
    parser = argparse.ArgumentParser(description="process ISA coverage info with RIT coverage log")
    parser.add_argument('-l', '--covlog', dest='covlog', 
            help='file for coverage log')
    parser.add_argument('-d', '--covdef', dest='covdef', default='./',
            help='folder including coverage definition')
    args = parser.parse_args()
    parseCovLog(args.covdef, args.covlog)
    genCovReport(args.covdef)

if __name__ == '__main__':
    main()
