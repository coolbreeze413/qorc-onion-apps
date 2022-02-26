
ONION FPGA RELOAD
=================

Intro
-----

This example reconfigures the EOS-S3 FPGA with different images dynamically.

A simple CLI exposes commands to reconfigure the FPGA to help demonstrate the sequence.

This addresses the quirky requirement (IO_19 needs to be LOW for FPGA power cycle), and 
shows a simple sequence to achieve it.

If you want to run this project via a debugger, then IO_19 needs to be HIGH when the 
EOS-S3 is reset to set the bootstraps correctly. This is generally done by jumpers on 
the board (e.g. QuickFeather, Pygmy).

**Once out of reset**, remove the jumpers from the board for IO_19.
This should enable the weak PD to GND, and IO_19 will be LOW.

For pygmy-stamp based boards, there is an onboard pull-up resistor, and hence removing
the IO_19 jumper is not enough, connect a wire from IO_19 to one of the GND pins on the 
board to create a strong pull-down.

If this application is flashed and run from boot-from-flash, it should mostly work, though 
pymgy-stamp based boards still need to wire the IO_19 to GND directly.

This example uses precompiled FPGA images from other examples in this repo in the form of 
'C Header' files, which is then directly included as code in the M4 build - this is the 
easiest method currently, to include multiple FPGA images.

The flash method has (currently) provision for one FPGA image only.

This needs to be addressed and made generic, sort of like a set of images and their map 
described by a csv file or similar by the BL to figure out stuff - similar to how the 
ESP-IDF does, offering greatest flexibility.

Well, there's always more work to do.

Usage
-----

CLI Interface example usage as below:

.. code-block:: bash


   ##########################
   Onion FPGA reload Example
   SW Version: qorc-onion-apps/qorc_fpgareload_header
   Feb 26 2022 01:47:53
   ##########################
   
   
   
   FPGA Reconfiguration involves a power cycle of the power domain
   This *requires* that IO_19 be pulled *LOW*
   
   
   If there are jumpers for IO_19 on the board, remove them
   
   
   If there are pullups on the board (pygmy-stamp based)
   short the IO_19 to a GND pin using a jumper wire
   
   
   side note: if this application is being run via debugger
           ensure IO_19 is *HIGH* if(and until) the board is reset
           so that the EOS-S3 boots up in DEBUGGER mode, 
           and then change it later to be *LOW* when changing the FPGA images 
   
   
   #*******************
   Command Line Interface
   App SW Version: qorc-onion-apps/qorc_fpgareload_header
   #*******************
   [0] > fpgaloader
   [1] fpgaloader > show 
   
       [0] blinky
   
       [1] pwm
   
       [2] breathe
   
   [1] fpgaloader > load 0
   
   fpga_num = 0
   loading fpga >>>> blinky
   
   [1] fpgaloader > show
   
       [0] blinky <=== [running]
   
       [1] pwm
   
       [2] breathe
   
   [1] fpgaloader > load 1
   
   fpga_num = 1
   loading fpga >>>> pwm
   
   [1] fpgaloader > show
   
       [0] blinky
   
       [1] pwm <=== [running]
   
       [2] breathe
   
   [1] fpgaloader > load 2
   
   fpga_num = 2
   loading fpga >>>> breathe
   
   [1] fpgaloader > show
   
       [0] blinky
   
       [1] pwm
   
       [2] breathe <=== [running]
   
   [1] fpgaloader > 
   
