#!/bin/bash
set -euo pipefail

# Get folder where this script lives
DIR="$(cd "$(dirname "$0")" && pwd)"

echo "##################################################"

echo "Removing quarantine flags from everything in:"
echo "  $DIR"

xattr -dr com.apple.quarantine "$DIR"

echo "Done!"
echo "You can now launch apps and scripts inside '$DIR' normally."

echo "##################################################"




