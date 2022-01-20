#!/bin/bash


################################################################################
#   getopt based parsing
################################################################################
# option strings
SHORT="" # no short options at all, clearer that way.
# reference on how to enforce no short options: https://unix.stackexchange.com/questions/162624/how-to-use-getopt-in-bash-command-line-with-only-long-options
LONG="qorc-sdk-path:,port:,help"


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
    printf " syntax: $0 --qorc-sdk-path=/path/to/qorc/sdk --port=serial_port_id\n"
    printf "\n"
    printf "example: $0 --qorc-sdk-path=$HOME/qorc-sdk --port=/dev/ttyUSB0\n"
    printf "\n"
}

# extract options and their arguments into variables
while true ; do
    case "$1" in
        --qorc-sdk-path )
            QORC_SDK_PATH="$2"
            shift 2
        ;;
        --port )
            PORT="$2"
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

# arg checks
if [ -z "$QORC_SDK_PATH" ] ; then
    printf "\nWARNING: QORC_SDK_PATH is not defined!\n"
fi

if [ -z "$PORT" ] ; then
    printf "\nERROR: PORT is not defined!\n"
    usage
    exit 1
fi


# confirmation print
printf "\n"
printf "QORC_SDK_PATH=$QORC_SDK_PATH\n"
printf "\n"
################################################################################


# scripts are run in non-interactive mode in bash, so aliases are not expanded by default.
# we need to enable this as we want to use aliases *before* sourcing the script creating the alias
# https://unix.stackexchange.com/a/1498
shopt -s expand_aliases

# setup QORC_SDK environment
if [ ! -z "$QORC_SDK_PATH" ] ; then
    cd $QORC_SDK_PATH
    source envsetup.sh
    cd - > /dev/null
fi


# setup QORC_SDK debug environment (optional)
# if [ ! -z "$QORC_SDK_PATH" ] ; then
#     cd $QORC_SDK_PATH/qorc-onion-apps/qorc_utils
#     source debugenvsetup.sh
#     cd - > /dev/null
# fi



PROJECT_ROOT_DIR=$(cd .. ; printf %s "$PWD")
PROJECT_RTL_DIR="${PROJECT_ROOT_DIR}/fpga/rtl"

PROJECT_FPGA_DESIGN_BIN=$(ls "$PROJECT_RTL_DIR"/*.bin)

printf "flash using port [%s], design [%s] with mode [%s]\n\n" "$PORT" "$PROJECT_FPGA_DESIGN_BIN" "fpga"

# qfprog is a function(earlier alias) created in envsetup.sh
qfprog --port "$PORT" --appfpga "$PROJECT_FPGA_DESIGN_BIN" --mode fpga --reset

# without the alias in envsetup.sh available, we can also do:
#qfprog="python3 ${QORC_SDK_PATH}/TinyFPGA-Programmer-Application/tinyfpga-programmer-gui.py"
#$qfprog --port "$PORT" --appfpga "$PROJECT_FPGA_DESIGN_BIN" --mode fpga --reset