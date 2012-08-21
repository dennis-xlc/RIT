#!/bin/bash
mkdir -p build
cd build
../configure --static --target-list=arm-linux-user
make
