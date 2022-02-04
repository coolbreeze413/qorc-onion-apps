ONION FPGA BREATHE Module Example
=================================

Intro
-----

We create a simple module to realize a "breathing effect", and instantiate 3 blocks from it, to control the RGB LEDs 
of the QuickFeather.

Using varying pwm, we control brightness from 0 (off) to max and back to 0 - creating inhale 
and exhale effects.

Once flashed and board is reset, we can see the RGB LED glow with a breathing effect, with 
the following color sequence:

- 2x blue
- 2x green
- 2x red
- 2x yellow
- 2x magenta
- 2x cyan
- [repeats from blue]


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

     cd <QORC_SDK_PATH> && source envsetup.sh && cd -


Clean/Build/Load/Flash (Command Line)
*************************************

- Clean the design using: :code:`.scaffolding/clean_fpga.sh`

- Build the design using: :code:`.scaffolding/build_fpga.sh`

- Load and run the design on the board using JLinkExe, using: :code:`.scaffolding/load_fpga_jlink.sh`

  (assumes the board has been booted in DEBUG mode)

- Load and run the design on the board using OpenOCD, using:

  (assumes the board has been booted in DEBUG mode)

  ::

    .scaffolding/load_fpga_openocd_gdb.sh --openocd-if-cfg=<PATH_TO_INTERFACE_CFG>

  The INTERFACE_CFG file depends on the debug adapter chosen.

  Here are a few common adapters that can be used with the EOS_S3:
  
  1. JLink Adapters: :code:`--openocd-if-cfg=.scaffolding/jlink_swd.cfg` (available in the current dir)
  2. FT2232H Boards: :code:`--openocd-if-cfg=.scaffolding/ft2232h_swd.cfg` (available in the current dir)
  3. STLinkv2 Adapters: :code:`--openocd-if-cfg=interface/stlink-v2.cfg` (available in the OpenOCD install scripts dir)
  4. DAPLink Adapters: :code:`--openocd-if-cfg=interface/cmsis-dap.cfg` (available in the OpenOCD install scripts dir)

  Practically, any adapter that supports OpenOCD and SWD can be used with the appropriate cfg file passed in.

- Flash and run the design on the board using qfprog: :code:`.scaffolding/flash_fpga.sh --port=/dev/ttyACM0`

  (assumes the board is put into :code:`programming` mode)

  Change the serial port as applicable.


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

   Remaining variables don't need to be changed.

2. Open the current directory in VS Code using :code:`File > Open Folder` menu
   
   - To be able to run the 'flash' task, remember to install the extension: :code:`augustocdias.tasks-shell-input`
     
     Also, the 'flash' task needs to scan for available serial ports in the system, so python, and pySerial need to be installed.


Clean/Build/Load/Flash (VS Code)
********************************

Any "task" can be run in VS Code using the :code:`Terminal > Run Task` menu, which shows a drop down list of tasks

-OR-

Using keyboard shortcuts: :code:`ctrl+p` and then type :code:`task<space>`, which shows a drop down list of tasks

- Clean the design using:
  
  :code:`clean-fpga` task

- Build the design using:

  :code:`build-fpga` task

- Load and run the design on the board using JLinkExe, using:

  (assumes the board has been booted in DEBUG mode)

  :code:`load-fpga (JLink)` task

- Load and run the design on the board using OpenOCD, using:

  (assumes the board has been booted in DEBUG mode)

  :code:`load-fpga (OpenOCD)` task

  This will show a drop down menu with the options of debug adapters currently tested:

  - JLink Adapters :code:`jlink_swd.cfg`
  - FT2232H Boards :code:`ft2232h_swd.cfg`
  - STLinkv2 Adapters :code:`interface/stlink-v2.cfg`
  - DAPLink Adapters :code:`interface/cmsis-dap.cfg`

  select the appropriate one.

- Flash and run the design on the board using qfprog:

  (assumes the board is put into :code:`programming` mode)

  :code:`flash-fpga` task

  This will show a drop down menu with the available serial ports in the system, select the appropriate one.
  
  (This is usually :code:`/dev/ttyACM0`)
