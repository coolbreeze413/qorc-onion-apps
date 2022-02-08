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
    printf "flash the bootloader\n"
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
    printf "\nCAUTION: This will replace the bootloader on the board with this one!\n\n"
    read -p "          Are you sure ? (no/yes): " CONFIRM
    CONFIRM=$(echo "$CONFIRM" | tr [:upper:] [:lower:])
    if [ ! "$CONFIRM" == "yes" ] ; then
        printf "\nAborting, as we did not get a yes!\n"
        exit 0
    fi
# --confirm=yes passed in: skip query, user knows what he is doing, or from vs code task
elif [ "$CONFIRM" == "yes" ] ; then
    printf "\nreceived --confirm=yes in the args, flashing bootloader...\n\n"
# unknown input, abort
else
    printf "\nnegative input received, aborting flash\n\n"
    exit 0
fi

################################################################################

PROJECT_DIR=$(cd "$SCRIPT_DIR/.." && pwd)
PROJECT_M4_DIR="${PROJECT_DIR}/GCC_Project"

# check if we have m4 code in the current project
PROJECT_M4_BIN=
if [ -d "$PROJECT_M4_DIR" ] ; then
    PROJECT_OUTPUT_BIN_DIR="${PROJECT_DIR}/GCC_Project/output/bin"
    PROJECT_M4_BIN=$(ls "$PROJECT_OUTPUT_BIN_DIR"/*.bin)
    if [ ! -f "$PROJECT_M4_BIN" ] ; then
        printf "\nERROR: m4 binary does not exist! (is build done?)\n"
        exit 1
    fi
fi

printf "\nrunning command:\n"
printf "qfprog --port $PORT --bootloader $PROJECT_M4_BIN --mode --reset\n\n"
qfprog --port "$PORT" --bootloader "$PROJECT_M4_BIN" --mode --reset
