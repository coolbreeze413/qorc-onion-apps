How To
======

Build FPGA
----------

From the project root dir, execute:

::
  
  ql_symbiflow -compile -d ql-eos-s3 -P pu64 -v fpga/rtl/*.v -t top -p fpga/rtl/quickfeather.pcf -dump jlink binary

This will create both a :code:`top.bin` which can be flashed, as well as :code:`top.jlink` which can be used with J-Link, in the project root dir.


Build M4
--------

From the project root dir, execute:

::
  
  make -C m4/GCC_Project

This will generate a :code:`m4.bin` in m4/GCC_Project/output/bin which can be flashed.


Flash FPGA and M4
------------------

Put the development board into Flash Mode.

From the project root dir, execute:

::
  
  qfprog --port /dev/ttyACM0 --m4app m4/GCC_Project/output/bin/m4.bin --appfpga top.bin --mode m4-fpga
  
  
Ensure that you have connected a USB-UART adapter connected to the EOSS3 UART pins (IO_44/IO_45), and you have a terminal connected to that port.


Run Code
--------

Once the board is flashed, and reset, you should see a banner like below on the serial terminal:

::

  ##########################
  Quicklogic QuickFeather FPGA GPIO CONTROLLER EXAMPLE
  SW Version: qorc-onion-apps/qf_hello-fpga-gpio-ctlr
  Feb 19 2021 10:08:44
  ##########################



  Hello GPIO!!

  #*******************
  Command Line Interface
  App SW Version: qorc-onion-apps/qf_hello-fpga-gpio-ctlr
  #*******************
  [0] >
  

Enter the GPIO Controller submenu using :code:`gpioctlr` and then type :code:`help` in the gpioctlr submenu for commands.

::

  [0] > gpioctlr
  [1] gpioctlr > help
  help-path: gpioctlr
  setout     - set gpio as output with value specified
  setin      - set gpio as input
  getval     - read gpio value
  exit       - exit/leave menu
  help       - show help
  ?          - show help
  help-end:
  
  
To set a GPIO as output with specific value, use :code: `setout IO_NUMBER VALUE_TO_SET`

For setting IO_22 to value 1 for example (this is connected to RED LED on the PygmyBB4/QF)

::

  [1] gpioctlr > setout 22 1
  io = 22
  val = 1
  [1] gpioctlr > 

To read GPIO value, we need to ensure that this IO has been set as input first using :code:`setin IO_NUMBER` and 
then we can read the value using :code:`getval IO_NUMBER`

For reading IO_5 for example, we set it to input mode first:

::
  
  [1] gpioctlr > setin 5
  io = 5
  
Connect a Jumper Wire from IO_5 to 3V3 on the PygmyBB4/QF board, and the read the value:

::

  [1] gpioctlr > getval 5
  io = 5
  val = 0
  read value = 1
  
Now connect the Jumper Wire from IO_5 to GND instead, and read the value:

::

  [1] gpioctlr > getval 5
  io = 5
  val = 1
  read value = 0
  

Details
-------

In brief, the GPIO Controller is instantiated in the FPGA, and defines 3 registers to control IO_0 - IO_31.

The OFFSETS of these registers are as in the code:

::

  localparam  REG_ADDR_GPIO_IN    =  8'h00        ; 
  localparam  REG_ADDR_GPIO_OUT   =  8'h04        ; 
  localparam  REG_ADDR_GPIO_OE    =  8'h08        ; 


The GPIO Controller itself is defined to have an OFFSET of as in the code:

::

  parameter       GPIO_BASE_ADDRESS           = 17'h04000     ;
  
Finally, the FPGA BASE ADDRESS (when accessed from the AHB side of M4) is :code:`0x40020000`

Using this information, we see that the basic design is :

- Control GPIO as output or input be setting 1 or 0 respectively to the appropriate bit in :code:`REG_ADDR_GPIO_OE (0x40024008)`

- If set as output, set the appropriate bit as 1 or 0 for High/Low in :code:`REG_ADDR_GPIO_OUT (0x40024004)`

- If set as input read the value at appropriate bit from :code:`REG_ADDR_GPIO_IN (0x40024000)`



  
