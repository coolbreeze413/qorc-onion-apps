#!/bin/bash

# customize loading the fpga design (.openocd) with BEFORE and AFTER steps as needed,
# as well as being able to directly run the loading process from OpenOCD itself rather
# than using another interface such as telnet/gdb
# REQ:
# 1. OpenOCD probe is connected to EOS_S3 and it is in DEBUG mode
# 2. openocd is available in the path ('source debugenvsetup.sh')
# 3. (.openocd) has been generated by adding a 'openocd' to the -dump command to ql_symbiflow

# fallback(s) to assuming it is on the path
OPENOCDEXE="openocd"
OPENOCD_INTERACE_CFG="interface/jlink_swd.cfg"

# if argument is passed to this script, then it must to path to openocd
# $1 == path to openocd
if [ $# == 2 ] ; then
    OPENOCDEXE="$1"
    OPENOCD_INTERACE_CFG="$2"
fi

# the generated .openocd script only focusses on loading the design and setting the IOMUX
#  registers for the design - it does not do anything related to initialization of the EOS_S3
# typically, we need to ensure that the EOS_S3 has been reset before loading the design.

# also, the script is generated with the expectation that it will be run from the gdb interface
# so the target is already defined while running, for example, the 'mww' commands.
# To run this directly, the target must be specified - in our case, the target is 'eos_s3.cpu', as 
# defined in the scripts/targets/eos_s3.cfg in openocd 

# we will customize a new openocd script on the fly and then use it.
# we can add steps BEFORE and AFTER the load-fpga-design script
# we will also add the '_TARGETNAME' in front of all commands that need it.

CUSTOM_OPENOCD_SCRIPT="custom_eoss3.openocd"
CUSTOM_OPENOCD_SCRIPT_LOG="custom_eoss3.openocdlog"

# write "NOTHING" into file, i.e. reset the contents, faster than delete + touch.
#https://askubuntu.com/a/549672
: > "$CUSTOM_OPENOCD_SCRIPT"

# BEFORE
echo "init" >> "$CUSTOM_OPENOCD_SCRIPT"
echo "reset halt" >> "$CUSTOM_OPENOCD_SCRIPT"
echo 'echo "_CHIPNAME=$_CHIPNAME"' >> "$CUSTOM_OPENOCD_SCRIPT"
echo 'echo "_TARGETNAME=$_TARGETNAME"' >> "$CUSTOM_OPENOCD_SCRIPT"
echo 'echo "_CPUTAPID=$_CPUTAPID"' >> "$CUSTOM_OPENOCD_SCRIPT"
echo ""  >> "$CUSTOM_OPENOCD_SCRIPT"
echo ""  >> "$CUSTOM_OPENOCD_SCRIPT"


# fpga-design (replace 'mww' with 'eos_s3.cpu mww') : TODO come back to make it more generic.
# https://stackoverflow.com/a/1521498/3379867
while IFS="" read -r FILE_LINE || [ -n "$FILE_LINE" ] ; do

    #echo ""
    #echo "$FILE_LINE"
    FILE_LINE=${FILE_LINE/"mww"/"eos_s3.cpu mww"}
    #echo "$FILE_LINE"
    #echo ""

    echo "$FILE_LINE" >> "$CUSTOM_OPENOCD_SCRIPT"

done < fpga/rtl/*.openocd


# AFTER
echo "" >> "$CUSTOM_OPENOCD_SCRIPT"
echo "" >> "$CUSTOM_OPENOCD_SCRIPT"
echo "load_bitstream" >> "$CUSTOM_OPENOCD_SCRIPT"
echo "eos_s3.cpu mdw 0x40005484" >> "$CUSTOM_OPENOCD_SCRIPT"
echo "shutdown" >> "$CUSTOM_OPENOCD_SCRIPT"


# run the custom script now
"$OPENOCDEXE" -f "$OPENOCD_INTERACE_CFG" -f target/eos_s3.cfg -f "${PWD}/${CUSTOM_OPENOCD_SCRIPT}" 2>&1 | tee "$CUSTOM_OPENOCD_SCRIPT_LOG"


# remove the custom script/log if needed
#rm "$CUSTOM_OPENOCD_SCRIPT"
#rm "$CUSTOM_OPENOCD_SCRIPT_LOG"
