#!/bin/bash

# purpose:
# install OpenOCD into QORC_SDK_PATH/openocd_install
# install JLink into QORC_SDK_PATH/jlink_install
# add to path, check if install ok for both (optional, enabled for now)

# this script should be sourced, not run, so that all commands run in current shell process.
if [ "${BASH_SOURCE[0]}" -ef "$0" ]
then
    printf "This script should be sourced, not executed!\n"
    exit 1
fi

SCRIPT_DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )
#printf "\nSCRIPT_DIR=$SCRIPT_DIR\n\n"


# this section is as long as this is a standalone script separate from the envsetup.sh
#printf "$#"
if [ "$#" != "1" ] ; then

    printf "\n\nERROR: please provide the path to the QORC SDK installation dir as an argument!\n\n"
    return 1

fi


if [ ! -d "$1" ] ; then

    printf "\n\nERROR:path specified for QORC_SDK does not exist: %s\n"${QORC_SDK_PATH}
    return 1

fi

# convert to absolute path
QORC_SDK_PATH=$(cd "$1" && pwd)

QORC_SDK_DEBUGENVSETUP_VER=1.5.1

# OpenOCD: https://github.com/xpack-dev-tools/openocd-xpack/releases/ -> get the linux-x64 archive from this page
# also, some systems may need udev rules depending on debug probe used, put into /etc/udev/rules -> this is upto the end user.
OPENOCD_ARCHIVE_URL="https://github.com/xpack-dev-tools/openocd-xpack/releases/download/v0.11.0-3/xpack-openocd-0.11.0-3-linux-x64.tar.gz"
OPENOCD_ARCHIVE_FILE="xpack-openocd-0.11.0-3-linux-x64.tar.gz"
OPENOCD_INSTALL_BASE_DIR="${QORC_SDK_PATH}/openocd_install"
OPENOCD_INSTALL_DIR="${OPENOCD_INSTALL_BASE_DIR}/xpack-openocd-0.11.0-3"
EXPECTED_OPENOCD_PATH="${OPENOCD_INSTALL_DIR}/bin/openocd"
# additionally, the custom openocd cfg files as needed:
# we are not doing this as we can keep the cfg files in the .scaffolding dir for the project
# this is much clearer than injecting files into a third-party installation, unless it was necessary
#OPENOCD_CFG_FILE_LIST=("ft2232h_swd.cfg" "jlink_swd.cfg")

# JLink: https://www.segger.com/downloads/jlink/ -> get the Linux/'64-bit TGZ Archive' from this page
# note that there is a EULA which needs a POST request, as described here: https://github.com/ScoopInstaller/Scoop/issues/4336
# also, some systems may need the udev rules from the JLink archive put into /etc/udev/rules -> this is upto the end user.
JLINK_ARCHIVE_URL="https://www.segger.com/downloads/jlink/JLink_Linux_V760d_x86_64.tgz"
JLINK_ARCHIVE_FILE="JLink_Linux_V760d_x86_64.tgz"
JLINK_INSTALL_BASE_DIR="${QORC_SDK_PATH}/jlink_install"
JLINK_INSTALL_DIR="${JLINK_INSTALL_BASE_DIR}/JLink_Linux_V760d_x86_64"
EXPECTED_JLINK_GDBSERVER_PATH="${JLINK_INSTALL_DIR}/JLinkGDBServerCLExe"

# SVD for the EOS_S3
SVD_URL="https://raw.githubusercontent.com/Josfemova/eos-s3-rs/main/svd/eos-s3.svd"
# switch to our fork, in case the svd file in the original repo does not have needed updates
#SVD_URL="wget https://raw.githubusercontent.com/coolbreeze413/eos-s3-rs/main/svd/eos-s3.svd"
SVD_FILE="eos_s3.svd"
SVD_INSTALL_DIR="${QORC_SDK_PATH}"

printf "\n\n=========================\n"
printf "qorc-sdk debugenvsetup %s\n" ${QORC_SDK_ENVSETUP_VER}
printf "=========================\n\n\n"

printf "executing debugenvsetup.sh from:\n%s\n" ${PWD}
printf "\nQORC SDK PATH specified: %s\n" ${QORC_SDK_PATH}


#---------------------------------------------------------
# OpenOCD
#---------------------------------------------------------
printf "\n[1] check openocd\n"
if [ ! -d "$OPENOCD_INSTALL_DIR" ]; then

    printf "    creating openocd directory : %s\n" "${OPENOCD_INSTALL_BASE_DIR}"
    mkdir "${OPENOCD_INSTALL_BASE_DIR}"

    if [ ! -f "$OPENOCD_ARCHIVE_FILE" ]; then

        printf "    downloading openocd archive.\n"
        wget -O "$OPENOCD_ARCHIVE_FILE" -q --show-progress --progress=bar:force 2>&1 "$OPENOCD_ARCHIVE_URL"

    fi

    printf "    extracting openocd archive.\n"
    tar -xf "$OPENOCD_ARCHIVE_FILE" -C "${OPENOCD_INSTALL_BASE_DIR}"

    rm -rf "$OPENOCD_ARCHIVE_FILE"

    if [ ! -f "$EXPECTED_OPENOCD_PATH" ]; then
        printf "\n    ERROR: openocd installation problem detected!\n"
        return 1
    else
        printf "    openocd installation looks good\n"
    fi

    # we are not doing this anymore, see note near OPENOCD_CFG_FILE_LIST declaration
    # copy the additional OpenOCD interface cfg files as needed
    # for OPENOCD_CFG_FILE in "${OPENOCD_CFG_FILE_LIST[@]}"
    # do
    #     cp -v "${OPENOCD_CFG_FILE}" "${OPENOCD_INSTALL_DIR}/scripts/interface"
    # done


fi

# add OpenOCD to path (not needed for using with our VSCode configuration, but useful for commandline usage)
export PATH="${OPENOCD_INSTALL_DIR}/bin:$PATH"
ACTUAL_OPENOCD_GDBSERVER_PATH=`which openocd`
if [ $ACTUAL_OPENOCD_GDBSERVER_PATH == $EXPECTED_OPENOCD_PATH ]; then

    printf "    ok.\n"

else

    printf "    !!openocd path not as expected, install/permission problems?!!\n"
    printf "    expected: %s\n" ${EXPECTED_OPENOCD_PATH}
    printf "         got: %s\n" ${ACTUAL_OPENOCD_GDBSERVER_PATH}
    return

fi



#---------------------------------------------------------
# JLink
#---------------------------------------------------------
printf "\n[2] check jlink\n"
if [ ! -d "$JLINK_INSTALL_DIR" ]; then

    printf "    creating jlink directory : %s\n" "${JLINK_INSTALL_BASE_DIR}"
    mkdir "${JLINK_INSTALL_BASE_DIR}"

    if [ ! -f "$JLINK_ARCHIVE_FILE" ]; then

        printf "    downloading jlink archive.\n"
        # https://github.com/ScoopInstaller/Scoop/issues/4336
        wget --post-data "accept_license_agreement=accepted" -O "$JLINK_ARCHIVE_FILE" -q --show-progress --progress=bar:force 2>&1 "$JLINK_ARCHIVE_URL"

    fi

    printf "    extracting jlink archive.\n"
    tar -xf "$JLINK_ARCHIVE_FILE" -C "${JLINK_INSTALL_BASE_DIR}"

    rm -rf "$JLINK_ARCHIVE_FILE"

    if [ ! -f "$EXPECTED_JLINK_GDBSERVER_PATH" ]; then
        printf "\n    ERROR: jlink installation problem detected!\n"
    else
        printf "    jlink installation looks good\n"
    fi

fi

# add JLink to path (not needed for using with our VSCode configuration, but useful for commandline usage)
export PATH="${JLINK_INSTALL_DIR}:$PATH"
ACTUAL_JLINK_GDBSERVER_PATH=`which JLinkGDBServerCLExe`
if [ $ACTUAL_JLINK_GDBSERVER_PATH == $EXPECTED_JLINK_GDBSERVER_PATH ]; then

    printf "    ok.\n"

else

    printf "    !!jlink path not as expected, install/permission problems?!!\n"
    printf "    expected: %s\n" ${EXPECTED_JLINK_GDBSERVER_PATH}
    printf "         got: %s\n" ${ACTUAL_JLINK_GDBSERVER_PATH}
    return

fi


#---------------------------------------------------------
# SVD
#---------------------------------------------------------
# we will always download latest svd from github repo currently.
# TODO: check later if we want to remove this (maybe once svd is stabilized?)
printf "\n[3] check svd\n"

if [ -f "${SVD_FILE}" ]; then
    rm "${SVD_FILE}"
fi

printf "    downloading EOS_S3 SVD.\n"
wget -O "$SVD_FILE" -q --show-progress --progress=bar:force 2>&1 "$SVD_URL"

if [ -f "${SVD_INSTALL_DIR}/${SVD_FILE}" ]; then
    rm "${SVD_INSTALL_DIR}/${SVD_FILE}"
fi
cp -v "$SVD_FILE" "$SVD_INSTALL_DIR"
rm "$SVD_FILE"

printf "    ok.\n"

return 0
