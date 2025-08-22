#!/bin/bash
#
JOBS=`grep -c processor /proc/cpuinfo`
let JOBS=${JOBS}*2
JOBS="-j${JOBS}"
BOOT_PATH="arch/arm/boot"
DZIMAGE="dzImage"

CHIPSET=msm8226
RELEASE=${1}
MODEL=${2}
CARRIER=${3}
OPERATOR=${4}
VARIANT=""

if [ "${CARRIER}" = "" ]; then
	DTS_PREFIX="${CHIPSET}-${MODEL}"
else
	DTS_PREFIX="${CHIPSET}-${MODEL}_${CARRIER}"
	VARIANT="${MODEL}_${CARRIER}"
if [ "${OPERATOR}" != "" ]; then
	VARIANT="${VARIANT}_${OPERATOR}"
else
	echo "Failed to get operator"
	exit 1
fi
fi

if [ "${VARIANT}" = "" ]; then
	echo "base_def : ${MODEL}_defconfig , Release : ${RELEASE}"
else
	echo "base_def : ${MODEL}_defconfig , variant_def : ${VARIANT}_defconfig, Release : ${RELEASE}"
fi

if [ "${RELEASE}" = "usr" ]; then
	echo "Now disable CONFIG_SLP_KERNEL_ENG for ${MODEL}_defconfig"

	sed -i 's/CONFIG_SLP_KERNEL_ENG=y/\# CONFIG_SLP_KERNEL_ENG is not set/g' arch/arm/configs/${MODEL}_defconfig
	if [ "$?" != "0" ]; then
		echo "Failed to disable CONFIG_SLP_KERNEL_ENG feature"
		exit 1
	fi
	echo "Now disable CONFIG_DYNAMIC_DEBUG for ${MODEL}_defconfig"

	sed -i 's/CONFIG_DYNAMIC_DEBUG=y/\# CONFIG_DYNAMIC_DEBUG is not set/g' arch/arm/configs/${MODEL}_defconfig
	if [ "$?" != "0" ]; then
		echo "Failed to disable CONFIG_DYNAMIC_DEBUG feature"
		exit 1
	fi
fi

if [ "${VARIANT}" = "" ]; then
	make ARCH=arm ${MODEL}_defconfig
else
	make ARCH=arm ${MODEL}_defconfig VARIANT_DEFCONFIG=${VARIANT}_defconfig
fi

if [ "$?" != "0" ]; then
	echo "Failed to make defconfig"
	exit 1
fi

make $JOBS zImage ARCH=arm
if [ "$?" != "0" ]; then
	echo "Failed to make zImage"
	exit 1
fi

DTC_PATH="scripts/dtc/"
DTC_EXEC=$DTC_PATH"dtc"
DTS_FILES="$BOOT_PATH/dts/$DTS_PREFIX"
DTS_LIST=dts_list

rm $BOOT_PATH/dts/*.dtb -f
ls $DTS_FILES-*.dts >> $DTS_LIST

for DTS_FILE in $(cat $DTS_LIST)
do
	DTB_FILE=$(echo $DTS_FILE | sed 's/.dts/.dtb/2')
	$DTC_EXEC -p 1024 -O dtb -o ${DTB_FILE} ${DTS_FILE}
	if [ "$?" != "0" ]; then
		echo "Failed to make each dtb"
		exit 1
	fi
done

rm $DTS_LIST -f

dtbtool -o $BOOT_PATH/merged-dtb -p $DTC_PATH -v $BOOT_PATH/dts/
if [ "$?" != "0" ]; then
	echo "Failed to make merged-dtb"
	exit 1
fi

mkdzimage -o $BOOT_PATH/$DZIMAGE -k $BOOT_PATH/zImage -d $BOOT_PATH/merged-dtb
if [ "$?" != "0" ]; then
	echo "Failed to make mkdzImage"
	exit 1
fi
