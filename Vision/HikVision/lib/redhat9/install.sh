#!/bin/sh

# This script will install and erase compression cards' SDK and driver.
#
# NOTE: Run this with root privilege.
# 
# Usage:
#       ./install.sh      to install all library files to /usr/lib
#       ./install.sh -e   to erase all installed library files


# Check root privilege.
if [ $(id -u) != "0" ]; then
	echo "You must be the superuser to run this script" >&2
	exit 1
fi


##### Constants

STATUS=
SRC_DIR=./
DST_DIR=/usr/lib

# The files to be installed and erased.
SO_FILES="libds40xxsdk.so libm4play.so libtmnetc.so libtmnets.so"


##### Functions

function usage
{
	echo "usage: install.sh [[-i] | [-e] | [-h]]"
	echo "parameter:"
	echo "  -i     install all libraries"
	echo "  -e     erase all libraries"
	echo "  -r     do text relocation to workaround SELinux"
	echo "  -h     print this message"

}   # end of usage


function install_all
{
	echo "Installing library files."

	for i in $SO_FILES ; do
		if [ ! -f $SRC_DIR/$i ]; then
			STATUS="No SUCH file"
		else
			cp -f $SRC_DIR/$i $DST_DIR
			if [ -f $DST_DIR/$i ]; then
				STATUS="SUCCESS"
			else
				STATUS="FAILED"
			fi
		fi

		echo "Copy $i to $DST_DIR : $STATUS"
	done

}   # end of install_all


function erase_all
{
	echo "Erasing all installed library files."

	cd $DST_DIR
	for i in $SO_FILES ; do
		echo "Remove $i in $DST_DIR"
		rm -f $i
	done

}   # end of erase_all


function text_reloc
{
	echo "Do text relocation to workaround SELinux."

	cd $DST_DIR
	for i in $SO_FILES ; do
		chcon -t textrel_shlib_t $DST_DIR/$i
	done

}   # end of text_reloc

function system_info
{
	echo "System release info"
	echo "Function not yet implemented"

}   # end of system_info


##### Main

# Install as default
if [ "$1" == "" ]; then
	install_all
	exit
fi

# Parse each parameter and delegate to certain function
while [ "$1" != "" ]; do
    case $1 in
        -e | --erase )          erase_all
                                exit
                                ;;
        -i | --install )        install_all
                                exit 
                                ;;
        -r | --reloc )          text_reloc
                                exit 
                                ;;
        -h | --help )           usage
                                exit
                                ;;
        * )                     usage
                                exit 1
    esac
    shift
done


