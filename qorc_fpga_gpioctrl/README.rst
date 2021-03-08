Intro
=====

This example application demonstrates one of the use-cases of the eFPGA core in the EOSS3.
The M4 Subsystem in the EOSS3 has access to a GPIO Controller, which has support for only 
8 pads to be used as a GPIO. 
If those pads are already muxed to different functions, it becomes restrictive in the number 
of GPIOs available in the system.

The eFPGA on the EOSS3 can access all of the 46 pads (IO_0 to IO_45) and use it according 
to any design on the eFPGA.

In this application, we take the first 32 pads (IO_0 to IO_31) and enable usage as GPIO pins 
via the eFPGA design. 

To control the GPIOs exposed in this way, from the M4, we use the AHB-Wishbone Interconnect 
between the M4 Subsystem and the eFPGA, hence making it possible to use it like any other 
peripheral on the M4 Subsystem.

Using a set of 32-bit registers, exposed over the AHB-Wishbone Interconnect, M4 code can 
just set bits on the registers and control the IO pads as either input (to read the logic value) 
or as output (to set the logic value).


How To
======

Build FPGA and M4
-----------------

From the project root dir, execute:

::
  
  make -C GCC_Project

This will generate:

- m4app binary: :code:`GCC_Project/output/bin/qorc_fpga_gpioctrl.bin`
- appfpga binary: :code:`fpga/rtl/AL4S3B_FPGA_Top.bin`


Build only FPGA (optional)
--------------------------

It is recommended to always use the :code:`make` to build both M4 and FPGA, as only the changed files are 
actually built.

If you really want to build only the FPGA binary, you can do so as below.

From the project root dir, execute:

::
  
  ql_symbiflow -compile -src fpga/rtl -d ql-eos-s3 -P pu64 -v AL4S3B_FPGA_Top.v AL4S3B_FPGA_IP.v AL4S3B_FPGA_QL_Reserved.v AL4S3B_FPGA_ONION_GPIOCTRL.v -t AL4S3B_FPGA_Top -p quickfeather.pcf -dump binary

This will create the appfpga binary: :code:`fpga/rtl/AL4S3B_FPGA_Top.bin`



Flash FPGA and M4
------------------

Put the development board into Flash Mode.

From the project root dir, execute:

::
  
  qfprog --port /dev/ttyACM0 --m4app GCC_Project/output/bin/qorc_fpga_gpioctrl.bin --appfpga fpga/rtl/AL4S3B_FPGA_Top.bin --mode fpga-m4 --reset
  
If you are flashing with a USB-UART connected to the EOSS3 UART port, then replace the ttyACMx with appropriate ttyUSBx, for example:

::

  qfprog --port /dev/ttyUSB0 --m4app GCC_Project/output/bin/qorc_fpga_gpioctrl.bin --appfpga fpga/rtl/AL4S3B_FPGA_Top.bin --mode fpga-m4 --reset
  

Before running the code in the next section, if you want to use the CLI to test, ensure that 
you have connected a USB-UART adapter connected to the EOSS3 UART pins (IO_44/IO_45), 
and you have a serial terminal connected to that port at 115200 8N1.


Run Code
--------

Once the board is flashed, and reset, you should see a pattern of 3 Magenta Blinks (after the 
flashing BLUE LED pattern put out by the bootloader as usual) indicating the code has loaded and 
started execution (FPGA and M4)

This pattern of 3 Magenta Blinks  is executed using the eFPGA based GPIO Controller Design itself.

This can be see in the code of `src/main.c [Line 85 - Line 102] <src/main.c#L85-L102>`__


This is followed by a banner like the below on the serial terminal:

::

  ##########################
  OnionApps FPGA GPIO Controller Example
  SW Version: qorc-onion-apps/qorc_fpga_gpioctrl
  Feb 19 2021 18:57:15
  ##########################
  
  
  
  Hello GPIO CONTROLLER!
  
  #*******************
  
  Command Line Interface
  
  App SW Version: qorc-onion-apps/qorc_fpga_gpioctrl
  
  #*******************
  
  [0] > 

  

Enter the GPIO Controller submenu using :code:`gpioctrl` and then type :code:`help` in the gpioctrl submenu for commands.

::
  
  [0] > gpioctrl
  
  [1] gpioctrl > help
  
  help-path: gpioctrl
  
  setout         - setout IO_X VAL
  
  setin          - setin IO_X
  
  getval         - getval IO_X (must be setin first)
  
  exit           - exit/leave menu
  
  help           - show help
  
  ?              - show help
  
  help-end:

  
  
To set a GPIO as output with specific value, use :code: `setout IO_PADNUMBER VALUE_TO_SET`

For example, to set IO_22 to value 1 (this is connected to RED LED on the PygmyBB4/QF):

::

  [1] gpioctrl > setout 22 1
  io = 22
  val = 1

The RED LED should have turned on.

To set IO_22 to value 0:

::

  [1] gpioctrl > setout 22 0
  io = 22
  val = 0

The RED LED should have turned off.



To read GPIO value, we need to ensure that this IO has been set as input first using :code:`setin IO_PADNUMBER` and 
then we can read the value using :code:`getval IO_PADNUMBER`

For reading IO_5 for example, we set it to input mode first:

::
  
  [1] gpioctrl > setin 5
  io = 5
  
Connect a Jumper Wire from IO_5 to 3V3 on the PygmyBB4/QF board, and the read the value:

::

  [1] gpioctrl > getval 5
  io = 5
  val = 0
  read value = 1
  
Now connect the Jumper Wire from IO_5 to GND instead, and read the value:

::

  [1] gpioctrl > getval 5
  io = 5
  val = 1
  read value = 0


Details
=======

Note that the :code:`IO_PADNUMBER` is the actual pad number of the EOSS3 and is clearly marked on the PygmyBB4 pins.

In the schematic also, we can see this pad number mentioned, for example, IO_22 can be seen connected to R_LED.

In brief, the GPIO Controller is instantiated in the FPGA, and defines 3 registers to control IO_0 - IO_31.

These registers are accessed like normal 32-bit registers from M4 code using the AHB-Wishbone interconnect.

From the M4 side, it looks like memory mapped AHB registers, just like any other peripheral.

The AHB-Wishbone Bridge converts the AHB read/write transactions into Wishbone read/write transactions.

We implement the Wishbone read/write transaction decoding in the eFPGA verilog code, and interpret 
the register read/write into logic for GPIO Control.

The OFFSETS of these registers are:
[fpga/rtl/GPIO_controller.v]

::

  localparam  REG_ADDR_GPIO_IN    =  8'h00        ; 
  localparam  REG_ADDR_GPIO_OUT   =  8'h04        ; 
  localparam  REG_ADDR_GPIO_OE    =  8'h08        ; 


The GPIO Controller itself is defined to have an OFFSET as below:
[fpga/rtl/AL4S3B_FPGA_IP.v]

::

  parameter       GPIO_BASE_ADDRESS           = 17'h04000     ;


Finally, the FPGA BASE ADDRESS (when accessed from the AHB side of M4) is :code:`0x40020000`
[HAL/inc/eoss3_dev.h]

Using this information, we see that the basic design is :

- Control GPIO as output or input by setting 1 or 0 respectively to the appropriate bit in :code:`REG_ADDR_GPIO_OE (0x40024008)`

- If set as output, set the appropriate bit as 1 or 0 for High/Low in :code:`REG_ADDR_GPIO_OUT (0x40024004)`

- If set as input read the value at appropriate bit from :code:`REG_ADDR_GPIO_IN (0x40024000)`



  
