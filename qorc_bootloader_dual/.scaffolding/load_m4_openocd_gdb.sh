#!/bin/bash


# m4 load:
# load m4 'images' -> m4 binary generated from gcc

# run openocd in background, run gdb and send appropriate sequence of commands.

# REQ:
# 1. OpenOCD probe is connected to EOS_S3 and it is in DEBUG mode
# 2. openocd is available in the path ('source debugenvsetup.sh')

SCRIPT_DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )

OPENOCD_PATH=$(which openocd)
OPENOCD_INTERFACE_CFG="jlink_swd.cfg"
GDB_PATH=$(which arm-none-eabi-gdb)
################################################################################
#   getopt based parsing
################################################################################
# option strings
SHORT="" # no short options at all, clearer that way.
# reference on how to enforce no short options: https://unix.stackexchange.com/questions/162624/how-to-use-getopt-in-bash-command-line-with-only-long-options
LONG="openocd-path:,openocd-if-cfg:,gdb-path:,help"


# read the options
OPTS=$(getopt --options "$SHORT" --long "$LONG" --name "$0" -- "$@")

if [ $? != 0 ] ; then echo "ERROR: failed to parse options...exiting." >&2 ; exit 1 ; fi

eval set -- "$OPTS"

usage()
{
    printf "\n"
    printf "[%s] usage:\n" $(basename $0)
    printf "\n"
    printf "build the fpga design\n"
    printf "\n"
    printf " syntax: $0 --openocd-path=/path/to/openocd --openocd-if-cfg=/path/to/probe/cfg --gdb-path=/path/to/arm-none-eabi-gdb\n"
    printf "\n"
    printf "example: $0 --openocd-path=/usr/bin/openocd --openocd-if-cfg=interface/ft2232h_swd.cfg --gdb-path=/usr/bin/arm-none-eabi-gdb\n"
    printf "\n"
}

# extract options and their arguments into variables
while true ; do
    case "$1" in
        --openocd-path )
            OPENOCD_PATH="$2"
            shift 2
        ;;
        --openocd-if-cfg )
            OPENOCD_INTERFACE_CFG="$2"
            shift 2
        ;;
        --gdb-path )
            GDB_PATH="$2"
            shift 2
        ;;
        -- )
            shift
            break
        ;;
        -h | --help | *)
            usage
            exit 0
        ;;
    esac
done
################################################################################

# arg checks
if [ -z "$OPENOCD_PATH" ] ; then
    printf "\nERROR: OPENOCD_PATH is not defined!\n"
    usage
    exit 1
fi

if [ -z "$OPENOCD_INTERFACE_CFG" ] ; then
    printf "\nERROR: OPENOCD_INTERFACE_CFG is not defined!\n"
    usage
    exit 1
fi

if [ -z "$GDB_PATH" ] ; then
    printf "\nERROR: GDB_PATH is not defined!\n"
    usage
    exit 1
fi


# confirmation print
printf "\n"
printf "OPENOCD_PATH=$OPENOCD_PATH\n"
printf "OPENOCD_INTERFACE_CFG=$OPENOCD_INTERFACE_CFG\n"
printf "GDB_PATH=$GDB_PATH\n"
printf "\n"


PROJECT_ROOT_DIR=$(cd "$SCRIPT_DIR/.." && pwd)

PROJECT_OUTPUT_BIN_DIR="${PROJECT_ROOT_DIR}/GCC_Project/output/bin"
PROJECT_M4_BIN=$(ls "$PROJECT_OUTPUT_BIN_DIR"/*.bin)

if [ ! -f "$PROJECT_M4_BIN" ] ; then
    printf "\nERROR: m4 binary does not exist!\n"
    exit 1
fi


CUSTOM_OPENOCD_GDB_LOG="m4_load_custom_openocd_gdb.log"

# run openocd (background, suppress output)
"$OPENOCD_PATH" -f "$OPENOCD_INTERFACE_CFG" -f target/eos_s3.cfg > "$CUSTOM_OPENOCD_GDB_LOG" 2>&1 &

# wait a second for the openocd server to startup
sleep 1

# ref: https://stackoverflow.com/a/58137098
# ref: https://stackoverflow.com/a/46867839
# run gdb, load m4, load fpga, wait for user commands.
# use -q to suppress banner: https://stackoverflow.com/a/61473013, but still enter the gdb shell after that.
# use -batch to fully automate the steps and close gdb automatically
"$GDB_PATH" \
        -q \
        -ex "target extended-remote localhost:3333" \
        -ex "monitor init" \
        -ex "monitor reset halt" \
        -ex "monitor load_image $PROJECT_M4_BIN 0x0" \
        -ex "monitor reset halt" \
        -ex "monitor resume" \
        -ex "monitor mdw 0x40005484"


# remove the custom script/log (disable for debugging the script)
rm "$CUSTOM_OPENOCD_GDB_LOG"
