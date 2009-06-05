#!/bin/bash
#
# File:		   setup.sh
#
# Description: Script to install DS-40xxHC card driver
#
# Change Log: 
#				

echo
echo "*** Welcome to DS-40xxHC Capture Card Driver Installation, version 1.0.1 ***"

if [ `whoami` != root ]; then
	echo -e "\n[Info] Login as root to install the driver!"
	exit 1
fi

MODULENAME="ds40xxhc"
INSTALPATH="/etc/${MODULENAME}"

KERNELVERS=`uname -r`

echo -en "\n1. Copy all required files to path ${INSTALPATH}\t\t"
mkdir -p $INSTALPATH
cp * $INSTALPATH &> /dev/null
cd $INSTALPATH
echo -en "\\033[0;31m"
echo "[Ok]"
echo -en "\\033[0;39m"

echo -en "\n2. Check kernel version, running kernel-${KERNELVERS}\t\t"
rm -f .kver &> /dev/null
touch .kver
echo $KERNELVERS > .kver

# Check if any corresponding driver exists for this kernel
if [ ! -f ${MODULENAME}${KERNELVERS}.ko ]; then 
	if [ ! -f ${MODULENAME}${KERNELVERS}.o ]; then
		echo -e "\n[Error] Current driver does not support this kernel version."
		echo -e "        Please contact the board provider.\n"
		# Remove copied files before
		rm -rf $INSTALPATH &> /dev/null
		exit 1
	fi
fi

echo -en "\\033[0;31m"
echo "[Ok]"
echo -en "\\033[0;39m"

echo -en "\n3. Installing system init script ...\t\t\t"
# Install system init
if [ -d /etc/rc.d/init.d ]; then
	INITPATH="/etc/rc.d/init.d"
	# Remove old script and links
	rm -f /etc/rc.d/init.d/$MODULENAME
	rm -f /etc/rc.d/rc?.d/[SK]??$MODULENAME
	if [ ! -f /etc/rc.d/init.d/$MODULENAME ]; then
		cp rc.$MODULENAME /etc/rc.d/init.d/$MODULENAME
		ln -sf ../init.d/$MODULENAME /etc/rc.d/rc0.d/K20$MODULENAME
		ln -sf ../init.d/$MODULENAME /etc/rc.d/rc6.d/K20$MODULENAME
		ln -sf ../init.d/$MODULENAME /etc/rc.d/rc1.d/S85$MODULENAME
		ln -sf ../init.d/$MODULENAME /etc/rc.d/rc2.d/S85$MODULENAME
		ln -sf ../init.d/$MODULENAME /etc/rc.d/rc3.d/S85$MODULENAME
		ln -sf ../init.d/$MODULENAME /etc/rc.d/rc5.d/S85$MODULENAME
	fi
# If we are in a Debian system
elif [ -d /etc/init.d ]; then
	INITPATH="/etc/init.d"
	# Remove old script and links
	rm -f /etc/init.d/$MODULENAME
	rm -f /etc/rc?.d/[SK]??$MODULENAME 
	if [ ! -f /etc/init.d/$MODULENAME ]; then
		cp rc.$MODULENAME /etc/init.d/$MODULENAME
		ln -sf ../init.d/$MODULENAME /etc/rc0.d/K20$MODULENAME
		ln -sf ../init.d/$MODULENAME /etc/rc6.d/K20$MODULENAME
		ln -sf ../init.d/$MODULENAME /etc/rc1.d/S85$MODULENAME
		ln -sf ../init.d/$MODULENAME /etc/rc2.d/S85$MODULENAME
		ln -sf ../init.d/$MODULENAME /etc/rc3.d/S85$MODULENAME
		ln -sf ../init.d/$MODULENAME /etc/rc5.d/S85$MODULENAME
	fi
fi
echo -en "\\033[0;31m"
echo "[Ok]"
echo -en "\\033[0;39m"

#`$INITPATH/$MODULENAME start`
echo -en "\n4. Successfuly installed DS-40xxHC card driver.\t\t\t"
echo -en "\\033[0;31m"
echo "[Ok]"
echo -en "\\033[0;39m"
echo -e "\nPlease reboot or start the device manualy with ${INITPATH}/${MODULENAME} start\n"

