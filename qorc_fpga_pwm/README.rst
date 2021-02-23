ONION FPGA PWM Module Example
=============================

Intro
-----

We create a simple pwm module, and instantiate 3 blocks from it, to control the RGB LEDs 
of the QuickFeather.

Once flashed and board is reset, we can see the RGB LED glow in magenta (RED+BLUE) with 3 steps of 
brightness, followed by yellow(RED+GREEN) with 3 steps of brightness, each step lasting for 
approximately 4 seconds.

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

- Navigate to the the application directory in :code:`qorc-sdk/qorc-onion-apps/qorc_fpga_pwm`

  ::

    cd PATH/TO/qorc-sdk
    cd qorc-onion-apps/qorc_fpga_pwm

- Build the FPGA design (from this project root directory) using:
  
  ::
  
    ql_symbiflow -compile -src fpga/rtl -d ql-eos-s3 -t AL4S3B_FPGA_Top -v AL4S3B_FPGA_Top.v ONION_PWM.v -p quickfeather.pcf -P PU64 -dump binary
   
  The fpga binary will be generated as : :code:`fpga/rtl/AL4S3B_FPGA_Top.bin`

- Put the QuickFeather in "Flash Mode", and Flash the fpga binary (from the project root directory) 
  using:
  
  ::
    
    qfprog --port /dev/ttyACM0 --appfpga fpga/rtl/AL4S3B_FPGA_Top.bin --mode fpga --reset

- The fpga binary should get flashed, and the board should get automatically reset, and the bootloader 
  will start running this application.

  You should see the brightness/color changes in the RGB LED on the QuickFeather as in the Intro above.
