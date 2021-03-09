Intro
=====

This example application demonstrates various use-cases of the eFPGA core in the EOSS3 in 
combination with the Cortex-M4F core (communication over AHB, Interrupts)

This combines multiple FPGA Designs into one, and makes all of them accessible over the 
AHB2WB interface - exposing the addresses as simple AHB addresses that the M4 can write/read.
This also demonstrates the usage of the Interrupt mechanism from the FPGA to the M4 (with a 
simple timer based design) and how the M4 code can be setup to configure and handle the interrupts.

The following are individual projects, which describe in detail each of the FPGA designs used, and 
can be referred to:

1. Simple GPIO CONTROLLER Example : control all EOSS3 IOs using FPGA, from M4

   https://github.com/coolbreeze413/qorc-onion-apps/tree/master/qorc_fpga_gpioctrl

2. Simple PWM CONTROLLER Example : built on top of the PWM example, use from M4
   
   https://github.com/coolbreeze413/qorc-onion-apps/tree/master/qorc_fpga_pwmctrl

3. Simple BREATHE CONTROLLER Example : built on top of the BREATHE example, use from M4

   https://github.com/coolbreeze413/qorc-onion-apps/tree/master/qorc_fpga_breathectrl

4. Simple TIMER CONTROLLER Example : use TIMER on FPGA, use from M4, provides Interrupt on 
   Timer Expiry from FPGA to M4, illustrates both Wishbone access and Interrupt Path

   https://github.com/coolbreeze413/qorc-onion-apps/tree/master/qorc_fpga_timerctrl


How To
======

Build FPGA and M4
-----------------

From the project root dir, execute:

::
  
  make -C GCC_Project

This will generate:

- m4app binary: :code:`GCC_Project/output/bin/qorc_fpga_compositeGPBTctrl.bin`
- appfpga binary: :code:`fpga/rtl/AL4S3B_FPGA_Top.bin`


Flash FPGA and M4
------------------

Put the development board into Flash Mode.

From the project root dir, execute:

::
  
  qfprog --port /dev/ttyACM0 --m4app GCC_Project/output/bin/qorc_fpga_compositeGPBTctrl.bin --appfpga fpga/rtl/AL4S3B_FPGA_Top.bin --mode fpga-m4 --reset
  
If you are flashing with a USB-UART connected to the EOSS3 UART port, then replace the ttyACMx with appropriate ttyUSBx, for example:

::

  qfprog --port /dev/ttyUSB0 --m4app GCC_Project/output/bin/qorc_fpga_compositeGPBTctrl.bin --appfpga fpga/rtl/AL4S3B_FPGA_Top.bin --mode fpga-m4 --reset
  

Before running the code in the next section, if you want to use the CLI to test, ensure that 
you have connected a USB-UART adapter connected to the EOSS3 UART pins (IO_44/IO_45), 
and you have a serial terminal connected to that port at 115200 8N1.


Run Code
--------

Once the board is flashed, and reset, you should see a pattern of:

1. Triple-Magenta-Blink controlled using the GPIO Controller FPGA Module

2. Triple-Magenta-PWMBlink controlled using the PWM Controller FPGA Module

3. Triple-Magenta-Breathe controlled using the BREATHE Controller FPGA Module

indicating the code has loaded and started execution (FPGA and M4).


Main Menu
~~~~~~~~~

This is followed by a banner like the below on the serial terminal:

::

  ##########################
  ONION FPGA Composite Controller Experiment
  SW Version: qorc-onion-apps/qorc_fpga_compositeGPBTctrl
  Mar  9 2021 22:43:55
  ##########################
  
  
  
  Hello GPIO/PWM/BREATHE/TIMER CONTROLLER!
  
  FPGA Device ID: 0x0000c00f
  [0] >

Each of the Module submenus are available, using help displays:

::

  [0] > help
  help-path: (top)
  gpioctrl       - FPGA GPIO Controller
  pwmctrl        - FPGA PWM Controller
  breathectrl    - FPGA BREATHE Controller
  timerctrl      - FPGA TIMER Controller
  exit           - exit/leave menu
  help           - show help
  ?              - show help
  help-end:


GPIOCTRL SubMenu
~~~~~~~~~~~~~~~~
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


PWMCTRL SubMenu
~~~~~~~~~~~~~~~

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



BREATHECTRL SubMenu
~~~~~~~~~~~~~~~~~~~

Enter the Breathe Controller submenu using :code:`breathectrl` and then type :code:`help` in the breathectrl submenu for commands.

::
  
  [0] > breathectrl
  [1] breathectrl > help
  help-path: breathectrl
  enbreathe      - enbreathe IO_X VAL(24-bit)
  disbreathe     - disbreathe IO_X
  getbreathe     - getbreathe IO_X
  exit           - exit/leave menu
  help           - show help
  ?              - show help
  help-end:


For example, set IO_22 to value 43690(~1 sec inhale, ~1 sec exhale):

::

  [1] breathectrl > enbreathe 22 43690
  io = 22
  val = 43690

The RED LED should have be breathing with approx 1 sec inhale, and 1 sec exhale period

To read current breathe config, use :code:`getbreathe IO_PADNUMBER`

For reading IO_22 for example:

::
  
  [1] breathectrl > getbreathe 22
  io = 22
  breathe_config = 0x8000aaaa
  breathe_period = 43690 [0x00aaaa]

To disable the breathing on IO_22:

::

  [1] breathectrl > disbreathe 22
  io = 22

The RED LED should have got turned off.

Read IO_22 breathe config:

::
  
  [1] breathectrl > getbreathe 22
  io = 22
  breathe_config = 0x00000000
  breathe is disabled

TIMERCTRL SubMenu
~~~~~~~~~~~~~~~~~

Enter the Timer Controller submenu using :code:`timerctrl` and then type :code:`help` in the timerctrl submenu for commands.

::
  
  [0] > timerctrl
  [1] timerctrl > help
  help-path: timerctrl
  entimer        - entimer ID VAL(30-bit)
  distimer       - distimer ID
  gettimer       - gettimer ID
  exit           - exit/leave menu
  help           - show help
  ?              - show help
  help-end:



To set a Timer with specific period, use :code:`entimer TIMER_ID TIMER_PERIOD_CLK_CYCLES`

:code:`NOTE: default clock we use here is 12MHz, and values of clock cycles below are calculated according to that.`

Currently, only one Timer is instantiated, and its ID is 0

For example, set TIMER0 to period 22369280(~1 sec):

::

  [1] timerctrl > entimer 0 22369280
  id = 22
  val = 22369280

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