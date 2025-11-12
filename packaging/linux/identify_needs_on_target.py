#!/usr/bin/env python3
import subprocess
import re
import shutil
import os
import sys

# On the development Linux machine run these commands in Terminal:
"""
QT_SRC="/home/stefan/Qt/6.8.3/gcc_64"
DEST="/home/stefan/_Projekte/PeasyGUI/dist/linux/PeasyGUI/bin"

rsync -av --include='*.so*' --exclude='*' "$QT_SRC/lib/" "$DEST/lib/"
rsync -av "$QT_SRC/plugins/platforms/" "$DEST/platforms/"
rsync -av "$QT_SRC/plugins/imageformats/" "$DEST/imageformats/"
tar cvf huge.tar ../../dist/linux
sfk httpserv
"""

# On the Live Linux Distro
# Download and extract the huge.rar
# Move all libs and linked libs from "bin/libs/" to e.g. a folder named "wech"
# Run this python script multiple times
START_SCRIPT = "/home/mint/Desktop/PeasyGUI_ausgeduennt/start_PeasyGUI.sh"
SOURCE_DIR = "/home/mint/Desktop/wech"
TARGET_DIR = "/home/mint/Desktop/PeasyGUI_ausgeduennt/bin/lib"

REPLACES = {
    '.so.6': '.so.6.8.3',
    '.so.73': '.so.73.2',
}

def run_peasy():
    """Runs the PeasyGUI startup script and returns stdout and stderr."""
    result = subprocess.run(
        ["bash", "-c", f"QT_DEBUG_PLUGINS=1 {START_SCRIPT}"],
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        text=True
    )
    return result

def parse_missing_lib(error_output):
    """Extracts the missing library name from the error messages."""
    # Match either format
    match = re.search(r"(\S+\.so\.\d+): cannot open shared object file", error_output)
    return match.group(1) if match else None

def find_source_lib(missing_lib):
    """Finds the matching library file in the source folder based on REPLACES rules."""
    for short, full in REPLACES.items():
        if missing_lib.endswith(short):
            source_name = missing_lib.replace(short, full)
            candidate = os.path.join(SOURCE_DIR, source_name)
            if os.path.exists(candidate):
                return candidate
    return None

def copy_library(source_lib, missing_lib):
    """Copies the found library into the target directory under the missing name."""
    target_path = os.path.join(TARGET_DIR, missing_lib)
    os.makedirs(TARGET_DIR, exist_ok=True)
    shutil.copy2(source_lib, target_path)
    print(f"✅ Copied: {source_lib} → {target_path}")

def main():
    print("🔹 Starting PeasyGUI...")
    result = run_peasy()

    if result.returncode == 0:
        print("✅ PeasyGUI started successfully.")
        return

    missing_lib = parse_missing_lib(result.stderr)
    if not missing_lib:
        print("❌ No missing library detected in the error output.")
        print(result.stderr)
        sys.exit(1)

    print(f"⚠️ Missing library detected: {missing_lib}")

    source_lib = find_source_lib(missing_lib)
    if not source_lib:
        print(f"❌ No matching source found for {missing_lib}.")
        sys.exit(1)

    copy_library(source_lib, missing_lib)

    print("🔁 Restarting PeasyGUI...")
    result2 = run_peasy()
    print(result2.stdout)
    print(result2.stderr)

if __name__ == "__main__":
    main()
