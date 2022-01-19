#!/bin/bash

# requires that 'source envsetup.sh' from the qorc-sdk is already run so the toolchains are on the path!

# to adapt to a new fpga-only project:
# [1] change the top module name (arg provided to '-t')
# [2] adjust the output format(s) for the fpga design to be generated: (arg(s) to '-dump')
# binary        (fpga bin for flashing into the spi flash, which the qorc bootloader can load)
# bitheader     (fpga bin as a 'C' header file)
# jlink         (fpga bin loading via jlink script)
# openocd       (fpga bin loading via openocd script)

PROJECT_ROOT_DIR=$(cd .. ; printf %s "$PWD")
PROJECT_RTL_DIR="${PROJECT_ROOT_DIR}/fpga/rtl"

PROJECT_VERILOG_FILES=$(cd ${PROJECT_RTL_DIR};ls *.v)

echo "$PROJECT_VERILOG_FILES" > ${PROJECT_RTL_DIR}/tmp_v_list

sed '/^$/d' $PROJECT_RTL_DIR/tmp_v_list > $PROJECT_RTL_DIR/tmp_f_list

PROJECT_VERILOG_FILES=$(cat $PROJECT_RTL_DIR/tmp_f_list)

rm $PROJECT_RTL_DIR/tmp_v_list $PROJECT_RTL_DIR/tmp_f_list

# note: provide an absolute path to the -src parameter (especially when dumping multiple output formats at the same time)
ql_symbiflow -compile -src "$PROJECT_RTL_DIR" -d ql-eos-s3 -t AL4S3B_FPGA_Top -v "$PROJECT_VERILOG_FILES" -p quickfeather.pcf -P PU64 -dump binary header openocd jlink

# if you have verilog files you don't want to be included, set the files yourself:
#ql_symbiflow -compile -src "$PROJECT_RTL_DIR" -d ql-eos-s3 -t AL4S3B_FPGA_Top -v AL4S3B_FPGA_Top.v ONION_BREATHE.v -p quickfeather.pcf -P PU64 -dump binary
