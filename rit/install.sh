#!/bin/bash

SCRIPT=$(readlink -f $0)
RIT_DIR=`dirname $SCRIPT`
DEST_DIR=$HOME/Houdini-RIT
UBT_DIR=$RIT_DIR/../..
if [ $# -ge 1 ]
then
    DEST_DIR=$1
fi
RIT_DEST_DIR=$DEST_DIR/rit
UBT_DEST_DIR=$DEST_DIR/ubt

mkdir -p $RIT_DEST_DIR/bin
mkdir -p $RIT_DEST_DIR/script
mkdir -p $RIT_DEST_DIR/art
mkdir -p $UBT_DEST_DIR/cosim
#mkdir -p $UBT_DEST_DIR/cosim-qemu
mkdir -p $UBT_DEST_DIR/tia
#mkdir -p $UBT_DEST_DIR/cov

pushd $RIT_DIR/src > /dev/null
make
cp -v $RIT_DIR/bin/* $RIT_DEST_DIR/bin/
cp -v $RIT_DIR/script/* $RIT_DEST_DIR/script/
cp -v $RIT_DIR/art/* $RIT_DEST_DIR/art/
cp -v $RIT_DIR/run_rit.sh $DEST_DIR/
rsync -av --exclude=.svn $RIT_DIR/src/ $RIT_DEST_DIR/src
popd > /dev/null

#pushd $UBT_DIR > /dev/null
#rm -rf $UBT_DIR/obj_arm
#echo "build ubt (cosim and tia)"
#$UBT_DIR/src/scripts/build_arm.sh > /dev/null
#cp -v $UBT_DIR/obj_arm/ubt-ia/bt/cms.elf $UBT_DEST_DIR/cosim/
cp -v $UBT_DIR/src/scripts/ubt_config/ubt.config.always.TIA.cosim $UBT_DEST_DIR/cosim/ubt.config
#cp -v $UBT_DIR/obj_arm/ubt-ia/bt/cms.elf $UBT_DEST_DIR/tia/
cp -v $UBT_DIR/src/scripts/ubt_config/ubt.config.always.TIA $UBT_DEST_DIR/tia/ubt.config
#rm -rf $UBT_DIR/obj_arm
#echo "build ubt (cosim-qemu)"
#$UBT_DIR/src/scripts/build_ubt.so.sh > /dev/null
#cp -v $UBT_DIR/arm-test/cosim/cosim-qemu $UBT_DEST_DIR/cosim-qemu/
#cp -v $UBT_DIR/arm-test/cosim/ubt.so $UBT_DEST_DIR/cosim-qemu/
#cp -v $UBT_DIR/arm-test/cosim/ubt.config $UBT_DEST_DIR/cosim-qemu/
#rm -rf $UBT_DIR/obj_arm
#echo "build ubt (coverage)"
#$UBT_DIR/src/scripts/build_arm.sh CODE_COVERAGE=1 > /dev/null
#cp -v $UBT_DIR/obj_arm/ubt-ia/bt/cms.elf $UBT_DEST_DIR/cov/
#popd > /dev/null

