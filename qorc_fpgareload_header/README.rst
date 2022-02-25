
ONION FPGA RELOAD
=================

Intro
-----

This example reconfigures the EOS-S3 FPGA with different images dynamically.

A simple CLI exposes commands to reconfigure the FPGA to help demonstrate the sequence.

This addresses the quirky requirement (IO_19 needs to be LOW for FPGA power cycle), and 
shows a simple sequence to achieve it.

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
   
   
   If there are jumpers on the board, remove them
   
   
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
   
