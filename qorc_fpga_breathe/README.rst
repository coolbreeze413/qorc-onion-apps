ONION FPGA BREATHE Module Example
=================================

We create a simple module to realize a "breathing effect", and instantiate 3 blocks from it, to control the RGB LEDs 
of the QuickFeather.

Using varying pwm, we control brightness from 0 (off) to max and back to 0 - creating inhale 
and exhale effects.

Once flashed and board is reset, we can see the RGB LED glow with a breathing effect, with 
the following color sequence:

- blue
- green
- red
- yellow
- magenta
- cyan
- [repeats from blue]


How To
------

- | Build the FPGA design using :
  | :code:`ql_symbiflow -compile -src fpga/rtl -d ql-eos-s3 -t AL4S3B_FPGA_Top -v AL4S3B_FPGA_Top.v ONION_BREATHE.v -p quickfeather.pcf -P PU64 -dump binary`
  | from this project's root directory
  | The binary will be generated as : :code:`fpga/rtl/AL4S3B_FPGA_Top.bin`


- | Flash the fpga binary using (from the application root directory)
  | :code:`qfprog --port /dev/ttyACM0 --appfpga fpga/rtl/AL4S3B_FPGA_Top.bin --mode fpga --reset`
