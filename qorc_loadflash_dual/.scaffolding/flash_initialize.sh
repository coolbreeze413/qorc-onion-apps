#!/bin/bash


SCRIPT_DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )

# check if QORC_SDK_PATH is already setup (source envsetup.sh has already been invoked in the current shell)
# paranoid check - Makefile does this already.
if [ -z "$QORC_SDK_PATH" ] ; then
    printf "\nERROR: QORC_SDK_PATH is not set in the environment, is the QORC SDK initialized?\n\n"
    printf "    initialize with: 'source envsetup.sh' from the QORC SDK directory in the current shell\n"
    exit 1
fi


CONFIRM=

################################################################################
#   getopt based parsing
################################################################################
# option strings
SHORT="" # no short options at all, clearer that way.
# reference on how to enforce no short options: https://unix.stackexchange.com/questions/162624/how-to-use-getopt-in-bash-command-line-with-only-long-options
LONG="confirm:,port:,help"


# read the options
OPTS=$(getopt --options "$SHORT" --long "$LONG" --name "$0" -- "$@")

if [ $? != 0 ] ; then echo "ERROR: failed to parse options...exiting." >&2 ; exit 1 ; fi

eval set -- "$OPTS"

usage()
{
    printf "\n"
    printf "[%s] usage:\n" $(basename $0)
    printf "\n"
    printf "flash the initialize package of bootloader, bootfpga, m4app, appfpga\n"
    printf "\n"
    printf " syntax: $0 --port=serial-port\n"
    printf "\n"
    printf "example: $0 --port=/dev/ttyUSB0\n"
    printf "\n"
    printf "add --confirm=yes to override confirmation prompt."
    printf "\n"
}

# extract options and their arguments into variables
while true ; do
    case "$1" in
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

if [ -z "$PORT" ] ; then
    printf "\nERROR: PORT is not defined!\n"
    usage
    exit 1
fi


# confirmation print
printf "\n"
printf "PORT=$PORT\n"
printf "\n"

# --confirm not passed in: pester user (command-line usage)
if [ -z "$CONFIRM" ] ; then
    printf "\nCAUTION: This will replace the bootloader, bootfpga, m4app and appfpga on the board!\n\n"
    read -p "          Are you sure ? (no/yes): " CONFIRM
    CONFIRM=$(echo "$CONFIRM" | tr [:upper:] [:lower:])
    if [ ! "$CONFIRM" == "yes" ] ; then
        printf "\nAborting, as we did not get a yes!\n"
        exit 0
    fi
# --confirm=yes passed in: skip query, user knows what he is doing, or from vs code task
elif [ "$CONFIRM" == "yes" ] ; then
    printf "\nreceived --confirm=yes in the args, flashing initialize binaries...\n\n"
# unknown input, abort
else
    printf "\nnegative input received, aborting flash\n\n"
    exit 0
fi

################################################################################

PROJECT_DIR=$(cd "$SCRIPT_DIR/.." && pwd)
PROJECT_INITIALIZE_DIR="${PROJECT_DIR}/initialize"

# check if we have all the binaries in place
PROJECT_BOOTLOADER_BIN=
PROJECT_BOOTFPGA_BIN=
PROJECT_FPGA_BIN=
PROJECT_M4_BIN=

PROJECT_BOOTLOADER_BIN=$(ls "$PROJECT_INITIALIZE_DIR"/qorc_bootloader_dual.bin)
PROJECT_BOOTFPGA_BIN=$(ls "$PROJECT_INITIALIZE_DIR"/qorc_bootfpga.bin)
PROJECT_FPGA_BIN=$(ls "$PROJECT_INITIALIZE_DIR"/helloworldfpga.bin)
PROJECT_M4_BIN=$(ls "$PROJECT_INITIALIZE_DIR"/qorc_helloworldm4fpga.bin)

printf "\n"
printf "PROJECT_BOOTLOADER_BIN=$PROJECT_BOOTLOADER_BIN\n"
printf "PROJECT_BOOTFPGA_BIN=$PROJECT_BOOTFPGA_BIN\n"
printf "PROJECT_FPGA_BIN=$PROJECT_FPGA_BIN\n"
printf "PROJECT_M4_BIN=$PROJECT_M4_BIN\n"
printf "\n"

if [ ! -f "$PROJECT_BOOTLOADER_BIN" ] || [ ! -f "$PROJECT_BOOTFPGA_BIN" ] || [ ! -f "$PROJECT_FPGA_BIN" ] || [ ! -f "$PROJECT_M4_BIN" ] ; then
    printf "\nERROR: one of the initialize binaries does not exist! (is build done?)\n"
    exit 1
fi

printf "\nrunning command:\n"
printf "qfprog --port $PORT --bootloader $PROJECT_BOOTLOADER_BIN --bootfpga $PROJECT_BOOTFPGA_BIN --m4app $PROJECT_M4_BIN --appfpga $PROJECT_FPGA_BIN --mode fpga-m4 --reset\n\n"
qfprog --port "$PORT" --bootloader "$PROJECT_BOOTLOADER_BIN" --bootfpga "$PROJECT_BOOTFPGA_BIN" --m4app "$PROJECT_M4_BIN" --appfpga "$PROJECT_FPGA_BIN" --mode fpga-m4 --reset
