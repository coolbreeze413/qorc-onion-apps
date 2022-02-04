#!/bin/bash


SCRIPT_DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )

CONFIRM="undefined"

################################################################################
#   getopt based parsing
################################################################################
# option strings
SHORT="" # no short options at all, clearer that way.
# reference on how to enforce no short options: https://unix.stackexchange.com/questions/162624/how-to-use-getopt-in-bash-command-line-with-only-long-options
LONG="qorc-sdk-path:,confirm:,port:,help"


# read the options
OPTS=$(getopt --options "$SHORT" --long "$LONG" --name "$0" -- "$@")

if [ $? != 0 ] ; then echo "ERROR: failed to parse options...exiting." >&2 ; exit 1 ; fi

eval set -- "$OPTS"

usage()
{
    printf "\n"
    printf "[%s] usage:\n" $(basename $0)
    printf "\n"
    printf "flash the bootloader\n"
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
        --confirm )
            CONFIRM="$2"
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

# --confirm="no" or --confirm="yes" neither passed in
if [ "$CONFIRM" == "undefined" ] ; then
    printf "\nCAUTION: This will replace the bootloader on the board with this one!\n\n"
    read -p "          Are you sure ? (no/yes): " CONFIRM
    CONFIRM=$(echo "$CONFIRM" | tr [:upper:] [:lower:])
    if [ ! "$CONFIRM" == "yes" ] ; then
        printf "\nAborting, as we did not get a yes!\n"
        exit 0
    fi
elif [ "$CONFIRM" == "yes" ] ; then
    printf "\nreceived --confirm=yes in the args, flashing bootloader...\n\n"
else
    printf "\nnegative input received, aborting flash\n\n"
    exit 0
fi

exit 0

# confirmation print
printf "\n"
printf "QORC_SDK_PATH=$QORC_SDK_PATH\n"
printf "\n"
################################################################################

# setup QORC_SDK environment
if [ ! -z "$QORC_SDK_PATH" ] ; then
    cd $QORC_SDK_PATH
    source envsetup.sh
    cd - > /dev/null
fi


PROJECT_ROOT_DIR=$(cd "$SCRIPT_DIR/.." && pwd)

PROJECT_OUTPUT_BIN_DIR="${PROJECT_ROOT_DIR}/GCC_Project/output/bin"
PROJECT_M4_BIN=$(ls "$PROJECT_OUTPUT_BIN_DIR"/*.bin)

printf "flash using port [%s], m4 [%s], with mode [%s]\n\n" "$PORT" "$PROJECT_M4_BIN" "m4"

# qfprog is a function created in envsetup.sh
qfprog --port "$PORT" --bootloader "$PROJECT_M4_BIN" --reset
