ONION FPGA PWM Module Example
=============================

This contains the the fpga design available as an independent component.


How To
------

- | Build the FPGA design using :
  | :code:`ql_symbiflow -compile -src fpga/rtl -d ql-eos-s3 -t AL4S3B_FPGA_Top -v AL4S3B_FPGA_Top.v ONION_PWM.v -p quickfeather.pcf -P PU64 -dump binary`
  | from this project's root directory
  | The binary will be generated as : :code:`fpga/rtl/AL4S3B_FPGA_Top.bin`


- | Flash the fpga binary using (from the application root directory)
  | :code:`qfprog --port /dev/ttyACM0 --appfpga fpga/rtl/AL4S3B_FPGA_Top.bin --mode fpga`
