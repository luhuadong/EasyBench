#!/bin/sh
# Release packaging: staging tarball + optional legacy self-extracting .run
#
# Preferred (CMake):
#   ./packaging/build-staging.sh
#   cd build && cpack -G TGZ
#   cd build && cpack -G DEB   # Debian/Ubuntu, needs dpkg-deb
#
# Legacy self-extractor (embedded installers / air-gapped targets):
#   ./packaging/package.sh

set -e

ROOT="$(cd "$(dirname "$0")/.." && pwd)"
cd "${ROOT}"

# Always refresh staging via CMake install
"${ROOT}/packaging/build-staging.sh"

DATE="$(date +%Y%m%d)"
APP="tuxibit"
TMP_DIR="gbox"
BIN="${APP}.tar.bz2"
RUN="${APP}.run"
ZIP="${APP}_${DATE}.zip"
SCRIPT="packaging/install.sh"
STAGE="${STAGE:-${ROOT}/dist/stage}"

DEBUG="${DEBUG:-}"
# DEBUG=echo ./packaging/package.sh

${DEBUG} mkdir -p "${TMP_DIR}"
${DEBUG} cp -a "${STAGE}/usr/bin/${APP}" "${TMP_DIR}/${APP}" 2>/dev/null \
    || cp -a "${STAGE}/bin/${APP}" "${TMP_DIR}/${APP}"
${DEBUG} cp "${ROOT}/resource/logo.png" "${TMP_DIR}/${APP}.png"
${DEBUG} cp "${ROOT}/config/tuxibit.conf" "${TMP_DIR}/${APP}.conf"
${DEBUG} cp "${BUILD_DIR:-build}/tuxibit.desktop" "${TMP_DIR}/${APP}.desktop" 2>/dev/null \
    || cp "${ROOT}/cmake/tuxibit.desktop.in" "${TMP_DIR}/${APP}.desktop"

if [ -d "${STAGE}/usr/share/tuxibit/deploy" ]; then
    ${DEBUG} mkdir -p "${TMP_DIR}/deploy/bin" "${TMP_DIR}/deploy/data"
    ${DEBUG} cp -a "${STAGE}/usr/share/tuxibit/deploy/bin/"* "${TMP_DIR}/deploy/bin/" 2>/dev/null || true
    ${DEBUG} cp -a "${STAGE}/usr/share/tuxibit/deploy/data/"* "${TMP_DIR}/deploy/data/" 2>/dev/null || true
else
    ${DEBUG} mkdir -p "${TMP_DIR}/deploy/bin" "${TMP_DIR}/deploy/data"
    ${DEBUG} cp "${ROOT}/deploy/bin/eepromARMtool" "${TMP_DIR}/deploy/bin/" 2>/dev/null || true
    ${DEBUG} cp "${ROOT}/deploy/data/I210NIC-origin.otp" "${TMP_DIR}/deploy/data/" 2>/dev/null || true
fi

if [ -f "${ROOT}/scripts/dev/${APP}.sh" ]; then
    ${DEBUG} cp "${ROOT}/scripts/dev/${APP}.sh" "${TMP_DIR}/${APP}.sh"
fi

${DEBUG} tar jcf "${BIN}" "${TMP_DIR}"
${DEBUG} cat "${SCRIPT}" "${BIN}" > "${RUN}"
${DEBUG} chmod +x "${RUN}"
${DEBUG} md5sum "${RUN}" > md5sum.txt
${DEBUG} zip "${ZIP}" "${RUN}" md5sum.txt
${DEBUG} rm -rf "${BIN}" "${TMP_DIR}"

echo "Legacy installer: ${ROOT}/${RUN}"
echo "MD5:              ${ROOT}/md5sum.txt"
echo "Zip bundle:       ${ROOT}/${ZIP}"
