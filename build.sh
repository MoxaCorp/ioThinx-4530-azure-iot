#!/bin/bash
# Copyright (C) 2019 Moxa Inc. All rights reserved.
# SPDX-License-Identifier: Apache-2.0

set -e
DIR_WRK=$(dirname $(readlink -e $0))

DST_SDK=$DIR_WRK/output/sdk_azure
export DIR_ENV=$DIR_WRK/output/env_azure

TOOLCHAIN_FILE=$DIR_WRK/config/toolchain-arm-linux.cmake
SYSROOT=$DIR_ENV/arm-linux-gnueabihf
BIN_SAMPLE=$DIR_WRK/sample/binary

function sdk_build()
{
    echo "-- Process: $FUNCNAME ..."
    rm -rf $BIN_SAMPLE
    mkdir -p $BIN_SAMPLE
    $DST_SDK/build_all/linux/build.sh --toolchain-file $TOOLCHAIN_FILE -cl --sysroot=$SYSROOT
    echo "-- Process: $FUNCNAME DONE!!"
}

sdk_build
