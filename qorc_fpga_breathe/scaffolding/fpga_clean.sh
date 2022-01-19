#!/bin/bash

# to adapt to a new fpga-only project: nothing needs to be changed

PROJECT_ROOT_DIR=$(cd .. ; printf %s "$PWD")
PROJECT_RTL_DIR="${PROJECT_ROOT_DIR}/fpga/rtl"

rm -rf "$PROJECT_RTL_DIR"/build || true
rm "$PROJECT_RTL_DIR"/*bit.h || true
rm "$PROJECT_RTL_DIR"/*.bin || true
rm "$PROJECT_RTL_DIR"/*.jlink || true
rm "$PROJECT_RTL_DIR"/*.openocd || true
rm "$PROJECT_RTL_DIR"/Makefile.symbiflow || true
