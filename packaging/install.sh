#!/bin/sh

LINES=100 # The number of file lines + 1, be careful after modified !!!

BIN_DIR="/usr/bin"
TMP_DIR="/tmp"
ICON_DIR="/usr/share/pixmaps"
CONFIG_DIR="/etc/easybench"
LEGACY_CONFIG_DIR="/etc/gbox"
DEPLOY_DIR="/usr/share/easybench/deploy"
DESKTOP_DIR="/usr/share/applications"
FONT_DIR="/usr/share/fonts/easybench"

APP_DIR="gbox"

APP="easybench"
BOOT=${APP}.sh
ICON=${APP}.png
CONFIG=${APP}.conf
DESKTOP=${APP}.desktop

CheckPermission() {
	account=`whoami`
	if [ ${account} != "root" ]; then
		echo "${account}, you are NOT the supervisor."
		echo "The root permission is required to run this installer."
		echo "Permission denied."
		exit 1
	fi
}

Unpack() {

	tail -n+${LINES} $0 > ${TMP_DIR}/${APP}.tar.bz2

	echo tar jxf ${TMP_DIR}/${APP}.tar.bz2 -C ${TMP_DIR}
	tar jxf ${TMP_DIR}/${APP}.tar.bz2 -C ${TMP_DIR}

	if [ $? -ne 0 ]; then
		echo "Unpack ${TMP_DIR}/${APP}.tar.bz2 failed"
		echo "Update failed"
		exit 1
	fi
}

Install() {

	mkdir -p ${CONFIG_DIR} ${LEGACY_CONFIG_DIR} ${DEPLOY_DIR}/bin ${DEPLOY_DIR}/data ${FONT_DIR}

	echo cp ${TMP_DIR}/${APP_DIR}/${APP} ${BIN_DIR}
	cp ${TMP_DIR}/${APP_DIR}/${APP} ${BIN_DIR}

	if [ -f ${TMP_DIR}/${APP_DIR}/${APP}.sh ]; then
		echo cp ${TMP_DIR}/${APP_DIR}/${APP}.sh ${BIN_DIR}
		cp ${TMP_DIR}/${APP_DIR}/${APP}.sh ${BIN_DIR}
	fi

	if [ -f ${TMP_DIR}/${APP_DIR}/deploy/bin/eepromARMtool ]; then
		echo cp ${TMP_DIR}/${APP_DIR}/deploy/bin/eepromARMtool ${DEPLOY_DIR}/bin/
		cp ${TMP_DIR}/${APP_DIR}/deploy/bin/eepromARMtool ${DEPLOY_DIR}/bin/
		cp ${TMP_DIR}/${APP_DIR}/deploy/bin/eepromARMtool ${BIN_DIR}
	fi

	if [ -f ${TMP_DIR}/${APP_DIR}/deploy/data/I210NIC-origin.otp ]; then
		cp ${TMP_DIR}/${APP_DIR}/deploy/data/I210NIC-origin.otp ${DEPLOY_DIR}/data/
	fi

	echo cp ${TMP_DIR}/${APP_DIR}/${ICON} ${ICON_DIR}
	cp ${TMP_DIR}/${APP_DIR}/${ICON} ${ICON_DIR}

	echo cp ${TMP_DIR}/${APP_DIR}/${DESKTOP} ${DESKTOP_DIR}
	cp ${TMP_DIR}/${APP_DIR}/${DESKTOP} ${DESKTOP_DIR}

	if [ -f ${CONFIG_DIR}/${CONFIG} ]; then
		cp ${TMP_DIR}/${APP_DIR}/${CONFIG} ${CONFIG_DIR}/${CONFIG}.example
	else
		cp ${TMP_DIR}/${APP_DIR}/${CONFIG} ${CONFIG_DIR}/${CONFIG}
	fi

	if [ -f ${CONFIG_DIR}/${CONFIG} ] && [ ! -f ${LEGACY_CONFIG_DIR}/${CONFIG} ]; then
		ln -sf ../easybench/${CONFIG} ${LEGACY_CONFIG_DIR}/${CONFIG} 2>/dev/null || true
	fi

	if command -v gtk-update-icon-cache >/dev/null 2>&1; then
		gtk-update-icon-cache -f -t /usr/share/icons/hicolor 2>/dev/null || true
	fi
	if command -v update-desktop-database >/dev/null 2>&1; then
		update-desktop-database ${DESKTOP_DIR} 2>/dev/null || true
	fi

	echo "Update success"
}

CheckPermission
Unpack
Install
sync

exit 0
