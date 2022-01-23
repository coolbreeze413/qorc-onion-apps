Intro
=====

This example application demonstrates one of the use-cases of the eFPGA core in the EOSS3.

The eFPGA on the EOSS3 can access all of the 46 pads (IO_0 to IO_45) and use it according 
to any design on the eFPGA.

"breathe" refers to the slow pulsing of LEDs from zero to peak brightness and back to zero 
imitating the breathing action of inhale-exhale. We use a few extra counters to vary the PWM 
on the IOs to achieve the "breathing" effect.

In this application, we take the first 32 pads (IO_0 to IO_31) and enable usage as "breathe" pins 
via the eFPGA design. 

To control the IOs exposed in this way, from the M4, we use the AHB-Wishbone Interconnect 
between the M4 Subsystem and the eFPGA, hence making it possible to use it like any other 
peripheral on the M4 Subsystem.

Using a set of 32-bit registers, exposed over the AHB-Wishbone Interconnect, M4 code can 
just set bits on the registers and control the IO pads to set a breathe-period/enable for 
BREATHE function.


How To
======

Build M4
--------

From the project root dir, execute:

::
  
  make -C GCC_Project

This will generate m4app binary: :code:`GCC_Project/output/bin/qorc_fpga_breathectrl.bin`


Build FPGA
----------

From the project root dir, execute:

::
  
  ql_symbiflow -compile -src fpga/rtl -d ql-eos-s3 -P PU64 -v AL4S3B_FPGA_Top.v AL4S3B_FPGA_IP.v AL4S3B_FPGA_QL_Reserved.v AL4S3B_FPGA_ONION_BREATHECTRL.v ONION_BREATHE.v -t AL4S3B_FPGA_Top -p quickfeather.pcf -dump binary

This will create the appfpga binary: :code:`fpga/rtl/AL4S3B_FPGA_Top.bin`

Alternatively, use the 'build_fpga.sh' script:

::

  ./.scaffolding/fpga_build.sh



Flash FPGA and M4
------------------

Put the development board into Flash Mode.

From the project root dir, execute:

::
  
  qfprog --port /dev/ttyACM0 --m4app GCC_Project/output/bin/qorc_fpga_breathectrl.bin --appfpga fpga/rtl/AL4S3B_FPGA_Top.bin --mode fpga-m4 --reset
  
If you are flashing with a USB-UART connected to the EOSS3 UART port, then replace the ttyACMx with appropriate ttyUSBx, for example:

::

  qfprog --port /dev/ttyUSB0 --m4app GCC_Project/output/bin/qorc_fpga_breathectrl.bin --appfpga fpga/rtl/AL4S3B_FPGA_Top.bin --mode fpga-m4 --reset
  

Before running the code in the next section, if you want to use the CLI to test, ensure that 
you have connected a USB-UART adapter connected to the EOSS3 UART pins (IO_44/IO_45), 
and you have a serial terminal connected to that port at 115200 8N1.


Run Code
--------

Once the board is flashed, and reset, you should see a pattern of 3 sets of Triple Magenta Breaths (after the 
flashing BLUE LED pattern put out by the bootloader as usual) indicating the code has loaded and 
started execution (FPGA and M4).

This pattern of 3 Triple-Magenta-Breaths is executed using the eFPGA based Breathe Controller Design itself.

Each Triple-Magenta-Breath would be seen with 3 decreasing "breathe periods" of LEDs, from ~2 seconds 
per breath (inhale+exhale) to ~0.6 seconds per breath.

This can be see in the code of `src/main.c [Line 92 - Line 117] <src/main.c#L92-L117>`__


This is followed by a banner like the below on the serial terminal:

::

  ##########################
  ONION FPGA BREATHE Controller Experiment
  SW Version: qorc-onion-apps/qorc_fpga_breathectrl
  Mar  8 2021 22:05:33
  ##########################
  
  
  
  Hello BREATHE CONTROLLER!
  
  FPGA Device ID: 0x0000c004
  [0] > 

  

Enter the Breathe Controller submenu using :code:`breathectrl` and then type :code:`help` in the breathectrl submenu for commands.

::
  
  [0] > breathectrl
  [1] breathectrl > help
  help-path: breathectrl
  enbreathe      - enbreathe IO_X period_msec
  disbreathe     - disbreathe IO_X
  getbreathe     - getbreathe IO_X
  exit           - exit/leave menu
  help           - show help
  ?              - show help
  help-end:


To set a GPIO as breathe output with specific period, use :code:`enbreathe IO_PADNUMBER PERIOD_MSEC`

For example, set IO_22 to 2 sec (1 sec inhale, 1 sec exhale):

::

  [1] breathectrl > enbreathe 22 2000
  io = 22
  val = 2000

The RED LED should have be breathing with approx 1 sec inhale, and 1 sec exhale period

To read current breathe config, use :code:`getbreathe IO_PADNUMBER`

For reading IO_22 for example:

::
  
  [1] breathectrl > getbreathe 22
  io = 22
  breathe_period = 2000 [0x000007d0] msec



To set IO_22 to 1 sec (0.5 sec inhale, 0.5 sec exhale):

::

  [1] breathectrl > enbreathe 22 1000
  io = 22
  val = 1000

The RED LED should have be breathing with approx 0.5 sec inhale, and 0.5 sec exhale period

Read IO_22 breathe config:

::
  
  [1] breathectrl > getbreathe 22
  io = 22
  breathe_period = 999 [0x000003e7] msec


To disable the breathing on IO_22:

::

  [1] breathectrl > disbreathe 22
  io = 22

The RED LED should have got turned off.

Read IO_22 breathe config:

::
  
  [1] breathectrl > getbreathe 22
  io = 22
  breathe is disabled



Details
=======

::

  .
  FURTHER INFORMATION TO BE ADDED
  .