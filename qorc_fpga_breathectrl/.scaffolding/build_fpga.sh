#!/bin/bash


SCRIPT_DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )


################################################################################
#   getopt based parsing
################################################################################
# option strings
SHORT="" # no short options at all, clearer that way.
# reference on how to enforce no short options: https://unix.stackexchange.com/questions/162624/how-to-use-getopt-in-bash-command-line-with-only-long-options
LONG="qorc-sdk-path:,help"


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
    printf " syntax: $0 --qorc-sdk-path=/path/to/qorc/sdk\n"
    printf "\n"
    printf "example: $0 --qorc-sdk-path=$HOME/qorc-sdk\n"
    printf "\n"
}

# extract options and their arguments into variables
while true ; do
    case "$1" in
        --qorc-sdk-path )
            ARG_QORC_SDK_PATH="$2"
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
if [ -z "$ARG_QORC_SDK_PATH" ] ; then
    # check if QORC_SDK_PATH is already setup (source envsetup.sh has already been invoked in the current shell)
    if [ -z "$QORC_SDK_PATH" ] ; then
        printf "\nERROR: QORC_SDK_PATH is not set in env, AND --qorc-sdk-path is not passed in!\n\n"
        printf "    run source envsetup.sh from the QORC SDK directory in the current shell\n"
        printf "    >>OR<<\n"
        printf "    set the QORC SDK PATH using the --qorc-sdk-path=/path/to/qorc-sdk parameter\n\n"
        exit 1
    fi
fi


# confirmation print
printf "\n"
printf "ARG_QORC_SDK_PATH=$ARG_QORC_SDK_PATH\n"
printf "\n"
################################################################################


# setup QORC_SDK environment, if needed.
# we check that the QORC_SDK_PATH env variable is not already set, only then we need to do this,
# because source envsetup.sh sets the QORC_SDK_PATH env variable.
if [ -z "$QORC_SDK_PATH" ] ; then
    cd $ARG_QORC_SDK_PATH
    source envsetup.sh
    cd - > /dev/null
fi


PROJECT_ROOT_DIR=$(cd "$SCRIPT_DIR/.." && pwd)
PROJECT_RTL_DIR="${PROJECT_ROOT_DIR}/fpga/rtl"

#PROJECT_VERILOG_FILES="AL4S3B_FPGA_Top.v AL4S3B_FPGA_IP.v AL4S3B_FPGA_ONION_BREATHECTRL.v ONION_BREATHE.v AL4S3B_FPGA_QL_Reserved.v"
PROJECT_VERILOG_FILES=$(cd ${PROJECT_RTL_DIR};ls *.v)
echo "$PROJECT_VERILOG_FILES" > ${PROJECT_RTL_DIR}/tmp_v_list
sed '/^$/d' $PROJECT_RTL_DIR/tmp_v_list > $PROJECT_RTL_DIR/tmp_f_list
PROJECT_VERILOG_FILES=$(cat $PROJECT_RTL_DIR/tmp_f_list)
rm $PROJECT_RTL_DIR/tmp_v_list $PROJECT_RTL_DIR/tmp_f_list

# name of the "top" module in the fpga design
PROJECT_TOP_MODULE="AL4S3B_FPGA_Top"

#PROJECT_PCF_FILE="quickfeather.pcf"
PROJECT_PCF_FILE=$(cd ${PROJECT_RTL_DIR};ls *.pcf)

# PD64/PU64/WD48
PROJECT_PACKAGE="PU64"

PROJECT_DEVICE="ql-eos-s3"

# note: provide an absolute path to the -src parameter (especially when dumping multiple output formats at the same time)
# note: for m4-fpga-standalone projects, fpga is built independently, always specify dump binary (and openocd,jlink for debugging or loading over SWD)

printf "running symbiflow command:\n\n"
printf "ql_symbiflow -compile -src $PROJECT_RTL_DIR -d $PROJECT_DEVICE -t $PROJECT_TOP_MODULE -v $PROJECT_VERILOG_FILES -p $PROJECT_PCF_FILE -P $PROJECT_PACKAGE -dump binary openocd jlink\n\n"

ql_symbiflow -compile -src "$PROJECT_RTL_DIR" -d "$PROJECT_DEVICE" -t "$PROJECT_TOP_MODULE" -v "$PROJECT_VERILOG_FILES" -p "$PROJECT_PCF_FILE" -P "$PROJECT_PACKAGE" -dump binary openocd jlink
