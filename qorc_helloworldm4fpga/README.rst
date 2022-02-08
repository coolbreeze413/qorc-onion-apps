QORC HelloWorld M4 FPGA Application
====================================

Intro
-----

A template project with both m4 code and fpga design.

:code:`note: the fpga design and m4 code in this template are independent and do not communicate.`

The fpga design is a simple LED-toggle, which toggles the red LED regularly.

The m4 code is a simple CLI application, which also executes a couple of I2C transactions.


Project Structure Guide
~~~~~~~~~~~~~~~~~~~~~~~

- top-level :code:`Makefile` should be in the root of this directory
- if m4 code exists, it should have a top-level makefile at :code:`GCC_Project/Makefile`
- if fpga design exists, it should have a top-level Makefile at :code:`fpga/Makefile`
- fpga rtl verilog code should be in :code:`fpga/rtl/`
- if the fpga rtl has API/driver code, the sources should be :code:`fpga/src` and includes should be in :code:`fpga/inc`


Restructuring Existing QORC SDK Project
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

:code:`note: this is only a guideline`

- top-level :code:`Makefile`: copy into the existing project root directory

- copy :code:`fpga/Makefile`: into the existing project :code:`fpga/` directory
  
  :code:`fpga/Makefile` should then be modified with correct 'top' verilog module name instead of :code:`export QORC_FPGA_TOP_MODULE=helloworldfpga`

- compare :code:`GCC_Project/` with this project, and:
    
  - top-level :code:`GCC_Project/Makefile`: copy and replace from this template

    diff: adds QORC_SDK_PATH check, and removes extra 'FPGA' (loader) dependency, as it is already covered
    by :code:`GCC_Project/makefiles/Makefile_Libraries`

  - :code:`GCC_Project/makefiles/Makefile_common`: copy and replace from this template

    diff: removes fpga design build rules

  - :code:`GCC_Project/makefiles/Makefile_appfpga`: copy and replace from this template

    diff: removes fpga design build variables

  - :code:`GCC_Project/makefiles/Makefile_appdir`: copy and replace from this template

    diff: removes unnecessary source file filters for m4-code

  For any other diffs, use your discretion.

- copy the :code:`.scaffolding` directory into the root of the existing project

- [VS Code] copy the :code:`.vscode` directory into the root of the existing project

  delete any existing :code:`settings.json` file in the :code:`.vscode` directory

  copy the :code:`settings.template.jsonc` as :code:`settings.json`, adjust the values in the file according to the 
  instructions at the top of the file.


Usage
------

Once the fpga design and m4 code is loaded and running 
(load using debugger/reset after flashing on the board) :

- you should see the red LED toggling periodically (fpga design)

- you should see a couple of I2C transactions, and a banner and CLI on the EOS-S3 UART at IO_44/IO_45 (m4 code)


How To
------

Command Line Usage
~~~~~~~~~~~~~~~~~~

:code:`note: all the commands below are run from the root of this directory.`

Initialize Environment
**********************

Before clean/build/load/flash, ensure that the bash environment is setup by doing the below:

1. Ensure that QORC-SDK is initialized and ready:

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

- Load and run the code/design on the board using JLinkExe, using:

  (assumes the board has been booted in DEBUG mode)

  ::
      
    make load-jlink

- Load and run the code/design on the board using OpenOCD, using:

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

- Flash and run the code/design on the board using qfprog:
  
  (assumes the board is put into :code:`programming` mode)

  ::

    export QORC_PORT=/path/to/serial/port   # needs to be done only once in current shell
    make flash

  Set the serial port as applicable, this is generally :code:`export QORC_PORT=/dev/ttyACM0`


VS Code Usage
~~~~~~~~~~~~~

Dependencies
************

- | VS Code Extension: :code:`ms-vscode.cpptools`
  | link: https://marketplace.visualstudio.com/items?itemName=ms-vscode.cpptools
  | why: C/C++ Intellisense, Debugging
  |

- | VS Code Extension: :code: `marus25.cortex-debug`
  | link: https://marketplace.visualstudio.com/items?itemName=marus25.cortex-debug
  | why: Cortex-M Debug Launch Configuration
  |

- | VS Code Extension: :code:`augustocdias.tasks-shell-input`
  | link: https://marketplace.visualstudio.com/items?itemName=augustocdias.tasks-shell-input
  | why: Scan serial-ports for :code:`flash` task, Select FPGA '.openocd' file for :code:`Debug (OpenOCD)` debug launch config
  |


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
   
   - To be able to run the 'flash' task or 'Debug (OpenOCD)' launch config, remember to install the extension: :code:`augustocdias.tasks-shell-input`
     
   - To be able to 'debug' the code with gdb, remember to install the extension: :code:`marus25.cortex-debug`

   On opening the folder, VS Code should prompt to install these "recommended extensions", if not already installed, 
   select :code:`Install All` to automatically install them.


Clean/Build/Load/Flash (VS Code)
********************************

Any "task" can be run in VS Code using the :code:`Terminal > Run Task` menu, which shows a drop down list of tasks

-OR-

Using keyboard shortcuts: :code:`ctrl+p` and then type :code:`task<space>`, which shows a drop down list of tasks

- Clean using:
  
  - fpga: run the :code:`clean-fpga` task
  - m4: run the :code:`clean-m4` task
  - both: run the :code:`clean` task

- Build using:

  - fpga: run the :code:`build-fpga` task
  - m4: run the :code:`build-m4` task
  - both: run the :code:`build` task

- Load and run the code/design on the board using JLinkExe, using:
  
  (assumes the board has been booted in DEBUG mode)

  run the :code:`load (JLink)` task

- Load and run the code/design on the board using OpenOCD, using:

  (assumes the board has been booted in DEBUG mode)

  run the :code:`load (OpenOCD)` task

  This will show a drop down menu with the options of debug adapters currently tested:

  - JLink Adapters :code:`.scaffolding/jlink_swd.cfg`
  - FT2232H Boards :code:`.scaffolding/ft2232h_swd.cfg`
  - STLinkv2 Adapters :code:`interface/stlink-v2.cfg`
  - DAPLink Adapters :code:`interface/cmsis-dap.cfg`

  select the appropriate one.

- Flash and run the code/design on the board using qfprog:

  (assumes the board is put into :code:`programming` mode)

  run the :code:`flash` task

  This will show a 'pickstring' drop down menu with the available serial ports in the system, select the appropriate one.
  
  (This is usually :code:`/dev/ttyACM0`)

- :code:`debug-load-fpga (JLink)` : This is a special task required only while debugging the code with JLink.

  Refer to the Debug sections for details.

- :code:`x-get-ports` : this is an **internal** task, which is used by the :code:`flash` task to obtain a list of
  available serial ports on the system to use for flashing. This list is displayed to the user as a 'pickstring'
  dropdown menu, as described in the :code:`flash` task above.


Debug
*****

- Debug the code via JLink :

  1. To bring up the :code:`Run and Debug` view, select the Run icon in the Activity Bar on the side of VS Code.
  
  2. Select :code:`Debug (JLink)` from the drop down at the top of the side bar
  
  3. Start Debugging by clicking the green :code:`Play Button`
  
  4. The code should load and break at :code:`main()`
  
  5. Run the :code:`load-fpga-debug (JLink)` task at this point, to load the FPGA design
  
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
