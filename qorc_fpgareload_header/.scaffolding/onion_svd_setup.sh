#!/bin/bash

# purpose:
# get the eos_s3 svd file into QORC_SDK_PATH/arm_toolchain_install/eos_s3.svd

# usage: source onion_svd_setup.sh (get svd file if not already present)
# usage: source onion_svd_setup.sh force (to force getting latest svd file from repo)

# this script should be sourced, not run, so that all commands run in current shell process.
if [ "${BASH_SOURCE[0]}" -ef "$0" ]
then
    printf "This script should be sourced, not executed!\n"
    exit 1
fi

# check if QORC SDK env is setup correctly
if [ ! -d "$QORC_SDK_PATH" ]; then
    printf "\nERROR: Looks like QORC SDK env is not setup yet.\n"
    printf "source envsetup.sh has not been run?\n\n"
    return 1
fi

# save dir path from where the 'source' is being executed
CURRENT_DIR=$(pwd)

# check if force flag is set, then we download the svd file again, even if it is already present.
FORCE=0
if [ $# -eq 1 ] && [ "$1" == "force" ] ; then
    FORCE=1
fi


# SVD for the EOS_S3
SVD_URL="https://raw.githubusercontent.com/Josfemova/eos-s3-rs/main/svd/eos-s3.svd"
# switch to our fork, in case the svd file in the original repo does not have needed updates
#SVD_URL="wget https://raw.githubusercontent.com/coolbreeze413/eos-s3-rs/main/svd/eos-s3.svd"
SVD_FILE="eos_s3.svd"
# we will keep the svd file in the arm toolchain base dir
ARM_TOOLCHAIN_INSTALL_BASE_DIR="${QORC_SDK_PATH}/arm_toolchain_install"
SVD_INSTALL_DIR="${ARM_TOOLCHAIN_INSTALL_BASE_DIR}"


#---------------------------------------------------------
# SVD
#---------------------------------------------------------
printf "\n[1] check svd\n"

# if forced, else, if svd file does not already exist, then grab the svd file
if [ $FORCE -eq 1 ] || [ ! -f "${SVD_INSTALL_DIR}/${SVD_FILE}" ] ; then
    printf "    downloading EOS_S3 SVD to ${SVD_INSTALL_DIR}/${SVD_FILE}\n"
    wget -O "${SVD_INSTALL_DIR}/${SVD_FILE}" -q --show-progress --progress=bar:force 2>&1 "$SVD_URL"
fi

printf "    ok.\n"

return 0
