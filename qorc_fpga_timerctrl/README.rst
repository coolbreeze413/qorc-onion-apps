Intro
=====

This example application demonstrates one of the use-cases of the eFPGA core in the EOSS3.

The eFPGA on the EOSS3 can send an interrupt to the Cortex-M4 core, and we have 4 dedicated 
interrupt "channels" from the FPGA to the NVIC.

In this application, we create a very simple one-shot timer controller peripheral in the design, and on 
completion of the programmed period, the timer will cause a rising trigger on the 0th 
interrupt channel.

The TimerController exposes one configuration register via the WB interface to configure/enable the timer.
The M4 side code introduces how to enable, configure and use the interrupts from the FPGA.


How To
======

Build FPGA and M4
-----------------

From the project root dir, execute:

::
  
  make -C GCC_Project

This will generate:

- m4app binary: :code:`GCC_Project/output/bin/qorc_fpga_timerctrl.bin`
- appfpga binary: :code:`fpga/rtl/AL4S3B_FPGA_Top.bin`


Build only FPGA (optional)
--------------------------

It is recommended to always use the :code:`make` to build both M4 and FPGA, as only the changed files are 
actually built.

If you really want to build only the FPGA binary, you can do so as below.

From the project root dir, execute:

::
  
  ql_symbiflow -compile -src fpga/rtl -d ql-eos-s3 -P pu64 -v AL4S3B_FPGA_Top.v AL4S3B_FPGA_IP.v AL4S3B_FPGA_QL_Reserved.v AL4S3B_FPGA_ONION_TIMERCTRL.v ONION_TIMER.v -t AL4S3B_FPGA_Top -p quickfeather.pcf -dump binary

This will create the appfpga binary: :code:`fpga/rtl/AL4S3B_FPGA_Top.bin`



Flash FPGA and M4
------------------

Put the development board into Flash Mode.

From the project root dir, execute:

::
  
  qfprog --port /dev/ttyACM0 --m4app GCC_Project/output/bin/qorc_fpga_timerctrl.bin --appfpga fpga/rtl/AL4S3B_FPGA_Top.bin --mode fpga-m4 --reset
  
If you are flashing with a USB-UART connected to the EOSS3 UART port, then replace the ttyACMx with appropriate ttyUSBx, for example:

::

  qfprog --port /dev/ttyUSB0 --m4app GCC_Project/output/bin/qorc_fpga_timerctrl.bin --appfpga fpga/rtl/AL4S3B_FPGA_Top.bin --mode fpga-m4 --reset
  

Before running the code in the next section, if you want to use the CLI to test, ensure that 
you have connected a USB-UART adapter connected to the EOSS3 UART pins (IO_44/IO_45), 
and you have a serial terminal connected to that port at 115200 8N1.


Run Code
--------

Once the board is flashed, and reset, you should see banner like the below on the serial terminal:

::

  ##########################
  ONION FPGA TIMER Controller Experiment
  SW Version: qorc-onion-apps/qorc_fpga_timerctrl
  Mar  9 2021 21:33:08
  ##########################
  
  
  
  Hello TIMER CONTROLLER!
  
  FPGA Device ID: 0x0000c008
  [0] > 

 

Enter the Timer Controller submenu using :code:`timerctrl` and then type :code:`help` in the timerctrl submenu for commands.

::
  
  [0] > timerctrl
  [1] timerctrl > help
  help-path: timerctrl
  entimer        - entimer ID period_msec
  distimer       - distimer ID
  gettimer       - gettimer ID
  exit           - exit/leave menu
  help           - show help
  ?              - show help
  help-end:



To set a Timer with specific period, use :code:`entimer TIMER_ID TIMER_PERIOD_MSEC`

Currently, only one Timer is instantiated, and its ID is 0, so :code:`TIMER_ID = 0`

For example, set TIMER0 to period 1 second:

::

  [1] timerctrl > entimer 0 1000
  id = 22
  val = 1000

You should see the prints on the serial terminal shortly after:

::

  timer0 enabled

  timer0 ISR

  timer0 disabled


These are output by a small monitor task - to which we send messages from the HAL enable/disable 
functions, and the FPGA Interrupt Handler

As a "debug" output, the TIMER0 also triggers the BLUE LED, and it should be visible for a very 
small amount of time, when the timer expires - we disable the timer as soon as the Interrupt 
Handler is hit, which disables the TIMER Interrupt, as well as the debug output.

To disable TIMER0 manually:

::

  [1] timerctrl > distimer 0
  io = 0
  [1] timerctrl > 
  timer0 disabled



Details
=======

::

  .
  FURTHER INFORMATION TO BE ADDED
  .