#!/bin/bash

# to adapt to a new fpga-only project: nothing needs to be changed

rm -rf fpga/rtl/build || true
rm fpga/rtl/*bit.h || true
rm fpga/rtl/*.bin || true
rm fpga/rtl/*.jlink || true
rm fpga/rtl/*.openocd || true
rm fpga/rtl/Makefile.symbiflow || true
