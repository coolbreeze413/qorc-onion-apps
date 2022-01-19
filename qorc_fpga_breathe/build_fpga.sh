#!/bin/bash

# to adapt to a new fpga-only project:
# [1] change the top module name (arg provided to '-t')
# [2] adjust the output format(s) for the fpga design to be generated: (arg(s) to '-dump')
# binary        (fpga bin for flashing into the spi flash, which the qorc bootloader can load)
# bitheader     (fpga bin as a 'C' header file)
# jlink         (fpga bin loading via jlink script)
# openocd       (fpga bin loading via openocd script)

TMP_SOURCE="fpga/rtl"

TMP_VERILOG_FILES=`cd ${TMP_SOURCE};ls *.v`

echo "$TMP_VERILOG_FILES" > ${TMP_SOURCE}/tmp_v_list

sed '/^$/d' $TMP_SOURCE/tmp_v_list > $TMP_SOURCE/tmp_f_list

TMP_VERILOG_FILES=`cat $TMP_SOURCE/tmp_f_list`

rm $TMP_SOURCE/tmp_v_list $TMP_SOURCE/tmp_f_list

# note: provide an absolute path to the -src parameter (especially when dumping multiple output formats at the same time)
ql_symbiflow -compile -src ${PWD}/fpga/rtl -d ql-eos-s3 -t AL4S3B_FPGA_Top -v "$TMP_VERILOG_FILES" -p quickfeather.pcf -P PU64 -dump binary header openocd jlink

# if you have verilog files you don't want to be included, set the files yourself:
#ql_symbiflow -compile -src fpga/rtl -d ql-eos-s3 -t AL4S3B_FPGA_Top -v AL4S3B_FPGA_Top.v ONION_BREATHE.v -p quickfeather.pcf -P PU64 -dump binary
