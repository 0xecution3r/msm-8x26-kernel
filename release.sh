#!/bin/bash
#
JOBS=`grep -c processor /proc/cpuinfo`
let JOBS=${JOBS}*2
JOBS="-j${JOBS}"
RELEASE_DATE=`date +%Y%m%d`
COMMIT_ID=`git log --pretty=format:'%h' -n 1`
BOOT_PATH="arch/arm/boot"
DZIMAGE="dzImage"
CHIPSET=msm8226
MODEL=${1}
CARRIER=${2}
OPERATOR=${3}
TIZEN_MODEL=tizen_${MODEL}
VARIANT=${CARRIER}_${OPERATOR}

if [ "${CARRIER}" = "" ]; then
	DTS_PREFIX="${CHIPSET}-${TIZEN_MODEL}_${CARRIER}"
else
	DTS_PREFIX="${CHIPSET}-${TIZEN_MODEL}_${CARRIER}"
fi

if [ "${MODEL}" = "" ]; then
	echo "Warnning: failed to get machine id."
	echo "ex)./release.sh model_name carrier_name operator_name"
	echo "ex)--------------------------------------------------"
	echo "ex)./release.sh w3g"
	echo "ex)./release.sh b3"
	echo "ex)./release.sh b3 gsm att"
	echo "ex)./release.sh b3 gsm tmo"
	echo "ex)./release.sh b3 gsm vmc"
	echo "ex)./release.sh b3 cdma kdi"
	echo "ex)./release.sh b3 cdma spr"
	echo "ex)./release.sh b3 cdma vzw"
	echo "ex)./release.sh b3 cdma usc"
	echo "ex)./release.sh orbis"
	echo "ex)./release.sh wc1"
	echo "ex)./release.sh wc1 gsm att"
	echo "ex)./release.sh wc1 gsm tmo"
	echo "ex)./release.sh wc1 gsm skt"
	echo "ex)./release.sh wc1 cdma vzw"
	echo "ex)./release.sh wc1 cdma spr"
	echo "ex)./release.sh wc1 cdma ctc"
	echo "ex)./release.sh wc1 lte skt"
	echo "ex)./release.sh wc1 lte lgt"
	echo "ex)./release.sh wc1 lte ktt"
	exit
fi

if [ "${VARIANT}" = "" ]; then
	make ARCH=arm ${TIZEN_MODEL}_defconfig
else
	make ARCH=arm ${TIZEN_MODEL}_defconfig VARIANT_DEFCONFIG=${TIZEN_MODEL}_${VARIANT}_defconfig
fi

if [ "$?" != "0" ]; then
	echo "Failed to make defconfig :"$ARCH
	exit 1
fi

make ${JOBS} zImage ARCH=arm
if [ "$?" != "0" ]; then
	echo "Failed to make zImage"
	exit 1
fi

DTC_PATH="scripts/dtc/"
DTC_EXEC=${DTC_PATH}"dtc"
DTS_FILES="${BOOT_PATH}/dts/${DTS_PREFIX}"
DTS_LIST=dts_list

rm ${BOOT_PATH}/dts/*.dtb -f
rm ${BOOT_PATH}/merged-dtb -f
rm ${BOOT_PATH}/${DZIMAGE}.u -f
ls ${DTS_FILES}-*.dts >> ${DTS_LIST}

for DTS_FILE in $(cat ${DTS_LIST})
do
	DTB_FILE=$(echo ${DTS_FILE} | sed 's/.dts/.dtb/2')
	${DTC_EXEC} -p 1024 -O dtb -o ${DTB_FILE} ${DTS_FILE}
	if [ "$?" != "0" ]; then
		echo "Failed to make each dtb"
		exit 1
	fi
done

rm ${DTS_LIST} -f

dtbtool -o ${BOOT_PATH}/merged-dtb -p ${DTC_PATH} -v ${BOOT_PATH}/dts/
if [ "$?" != "0" ]; then
	echo "Failed to make merged-dtb"
	exit 1
fi

mkdzimage -o ${BOOT_PATH}/${DZIMAGE} -k ${BOOT_PATH}/zImage -d ${BOOT_PATH}/merged-dtb
if [ "$?" != "0" ]; then
	echo "Failed to make mkdzImage"
	exit 1
fi

if [ "${OPERATOR}" = "att" ]; then
	mv ${BOOT_PATH}/${DZIMAGE} ${BOOT_PATH}/${DZIMAGE}.u
	java -jar ./scripts/signclient.jar -runtype ss_openssl_all -model SM-R730A_NA_ATT -input ${BOOT_PATH}/${DZIMAGE}.u -output ${BOOT_PATH}/${DZIMAGE}
elif [ "${OPERATOR}" = "vzw" ]; then
	mv ${BOOT_PATH}/${DZIMAGE} ${BOOT_PATH}/${DZIMAGE}.u
	java -jar ./scripts/signclient.jar -runtype ss_openssl_all -model SM-R730V_NA_VZW -input ${BOOT_PATH}/${DZIMAGE}.u -output ${BOOT_PATH}/${DZIMAGE}
fi

sudo ls > /dev/null
./scripts/mkmodimg.sh

if [ "${VARIANT}" != "" ]; then
	RELEASE_IMAGE=System_${MODEL}_${VARIANT}_${RELEASE_DATE}-${COMMIT_ID}.tar
else
	RELEASE_IMAGE=System_${MODEL}_${RELEASE_DATE}-${COMMIT_ID}.tar
fi

tar cf ${RELEASE_IMAGE} -C ${BOOT_PATH} ${DZIMAGE}
if [ "$?" != "0" ]; then
	echo "Failed to tar ${DZIMAGE}"
	exit 1
fi

tar rf ${RELEASE_IMAGE} -C usr/tmp-mod modules.img
if [ "$?" != "0" ]; then
	echo "Failed to tar modules.img"
	exit 1
fi

echo ${RELEASE_IMAGE}
