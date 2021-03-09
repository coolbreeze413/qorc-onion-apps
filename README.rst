qorc-onion-apps
===============

experimental stuff that can be run with the qorc-sdk.

Bootloaders
------------

1. "DUAL" Bootloader : listens to both EOSS3 UART and the USBSERIAL, HOST can flash the 
   images using either port - useful for situations when the USBSERIAL does not work on certain 
   HOST machines.

   https://github.com/coolbreeze413/qorc-onion-apps/tree/master/qorc_bootloader_dual

2. "CLI" Bootloader : exposes a CLI interface on the USBSERIAL to have a menu-driven BL, 
   expanding the applications beyond flash/load.
   :code:`warning: UNSTABLE`

FPGA Standalone Examples
-------------------------

1. Simple PWM Module Example

   https://github.com/coolbreeze413/qorc-onion-apps/tree/master/qorc_fpga_pwm

2. Simple "BREATHE" Module Example

   https://github.com/coolbreeze413/qorc-onion-apps/tree/master/qorc_fpga_breathe


FPGA + M4 Combined Examples
----------------------------

1. Simple GPIO CONTROLLER Example : control all EOSS3 IOs using FPGA, from M4

   https://github.com/coolbreeze413/qorc-onion-apps/tree/master/qorc_fpga_gpioctrl

2. Simple PWM CONTROLLER Example : built on top of the PWM example, use from M4
   
   https://github.com/coolbreeze413/qorc-onion-apps/tree/master/qorc_fpga_pwmctrl

3. Simple BREATHE CONTROLLER Example : built on top of the BREATHE example, use from M4

   https://github.com/coolbreeze413/qorc-onion-apps/tree/master/qorc_fpga_breathectrl

4. Simple TIMER CONTROLLER Example : use TIMER on FPGA, use from M4, provides Interrupt on 
   Timer Expiry from FPGA to M4, illustrates both Wishbone access and Interrupt Path

   https://github.com/coolbreeze413/qorc-onion-apps/tree/master/qorc_fpga_timerctrl

5. Composite FPGA_IP with all of the above to illustrate a building-block method of having 
   multiple Functional Blocks in the design, all accessible from the M4, and co-operatively 
   sharing the IOs

   https://github.com/coolbreeze413/qorc-onion-apps/tree/master/qorc_fpga_compositeGPBctrl


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