#!/bin/sh
# Build release binary and populate dist/stage via `cmake --install` (DESTDIR).
# Usage: ./packaging/build-staging.sh

set -e

ROOT="$(cd "$(dirname "$0")/.." && pwd)"
BUILD_DIR="${BUILD_DIR:-${ROOT}/build}"
DIST_DIR="${DIST_DIR:-${ROOT}/dist}"
STAGE="${STAGE:-${DIST_DIR}/stage}"
PREFIX="${PREFIX:-/usr}"

cd "${ROOT}"

if [ ! -f "${BUILD_DIR}/CMakeCache.txt" ]; then
    cmake -B "${BUILD_DIR}" -DCMAKE_BUILD_TYPE=Release
fi

cmake --build "${BUILD_DIR}" -j"$(nproc 2>/dev/null || echo 2)"

rm -rf "${STAGE}"
DESTDIR="${STAGE}" cmake --install "${BUILD_DIR}" --prefix "${PREFIX}"

VERSION="$(cmake -B "${BUILD_DIR}" -N 2>/dev/null | awk -F= '/CMAKE_PROJECT_VERSION:STATIC/{print $2; exit}')"
VERSION="${VERSION:-0.2.0}"

ARCH="$(uname -m)"
TARBALL="${DIST_DIR}/easybench-${VERSION}-linux-${ARCH}.tar.gz"
mkdir -p "${DIST_DIR}"
tar -C "${STAGE}" -czf "${TARBALL}" .

echo "Staging tree: ${STAGE}"
echo "Archive:      ${TARBALL}"
echo ""
echo "Install on target (as root):"
echo "  sudo tar -C / -xzf ${TARBALL}"
echo "Or use CPack from build dir: cd ${BUILD_DIR} && cpack -G TGZ"
