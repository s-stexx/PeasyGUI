#!/bin/bash

set -euo pipefail

TOP_LEVEL_DIR="$(cd "$(dirname "$0")/../.." && pwd)"

cd "$TOP_LEVEL_DIR"


# -------------------------
# Configuration
# -------------------------
QT_PATH="/Applications/Qt/6.8.3/macos"
BUILD_DIR="build/macOS-MinSizeRel"
DIST_DIR="dist/macos"
CMAKE_PREFIX_PATH="$QT_PATH/lib/cmake"


# ----------------------------------------------------------------------------------------
echo ">>> Configuring and building PeasyGUI with CMake ..."

cmake -S . -B "$BUILD_DIR" \
  -DCMAKE_BUILD_TYPE=MinSizeRel \
  -DCMAKE_PREFIX_PATH="$CMAKE_PREFIX_PATH"

cmake --build "$BUILD_DIR" --config MinSizeRel

GUI_PATH="$BUILD_DIR/PeasyGUI.app"

if [ ! -d "$GUI_PATH" ]; then
  echo "Error: $GUI_PATH not found!"
  exit 1
fi


# ----------------------------------------------------------------------------------------
echo ">>> Configuring and building example(s) with CMake ..."

cd packaging/examples/3_c_greet

mkdir -p build
cd build
cmake ..
cmake --build .

cd "$TOP_LEVEL_DIR"

rm -rf "packaging/examples/3_c_greet/build"


# ----------------------------------------------------------------------------------------
echo ">>> Running macdeployqt ..."

"$QT_PATH/bin/macdeployqt" "$GUI_PATH"


# ----------------------------------------------------------------------------------------
echo ">>> Code sign command requires superuser privileges ..."

sudo codesign --force --deep -s - "$GUI_PATH"


# ----------------------------------------------------------------------------------------
echo ">>> Copying licenses texts ..."

LICENSE_DIR="$GUI_PATH/Contents/Resources/licenses"
mkdir -p "$LICENSE_DIR"

cp README.md "$LICENSE_DIR/README.md"
cp LICENSE "$LICENSE_DIR/LICENSE"
cp third_party_licenses/LICENSE.GPL3 "$LICENSE_DIR/LICENSE.GPL3"
cp third_party_licenses/LICENSE.LGPL3 "$LICENSE_DIR/LICENSE.LGPL3"


# ----------------------------------------------------------------------------------------
echo ">>> Preparing DMG structure..."

STAGE_DIR="$DIST_DIR/tmp"

rm -rf "$STAGE_DIR"
mkdir -p "$STAGE_DIR/PeasyGUI"

cp -R "$GUI_PATH" "$STAGE_DIR/PeasyGUI/"
cp packaging/macos/Unlock.command "$STAGE_DIR/PeasyGUI/"
cp packaging/macos/Installation_steps.txt "$STAGE_DIR/"

EXAMPLES_SRC="$TOP_LEVEL_DIR/packaging/examples"
EXAMPLES_DST="$STAGE_DIR/PeasyGUI/examples"

rsync -av --exclude '*.sh' "$EXAMPLES_SRC/" "$EXAMPLES_DST/"


# ----------------------------------------------------------------------------------------
echo ">>> Creating DMG..."

DMG_NAME="PeasyGUI-0000000.dmg"

hdiutil create -volname "PeasyGUI" \
  -srcfolder "$STAGE_DIR" \
  -ov -format UDZO "$DIST_DIR/$DMG_NAME"

rm -rf "$STAGE_DIR"

echo ">>> Done!"
echo "Created: $DIST_DIR/$DMG_NAME"

open "$DIST_DIR"