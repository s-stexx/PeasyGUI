#!/bin/bash
SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
BIN_DIR="$SCRIPT_DIR/bin"
LIB_DIR="$BIN_DIR/lib"

export LD_LIBRARY_PATH="$BIN_DIR:$LIB_DIR:$LD_LIBRARY_PATH"
export QT_PLUGIN_PATH="$LIB_DIR"
export QT_QPA_PLATFORM_PLUGIN_PATH="$BIN_DIR/platforms"

exec "$BIN_DIR/PeasyGUI" "$@"
