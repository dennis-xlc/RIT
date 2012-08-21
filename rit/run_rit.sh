#!/bin/bash

SCRIPT=$(readlink -f $0)
S_DIR=`dirname $SCRIPT`
D_FAIL=fail_seeds
D_LOG=$S_DIR/log
CLOG=$D_LOG/rit.cov
TMP_CLOG=$D_LOG/tmp
IS_LOG_COV=1

if [ "$1" == "--test" ]
then
    RUN_CNT=2
    LOG_CNT=2
else
    RUN_CNT=1000
    LOG_CNT=10
fi

if [ "x$THD_NUM" == "x" ]
then
    THD_NUM=`/usr/bin/getconf _NPROCESSORS_ONLN`
    if [ $THD_NUM -gt 4 ]
    then
        THD_NUM=`expr $THD_NUM - 2`
    else
        THD_NUM=2
    fi
fi

#add toolchain to PATH
if [ -d $S_DIR/local/Sourcery_G++_Lite/bin ]
then
    export PATH="$PATH:$S_DIR/local/Sourcery_G++_Lite/bin"
fi
if [ -d $S_DIR/local/bin ]
then
    export PATH="$PATH:$S_DIR/local/bin/"
fi

#build librit.so
pushd $S_DIR/rit/src
make clean
make librit.so
if [ $? -ne 0 ]
then
    echo "build librit.so failed"
    exit 1
fi
cp librit.so $S_DIR/rit/bin/
popd > /dev/null

#init running env
for i in `seq $THD_NUM`
do
    mkdir -p $S_DIR/tmp/$i/tmp
    cp -r ubt $S_DIR/tmp/$i/
    cp -r rit $S_DIR/tmp/$i/
done
mkdir -p $D_FAIL
mkdir -p $D_LOG
touch $TMP_CLOG.log 
touch $TMP_CLOG.fsm.log

cnt=1
for t in `seq $RUN_CNT`
do
    for n in `seq $LOG_CNT`
    do
        for i in `seq $THD_NUM`
        do
            if [ -f $S_DIR/tmp/$i/rit.log ]
            then
                rm -f $S_DIR/tmp/$i/rit.log
            fi
            cd $S_DIR/tmp/$i
            $S_DIR/tmp/$i/rit/script/gen_seq_seeds.sh &
        done
        wait
        for i in `seq $THD_NUM`
        do
            cat $S_DIR/tmp/$i/rit.log >> $S_DIR/rit.log
        done
    done
    
    if [ $IS_LOG_COV -ne 0 ]
    then
        cat $TMP_CLOG.log $S_DIR/tmp/*/rit-cov.log | sort | uniq > $CLOG.log
        cat $TMP_CLOG.fsm.log $S_DIR/tmp/*/rit-cov.fsm.log | sort | uniq > $CLOG.fsm.log
        cp $CLOG.log $TMP_CLOG.log
        cp $CLOG.fsm.log $TMP_CLOG.fsm.log
        rm -f $S_DIR/tmp/*/rit-cov.log
        rm -f $S_DIR/tmp/*/rit-cov.fsm.log
    fi
    
    mv $S_DIR/tmp/*/$D_FAIL/* $S_DIR/$D_FAIL/
    cnt=`expr $cnt + 1`
done

