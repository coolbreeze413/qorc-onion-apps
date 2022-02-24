#!/bin/bash


# fpga-m4 load:
# load fpga design, m4 code, or both using OpenOCD + gdb
# we check if the current project has an fpga design (fpga/ dir), m4 code (GCC_Project/ dir) and accordingly
#    generate a 'combined' gdb sequence which loads the project onto the EOS-S3
# the fpga design is loaded using the generated (*.openocd) cfg script file from the Symbiflow tooling.
#     the openocd script exposes a TCL proc "load_bitstream" which can be called from gdb to load the fpga design.
# the m4 code is loaded using the ELF (*.elf) generated from gcc (this can also be done using the bin file if needed)

# summary: run openocd in background (with the .openocd cfg script passed in), run gdb and send appropriate sequence of commands.
# we stay in gdb so user can further debug stuff.

# REQ:
# 1. OpenOCD-supported debug adapter is connected to EOS-S3 and the EOS-S3 is booted in DEBUG mode
# 2. openocd, arm-none-eabi-gdb, are available in the path ('source envsetup.sh' of the QORC SDK has been done)
# 3. [if project has fpga design] (.openocd) has been generated by adding a 'openocd' to the -dump command to ql_symbiflow
#    the current makefile for fpga add the openocd to -dump by default

SCRIPT_DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )

OPENOCD_INTERFACE_CFG=
OPENOCD_PATH=$(which openocd)
GDB_PATH=$(which arm-none-eabi-gdb)
################################################################################
#   getopt based parsing
################################################################################
# option strings
SHORT="" # no short options at all, clearer that way.
# reference on how to enforce no short options: https://unix.stackexchange.com/questions/162624/how-to-use-getopt-in-bash-command-line-with-only-long-options
LONG="openocd-if-cfg:,help"


# read the options
OPTS=$(getopt --options "$SHORT" --long "$LONG" --name "$0" -- "$@")

if [ $? != 0 ] ; then echo "ERROR: failed to parse options...exiting." >&2 ; exit 1 ; fi

eval set -- "$OPTS"

usage()
{
    printf "\n"
    printf "[%s] usage:\n" $(basename $0)
    printf "\n"
    printf "load fpga design, m4 code using OpenOCD and gdb\n"
    printf "\n"
    printf " syntax: $0 --openocd-if-cfg=/path/to/adapter/cfg \n"
    printf "\n"
    printf "example: $0 --openocd-if-cfg=.scaffolding/ft2232h_swd.cfg \n"
    printf "\n"
}

# extract options and their arguments into variables
while true ; do
    case "$1" in
        --openocd-if-cfg )
            OPENOCD_INTERFACE_CFG="$2"
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
if [ -z "$OPENOCD_INTERFACE_CFG" ] ; then
    printf "\nERROR: OPENOCD_INTERFACE_CFG is not defined!\n"
    usage
    exit 1
fi

# sanity checks
if [ -z "$OPENOCD_PATH" ] ; then
    printf "\nERROR: OPENOCD_PATH is not defined!\n"
    printf "\nOpenOCD should be on the path, is the QORC SDK initialized with 'source envsetup.sh'?\n"
    exit 1
fi


if [ -z "$GDB_PATH" ] ; then
    printf "\nERROR: GDB_PATH is not defined!\n"
    printf "\narm-none-eabi-gdb should be on the path, is the QORC SDK initialized with 'source envsetup.sh'?\n"
    exit 1
fi


# confirmation print
printf "\n"
printf "OPENOCD_INTERFACE_CFG=$OPENOCD_INTERFACE_CFG\n"
printf "OPENOCD_PATH=$OPENOCD_PATH\n"
printf "GDB_PATH=$GDB_PATH\n"
printf "\n"


PROJECT_DIR=$(cd "$SCRIPT_DIR/.." && pwd)
PROJECT_M4_DIR="${PROJECT_DIR}/GCC_Project"
PROJECT_FPGA_DIR="${PROJECT_DIR}/fpga"


# check if we have m4 code in the current project
PROJECT_M4_ELF=
if [ -d "$PROJECT_M4_DIR" ] ; then
    PROJECT_OUTPUT_BIN_DIR="${PROJECT_DIR}/GCC_Project/output/bin"
    PROJECT_M4_BIN=$(ls "$PROJECT_OUTPUT_BIN_DIR"/*.bin 2>/dev/null)
    PROJECT_M4_ELF=$(ls "$PROJECT_OUTPUT_BIN_DIR"/*.elf 2>/dev/null)
    if [ ! -f "$PROJECT_M4_ELF" ] ; then
        printf "\nERROR: m4 elf does not exist! (is build done?)\n"
        exit 1
    fi
fi
printf "PROJECT_M4_ELF=$PROJECT_M4_ELF\n\n"

# check if we have fpga design in the current project
PROJECT_FPGA_DESIGN_OPENOCD=
if [ -d "$PROJECT_FPGA_DIR" ] ; then
    PROJECT_RTL_DIR="${PROJECT_FPGA_DIR}/rtl"
    # note: the openocd script generated, exposes a jim-tcl proc(edure) called 'load_bitstream' that can be executed to load the fpga.
    # this should be passed in to openocd
    PROJECT_FPGA_DESIGN_OPENOCD=$(ls "$PROJECT_RTL_DIR"/*.openocd 2>/dev/null)
    if [ ! -f "$PROJECT_FPGA_DESIGN_OPENOCD" ] ; then
        printf "\nERROR: fpga .openocd does not exist! (is build done?)\n"
        exit 1
    fi
fi
printf "PROJECT_FPGA_DESIGN_OPENOCD=$PROJECT_FPGA_DESIGN_OPENOCD\n\n"

CUSTOM_OPENOCD_GDB_LOG="custom_eoss3_m4_fpga.openocd.log"

# [step 1]run openocd (background, suppress output)
if [ -f "$PROJECT_FPGA_DESIGN_OPENOCD" ] ; then
    # pass in the fpga generated .openocd cfg script, and the debug adapter 'interface' cfg script
    "$OPENOCD_PATH" -f "$OPENOCD_INTERFACE_CFG" -f target/eos_s3.cfg -f "$PROJECT_FPGA_DESIGN_OPENOCD" -c "init" -c "reset halt" > "$CUSTOM_OPENOCD_GDB_LOG" 2>&1 &
else
    # pass in the debug adapter 'interface' cfg script
    "$OPENOCD_PATH" -f "$OPENOCD_INTERFACE_CFG" -f target/eos_s3.cfg > "$CUSTOM_OPENOCD_GDB_LOG" -c "init" -c "reset halt" 2>&1 &
fi

# [step 2] wait a second for the openocd server to startup
sleep 1

# [step 3] run gdb, load m4, load fpga, run m4 code, wait for user commands.
# ref: https://stackoverflow.com/a/58137098
# ref: https://stackoverflow.com/a/46867839
# use -q to suppress banner: https://stackoverflow.com/a/61473013, but still enter the gdb shell after that.
# use -batch to fully automate the steps and close gdb automatically
# note that we use a gdbinit script, passed in using -x to:
#   define a hook for quit, which will also close the openocd server.
#   set pagination off
#   set confirm off while quitting
# to load the bin file, use: -ex "monitor load_image $PROJECT_M4_BIN 0x0" before "resume", and remove the ELF file from gdb args.
if [ -f "$PROJECT_FPGA_DESIGN_OPENOCD" ] && [ -f "$PROJECT_M4_ELF" ] ; then

    # fpga + m4, load both elf and bitstream
    "$GDB_PATH" \
        $PROJECT_M4_ELF \
        -q \
        -x "$SCRIPT_DIR/load_fpga_m4_openocd_gdbinit" \
        -ex "target extended-remote localhost:3333" \
        -ex "load" \
        -ex "monitor reset halt" \
        -ex "monitor load_bitstream" \
        -ex "monitor resume" \
        -ex "monitor echo \"running m4 code...\"" \
        -ex "monitor mdw 0x40005484" \
        -ex "monitor echo \"use 'q' or 'quit' to exit gdb session\n\"" \
        -ex "monitor echo \"use 'ctrl+z' for the nuclear option and kill openocd and arm-none-eabi-gdb\n\""

elif [ -f "$PROJECT_FPGA_DESIGN_OPENOCD" ] ; then

    # fpga-only, load bitstream
    "$GDB_PATH" \
        -q \
        -x "$SCRIPT_DIR/load_fpga_m4_openocd_gdbinit" \
        -ex "target extended-remote localhost:3333" \
        -ex "monitor load_bitstream" \
        -ex "monitor mdw 0x40005484" \
        -ex "monitor echo \"use 'q' or 'quit' to exit gdb session\n\"" \
        -ex "monitor echo \"use 'ctrl+z' for the nuclear option and kill openocd and arm-none-eabi-gdb\n\""

elif [ -f "$PROJECT_M4_ELF" ] ; then

    # m4-only, load elf
    "$GDB_PATH" \
        $PROJECT_M4_ELF \
        -q \
        -x "$SCRIPT_DIR/load_fpga_m4_openocd_gdbinit" \
        -ex "target extended-remote localhost:3333" \
        -ex "load" \
        -ex "monitor reset halt" \
        -ex "monitor resume" \
        -ex "monitor echo \"running m4 code...\"" \
        -ex "monitor mdw 0x40005484" \
        -ex "monitor echo \"use 'q' or 'quit' to exit gdb session\n\"" \
        -ex "monitor echo \"use 'ctrl+z' for the nuclear option and kill openocd and arm-none-eabi-gdb\n\""

fi


# remove the custom script/log (disable for debugging the script)
rm "$CUSTOM_OPENOCD_GDB_LOG"
