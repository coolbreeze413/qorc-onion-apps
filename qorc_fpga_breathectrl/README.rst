QORC FPGA Breathe Controller Application
========================================


Intro
-----

This example application demonstrates one of the use-cases of the eFPGA core in the EOSS3.

The eFPGA on the EOSS3 can access all of the 46 pads (IO_0 to IO_45) and use it according 
to any design on the eFPGA.

"breathe" refers to the slow pulsing of LEDs from zero to peak brightness and back to zero 
imitating the breathing action of inhale-exhale. We use a few extra counters to vary the PWM 
on the IOs to achieve the "breathing" effect.

In this application, we take 3 pads (IO_18, IO_21, IO_22) and enable usage as "breathe" pins 
via the eFPGA design. 

To control the IOs exposed in this way, from the M4, we use the AHB-Wishbone Interconnect 
between the M4 Subsystem and the eFPGA, hence making it possible to use it like any other 
peripheral on the M4 Subsystem.

Using a set of 32-bit registers, exposed over the AHB-Wishbone Interconnect, M4 code can 
just set bits on the registers and control the IO pads to set a breathe-period/enable for 
BREATHE function.


Usage
-----

:code:`note: ensure that you have connected a USB-UART adapter connected to the EOSS3 UART pins (IO_44/IO_45), and you have a serial terminal connected to that port at 115200 8N1.`

Once the code is running, you should see a pattern of 3 sets of Triple Magenta Breaths 
indicating the code has loaded and started execution (FPGA and M4).

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


How To
------

Command Line Usage
~~~~~~~~~~~~~~~~~~

Note that, all the commands below are run from the root of this directory.

Initialize Environment
**********************

Before clean/build/load/flash, ensure that the bash environment is setup by doing the below:

1. Ensure that QORC-SDK is initialized and ready (to use :code:`JLinkExe` or :code:`OpenOCD` for loading, :code:`qfprog` for flashing):

   ::

     source <QORC_SDK_PATH>/envsetup.sh


Clean/Build/Load/Flash (Command Line)
*************************************

- Clean using:

  fpga: :code:`make clean-fpga`

  m4: :code:`make clean-m4`

  both: :code:`make clean`

- Build using:

  fpga: :code:`make fpga`

  m4: :code:`make m4`

  both: :code:`make`

- Load and run the design on the board using JLinkExe, using:

  (assumes the board has been booted in DEBUG mode)

  ::
      
    make load-jlink

- Load and run the design on the board using OpenOCD, using:

  (assumes the board has been booted in DEBUG mode)

  ::

    export QORC_OCD_IF_CFG=/path/to/inteface/cfg    # needs to be done only once in the current shell
    make load-openocd

  The interface cfg file depends on the debug adapter chosen.

  Here are a few common adapters that can be used with the EOS_S3:
  
  1. JLink Adapters: :code:`export QORC_OCD_IF_CFG=.scaffolding/jlink_swd.cfg` (available in the current dir)
  2. FT2232H Boards: :code:`export QORC_OCD_IF_CFG=.scaffolding/ft2232h_swd.cfg` (available in the current dir)
  3. STLinkv2 Adapters: :code:`export QORC_OCD_IF_CFG=interface/stlink-v2.cfg` (available in the OpenOCD install scripts dir)
  4. DAPLink Adapters: :code:`export QORC_OCD_IF_CFG=interface/cmsis-dap.cfg` (available in the OpenOCD install scripts dir)

  Practically, any adapter that supports OpenOCD and SWD can be used with the appropriate cfg file passed in.

- Flash and run the design on the board using qfprog:
  
  (assumes the board is put into :code:`programming` mode)

  ::

    export QORC_PORT=/path/to/serial/port   # needs to be done only once in current shell
    make flash

  Set the serial port as applicable, this is generally :code:`export QORC_PORT=/dev/ttyACM0`


VS Code Usage
~~~~~~~~~~~~~

Initialize Project Configuration
********************************

The first time the project is going to be used from VS Code, we need to do the following:

1. copy :code:`.vscode/settings.template.jsonc` as :code:`.vscode/settings.json`

   Ensure the following variables are correctly defined:

   ::

     "qorc_sdk_path" : "${workspaceFolder}/../..",

   In VS Code:

   :code:`${env:HOME}` refers to $HOME of the current user

   :code:`${workspaceFolder}` refers to the current directory

   Remaining variables don't need to be changed

2. Open the current directory in VS Code using :code:`File > Open Folder` menu
   
   - To be able to run the 'flash' task, remember to install the extension: :code:`augustocdias.tasks-shell-input`
     
     Also, the 'flash' task needs to scan for available serial ports in the system, so python, and pySerial need to be installed.

   - To be able to 'debug' the code with gdb, remember to install the extension: :code:`marus25.cortex-debug`

   On opening the folder, VS Code should prompt to install these "recommended extensions", if not already installed, 
   select :code:`Install All` to automatically install them.


Clean/Build/Load/Flash (VS Code)
********************************

Any "task" can be run in VS Code using the :code:`Terminal > Run Task` menu, which shows a drop down list of tasks

-OR-

Using keyboard shortcuts: :code:`ctrl+p` and then type :code:`task<space>`, which shows a drop down list of tasks

- Clean using:
  
  - fpga: :code:`clean-fpga` task
  - m4: :code:`clean-m4` task
  - both: :code:`clean` task

- Build using:

  - fpga: :code:`build-fpga` task
  - m4: :code:`build-m4` task
  - both: :code:`build` task

- Load and run the design on the board using JLinkExe, using:
  
  (assumes the board has been booted in DEBUG mode)

  :code:`load (JLink)` task

- Load and run the design on the board using OpenOCD, using:

  (assumes the board has been booted in DEBUG mode)

  :code:`load (OpenOCD)` task

  This will show a drop down menu with the options of debug adapters currently tested:

  - JLink Adapters :code:`.scaffolding/jlink_swd.cfg`
  - FT2232H Boards :code:`.scaffolding/ft2232h_swd.cfg`
  - STLinkv2 Adapters :code:`interface/stlink-v2.cfg`
  - DAPLink Adapters :code:`interface/cmsis-dap.cfg`

  select the appropriate one.

- Flash and run the design on the board using qfprog:

  (assumes the board is put into :code:`programming` mode)

  :code:`flash` task

  This will show a drop down menu with the available serial ports in the system, select the appropriate one.
  
  (This is usually :code:`/dev/ttyACM0`)

- :code:`debug-load-fpga (JLink)` : this is a special task required only while debugging the code with JLink.

  Refer to the Debug sections for details.


Debug
*****

- Debug the code via JLink :

  1. To bring up the :code:`Run and Debug` view, select the Run icon in the Activity Bar on the side of VS Code.
  
  2. Select :code:`Debug (JLink)` from the drop down at the top of the side bar
  
  3. Start Debugging by clicking the green :code:`Play Button`
  
  4. The code should load and break at :code:`main()`
  
  5. Run the task :code:`debug-load-fpga (JLink)` at this point, to load the FPGA design
  
  6. Resume/Continue debugging using the blue :code:`Continue/Break` button at the top or using :code:`F8`


- Debug the code via OpenOCD :

  1. To bring up the :code:`Run and Debug` view, select the Run icon in the Activity Bar on the side of VS Code.
  
  2. Select :code:`Debug (OpenOCD)` from the drop down at the top of the side bar
  
  3. Start Debugging by clicking the green :code:`Play Button`
  
  4. A drop-down menu appears to select the debug adapter being used, currently the choices are:
   
     - :code:`.scaffolding/jlink_swd.cfg`
     - :code:`.scaffolding/ft2232h_swd.cfg`
     - :code:`interface/stlink-v2.cfg`
     - :code:`interface/cmsis-dap.cfg`

     More can be added in the :code:`launch.json` file.
     
     Select the appropriate one.

  5. The fpga bitstream (.openocd) should get loaded, then the m4 code should load and break at :code:`main()`
  
  6. Resume/Continue debugging using the blue :code:`Continue/Break` button at the top or using :code:`F8`


- Common Debugging Steps with the :code:`Cortex-Debug` extension in VS Code:

  1. Place breakpoints in the code by clicking near the line number
  
  2.  Use the :code:`Step Over`, :code:`Step Into`, :code:`Step Out`, :code:`Restart`, :code:`Stop` buttons to control the debugging session


References
~~~~~~~~~~

1. https://code.visualstudio.com/docs/editor/debugging
2. https://marketplace.visualstudio.com/items?itemName=marus25.cortex-debug
3. https://mcuoneclipse.com/2021/05/09/visual-studio-code-for-c-c-with-arm-cortex-m-part-4/
