#!/bin/sh

echo " "
echo " "
echo "---------------------------------------------------"
cd `dirname $0`

../../PeasyGUI.app/Contents/MacOS/PeasyGUI --ini="examples/4_py_imageviewer/PeasyGUI_viewer.ini" &

echo "---------------------------------------------------"
echo " "
echo " "

exit