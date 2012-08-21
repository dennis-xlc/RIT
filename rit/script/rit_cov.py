#!/usr/bin/python

import sqlite3 as db
import argparse
import os

DBNAME      = r"rit_cov.db"
FNAME       = "rit.ilog"
T_ISA       = r"isa_crossitem"
T_DIST      = r"distribution"

STR_NA      = r"NA"
K_ADDR      = r"inst_addr"

S_REG_BASE  = 64
D_REG_BASE  = 128
Q_REG_BASE  = 256

class TBEntry(object):
    ISA         = "isa"
    OPKIND      = "opkind"
    OP          = "opcode"
    SBIT        = "sbit"
    COND        = "cond_code"
    D1          = "dst_reg1"
    D2          = "dst_reg2"
    S1          = "src_reg1"
    S2          = "src_reg2"
    S3          = "src_reg3"
    S4          = "src_reg4"
    SHIFT_TYPE  = "shift_type"
    SHIFT_IMM   = "shift_imm"
    SHIFT_REG   = "shift_reg"
    IMM_BIT     = "imm_bit"
    IMM_VAL     = "imm_val"
    ROTATE_REG  = "rotate_reg"
    VEC_STRIDE  = "vec_stride"
    VEC_LEN     = "vec_len"
    PUW         = "puw"
    ROUND       = "round"
    FPR_VAL1     = "fpr_val1"
    FPR_VAL2     = "fpr_val2"
    FPR_VAL3     = "fpr_val3"
    FPR_VAL4     = "fpr_val4"
    MISC         = "misc"


ISA_TB = [TBEntry.ISA,
          TBEntry.OPKIND,
          TBEntry.OP,
          TBEntry.SBIT,
          TBEntry.COND,
          TBEntry.D1,
          TBEntry.D2,
          TBEntry.S1,
          TBEntry.S2,
          TBEntry.S3,
          TBEntry.S4,
          TBEntry.SHIFT_TYPE,
          TBEntry.SHIFT_IMM,
          TBEntry.SHIFT_REG,
          TBEntry.IMM_BIT,
          TBEntry.IMM_VAL,
          TBEntry.ROTATE_REG,
          TBEntry.VEC_STRIDE,
          TBEntry.VEC_LEN,
          TBEntry.PUW,
          TBEntry.ROUND,
          TBEntry.FPR_VAL1,
          TBEntry.FPR_VAL2,
          TBEntry.FPR_VAL3,
          TBEntry.FPR_VAL4,
          TBEntry.MISC]


def parse_reg(input):
    if input == '-':
        return STR_NA
    else:
        r = int(input)
        if r >= 0 and r < 16:
            if r <= 12:
                if r > 7:
                    return "8,12"
                elif r > 3:
                    return "4,7"
                else:
                    return "0,3"
            else:
                return "%d" % r
        elif r >= Q_REG_BASE:
            qr = r - Q_REG_BASE
            if qr <= 7:
                return "0,7"
            elif qr <= 15:
                return "8,15"
            else:
                print "[WARNING] invalid Quadword register"
                return STR_NA
        elif r >= D_REG_BASE:
            dr = r - D_REG_BASE
            if dr <= 15:
                return "0,15"
            elif dr <= 31:
                return "16,31"
            else:
                print "[WARNING] invalid Doubleword register"
                return STR_NA
        elif r >= S_REG_BASE:
            sr = r - S_REG_BASE
            if sr % 2 == 0:
                return "EVEN"
            elif sr % 2 != 0:
                return "ODD"
            else:
                print "[WARNING] invalid Singleword register"
                return STR_NA
            return "S"
        else:
            print "[WARNING] invalid register %s" % input
            return STR_NA

def parse_shift_type(input):
    return input

def parse_shift_imm(input):
    if input == '-':
        return STR_NA
    else:
        v = int(input, 16)
        if v > 1 and v < 30:
            return "2,29"
        elif v > 31 or v < 0:
            print "[WARNING] shift_imm %s out of range" % input
            return STR_NA
        else:
            return str(v)

def parse_shift_reg(input):
    if input == '-':
        return STR_NA
    else:
        v = int(input, 16)
        if v < 34:
            if v > 1 and v < 30:
                return "2,29"
            else:
                return str(v)
        elif v < 257:
            if v > 253:
                return str(v)
            else:
                return "34,253"
        else:
            if v == 0xfffffffe:
                return "4294967294"
            elif v == 0xffffffff:
                return "4294967295"
            else:
                return "257,4294967293"


def parse_single_vfp_val(input):
    v = int(input,16)
    if v == 0:
        return "0"
    elif v >= 1 and v <= 0x7fffff:
        return "1,8388607"
    elif v >= 0x00800000 and v <= 0x7f7fffff:
        return "8388608,2139095039"
    elif v == 0x7f800000:
        return "2139095040"
    elif v >= 0x7f800001 and v <= 0x7fffffff:
        return "2139095041,2147483647"
    elif v == 0x80000000:
        return "2147483648"
    elif v >= 0x80000001 and v <= 0x807fffff:
        return "2147483649,2155872255"
    elif v >= 0x80800000 and v <= 0xff7fffff:
        return "2155872256,4286578687"
    elif v == 0xff800000:
        return "4286578688"
    else:
        return "4286578689,4294967295"

def parse_double_vfp_val(input):
    v = int(input,16)
    if v == 0:
        return "0"
    elif v >= 1 and v <= 0x000fffffffffffff:
        return "1,4503599627370495"
    elif v >= 0x0010000000000000 and v <= 0x7fefffffffffffff:
        return "4503599627370496,9218868437227405311"
    elif v == 0x7ff0000000000000:
        return "9218868437227405312"
    elif v >= 0x7ff0000000000001 and v <= 0x7fffffffffffffff:
        return "9218868437227405313,9223372036854775807"
    elif v == 0x8000000000000000:
        return "9223372036854775808"
    elif v >= 0x8000000000000001 and v <= 0x800fffffffffffff:
        return "9223372036854775809,9227875636482146303"
    elif v >= 0x8010000000000000 and v <= 0xffefffffffffffff:
        return "9227875636482146304,18442240474082181119"
    elif v == 0xfff0000000000000:
        return "18442240474082181120"
    else:
        return "18442240474082181121,18446744073709551615"

def parse_vfp_val(type, input):
    if input == '-':
        return STR_NA
    else:
        if type == 's':
            print "%s, %s" %(type,input)
            return parse_single_vfp_val(input)
        elif type == 'd':
            return parse_double_vfp_val(input)
        else:
            #TODO
            return STR_NA

def parse_imm(str_bit, str_val):
    bit = int(str_bit)
    val = int(str_val)
    if val == '-':
        return STR_NA
    else:
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

            

def parseLine(line):
    item = {}
    for i in ISA_TB:
        item[i] = STR_NA
    subs = line.split('###')
    if not len(subs) is 3:
        return item
    instinfo = subs[0]
    hexcode = subs[1]
    disinst = subs[2]

    #process instinfo
    for substr in instinfo.strip().split():
        idx = substr.find('=')
        if idx < 0:
            continue
        k = substr[:idx]
        v = substr[idx+2:-1]
        if k == 'isa':
            if v == '0':
                item[TBEntry.ISA] = 'A'
            elif v == '1':
                item[TBEntry.ISA] = 'T'
            else:
                print "[WARNING] unknown val %s for isa" % val
        elif k == 'opk':
            item[TBEntry.OPKIND] = v
        elif k == 'src':
            tmps = v.split(',')
            if len(tmps) is 4:
                item[TBEntry.S1] = parse_reg(tmps[0])
                item[TBEntry.S2] = parse_reg(tmps[1])
                item[TBEntry.S3] = parse_reg(tmps[2])
                item[TBEntry.S4] = parse_reg(tmps[3])
            else:
                print v
        elif k == 'dst':
            tmps = v.split(',')
            if len(tmps) is 2:
                item[TBEntry.D1] = parse_reg(tmps[0])
                item[TBEntry.D2] = parse_reg(tmps[1])
            else:
                print v
        elif k == 'shift':
            tmps = v.split(',')
            if len(tmps) is 3:
                if tmps[0] != '-':
                    item[TBEntry.SHIFT_TYPE] = parse_shift_type(tmps[0])
                if tmps[1] != '-':
                    item[TBEntry.SHIFT_IMM] = parse_shift_imm(tmps[1])
                if tmps[2] != '-':
                    item[TBEntry.SHIFT_REG] = parse_shift_reg(tmps[2])
            else:
                print v
        elif k == 'c':
            if v == '0':
                item[TBEntry.COND] = 'AL'
            elif v == '1':
                item[TBEntry.COND] = 'NAL'
            else:
                print "[WARNING] unknown val %s for cond"
        elif k == 's':
            if v == '0':
                item[TBEntry.SBIT] = '0'
            elif v == '1':
                item[TBEntry.SBIT] = '1'
        elif k == 'imm':
            tmps = v.split(',')
            print "imm: %s,%s" %(tmps[0],tmps[1])
            if len(tmps) is 2:
                if tmps[0] != '-':
                    item[TBEntry.IMM_BIT] = tmps[0]
                    item[TBEntry.IMM_VAL] = parse_imm(tmps[0], tmps[1])
            else:
                print v
        elif k == 'rotate_reg':
            if v != '-':
                item[TBEntry.ROTATE_REG] = v
        elif k == 'vfp':
            tmps = v.split(',')
            if len(tmps) is 4:
                if tmps[0] != '-':
                    item[TBEntry.VEC_STRIDE] = tmps[0]
                if tmps[1] != '-':
                    item[TBEntry.VEC_LEN] = tmps[1]
                if tmps[2] != '-':
                    item[TBEntry.PUW] = tmps[2]
                if tmps[3] != '-':
                    item[TBEntry.ROUND] = tmps[3]
        elif k == 'vfp_val':
            tmps = v.split(",")
            if len(tmps) is 4:
                if tmps[0] != '-':
                    subtmps = tmps[0].split("#")
                    item[TBEntry.FPR_VAL1] = parse_vfp_val(subtmps[0], subtmps[1])
                if tmps[1] != '-':
                    subtmps = tmps[1].split("#")
                    item[TBEntry.FPR_VAL2] = parse_vfp_val(subtmps[0], subtmps[1])
                if tmps[2] != '-':
                    subtmps = tmps[2].split("#")
                    item[TBEntry.FPR_VAL3] = parse_vfp_val(subtmps[0], subtmps[1])
                if tmps[3] != '-':
                    subtmps = tmps[3].split("#")
                    item[TBEntry.FPR_VAL4] = parse_vfp_val(subtmps[0], subtmps[1])
        else:
            print "[WANRING] unknown key %s" % k

    sub_disinsts = disinst.strip().split()
    op = sub_disinsts[0].split('.')
    if item[TBEntry.SBIT] == '1' and op[0][-1] == 's':
        item[TBEntry.OP] = op[0][:-1]
    else:
        item[TBEntry.OP] = op[0]

    sub_hexcodes = hexcode.strip().split(':')
    inst_addr = int(sub_hexcodes[0], 16)
    item[K_ADDR] = inst_addr
    return item

def createDB(con):
    create_sql = "CREATE TABLE IF NOT EXISTS %s (%s)"
    id_str = 'id INTEGER PRIMARY KEY'
    cur = con.cursor()
    tb_items = id_str + ', ' + ', '.join([i+' TEXT' for i in ISA_TB])
    cur.execute(create_sql % (T_ISA, tb_items))
    con.commit()
    cur.execute(create_sql % (T_DIST, id_str + ', count INTEGER'))
    con.commit()

def insertItem(con, instr, count):
    select_item_sql = "SELECT id FROM %s WHERE %s" % (T_ISA, 
                                                      ' and '.join([i+'=:'+i for i in ISA_TB]))
    insert_item_sql = "INSERT INTO %s (%s) VALUES (%s)" % (T_ISA, 
                                                           ', '.join(ISA_TB), 
                                                           ', '.join([':'+i for i in ISA_TB]))
    init_cnt_sql = "INSERT INTO %s (id, count) VALUES (?, ?)" % T_DIST
    update_cnt_sql = "UPDATE %s SET count=count+? WHERE id=?" % T_DIST
    cur = con.cursor()
    cur.execute(select_item_sql, instr)
    row = cur.fetchone()
    if row:
        #update counter for existed item
        id = row[0]
        cur.execute(update_cnt_sql, (count, str(id)))
    else:
        #add new item to table
        cur.execute(insert_item_sql, instr)
        id = cur.lastrowid
        cur.execute(init_cnt_sql, (str(id), count))
    con.commit()

def logInstInfo(dst, files, to_db=False):
    all_items = {}
    for f in files:
        print "[INFO] Processing %s..." % f
        all_addrs = set()
        lines = open(f, 'r').readlines()
        for l in lines:
            if l.startswith("INST:"):
                item = parseLine(l)
                l_item = tuple([item[i] for i in ISA_TB])
                addr = item[K_ADDR]
                if l_item in all_items:
                    if addr not in all_addrs:
                        all_items[l_item] += 1
                        all_addrs.add(addr)
                else:
                    all_items[l_item] = 1
                    all_addrs.add(addr)
    saveItemsToFile(os.path.join(dst, FNAME), all_items)
    if to_db:
        saveItemsToDB(os.path.join(dst, DBNAME), all_items)

def saveItemsToDB(db_name, items):
    print "[INFO] Saving to db %s..." % db_name
    con = db.connect(db_name)
    createDB(con)
    tsize = len(ISA_TB)
    item_dict = {}
    for k in items.keys():
        for i in range(tsize):
            item_dict[ISA_TB[i]] = k[i]
        insertItem(con, item_dict, items[k])
    con.close()

def saveItemsToFile(file_name, items):
    print "[INFO] Saving to file %s..." % file_name
    f = open(file_name, 'a')
    for k, v in items.items():
        f.write("%s: %d\n" % (k, v))
    f.close()

def mergeLogs(dst, files, to_db=False):
    all_items = {}
    for f in files:
        print "[INFO] Processing %s..." % f
        #lines = open(f, 'r').readlines()
        content = open(f, 'r')
        for l in content:
            subs = l.split(':')
            key = eval(subs[0])
            count = eval(subs[1])
            if key in all_items:
                all_items[key] += count
            else:
                all_items[key] = count
    saveItemsToFile(os.path.join(dst, FNAME), all_items)
    if to_db:
        saveItemsToDB(os.path.join(dst, DBNAME), all_items)

def main():
    parser = argparse.ArgumentParser(description="process rit coverage log")
    parser.add_argument('files', nargs='+', 
                        help='input raw coverage logs')
    parser.add_argument('-d', '--dst', dest='dst', default='./',  
                        help='folder to save coverage info')
    parser.add_argument('-m', '--merge', action='store_true', default=False,
                        dest='merge_log', help='merge input *.ilog files')
    args = parser.parse_args()
    if args.merge_log:
        mergeLogs(args.dst, args.files)
    else:
        logInstInfo(args.dst, args.files)

def test():
    con = db.connect(DBNAME)
    createDB(con)
    instr = parseLine("")
    for i in range(2):
        insertItem(con,instr)
    instr[TBEntry.OPKIND] = "ALU_OP"
    for i in range(2):
        insertItem(con, instr, 10)
    con.close()


if __name__ == '__main__':
    main()
