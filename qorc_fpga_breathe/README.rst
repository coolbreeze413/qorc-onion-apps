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

:code:`note: all the commands below are run from the root of this directory.`

Initialize Environment
**********************

Before clean/build/load/flash, ensure that the bash environment is setup by doing the below:

1. Ensure that QORC-SDK is initialized and ready:

   .. code-block:: bash

     source ../../envsetup.sh

   .. code-block:: none

     note: assumes this repo 'qorc-onion-apps' is cloned into the QORC SDK dir, at the same level as qf_apps

   In general, use:

   .. code-block:: none

     source <QORC_SDK_PATH>/envsetup.sh

2. [Only] If you are using the RP2040/Pico as a debugger (picoprobe), then ensure that the RaspberryPi
   flavor of OpenOCD is initialized and ready:

   .. code-block:: bash

     source .scaffolding/onion_openocd_picoprobe_setup.sh

3. If you want to use the SVD file for EOS-S3 to watch registers while debugging, then ensure that
   the SVD file is initialized and ready:

   .. code-block:: bash

     source .scaffolding/onion_svd_setup.sh

   .. code-block:: none

     note: the SVD file is in very early alpha and missing many registers, use with discretion!

   To force download of latest SVD file from repo and replace the existing one, use:

   .. code-block:: bash

     source .scaffolding/onion_svd_setup.sh force


Clean/Build/Load/Flash (Command Line)
*************************************

- Clean using:

  fpga: :code:`make clean` -or- :code:`make clean-fpga`

- Build using:

  fpga: :code:`make` -or- :code:`make fpga`

- Load and run the design on the board using JLinkExe, using:

  (assumes the board has been booted in DEBUG mode)

  .. code-block:: bash
      
    make load-jlink

- Load and run the design on the board using OpenOCD, using:

  (assumes the board has been booted in DEBUG mode)

  .. code-block:: bash

    export QORC_OCD_IF_CFG=/path/to/inteface/cfg    # needs to be done only once in the current shell
    make load-openocd

  The interface cfg file depends on the debug adapter chosen.

  Here are a few common adapters that can be used with the EOS_S3:
  
  1. JLink Adapters: :code:`export QORC_OCD_IF_CFG=.scaffolding/jlink_swd.cfg` (available in the current dir)
  2. FT2232H Boards: :code:`export QORC_OCD_IF_CFG=.scaffolding/ft2232h_swd.cfg` (available in the current dir)
  3. STLinkv2 Adapters: :code:`export QORC_OCD_IF_CFG=interface/stlink-v2.cfg` (available in the OpenOCD install scripts dir)
  4. DAPLink Adapters: :code:`export QORC_OCD_IF_CFG=interface/cmsis-dap.cfg` (available in the OpenOCD install scripts dir)
  5. picoprobe: :code:`export QORC_OCD_IF_CFG=interface/picoprobe.cfg` (available in the OpenOCD install scripts dir, only if RaspberryPi flavor OpenOCD has been initialized)

  Practically, any adapter that supports OpenOCD and SWD can be used with the appropriate cfg file passed in.

- Flash and run the design on the board using qfprog:
  
  (assumes the board is put into :code:`programming` mode)

  .. code-block:: bash

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

- | VS Code Extension: :code:`marus25.cortex-debug`
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

   .. code-block:: none

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

- Build using:

  - fpga: run the :code:`build-fpga` task

- Load and run the design on the board using JLinkExe, using:
  
  (assumes the board has been booted in DEBUG mode)

  run the :code:`load (JLink)` task

- Load and run the design on the board using OpenOCD, using:

  (assumes the board has been booted in DEBUG mode)

  run the :code:`load (OpenOCD)` task

  This will show a drop down menu with the options of debug adapters currently tested:

  - JLink Adapters :code:`.scaffolding/jlink_swd.cfg`
  - FT2232H Boards :code:`.scaffolding/ft2232h_swd.cfg`
  - STLinkv2 Adapters :code:`interface/stlink-v2.cfg`
  - DAPLink Adapters :code:`interface/cmsis-dap.cfg`

  select the appropriate one.

- Load and run the design on the board using OpenOCD and picoprobe, using:

  (assumes the board has been booted in DEBUG mode)

  run the :code:`load (OpenOCD-picoprobe)` task

- Flash and run the design on the board using qfprog:

  (assumes the board is put into :code:`programming` mode)

  run the :code:`flash` task

  This will show a 'pickstring' drop down menu with the available serial ports in the system, select the appropriate one.
  
  (This is usually :code:`/dev/ttyACM0`)


- :code:`x-get-ports` : this is an **internal** task, which is used by the :code:`flash` task to obtain a list of
  available serial ports on the system to use for flashing. This list is displayed to the user as a 'pickstring'
  dropdown menu, as described in the :code:`flash` task above.


References
~~~~~~~~~~

1. https://code.visualstudio.com/docs/editor/debugging
2. https://marketplace.visualstudio.com/items?itemName=marus25.cortex-debug
3. https://mcuoneclipse.com/2021/05/09/visual-studio-code-for-c-c-with-arm-cortex-m-part-4/
