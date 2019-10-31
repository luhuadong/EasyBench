#!/bin/sh

DATE=`date +%Y%m%d`
#DEBUG="echo"
DEBUG=""

APP="gyt_box"
BOOT=${APP}.sh
ICON=${APP}.png
CONFIG=${APP}.conf
DESKTOP=${APP}.desktop

TMP_DIR="gbox"
BIN=${APP}.tar.bz2
RUN=${APP}.run
ZIP=${APP}_${DATE}.zip

SCRIPT="install.sh"

PREFIX="arm-none-eabi-"
#PREFIX=""
CC=${PREFIX}gcc
STRIP=${PREFIX}strip

${DEBUG} ${STRIP} ${APP}
${DEBUG} mkdir ${TMP_DIR}
${DEBUG} cp ${APP} ${TMP_DIR}
${DEBUG} cp ${BOOT} ${TMP_DIR}
${DEBUG} cp ${ICON} ${TMP_DIR}
${DEBUG} cp ${CONFIG} ${TMP_DIR}
${DEBUG} cp ${DESKTOP} ${TMP_DIR}
${DEBUG} cp eepromARMtool ${TMP_DIR}

${DEBUG} tar jcf ${BIN} ${TMP_DIR}
${DEBUG} cat ${SCRIPT} ${BIN} > ${RUN}
${DEBUG} chmod +x ${RUN}
${DEBUG} md5sum ${RUN} > md5sum.txt
${DEBUG} zip ${ZIP} ${RUN} md5sum.txt

${DEBUG} rm -rf ${BIN} ${TMP_DIR}

exit 0
