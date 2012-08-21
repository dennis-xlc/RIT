#!/bin/bash

SEED_NUM=100
GCC_ARM=arm-none-linux-gnueabi-gcc

SCRIPT=$(readlink -f $0)
SCRIPT_DIR=`dirname $SCRIPT`
RIT_ROOT=$(readlink -f $SCRIPT_DIR/../..)
RIT_DIRNAME=`basename $RIT_ROOT`
LOG=$RIT_ROOT/rit.genseeds.log
ECPLOG=$RIT_ROOT/ritecp.log
#QEMU_ARM=$RIT_ROOT/rit/bin/qemu-arm
QEMU_ARM=qemu-arm
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
    python $RIT_ROOT/rit/python/runrit.py

    mv $RIT_ROOT/*.bin $DEST_DIR/
    rm $RIT_ROOT/*.s

    #run seed with qemu
    #$QEMU_ARM -singlestep -d in_asm $RIT_ROOT/tmp_seed.bin > /dev/null
    for f in `ls $DEST_DIR`
    do
        $QEMU_ARM $DEST_DIR/$f > /dev/null
        if [ $? -ne 0 ]
        then
            echo "$seedname qemu failed" >> $LOG
            mv $DEST_DIR/$f $FAIL_DIR/
            return 3
        fi
    done
    return 0
}

mkdir -p $DEST_DIR
mkdir -p $FAIL_DIR

cd $RIT_ROOT
gen_seed
python $RIT_ROOT/rit/script/process_seeds.py -l $RIT_ROOT/rit.log
#cat ritecp.tmp.log >> $ECPLOG | sort | uniq > tmp.log
#mv tmp.log $ECPLOG
#rm -f ritecp.tmp.log
#rm -f tmp.log
echo "$SEED_NUM seeds generated @"`date` >> $LOG  

