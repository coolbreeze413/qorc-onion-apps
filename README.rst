QORC ONION Apps
===============

Experimental stuff that can be run with the QORC SDK.

Additionally, the applications support makefile usage for all tasks - clean/build/flash/load(JLink/OpenOCD) 
as well as VSCode support out of the box for all of the above, as well as Debug(JLink/OpenOCD).


Flashloaders
------------

1. "DUAL" Flashloader : used to prep a new board with EOSS3 and SPI Flash to flash the bootloader, bootfpga, appfpga and m4app, or to 'reinitialize' a board
   with updated binaries of bootloader/bootfpga.

   Listens to both USBSERIAL, as well as the EOSS3 UART at IO_44/IO_45, so either USB port on the board, or external USB-UART cable can be used to flash the images.
   
   Loading this can be done via JLink/OpenOCD, and then the board can be prepped by using a simple 'flash-initialize' to flash the set of bootloader, 
   bootfpga (usb-serial fpga image), m4app, appfpga images (simple helloworld m4+fpga).

   refer : `qorc_loadflash_dual <./qorc_loadflash_dual>`__

Bootloaders
------------

1. "DUAL" Bootloader : used to flash m4 and/or fpga images onto the SPI Flash, and load them from SPI Flash.

   Listens to both USBSERIAL, as well as the EOSS3 UART at IO_44/IO_45, so either USB port on the board, or external USB-UART cable can be used to flash the images.

   refer : `qorc_bootloader_dual <./qorc_bootloader_dual>`__

2. "CLI" Bootloader : exposes a CLI interface on the USBSERIAL to have a menu-driven BL, 
   expanding the applications beyond flash/load.
   
   :code:`warning: UNSTABLE`

   refer : `qorc_bootloader_cli <./qorc_bootloader_cli>`__


Templates
---------

A set of projects for different scenarios that be used as a 'template' to create a new project painlessly and then add any features needed, without
needing to worry about changes in the build/flash/load/debug infrastructure.

1. `qorc_helloworldm4 <./qorc_helloworldm4>`__ : project which uses only the Cortex-M4 core of the EOSS3

2. `qorc_helloworldfpga <./qorc_helloworldfpga>`__ : project which uses only the eFPGA core of the EOSS3

3. `qorc_helloworldm4fpga <./qorc_helloworldm4fpga>`__ : project which uses both the Cortex-M4 core and eFPGA core of the EOSS3 (without communication between M4 and eFPGA cores)

4. `qorc_helloworldm4fpgaheader <./qorc_helloworldm4fpgaheader>`__ : project which uses both the Cortex-M4 core and eFPGA core of the EOSS3 (without communication between M4 and eFPGA cores)
   
   The difference in this project vs (3) is that, the fpga code is built to generate a 'C Header' with a binary array containing the FPGA bitstream instead of a separate binary image.

   This 'C Header' is included as regular C code and compiled into a single binary for the Cortex-M4, but contains code for both the Cortex-M4 as well as the FPGA.

   This is more of a legacy method, and can be used if needed.

   :code:`TODO`


FPGA Standalone Examples
-------------------------

1. Simple "PWM" Module Example

   `qorc_fpga_pwm <./qorc_fpga_pwm>`__

2. Simple "BREATHE" Module Example

   `qorc_fpga_breathe <./qorc_fpga_breathe>`__


FPGA + M4 Combined Examples
---------------------------

1. Simple GPIO CONTROLLER Example : control all EOSS3 IOs using FPGA, from M4

   `qorc_fpga_gpioctrl <./qorc_fpga_gpioctrl>`__

2. Simple PWM CONTROLLER Example : built on top of the PWM example, use from M4
   
   `qorc_fpga_pwmctrl <./qorc_fpga_pwmctrl>`__

3. Simple BREATHE CONTROLLER Example : built on top of the BREATHE example, use from M4

   `qorc_fpga_breathectrl <./qorc_fpga_breathectrl>`__

4. Simple TIMER CONTROLLER Example : use TIMER on FPGA, use from M4, provides Interrupt on 
   Timer Expiry from FPGA to M4, illustrates both Wishbone access and Interrupt Path

   `qorc_fpga_timerctrl <./qorc_fpga_timerctrl>`__

5. Composite FPGA_IP with all of the above to illustrate a building-block method of having 
   multiple Functional Blocks in the design, all accessible from the M4, and co-operatively 
   sharing the IOs

   `qorc_fpga_compositeGPBctrl <./qorc_fpga_compositeGPBctrl>`__


Experimental
------------

1. Dynamically load FPGA images as and when needed:

   :code:`warning: UNSTABLE`
   
   - `qorc_fpgareload_flash <./qorc_fpgareload_flash>`__ : demo for loading different FPGA bitstreams stored in SPI flash.
   - `qorc_fpgareload_header <./qorc_fpgareload_header>`__ : demo for loading different FPGA bitstreams built into the Cortex-M4 binary (legacy 'C Header' FPGA bitstreams)

2. Pygmy Test:

   - `qorc_pygmy_test <./qorc_pygmy_test>`__ : a test code to verify various peripherals attached to the Pygmy-based boards from OptimusLogic.

Useful Links
------------

1. QORC SDK Setup

   https://qorc-sdk.readthedocs.io/en/latest/qorc-setup/qorc-setup.html

2. QORC SDK FPGA Toolchain : Mapping Pins in the PCF File

   https://quicklogic-fpga-tool-docs.readthedocs.io/en/latest/tutorial/PcfDescription.html

3. QORC SDK FPGA Toolchain : Details of Standalone Binary structure

   https://quicklogic-fpga-tool-docs.readthedocs.io/en/latest/tutorial/GeneratetheBinaryFile.html

4. QORC SDK BOOTLOADER : M4/FPGA Loading Details (**needs cleanup**)

   https://github.com/QuickLogic-Corp/qorc-sdk/blob/flashing-loading-changes-for-fpga/qf_apps/qf_bootloader/flashing-loading-changes.rst

   https://github.com/QuickLogic-Corp/qorc-sdk/blob/flashing-loading-changes-for-fpga/qf_apps/qf_bootloader/flash_memory_map.rst


OptimusLogic Pygmy Based Devkit Series Info
-------------------------------------------

Features, Pinouts of various devkits (BB0, BB4, Sensei-Lite, Sensei):

https://www.optimuslogic.in/product_pygmy.html
