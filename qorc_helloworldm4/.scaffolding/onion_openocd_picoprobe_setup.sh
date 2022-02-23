#!/bin/bash

# purpose:
# (first time) install OpenOCD(picoprobe) into QORC_SDK_PATH/openocd_install/openocd-picoprobe
# add to path, check if install ok

# usage: source onion_openocd_picoprobe_setup.sh

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

# note: some systems may need udev rules depending on debug probe used, put into /etc/udev/rules -> this is upto the end user.
OPENOCD_INSTALL_BASE_DIR="${QORC_SDK_PATH}/openocd_install"
OPENOCD_PICOPROBE_REPO="https://github.com/raspberrypi/openocd.git"
OPENOCD_PICOPROBE_REPO_DIR="${OPENOCD_INSTALL_BASE_DIR}/openocd-picoprobe-repo"
OPENOCD_PICOPROBE_INSTALL_DIR="${OPENOCD_INSTALL_BASE_DIR}/openocd-picoprobe"
EXPECTED_OPENOCD_PICOPROBE_PATH="${OPENOCD_PICOPROBE_INSTALL_DIR}/bin/openocd"


#---------------------------------------------------------
# OpenOCD (picoprobe)
#---------------------------------------------------------
printf "\n[1] check openocd (picoprobe)\n"
# first time : install dir will not exist
if [ ! -d "$OPENOCD_PICOPROBE_INSTALL_DIR" ]; then

    # 1. check pre-requisite dependencies for openocd-picoprobe
    
    # refs:
    # https://www.digikey.in/en/maker/projects/raspberry-pi-pico-and-rp2040-cc-part-2-debugging-with-vs-code/470abc7efb07432b82c95f6f67f184c0
    # https://shawnhymel.com/2168/how-to-build-openocd-and-picotool-for-the-raspberry-pi-pico-on-windows/#Build_OpenOCD


    # pre-requisites:
    # sudo apt install automake autoconf build-essential texinfo libtool libftdi-dev libusb-1.0-0-dev

    # 2 ways to check if package installed, in debian/ubuntu:
    # dpkg -s automake 2>/dev/null | grep Status | grep installed > /dev/null
    # $? == 0 installed, else not.
    # -OR-
    # apt list -qq libusb-1.0-0-dev --installed 2>/dev/null | grep installed > /dev/null
    # $? == 0 installed, else not.
    # apt list also works, but is way slower than dpkg, and dpkg is the 
    #preferred way on Debian based systems

    PICOPROBE_OPENOCD_DEPENDENCY_LIST=(automake autoconf build-essential texinfo libtool libftdi-dev libusb-1.0-0-dev)
    INSTALL_STATUS=0
    MISSING_DEPENDENCY_LIST=()
    for item in "${PICOPROBE_OPENOCD_DEPENDENCY_LIST[@]}" ; do 
        #echo "$item"
        dpkg -s "$item" 2>/dev/null | grep Status | grep installed > /dev/null
        #apt list -qq "$item" --installed 2>/dev/null | grep installed > /dev/null
        INSTALL_STATUS=$?
        if [ ! $INSTALL_STATUS -eq 0 ] ; then
            MISSING_DEPENDENCY_LIST+=("$item")
        fi
    done

    # check if any dependencies are missing:
    if [ ! ${#MISSING_DEPENDENCY_LIST[@]} -eq 0 ] ; then
        printf "\nERROR: please ensure that all of the dependencies are installed first!\n\n"
        for item in "${MISSING_DEPENDENCY_LIST[@]}"
            do
            printf "[MISSING DEPENDENCY] $item\n"
            done
        printf "\n"
        return 1
    fi


    # 2. obtain and build openocd-picoprobe
    printf "\n\n"
    printf "    clone repo: [%s]\n" "${OPENOCD_PICOPROBE_REPO}"
    printf "          into: [%s]\n" "${OPENOCD_PICOPROBE_REPO_DIR}"

    git clone "${OPENOCD_PICOPROBE_REPO}" --branch picoprobe --depth=1 "${OPENOCD_PICOPROBE_REPO_DIR}"

    printf "\n\n"
    printf "    configure, build, install to: [%s]\n" "${OPENOCD_PICOPROBE_INSTALL_DIR}"

    cd "${OPENOCD_PICOPROBE_REPO_DIR}"

    ./bootstrap
    ./configure --enable-picoprobe --prefix "$OPENOCD_PICOPROBE_INSTALL_DIR"
    make -j`nproc`
    make install

    cd - > /dev/null

    # cleanup the repo dir after build
    rm -rf "${OPENOCD_PICOPROBE_REPO_DIR}"

    if [ ! -f "$EXPECTED_OPENOCD_PICOPROBE_PATH" ]; then
        printf "\n    ERROR: openocd installation problem detected!\n"
        return 1
    else
        printf "    openocd installation looks good\n"
    fi

fi

# add OpenOCD to path
export PATH="${OPENOCD_PICOPROBE_INSTALL_DIR}/bin:$PATH"
ACTUAL_OPENOCD_PICOPROBE_PATH=`which openocd`
if [ $ACTUAL_OPENOCD_PICOPROBE_PATH == $EXPECTED_OPENOCD_PICOPROBE_PATH ]; then

    printf "    ok.\n"

else

    printf "    !!openocd path not as expected, install/permission problems?!!\n"
    printf "    expected: %s\n" ${EXPECTED_OPENOCD_PICOPROBE_PATH}
    printf "         got: %s\n" ${ACTUAL_OPENOCD_PICOPROBE_PATH}
    return 1

fi


return 0
