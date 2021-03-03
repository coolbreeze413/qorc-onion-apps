Intro
=====

This example application demonstrates one of the use-cases of the eFPGA core in the EOSS3.

There is no built-in PWM Controller available in the EOSS3.

The eFPGA on the EOSS3 can access all of the 46 pads (IO_0 to IO_45) and use it according 
to any design on the eFPGA.

In this application, we take the first 32 pads (IO_0 to IO_31) and enable usage as PWM pins 
via the eFPGA design. 

To control the PWMs exposed in this way, from the M4, we use the AHB-Wishbone Interconnect 
between the M4 Subsystem and the eFPGA, hence making it possible to use it like any other 
peripheral on the M4 Subsystem.

Using a set of 32-bit registers, exposed over the AHB-Wishbone Interconnect, M4 code can 
just set bits on the registers and control the IO pads to set a duty-cycle/enable for PWM function.


How To
======

Build FPGA and M4
-----------------

From the project root dir, execute:

::
  
  make -C GCC_Project

This will generate:

- m4app binary: :code:`GCC_Project/output/bin/qorc_fpga_pwmctrl.bin`
- appfpga binary: :code:`fpga/rtl/AL4S3B_FPGA_Top.bin`


Build only FPGA (optional)
--------------------------

It is recommended to always use the :code:`make` to build both M4 and FPGA, as only the changed files are 
actually built.

If you really want to build only the FPGA binary, you can do so as below.

From the project root dir, execute:

::
  
  ql_symbiflow -compile -src fpga/rtl -d ql-eos-s3 -P pu64 -v AL4S3B_FPGA_Top.v AL4S3B_FPGA_IP.v AL4S3B_FPGA_QL_Reserved.v AL4S3B_FPGA_ONION_PWMCTRL.v ONION_PWM.v -t AL4S3B_FPGA_Top -p quickfeather.pcf -dump binary

This will create the appfpga binary: :code:`fpga/rtl/AL4S3B_FPGA_Top.bin`



Flash FPGA and M4
------------------

Put the development board into Flash Mode.

From the project root dir, execute:

::
  
  qfprog --port /dev/ttyACM0 --m4app GCC_Project/output/bin/qorc_fpga_pwmctrl.bin --appfpga fpga/rtl/AL4S3B_FPGA_Top.bin --mode fpga-m4
  
If you are flashing with a USB-UART connected to the EOSS3 UART port, then replace the ttyACMx with appropriate ttyUSBx, for example:

::

  qfprog --port /dev/ttyUSB0 --m4app GCC_Project/output/bin/qorc_fpga_pwmctrl.bin --appfpga fpga/rtl/AL4S3B_FPGA_Top.bin --mode fpga-m4
  

Before running the code in the next section, if you want to use the CLI to test, ensure that 
you have connected a USB-UART adapter connected to the EOSS3 UART pins (IO_44/IO_45), 
and you have a serial terminal connected to that port at 115200 8N1.


Run Code
--------

Once the board is flashed, and reset, you should see a pattern of 3 sets of Triple Magenta Blinks (after the 
flashing BLUE LED pattern put out by the bootloader as usual) indicating the code has loaded and 
started execution (FPGA and M4).

This pattern of 3 Triple-Magenta-Blinks is executed using the eFPGA based PWM Controller Design itself.

Each Triple-Magenta-Blink would be seen with 3 increasing brightness levels of the leds.

This can be see in the code of `src/main.c [Line 85 - Line 131] <src/main.c#L85-L131>`__


This is followed by a banner like the below on the serial terminal:

::

  ##########################
  OnionApps FPGA PWM Controller Example
  SW Version: qorc-onion-apps/qorc_fpga_pwmctrl
  Feb 19 2021 18:57:15
  ##########################
  
  
  
  Hello PWM CONTROLLER!
  
  #*******************
  
  Command Line Interface
  
  App SW Version: qorc-onion-apps/qorc_fpga_pwmctrl
  
  #*******************
  
  [0] > 

  

Enter the PWM Controller submenu using :code:`pwmctrl` and then type :code:`help` in the pwmctrl submenu for commands.

::
  
  [0] > pwmctrl
  [1] pwmctrl > help
  help-path: pwmctrl
  enpwm          - enpwm IO_X VAL(0-255)
  dispwm         - dispwm IO_X
  getpwm         - getpwm IO_X
  exit           - exit/leave menu
  help           - show help
  ?              - show help
  help-end:

To set a GPIO as output with specific value, use :code:`enpwm IO_PADNUMBER VALUE_TO_SET`

For example, to set IO_22 to value 50  (this is connected to RED LED on the PygmyBB4/QF):

::

  [1] pwmctrl > enpwm 22 50
  io = 22
  val = 50

The RED LED should have turned on, with approximately half brightness.

To read current PWM config, use :code:`getpwm IO_PADNUMBER`

For reading IO_22 for example

::
  
  [1] pwmctrl > getpwm 22
  io = 22
  read value = 0x80000032


To set IO_22 to value 150:

::

  [1] pwmctrl > enpwm 22 150
  io = 22
  val = 150

The RED LED should have got set to approximately 80% brightness.

Read IO_22 PWM:

::
  
  [1] pwmctrl > getpwm 22
  io = 22
  read value = 0x80000096



To disable the PWM on IO_22:

::

  [1] pwmctrl > dispwm 22
  io = 22

The RED LED should have got turned off.

Read IO_22 PWM:

::
  
  [1] pwmctrl > getpwm 22
  io = 22
  read value = 0x00000000





Details
=======

Note that the :code:`IO_PADNUMBER` is the actual pad number of the EOSS3 and is clearly marked on the PygmyBB4 pins.

In the schematic also, we can see this pad number mentioned, for example, IO_22 can be seen connected to R_LED.

In brief, the PWM Controller is instantiated in the FPGA, and defines 32 registers to control IO_0 - IO_31.

These registers are accessed like normal 32-bit registers from M4 code using the AHB-Wishbone interconnect.

From the M4 side, it looks like memory mapped AHB registers, just like any other peripheral.

The AHB-Wishbone Bridge converts the AHB read/write transactions into Wishbone read/write transactions.

We implement the Wishbone read/write transaction decoding in the eFPGA verilog code, and interpret 
the register read/write into logic for PWM Control.

::

  .
  FURTHER INFORMATION TO BE ADDED
  .