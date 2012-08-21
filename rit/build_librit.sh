#!/bin/bash - 
#===============================================================================
#
#          FILE:  build_librit.sh
# 
#         USAGE:  ./build_librit.sh 
# 
#   DESCRIPTION:  
# 
#       OPTIONS:  ---
#  REQUIREMENTS:  ---
#          BUGS:  ---
#         NOTES:  ---
#        AUTHOR:  (), 
#       COMPANY: 
#       CREATED: 08/20/2012 01:48:46 PM CST
#      REVISION:  ---
#===============================================================================

set -o nounset                              # Treat unset variables as an error

#!/bin/bash
#########################################################################
# Author: gnuhpc(http://blog.csdn.net/gnuhpc)
# Created Time: Mon 20 Aug 2012 01:48:46 PM CST
# File Name: build_librit.sh
# Description: 
#########################################################################


#!/bin/bash

SCRIPT=$(readlink -f $0)
S_DIR=`dirname $SCRIPT`

#build librit.so
pushd $S_DIR/src
make clean
make librit.so
if [ $? -ne 0 ]
then
	echo "build librit.so failed"
	exit 1
fi
cp librit.so $S_DIR/bin/
popd > /dev/null
