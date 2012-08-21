#!/bin/bash

SCRIPTPATH=$(readlink -f $0)
S_DIR=`dirname $SCRIPTPATH`

CUR_DATE=`date +"%m%d%H%M"`
TOTAL=4000

gen_seed()
{
    idx=$1
    python $S_DIR/gen_vfp.py > tmp.s
    if [ $? -ne 0 ]
    then
        echo "gen seed fail"
        return 1
    fi

    python $S_DIR/build_vfp.py tmp.s
    if [ $? -ne 0 ]
    then
        echo "build seed fail"
        mv tmp.s ../fails/$CUR_DATE.$idx.s
        return 2
    fi

    qemu-arm tmp.bin > tmp.out
    if [ $? -ne 0 ]
    then
        echo "run seed with qemu fail"
        return 3
    fi

    instr_eip=`nm tmp.bin | grep rit_instr | awk '{print $1}' | sed 's/^0*//g'`
    instr_line=`arm-none-linux-gnueabi-objdump -d tmp.bin | grep "$instr_eip:"`
    raw_instr=`echo $instr_line | awk '{print $2}'`
    opcode=`echo $instr_line | awk '{print $3}'`
    seedname=$opcode.$raw_instr
    mv tmp.s ../src/$seedname.s
    mv tmp.bin ../seeds/$seedname
    mv tmp.out ../output/$seedname.out
}

mkdir -p tmp
mkdir -p seeds
mkdir -p src
mkdir -p output
mkdir -p fails

cd tmp
for i in `seq $TOTAL`
do
    gen_seed $i
done
