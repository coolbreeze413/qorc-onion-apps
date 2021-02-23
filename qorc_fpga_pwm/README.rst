ONION FPGA PWM Module Example
=============================

We create a simple pwm module, and instantiate 3 blocks from it, to control the RGB LEDs 
of the QuickFeather.

Once flashed and board is reset, we can see the RGB LED glow in magenta with 7 steps of 
brightness, followed by yellow with 7 steps of brightness, each step lasting for 
approximately 1 second.

How To
------

- | Build the FPGA design using :
  | :code:`ql_symbiflow -compile -src fpga/rtl -d ql-eos-s3 -t AL4S3B_FPGA_Top -v AL4S3B_FPGA_Top.v ONION_PWM.v -p quickfeather.pcf -P PU64 -dump binary`
  | from this project's root directory
  | The binary will be generated as : :code:`fpga/rtl/AL4S3B_FPGA_Top.bin`


- | Flash the fpga binary using (from the application root directory)
  | :code:`qfprog --port /dev/ttyACM0 --appfpga fpga/rtl/AL4S3B_FPGA_Top.bin --mode fpga --reset`
