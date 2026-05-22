#!/bin/sh
# Install launcher + icon into the current user's XDG directories (no root).
# Useful for Ubuntu desktop / dock testing after a local build.

set -e

ROOT="$(cd "$(dirname "$0")/.." && pwd)"
TB_BIN="${TB_BIN:-$ROOT/build/tuxibit}"
ICON_DIR="$HOME/.local/share/icons/hicolor"
APPS_DIR="$HOME/.local/share/applications"

if [ ! -x "$TB_BIN" ]; then
    echo "Executable not found: $TB_BIN" >&2
    echo "Build first: cmake -B build && cmake --build build" >&2
    exit 1
fi

for size in 48 128 256 512; do
    mkdir -p "$ICON_DIR/${size}x${size}/apps"
    cp "$ROOT/resource/logo.png" "$ICON_DIR/${size}x${size}/apps/tuxibit.png"
done

mkdir -p "$APPS_DIR"
sed "s|^Exec=tuxibit|Exec=$TB_BIN|" "$ROOT/cmake/tuxibit.desktop.in" > "$APPS_DIR/tuxibit.desktop"

if command -v gtk-update-icon-cache >/dev/null 2>&1; then
    gtk-update-icon-cache -f -t "$ICON_DIR" 2>/dev/null || true
fi
if command -v update-desktop-database >/dev/null 2>&1; then
    update-desktop-database "$APPS_DIR" 2>/dev/null || true
fi

echo "Installed:"
echo "  $APPS_DIR/tuxibit.desktop"
echo "  $ICON_DIR/*/apps/tuxibit.png"
echo "Log out and back in, or restart the shell, if the dock icon does not refresh."
