#!/bin/bash

SEED_NUM=20
GCC_ARM=arm-none-linux-gnueabi-gcc

SCRIPT=$(readlink -f $0)
SCRIPT_DIR=`dirname $SCRIPT`
RIT_ROOT=$(readlink -f $SCRIPT_DIR/../..)
RIT_DIRNAME=`basename $RIT_ROOT`
LOG=$RIT_ROOT/rit.genseeds.log
QEMU_ARM=qemu-arm
QEMU_COV=$RIT_ROOT/rit/bin/qemu-arm
DEST_DIR=$RIT_ROOT/tmp
FAIL_DIR=$RIT_ROOT/error

#=== FUNCTION ===========
#        NAME: print_help
# DESCRIPTION: Display usage information for this script.
#   PARAMETER: None
#========================
print_help()
{
    echo "Usage: "`basename $0`" [OPTIONS]"
    echo "--single-fsm            generate test case no-random"
}

#=== FUNCTION ==========
#        NAME: gen_seed
# DESCRIPTION: Call rit.py to generate seed
gen_seed()
{
    timestamp=`date +%m%d%H%M.%S%N | cut -b1-13`
    seedname=$HOSTNAME.$RIT_DIRNAME.$timestamp
    #generate seed source file
    python $RIT_ROOT/rit/art/art.py -o $RIT_ROOT/tmp_seed.s
    if [ $? -ne 0 ]
    then
        echo "gen seed failed @"`date` >> $LOG.rit
        if [ -f $RIT_ROOT/tmp_seed.s ]
        then
            mv $RIT_ROOT/tmp_seed.s $FAIL_DIR/$seedname.s
        fi
        return 1
    fi

    #run seed with qemu
    $QEMU_COV -singlestep -d in_asm $RIT_ROOT/tmp_seed.bin > /dev/null
    if [ $? -ne 0 ]
    then
        echo "$seedname qemu failed" >> $LOG
        return 3
    fi
    nm $RIT_ROOT/tmp_seed.bin > $RIT_ROOT/tmp.covinfo
    cat qemu.log >> $RIT_ROOT/tmp.covinfo
    python $RIT_ROOT/rit/script/rit-cov.py $RIT_ROOT/tmp.covinfo tmp.cov
    cp $RIT_ROOT/tmp_seed.bin $DEST_DIR/$seedname.bin
    return 0
}

mkdir -p $DEST_DIR
mkdir -p $FAIL_DIR

for i in `seq $SEED_NUM`
do
    gen_seed
done
python $RIT_ROOT/rit/script/process_seeds.py -l $RIT_ROOT/rit.log
echo "$SEED_NUM seeds generated @"`date` >> $LOG  
cd $RIT_ROOT
cat tmp.cov.log | sort | uniq >> rit-cov.log
cat tmp.cov.fsm.log | sort | uniq >> rit-cov.fsm.log
rm -f tmp.cov.log
rm -f tmp.cov.fsm.log


