QuickFeather HelloWorldM4 Application
=====================================

This test/example contains the :code:`qf_helloworldm4` example, which is derived from the :code:`qf_helloworldsw` example.
This is a simple M4-only application, which uses the EOS S3 UART to expose a command line interface (CLI).


How To
------
- Setup:

  - | Update the programmer to the latest from `TinyFPGAProgrammer-Application <https://github.com/QuickLogic-Corp/TinyFPGA-Programmer-Application>`_ :code:`master`.
    |
    | This has the required changes to support independent m4app, independent appfpga and m4app+appfpga images.
    | Refer to `TinyFPGAProgrammer-Application <https://github.com/QuickLogic-Corp/TinyFPGA-Programmer-Application>`_ for details on changes.
    |

  - | Update the bootloader to the latest from `qorc-sdk <https://github.com/QuickLogic-Corp/qorc-sdk>`_ :code:`master`, if not already done.
    
    - | Clone the repo, or update it to latest using:
      | :code:`git checkout master && git pull`
    
    - Build the :code:`qf_bootloader` app as usual using :code:`make` from :code:`qf_apps/qf_bootloader/GCC_Project` directory
     
    - | Flash the built bootloader bin using:
      | :code:`qfprog --bootloader output/bin/qf_bootloader.bin --mode fpga-m4` from :code:`qf_apps/qf_bootloader/GCC_Project` directory


- Build:

  - Build this application as usual using :code:`make` from the :code:`GCC_Project` directory

  - The M4 binary is generated in :code:`GCC_Project/output/bin`

- Flash:

  - Set the board in programming mode.

  - | Flash the m4 binary using (from the :code:`GCC_Project` directory)
    
    ::

      qfprog --port /dev/ttyACM0 --m4app output/bin/qf_helloworldm4.bin --mode m4

    | Note the :code:`--mode` option at the end, which is now mandatory - this specifies the operating mode which the bootloader uses.
    | :code:`--mode m4` ensures that the bootloader knows that only the m4app binary is flashed, and it will load the flashed m4app binary only.
    |

- Run:

  - | Ensure that a (3.3V) USB-UART cable is connected to the EOS S3 UART pins.
    | For QuickFeather, refer to `quick-feather-dev-board <https://github.com/QuickLogic-Corp/quick-feather-dev-board#advanced>`_ for details.
    | Connect a Serial Terminal app to the USB port of the USB-UART cable (most likely a ttyUSBx device) at 115200 baud and 8N1 configuration.

  - Reset the board, the m4app should get loaded by the bootloader and start running.

  - You should see a banner similar to below on the EOS S3 UART:  
  
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

    - Keep the USR button pressed (connected to IO_6 on QuickFeather) and execute: :code:`diag userbutton` to check state - it should show :code:`TODO fill the ouput here`
    - Without the USR button pressed, :code:`diag userbutton` should show: :code:`TODO fill the output here`
    
