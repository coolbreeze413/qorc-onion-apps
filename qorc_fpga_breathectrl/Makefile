# simple makefile wrapper for QORC SDK projects
SHELL:=bash

# for all targets, check whether the QORC SDK environment has been setup first.
ifndef QORC_SDK_PATH
$(info )
$(info QORC_SDK_PATH is not set in the environment, is the QORC SDK initialized?)
$(info initialize with: 'source envsetup.sh' from the QORC SDK directory in the current shell)
$(info )
$(error QORC_SDK_PATH not defined)
endif


.PHONY: all
all: fpga m4


.PHONY: clean
clean: clean-fpga clean-m4


.PHONY: m4
m4:
	@$(MAKE) -C GCC_Project


.PHONY: clean-m4
clean-m4:
	@$(MAKE) -C GCC_Project clean


.PHONY: fpga
fpga:
	@$(MAKE) -C fpga


.PHONY: clean-fpga
clean-fpga:
	@$(MAKE) -C fpga clean


.PHONY: load-jlink
load-jlink:
	.scaffolding/load_fpga_m4_jlink.sh


# QORC_OCD_IF_CFG is used, to be able to use the correct (interface) debug adapter config in OpenOCD
# 1. set QORC_OCD_IF_CFG=/path/to/cfg in shell env, before invoking make load-openocd (recommended, needed only once)
# or
# 2. pass in value while invoking: make load-openocd QORC_OCD_IF_CFG=/path/to/cfg
# default fallback, if not specified, is to assume a JLink adapter, with the cfg file in the .scaffolding dir
QORC_OCD_IF_CFG?=
.PHONY: load-openocd
load-openocd:
	@{ \
	if [ -z $$QORC_OCD_IF_CFG ] ; then \
		printf "\nERROR: QORC_OCD_IF_CFG not defined\n\n" ;\
		printf "    use: 'QORC_OCD_IF_CFG=/path/to/cfg', before invoking make $@\n" ;\
		printf "    >>OR<<\n" ;\
		printf "    use 'make $@ QORC_OCD_IF_CFG=/port/name'\n\n" ;\
		exit 1 ;\
	else \
		.scaffolding/load_fpga_m4_openocd_gdb.sh --openocd-if-cfg=$(QORC_OCD_IF_CFG) ;\
	fi \
	}


# QORC_PORT is used, to be able to flash using the correct serial port.
# 1. set QORC_PORT in shell env, before invoking make flash (recommended, needed only once)
# or
# 2. pass in the value while invoking make flash like so: make flash QORC_PORT=/dev/ttyPORTNAME
QORC_PORT?=
.PHONY: flash
flash:
	@{ \
	if [ -z $$QORC_PORT ] ; then \
		printf "\nERROR: QORC_PORT not defined\n\n" ;\
		printf "    use: 'QORC_PORT=/port/name', before invoking make $@\n" ;\
		printf "    >>OR<<\n" ;\
		printf "    use 'make $@ QORC_PORT=/port/name'\n\n" ;\
		exit 1 ;\
	else \
		.scaffolding/flash_fpga_m4.sh --port=$(QORC_PORT) ;\
	fi \
	}
