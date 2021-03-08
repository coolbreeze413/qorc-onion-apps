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

Prerequisites
-------------

- Ensure that you have cloned the latest qorc-sdk, and you are able to build basic fpga/m4 
  application in :code:`qorc-sdk/qf_apps/qf_helloworldhw` to ensure that your (ARM GCC/SymbiFlow)toolchain setup 
  is working correctly, and setup according to the qorc-sdk instructions.

  https://qorc-sdk.readthedocs.io/en/latest/qorc-setup/qorc-setup.html



- Ensure that the bootloader in the QuickFeather is updated to the latest available from 
  the qorc-sdk repository.

  This is also covered in the setup steps above, the specific link is:
  https://qorc-sdk.readthedocs.io/en/latest/qorc-setup/qorc-setup.html#bootloader-update

- Clone the qorc-onion-apps inside the qorc-sdk directory:
  
  ::

    cd PATH/TO/qorc-sdk
    git clone https://github.com/coolbreeze413/qorc-onion-apps

  This will create :code:`qorc-onion-apps` at the same level as qf_apps in qorc-sdk directory.


How To
------

- Navigate to the the application directory in :code:`qorc-sdk/qorc-onion-apps/qorc_fpga_breathe`

  ::

    cd PATH/TO/qorc-sdk
    cd qorc-onion-apps/qorc_fpga_breathe

- Build the FPGA design (from this project root directory) using:
  
  ::
  
    ql_symbiflow -compile -src fpga/rtl -d ql-eos-s3 -t AL4S3B_FPGA_Top -v AL4S3B_FPGA_Top.v ONION_BREATHE.v -p quickfeather.pcf -P PU64 -dump binary
   
  The fpga binary will be generated as : :code:`fpga/rtl/AL4S3B_FPGA_Top.bin`

- Put the QuickFeather in "Flash Mode", and Flash the fpga binary (from the project root directory) 
  using:
  
  ::
    
    qfprog --port /dev/ttyACM0 --appfpga fpga/rtl/AL4S3B_FPGA_Top.bin --mode fpga --reset

- The fpga binary should get flashed, and the board should get automatically reset, and the bootloader 
  will start running this application.

  You should see the "breathing-effect" glow of the RGB LED on the QuickFeather as in the Intro above.
