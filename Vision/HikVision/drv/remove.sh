#!/bin/bash
#
# File:		   remove.sh
#
# Description: The script is to remove ds40xxhc driver
#
# Change Log:
#				

DEVICENAME="ds40xxhc"
MODULENAME="ds40xxhc"
INSTALPATH="/etc/${DEVICENAME}"

if [ `whoami` != root ]; then
	echo -e "\n[Info] Login as root to install the device driver!"
	exit 1
fi

echo
echo -e -n "Removing $DEVICENAME device node ...\t\t"
rm -f /dev/$DEVICENAME*
echo "[Ok]"

if [ -d /etc/rc.d/init.d ]; then
	rm -f /etc/rc.d/rc?.d/[SK]??${MODULENAME}
# If we are in a Debian system
elif [ -d /etc/init.d ]; then
	rm -f /etc/rc?.d/[SK]??${MODULENAME}
fi

if [ -d /etc/rc.d/init.d ]; then
	rm -f /etc/rc.d/init.d/${MODULENAME}
elif [ -d /etc/init.d ]; then
	rm -f /etc/init.d/${MODULENAME}
fi

# Remove ourselves
echo -en "Removing all driver required files ...\t\t"
if [ -d $INSTALPATH ]; then
	rm -rf $INSTALPATH
fi
echo "[Ok]"

echo -e -n "Removing driver module from kernel ...\t\t"
if grep -q "$MODULENAME" /proc/modules 2>/dev/null ; then
	/sbin/rmmod $MODULENAME || exit 1
fi

echo "[Ok]"

echo -e "\nDS-40xxHC Card Driver Removed Successfuly.\n"

