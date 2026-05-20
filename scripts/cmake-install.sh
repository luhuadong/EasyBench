#!/bin/sh
# System-wide install via CMake (requires root for /usr, /etc).
# Usage:
#   ./scripts/cmake-install.sh
#   PREFIX=/opt/easybench ./scripts/cmake-install.sh
#   BUILD_DIR=build-release ./scripts/cmake-install.sh

set -e

ROOT="$(cd "$(dirname "$0")/.." && pwd)"
BUILD_DIR="${BUILD_DIR:-${ROOT}/build}"
PREFIX="${PREFIX:-/usr}"

if [ ! -f "${BUILD_DIR}/CMakeCache.txt" ]; then
    echo "Configure first: cmake -B ${BUILD_DIR#${ROOT}/}" >&2
    exit 1
fi

if [ ! -x "${BUILD_DIR}/easybench" ]; then
    echo "Build first: cmake --build ${BUILD_DIR#${ROOT}/} -j\$(nproc)" >&2
    exit 1
fi

if [ "${PREFIX}" = "/usr" ] || [ "${PREFIX}" = "/usr/local" ]; then
    if [ "$(id -u)" -ne 0 ]; then
        echo "Installing to ${PREFIX} requires root. Re-run with sudo." >&2
        exit 1
    fi
fi

cmake --install "${BUILD_DIR}" --prefix "${PREFIX}"

if [ "${PREFIX}" = "/usr" ] && [ -f /etc/easybench/easybench.conf ] && [ ! -e /etc/gbox/easybench.conf ]; then
    mkdir -p /etc/gbox
    ln -sf ../easybench/easybench.conf /etc/gbox/easybench.conf 2>/dev/null || true
fi

echo ""
echo "Installed to: ${PREFIX}"
echo "  binary:     ${PREFIX}/bin/easybench"
echo "  config:     /etc/easybench/easybench.conf (created if missing)"
echo "  deploy:     ${PREFIX}/share/easybench/deploy/"
echo "  desktop:    ${PREFIX}/share/applications/easybench.desktop"
