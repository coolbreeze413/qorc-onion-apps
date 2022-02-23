QuickFeather LoadFlash (Dual) Application
==========================================


Usage
-----

The :code:`loadflash` can work with both EOSS3 UART at IO_44/IO_45 and the USB-Serial port at the same time.

Once this application is loaded, we can flash or read the board using either a UART cable connected to the EOSS3 UART
or a USB cable connected to the USB-Serial µUSB port.


This is usually used to prep (or reflash) a new blank board with the EOSS3 and SPI Flash installed, with:

- bootloader
- bootfpga
- m4app
- appfpga

Once the bootloader and bootfpga are flashed, the board can be reflashed anytime by using the bootloader to enter :code:`programming` 
mode.

The flash memory map defined for q-series devices is as below:

.. image:: qorc-flash-memory-map-addresses.svg
    :alt: flash memory map


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

  m4: :code:`make clean` -or- :code:`make clean-m4`

- Build using:

  m4: :code:`make` -or- :code:`make m4`

- Load and run the code on the board using JLinkExe, using:

  (assumes the board has been booted in DEBUG mode)

  .. code-block:: bash

    make load-jlink

- Load and run the code on the board using OpenOCD, using:

  (assumes the board has been booted in DEBUG mode)

  .. code-block:: bash

    export QORC_OCD_IF_CFG=/path/to/inteface/cfg    # needs to be done only once in the current shell
    make load-openocd

  The interface cfg file depends on the debug adapter chosen.

  Here are a few common adapters that can be used with the EOS-S3:
  
  1. JLink Adapters: :code:`export QORC_OCD_IF_CFG=.scaffolding/jlink_swd.cfg` (available in the current dir)
  2. FT2232H Boards: :code:`export QORC_OCD_IF_CFG=.scaffolding/ft2232h_swd.cfg` (available in the current dir)
  3. STLinkv2 Adapters: :code:`export QORC_OCD_IF_CFG=interface/stlink-v2.cfg` (available in the OpenOCD install scripts dir)
  4. DAPLink Adapters: :code:`export QORC_OCD_IF_CFG=interface/cmsis-dap.cfg` (available in the OpenOCD install scripts dir)
  5. picoprobe: :code:`export QORC_OCD_IF_CFG=interface/picoprobe.cfg` (available in the OpenOCD install scripts dir, only if RaspberryPi flavor OpenOCD has been initialized)

  Practically, any adapter that supports OpenOCD and SWD can be used with the appropriate cfg file passed in.

- Initialize the flash using qfprog:
  
  :code:`!! CAUTION !! : This will replace the bootloader, bootfpga, m4app, appfpga on the board! (if any)`

  1. Put the board into :code:`DEBUG` mode, and load the current project using either JLink or OpenOCD as described above.

  2. Once the code is loaded, regular magenta fast flashing should be seen (red + blue leds), at this point, run the below command:

     .. code-block:: bash 
         
         export QORC_PORT=/path/to/serial/port   # needs to be done only once in current shell
         make flash-initialize 

     Set the serial port as applicable, this is generally :code:`export QORC_PORT=/dev/ttyACM0` 
     
     This will further ask for confirmation before flashing the images to the board, input :code:`yes` to confirm. 
     
     This does:
     
     - build `qorc_bootloader_dual <../qorc_bootloader_dual/README.rst>`__ and copy the bootloader binary to `initialize/ <./initialize/>`__
     - copy the USB-Serial fpga binary `qorc_bootfpga.bin <./fpga_usbserial/qorc_bootfpga.bin/>`__ to `initialize/ <./initialize/>`__
     - build the `qorc_helloworldm4fpga <../qorc_helloworldm4fpga/README.rst>`__ and copy the m4app and appfpga binaries to `initialize/ <./initialize/>`__
     - flashes all of the binaries on the board using qfprog
     
     Makefile can be referred to for implementation: `Makefile <./Makefile>`__


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
  | why: Scan serial-ports for :code:`flash-bootloader` task
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
   
   - To be able to run the 'flash-bootloader' task or 'Debug (OpenOCD)' launch config, remember to install the extension: :code:`augustocdias.tasks-shell-input`
     
   - To be able to 'debug' the code with gdb, remember to install the extension: :code:`marus25.cortex-debug`

   On opening the folder, VS Code should prompt to install these "recommended extensions", if not already installed, 
   select :code:`Install All` to automatically install them.


Clean/Build/Load/Flash (VS Code)
********************************

Any "task" can be run in VS Code using the :code:`Terminal > Run Task` menu, which shows a drop down list of tasks

-OR-

Using keyboard shortcuts: :code:`ctrl+p` and then type :code:`task<space>`, which shows a drop down list of tasks

- Clean using:
  
  - m4: run the :code:`clean-m4` task

- Build using:

  - m4: run the :code:`build-m4` task

- Load and run the code on the board using JLinkExe, using:
  
  (assumes the board has been booted in DEBUG mode)

  run the :code:`load (JLink)` task

- Load and run the code on the board using OpenOCD, using:

  (assumes the board has been booted in DEBUG mode)

  run the :code:`load (OpenOCD)` task

  This will show a drop down menu with the options of debug adapters currently tested:

  - JLink Adapters :code:`.scaffolding/jlink_swd.cfg`
  - FT2232H Boards :code:`.scaffolding/ft2232h_swd.cfg`
  - STLinkv2 Adapters :code:`interface/stlink-v2.cfg`
  - DAPLink Adapters :code:`interface/cmsis-dap.cfg`

  select the appropriate one.

- Load and run the code on the board using OpenOCD and picoprobe, using:

  (assumes the board has been booted in DEBUG mode)

  run the :code:`load (OpenOCD-picoprobe)` task

- Initialize the flash using qfprog:
  
  :code:`!! CAUTION !! : This will replace the bootloader, bootfpga, m4app, appfpga on the board! (if any)`

  1. Put the board into :code:`DEBUG` mode, and load the current project using either JLink or OpenOCD task(s) as described above.
     
     Note that, a :code:`Debug` session via JLink/OpenOCD can also be launched, instead of using the :code:`load` tasks.

  2. Once the code is loaded, regular magenta fast flashing should be seen (red + blue leds), at this point, run the :code:`flash-bootloader` task

     This will show a 'pickstring' drop down menu with the available serial ports in the system, select the appropriate one.
  
     (This is usually :code:`/dev/ttyACM0`)

     This will further ask for confirmation before flashing the images to the board, select :code:`yes` to confirm.
     
     This does:
     
     - build `qorc_bootloader_dual <../qorc_bootloader_dual/README.rst>`__ and copy the bootloader binary to `initialize/ <./initialize/>`__
     - copy the USB-Serial fpga binary `qorc_bootfpga.bin <./fpga_usbserial/qorc_bootfpga.bin/>`__ to `initialize/ <./initialize/>`__
     - build the `qorc_helloworldm4fpga <../qorc_helloworldm4fpga/README.rst>`__ and copy the m4app and appfpga binaries to `initialize/ <./initialize/>`__
     - flashes all of the binaries on the board using qfprog
     
     Makefile can be referred to for implementation: `Makefile <./Makefile>`__

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
  
  5. Resume/Continue debugging using the blue :code:`Continue/Break` button at the top or using :code:`F8`


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

  5. The code should load and break at :code:`main()`
  
  6. Resume/Continue debugging using the blue :code:`Continue/Break` button at the top or using :code:`F8`


- Debug the code via OpenOCD and picoprobe :

  1. To bring up the :code:`Run and Debug` view, select the Run icon in the Activity Bar on the side of VS Code.
  
  2. Select :code:`Debug (OpenOCD-picoprobe)` from the drop down at the top of the side bar
  
  3. Start Debugging by clicking the green :code:`Play Button`
  
  4. The code should load and break at :code:`main()`
  
  5. Resume/Continue debugging using the blue :code:`Continue/Break` button at the top or using :code:`F8`


- Common Debugging Steps with the :code:`Cortex-Debug` extension in VS Code:

  1. Place breakpoints in the code by clicking near the line number
  
  2.  Use the :code:`Step Over`, :code:`Step Into`, :code:`Step Out`, :code:`Restart`, :code:`Stop` buttons to control the debugging session


References
~~~~~~~~~~

1. https://code.visualstudio.com/docs/editor/debugging
2. https://marketplace.visualstudio.com/items?itemName=marus25.cortex-debug
3. https://mcuoneclipse.com/2021/05/09/visual-studio-code-for-c-c-with-arm-cortex-m-part-4/
