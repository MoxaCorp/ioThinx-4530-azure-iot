#!/bin/bash
# Copyright (C) 2019 Moxa Inc. All rights reserved.
# SPDX-License-Identifier: Apache-2.0

set -e
DIR_WRK=$(dirname $(readlink -e $0))
SCRIPT=$(basename $0)

GIT_SDK=https://github.com/Azure/azure-iot-sdk-c.git
VER_SDK=2018-10-03
SRC_TOOLCHAIN=/usr/local/arm-linux-gnueabihf

SRC_ROOTFS=$DIR_WRK/rootfs
DST_SDK=$DIR_WRK/output/sdk_azure
DST_TOOLCHAIN=$DIR_WRK/output/env_azure
DST_ROOTFS=$DST_TOOLCHAIN/arm-linux-gnueabihf

CMAKE_SAMPLE=$DST_SDK/serializer/samples/CMakeLists.txt
SRC_SAMPLE=$DIR_WRK/sample/source
BIN_SAMPLE=$DIR_WRK/sample/binary

function sdk_update()
{
    echo "-- Process: $FUNCNAME ..."

    pushd $DST_SDK > /dev/null
    git checkout -q $VER_SDK
    git submodule update --init --recursive
    popd > /dev/null

    echo "if(\${use_mqtt} AND (NOT DEFINED use_mqtt_kit OR NOT \${use_mqtt_kit}))" >> $CMAKE_SAMPLE
    echo "   add_subdirectory($SRC_SAMPLE $BIN_SAMPLE)" >> $CMAKE_SAMPLE
    echo "endif()" >> $CMAKE_SAMPLE

    echo "-- Process: $FUNCNAME DONE!!"
}

function sdk_clone()
{
    echo "-- Process: $FUNCNAME ..."
    rm -rf $DST_SDK
    mkdir -p $DST_SDK
    git clone --recursive $GIT_SDK $DST_SDK
    echo "-- Process: $FUNCNAME DONE!!"
}

function sdk_env()
{
    echo "-- Process: $FUNCNAME ..."
    rm -rf $DST_TOOLCHAIN $DST_ROOTFS
    mkdir -p $DST_TOOLCHAIN $DST_ROOTFS
    rsync -rvhL $SRC_TOOLCHAIN/* $DST_TOOLCHAIN
    rsync -rvhl $SRC_ROOTFS/* $DST_ROOTFS
    echo "-- Process: $FUNCNAME DONE!!"
}

function sdk_arg()
{
    function usage()
    {
        echo
        echo "Usage: ./$SCRIPT [options]"
        echo
        echo "Options:"
        echo -e "\t-git\t\tGit repository of SDK."
        echo -e "\t\t\tDefault: $GIT_SDK"
        echo
        echo -e "\t-ver\t\tVersion of SDK."
        echo -e "\t\t\tDefault: $VER_SDK"
        echo
        echo -e "\t--toolchain\tGNU cross-toolchain directory."
        echo -e "\t\t\tDefault: $SRC_TOOLCHAIN"
        echo
        echo -e "\t--help\t\tDisplay this help and exit."
        echo
        echo "Examples:"
        echo -e "\tDefault\t\t./$SCRIPT"
        echo -e "\tSpecify\t\t./$SCRIPT -git $GIT_SDK -ver $VER_SDK"
        echo -e "\t\t\t./$SCRIPT --toolchain $SRC_TOOLCHAIN"
        echo
        exit
    }

    arg_id=0
    for arg in "$@"
    do
        case $arg_id in
            "0")
                case $arg in
                    "-git") arg_id=1;;
                    "-ver") arg_id=2;;
                    "--toolchain") arg_id=3;;
                    "--help" | *) usage;;
                esac
            ;;
            "1")
                arg_id=0
                GIT_SDK=$arg
            ;;
            "2")
                arg_id=0
                VER_SDK=$arg
            ;;
            "3")
                arg_id=0
                SRC_TOOLCHAIN=$arg
            ;;
        esac
    done
}

function main()
{
    sdk_arg "$@"
    sdk_env
    sdk_clone
    sdk_update
}

main "$@"
