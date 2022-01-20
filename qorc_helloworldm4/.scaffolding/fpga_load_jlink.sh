#!/bin/bash

# load the fpga design generated as a jlink command file(.jlink)
# REQ:
# 1. JLink probe is connected to EOS_S3 and it is in DEBUG mode
# 2. JLinkExe is available in the path ('source debugenvsetup.sh') OR the path-to-JLinkExe is passed in as argument.
# 3. (.jlink) has been generated by adding a 'jlink' to the -dump command to ql_symbiflow

JLINK_EXE_PATH=$(which JLinkExe)

################################################################################
#   getopt based parsing
################################################################################
# option strings
SHORT="" # no short options at all, clearer that way.
# reference on how to enforce no short options: https://unix.stackexchange.com/questions/162624/how-to-use-getopt-in-bash-command-line-with-only-long-options
LONG="jlink-exe-path:,help"


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
    printf " syntax: $0 --jlink-exe-path=/path/to/JLinkExe\n"
    printf "\n"
    printf "example: $0 --jlink-exe-path=/usr/bin/JLinkExe\n"
    printf "\n"
}

# extract options and their arguments into variables
while true ; do
    case "$1" in
        --jlink-exe-path )
            JLINK_EXE_PATH="$2"
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
if [ -z "$JLINK_EXE_PATH" ] ; then
    printf "\nERROR: JLINK_EXE_PATH is not defined!\n"
    usage
    exit 1
fi


# confirmation print
printf "\n"
printf "JLINK_EXE_PATH=$JLINK_EXE_PATH\n"
printf "\n"
################################################################################



PROJECT_ROOT_DIR=$(cd .. ; printf %s "$PWD")
PROJECT_RTL_DIR="${PROJECT_ROOT_DIR}/fpga/rtl"


# the generated .jlink script only focusses on loading the design and setting the IOMUX
#  registers for the design - it does not do anything related to initialization of the EOS_S3
# typically, we need to ensure that the EOS_S3 has been reset before loading the design.

# https://wiki.segger.com/J-Link_Commander

# init the S3 with reset
"$JLINK_EXE_PATH" -Device Cortex-M4 -If SWD -Speed 4000 -commandFile reset_eoss3.jlink

# load fpga design : note that the JLinkExe will still be running after load, enter 'q' to quit
"$JLINK_EXE_PATH" -Device Cortex-M4 -If SWD -Speed 4000 -commandFile "$PROJECT_RTL_DIR"/*.jlink
