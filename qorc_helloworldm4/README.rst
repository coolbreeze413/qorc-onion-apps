QORC HelloWorldM4 Application
=============================

This test/example contains the :code:`qorc_helloworldm4` example, which is derived from the :code:`qf_helloworldsw` example.
This is a simple M4-only application, which uses the EOS S3 UART to expose a command line interface (CLI).


Usage
------
:code:`note: ensure that you have connected a USB-UART adapter connected to the EOSS3 UART pins (IO_44/IO_45), and you have a serial terminal connected to that port at 115200 8N1.`

Once the code is running, you should see a banner similar to below on the EOS S3 UART:  

::

    ##########################
    Quicklogic QuickFeather M4 standalone
    SW Version: qorc-sdk/qf_apps/qf_helloworldm4
    Sep 20 2020 14:24:43
    ##########################

    #*******************
    Command Line Interface
    App SW Version: qorc-sdk/qf_apps/qf_helloworldm4
    #*******************
    [0] >


- | The :code:`diag` submenu option is available, and can be used to toggle the RGB leds or detect the USR button press:
  |
  | Toggle LEDs:
  | At the :code:`[0] >` prompt, which is the level 0 prompt, use:

- :code:`diag red` to toggle the red led
- :code:`diag green` to toggle the green led
- :code:`diag blue` to toggle the blue led

| Detect USR button press:
| At the :code:`[0] >` prompt, which is the level 0 prompt, do: 

- Keep the USR button pressed (connected to IO_6 on QuickFeather) and execute: :code:`diag userbutton` to check state - it should show :code:`Pressed`
- Without the USR button pressed, :code:`diag userbutton` should show: :code:`Not Pressed`


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

- Clean using:

  m4: :code:`make -C GCC_Project/ clean`

- Build using:

  m4: :code:`make -C GCC_Project/`

- Load and run the design on the board using JLinkExe, using: :code:`.scaffolding/load_m4_jlink.sh`

  (assumes the board has been booted in DEBUG mode)

- Load and run the design on the board using OpenOCD, using:

  (assumes the board has been booted in DEBUG mode)

  ::

    .scaffolding/load_m4_openocd_gdb.sh --openocd-if-cfg=<PATH_TO_INTERFACE_CFG>

  The INTERFACE_CFG file depends on the debug adapter chosen.

  Here are a few common adapters that can be used with the EOS_S3:
  
  1. JLink Adapters: :code:`--openocd-if-cfg=.scaffolding/jlink_swd.cfg` (available in the current dir)
  2. FT2232H Boards: :code:`--openocd-if-cfg=.scaffolding/ft2232h_swd.cfg` (available in the current dir)
  3. STLinkv2 Adapters: :code:`--openocd-if-cfg=interface/stlink-v2.cfg` (available in the OpenOCD install scripts dir)
  4. DAPLink Adapters: :code:`--openocd-if-cfg=interface/cmsis-dap.cfg` (available in the OpenOCD install scripts dir)

  Practically, any adapter that supports OpenOCD and SWD can be used with the appropriate cfg file passed in.

- Flash and run the design on the board using qfprog: :code:`.scaffolding/flash_m4.sh --port=/dev/ttyACM0`
  
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

   Remaining variables don't need to be changed

2. Open the current directory in VS Code using :code:`File > Open Folder` menu
   
   - To be able to run the 'flash' task, remember to install the extension: :code:`augustocdias.tasks-shell-input`
     
     Also, the 'flash' task needs to scan for available serial ports in the system, so python, and pySerial need to be installed.

   - To be able to 'debug' the code with gdb, remember to install the extension: :code:`marus25.cortex-debug`

   On opening the folder, VS Code should prompt to install "recommended extensions" and this can install them automatically.


Clean/Build/Load/Flash (VS Code)
********************************

Any "task" can be run in VS Code using the :code:`Terminal > Run Task` menu, which shows a drop down list of tasks

-OR-

Using keyboard shortcuts: :code:`ctrl+p` and then type :code:`task<space>`, which shows a drop down list of tasks

- Clean using:
  
  - m4: :code:`clean-m4` task

- Build using:

  - m4: :code:`build-m4` task

- Load and run the design on the board using JLinkExe, using:
  
  (assumes the board has been booted in DEBUG mode)

  :code:`load-m4 (JLink)` task

- Load and run the design on the board using OpenOCD, using:

  (assumes the board has been booted in DEBUG mode)

  :code:`load-m4 (OpenOCD)` task

  This will show a drop down menu with the options of debug adapters currently tested:

  - JLink Adapters :code:`jlink_swd.cfg`
  - FT2232H Boards :code:`ft2232h_swd.cfg`
  - STLinkv2 Adapters :code:`interface/stlink-v2.cfg`
  - DAPLink Adapters :code:`interface/cmsis-dap.cfg`

  select the appropriate one.

- Flash and run the design on the board using qfprog:

  (assumes the board is put into :code:`programming` mode)

  :code:`flash-m4` task

  This will show a drop down menu with the available serial ports in the system, select the appropriate one.
  
  (This is usually :code:`/dev/ttyACM0`)


Debug
*****

- Debug the code via JLink :

  1. To bring up the :code:`Run and Debug` view, select the Run icon in the Activity Bar on the side of VS Code.
  
  2. Select :code:`Debug (JLink)` from the drop down at the top of the side bar
  
  3. Start Debugging by clicking the green :code:`Play Button`
  
  4. The code should load and break at :code:`main()`
  
  5. Resume/Continue debugging using the blue :code:`Continue/Break` button at the top or using :code:`F8`


- Debug the code via OpenOCD :

  1. To bring up the :code:`Run and Debug` view, select the Run icon in the Activity Bar on the side of VS Code.
  
  2. Select :code:`Debug (OpenOCD)` from the drop down at the top of the side bar
  
  3. Start Debugging by clicking the green :code:`Play Button`
  
  4. A drop-down menu appears to select the debug adapter being used, currently the choices are:
   
     - :code:`jlink_swd.cfg`
     - :code:`ft2232h_swd.cfg`
     - :code:`interface/stlink-v2.cfg`
     - :code:`interface/cmsis-dap.cfg`

     More can be added in the :code:`launch.json` file.
     
     Select the appropriate one.

  5. The m4 code should load and break at :code:`main()`
  
  6. Resume/Continue debugging using the blue :code:`Continue/Break` button at the top or using :code:`F8`


- Common Debugging Steps with the :code:`Cortex-Debug` extension in VS Code:

  1. Place breakpoints in the code by clicking near the line number
  
  2.  Use the :code:`Step Over`, :code:`Step Into`, :code:`Step Out`, :code:`Restart`, :code:`Stop` buttons to control the debugging session


References
~~~~~~~~~~

1. https://code.visualstudio.com/docs/editor/debugging
2. https://marketplace.visualstudio.com/items?itemName=marus25.cortex-debug
3. https://mcuoneclipse.com/2021/05/09/visual-studio-code-for-c-c-with-arm-cortex-m-part-4/
